/* Microchip PIC32-specific support for 32-bit ELF
   Copyright 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
   2008, 2009, 2010  Free Software Foundation, Inc.

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

#include "bfd.h"
#include "sysdep.h"
#include "elf-bfd.h"
#include "pic32-utils.h"

/* External function prototypes */
extern struct bfd_hash_entry *mchp_undefsym_newfunc
  PARAMS ((struct bfd_hash_entry *, struct bfd_hash_table *, const char *));
extern struct mchp_undefsym_table *mchp_undefsym_init
  PARAMS ((struct bfd_link_info *info));

/* Variables set by pic32 linker options */
bfd_boolean pic32_debug = 0;
bfd_boolean pic32_report_mem = 0;
bfd_boolean pic32_smart_io = TRUE; /* Enabled by default */
bfd_boolean pic32_has_smart_io_option = 0;
bfd *mchp_output_bfd;

/* Data Structures for the Undefined Symbol Table */
struct mchp_undefsym_table *undefsyms;

/**********************************************************************
**
** Undefined Symbol Hash Table Routines
**
** We use a hash table to keep track of undefined
** symbols and their object signatures.
**
** Derived from the basic table provided in /bfd/hash.c
*/

/* Create a new hash table entry */
struct bfd_hash_entry *
mchp_undefsym_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  struct mchp_undefsym_entry *ret = (struct mchp_undefsym_entry *) entry;

  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (ret == (struct mchp_undefsym_entry *) NULL)
    ret = ((struct mchp_undefsym_entry *)
	   bfd_hash_allocate (table, sizeof (struct mchp_undefsym_entry)));
  if (ret == (struct mchp_undefsym_entry *) NULL)
    return NULL;

  /* Call the allocation method of the superclass.  */
  ret = ((struct mchp_undefsym_entry *)
	 bfd_hash_newfunc ((struct bfd_hash_entry *) ret, table, string));

  if (ret) {     /* Initialize the local fields. */
    ret->most_recent_reference = 0;
    ret->external_options_mask = 0;
    ret->options_set = 0;
    }

  return (struct bfd_hash_entry *) ret;
}
/* Create a new hash table */
struct mchp_undefsym_table *
mchp_undefsym_init (struct bfd_link_info *info)
{
  struct mchp_undefsym_table *table;

  table = ((struct mchp_undefsym_table *)
	   bfd_alloc (info->output_bfd, sizeof (struct mchp_undefsym_table)));
  if (table == NULL)
    return NULL;

  if (! bfd_hash_table_init_n (&table->table, mchp_undefsym_newfunc,
                               sizeof(struct mchp_undefsym_entry),
                               MCHP_UNDEFSYM_INIT)) {
      free (table);
      return NULL;
    }

  table->num = 0;
  return table;
}


