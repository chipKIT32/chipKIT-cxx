# This shell script emits a C file. -*- C -*-
#   Copyright (C) 2007 Cavium Networks.
#
# This file is part of GLD, the Gnu Linker.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
#

# This file is sourced from elf32.em and used to define OCTEON and ELF
# specific things.

source_em ${srcdir}/emultempl/mipself.em

fragment <<EOF

static void
elfocteon_before_parse (void)
{
  gld${EMULATION_NAME}_before_parse ();

  /* To be able to directly map file blocks to memory blocks, demand paging
     requires addresses and file offsets to be equal modulo the page size 
     (64K). We also want segment virtual addresses to be 32M aligned which 
     would require extensive padding in the file up to pagesize.  Instead 
     disable demand paging which is not used on our embedded target.  */
  config.magic_demand_paged = FALSE;
}
EOF

LDEMUL_BEFORE_PARSE=elfocteon_before_parse
