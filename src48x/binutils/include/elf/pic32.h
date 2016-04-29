/*
 * pic32.h -- ELF format helper for the MIPS-based PIC32 architecture.
 * Copyright (C) 2000 Free Software Foundation, Inc.
 * Contributed by Microchip Corporation.
 * Written by Jason Kajita.
 *
 * This file is part of BFD, the Binary File Descriptor library.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#ifndef _ELF_PIC32_H
#define _ELF_PIC32_H

/* Section Lists */
struct pic32_section
{
  struct pic32_section *next;
  unsigned int attributes;
  PTR *file;
  asection *sec;
  unsigned int use_vma;
};

/************************************************************************/
/*   pic32-specific section flags                                       */
/************************************************************************/
#define SHF_MEMORY   (1 << 19)  /* User-defined memory */
/* OS and processor-specific flags start at postion 20 */
#define SHF_DMA      (1 << 20)  /* DMA memory */
#define SHF_NOLOAD   (1 << 21)  /* Do not allocate or load */
#define SHF_NEAR     (1 << 22)  /* Near memory */
#define SHF_PERSIST  (1 << 23)  /* Persistent */
#define SHF_ABSOLUTE (1 << 24)  /* Absolute address */
#define SHF_REVERSE  (1 << 25)  /* Reverse aligned */
#define SHF_RAMFUNC  (1 << 26)  /* RAMFUNC */
#define SHF_COHERENT (1 << 27)  /* COHERENT */
#define SHF_KEEP     (1 << 28)  /* KEEP */
/************************************************************************/

/*
 * pic32-specific "extended" section flags
 *
 * To work around space limitations in the section flags field,
 * these values are encoded in a symbol named __ext__attr_secname
 */

#define STYP_SERIAL_MEM    (0x1)        /* Serial memory */


#define PIC32_SECTION_IN_DATA_MEMORY(sec) \
  (PIC32_IS_BSS_ATTR(sec) ||              \
   PIC32_IS_BSS_ATTR_WITH_MEMORY_ATTR(sec) ||              \
   PIC32_IS_DATA_ATTR(sec) ||             \
   PIC32_IS_DATA_ATTR_WITH_MEMORY_ATTR(sec) ||             \
   PIC32_IS_PERSIST_ATTR(sec) ||          \
   PIC32_IS_STACK_ATTR(sec) ||            \
   PIC32_IS_HEAP_ATTR(sec))
#define PIC32_SECTION_IS_BSS_TYPE(sec) \
  (((sec->flags & (SEC_ALLOC | SEC_DATA | SEC_READONLY | SEC_CODE)) \
     == SEC_ALLOC))

struct pic32_memory
{
  struct pic32_memory *next;
  bfd_vma addr;
  bfd_vma size;
  bfd_vma offset;
};

struct expression {
        unsigned char *values_range;
        long long base_value;
        long long addened;};
struct location {
        bfd_boolean unused;
        long long address;
        long long end_address;};
struct pic32_fill_option
{
  struct pic32_fill_option *next;
  struct expression expr;
  struct location loc;
  struct pic32_section *fill_section_list;
  int fill_width;
  int range_size;
};

#endif /* _ELF_PIC32_H */

