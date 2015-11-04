/* elfio.c: ELF file i/o primitives

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

#define ELFINU8(b,w) \
  (w = bp[0])

#define ELFINMSBU16(b,w) \
  (w = ((b)[0] << 8) | (b)[1])

#define ELFINLSBU16(b,w) \
  (w = ((b)[1] << 8) | (b)[0])

#define ELFINMSBU32(b,w) \
  (w = ((unsigned long)(b)[0] << 24) | \
       ((unsigned long)(b)[1] << 16) | \
       ((b)[2] << 8) | \
       (b)[3])

#define ELFINLSBU32(b,w) \
  (w = ((unsigned long)(b)[3] << 24) | \
       ((unsigned long)(b)[2] << 16) | \
       ((b)[1] << 8) | \
       (b)[0])

#define ELFOUTU8(b,w) \
  (bp[0] = w)

#define ELFOUTMSBU16(b,w) \
  ((b)[0] = (w >> 8), \
   (b)[1]  = w)

#define ELFOUTLSBU16(b,w) \
  ((b)[1] = (w >> 8), \
   (b)[0]  = w)

#define ELFOUTMSBU32(b,w) \
  ((b)[0] = (w >> 24), \
   (b)[1] = (w >> 16), \
   (b)[2] = (w >> 8), \
   (b)[3]  = w)

#define ELFOUTLSBU32(b,w) \
  ((b)[3] = (w >> 24), \
   (b)[2] = (w >> 16), \
   (b)[1] = (w >> 8), \
   (b)[0]  = w)

static unsigned long elfinlsbu32(unsigned char *bp)
{
    unsigned long v;
    ELFINLSBU32(bp,v);
    return v;
}

static unsigned int elfinlsbu16(unsigned char *bp)
{
    unsigned int v;
    ELFINLSBU16(bp,v);
    return v;
}

static unsigned long elfinmsbu32(unsigned char *bp)
{
    unsigned long v;
    ELFINMSBU32(bp,v);
    return v;
}
static unsigned int elfinmsbu16(unsigned char *bp)
{
    unsigned int v;
    ELFINMSBU16(bp,v);
    return v;
}

static void elfoutmsbu32(unsigned char *bp,unsigned long v)
{
    ELFOUTMSBU32(bp,v);
}

static void elfoutmsbu16(unsigned char *bp,unsigned int v)
{
    ELFOUTMSBU16(bp,v);
}

static void elfoutlsbu32(unsigned char *bp,unsigned long v)
{
    ELFOUTLSBU32(bp,v);
}

static void elfoutlsbu16(unsigned char *bp,unsigned int v)
{
    ELFOUTLSBU16(bp,v);
}


#define X3

#if defined(X1)
/* this is applaing because there is no optimisation of the test
 * of format between succesive macro calls
*/
#define ELFINU16(b,w) (format?ELFINLSBU16(b,w):ELFINMSBU16(b,w))
#define ELFINU32(b,w) (format?ELFINLSBU32(b,w):ELFINMSBU32(b,w))
#define ELFOUTU16(b,w) (format?ELFOUTLSBU16(b,w):ELFOUTMSBU16(b,w))
#define ELFOUTU32(b,w) (format?ELFOUTLSBU32(b,w):ELFOUTMSBU32(b,w))
#elif defined(X2)
/* this is also appaling because there is no optimisation of the test
 * of format between succesive macro calls
*/
#define ELFINU16(b,w) w = (format?elfinlsbu16(b):elfinmsbu16(b))
#define ELFINU32(b,w) w = (format?elfinlsbu32(b):elfinmsbu32(b))
#define ELFOUTU16(b,w) (format?elfoutlsbu16(b,w):elfoutmsbu16(b,w))
#define ELFOUTU32(b,w) (format?elfoutlsbu32(b,w):elfoutmsbu32(b,w))
#elif defined(X3)
/* this works better but still is poor as the function address
 * is loaded from table before each call
*/
static const struct {
    void (*out16)(unsigned char *bp,unsigned int v);
    void (*out32)(unsigned char *bp,unsigned long v);
    unsigned int (*in16)(unsigned char *bp);
    unsigned long (*in32)(unsigned char *bp);
} fvec[] = {
    {elfoutmsbu16,elfoutmsbu32,elfinmsbu16,elfinmsbu32},
    {elfoutlsbu16,elfoutlsbu32,elfinlsbu16,elfinlsbu32}
};

#define ELFINU16(b,w) w = fvec[format].in16(b)
#define ELFINU32(b,w) w = fvec[format].in32(b)
#define ELFOUTU16(b,w) fvec[format].out16(b,w)
#define ELFOUTU32(b,w) fvec[format].out32(b,w)
#endif


#define ELFINCHAR(b,w) 	(ELFINU8(b,w),(b)+=1)
#define ELFINHALF(b,w) 	(ELFINU16(b,w),(b)+=2)
#define ELFINWORD(b,w) 	(ELFINU32(b,w),(b)+=4)
#define ELFINADDR(b,w) 	(ELFINU32(b,w),(b)+=4)
#define ELFINOFF(b,w) 	(ELFINU32(b,w),(b)+=4)

#define ELFOUTCHAR(b,w) (ELFOUTU8(b,w),(b)+=1)
#define ELFOUTHALF(b,w) (ELFOUTU16(b,w),(b)+=2)
#define ELFOUTWORD(b,w) (ELFOUTU32(b,w),(b)+=4)
#define ELFOUTADDR(b,w) (ELFOUTU32(b,w),(b)+=4)
#define ELFOUTOFF(b,w) 	(ELFOUTU32(b,w),(b)+=4)

/* Maximum sizes of things we will consider reading
 * these are a bit more than that cos of poosible compiler struct padding
 */
#define MAXEHDRSIZE	sizeof(Elf32_Ehdr)
#define MAXPHDRSIZE	sizeof(Elf32_Phdr)
#define MAXSHDRSIZE	sizeof(Elf32_Shdr)
#define MAXRELASIZE	sizeof(Elf32_Rela)
#define MAXSYMSIZE	sizeof(Elf32_Sym)
#define MAXGPTABSIZE	sizeof(Elf32_Gptab)
#define MAXREGINFOSIZE	sizeof(Elf32_Reginfo)

#define FORMATSELECT(eh) ((eh->e_ident[5] == ELFDATA2MSB)?0:1)

size_t elfShdrRead(FILE *fp,
		   const Elf32_Ehdr *eh, Elf32_Shdr *sh,size_t nhdr)
{
    int format = FORMATSELECT(eh);
    size_t n;

    for(n=0; n<nhdr; sh++,n++) {
  	unsigned char 	buf[MAXSHDRSIZE];
	unsigned char 	*bp = buf;

	if(fread(buf,eh->e_shentsize,1,fp) != 1)
	  break;

	/* convert from bytes to numbers
	 */
	ELFINWORD(bp,sh->sh_name);
	ELFINWORD(bp,sh->sh_type);
	ELFINWORD(bp,sh->sh_flags);
	ELFINADDR(bp,sh->sh_addr);
	ELFINOFF(bp,sh->sh_offset);
	ELFINWORD(bp,sh->sh_size);
	ELFINWORD(bp,sh->sh_link);
	ELFINWORD(bp,sh->sh_info);
	ELFINWORD(bp,sh->sh_addralign);
	ELFINWORD(bp,sh->sh_entsize);
    }
    return n;
}

size_t elfShdrWrite(FILE *fp,
		  const Elf32_Ehdr *eh,
		  const Elf32_Shdr *sh,size_t nhdr)
{
    int format = FORMATSELECT(eh);
    size_t n;

    for(n=0; n<nhdr; sh++,n++) {
  	unsigned char 	buf[MAXSHDRSIZE];
	unsigned char 	*bp = buf;

	/* convert from numbers to bytes
	 */
	ELFOUTWORD(bp,sh->sh_name);
	ELFOUTWORD(bp,sh->sh_type);
	ELFOUTWORD(bp,sh->sh_flags);
	ELFOUTADDR(bp,sh->sh_addr);
	ELFOUTOFF(bp,sh->sh_offset);
	ELFOUTWORD(bp,sh->sh_size);
	ELFOUTWORD(bp,sh->sh_link);
	ELFOUTWORD(bp,sh->sh_info);
	ELFOUTWORD(bp,sh->sh_addralign);
	ELFOUTWORD(bp,sh->sh_entsize);

	if(fwrite(buf,bp-buf,1,fp) != 1)
	  break;
    }
    return n;
}

size_t elfPhdrRead(FILE *fp,
		   const Elf32_Ehdr *eh, Elf32_Phdr *ph,size_t nhdr)
{
    int format = FORMATSELECT(eh);
    size_t n;

    for(n=0; n<nhdr; ph++,n++) {
  	unsigned char 	buf[MAXPHDRSIZE];
	unsigned char 	*bp = buf;

	if(fread(buf,eh->e_phentsize,1,fp) != 1)
	  break;

	/* convert from bytes to numbers */
	ELFINWORD(bp,ph->p_type);
	ELFINOFF(bp,ph->p_offset);
	ELFINADDR(bp,ph->p_vaddr);
	ELFINADDR(bp,ph->p_paddr);
	ELFINWORD(bp,ph->p_filesz);
	ELFINWORD(bp,ph->p_memsz);
	ELFINWORD(bp,ph->p_flags);
	ELFINWORD(bp,ph->p_align);
    }
    return n;
}

size_t elfPhdrWrite(FILE *fp,
		  const Elf32_Ehdr *eh,
		  const Elf32_Phdr *ph,size_t nhdr)
{
    int format = FORMATSELECT(eh);
    size_t n;

    for(n=0; n<nhdr; ph++,n++) {
  	unsigned char 	buf[MAXPHDRSIZE];
	unsigned char 	*bp = buf;

	/* convert from numbers to bytes */
	ELFOUTWORD(bp,ph->p_type);
	ELFOUTOFF(bp,ph->p_offset);
	ELFOUTADDR(bp,ph->p_vaddr);
	ELFOUTADDR(bp,ph->p_paddr);
	ELFOUTWORD(bp,ph->p_filesz);
	ELFOUTWORD(bp,ph->p_memsz);
	ELFOUTWORD(bp,ph->p_flags);
	ELFOUTWORD(bp,ph->p_align);

	if(fwrite(buf,bp-buf,1,fp) != 1)
	  break;
    }
    return n;
}

size_t elfSymRead(FILE *fp,
		   const Elf32_Ehdr *eh,
		   const Elf32_Shdr *sh,
		   Elf32_Sym *sym,size_t nsym)
{
    int format = FORMATSELECT(eh);
    size_t n;

    for(n=0; n<nsym; sym++,n++) {
  	unsigned char 	buf[MAXSYMSIZE];
	unsigned char 	*bp = buf;

	if(fread(buf,sh->sh_entsize,1,fp) != 1)
	  break;

	/* convert from bytes to numbers
	 */
	ELFINWORD(bp,sym->st_name);
	ELFINADDR(bp,sym->st_value);
	ELFINWORD(bp,sym->st_size);
	ELFINCHAR(bp,sym->st_info);
	ELFINCHAR(bp,sym->st_other);
	ELFINHALF(bp,sym->st_shndx);
    }
    return n;
}

size_t elfSymWrite(FILE *fp,
		   const Elf32_Ehdr *eh,
		   const Elf32_Shdr *sh,
		   const Elf32_Sym *sym,size_t nhdr)
{
    int format = FORMATSELECT(eh);
    size_t n;

    for(n=0; n<nhdr; sym++,n++) {
  	unsigned char 	buf[MAXSYMSIZE];
	unsigned char 	*bp = buf;

	/* convert from numbers to bytes
	 */
	ELFOUTWORD(bp,sym->st_name);
	ELFOUTADDR(bp,sym->st_value);
	ELFOUTWORD(bp,sym->st_size);
	ELFOUTCHAR(bp,sym->st_info);
	ELFOUTCHAR(bp,sym->st_other);
	ELFOUTHALF(bp,sym->st_shndx);

	if(fwrite(buf,bp-buf,1,fp) != 1)
	  break;
    }
    return n;
}

size_t elfRelaWrite(FILE *fp,
		   const Elf32_Ehdr *eh,
		   const Elf32_Shdr *sh,
		   const Elf32_Rela *rela,size_t nhdr)
{
    int format = FORMATSELECT(eh);
    size_t n;

    for(n=0; n<nhdr; rela++,n++) {
  	unsigned char 	buf[MAXRELASIZE];
	unsigned char 	*bp = buf;

	/* convert from numbers to bytes
	 */
	ELFOUTWORD(bp,rela->r_offset);
	ELFOUTWORD(bp,rela->r_info);
	ELFOUTWORD(bp,rela->r_addend);

	if(fwrite(buf,bp-buf,1,fp) != 1)
	  break;
    }
    return n;
}

size_t elfEhdrWrite(FILE *fp,const Elf32_Ehdr *eh)
{
    int format = FORMATSELECT(eh);
    unsigned char buf[MAXEHDRSIZE];
    unsigned char *bp = buf;

    memcpy(bp,eh->e_ident,EI_NIDENT);
    bp += EI_NIDENT;

    ELFOUTHALF(bp,eh->e_type);
    ELFOUTHALF(bp,eh->e_machine);
    ELFOUTWORD(bp,eh->e_version);
    ELFOUTADDR(bp,eh->e_entry);
    ELFOUTOFF(bp,eh->e_phoff);
    ELFOUTOFF(bp,eh->e_shoff);
    ELFOUTWORD(bp,eh->e_flags);
    ELFOUTHALF(bp,eh->e_ehsize);
    ELFOUTHALF(bp,eh->e_phentsize);
    ELFOUTHALF(bp,eh->e_phnum);
    ELFOUTHALF(bp,eh->e_shentsize);
    ELFOUTHALF(bp,eh->e_shnum);
    ELFOUTHALF(bp,eh->e_shstrndx);

    return fwrite(&buf,bp-buf,1,fp);
}

size_t elfEhdrRead(FILE *fp,Elf32_Ehdr *eh)
{
    unsigned char buf[MAXEHDRSIZE];
    unsigned char *bp = buf;

    if(fread(buf,ELFEHDRSZ,1,fp) != 1)
      return 0;

    memcpy(eh->e_ident,bp,EI_NIDENT);
    bp += EI_NIDENT;

    if(eh->e_ident[0] != ELFMAG0 ||
       eh->e_ident[1] != ELFMAG1 ||
       eh->e_ident[2] != ELFMAG2 ||
       eh->e_ident[3] != ELFMAG3)
      return 0;

    if(eh->e_ident[4] != ELFCLASS32)
      return 0;

    if(eh->e_ident[5] != ELFDATA2MSB && eh->e_ident[5] != ELFDATA2LSB)
      return 0;

#ifdef notdef
    /* what should the version in the ident be ?? */
    if(eh->e_ident[6] != 1)
      return 0;
#endif

    {
	int i;

	for(i=7;i<EI_NIDENT;i++)
	  if(eh->e_ident[i] != 0)
	    return 0;
    }
    {
	int format = FORMATSELECT(eh);

	ELFINHALF(bp,eh->e_type);
	ELFINHALF(bp,eh->e_machine);
	ELFINWORD(bp,eh->e_version);
	ELFINADDR(bp,eh->e_entry);
	ELFINOFF(bp,eh->e_phoff);
	ELFINOFF(bp,eh->e_shoff);
	ELFINWORD(bp,eh->e_flags);
	ELFINHALF(bp,eh->e_ehsize);
	ELFINHALF(bp,eh->e_phentsize);
	ELFINHALF(bp,eh->e_phnum);
	ELFINHALF(bp,eh->e_shentsize);
	ELFINHALF(bp,eh->e_shnum);
	ELFINHALF(bp,eh->e_shstrndx);
    }
    return 1;
}


size_t elfReginfoWrite(FILE *fp,
		       const Elf32_Ehdr *eh,
		       const Elf32_Shdr *sh,
		       const Elf32_Reginfo *ri,size_t nhdr)
{
    int format = FORMATSELECT(eh);
    size_t n;

    for(n=0; n<nhdr; ri++,n++) {
  	unsigned char 	buf[MAXREGINFOSIZE];
	unsigned char 	*bp = buf;
	int i;

	/* convert from numbers to bytes
	 */
	ELFOUTWORD(bp,ri->ri_gprmask);
	for (i = 0; i < 4; i++) {
	    ELFOUTWORD(bp,ri->ri_cprmask[i]);
	}
	ELFOUTWORD(bp,ri->ri_gp_value);

	if(fwrite(buf,bp-buf,1,fp) != 1)
	  break;
    }
    return n;
}


size_t elfReginfoRead (FILE *fp,
		       const Elf32_Ehdr *eh,
		       const Elf32_Shdr *sh,
		       Elf32_Reginfo *ri,size_t nhdr)
{
    int format = FORMATSELECT(eh);
    size_t n;

    for(n=0; n<nhdr; ri++,n++) {
  	unsigned char 	buf[MAXREGINFOSIZE];
	unsigned char 	*bp = buf;
	int i;

	if(fread(buf,sh->sh_entsize,1,fp) != 1)
	  break;

	/* convert from numbers to bytes
	 */
	ELFINWORD(bp,ri->ri_gprmask);
	for (i = 0; i < 4; i++) {
	    ELFINWORD(bp,ri->ri_cprmask[i]);
	}
	ELFINWORD(bp,ri->ri_gp_value);
    }
    return n;
}

size_t elfGptabWrite(FILE *fp,
		       const Elf32_Ehdr *eh,
		       const Elf32_Shdr *sh,
		       const Elf32_Gptab *gt,size_t nhdr)
{
    int format = FORMATSELECT(eh);
    size_t n;

    for(n=0; n<nhdr; gt++,n++) {
  	unsigned char 	buf[MAXGPTABSIZE];
	unsigned char 	*bp = buf;

	/* convert from numbers to bytes
	 */
	ELFOUTWORD(bp,gt->gt_entry.gt_g_value);
	ELFOUTWORD(bp,gt->gt_entry.gt_bytes);

	if(fwrite(buf,bp-buf,1,fp) != 1)
	  break;
    }
    return n;
}


size_t elfGptabRead(FILE *fp,
		    const Elf32_Ehdr *eh,
		    const Elf32_Shdr *sh,
		    Elf32_Gptab *gt,size_t nhdr)
{
    int format = FORMATSELECT(eh);
    size_t n;

    for(n=0; n<nhdr; gt++,n++) {
  	unsigned char 	buf[MAXGPTABSIZE];
	unsigned char 	*bp = buf;

	if(fread(buf,sh->sh_entsize,1,fp) != 1)
	  break;

	/* convert from numbers to bytes
	 */
	ELFINWORD(bp,gt->gt_entry.gt_g_value);
	ELFINWORD(bp,gt->gt_entry.gt_bytes);
    }
    return n;
}
