/*
** pic32-options.h
**
** This file is part of BFD, the Binary File Descriptor library.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
** USA.
**
** This file contains data declarations and
** function prototypes related to pic32-specifc
** options processing for the linker.
**
** It is included by the following target-specific
** emulation files:
**
**   ld/emultmpl/pic32_elf32.em
**
*/

#ifndef _PIC32_OPTIONS_H
#define _PIC32_OPTIONS_H

/*
** Externals declared in bfd *.c
*/

extern bfd_boolean pic32_report_mem;
extern bfd_boolean pic32_debug;
extern bfd_boolean pic32_smart_io;
extern bfd_boolean pic32_has_smart_io_option;

static void gldelf32pic32mx_list_options
   PARAMS ((FILE *));
static int gldelf32pic32mx_parse_args
   PARAMS ((int, char**));

/* Emulation-specific Options */
const char * shortopts = "-Dp:"; /* note: leading "-" is important */

/* Codes used for the long options with no short synonyms.  1000 isn't
   special; it's just an arbitrary non-ASCII char value that is
   different from the range used in lexsup.c  */
enum elfpic32mx_options {
  REPORT_MEM_OPTION = 1000,
  SMART_IO_OPTION,
  NO_SMART_IO_OPTION
};

static struct option longopts[] =
{
  { "debug", no_argument, NULL, 'D' },
  { "smart-io", no_argument, NULL, SMART_IO_OPTION },
  { "no-smart-io", no_argument, NULL, NO_SMART_IO_OPTION },
  { "report-mem", no_argument, NULL, REPORT_MEM_OPTION },
  { NULL,        no_argument,       NULL, 0                }
};
#endif

