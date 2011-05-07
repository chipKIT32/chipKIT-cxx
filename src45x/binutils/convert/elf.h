/* elf.h: ELF data structures and values

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

typedef unsigned char	Elf_Char;
typedef unsigned short	Elf32_Half;
typedef unsigned long	Elf32_Word;
typedef long		Elf32_Sword;
typedef unsigned long	Elf32_Off;
typedef unsigned long	Elf32_Addr;

#define EI_NIDENT	16

typedef struct {
    Elf_Char	e_ident[EI_NIDENT];
    Elf32_Half	e_type;
    Elf32_Half	e_machine;
    Elf32_Word	e_version;
    Elf32_Addr	e_entry;
    Elf32_Off	e_phoff;
    Elf32_Off	e_shoff;
    Elf32_Word	e_flags;
    Elf32_Half	e_ehsize;
    Elf32_Half	e_phentsize;
    Elf32_Half	e_phnum;
    Elf32_Half	e_shentsize;
    Elf32_Half	e_shnum;
    Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

typedef struct {
  Elf32_Word	p_type;			/* Identifies program segment type */
  Elf32_Off	p_offset;		/* Segment file offset */
  Elf32_Addr	p_vaddr;		/* Segment virtual address */
  Elf32_Addr	p_paddr;		/* Segment physical address */
  Elf32_Word	p_filesz;		/* Segment size in file */
  Elf32_Word	p_memsz;		/* Segment size in memory */
  Elf32_Word	p_flags;		/* Segment flags */
  Elf32_Word	p_align;		/* Segment alignment, file & memory */
} Elf32_Phdr;

typedef struct {
    Elf32_Word	sh_name;
    Elf32_Word	sh_type;
    Elf32_Word	sh_flags;
    Elf32_Addr	sh_addr;
    Elf32_Off	sh_offset;
    Elf32_Word	sh_size;
    Elf32_Word	sh_link;
    Elf32_Word	sh_info;
    Elf32_Word	sh_addralign;
    Elf32_Word	sh_entsize;
} Elf32_Shdr;

typedef struct {
    Elf32_Word	st_name;
    Elf32_Addr	st_value;
    Elf32_Word	st_size;
    Elf_Char	st_info;
    Elf_Char	st_other;
    Elf32_Half	st_shndx;
} Elf32_Sym;

typedef struct {
    Elf32_Addr		r_offset;
    Elf32_Word		r_info;
    Elf32_Sword		r_addend;
} Elf32_Rela;

typedef struct {
    Elf32_Addr		r_offset;
    Elf32_Word		r_info;
} Elf32_Rel;

/* these are the external file sizes (should be absolute values) */
#define ELFEHDRSZ	sizeof(Elf32_Ehdr)
#define ELFPHDRSZ	sizeof(Elf32_Phdr)
#define ELFSHDRSZ	sizeof(Elf32_Shdr)
#define ELFSYMSZ	sizeof(Elf32_Sym)
#define ELFRELASZ	sizeof(Elf32_Rela)
#define ELFRELSZ	sizeof(Elf32_Rel)

size_t elfEhdrRead(FILE *,Elf32_Ehdr *);
size_t elfEhdrWrite(FILE *,const Elf32_Ehdr *);

size_t elfPhdrRead(FILE *, const Elf32_Ehdr *, Elf32_Phdr *, size_t);
size_t elfPhdrWrite(FILE *, const Elf32_Ehdr *, const Elf32_Phdr *, size_t);

size_t elfShdrRead(FILE *,const Elf32_Ehdr *, Elf32_Shdr *,size_t);
size_t elfShdrWrite(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,size_t);

size_t elfSymRead(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,
		  Elf32_Sym *,size_t);
size_t elfSymWrite(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,
		   const Elf32_Sym *,size_t);

size_t elfRelaWrite(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,
		   const Elf32_Rela *,size_t);
size_t elfRelWrite(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,
		   const Elf32_Rel *,size_t);


/* ELF header */

/* e_ident[] fields */
#define EI_MAG0		0		/* id byte 0 index */
#define ELFMAG0		0x7F		/* id byte 0 value */

#define EI_MAG1		1		/* id byte 1 index */
#define ELFMAG1		'E'		/* id byte 1 value */

#define EI_MAG2		2		/* id byte 2 index */
#define ELFMAG2		'L'		/* id byte 3 value */

#define EI_MAG3		3		/* id byte 3 index */
#define ELFMAG3		'F'		/* id byte 3 value */

#define EI_CLASS	4		/* file class */
#define ELFCLASSNONE	0		/* invalid */
#define ELFCLASS32	1		/* 32-bit */
#define ELFCLASS64	2		/* 64-bit */

#define EI_DATA		5		/* data encoding */
#define ELFDATANONE	0		/* invalid */
#define ELFDATA2LSB	1		/* little endian */
#define ELFDATA2MSB	2		/* big endian */

#define EI_VERSION	6		/* file format version */

#define EI_PAD		7		/* start of padding */


/* e_type values */
#define ET_NONE		0		/* no file type */
#define ET_REL		1		/* relocatable  */
#define ET_EXEC		2		/* executable */
#define ET_DYN		3		/* shared object */
#define ET_CORE		4		/* core file */
#define ET_LOPROC	0xFF00		/* processor-specific (lo) */
#define ET_HIPROC	0xFFFF		/* processor-specific (hi) */

/* e_machine values */
#define EM_NONE		0		/* undefined machine */
#define EM_M32		1		/* AT&T WE 32100 */
#define EM_SPARC	2		/* SUN SPARC */
#define EM_386		3		/* Intel 80386 */
#define EM_68K		4		/* Motorola M68K family */
#define EM_88K		5		/* Motorola M88K family */
#define EM_860		7		/* Intel 80860 */
#define EM_MIPS		8		/* MIPS Rx000 */

/* e_version values */
#define EV_NONE		0		/* invalid */
#define EV_CURRENT	1		/* current version */

/* Program header */

/* p_type field */
#define	PT_NULL		0		/* unused */
#define PT_LOAD		1		/* loadable segment */
#define PT_DYNAMIC	2		/* dynamic link information */
#define PT_INTERP	3		/* interpreter */
#define PT_NOTE		4		/* auxiliary information */
#define PT_SHLIB	5		/* shared library */
#define PT_PHDR		6		/* self */
#define PT_LOPROC	0x70000000	/* processor-specific (lo) */
#define PT_HIPROC	0x7FFFFFFF	/* processor-specific (ho) */

/* p_flags bit fields */
#define PF_X		(1 << 0)	/* executable */
#define PF_W		(1 << 1)	/* writable */
#define PF_R		(1 << 2)	/* readable */
#define PF_MASKPROC	0xF0000000	/* processor-specific bits */

/* Section header */

/* sh_type values */
#define SHT_NULL	0		/* unused */
#define SHT_PROGBITS	1		/* program code or data */
#define SHT_SYMTAB	2		/* symbol table */
#define SHT_STRTAB	3		/* string table */
#define SHT_RELA	4		/* relocation records (with addends) */
#define SHT_HASH	5		/* symbol hash table */
#define SHT_DYNAMIC	6		/* dynamic linking table */
#define SHT_NOTE	7		/* comments */
#define SHT_NOBITS	8		/* zero program data (i.e. bss)*/
#define SHT_REL		9		/* relocation records (w/o addends) */
#define SHT_SHLIB	10		/* shared library */
#define SHT_DYNSYM	11		/* dynamic linking symbol table */
#define SHT_LOPROC	0x70000000	/* processor specific (lo) */
#define SHT_HIPROC	0x7FFFFFFF	/* processor specific (hi) */
#define SHT_LOUSER	0x80000000	/* user specific (lo) */
#define SHT_HIUSER	0x8FFFFFFF	/* user specific (hi) */

/* sh_flags bit fields */
#define SHF_WRITE	(1 << 0)	/* writable section */
#define SHF_ALLOC	(1 << 1)	/* allocated in program space */
#define SHF_EXECINSTR	(1 << 2)	/* executable instructions */
#define SHF_MASKPROC	0xF0000000	/* processor-specific bits */

/* symbol binding */
#define STB_LOCAL	0		/* local to this file */
#define STB_GLOBAL	1		/* globally visible */
#define STB_WEAK	2		/* weakly global */
#define STB_LOPROC	13		/* processor-specific (lo) */
#define STB_HIPROC	15		/* processor-specific (hi) */

/* symbol type */
#define STT_NOTYPE	0		/* unspecified */
#define STT_OBJECT	1		/* data object */
#define STT_FUNC	2		/* code object */
#define STT_SECTION	3		/* section symbol */
#define STT_FILE	4		/* file name symbol */
#define STT_LOPROC	13		/* processor-specific (lo) */
#define STT_HIPROC	15		/* processor-specific (hi) */

/* special reserved values for st_shndx */
#define SHN_UNDEF	0		/* undefined (external) symbol */
#define SHN_LORESERV	0xFF00		/* reserved values (lo) */
#define SHN_LOPROC	0xFF00		/* processor-specific (lo) */
#define SHN_HIPROC	0xFF1F		/* processor-specific (hi) */
#define SHN_ABS		0xFFF1		/* absolute symbol */
#define SHN_COMMON	0xFFF2		/* common symbol */
#define SHN_HIRESERVE	0xFFFF		/* reserved values (hi)*/

#define ELF32_R_SYM(i)		((i) >> 8)
#define ELF32_R_TYPE(i)		((i) & 0xff)
#define ELF32_R_INFO(sym,type)	(((sym) << 8) | ((type) & 0xff))

#define ELF32_ST_BIND(i)	((i) >> 4)
#define ELF32_ST_TYPE(i)	((i) & 0xf)
#define ELF32_ST_INFO(bind,type) (((bind) << 4) | ((type) & 0xf))
