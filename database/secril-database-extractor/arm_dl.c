/*
 * Free RIL implementation for Samsung Android-based smartphones.
 * Copyright (C) 2012  Sergey Gridasov <grindars@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#if defined(__x86_64__)
#error 64-bit host is not supported - how should I relocate 32-bit executable on 64-bit system?!
#endif

#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <elf.h>
#include <stdio.h>

#include "arm_dl.h"

static const unsigned char valid_ident[EI_PAD] = {
    [EI_MAG0]    = ELFMAG0,
    [EI_MAG1]    = ELFMAG1,
    [EI_MAG2]    = ELFMAG2,
    [EI_MAG3]    = ELFMAG3,
    [EI_CLASS]   = ELFCLASS32,
    [EI_DATA]    = ELFDATA2LSB,
    [EI_VERSION] = EV_CURRENT
};

static void *load_elf_image(int hndl, void **dynamic, size_t *size) {
    long page = sysconf(_SC_PAGESIZE);
    unsigned int i;

    *dynamic = NULL;

    Elf32_Ehdr ehdr;

    if(read(hndl, &ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)) {
        errno = EINVAL;

        return NULL;
    }

    if(memcmp(ehdr.e_ident, valid_ident, EI_PAD) != 0 ||
       ehdr.e_type != ET_DYN ||
       ehdr.e_machine != EM_ARM ||
       ehdr.e_version != EV_CURRENT ||
       ehdr.e_phoff == 0 ||
       ehdr.e_phentsize != sizeof(Elf32_Phdr)) {

        errno = EINVAL;

        return NULL;
    }

    if(lseek(hndl, ehdr.e_phoff, SEEK_SET) != ehdr.e_phoff) {
        errno = EINVAL;

        return NULL;
    }

    uint32_t image_size = 0;

    for(i = 0; i < ehdr.e_phnum; i++) {
        Elf32_Phdr phdr;

        if(read(hndl, &phdr, sizeof(Elf32_Phdr)) != sizeof(Elf32_Phdr)) {
            errno = EINVAL;

            return NULL;
        }

        if(phdr.p_type == PT_LOAD) {
            uint32_t segment_end = phdr.p_paddr + phdr.p_memsz;

            if(segment_end > image_size)
                image_size = segment_end;
        }
    }

    if(image_size == 0) {
        errno = EINVAL;

        return NULL;
    }

    image_size = (image_size + page) & ~page;
    *size = image_size;

    void *image = mmap(NULL, image_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    if(image == MAP_FAILED)
        return NULL;

    lseek(hndl, ehdr.e_phoff, SEEK_SET);

    for(i = 0; i < ehdr.e_phnum; i++) {
        Elf32_Phdr phdr;

        read(hndl, &phdr, sizeof(Elf32_Phdr));

        if(phdr.p_type == PT_LOAD) {
            if(phdr.p_filesz)
                pread(hndl, (char *)image + phdr.p_paddr, phdr.p_filesz, phdr.p_offset);

            memset((char *)image + phdr.p_paddr + phdr.p_filesz, 0, phdr.p_memsz - phdr.p_filesz);

        } else if(phdr.p_type == PT_DYNAMIC) {
            *dynamic = (char *)image + phdr.p_paddr;
        }
    }

    return image;
}

static uint32_t *build_dyn_data(Elf32_Dyn *dyn, void *image) {
    unsigned int i;
    uint32_t *dyn_data = malloc(sizeof(uint32_t) * (DT_BIND_NOW + 1));

    if(dyn_data == NULL)
        return NULL;

    memset(dyn_data, 0, sizeof(uint32_t) * (DT_BIND_NOW + 1));

    for(i = 0; dyn[i].d_tag != DT_NULL; i++) {
        uint32_t val = 0;

        switch(dyn[i].d_tag) {
            case DT_PLTRELSZ:
            case DT_RELASZ:
            case DT_RELAENT:
            case DT_STRSZ:
            case DT_SYMENT:
            case DT_SONAME:
            case DT_RPATH:
            case DT_NEEDED:
            case DT_RELSZ:
            case DT_RELENT:
            case DT_PLTREL:
                val = dyn[i].d_un.d_val;

                break;

            case DT_HASH:
            case DT_STRTAB:
            case DT_SYMTAB:
            case DT_RELA:
            case DT_PLTGOT:
            case DT_INIT:
            case DT_FINI:
            case DT_REL:
            case DT_DEBUG:
            case DT_JMPREL:
                val = (uint32_t) image + dyn[i].d_un.d_ptr;

                break;

            case DT_BIND_NOW:
            case DT_TEXTREL:
            case DT_SYMBOLIC:
                val = 1;

                break;

            default:
                continue;
        }

        dyn_data[dyn[i].d_tag] = val;
    }

    return dyn_data;
}

static int do_relocation(uint32_t *tags, void *image, unsigned int offset, unsigned int info, int addend, int addend_present) {

    int sym = ELF32_R_SYM(info);
    uint32_t *target = (uint32_t *)((unsigned char *) image + offset);

    Elf32_Sym *symtable = (Elf32_Sym *) tags[DT_SYMTAB];
    char *strtable = (char *) tags[DT_STRTAB];

    switch(ELF32_R_TYPE(info)) {
        case R_ARM_GLOB_DAT:
        case R_ARM_JUMP_SLOT:
            if(sym == STN_UNDEF) {
                printf("R_ARM_JUMP_SLOT to undefined symbol\n");

                return -1;
            }

            if(symtable[sym].st_shndx != SHN_UNDEF) {
                printf("R_ARM_JUMP_SLOT to defined symbol\n");

                return -1;
            }

            *target = 0 + addend;

            break;

        case R_ARM_RELATIVE:
            if(sym == STN_UNDEF) {
                if(addend_present)
                    *target = (uint32_t) image + addend;
                else
                    *target += (uint32_t) image;
            } else {
                printf("R_ARM_RELATIVE with symbol %u\n", sym);

                return -1;
            }

            break;

        case R_ARM_ABS32:
            if(!addend_present)
                addend = *target;

            if(sym == STN_UNDEF) {
                printf("R_ARM_ABS32 to undefined symbol\n");

                return -1;
            }

            if(symtable[sym].st_shndx == SHN_UNDEF) {
                *target = (uint32_t) 0 + addend;
            } else {
                *target = (uint32_t) image + symtable[sym].st_value + addend;
            }

            break;
        default:
            printf("unsupported relocation: symbol %u, type %hhu, offset %p, addend %d\n", sym, ELF32_R_TYPE(info), (void *) offset, addend);

            return -1;
    }

    return 0;
}

static int do_rel(uint32_t *tags, void *image, Elf32_Rel *rel, unsigned int size, unsigned int ent) {
    unsigned int off;

    if(ent != sizeof(Elf32_Rel))
        return -1;

    for(off = 0; off < size; off += sizeof(Elf32_Rel), rel++) {
        if(do_relocation(tags, image, rel->r_offset, rel->r_info, 0, 0) == -1)
            return -1;
    }

    return 0;
}

static int do_rela(uint32_t *tags, void *image, Elf32_Rela *rela, unsigned int size, unsigned int ent) {
    unsigned int off;

    if(ent != sizeof(Elf32_Rela))
        return -1;

    for(off = 0; off < size; off += sizeof(Elf32_Rela), rela++) {
        if(do_relocation(tags, image, rela->r_offset, rela->r_info, rela->r_addend, 1) == -1)
            return -1;
    }

    return 0;
}

static int relocate(uint32_t *tags, void *image) {
    if(tags[DT_REL])
        if(do_rel(tags, image, (Elf32_Rel *) tags[DT_REL], tags[DT_RELSZ], tags[DT_RELENT]) == -1)
            return -1;

    if(tags[DT_RELA])
        if(do_rela(tags, image, (Elf32_Rela *) tags[DT_RELA], tags[DT_RELASZ], tags[DT_RELAENT]) == -1)
            return -1;

    if(tags[DT_JMPREL]) {
        switch(tags[DT_PLTREL]) {
            case DT_REL:
                if(do_rel(tags, image, (Elf32_Rel *) tags[DT_JMPREL], tags[DT_PLTRELSZ], sizeof(Elf32_Rel)) == -1)
                    return -1;

                break;

            case DT_RELA:
                if(do_rela(tags, image, (Elf32_Rela *) tags[DT_JMPREL], tags[DT_PLTRELSZ], sizeof(Elf32_Rela)) == -1)
                    return -1;

                break;

            default:
                return -1;
        }
    }

    return 0;
}

arm_dlhandle_t *arm_dlopen(const char *filename) {
    int fd = open(filename, O_RDONLY);

    if(fd == -1)
        return NULL;

    void *base, *dynamic;
    size_t size;

    base = load_elf_image(fd, &dynamic, &size);
    close(fd);

    if(base == NULL)
        return NULL;

    uint32_t *dyn_data = build_dyn_data(dynamic, base);

    if(dyn_data == NULL) {
        munmap(base, size);

        return NULL;
    }

    if(relocate(dyn_data, base) == -1) {
        free(dyn_data);
        munmap(base, size);

        return NULL;
    }

    arm_dlhandle_t *handle = malloc(sizeof(arm_dlhandle_t));
    if(handle == NULL) {
        free(dyn_data);
        munmap(base, size);

        return NULL;
    }

    handle->base = base;
    handle->size = size;
    handle->dyn_data = dyn_data;

    return handle;
}

void arm_dlclose(arm_dlhandle_t *handle) {
    free(handle->dyn_data);
    munmap(handle->base, handle->size);
    free(handle);
}

static unsigned long elf_hash(const unsigned char *name) {
    unsigned long h = 0, g;

    while (*name) {
        h = (h << 4) + *name++;

        g = h & 0xf0000000;

        if(g)
            h ^= g >> 24;

        h &= ~g;
    }

    return h;
}

void *arm_dlsym(arm_dlhandle_t *handle, const char *symbol) {
    uint32_t *hash = (uint32_t *) handle->dyn_data[DT_HASH];
    Elf32_Sym *symtable = (Elf32_Sym *) handle->dyn_data[DT_SYMTAB];
    char *strtable = (char *) handle->dyn_data[DT_STRTAB];

    uint32_t nbucket = hash[0], nchain = hash[1];
    uint32_t *bucket = &hash[2], *chain = &hash[nbucket + 2];

    unsigned long hash_value = elf_hash((const unsigned char *) symbol);
    unsigned int y;

    for(y = bucket[hash_value % nbucket]; y != STN_UNDEF && y < nchain; y = chain[y])
        if(strcmp(strtable + symtable[y].st_name, symbol) == 0)
            return (char *) handle->base + symtable[y].st_value;

    return NULL;
}
