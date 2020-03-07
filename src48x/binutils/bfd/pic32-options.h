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
extern bfd_boolean pic32_memory_summary;
extern char *memory_summary_arg;
extern bfd_boolean pic32_debug;
extern bfd_boolean pic32_smart_io;
extern bfd_boolean pic32_has_smart_io_option;
extern bfd_boolean pic32_allocate;
extern bfd_boolean pic32_data_init;
extern bfd_boolean pic32_has_data_init_option;
extern bfd_boolean pic32_has_fill_option;
extern struct pic32_fill_option *pic32_fill_option_list;
extern bfd_boolean pic32_has_stack_option;
extern unsigned int pic32_stack_size;
extern bfd_boolean pic33_has_stackguard_option;
extern unsigned int pic32_stackguard_size;
extern bfd_boolean pic32_has_heap_option;
extern bfd_boolean pic32_heap_required;
extern unsigned int pic32_heap_size;
extern bfd_boolean pic32_has_crypto_option;
extern const char * crypto_file;
extern bfd_boolean pic32_has_processor_option;
extern bfd_boolean pic32_has_hardfloat_option;
extern bfd_boolean pic32_has_softfloat_option;
extern bfd_boolean pic32_code_in_dinit;
extern bfd_boolean pic32_has_code_in_dinit_option;
extern bfd_boolean pic32_has_dinit_in_serial_mem_option;
extern bfd_boolean pic32_dinit_in_serial_mem;
extern bfd_boolean pic32_dinit_has_absolute_address;
extern bfd_vma dinit_address;

/* lghica - co-resident */
extern bfd_boolean      pic32_memory_usage;
extern bfd_boolean      pic32_reserve_const;
extern unsigned long    reserve_const_arg;
extern bfd_boolean      pic32_pad_flash_option;
extern bfd_vma          pad_flash_arg;
extern bfd_boolean      pic32_application_id;
extern char             *application_id;
extern bfd_boolean      pic32_coresident_app;
extern bfd_boolean      pic32_inherit_application_info;
extern char             *inherited_application;

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
  REPORT_MAFRLCSJ_OPTION,
  REPORT_MAFRLCSJ2_OPTION,
  SMART_IO_OPTION,
  NO_SMART_IO_OPTION,
  DATA_INIT_OPTION,
  NO_DATA_INIT_OPTION,
  FILL_OPTION,
  CRYPTO_OPTION,
  MEMRESERVE_OPTION,
  HARDFLOAT_OPTION,
  MEMORY_SUMMARY,
  SOFTFLOAT_OPTION,
  CODE_IN_DINIT_OPTION,
  NO_CODE_IN_DINIT_OPTION,
  DINIT_IN_SERIAL_MEM_OPTION,
  NO_DINIT_IN_SERIAL_MEM_OPTION,
  DINIT_ADDRESS_OPTION,
  /* lghica co-resident */
    MEMORY_USAGE,
    RESERVE_CONST,
    PAD_FLASH,
    APPLICATION_ID,
    CORESIDENT,
    INHERIT_APPLICATION_INFO
};

static struct option longopts[] =
{
  { "debug", no_argument, NULL, 'D' },
  { "smart-io", no_argument, NULL, SMART_IO_OPTION },
  { "no-smart-io", no_argument, NULL, NO_SMART_IO_OPTION },
  { "report-mem", no_argument, NULL, REPORT_MEM_OPTION },

  { "memorysummary", required_argument, NULL, MEMORY_SUMMARY },
  { "data-init", no_argument, NULL, DATA_INIT_OPTION },
  { "no-data-init", no_argument, NULL, NO_DATA_INIT_OPTION },
  { "fill", required_argument, NULL, FILL_OPTION },
  { "crypto", required_argument, NULL, CRYPTO_OPTION },
  { "mreserve", required_argument, NULL, MEMRESERVE_OPTION },
  { "hard-float", no_argument, NULL, HARDFLOAT_OPTION },
  { "soft-float", no_argument, NULL, SOFTFLOAT_OPTION },  
  { "code-in-dinit", no_argument, NULL, CODE_IN_DINIT_OPTION },
  { "no-code-in-dinit", no_argument, NULL, NO_CODE_IN_DINIT_OPTION },  
  { "dinit-in-serial-mem", no_argument, NULL, DINIT_IN_SERIAL_MEM_OPTION },
  { "no-dinit-in-serial-mem", no_argument, NULL, NO_DINIT_IN_SERIAL_MEM_OPTION },
  { "dinit-address", required_argument, NULL, DINIT_ADDRESS_OPTION },
    /* lghica co-resident */
    { "memory-usage",           no_argument,        NULL, MEMORY_USAGE },
    { "reserve-const",          optional_argument,  NULL, RESERVE_CONST },
    { "pad-flash",              optional_argument,  NULL, PAD_FLASH },
    { "application-id",         required_argument,  NULL, APPLICATION_ID },
#if 0 /* Disable co-resident for now */
    { "coresident",             no_argument,        NULL, CORESIDENT },
#endif
    { "inherit-application-info", required_argument, NULL, INHERIT_APPLICATION_INFO },
  { NULL,        no_argument,       NULL, 0                }
};
#endif

