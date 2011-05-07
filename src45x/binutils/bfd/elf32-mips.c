/* MIPS-specific support for 32-bit ELF
   Copyright 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002,
   2003, 2004, 2005, 2006, 2007, 2008, 2009 Free Software Foundation, Inc.

   Most of the information added by Ian Lance Taylor, Cygnus Support,
   <ian@cygnus.com>.
   N32/64 ABI support added by Mark Mitchell, CodeSourcery, LLC.
   <mark@codesourcery.com>
   Traditional MIPS targets support added by Koundinya.K, Dansk Data
   Elektronik & Operations Research Group. <kk@ddeorg.soft.net>

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


/* This file handles MIPS ELF targets.  SGI Irix 5 uses a slightly
   different MIPS ELF from other targets.  This matters when linking.
   This file supports both, switching at runtime.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "bfdlink.h"
#include "genlink.h"
#include "elf-bfd.h"
#include "elfxx-mips.h"
#include "elf/mips.h"
#include "elf-vxworks.h"

/* Get the ECOFF swapping routines.  */
#include "coff/sym.h"
#include "coff/symconst.h"
#include "coff/internal.h"
#include "coff/ecoff.h"
#include "coff/mips.h"
#define ECOFF_SIGNED_32
#include "ecoffswap.h"

#if defined(TARGET_IS_PIC32MX)
#include "elf32-pic32.c"
#endif

static bfd_reloc_status_type gprel32_with_gp
  (bfd *, asymbol *, arelent *, asection *, bfd_boolean, void *, bfd_vma);
static bfd_reloc_status_type mips_elf_gprel32_reloc
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);
static bfd_reloc_status_type mips32_64bit_reloc
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);
static reloc_howto_type *bfd_elf32_bfd_reloc_type_lookup
  (bfd *, bfd_reloc_code_real_type);
static reloc_howto_type *mips_elf32_rtype_to_howto
  (unsigned int, bfd_boolean);
static void mips_info_to_howto_rel
  (bfd *, arelent *, Elf_Internal_Rela *);
static void mips_info_to_howto_rela
  (bfd *, arelent *, Elf_Internal_Rela *);
static bfd_boolean mips_elf_sym_is_global
  (bfd *, asymbol *);
static bfd_boolean mips_elf32_object_p
  (bfd *);
static bfd_boolean mips_elf_is_local_label_name
  (bfd *, const char *);
static bfd_reloc_status_type mips16_gprel_reloc
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);
static bfd_reloc_status_type mips_elf_final_gp
  (bfd *, asymbol *, bfd_boolean, char **, bfd_vma *);
static bfd_boolean mips_elf_assign_gp
  (bfd *, bfd_vma *);
static bfd_boolean elf32_mips_grok_prstatus
  (bfd *, Elf_Internal_Note *);
static bfd_boolean elf32_mips_grok_psinfo
  (bfd *, Elf_Internal_Note *);
static irix_compat_t elf32_mips_irix_compat
  (bfd *);

extern const bfd_target bfd_elf32_bigmips_vec;
extern const bfd_target bfd_elf32_littlemips_vec;

/* Nonzero if ABFD is using the N32 ABI.  */
#define ABI_N32_P(abfd) \
  ((elf_elfheader (abfd)->e_flags & EF_MIPS_ABI2) != 0)

/* Whether we are trying to be compatible with IRIX at all.  */
#define SGI_COMPAT(abfd) \
  (elf32_mips_irix_compat (abfd) != ict_none)

/* The number of local .got entries we reserve.  */
#define MIPS_RESERVED_GOTNO (2)

/* In case we're on a 32-bit machine, construct a 64-bit "-1" value
   from smaller values.  Start with zero, widen, *then* decrement.  */
#define MINUS_ONE	(((bfd_vma)0) - 1)

/* The relocation table used for SHT_REL sections.  */

static reloc_howto_type elf_mips_howto_table_rel[] =
{
  /* No relocation.  */
  HOWTO (R_MIPS_NONE,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_NONE",		/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 16 bit relocation.  */
  HOWTO (R_MIPS_16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_16",		/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 32 bit relocation.  */
  HOWTO (R_MIPS_32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_32",		/* name */
	 TRUE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 32 bit symbol relative relocation.  */
  HOWTO (R_MIPS_REL32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_REL32",	/* name */
	 TRUE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 26 bit jump address.  */
  HOWTO (R_MIPS_26,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 			/* This needs complex overflow
				   detection, because the upper four
				   bits must match the PC + 4.  */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_26",		/* name */
	 TRUE,			/* partial_inplace */
	 0x03ffffff,		/* src_mask */
	 0x03ffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* High 16 bits of symbol value.  */
  HOWTO (R_MIPS_HI16,		/* type */
	 16,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_hi16_reloc, /* special_function */
	 "R_MIPS_HI16",		/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Low 16 bits of symbol value.  */
  HOWTO (R_MIPS_LO16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_lo16_reloc, /* special_function */
	 "R_MIPS_LO16",		/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* GP relative reference.  */
  HOWTO (R_MIPS_GPREL16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf32_gprel16_reloc, /* special_function */
	 "R_MIPS_GPREL16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Reference to literal section.  */
  HOWTO (R_MIPS_LITERAL,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf32_gprel16_reloc, /* special_function */
	 "R_MIPS_LITERAL",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Reference to global offset table.  */
  HOWTO (R_MIPS_GOT16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_got16_reloc, /* special_function */
	 "R_MIPS_GOT16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 16 bit PC relative reference.  Note that the ABI document has a typo
     and claims R_MIPS_PC16 to be not rightshifted, rendering it useless.
     We do the right thing here.  */
  HOWTO (R_MIPS_PC16,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_PC16",		/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* 16 bit call through global offset table.  */
  HOWTO (R_MIPS_CALL16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_CALL16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 32 bit GP relative reference.  */
  HOWTO (R_MIPS_GPREL32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 mips_elf_gprel32_reloc, /* special_function */
	 "R_MIPS_GPREL32",	/* name */
	 TRUE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* The remaining relocs are defined on Irix 5, although they are
     not defined by the ABI.  */
  EMPTY_HOWTO (13),
  EMPTY_HOWTO (14),
  EMPTY_HOWTO (15),

  /* A 5 bit shift field.  */
  HOWTO (R_MIPS_SHIFT5,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 5,			/* bitsize */
	 FALSE,			/* pc_relative */
	 6,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_SHIFT5",	/* name */
	 TRUE,			/* partial_inplace */
	 0x000007c0,		/* src_mask */
	 0x000007c0,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 6 bit shift field.  */
  /* FIXME: This is not handled correctly; a special function is
     needed to put the most significant bit in the right place.  */
  HOWTO (R_MIPS_SHIFT6,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 6,			/* bitsize */
	 FALSE,			/* pc_relative */
	 6,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_SHIFT6",	/* name */
	 TRUE,			/* partial_inplace */
	 0x000007c4,		/* src_mask */
	 0x000007c4,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A 64 bit relocation.  */
  HOWTO (R_MIPS_64,		/* type */
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 64,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 mips32_64bit_reloc,	/* special_function */
	 "R_MIPS_64",		/* name */
	 TRUE,			/* partial_inplace */
	 MINUS_ONE,		/* src_mask */
	 MINUS_ONE,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Displacement in the global offset table.  */
  HOWTO (R_MIPS_GOT_DISP,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_GOT_DISP",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Displacement to page pointer in the global offset table.  */
  HOWTO (R_MIPS_GOT_PAGE,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_GOT_PAGE",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Offset from page pointer in the global offset table.  */
  HOWTO (R_MIPS_GOT_OFST,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_GOT_OFST",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* High 16 bits of displacement in global offset table.  */
  HOWTO (R_MIPS_GOT_HI16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_GOT_HI16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Low 16 bits of displacement in global offset table.  */
  HOWTO (R_MIPS_GOT_LO16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_GOT_LO16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 64 bit subtraction.  Used in the N32 ABI.  */
  HOWTO (R_MIPS_SUB,		/* type */
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 64,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_SUB",		/* name */
	 TRUE,			/* partial_inplace */
	 MINUS_ONE,		/* src_mask */
	 MINUS_ONE,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Used to cause the linker to insert and delete instructions?  */
  EMPTY_HOWTO (R_MIPS_INSERT_A),
  EMPTY_HOWTO (R_MIPS_INSERT_B),
  EMPTY_HOWTO (R_MIPS_DELETE),

  /* Get the higher value of a 64 bit addend.  */
  HOWTO (R_MIPS_HIGHER,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_HIGHER",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Get the highest value of a 64 bit addend.  */
  HOWTO (R_MIPS_HIGHEST,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_HIGHEST",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* High 16 bits of displacement in global offset table.  */
  HOWTO (R_MIPS_CALL_HI16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_CALL_HI16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Low 16 bits of displacement in global offset table.  */
  HOWTO (R_MIPS_CALL_LO16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_CALL_LO16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Section displacement.  */
  HOWTO (R_MIPS_SCN_DISP,       /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_SCN_DISP",     /* name */
	 TRUE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  EMPTY_HOWTO (R_MIPS_REL16),
  EMPTY_HOWTO (R_MIPS_ADD_IMMEDIATE),
  EMPTY_HOWTO (R_MIPS_PJUMP),
  EMPTY_HOWTO (R_MIPS_RELGOT),

  /* Protected jump conversion.  This is an optimization hint.  No
     relocation is required for correctness.  */
  HOWTO (R_MIPS_JALR,	        /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_JALR",	        /* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x00000000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* TLS GD/LD dynamic relocations.  */
  HOWTO (R_MIPS_TLS_DTPMOD32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_TLS_DTPMOD32",	/* name */
	 TRUE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  HOWTO (R_MIPS_TLS_DTPREL32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_TLS_DTPREL32",	/* name */
	 TRUE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  EMPTY_HOWTO (R_MIPS_TLS_DTPMOD64),
  EMPTY_HOWTO (R_MIPS_TLS_DTPREL64),

  /* TLS general dynamic variable reference.  */
  HOWTO (R_MIPS_TLS_GD,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_TLS_GD",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* TLS local dynamic variable reference.  */
  HOWTO (R_MIPS_TLS_LDM,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_TLS_LDM",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* TLS local dynamic offset.  */
  HOWTO (R_MIPS_TLS_DTPREL_HI16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_TLS_DTPREL_HI16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* TLS local dynamic offset.  */
  HOWTO (R_MIPS_TLS_DTPREL_LO16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_TLS_DTPREL_LO16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* TLS thread pointer offset.  */
  HOWTO (R_MIPS_TLS_GOTTPREL,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_TLS_GOTTPREL",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* TLS IE dynamic relocations.  */
  HOWTO (R_MIPS_TLS_TPREL32,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_TLS_TPREL32",	/* name */
	 TRUE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  EMPTY_HOWTO (R_MIPS_TLS_TPREL64),

  /* TLS thread pointer offset.  */
  HOWTO (R_MIPS_TLS_TPREL_HI16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_TLS_TPREL_HI16", /* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* TLS thread pointer offset.  */
  HOWTO (R_MIPS_TLS_TPREL_LO16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_TLS_TPREL_LO16", /* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 32 bit relocation with no addend.  */
  HOWTO (R_MIPS_GLOB_DAT,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_GLOB_DAT",	/* name */
	 FALSE,			/* partial_inplace */
	 0x0,			/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */
};

/* The reloc used for BFD_RELOC_CTOR when doing a 64 bit link.  This
   is a hack to make the linker think that we need 64 bit values.  */
static reloc_howto_type elf_mips_ctor64_howto =
  HOWTO (R_MIPS_64,		/* type */
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mips32_64bit_reloc,	/* special_function */
	 "R_MIPS_64",		/* name */
	 TRUE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE);		/* pcrel_offset */

static reloc_howto_type elf_mips16_howto_table_rel[] =
{
  /* The reloc used for the mips16 jump instruction.  */
  HOWTO (R_MIPS16_26,		/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 			/* This needs complex overflow
				   detection, because the upper four
				   bits must match the PC.  */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS16_26",		/* name */
	 TRUE,			/* partial_inplace */
	 0x3ffffff,		/* src_mask */
	 0x3ffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* The reloc used for the mips16 gprel instruction.  */
  HOWTO (R_MIPS16_GPREL,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 mips16_gprel_reloc,	/* special_function */
	 "R_MIPS16_GPREL",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,	        /* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A MIPS16 reference to the global offset table.  */
  HOWTO (R_MIPS16_GOT16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_got16_reloc, /* special_function */
	 "R_MIPS16_GOT16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,	        /* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* A MIPS16 call through the global offset table.  */
  HOWTO (R_MIPS16_CALL16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS16_CALL16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,	        /* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* MIPS16 high 16 bits of symbol value.  */
  HOWTO (R_MIPS16_HI16,		/* type */
	 16,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_hi16_reloc, /* special_function */
	 "R_MIPS16_HI16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,	        /* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* MIPS16 low 16 bits of symbol value.  */
  HOWTO (R_MIPS16_LO16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_lo16_reloc, /* special_function */
	 "R_MIPS16_LO16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,	        /* dst_mask */
	 FALSE),		/* pcrel_offset */
};

static reloc_howto_type elf_micromips_howto_table_rel[] =
{
  /* 16 bit relocation.  */
  HOWTO (R_MICROMIPS_16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_lo16_reloc, /* special_function */
	 "R_MICROMIPS_16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  EMPTY_HOWTO (131),
  EMPTY_HOWTO (132),

  /* 26 bit jump address.  */
  HOWTO (R_MICROMIPS_26_S1,	/* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 			/* This needs complex overflow
				   detection, because the upper four
				   bits must match the PC.  */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_26_S1",	/* name */
	 TRUE,			/* partial_inplace */
	 0x3ffffff,		/* src_mask */
	 0x3ffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* High 16 bits of symbol value.  */
  HOWTO (R_MICROMIPS_HI16,	/* type */
	 16,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_hi16_reloc, /* special_function */
	 "R_MICROMIPS_HI16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Low 16 bits of symbol value.  */
  HOWTO (R_MICROMIPS_LO16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_lo16_reloc, /* special_function */
	 "R_MICROMIPS_LO16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* GP relative reference.  */
  HOWTO (R_MICROMIPS_GPREL16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf32_gprel16_reloc, /* special_function */
	 "R_MICROMIPS_GPREL16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Reference to literal section.  */
  HOWTO (R_MICROMIPS_LITERAL,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf32_gprel16_reloc, /* special_function */
	 "R_MICROMIPS_LITERAL",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Reference to global offset table.  */
  HOWTO (R_MICROMIPS_GOT16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_got16_reloc, /* special_function */
	 "R_MICROMIPS_GOT16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* This is for microMIPS branches.  */
  HOWTO (R_MICROMIPS_PC7_S1,	/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 7,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_PC7_S1",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000007f,		/* src_mask */
	 0x0000007f,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  HOWTO (R_MICROMIPS_PC10_S1,	/* type */
	 1,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 10,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_PC10_S1",	/* name */
	 TRUE,			/* partial_inplace */
	 0x000003ff,		/* src_mask */
	 0x000003ff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  HOWTO (R_MICROMIPS_PC16_S1,	/* type */
	 1,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_PC16_S1",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */

  /* 16 bit call through global offset table.  */
  HOWTO (R_MICROMIPS_CALL16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_CALL16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  EMPTY_HOWTO (143),
  EMPTY_HOWTO (144),

  /* Displacement in the global offset table.  */
  HOWTO (R_MICROMIPS_GOT_DISP,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_GOT_DISP",/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Displacement to page pointer in the global offset table.  */
  HOWTO (R_MICROMIPS_GOT_PAGE,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_GOT_PAGE",/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Offset from page pointer in the global offset table.  */
  HOWTO (R_MICROMIPS_GOT_OFST,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_GOT_OFST",/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* High 16 bits of displacement in global offset table.  */
  HOWTO (R_MICROMIPS_GOT_HI16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_GOT_HI16",/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Low 16 bits of displacement in global offset table.  */
  HOWTO (R_MICROMIPS_GOT_LO16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_GOT_LO16",/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 64 bit subtraction.  Used in the N32 ABI.  */
  HOWTO (R_MICROMIPS_SUB,	/* type */
	 0,			/* rightshift */
	 4,			/* size (0 = byte, 1 = short, 2 = long) */
	 64,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_SUB",	/* name */
	 TRUE,			/* partial_inplace */
	 MINUS_ONE,		/* src_mask */
	 MINUS_ONE,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Get the higher value of a 64 bit addend.  */
  HOWTO (R_MICROMIPS_HIGHER,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_HIGHER",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Get the highest value of a 64 bit addend.  */
  HOWTO (R_MICROMIPS_HIGHEST,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_HIGHEST",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* High 16 bits of displacement in global offset table.  */
  HOWTO (R_MICROMIPS_CALL_HI16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_CALL_HI16",/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Low 16 bits of displacement in global offset table.  */
  HOWTO (R_MICROMIPS_CALL_LO16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_CALL_LO16",/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Section displacement.  */
  HOWTO (R_MICROMIPS_SCN_DISP,  /* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_SCN_DISP",/* name */
	 TRUE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Protected jump conversion.  This is an optimization hint.  No
     relocation is required for correctness.  */
  HOWTO (R_MICROMIPS_JALR,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_JALR",	/* name */
	 FALSE,			/* partial_inplace */
	 0x00000000,		/* src_mask */
	 0x00000000,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* Low 16 bits of symbol value.  Note that the high 16 bits of symbol values
     must be zero.  This is used for relaxation.  */
  HOWTO (R_MICROMIPS_HI0_LO16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_HI0_LO16",/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  EMPTY_HOWTO (158),
  EMPTY_HOWTO (159),
  EMPTY_HOWTO (160),
  EMPTY_HOWTO (161),

  /* TLS general dynamic variable reference.  */
  HOWTO (R_MICROMIPS_TLS_GD,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_TLS_GD",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* TLS local dynamic variable reference.  */
  HOWTO (R_MICROMIPS_TLS_LDM,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_TLS_LDM",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* TLS local dynamic offset.  */
  HOWTO (R_MICROMIPS_TLS_DTPREL_HI16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_TLS_DTPREL_HI16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* TLS local dynamic offset.  */
  HOWTO (R_MICROMIPS_TLS_DTPREL_LO16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_TLS_DTPREL_LO16",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* TLS thread pointer offset.  */
  HOWTO (R_MICROMIPS_TLS_GOTTPREL,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_TLS_GOTTPREL",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  EMPTY_HOWTO (167),
  EMPTY_HOWTO (168),

  /* TLS thread pointer offset.  */
  HOWTO (R_MICROMIPS_TLS_TPREL_HI16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_TLS_TPREL_HI16", /* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* TLS thread pointer offset.  */
  HOWTO (R_MICROMIPS_TLS_TPREL_LO16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_TLS_TPREL_LO16", /* name */
	 TRUE,			/* partial_inplace */
	 0x0000ffff,		/* src_mask */
	 0x0000ffff,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  EMPTY_HOWTO (171),

  /* GP- and PC-relative relocations.  */
  HOWTO (R_MICROMIPS_GPREL7_S2,	/* type */
	 2,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 7,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf32_gprel16_reloc, /* special_function */
	 "R_MICROMIPS_GPREL7_S2",	/* name */
	 TRUE,			/* partial_inplace */
	 0x0000007f,		/* src_mask */
	 0x0000007f,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  HOWTO (R_MICROMIPS_PC23_S2,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 23,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MICROMIPS_PC23_S2",	/* name */
	 TRUE,			/* partial_inplace */
	 0x007fffff,		/* src_mask */
	 0x007fffff,		/* dst_mask */
	 TRUE),			/* pcrel_offset */
};

/* 16 bit offset for pc-relative branches.  */
static reloc_howto_type elf_mips_gnu_rel16_s2 =
  HOWTO (R_MIPS_GNU_REL16_S2,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_GNU_REL16_S2",	/* name */
	 TRUE,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 TRUE);			/* pcrel_offset */

/* 32 bit pc-relative.  This was a GNU extension used by embedded-PIC.
   It was co-opted by mips-linux for exception-handling data.  It is no
   longer used, but should continue to be supported by the linker for
   backward compatibility.  (GCC stopped using it in May, 2004.)  */
static reloc_howto_type elf_mips_gnu_pcrel32 =
  HOWTO (R_MIPS_PC32,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 TRUE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_signed, /* complain_on_overflow */
	 _bfd_mips_elf_generic_reloc, /* special_function */
	 "R_MIPS_PC32",		/* name */
	 TRUE,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 TRUE);			/* pcrel_offset */

/* GNU extension to record C++ vtable hierarchy */
static reloc_howto_type elf_mips_gnu_vtinherit_howto =
  HOWTO (R_MIPS_GNU_VTINHERIT,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 NULL,			/* special_function */
	 "R_MIPS_GNU_VTINHERIT", /* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE);		/* pcrel_offset */

/* GNU extension to record C++ vtable member usage */
static reloc_howto_type elf_mips_gnu_vtentry_howto =
  HOWTO (R_MIPS_GNU_VTENTRY,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont, /* complain_on_overflow */
	 _bfd_elf_rel_vtable_reloc_fn, /* special_function */
	 "R_MIPS_GNU_VTENTRY",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE);		/* pcrel_offset */

/* Originally a VxWorks extension, but now used for other systems too.  */
static reloc_howto_type elf_mips_copy_howto =
  HOWTO (R_MIPS_COPY,		/* type */
	 0,			/* rightshift */
	 0,			/* this one is variable size */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MIPS_COPY",		/* name */
	 FALSE,			/* partial_inplace */
	 0x0,         		/* src_mask */
	 0x0,		        /* dst_mask */
	 FALSE);		/* pcrel_offset */

/* Originally a VxWorks extension, but now used for other systems too.  */
static reloc_howto_type elf_mips_jump_slot_howto =
  HOWTO (R_MIPS_JUMP_SLOT,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_bitfield, /* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_MIPS_JUMP_SLOT",	/* name */
	 FALSE,			/* partial_inplace */
	 0x0,         		/* src_mask */
	 0x0,		        /* dst_mask */
	 FALSE);		/* pcrel_offset */

/* Set the GP value for OUTPUT_BFD.  Returns FALSE if this is a
   dangerous relocation.  */

static bfd_boolean
mips_elf_assign_gp (bfd *output_bfd, bfd_vma *pgp)
{
  unsigned int count;
  asymbol **sym;
  unsigned int i;

  /* If we've already figured out what GP will be, just return it.  */
  *pgp = _bfd_get_gp_value (output_bfd);
  if (*pgp)
    return TRUE;

  count = bfd_get_symcount (output_bfd);
  sym = bfd_get_outsymbols (output_bfd);

  /* The linker script will have created a symbol named `_gp' with the
     appropriate value.  */
  if (sym == NULL)
    i = count;
  else
    {
      for (i = 0; i < count; i++, sym++)
	{
	  register const char *name;

	  name = bfd_asymbol_name (*sym);
	  if (*name == '_' && strcmp (name, "_gp") == 0)
	    {
	      *pgp = bfd_asymbol_value (*sym);
	      _bfd_set_gp_value (output_bfd, *pgp);
	      break;
	    }
	}
    }

  if (i >= count)
    {
      /* Only get the error once.  */
      *pgp = 4;
      _bfd_set_gp_value (output_bfd, *pgp);
      return FALSE;
    }

  return TRUE;
}

/* We have to figure out the gp value, so that we can adjust the
   symbol value correctly.  We look up the symbol _gp in the output
   BFD.  If we can't find it, we're stuck.  We cache it in the ELF
   target data.  We don't need to adjust the symbol value for an
   external symbol if we are producing relocatable output.  */

static bfd_reloc_status_type
mips_elf_final_gp (bfd *output_bfd, asymbol *symbol, bfd_boolean relocatable,
		   char **error_message, bfd_vma *pgp)
{
  if (bfd_is_und_section (symbol->section)
      && ! relocatable)
    {
      *pgp = 0;
      return bfd_reloc_undefined;
    }

  *pgp = _bfd_get_gp_value (output_bfd);
  if (*pgp == 0
      && (! relocatable
	  || (symbol->flags & BSF_SECTION_SYM) != 0))
    {
      if (relocatable)
	{
	  /* Make up a value.  */
	  *pgp = symbol->section->output_section->vma /*+ 0x4000*/;
	  _bfd_set_gp_value (output_bfd, *pgp);
	}
      else if (!mips_elf_assign_gp (output_bfd, pgp))
	{
	  *error_message =
	    (char *) _("GP relative relocation when _gp not defined");
	  return bfd_reloc_dangerous;
	}
    }

  return bfd_reloc_ok;
}

/* Do a R_MIPS_GPREL16 relocation.  This is a 16 bit value which must
   become the offset from the gp register.  This function also handles
   R_MIPS_LITERAL relocations, although those can be handled more
   cleverly because the entries in the .lit8 and .lit4 sections can be
   merged.  */

bfd_reloc_status_type
_bfd_mips_elf32_gprel16_reloc (bfd *abfd, arelent *reloc_entry,
			       asymbol *symbol, void *data,
			       asection *input_section, bfd *output_bfd,
			       char **error_message)
{
  bfd_boolean relocatable;
  bfd_reloc_status_type ret;
  bfd_byte *location;
  bfd_vma gp;

  /* R_MIPS_LITERAL/R_MICROMIPS_LITERAL relocations are defined for local
     symbols only.  */
  if ((reloc_entry->howto->type == R_MIPS_LITERAL
       || reloc_entry->howto->type == R_MICROMIPS_LITERAL)
      && output_bfd != NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (symbol->flags & BSF_LOCAL) != 0)
    {
      *error_message = (char *)
	_("literal relocation occurs for an external symbol");
      return bfd_reloc_outofrange;
    }

  if (output_bfd != NULL)
    relocatable = TRUE;
  else
    {
      relocatable = FALSE;
      output_bfd = symbol->section->output_section->owner;
    }

  ret = mips_elf_final_gp (output_bfd, symbol, relocatable, error_message,
			   &gp);
  if (ret != bfd_reloc_ok)
    return ret;

  location = (bfd_byte *) data + reloc_entry->address;
  _bfd_mips16_elf_reloc_unshuffle (abfd, reloc_entry->howto->type, FALSE,
				   location);
  ret = _bfd_mips_elf_gprel16_with_gp (abfd, symbol, reloc_entry,
					input_section, relocatable,
					data, gp);
  _bfd_mips16_elf_reloc_shuffle (abfd, reloc_entry->howto->type, !relocatable,
				 location);
  return ret;
}

/* Do a R_MIPS_GPREL32 relocation.  This is a 32 bit value which must
   become the offset from the gp register.  */

static bfd_reloc_status_type
mips_elf_gprel32_reloc (bfd *abfd, arelent *reloc_entry, asymbol *symbol,
			void *data, asection *input_section, bfd *output_bfd,
			char **error_message)
{
  bfd_boolean relocatable;
  bfd_reloc_status_type ret;
  bfd_vma gp;

  /* R_MIPS_GPREL32 relocations are defined for local symbols only.  */
  if (output_bfd != NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (symbol->flags & BSF_LOCAL) != 0)
    {
      *error_message = (char *)
	_("32bits gp relative relocation occurs for an external symbol");
      return bfd_reloc_outofrange;
    }

  if (output_bfd != NULL)
    relocatable = TRUE;
  else
    {
      relocatable = FALSE;
      output_bfd = symbol->section->output_section->owner;
    }

  ret = mips_elf_final_gp (output_bfd, symbol, relocatable,
			   error_message, &gp);
  if (ret != bfd_reloc_ok)
    return ret;

  return gprel32_with_gp (abfd, symbol, reloc_entry, input_section,
			  relocatable, data, gp);
}

static bfd_reloc_status_type
gprel32_with_gp (bfd *abfd, asymbol *symbol, arelent *reloc_entry,
		 asection *input_section, bfd_boolean relocatable,
		 void *data, bfd_vma gp)
{
  bfd_vma relocation;
  bfd_vma val;

  if (bfd_is_com_section (symbol->section))
    relocation = 0;
  else
    relocation = symbol->value;

  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;

  if (reloc_entry->address > bfd_get_section_limit (abfd, input_section))
    return bfd_reloc_outofrange;

  /* Set val to the offset into the section or symbol.  */
  val = reloc_entry->addend;

  if (reloc_entry->howto->partial_inplace)
    val += bfd_get_32 (abfd, (bfd_byte *) data + reloc_entry->address);

  /* Adjust val for the final section location and GP value.  If we
     are producing relocatable output, we don't want to do this for
     an external symbol.  */
  if (! relocatable
      || (symbol->flags & BSF_SECTION_SYM) != 0)
    val += relocation - gp;

  if (reloc_entry->howto->partial_inplace)
    bfd_put_32 (abfd, val, (bfd_byte *) data + reloc_entry->address);
  else
    reloc_entry->addend = val;

  if (relocatable)
    reloc_entry->address += input_section->output_offset;

  return bfd_reloc_ok;
}

/* Handle a 64 bit reloc in a 32 bit MIPS ELF file.  These are
   generated when addresses are 64 bits.  The upper 32 bits are a simple
   sign extension.  */

static bfd_reloc_status_type
mips32_64bit_reloc (bfd *abfd, arelent *reloc_entry,
		    asymbol *symbol ATTRIBUTE_UNUSED,
		    void *data, asection *input_section,
		    bfd *output_bfd, char **error_message)
{
  bfd_reloc_status_type r;
  arelent reloc32;
  unsigned long val;
  bfd_size_type addr;

  /* Do a normal 32 bit relocation on the lower 32 bits.  */
  reloc32 = *reloc_entry;
  if (bfd_big_endian (abfd))
    reloc32.address += 4;
  reloc32.howto = &elf_mips_howto_table_rel[R_MIPS_32];
  r = bfd_perform_relocation (abfd, &reloc32, data, input_section,
			      output_bfd, error_message);

  /* Sign extend into the upper 32 bits.  */
  val = bfd_get_32 (abfd, (bfd_byte *) data + reloc32.address);
  if ((val & 0x80000000) != 0)
    val = 0xffffffff;
  else
    val = 0;
  addr = reloc_entry->address;
  if (bfd_little_endian (abfd))
    addr += 4;
  bfd_put_32 (abfd, val, (bfd_byte *) data + addr);

  return r;
}

/* Handle a mips16 GP relative reloc.  */

static bfd_reloc_status_type
mips16_gprel_reloc (bfd *abfd, arelent *reloc_entry, asymbol *symbol,
		    void *data, asection *input_section, bfd *output_bfd,
		    char **error_message)
{
  bfd_boolean relocatable;
  bfd_reloc_status_type ret;
  bfd_byte *location;
  bfd_vma gp;

  /* If we're relocating, and this is an external symbol, we don't want
     to change anything.  */
  if (output_bfd != NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (symbol->flags & BSF_LOCAL) != 0)
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    relocatable = TRUE;
  else
    {
      relocatable = FALSE;
      output_bfd = symbol->section->output_section->owner;
    }

  ret = mips_elf_final_gp (output_bfd, symbol, relocatable, error_message,
			   &gp);
  if (ret != bfd_reloc_ok)
    return ret;

  location = (bfd_byte *) data + reloc_entry->address;
  _bfd_mips16_elf_reloc_unshuffle (abfd, reloc_entry->howto->type, FALSE,
				   location);
  ret = _bfd_mips_elf_gprel16_with_gp (abfd, symbol, reloc_entry,
				       input_section, relocatable,
				       data, gp);
  _bfd_mips16_elf_reloc_shuffle (abfd, reloc_entry->howto->type, !relocatable,
				 location);

  return ret;
}

/* A mapping from BFD reloc types to MIPS ELF reloc types.  */

struct elf_reloc_map {
  bfd_reloc_code_real_type bfd_val;
  enum elf_mips_reloc_type elf_val;
};

static const struct elf_reloc_map mips_reloc_map[] =
{
  { BFD_RELOC_NONE, R_MIPS_NONE },
  { BFD_RELOC_16, R_MIPS_16 },
  { BFD_RELOC_32, R_MIPS_32 },
  /* There is no BFD reloc for R_MIPS_REL32.  */
  { BFD_RELOC_64, R_MIPS_64 },
  { BFD_RELOC_MIPS_JMP, R_MIPS_26 },
  { BFD_RELOC_HI16_S, R_MIPS_HI16 },
  { BFD_RELOC_LO16, R_MIPS_LO16 },
  { BFD_RELOC_GPREL16, R_MIPS_GPREL16 },
  { BFD_RELOC_MIPS_LITERAL, R_MIPS_LITERAL },
  { BFD_RELOC_MIPS_GOT16, R_MIPS_GOT16 },
  { BFD_RELOC_16_PCREL_S2, R_MIPS_PC16 },
  { BFD_RELOC_MIPS_CALL16, R_MIPS_CALL16 },
  { BFD_RELOC_GPREL32, R_MIPS_GPREL32 },
  { BFD_RELOC_MIPS_GOT_HI16, R_MIPS_GOT_HI16 },
  { BFD_RELOC_MIPS_GOT_LO16, R_MIPS_GOT_LO16 },
  { BFD_RELOC_MIPS_CALL_HI16, R_MIPS_CALL_HI16 },
  { BFD_RELOC_MIPS_CALL_LO16, R_MIPS_CALL_LO16 },
  { BFD_RELOC_MIPS_SUB, R_MIPS_SUB },
  { BFD_RELOC_MIPS_GOT_PAGE, R_MIPS_GOT_PAGE },
  { BFD_RELOC_MIPS_GOT_OFST, R_MIPS_GOT_OFST },
  { BFD_RELOC_MIPS_GOT_DISP, R_MIPS_GOT_DISP },
  { BFD_RELOC_MIPS_JALR, R_MIPS_JALR },
  { BFD_RELOC_MIPS_TLS_DTPMOD32, R_MIPS_TLS_DTPMOD32 },
  { BFD_RELOC_MIPS_TLS_DTPREL32, R_MIPS_TLS_DTPREL32 },
  { BFD_RELOC_MIPS_TLS_DTPMOD64, R_MIPS_TLS_DTPMOD64 },
  { BFD_RELOC_MIPS_TLS_DTPREL64, R_MIPS_TLS_DTPREL64 },
  { BFD_RELOC_MIPS_TLS_GD, R_MIPS_TLS_GD },
  { BFD_RELOC_MIPS_TLS_LDM, R_MIPS_TLS_LDM },
  { BFD_RELOC_MIPS_TLS_DTPREL_HI16, R_MIPS_TLS_DTPREL_HI16 },
  { BFD_RELOC_MIPS_TLS_DTPREL_LO16, R_MIPS_TLS_DTPREL_LO16 },
  { BFD_RELOC_MIPS_TLS_GOTTPREL, R_MIPS_TLS_GOTTPREL },
  { BFD_RELOC_MIPS_TLS_TPREL32, R_MIPS_TLS_TPREL32 },
  { BFD_RELOC_MIPS_TLS_TPREL64, R_MIPS_TLS_TPREL64 },
  { BFD_RELOC_MIPS_TLS_TPREL_HI16, R_MIPS_TLS_TPREL_HI16 },
  { BFD_RELOC_MIPS_TLS_TPREL_LO16, R_MIPS_TLS_TPREL_LO16 }
};

static const struct elf_reloc_map mips16_reloc_map[] =
{
  { BFD_RELOC_MIPS16_JMP, R_MIPS16_26 - R_MIPS16_min },
  { BFD_RELOC_MIPS16_GPREL, R_MIPS16_GPREL - R_MIPS16_min },
  { BFD_RELOC_MIPS16_GOT16, R_MIPS16_GOT16 - R_MIPS16_min },
  { BFD_RELOC_MIPS16_CALL16, R_MIPS16_CALL16 - R_MIPS16_min },
  { BFD_RELOC_MIPS16_HI16_S, R_MIPS16_HI16 - R_MIPS16_min },
  { BFD_RELOC_MIPS16_LO16, R_MIPS16_LO16 - R_MIPS16_min },
};

static const struct elf_reloc_map micromips_reloc_map[] =
{
  { BFD_RELOC_MICROMIPS_16, R_MICROMIPS_16 - R_MICROMIPS_min },
   { BFD_RELOC_MICROMIPS_JMP, R_MICROMIPS_26_S1 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_HI16_S, R_MICROMIPS_HI16 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_LO16, R_MICROMIPS_LO16 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_GPREL16, R_MICROMIPS_GPREL16 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_LITERAL, R_MICROMIPS_LITERAL - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_GOT16, R_MICROMIPS_GOT16 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_7_PCREL_S1, R_MICROMIPS_PC7_S1 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_10_PCREL_S1, R_MICROMIPS_PC10_S1 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_16_PCREL_S1, R_MICROMIPS_PC16_S1 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_CALL16, R_MICROMIPS_CALL16 - R_MICROMIPS_min },
   { BFD_RELOC_MICROMIPS_GOT_DISP, R_MICROMIPS_GOT_DISP - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_GOT_PAGE, R_MICROMIPS_GOT_PAGE - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_GOT_OFST, R_MICROMIPS_GOT_OFST - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_GOT_HI16, R_MICROMIPS_GOT_HI16 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_GOT_LO16, R_MICROMIPS_GOT_LO16 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_SUB, R_MICROMIPS_SUB - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_HIGHER, R_MICROMIPS_HIGHER - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_HIGHEST, R_MICROMIPS_HIGHEST - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_CALL_HI16, R_MICROMIPS_CALL_HI16 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_CALL_LO16, R_MICROMIPS_CALL_LO16 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_SCN_DISP, R_MICROMIPS_SCN_DISP - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_JALR, R_MICROMIPS_JALR - R_MICROMIPS_min },
  /* There is no BFD reloc for R_MICROMIPS_HI0_LO16.  */
   { BFD_RELOC_MICROMIPS_TLS_GD, R_MICROMIPS_TLS_GD - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_TLS_LDM, R_MICROMIPS_TLS_LDM - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_TLS_DTPREL_HI16,
    R_MICROMIPS_TLS_DTPREL_HI16 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_TLS_DTPREL_LO16,
    R_MICROMIPS_TLS_DTPREL_LO16 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_TLS_GOTTPREL,
    R_MICROMIPS_TLS_GOTTPREL - R_MICROMIPS_min },
   { BFD_RELOC_MICROMIPS_TLS_TPREL_HI16,
    R_MICROMIPS_TLS_TPREL_HI16 - R_MICROMIPS_min },
  { BFD_RELOC_MICROMIPS_TLS_TPREL_LO16,
    R_MICROMIPS_TLS_TPREL_LO16 - R_MICROMIPS_min },
   /* There is no BFD reloc for R_MICROMIPS_GPREL7_S2.  */
  /* There is no BFD reloc for R_MICROMIPS_PC23_S2.  */
};

/* Given a BFD reloc type, return a howto structure.  */

static reloc_howto_type *
bfd_elf32_bfd_reloc_type_lookup (bfd *abfd, bfd_reloc_code_real_type code)
{
  unsigned int i;
  reloc_howto_type *howto_table = elf_mips_howto_table_rel;
  reloc_howto_type *howto16_table = elf_mips16_howto_table_rel;
  reloc_howto_type *howto_micromips_table = elf_micromips_howto_table_rel;

  for (i = 0; i < sizeof (mips_reloc_map) / sizeof (struct elf_reloc_map);
       i++)
    {
      if (mips_reloc_map[i].bfd_val == code)
	return &howto_table[(int) mips_reloc_map[i].elf_val];
    }

  for (i = 0; i < sizeof (mips16_reloc_map) / sizeof (struct elf_reloc_map);
       i++)
    {
      if (mips16_reloc_map[i].bfd_val == code)
	return &howto16_table[(int) mips16_reloc_map[i].elf_val];
    }

  for (i = 0; i < sizeof (micromips_reloc_map) / sizeof (struct elf_reloc_map);
       i++)
    {
      if (micromips_reloc_map[i].bfd_val == code)
	return &howto_micromips_table[(int) micromips_reloc_map[i].elf_val];
    }

  switch (code)
    {
    default:
      bfd_set_error (bfd_error_bad_value);
      return NULL;

    case BFD_RELOC_CTOR:
      /* We need to handle BFD_RELOC_CTOR specially.
	 Select the right relocation (R_MIPS_32 or R_MIPS_64) based on the
	 size of addresses of the ABI.  */
      if ((elf_elfheader (abfd)->e_flags & (E_MIPS_ABI_O64
					    | E_MIPS_ABI_EABI64)) != 0)
	return &elf_mips_ctor64_howto;
      else
	return &howto_table[(int) R_MIPS_32];

    case BFD_RELOC_VTABLE_INHERIT:
      return &elf_mips_gnu_vtinherit_howto;
    case BFD_RELOC_VTABLE_ENTRY:
      return &elf_mips_gnu_vtentry_howto;
    case BFD_RELOC_32_PCREL:
      return &elf_mips_gnu_pcrel32;
    case BFD_RELOC_MIPS_COPY:
      return &elf_mips_copy_howto;
    case BFD_RELOC_MIPS_JUMP_SLOT:
      return &elf_mips_jump_slot_howto;
    }
}

static reloc_howto_type *
bfd_elf32_bfd_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
				 const char *r_name)
{
  unsigned int i;

  for (i = 0;
       i < (sizeof (elf_mips_howto_table_rel)
	    / sizeof (elf_mips_howto_table_rel[0]));
       i++)
    if (elf_mips_howto_table_rel[i].name != NULL
	&& strcasecmp (elf_mips_howto_table_rel[i].name, r_name) == 0)
      return &elf_mips_howto_table_rel[i];

  for (i = 0;
       i < (sizeof (elf_mips16_howto_table_rel)
	    / sizeof (elf_mips16_howto_table_rel[0]));
       i++)
    if (elf_mips16_howto_table_rel[i].name != NULL
	&& strcasecmp (elf_mips16_howto_table_rel[i].name, r_name) == 0)
      return &elf_mips16_howto_table_rel[i];

  for (i = 0;
       i < (sizeof (elf_micromips_howto_table_rel)
	    / sizeof (elf_micromips_howto_table_rel[0]));
       i++)
    if (elf_micromips_howto_table_rel[i].name != NULL
	&& strcasecmp (elf_micromips_howto_table_rel[i].name, r_name) == 0)
      return &elf_micromips_howto_table_rel[i];

  if (strcasecmp (elf_mips_gnu_pcrel32.name, r_name) == 0)
    return &elf_mips_gnu_pcrel32;
  if (strcasecmp (elf_mips_gnu_rel16_s2.name, r_name) == 0)
    return &elf_mips_gnu_rel16_s2;
  if (strcasecmp (elf_mips_gnu_vtinherit_howto.name, r_name) == 0)
    return &elf_mips_gnu_vtinherit_howto;
  if (strcasecmp (elf_mips_gnu_vtentry_howto.name, r_name) == 0)
    return &elf_mips_gnu_vtentry_howto;
  if (strcasecmp (elf_mips_copy_howto.name, r_name) == 0)
    return &elf_mips_copy_howto;
  if (strcasecmp (elf_mips_jump_slot_howto.name, r_name) == 0)
    return &elf_mips_jump_slot_howto;

  return NULL;
}

/* Given a MIPS Elf_Internal_Rel, fill in an arelent structure.  */

static reloc_howto_type *
mips_elf32_rtype_to_howto (unsigned int r_type,
			   bfd_boolean rela_p ATTRIBUTE_UNUSED)
{
  switch (r_type)
    {
    case R_MIPS_GNU_VTINHERIT:
      return &elf_mips_gnu_vtinherit_howto;
    case R_MIPS_GNU_VTENTRY:
      return &elf_mips_gnu_vtentry_howto;
    case R_MIPS_GNU_REL16_S2:
      return &elf_mips_gnu_rel16_s2;
    case R_MIPS_PC32:
      return &elf_mips_gnu_pcrel32;
    case R_MIPS_COPY:
      return &elf_mips_copy_howto;
    case R_MIPS_JUMP_SLOT:
      return &elf_mips_jump_slot_howto;
    default:
      if (r_type >= R_MICROMIPS_min && r_type < R_MICROMIPS_max)
	return &elf_micromips_howto_table_rel[r_type - R_MICROMIPS_min];
      if (r_type >= R_MIPS16_min && r_type < R_MIPS16_max)
        return &elf_mips16_howto_table_rel[r_type - R_MIPS16_min];
      BFD_ASSERT (r_type < (unsigned int) R_MIPS_max);
      return &elf_mips_howto_table_rel[r_type];
    }
}

/* Given a MIPS Elf_Internal_Rel, fill in an arelent structure.  */

static void
mips_info_to_howto_rel (bfd *abfd, arelent *cache_ptr, Elf_Internal_Rela *dst)
{
  const struct elf_backend_data *bed;
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  bed = get_elf_backend_data (abfd);
  cache_ptr->howto = bed->elf_backend_mips_rtype_to_howto (r_type, FALSE);

  /* The addend for a GPREL16 or LITERAL relocation comes from the GP
     value for the object file.  We get the addend now, rather than
     when we do the relocation, because the symbol manipulations done
     by the linker may cause us to lose track of the input BFD.  */
  if (((*cache_ptr->sym_ptr_ptr)->flags & BSF_SECTION_SYM) != 0
      && (gprel16_reloc_p (r_type)
	  || r_type == (unsigned int) R_MIPS_LITERAL
	  || r_type == (unsigned int) R_MICROMIPS_GPREL7_S2
	  || r_type == (unsigned int) R_MICROMIPS_GPREL16
	  || r_type == (unsigned int) R_MICROMIPS_LITERAL))
    cache_ptr->addend = elf_gp (abfd);
}

/* Given a MIPS Elf_Internal_Rela, fill in an arelent structure.  */

static void
mips_info_to_howto_rela (bfd *abfd, arelent *cache_ptr, Elf_Internal_Rela *dst)
{
  mips_info_to_howto_rel (abfd, cache_ptr, dst);

  /* If we ever need to do any extra processing with dst->r_addend
     (the field omitted in an Elf_Internal_Rel) we can do it here.  */
}

/* Determine whether a symbol is global for the purposes of splitting
   the symbol table into global symbols and local symbols.  At least
   on Irix 5, this split must be between section symbols and all other
   symbols.  On most ELF targets the split is between static symbols
   and externally visible symbols.  */

static bfd_boolean
mips_elf_sym_is_global (bfd *abfd ATTRIBUTE_UNUSED, asymbol *sym)
{
  if (SGI_COMPAT (abfd))
    return (sym->flags & BSF_SECTION_SYM) == 0;
  else
    return ((sym->flags & (BSF_GLOBAL | BSF_WEAK | BSF_GNU_UNIQUE)) != 0
	    || bfd_is_und_section (bfd_get_section (sym))
	    || bfd_is_com_section (bfd_get_section (sym)));
}

/* Set the right machine number for a MIPS ELF file.  */

static bfd_boolean
mips_elf32_object_p (bfd *abfd)
{
  unsigned long mach;

  /* Irix 5 and 6 are broken.  Object file symbol tables are not always
     sorted correctly such that local symbols precede global symbols,
     and the sh_info field in the symbol table is not always right.  */
  if (SGI_COMPAT (abfd))
    elf_bad_symtab (abfd) = TRUE;

  if (ABI_N32_P (abfd))
    return FALSE;

  mach = _bfd_elf_mips_mach (elf_elfheader (abfd)->e_flags);
  bfd_default_set_arch_mach (abfd, bfd_arch_mips, mach);

  return TRUE;
}

/* MIPS ELF local labels start with '$', not 'L'.  */

static bfd_boolean
mips_elf_is_local_label_name (bfd *abfd, const char *name)
{
  if (name[0] == '$')
    return TRUE;

  /* On Irix 6, the labels go back to starting with '.', so we accept
     the generic ELF local label syntax as well.  */
  return _bfd_elf_is_local_label_name (abfd, name);
}

/* Support for core dump NOTE sections.  */
static bfd_boolean
elf32_mips_grok_prstatus (bfd *abfd, Elf_Internal_Note *note)
{
  int offset;
  unsigned int size;

  switch (note->descsz)
    {
      default:
	return FALSE;

      case 256:		/* Linux/MIPS */
	/* pr_cursig */
	elf_tdata (abfd)->core_signal = bfd_get_16 (abfd, note->descdata + 12);

	/* pr_pid */
	elf_tdata (abfd)->core_pid = bfd_get_32 (abfd, note->descdata + 24);

	/* pr_reg */
	offset = 72;
	size = 180;

	break;
    }

  /* Make a ".reg/999" section.  */
  return _bfd_elfcore_make_pseudosection (abfd, ".reg",
					  size, note->descpos + offset);
}

static bfd_boolean
elf32_mips_grok_psinfo (bfd *abfd, Elf_Internal_Note *note)
{
  switch (note->descsz)
    {
      default:
	return FALSE;

      case 128:		/* Linux/MIPS elf_prpsinfo */
	elf_tdata (abfd)->core_program
	 = _bfd_elfcore_strndup (abfd, note->descdata + 32, 16);
	elf_tdata (abfd)->core_command
	 = _bfd_elfcore_strndup (abfd, note->descdata + 48, 80);
    }

  /* Note that for some reason, a spurious space is tacked
     onto the end of the args in some (at least one anyway)
     implementations, so strip it off if it exists.  */

  {
    char *command = elf_tdata (abfd)->core_command;
    int n = strlen (command);

    if (0 < n && command[n - 1] == ' ')
      command[n - 1] = '\0';
  }

  return TRUE;
}

static bfd_boolean
elf32_mips_relax_delete_bytes (bfd *abfd,
			      asection *sec, bfd_vma addr, int count)
{
  Elf_Internal_Shdr *symtab_hdr;
  unsigned int sec_shndx;
  bfd_byte *contents;
  Elf_Internal_Rela *irel, *irelend;
  bfd_vma toaddr;
  Elf_Internal_Sym *isym;
  Elf_Internal_Sym *isymend;
  struct elf_link_hash_entry **sym_hashes;
  struct elf_link_hash_entry **end_hashes;
  struct elf_link_hash_entry **start_hashes;
  unsigned int symcount;

  sec_shndx = _bfd_elf_section_from_bfd_section (abfd, sec);
  contents = elf_section_data (sec)->this_hdr.contents;

  toaddr = sec->size;

  irel = elf_section_data (sec)->relocs;
  irelend = irel + sec->reloc_count;

  /* Actually delete the bytes.  */
  memmove (contents + addr, contents + addr + count,
	   (size_t) (toaddr - addr - count));
  sec->size -= count;

  /* Adjust all the relocs.  */
  for (irel = elf_section_data (sec)->relocs; irel < irelend; irel++)
    {
      /* Get the new reloc address.  */
      if ((irel->r_offset > addr
	   && irel->r_offset < toaddr))
	irel->r_offset -= count;
    }

  /* microMIPS local and global symbols have the least significant bit
     set. Because all values are either multiple of 2 or multiple of
     4, compensating for this bit is as simple as setting it in
     comparisons. Just to be sure, check anyway before proceeding. */
  BFD_ASSERT(addr % 2 == 0);
  BFD_ASSERT(toaddr % 2 == 0);
  BFD_ASSERT(count % 2 == 0);

  addr |= 1;
  toaddr |= 1;

  /* Adjust the local symbols defined in this section.  */
  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  isym = (Elf_Internal_Sym *) symtab_hdr->contents;
  for (isymend = isym + symtab_hdr->sh_info; isym < isymend; isym++)
    {
      if (isym->st_shndx == sec_shndx
	  && isym->st_value > addr
	  && isym->st_value < toaddr)
	isym->st_value -= count;
    }

  /* Now adjust the global symbols defined in this section.  */
  symcount = (symtab_hdr->sh_size / sizeof (Elf32_External_Sym)
	      - symtab_hdr->sh_info);
  sym_hashes = start_hashes = elf_sym_hashes (abfd);
  end_hashes = sym_hashes + symcount;

  for (; sym_hashes < end_hashes; sym_hashes++)
    {
      struct elf_link_hash_entry *sym_hash = *sym_hashes;

      if ((sym_hash->root.type == bfd_link_hash_defined
	   || sym_hash->root.type == bfd_link_hash_defweak)
	  && sym_hash->root.u.def.section == sec
	  && sym_hash->root.u.def.value > addr
	  && sym_hash->root.u.def.value < toaddr)
	sym_hash->root.u.def.value -= count;
    }

  return TRUE;
}


/*
 *  Opcodes needed for microMIPS relaxation as found in
 *  opcodes/micromips-opc.c
 */

struct opcode_descriptor {
  unsigned long match;
  unsigned long mask;
};


/*
 *  32 and 16-bit branch
 */
const struct opcode_descriptor b_insns_32[] = {
  { 1, 1 }, /* ignored by find_match() */
  { /* "b",       "p",         */ 0x94000000, 0xffff0000 }, /* beq 0, 0 */
  { /* "b",       "p",         */ 0x40400000, 0xffff0000 }, /* bgez 0 */
  { 0, 0 }  /* end marker for find_match() */
};

const struct opcode_descriptor b_insn_16 =
  { /* "b",       "mD",        */ 0xcc00,     0xfc00 };


/*
 *  32 and 16-bit branch eq and ne zero
 *
 *  NOTE: all opcode tables have beq/bne in the same order: first the
 *  eq and second the ne.  This convention is used when replacing a
 *  32-bit beq/bne with the 16-bit version.
 */

/* By convention, the register in bits 25:21 is zero for the 32-bit
   beqz/c and bnez/c.  The other register is in bits 20:16.  */
#define BZ32_REG(opcode)  (((opcode) >> 16) & 0x1f)
#define BZ32_REG_FIELD(r) (((r) & 0x1f) << 16)

const struct opcode_descriptor bz_insns_32[] = {
  { 1, 1 }, /* ignored by find_match() */
  { /* "beqz",    "s,p",       */ 0x94000000, 0xffe00000 },
  { /* "bnez",    "s,p",       */ 0xb4000000, 0xffe00000 },
  { 0, 0 }  /* end marker for find_match() */
};

const struct opcode_descriptor bzc_insns_32[] = {
  { 1, 1 }, /* ignored by find_match() */
  { /* "beqzc",   "s,p",       */ 0x40e00000, 0xffe00000 },
  { /* "bnezc",   "s,p",       */ 0x40a00000, 0xffe00000 },
  { 0, 0 }  /* end marker for find_match() */
};

const struct opcode_descriptor bz_insns_16[] = {
  { 1, 1 }, /* ignored by find_match() */
  { /* "beqz",    "md,mE",     */ 0x8c00,     0xfc00 },
  { /* "bnez",    "md,mE",     */ 0xac00,     0xfc00 },
  { 0, 0 }  /* end marker for find_match() */
};

#define BZ16_VALID_REG(r) \
  ((2 <= (r) && (r) <= 7) || (16 <= (r) && (r) <= 17))
#define BZ16_REG_FIELD(r) \
  (((2 <= (r) && (r) <= 7)? (r): ((r) - 16)) << 7)


/*
 *  32-bit instructions with delay slot
 *
 *  NOTE jalr_insns_32_bdXX opcode tables have jalr instructions
 *  listed in the same order.  This convention is used to replace a
 *  32-bit delay slot jalr with a 16-bit delay slot jalr.
 */
const struct opcode_descriptor jal_insn_32_bd16 =
  { /* "jals",    "a",	       */ 0x74000000, 0xfc000000 };

const struct opcode_descriptor jal_insn_32_bd32 =
  { /* "jal",     "a",	       */ 0xf4000000, 0xfc000000 };

const struct opcode_descriptor jalr_insns_32_bd16[] = {
  { 1, 1 }, /* ignored by find_match() */
  { /* "jalrs",   "t,s",       */ 0x00004f3c, 0xfc00ffff },
  { /* "jalrs.hb","t,s",       */ 0x00005f3c, 0xfc00ffff },
};

const struct opcode_descriptor jalr_insns_32_bd32[] = {
  { 1, 1 }, /* ignored by find_match() */
  { /* "jalr",    "t,s",       */ 0x00000f3c, 0xfc00ffff },
  { /* "jalr.hb", "t,s",       */ 0x00001f3c, 0xfc00ffff },
  { 0, 0 }  /* end marker for find_match() */
};

const struct opcode_descriptor ds_insns_32_bd16[] = {
  { 1, 1 }, /* ignored by find_match() */
  { /* "jals",    "a",	       */ 0x74000000, 0xfc000000 },

  { /* "jalrs",   "t,s",       */ 0x00004f3c, 0xfc00ffff },
  { /* "jalrs.hb","t,s",       */ 0x00005f3c, 0xfc00ffff },

  { /* "beq",     "s,t,p",     */ 0x94000000, 0xfc000000 },
  { /* "bgez",    "s,p",       */ 0x40400000, 0xffe00000 },
  { /* "bgtz",    "s,p",       */ 0x40c00000, 0xffe00000 },
  { /* "blez",    "s,p",       */ 0x40800000, 0xffe00000 },
  { /* "bltz",    "s,p",       */ 0x40000000, 0xffe00000 },
  { /* "bne",     "s,t,p",     */ 0xb4000000, 0xfc000000 },
  { /* "j",       "a",         */ 0xd4000000, 0xfc000000 },
  { 0, 0 }  /* end marker for find_match() */
};

const struct opcode_descriptor ds_insns_32_bd32[] = {
  { 1, 1 }, /* ignored by find_match() */
  { /* "bal",     "p",	       */ 0x40600000, 0xffff0000 },
  { /* "bgezal",  "s,p",       */ 0x40600000, 0xffe00000 },
  { /* "bltzal",  "s,p",       */ 0x40200000, 0xffe00000 },
  { /* "jalx",    "a",	       */ 0xf0000000, 0xfc000000 },
  { 0, 0 }  /* end marker for find_match() */
};


/*
 *  16-bit instructions with delay slot
 */
const struct opcode_descriptor jalr_insn_16_bd16 =
  { /* "jalrs",   "my,mj",     */ 0x45e0,     0xffe0 };

const struct opcode_descriptor jalr_insn_16_bd32 =
  { /* "jalr",    "my,mj",     */ 0x45c0,     0xffe0 };

const struct opcode_descriptor ds_insns_16_bd16[] = {
  { 1, 1 },/* ignored by find_match() */
  { /* "jalrs",   "my,mj",     */ 0x45e0,     0xffe0 },

  { /* "b",       "mD",        */ 0xcc00,     0xfc00 },
  { /* "beqz",    "md,mE",     */ 0x8c00,     0xfc00 },
  { /* "bnez",    "md,mE",     */ 0xac00,     0xfc00 },
  { /* "jr",      "mj",        */ 0x4580,     0xffe0 },
  { 0, 0 }  /* end marker for find_match() */
};


/*
 *  lui instruction
 */
const struct opcode_descriptor lui_insn =
 { /* "lui",     "s,u",        */ 0x41a00000, 0xffe00000 };


/*
 * addiu instruction
 */
const struct opcode_descriptor addiu_insn =
  { /* "addiu",   "t,r,j",     */ 0x30000000, 0xfc000000 };

const struct opcode_descriptor addiupc_insn =
  { /* "addiu",  "mb,$pc,mQ",  */ 0x78000000, 0xfc000000 };

#define ADDIU_REG(opcode) (((opcode) >> 21) & 0x1f)
#define ADDIUPC_VALID_REG(r) \
  ((2 <= (r) && (r) <= 7) || (16 <= (r) && (r) <= 17))
#define ADDIUPC_REG_FIELD(r) \
  (((2 <= (r) && (r) <= 7)? (r): ((r) - 16)) << 23)


/*
 *  gp-relative lw instructions
 */
const struct opcode_descriptor lwgp_insn_32 =
  { /* "lw",      "t,o($28)",  */ 0xfc1c0000, 0xfc1f0000 };

const struct opcode_descriptor lwgp_insn_16 =
  { /* "lw",     "md,mA($28)", */ 0x6400,     0xfc00 };

#define LWGP32_REG(opcode) (((opcode) >> 21) & 0x1f)
#define LWGP16_VALID_REG(r) \
  ((2 <= (r) && (r) <= 7) || (16 <= (r) && (r) <= 17))
#define LWGP16_REG_FIELD(r) \
  (((2 <= (r) && (r) <= 7)? (r): ((r) - 16)) << 7)


/*
 *  relaxable instructions in JAL delay slot: move
 *
 */

/* The 16-bit move has rd in 9:5 and rs in 4:0. The 32-bit moves
   (addu, or) have rd in 15:11 and rs in 10:16 */
#define MOVE32_RD(opcode) (((opcode) >> 11) & 0x1f)
#define MOVE32_RS(opcode) (((opcode) >> 16) & 0x1f)

#define MOVE16_RD_FIELD(r) (((r) & 0x1f) << 5)
#define MOVE16_RS_FIELD(r) (((r) & 0x1f)     )

const struct opcode_descriptor move_insns_32[] = {
  { 1, 1 }, /* ignored by find_match() */
  { /* "move",    "d,s",	*/ 0x00000150, 0xffe007ff }, /* addu d,s,$0 */
  { /* "move",    "d,s",	*/ 0x00000290, 0xffe007ff }, /* or   d,s,$0 */
  { 0, 0 }  /* end marker for find_match() */
};

const struct opcode_descriptor move_insn_16 =
  { /* "move",    "mp,mj",	*/ 0x0c00,     0xfc00 };


/*
 *  nop instructions
 */
const struct opcode_descriptor nop_insn_32 =
  { /* "nop",     "",		*/ 0x00000000, 0xffffffff };

const struct opcode_descriptor nop_insn_16 =
  { /* "nop",     "",		*/ 0x0c00,     0xffff };


/*
 *  Instruction match support
 */
#define MATCH(opcode, insn) ((opcode & insn.mask) == insn.match)

static unsigned long
find_match (unsigned long opcode, const struct opcode_descriptor insn[])
{
    unsigned long indx;

    /* First opcode_table[] entry is ignored.  */
    for (indx = 1; insn[indx].mask != 0; indx++)
      if (MATCH (opcode, insn[indx]))
	return indx;

    return 0;
}


/*
 *  Delay slot size and relaxation support
 */
static unsigned long
relax_delay_slot (bfd *abfd, bfd_byte *ptr, unsigned long* opcode_32)
{
  unsigned long bdsize = 0;

  unsigned long fndopc;
  unsigned long p16opc, p32opc;

  /* Opcodes preceding the current instruction.  */
  p16opc  = bfd_get_16 (abfd, ptr - 2);
  p32opc  = bfd_get_16 (abfd, ptr - 4) << 16;
  p32opc |= p16opc;

  *opcode_32 = p32opc;

  if ((fndopc = find_match (p32opc, jalr_insns_32_bd32)) != 0)
    {
      /* 32-bit JALR with 32-bit delay slot that is changed to 32-bit
         JALRS with 16-bit delay slot.  */
      p32opc = jalr_insns_32_bd16[fndopc].match
	       | (p32opc & (~jalr_insns_32_bd16[fndopc].mask));
      bfd_put_16 (abfd, (p32opc >> 16) & 0xffff, ptr - 4);
      bfd_put_16 (abfd,  p32opc        & 0xffff, ptr - 2);

      bdsize = 16;
    }
  else if (MATCH (p16opc, jalr_insn_16_bd32) != 0)
    {
      /* 16-bit JALR with 32-bit delay slot that is changed to 16-bit
         JALRS with 16-bit delay slot.  */
      p16opc = jalr_insn_16_bd16.match
	       | (p16opc & (~jalr_insn_16_bd16.mask));
      bfd_put_16 (abfd,  p16opc & 0xffff, ptr - 2);

      bdsize = 16;
    }
  else if (find_match (p32opc, ds_insns_32_bd32) != 0)
    {
      /* branch/jump with 32-bit delay slot.  */
      bdsize = 32;
    }
  else if (find_match (p32opc, ds_insns_32_bd16) != 0
	   || find_match (p16opc, ds_insns_16_bd16) != 0)
    {
      /* branch/jump with 16-bit delay slot.  */
      bdsize = 16;
    }

  return bdsize;
}

/*
 *  Bitsize checking
 */
#define IS_BITSIZE(val, adjustment, N)				\
  ((((unsigned int) (val) + (adjustment)) >> (N-1)) ==  0 ||	\
   (((signed   int) (val) - (adjustment)) >> (N-1)) == -1)


static bfd_boolean
elf32_mips_relax_section (bfd *abfd, asection *sec,
			  struct bfd_link_info *link_info, bfd_boolean *again)
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Rela *internal_relocs;
  Elf_Internal_Rela *irel, *irelend;
  bfd_byte *contents = NULL;
  Elf_Internal_Sym *isymbuf = NULL;

  /* Assume nothing changes.  */
  *again = FALSE;

  /* We don't have to do anything for a relocatable link, if
     this section does not have relocs, or if this is not a
     code section.  */

  if (link_info->relocatable
      || (sec->flags & SEC_RELOC) == 0
      || sec->reloc_count == 0
      || (sec->flags & SEC_CODE) == 0)
    return TRUE;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;

  /* Get a copy of the native relocations.  */
  internal_relocs = (_bfd_elf_link_read_relocs
		     (abfd, sec, (PTR) NULL, (Elf_Internal_Rela *) NULL,
		      link_info->keep_memory));
  if (internal_relocs == NULL)
    goto error_return;

  /* Walk through them looking for relaxing opportunities.  */
  irelend = internal_relocs + sec->reloc_count;
  for (irel = internal_relocs; irel < irelend; irel++)
    {
      bfd_vma symval;
      bfd_vma pcrval;

      bfd_boolean target_is_micromips_code_p = FALSE;

      unsigned long opcode;

      unsigned long p32opc;
      unsigned long fndopc;

      /* The number of bytes to delete for relaxation and from where
         to delete these bytes starting at irel->r_offset.  */
      int delcnt = 0;
      int deloff = 0;

      /* If this isn't something that can be relaxed, then ignore
	 this reloc.  */
      if (ELF32_R_TYPE (irel->r_info) != (int) R_MICROMIPS_HI16 &&
	  ELF32_R_TYPE (irel->r_info) != (int) R_MICROMIPS_LO16 &&
	  ELF32_R_TYPE (irel->r_info) != (int) R_MICROMIPS_PC16_S1 &&
	  ELF32_R_TYPE (irel->r_info) != (int) R_MICROMIPS_26_S1 &&
	  ELF32_R_TYPE (irel->r_info) != (int) R_MICROMIPS_GPREL16)
	continue;

      /* Get the section contents if we haven't done so already.  */
      if (contents == NULL)
	{
	  /* Get cached copy if it exists.  */
	  if (elf_section_data (sec)->this_hdr.contents != NULL)
	    contents = elf_section_data (sec)->this_hdr.contents;
	  /* Go get them off disk.  */
	  else if (!bfd_malloc_and_get_section (abfd, sec, &contents))
	    goto error_return;
	}

      /* Read this BFD's local symbols if we haven't done so already.  */
      if (isymbuf == NULL && symtab_hdr->sh_info != 0)
	{
	  isymbuf = (Elf_Internal_Sym *) symtab_hdr->contents;
	  if (isymbuf == NULL)
	    isymbuf = bfd_elf_get_elf_syms (abfd, symtab_hdr,
					    symtab_hdr->sh_info, 0,
					    NULL, NULL, NULL);
	  if (isymbuf == NULL)
	    goto error_return;
	}

      /* Get the value of the symbol referred to by the reloc.  */
      if (ELF32_R_SYM (irel->r_info) < symtab_hdr->sh_info)
	{
	  /* A local symbol.  */
	  Elf_Internal_Sym *isym;
	  asection *sym_sec;

	  isym = isymbuf + ELF32_R_SYM (irel->r_info);
	  if (isym->st_shndx == SHN_UNDEF)
	    sym_sec = bfd_und_section_ptr;
	  else if (isym->st_shndx == SHN_ABS)
	    sym_sec = bfd_abs_section_ptr;
	  else if (isym->st_shndx == SHN_COMMON)
	    sym_sec = bfd_com_section_ptr;
	  else
	    sym_sec = bfd_section_from_elf_index (abfd, isym->st_shndx);
	  symval = (isym->st_value
		    + sym_sec->output_section->vma
		    + sym_sec->output_offset);
	  target_is_micromips_code_p = ELF_ST_IS_MICROMIPS (isym->st_other);
	}
      else
	{
	  unsigned long indx;
	  struct elf_link_hash_entry *h;

	  /* An external symbol.  */
	  indx = ELF32_R_SYM (irel->r_info) - symtab_hdr->sh_info;
	  h = elf_sym_hashes (abfd)[indx];
	  BFD_ASSERT (h != NULL);

	  if (h->root.type != bfd_link_hash_defined
	      && h->root.type != bfd_link_hash_defweak)
	    /* This appears to be a reference to an undefined
	       symbol.  Just ignore it -- it will be caught by the
	       regular reloc processing.  */
	    continue;

	  symval = (h->root.u.def.value
		    + h->root.u.def.section->output_section->vma
		    + h->root.u.def.section->output_offset);
	}


      /* For simplicity of coding, we are going to modify the
         section contents, the section relocs, and the BFD symbol
         table.  We must tell the rest of the code not to free up this
         information.  It would be possible to instead create a table
         of changes which have to be made, as is done in coff-mips.c;
         that would be more work, but would require less memory when
         the linker is run.  */

      opcode  =   bfd_get_16 (abfd, contents + irel->r_offset    ) << 16;
      opcode |= (irel->r_offset + 2 < sec->size
		 ? bfd_get_16 (abfd, contents + irel->r_offset + 2) : 0);

      pcrval = (symval
		- (sec->output_section->vma + sec->output_offset)
		- (irel->r_offset + 2));

      /* R_MICROMIPS_HI16 / LUI relaxation to R_MICROMIPS_HI0_LO16 or
         R_MICROMIPS_PC23_S2.  The R_MICROMIPS_PC23_S2 condition is

           (symval % 4 == 0 && IS_BITSIZE (pcrval, X, 25))

         where the X adjustment compensate for R_MICROMIPS_HI16 and
         R_MICROMIPS_LO16 being at most X bytes appart when the
         distance to the target approaches 32 MB.  */
      if (ELF32_R_TYPE (irel->r_info) == (int) R_MICROMIPS_HI16
	  && MATCH (opcode, lui_insn)
	  && (IS_BITSIZE (symval, 0, 16)
	      || (symval % 4 == 0 && IS_BITSIZE (pcrval, 128, 25))))
	{
	  unsigned long bdsize;

	  /* Delay slot relaxation.  */
	  bdsize = relax_delay_slot (abfd, contents + irel->r_offset, &p32opc);

	  /* Assume is possible to delete the LUI instruction:
	     4 bytes at irel->r_offset.  */
	  delcnt = 4;
	  deloff = 0;

	  /* Fix the relocation's type.  */
	  irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
				       R_MIPS_NONE);

	  /* If LUI sits in a delay slot, either replace LUI itself
	     with a 16- or 32-bit nop, or, if the delay slot is from a
	     compactable branch, replace the original branch with the
	     compact version.  */
	  if (bdsize == 16)
	    {
	       /* Replace the 32-bit LUI instruction with a 16-bit NOP.  */
	      bfd_put_16 (abfd, nop_insn_16.match, contents + irel->r_offset);

	      /* Delete the remainder 2 bytes at irel->r_offset + 2.  */
	      delcnt = 2;
	      deloff = 2;
	    }
	  else if (bdsize == 32)
	    {
	      /* Replace the 32-bit LUI instruction with a 32-bit NOP.  */
	      bfd_put_16 (abfd, (nop_insn_32.match >> 16) & 0xffff,
			  contents + irel->r_offset);
	      bfd_put_16 (abfd,  nop_insn_32.match        & 0xffff,
			  contents + irel->r_offset + 2);

	      /* Can't delete anything.  */
	      delcnt = 0;
	    }
	  else if ((fndopc = find_match (p32opc, bz_insns_32)) != 0)
	    {
	      /* Replace beqz/bnez with the compact version.  */
	      p32opc = (bzc_insns_32[fndopc].match
			| BZ32_REG_FIELD (BZ32_REG (opcode))
			| 0xfffe);			/* addend value */

	      bfd_put_16 (abfd, (p32opc >> 16) & 0xffff,
			  contents + irel->r_offset - 4);
	      bfd_put_16 (abfd,  p32opc        & 0xffff,
			  contents + irel->r_offset - 2);
	    }
	}

      /* R_MICROMIPS_LO16 relaxation to R_MICROMIPS_HI0_LO16.  */
      else if (ELF32_R_TYPE (irel->r_info) == (int) R_MICROMIPS_LO16
	       && IS_BITSIZE (symval, 0, 16))
	{
	  /* Fix the relocation's type.  */
	  irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
				       R_MICROMIPS_HI0_LO16);

	  /* Instructions using R_MICROMIPS_LO16 have the base or
	     source register in bits 20:16.  This register becomes $0
	     (zero) as the result of the R_MICROMIPS_HI16 being 0.  */
	  opcode &= ~0x001F0000;
	  bfd_put_16 (abfd, (opcode >> 16) & 0xffff,
		      contents + irel->r_offset);
	}

      /* R_MICROMIPS_LO16 / ADDIU relaxation to R_MICROMIPS_PC23_S2.  */
      else if (ELF32_R_TYPE (irel->r_info) == (int) R_MICROMIPS_LO16
	       && symval % 4 == 0
	       && IS_BITSIZE (pcrval, 128, 25)
	       && MATCH (opcode, addiu_insn)
	       && ADDIUPC_VALID_REG (ADDIU_REG (opcode)))
	{
	  /* Fix the relocation's type.  */
	  irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
				       R_MICROMIPS_PC23_S2);

	  /* Replace addiu with the addiupc version.  */
	  opcode = (addiupc_insn.match
		    | ADDIUPC_REG_FIELD (ADDIU_REG (opcode)));

	  bfd_put_16 (abfd, (opcode >> 16) & 0xffff,
		      contents + irel->r_offset);
	  bfd_put_16 (abfd,  opcode        & 0xffff,
		      contents + irel->r_offset + 2);
	}

      /* Compact branch relaxation -- due to the multitude of macros
         employed by the compiler/assembler, compact branches are not
         aways generated.  Obviously, this can/will be fixed elsewhere,
         but there is no drawback in double checking it here.  */
      else if (ELF32_R_TYPE (irel->r_info) == (int) R_MICROMIPS_PC16_S1
	       && (fndopc = find_match (opcode, bz_insns_32)) != 0
	       && MATCH (bfd_get_16 (abfd, contents + irel->r_offset + 4),
			 nop_insn_16))
	{
	  /* Replace beqz/bnez with the compact version.  */
	  opcode = (bzc_insns_32[fndopc].match
		    | BZ32_REG_FIELD (BZ32_REG (opcode))
		    | 0xfffe);				/* addend value */

	  bfd_put_16 (abfd, (opcode >> 16) & 0xffff,
		      contents + irel->r_offset);
	  bfd_put_16 (abfd,  opcode        & 0xffff,
		      contents + irel->r_offset + 2);

	  /* Delete delay slot nop: two bytes from irel->offset + 4.  */
	  delcnt = 2;
	  deloff = 4;
	}

      /* R_MICROMIPS_PC16_S1 relaxation to R_MICROMIPS_PC10_S1.  */
      else if (ELF32_R_TYPE (irel->r_info) == (int) R_MICROMIPS_PC16_S1
	       && IS_BITSIZE (pcrval, 0, 11)
	       && find_match (opcode, b_insns_32))
	{
	  /* Fix the relocation's type.  */
	  irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
				       R_MICROMIPS_PC10_S1);

	  /* Replace the the 32-bit opcode with a 16-bit opcode.  */
	  bfd_put_16 (abfd,
		      b_insn_16.match | 0x3ff,		/* addend value */
		      contents + irel->r_offset);

	  /* Delete 2 bytes from irel->r_offset + 2.  */
	  delcnt = 2;
	  deloff = 2;
	}

      /* R_MICROMIPS_PC16_S1 relaxation to R_MICROMIPS_PC7_S1.  */
      else if (ELF32_R_TYPE (irel->r_info) == (int) R_MICROMIPS_PC16_S1
	       && IS_BITSIZE (pcrval, 0, 8)
	       && (fndopc = find_match (opcode, bz_insns_32)) != 0
	       && BZ16_VALID_REG (BZ32_REG (opcode)))
	{
	  /* Fix the relocation's type.  */
	  irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
				       R_MICROMIPS_PC7_S1);

	  /* Replace the the 32-bit opcode with a 16-bit opcode.  */
	  bfd_put_16 (abfd,
		      (bz_insns_16[fndopc].match
		       | BZ16_REG_FIELD (BZ32_REG (opcode))
		       | 0x7f),				/* addend value */
		      contents + irel->r_offset);

	  /* Delete 2 bytes from irel->r_offset + 2.  */
	  delcnt = 2;
	  deloff = 2;
	}

      /* R_MICROMIPS_26_S1 -- JAL to JALS relaxation for microMIPS targets.  */
      else if (ELF32_R_TYPE (irel->r_info) == (int) R_MICROMIPS_26_S1
	       && target_is_micromips_code_p
	       && MATCH (opcode, jal_insn_32_bd32))
	{
	  unsigned long n32opc;
	  bfd_boolean relaxed = FALSE;

	  n32opc  =
	    bfd_get_16 (abfd, contents + irel->r_offset + 4    ) << 16;
	  n32opc |= irel->r_offset + 2 < sec->size?
	    bfd_get_16 (abfd, contents + irel->r_offset + 6): 0;

	  if (MATCH (n32opc, nop_insn_32))
	    {
	      /* Replace delay slot 32-bit nop with a 16-bit nop.  */
	      bfd_put_16 (abfd, nop_insn_16.match,
			  contents + irel->r_offset + 4);

	      relaxed = TRUE;
	  }
	  else if (find_match (n32opc, move_insns_32))
	    {
	      /* Replace delay slot 32-bit move with 16-bit move.  */
	      bfd_put_16 (abfd,
			  (move_insn_16.match
			   | MOVE16_RD_FIELD (MOVE32_RD (n32opc))
			   | MOVE16_RS_FIELD (MOVE32_RS (n32opc))),
			  contents + irel->r_offset + 4);

	      relaxed = TRUE;
	    }
	  /* Other 32-bit instructions relaxable to 16-bit
	     instructions will be handled here later.  */

	  if (relaxed)
	    {
	      /* JAL with 32-bit delay slot that is changed to a JALS
	         with 16-bit delay slot.  */
	      bfd_put_16 (abfd, (jal_insn_32_bd16.match >> 16) & 0xffff,
			  contents + irel->r_offset);
	      bfd_put_16 (abfd,  jal_insn_32_bd16.match        & 0xffff,
			  contents + irel->r_offset + 2);

	      /* Delete 2 bytes from irel->r_offset + 6.  */
	      delcnt = 2;
	      deloff = 6;
	    }
	}

      if (delcnt != 0)
	{
	  /* Note that we've changed the relocs, section contents, etc.  */
	  elf_section_data (sec)->relocs = internal_relocs;
	  elf_section_data (sec)->this_hdr.contents = contents;
	  symtab_hdr->contents = (unsigned char *) isymbuf;

	  /* Delete bytes depending on the delcnt and deloff.  */
	  if (!elf32_mips_relax_delete_bytes (abfd, sec,
					      irel->r_offset + deloff,
					      delcnt))
	    goto error_return;

	  /* That will change things, so, we should relax again.
	     Note that this is not required, and it may be slow.  */
	  *again = TRUE;
	}
    }

  if (isymbuf != NULL
      && symtab_hdr->contents != (unsigned char *) isymbuf)
    {
      if (! link_info->keep_memory)
	free (isymbuf);
      else
	{
	  /* Cache the symbols for elf_link_input_bfd.  */
	  symtab_hdr->contents = (unsigned char *) isymbuf;
	}
    }

  if (contents != NULL
      && elf_section_data (sec)->this_hdr.contents != contents)
    {
      if (! link_info->keep_memory)
	free (contents);
      else
	{
	  /* Cache the section contents for elf_link_input_bfd.  */
	  elf_section_data (sec)->this_hdr.contents = contents;
	}
    }

  if (internal_relocs != NULL
      && elf_section_data (sec)->relocs != internal_relocs)
    free (internal_relocs);

  return TRUE;

 error_return:
  if (isymbuf != NULL
      && symtab_hdr->contents != (unsigned char *) isymbuf)
    free (isymbuf);
  if (contents != NULL
      && elf_section_data (sec)->this_hdr.contents != contents)
    free (contents);
  if (internal_relocs != NULL
      && elf_section_data (sec)->relocs != internal_relocs)
    free (internal_relocs);

  return FALSE;
}

/* Depending on the target vector we generate some version of Irix
   executables or "normal" MIPS ELF ABI executables.  */
static irix_compat_t
elf32_mips_irix_compat (bfd *abfd)
{
  if ((abfd->xvec == &bfd_elf32_bigmips_vec)
      || (abfd->xvec == &bfd_elf32_littlemips_vec))
    return ict_irix5;
  else
    return ict_none;
}

/* ECOFF swapping routines.  These are used when dealing with the
   .mdebug section, which is in the ECOFF debugging format.  */
static const struct ecoff_debug_swap mips_elf32_ecoff_debug_swap = {
  /* Symbol table magic number.  */
  magicSym,
  /* Alignment of debugging information.  E.g., 4.  */
  4,
  /* Sizes of external symbolic information.  */
  sizeof (struct hdr_ext),
  sizeof (struct dnr_ext),
  sizeof (struct pdr_ext),
  sizeof (struct sym_ext),
  sizeof (struct opt_ext),
  sizeof (struct fdr_ext),
  sizeof (struct rfd_ext),
  sizeof (struct ext_ext),
  /* Functions to swap in external symbolic data.  */
  ecoff_swap_hdr_in,
  ecoff_swap_dnr_in,
  ecoff_swap_pdr_in,
  ecoff_swap_sym_in,
  ecoff_swap_opt_in,
  ecoff_swap_fdr_in,
  ecoff_swap_rfd_in,
  ecoff_swap_ext_in,
  _bfd_ecoff_swap_tir_in,
  _bfd_ecoff_swap_rndx_in,
  /* Functions to swap out external symbolic data.  */
  ecoff_swap_hdr_out,
  ecoff_swap_dnr_out,
  ecoff_swap_pdr_out,
  ecoff_swap_sym_out,
  ecoff_swap_opt_out,
  ecoff_swap_fdr_out,
  ecoff_swap_rfd_out,
  ecoff_swap_ext_out,
  _bfd_ecoff_swap_tir_out,
  _bfd_ecoff_swap_rndx_out,
  /* Function to read in symbolic data.  */
  _bfd_mips_elf_read_ecoff_info
};

#define ELF_ARCH			bfd_arch_mips
#define ELF_MACHINE_CODE		EM_MIPS

#define elf_backend_collect		TRUE
#define elf_backend_type_change_ok	TRUE
#define elf_backend_can_gc_sections	TRUE
#define elf_info_to_howto		mips_info_to_howto_rela
#define elf_info_to_howto_rel		mips_info_to_howto_rel
#define elf_backend_sym_is_global	mips_elf_sym_is_global
#define elf_backend_object_p		mips_elf32_object_p
#define elf_backend_symbol_processing	_bfd_mips_elf_symbol_processing
#define elf_backend_section_processing	_bfd_mips_elf_section_processing
#define elf_backend_section_from_shdr	_bfd_mips_elf_section_from_shdr
#define elf_backend_fake_sections	_bfd_mips_elf_fake_sections
#define elf_backend_section_from_bfd_section \
					_bfd_mips_elf_section_from_bfd_section
#define elf_backend_add_symbol_hook	_bfd_mips_elf_add_symbol_hook
#define elf_backend_link_output_symbol_hook \
					_bfd_mips_elf_link_output_symbol_hook
#define elf_backend_create_dynamic_sections \
					_bfd_mips_elf_create_dynamic_sections
#define elf_backend_check_relocs	_bfd_mips_elf_check_relocs
#define elf_backend_merge_symbol_attribute \
					_bfd_mips_elf_merge_symbol_attribute
#define elf_backend_get_target_dtag	_bfd_mips_elf_get_target_dtag
#define elf_backend_adjust_dynamic_symbol \
					_bfd_mips_elf_adjust_dynamic_symbol
#define elf_backend_always_size_sections \
					_bfd_mips_elf_always_size_sections
#define elf_backend_size_dynamic_sections \
					_bfd_mips_elf_size_dynamic_sections
#define elf_backend_init_index_section	_bfd_elf_init_1_index_section
#define elf_backend_relocate_section	_bfd_mips_elf_relocate_section
#define elf_backend_finish_dynamic_symbol \
					_bfd_mips_elf_finish_dynamic_symbol
#define elf_backend_finish_dynamic_sections \
					_bfd_mips_elf_finish_dynamic_sections
#define elf_backend_final_write_processing \
					_bfd_mips_elf_final_write_processing
#define elf_backend_additional_program_headers \
					_bfd_mips_elf_additional_program_headers
#define elf_backend_modify_segment_map	_bfd_mips_elf_modify_segment_map
#define elf_backend_gc_mark_hook	_bfd_mips_elf_gc_mark_hook
#define elf_backend_gc_sweep_hook	_bfd_mips_elf_gc_sweep_hook
#define elf_backend_copy_indirect_symbol \
					_bfd_mips_elf_copy_indirect_symbol
#define elf_backend_grok_prstatus	elf32_mips_grok_prstatus
#define elf_backend_grok_psinfo		elf32_mips_grok_psinfo
#define elf_backend_ecoff_debug_swap	&mips_elf32_ecoff_debug_swap

#define elf_backend_got_header_size	(4 * MIPS_RESERVED_GOTNO)
#define elf_backend_may_use_rel_p	1
#define elf_backend_may_use_rela_p	0
#define elf_backend_default_use_rela_p	0
#define elf_backend_sign_extend_vma	TRUE
#define elf_backend_plt_readonly	1
#define elf_backend_plt_sym_val		_bfd_mips_elf_plt_sym_val

#define elf_backend_discard_info	_bfd_mips_elf_discard_info
#define elf_backend_ignore_discarded_relocs \
					_bfd_mips_elf_ignore_discarded_relocs
#define elf_backend_write_section	_bfd_mips_elf_write_section
#define elf_backend_mips_irix_compat	elf32_mips_irix_compat
#define elf_backend_mips_rtype_to_howto	mips_elf32_rtype_to_howto
#define bfd_elf32_bfd_is_local_label_name \
					mips_elf_is_local_label_name
#define bfd_elf32_find_nearest_line	_bfd_mips_elf_find_nearest_line
#define bfd_elf32_find_inliner_info	_bfd_mips_elf_find_inliner_info
#define bfd_elf32_new_section_hook	_bfd_mips_elf_new_section_hook
#define bfd_elf32_set_section_contents	_bfd_mips_elf_set_section_contents
#define bfd_elf32_bfd_get_relocated_section_contents \
				_bfd_elf_mips_get_relocated_section_contents
#define bfd_elf32_mkobject		_bfd_mips_elf_mkobject
#define bfd_elf32_bfd_link_hash_table_create \
					_bfd_mips_elf_link_hash_table_create
#define bfd_elf32_bfd_final_link	_bfd_mips_elf_final_link
#define bfd_elf32_bfd_merge_private_bfd_data \
					_bfd_mips_elf_merge_private_bfd_data
#define bfd_elf32_bfd_set_private_flags	_bfd_mips_elf_set_private_flags
#define bfd_elf32_bfd_print_private_bfd_data \
					_bfd_mips_elf_print_private_bfd_data
#define bfd_elf32_bfd_relax_section	elf32_mips_relax_section

/* Support for SGI-ish mips targets.  */
#define TARGET_LITTLE_SYM		bfd_elf32_littlemips_vec
#define TARGET_LITTLE_NAME		"elf32-littlemips"
#define TARGET_BIG_SYM			bfd_elf32_bigmips_vec
#define TARGET_BIG_NAME			"elf32-bigmips"

/* The SVR4 MIPS ABI says that this should be 0x10000, but Irix 5 uses
   a value of 0x1000, and we are compatible.  */
#define ELF_MAXPAGESIZE			0x1000
#define ELF_COMMONPAGESIZE		0x1000

#include "elf32-target.h"

/* Support for traditional mips targets.  */
#undef TARGET_LITTLE_SYM
#undef TARGET_LITTLE_NAME
#undef TARGET_BIG_SYM
#undef TARGET_BIG_NAME

#undef ELF_MAXPAGESIZE
#undef ELF_COMMONPAGESIZE

#define TARGET_LITTLE_SYM               bfd_elf32_tradlittlemips_vec
#define TARGET_LITTLE_NAME              "elf32-tradlittlemips"
#define TARGET_BIG_SYM                  bfd_elf32_tradbigmips_vec
#define TARGET_BIG_NAME                 "elf32-tradbigmips"

/* The MIPS ABI says at Page 5-1:
   Virtual addresses and file offsets for MIPS segments are congruent
   modulo 64 KByte (0x10000) or larger powers of 2.  Because 64 KBytes
   is the maximum page size, the files are suitable for paging
   regardless of physical page size.  */
#define ELF_MAXPAGESIZE			0x10000
#define ELF_COMMONPAGESIZE		0x1000
#define elf32_bed			elf32_tradbed

/* Include the target file again for this target.  */
#include "elf32-target.h"

/* Implement elf_backend_final_write_processing for VxWorks.  */

static void
mips_vxworks_final_write_processing (bfd *abfd, bfd_boolean linker)
{
  _bfd_mips_elf_final_write_processing (abfd, linker);
  elf_vxworks_final_write_processing (abfd, linker);
}

#undef TARGET_LITTLE_SYM
#undef TARGET_LITTLE_NAME
#undef TARGET_BIG_SYM
#undef TARGET_BIG_NAME

#undef ELF_MAXPAGESIZE
#undef ELF_COMMONPAGESIZE

#define TARGET_LITTLE_SYM               bfd_elf32_littlemips_vxworks_vec
#define TARGET_LITTLE_NAME              "elf32-littlemips-vxworks"
#define TARGET_BIG_SYM                  bfd_elf32_bigmips_vxworks_vec
#define TARGET_BIG_NAME                 "elf32-bigmips-vxworks"

#undef elf32_bed
#define elf32_bed			elf32_mips_vxworks_bed

#define ELF_MAXPAGESIZE			0x1000
#define ELF_COMMONPAGESIZE		0x1000

#undef elf_backend_want_got_plt
#define elf_backend_want_got_plt		1
#undef elf_backend_want_plt_sym
#define elf_backend_want_plt_sym		1
#undef elf_backend_may_use_rel_p
#define elf_backend_may_use_rel_p		0
#undef elf_backend_may_use_rela_p
#define elf_backend_may_use_rela_p		1
#undef elf_backend_default_use_rela_p
#define elf_backend_default_use_rela_p		1
#undef elf_backend_got_header_size
#define elf_backend_got_header_size		(4 * 3)
#undef elf_backend_plt_sym_val

#undef elf_backend_finish_dynamic_symbol
#define elf_backend_finish_dynamic_symbol \
  _bfd_mips_vxworks_finish_dynamic_symbol
#undef bfd_elf32_bfd_link_hash_table_create
#define bfd_elf32_bfd_link_hash_table_create \
  _bfd_mips_vxworks_link_hash_table_create
#undef elf_backend_add_symbol_hook
#define elf_backend_add_symbol_hook \
  elf_vxworks_add_symbol_hook
#undef elf_backend_link_output_symbol_hook
#define elf_backend_link_output_symbol_hook \
  elf_vxworks_link_output_symbol_hook
#undef elf_backend_emit_relocs
#define elf_backend_emit_relocs \
  elf_vxworks_emit_relocs
#undef elf_backend_final_write_processing
#define elf_backend_final_write_processing \
  mips_vxworks_final_write_processing

#undef elf_backend_additional_program_headers
#undef elf_backend_modify_segment_map
#undef elf_backend_symbol_processing
/* NOTE: elf_backend_rela_normal is not defined for MIPS.  */

#include "elf32-target.h"
