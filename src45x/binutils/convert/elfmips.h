/* elfmips.h: MIPS ABI specific ELF definitions

   Copyright (c) 1993-2003, 2008 Free Software Foundation, Inc.
   Contributed by MIPS Technologies, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; If not, write to the Free Software
   Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include "elf.h"

/* MIPSABI program header special */
#define PT_MIPS_REGINFO		(PT_LOPROC + 0)

/* SDEMIPS relocation (rela type) */
#define R_SDE_8			0
#define R_SDE_16		1
#define R_SDE_32		2
#define R_SDE_ILA		35
#define R_SDE_IGPREL		36
#define R_SDE_ILAHI		37
#define R_SDE_ILALO		38
#define R_SDE_IBRA		39
#define R_SDE_IJMP		40
#define R_SDE_ILITERAL		41

/* MIPSABI relocation (rel type) */
#define R_MIPS_NONE		0
#define R_MIPS_16		1
#define R_MIPS_32		2
#define R_MIPS_REL32		3
#define R_MIPS_26		4
#define R_MIPS_HI16		5
#define R_MIPS_LO16		6
#define R_MIPS_GPREL16		7
#define R_MIPS_LITERAL		8
#define R_MIPS_GOT16		9
#define R_MIPS_PC16		10
#define R_MIPS_CALL16		11
#define R_MIPS_GPREL32		12

/* MIPSABI special section numbers */
#define SHN_MIPS_ACOMMON	(SHN_LOPROC + 0)
#define SHN_MIPS_SCOMMON	(SHN_LOPROC + 3)
#define SHN_MIPS_SUNDEFINED	(SHN_LOPROC + 4)

/* MIPSABI special section types */
#define SHT_MIPS_LIBLIST	(SHT_LOPROC + 0)
#define SHT_MIPS_CONFLICT	(SHT_LOPROC + 2)
#define SHT_MIPS_GPTAB		(SHT_LOPROC + 3)
#define SHT_MIPS_UCODE		(SHT_LOPROC + 4)
#define SHT_MIPS_DEBUG		(SHT_LOPROC + 5)
#define SHT_MIPS_REGINFO	(SHT_LOPROC + 6)

/* MIPSABI special section flags */
#define SHF_MIPS_GPREL		0x10000000

/* MIPSABI processor specific flags */
#define EF_MIPS_NOREORDER	0x00000001
#define EF_MIPS_PIC		0x00000002
#define EF_MIPS_CPIC		0x00000004
#define EF_MIPS_ARCH		0xf0000000
#define  E_MIPS_ARCH_1		0x00000000 /* -mips1 */
#define  E_MIPS_ARCH_2		0x10000000 /* -mips2 */
#define  E_MIPS_ARCH_3		0x20000000 /* -mips3 */
#define  E_MIPS_ARCH_4		0x30000000 /* -mips4 */
/* The following are not MIPSABI, but SDE-MIPS only */
#define EF_MIPS_CPU		0x000f0000
#define  E_MIPS_CPU_STD		0x00000000
#define  E_MIPS_CPU_CW4		0x00010000
#define  E_MIPS_CPU_R4100	0x00020000
#define  E_MIPS_CPU_R4650	0x00030000
#define  E_MIPS_CPU_R3900	0x00040000
#define  E_MIPS_CPU_RM52XX	0x00050000
#define EF_MIPS_NOGPCOUNT	0x00001000

/* MIPSABI special sections */

/* .reginfo */
typedef struct {
    Elf32_Word		ri_gprmask;
    Elf32_Word		ri_cprmask[4];
    Elf32_Sword		ri_gp_value;
} Elf32_Reginfo;

#define ELFREGINFOSZ	sizeof(Elf32_Reginfo)

size_t elfReginfoWrite(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,
		       const Elf32_Reginfo *,size_t);
size_t elfReginfoRead(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,
		      Elf32_Reginfo *,size_t);

/* .gptab.xxx */
typedef union {
    struct {
	Elf32_Word		gt_current_g_value;
	Elf32_Word		gt_unused;
    } gt_header;
    struct {
	Elf32_Word		gt_g_value;
	Elf32_Word		gt_bytes;
    } gt_entry;
} Elf32_Gptab;

#define ELFGPTABSZ	sizeof(Elf32_Gptab)

size_t elfGptabRead(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,
		    Elf32_Gptab *,size_t);
size_t elfGptabWrite(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,
		   const Elf32_Gptab *,size_t);

