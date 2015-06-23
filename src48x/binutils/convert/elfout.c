/* elfout.c -- ELF Output

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

#include "sysdep.h"
#include "elfmips.h"

#define BUFSIZE		8192	/* max bytes per block */

static FILE		*elffp;
static unsigned long	elffirst = ~0;
static unsigned long	elfaddr = ~0;
static long		elfoffs;
static int 		elflen;
static unsigned char	*elfbuf;

static Elf32_Ehdr	ehdr;
#define NSECTIONS	9
static Elf32_Phdr	phdr[NSECTIONS];
static Elf32_Shdr	shdr[NSECTIONS + 1];	/* plus 1 for shsectname */
static int		nsect;

extern char *		progname;
extern int		promflag;
extern unsigned long	prombase;

static void
elfFlush (int endsect)
{
    if (elflen > 0) {
	fwrite (elfbuf, 1, elflen, elffp);
	elfoffs += elflen;
	elfaddr += elflen;
	elflen = 0;
    }

    if (endsect && nsect > 1) {
	Elf32_Shdr *sh = &shdr[nsect - 1];
	sh->sh_size = elfoffs - sh->sh_offset;
    }
}

void
ElfStart (FILE *fp, unsigned long ep, int bigendian)
{
    elfbuf = xmalloc (BUFSIZE);
    elffp = fp;

    memset (&ehdr, 0, sizeof (ehdr));
    ehdr.e_ident[EI_MAG0] = ELFMAG0;
    ehdr.e_ident[EI_MAG1] = ELFMAG1;
    ehdr.e_ident[EI_MAG2] = ELFMAG2;
    ehdr.e_ident[EI_MAG3] = ELFMAG3;
    ehdr.e_ident[EI_CLASS] = ELFCLASS32;
    ehdr.e_ident[EI_DATA] = bigendian ? ELFDATA2MSB : ELFDATA2LSB;
    ehdr.e_ident[EI_VERSION] = EV_CURRENT;
    ehdr.e_type = ET_EXEC;
    ehdr.e_machine = EM_MIPS;
    ehdr.e_version = EV_CURRENT;
    ehdr.e_ehsize = sizeof (Elf32_Ehdr);
    ehdr.e_phentsize = sizeof (Elf32_Phdr);
    ehdr.e_shentsize = sizeof (Elf32_Shdr);
    elfoffs = ehdr.e_shoff + sizeof (shdr);

    nsect = 1;
    fseek (elffp, elfoffs, SEEK_SET);
}


void
ElfOutput (unsigned long addr, unsigned char byte)
{
    if (promflag)
	/* convert back to PROM virtual address in KSEG1 */
	addr = (addr + prombase) | 0xa0000000;

    if (addr != elfaddr + elflen) {
	Elf32_Shdr *sh = &shdr[nsect];

	elfFlush (1);

	if (nsect == NSECTIONS) {
	    fprintf (stderr, "%s: too many ELF output sections\n",
		     progname);
	    exit (1);
	}

	sh->sh_type = SHT_PROGBITS;
	sh->sh_flags = SHF_ALLOC | SHF_EXECINSTR;
	sh->sh_addr = addr;
	sh->sh_offset = elfoffs;
	sh->sh_size = 0;
	sh->sh_addralign = 1;
	nsect++;

	elfaddr = addr;
    }

    if (elflen == BUFSIZE)
	elfFlush (0);
    elfbuf[elflen++] = byte;
}


void
ElfEnd (unsigned long ep)
{
    Elf32_Shdr *sh;
    Elf32_Phdr *ph;
    char * strtab;
    int stroffs, i;

    if (nsect <= 1)
	return;

    elfFlush (1);

    strtab = xmalloc (nsect * sizeof ".psect##"
		     + sizeof ".shstrtab" + 1);

    /* Prepare the Section Header string table */
    stroffs = 0;
    strtab[stroffs++] = '\0';	/* initial null byte */
    for (sh = &shdr[1], i = 1; i < nsect; sh++, i++) {
	sh->sh_name = stroffs;
	sprintf (&strtab[stroffs], ".psect%d", i);
	stroffs += strlen (&strtab[stroffs]) + 1;
    }

    /* Prepare the section header for the string table */
    sh = &shdr[nsect];
    sh->sh_name = stroffs;
    strcpy (&strtab[stroffs], ".shstrtab");
    stroffs += sizeof ".shstrtab";
    sh->sh_type = SHT_STRTAB;
    sh->sh_offset = elfoffs;
    sh->sh_size = stroffs;

    /* Write the Section Header string table */
    fwrite (strtab, 1, stroffs, elffp);
    free (strtab);

    /* Prepare Program Header */
    for (ph = phdr, sh = &shdr[1], i = 1; i < nsect; ph++, sh++, i++) {
	ph->p_type = PT_LOAD;
	ph->p_offset = sh->sh_offset;
	ph->p_vaddr = sh->sh_addr;
	ph->p_paddr = sh->sh_addr;
	ph->p_filesz = ph->p_memsz = sh->sh_size;
	ph->p_flags = PF_R | PF_X;
	ph->p_align = sh->sh_addralign;
    }

    /* Prepare ELF header */
    ehdr.e_phnum = ph - phdr;
    ehdr.e_shstrndx = nsect;
    ehdr.e_shnum = nsect + 1;	/* + 1 for string table */
    ehdr.e_phoff = sizeof (Elf32_Ehdr);
    ehdr.e_shoff = ehdr.e_phoff + sizeof (Elf32_Phdr) * ehdr.e_phnum;
    if (promflag)
	ehdr.e_entry = 0xbfc00000;	/* should this be progreammable */
    else
	ehdr.e_entry = ep;

    /* Write the headers */
    fseek (elffp, 0L, SEEK_SET);
    elfEhdrWrite (elffp, &ehdr);
    elfPhdrWrite (elffp, &ehdr, phdr, ehdr.e_phnum);
    elfShdrWrite (elffp, &ehdr, shdr, ehdr.e_shnum);

    free (elfbuf);
}
