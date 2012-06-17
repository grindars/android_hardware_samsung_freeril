/*
 * Copyright (C) 2010 Google, Inc.
 * Copyright (C) 2010 Samsung Electronics.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __MODEM_PRJ_H__
#define __MODEM_PRJ_H__

#include <sys/ioctl.h>

#define MAX_LINK_DEVTYPE        3
#define MAX_RAW_DEVS            32
#define MAX_NUM_IO_DEV          (MAX_RAW_DEVS + 4)

#define IOCTL_MODEM_ON                  _IO('o', 0x19)
#define IOCTL_MODEM_OFF                 _IO('o', 0x20)
#define IOCTL_MODEM_RESET               _IO('o', 0x21)
#define IOCTL_MODEM_BOOT_ON             _IO('o', 0x22)
#define IOCTL_MODEM_BOOT_OFF            _IO('o', 0x23)
#define IOCTL_MODEM_START               _IO('o', 0x24)

#define IOCTL_MODEM_PROTOCOL_SUSPEND    _IO('o', 0x25)
#define IOCTL_MODEM_PROTOCOL_RESUME     _IO('o', 0x26)

#define IOCTL_MODEM_STATUS              _IO('o', 0x27)
#define IOCTL_MODEM_GOTA_START          _IO('o', 0x28)
#define IOCTL_MODEM_FW_UPDATE           _IO('o', 0x29)

#define IOCTL_MODEM_NET_SUSPEND         _IO('o', 0x30)
#define IOCTL_MODEM_NET_RESUME          _IO('o', 0x31)

#define IOCTL_MODEM_DUMP_START          _IO('o', 0x32)
#define IOCTL_MODEM_DUMP_UPDATE         _IO('o', 0x33)
#define IOCTL_MODEM_FORCE_CRASH_EXIT    _IO('o', 0x34)
#define IOCTL_MODEM_CP_UPLOAD           _IO('o', 0x35)
#define IOCTL_MODEM_DUMP_RESET          _IO('o', 0x36)

#define IOCTL_DPRAM_SEND_BOOT           _IO('o', 0x40)
#define IOCTL_DPRAM_INIT_STATUS         _IO('o', 0x43)

/* ioctl command definitions. */
#define IOCTL_DPRAM_PHONE_POWON         _IO('o', 0xd0)
#define IOCTL_DPRAM_PHONEIMG_LOAD       _IO('o', 0xd1)
#define IOCTL_DPRAM_NVDATA_LOAD         _IO('o', 0xd2)
#define IOCTL_DPRAM_PHONE_BOOTSTART     _IO('o', 0xd3)

#define IOCTL_DPRAM_PHONE_UPLOAD_STEP1  _IO('o', 0xde)
#define IOCTL_DPRAM_PHONE_UPLOAD_STEP2  _IO('o', 0xdf)

/* modem status */
#define MODEM_OFF               0
#define MODEM_CRASHED           1
#define MODEM_RAMDUMP           2
#define MODEM_POWER_ON          3
#define MODEM_BOOTING_NORMAL    4
#define MODEM_BOOTING_RAMDUMP   5
#define MODEM_DUMPING           6
#define MODEM_RUNNING           7

#define IOCTL_LINK_CONTROL_ENABLE       _IO('o', 0x30)
#define IOCTL_LINK_CONTROL_ACTIVE       _IO('o', 0x31)
#define IOCTL_LINK_GET_HOSTWAKE         _IO('o', 0x32)
#define IOCTL_LINK_CONNECTED            _IO('o', 0x33)
#define IOCTL_LINK_SET_BIAS_CLEAR       _IO('o', 0x34)

#define IOCTL_LINK_PORT_ON              _IO('o', 0x35)
#define IOCTL_LINK_PORT_OFF             _IO('o', 0x36)

#endif
