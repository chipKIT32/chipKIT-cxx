/*
   Copyright 1990, 1991, 1992, 1993, 1994, 1995, 2001, 2005, 2007
   Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */
   
#ifndef _PIC32_UTILS_H
#define _PIC32_UTILS_H

/*****************************************************************************/

/*
** Macro to clear all section attributes
**
** We also clear parameters to address()
** and merge(), but not align(), reverse().
*/
#define PIC32_CLEAR_ALL_ATTR(sec) \
  { (sec)->flags = 0;             \
  (sec)->near = 0;                \
  (sec)->persistent = 0;          \
  (sec)->absolute = 0;            \
  (sec)->reverse = 0;             \
  (sec)->dma = 0;                 \
  (sec)->memory = 0;              \
  (sec)->heap = 0;                \
  (sec)->stack = 0;               \
  (sec)->vma = 0;                 \
  (sec)->lma = 0;                 \
  (sec)->entsize = 0;}
/*
** Macros used to set section attributes
*/
#define PIC32_SET_CODE_ATTR(sec) \
  (sec)->flags |= (SEC_HAS_CONTENTS | SEC_LOAD | SEC_CODE | SEC_ALLOC);
#define PIC32_SET_DATA_ATTR(sec) \
  (sec)->flags |= (SEC_HAS_CONTENTS | SEC_LOAD | SEC_DATA | SEC_ALLOC);
#define PIC32_SET_BSS_ATTR(sec)   \
  { (sec)->flags |= SEC_ALLOC;      \
  (sec)->flags &= ~(SEC_LOAD | SEC_DATA | SEC_HAS_CONTENTS); }
#define PIC32_SET_PERSIST_ATTR(sec)  \
  { (sec)->persistent = 1;           \
  (sec)->flags |= SEC_ALLOC;         \
  (sec)->flags &= ~(SEC_LOAD | SEC_DATA); }
#define PIC32_SET_MEMORY_ATTR(sec) \
  { (sec)->memory = 1;             \
  (sec)->flags |= (SEC_HAS_CONTENTS | SEC_ALLOC); }
#define PIC32_SET_HEAP_ATTR(sec) \
  { (sec)->heap = 1;             \
  (sec)->flags |= SEC_ALLOC;     \
  (sec)->flags &= ~(SEC_LOAD | SEC_DATA | SEC_HAS_CONTENTS); }
#define PIC32_SET_STACK_ATTR(sec) \
  { (sec)->stack = 1;            \
  (sec)->flags |= SEC_ALLOC;     \
  (sec)->flags &= ~(SEC_LOAD | SEC_DATA | SEC_HAS_CONTENTS); }

#define PIC32_SET_ABSOLUTE_ATTR(sec) \
  (sec)->absolute = 1;
#define PIC32_SET_NEAR_ATTR(sec) \
  (sec)->near = 1;
#define PIC32_SET_REVERSE_ATTR(sec) \
  (sec)->reverse = 1;
#define PIC32_SET_DMA_ATTR(sec) \
  (sec)->dma = 1;
#define PIC32_SET_NOLOAD_ATTR(sec)  \
  { (sec)->flags |= SEC_NEVER_LOAD; \
    (sec)->flags &= ~ SEC_LOAD; }
#define PIC32_SET_MERGE_ATTR(sec) \
  (sec)->flags |= SEC_MERGE;
#define PIC32_SET_INFO_ATTR(sec) \
  { (sec)->flags |= SEC_DEBUGGING; \
    (sec)->flags &= ~SEC_ALLOC; }

/* UNORDERED is used internally by the assembler
   and is not encoded in the object file */
#define PIC32_SET_UNORDERED_ATTR(sec) \
  (sec)->unordered = 1;

/*
** Macros used to query section attributes
*/
#define PIC32_IS_CODE_ATTR(sec) \
  (((sec)->flags & (SEC_CODE | SEC_ALLOC)) == (SEC_CODE | SEC_ALLOC))
#define PIC32_IS_DATA_ATTR(sec) \
  ((((sec)->flags & (SEC_DATA | SEC_ALLOC)) == (SEC_DATA | SEC_ALLOC)) && \
   ((sec)->memory !=1))
#define PIC32_IS_BSS_ATTR(sec) \
  ((((sec)->flags & (SEC_ALLOC|SEC_LOAD|SEC_CODE|SEC_DATA|SEC_HAS_CONTENTS)) == SEC_ALLOC) && \
   ((sec)->persistent != 1) && ((sec)->memory !=1) && \
   ((sec)->heap != 1) && ((sec)->stack !=1))
#define PIC32_IS_PERSIST_ATTR(sec) \
  ((((sec)->flags & (SEC_ALLOC|SEC_LOAD|SEC_CODE|SEC_DATA|SEC_HAS_CONTENTS)) == SEC_ALLOC) && \
   ((sec)->persistent == 1))
#define PIC32_IS_MEMORY_ATTR(sec) \
  ((((sec)->flags & (SEC_ALLOC|SEC_LOAD|SEC_CODE|SEC_DATA)) == SEC_ALLOC) && \
  ((sec)->memory == 1))
#define PIC32_IS_HEAP_ATTR(sec) \
  ((((sec)->flags & (SEC_ALLOC|SEC_LOAD|SEC_CODE|SEC_DATA|SEC_HAS_CONTENTS)) == SEC_ALLOC) && \
  ((sec)->heap == 1))
#define PIC32_IS_STACK_ATTR(sec) \
  ((((sec)->flags & (SEC_ALLOC|SEC_LOAD|SEC_CODE|SEC_DATA|SEC_HAS_CONTENTS)) == SEC_ALLOC) && \
  ((sec)->stack == 1))

#define PIC32_IS_ABSOLUTE_ATTR(sec) \
  ((sec)->absolute == 1)
#define PIC32_IS_NEAR_ATTR(sec) \
  ((sec)->near == 1)
#define PIC32_IS_REVERSE_ATTR(sec) \
  ((sec)->reverse == 1)
#define PIC32_IS_DMA_ATTR(sec) \
  ((sec)->dma == 1)
#define PIC32_IS_NOLOAD_ATTR(sec) \
  (((sec)->flags & SEC_NEVER_LOAD) == SEC_NEVER_LOAD)
#define PIC32_IS_MERGE_ATTR(sec) \
  (((sec)->flags & SEC_MERGE) == SEC_MERGE)
#define PIC32_IS_INFO_ATTR(sec) \
  (((sec)->flags & SEC_DEBUGGING) == SEC_DEBUGGING)

/* UNORDERED is used internally by the assembler
   and is not encoded in the object file */
#define PIC32_IS_UNORDERED_ATTR(sec) \
  ((sec)->unordered == 1)
#if 0
#define PIC30_IS_EXTERNAL_ATTR(sec) \
  (0)

#define PIC30_IS_LOCAL_DATA(sec)       \
  ((!PIC30_IS_EDS_ATTR(sec) &&         \
      (PIC30_IS_BSS_ATTR(sec) ||       \
       PIC30_IS_DATA_ATTR(sec) ||      \
       PIC30_IS_PERSIST_ATTR(sec))))
#endif

#define PIC32_IS_EVEN(value) (((value) & 0x1) == 0)

/*****************************************************************************/

/*
** Define some structures for the Undefined Symbol hash table.
**
** This is used by the linker to collect object signatures
** for undefined symbols. 
**
*/
#define MCHP_UNDEFSYM_INIT 20
struct mchp_undefsym_entry {
  struct bfd_hash_entry root;
  bfd *most_recent_reference;
  int external_options_mask;
  int options_set;
};

struct mchp_undefsym_table {
  struct bfd_hash_table table;
  int num;  /* # of entries */
};

#define mchp_undefsym_lookup(t, string, create, copy) \
  ((struct mchp_undefsym_entry *) \
   bfd_hash_lookup (&(t)->table, (string), (create), (copy)))

extern struct mchp_undefsym_table *undefsyms;

extern struct bfd_hash_entry *mchp_undefsym_newfunc
  PARAMS ((struct bfd_hash_entry *, struct bfd_hash_table *, const char *));

extern struct mchp_undefsym_table *mchp_undefsym_init
  PARAMS ((struct bfd_link_info *info));
  
/*****************************************************************************/
#endif

