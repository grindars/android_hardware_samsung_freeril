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

#ifndef __ARM_DL__H__
#define __ARM_DL__H__

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct arm_dlhandle {
    void *base;
    size_t size;
    uint32_t *dyn_data;
} arm_dlhandle_t;

arm_dlhandle_t *arm_dlopen(const char *filename);
void arm_dlclose(arm_dlhandle_t *handle);
void *arm_dlsym(arm_dlhandle_t *handle, const char *symbol);

#if defined(__cplusplus)
}
#endif

#endif
