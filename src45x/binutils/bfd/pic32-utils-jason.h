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
** Define some structures for the Undefined Symbol hash table.
**
** This is used by the linker to collect object signatures
** for undefined symbols. Signatures are used to help
** choose between multiple definitions of a symbol
** in archive files (aka libraries).
**
** As multiple objects refer to the same symbol,
** a composite signature is created. The most recent file
** to reference is saved, to facilitate error reporting.
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
