/* convert.c -- ELF to hex (etc) file converter

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
#include "conv-version.h"

#include <sys/stat.h>

#include <getopt.h>

struct format {
    const char	*name;
    int		binary;
    void	(*start) (FILE *, unsigned long, int);
    void	(*output) (unsigned long, unsigned char);
    void	(*sym) (char *, unsigned long);
    void	(*end) (unsigned long);
    const char	*desc;
};

extern void Srec3Start (FILE *, unsigned long, int);
extern void Srec2Start (FILE *, unsigned long, int);
extern void Srec1Start (FILE *, unsigned long, int);
extern void SrecOutput (unsigned long, unsigned char);
extern void SrecSym (char *, unsigned long);
extern void SrecEnd (unsigned long);

extern void LsiStart (FILE *, unsigned long, int);
extern void LsiOutput (unsigned long, unsigned char);
extern void LsiSym (char *, unsigned long);
extern void LsiEnd (unsigned long);

extern void StagStart (FILE *, unsigned long, int);
extern void StagOutput (unsigned long, unsigned char);
extern void StagEnd (unsigned long);

extern void BinStart (FILE *, unsigned long, int);
extern void BinOutput (unsigned long, unsigned char);
extern void BinEnd (unsigned long);

extern void IdtStart (FILE *, unsigned long, int);
extern void IdtOutput (unsigned long, unsigned char);
extern void IdtEnd (unsigned long);

extern void ElfStart (FILE *, unsigned long, int);
extern void ElfOutput (unsigned long, unsigned char);
extern void ElfEnd (unsigned long);

extern void MipsStart (FILE *, unsigned long, int);
extern void MipsOutput (unsigned long, unsigned char);
extern void MipsEnd (unsigned long);

static const struct format formats[] = {
    {"srec",	0,	Srec3Start, SrecOutput, SrecSym, SrecEnd,
     "Motorola S-record (32-bit address)"},
    {"s3",	0,	Srec3Start, SrecOutput, SrecSym, SrecEnd,
     "Motorola S-record (32-bit address)"},
    {"s2",	0,	Srec2Start, SrecOutput, SrecSym, SrecEnd,
     "Motorola S-record (24-bit address)"},
    {"s1",	0,	Srec1Start, SrecOutput, SrecSym, SrecEnd,
     "Motorola S-record (16-bit address)"},
    {"lsi",	0,	LsiStart, LsiOutput, LsiSym, LsiEnd,
     "LSI fast download (PMON)"},
    {"fl",	0,	MipsStart, MipsOutput, 0, MipsEnd,
     "MIPS flash download"},
    {"idt",	1,	IdtStart, IdtOutput, 0, IdtEnd,
     "IDT/sim binary S-record"},
    {"sbin",	1,	StagStart, StagOutput, 0, StagEnd,
     "Stag PROM programmer"},
    {"bin",	1,	BinStart, BinOutput, 0, BinEnd,
     "Raw binary"},
    {"relf",	1,	ElfStart, ElfOutput, 0, ElfEnd,
     "Relocated ELF"},
    {0}
};


char *		progname;
int		promflag = 0;
static int 	symflag = 0;
static int 	listflag = 0;
static int	verbose = 0;

#define MAXALIGN	16	/* align segments to 16 byte boundaries */

#define MAXSEGS		20
static unsigned	segomit[MAXSEGS];
static unsigned	omitseg[MAXSEGS];
static unsigned	nomit;

#define MAXWIDTH	8
static unsigned	bytelist[MAXWIDTH];
static unsigned	nbytes;
static unsigned	width = 4;		/* in bytes */

typedef unsigned long msize_t;
       msize_t	prombase  = 0x1fc00000;
static msize_t	promlimit = 0;
static msize_t	promstart;
static msize_t	promend;

static void
usage (int error)
{
    const struct format *fmt;
    FILE* to = error ? stderr : stdout;

    fprintf(to,
"usage: %s [-lvypr] [-f fmt] [-w width] [-b byte,...] [-i offs] [-s size]\n"
"                [-R offs] [-x seg,...] [-a addr] [-o file] [file]\n"
"	-l		list segments only, no conversion\n"
"	-v		verbose conversion option\n"
"	-y		include symbols\n"
"	-p		prom mode (start addresses at zero)\n"
"	-a addr[:lim]	address of prom (default: 1fc00000:20000000)\n"
"	-a addr[/size]	address of prom (default: 1fc00000/400000)\n"
"	-r		raw binary input (not ELF)\n"
"	-R offs		raw binary, start addresses at offs (default: 0)\n"
"	-EB | -EL	set big or little endianness\n"
"	-f fmt		output format (default: srec)\n"
"	-w width	data width in bits (default: 32)\n"
"	-b byte,...	select bytes (default: 0,1,2,3)\n"
"	-i offs		initial input offset (default: 0)\n"
"	-s size		prom size (default: infinite)\n"
"	-x seg,...	exclude segments (default: none)\n"
	    , progname);

    fprintf (to, "\nOutput formats (-f):\n");
    for (fmt = formats; fmt->name; fmt++)
      fprintf (to, "  %s\t%s\n", fmt->name, fmt->desc ? fmt->desc : "");

    fprintf (to, "\nReport bugs to " CONVERT_REPORT_BUGS_TO ".\n");

    exit(error);
}


static int
parselist (char *arg, unsigned *list, int lsize)
{
    char *s, *ep;
    int n = 0;

    for (s = strtok (arg, ","); s; s = strtok (0, ",")) {
	if (n >= lsize) {
	    fprintf (stderr, "%s: too many items in list\n", progname);
	    usage (1);
	}
	list[n++] = strtol (s, &ep, 0);
	if (*ep) {
	    fprintf (stderr, "%s: bad number in list: %s\n", progname, s);
	    usage (1);
	}
    }
    return n;
}

static char *
phtype (Elf32_Phdr *ph)
{
    static char buf[11];

    switch (ph->p_type) {
    case PT_NULL:
 	return "NULL    ";
    case PT_LOAD:
 	return "LOAD    ";
    case PT_DYNAMIC:
	return "DYNAMIC ";
    case PT_INTERP:
 	return "INTERP  ";
    case PT_NOTE:
	return "NOTE    ";
    case PT_SHLIB:
	return "SHLIB   ";
    case PT_PHDR:
	return "PHDR    ";
    case PT_MIPS_REGINFO:
	return "REGINFO ";
    default:
	sprintf (buf, "%08x", ph->p_type);
	return buf;
   }
}


static char *
phflags (Elf32_Phdr *ph)
{
    static char buf[4];
    buf[0] = (ph->p_flags & PF_R) ? 'r' : '-';
    buf[1] = (ph->p_flags & PF_W) ? 'w' : '-';
    buf[2] = (ph->p_flags & PF_X) ? 'x' : '-';
    buf[3] = '\0';
    return buf;
}


static void
convertSegment (FILE *ifp, const struct format *fmt, Elf32_Phdr *ph)
{
    unsigned char buf[MAXWIDTH];
    static msize_t req_top = 0, offs_top = 0;
    static unsigned int last_flags = 0;
    msize_t req, offset;
    unsigned long skip = 0;
    long n;

    /* get requested physical address */
    req = (ph->p_paddr ? ph->p_paddr : ph->p_vaddr);
    if (promflag) {
	offset = req & 0x1fffffff;
	if (offset < prombase || offset >= promlimit) {
	    /* not a valid prom address: force to rom */
	    if (offs_top == 0) {
		/* force to start, whatever it says in the file */
		offset = prombase;
	    }
	    else {
		unsigned long align = ph->p_align ? ph->p_align - 1 : 0;
		if (req == ((req_top + align) & ~align)
		    && ((ph->p_flags ^ last_flags) & PF_W) == 0) {
		  /* segment is contiguous and of same read/write type,
		     then keep its relative offset the same */
		  offset = offs_top + (req - req_top);
		}
		else {
		  /* segment is not contiguous, or a different type,
		     put at next MAXALIGN boundary. */
		  align = MAXALIGN - 1;
		  offset = (offs_top + align) & ~align;
		}
	    }
	}
    }
    else {
	/* not a prom: preserve load address */
	offset = req;
    }

    req_top = req + ph->p_memsz;
    offs_top = offset + ph->p_memsz;
    last_flags = ph->p_flags;

    if (promflag) {
	/* convert to offset from start of PROM */
	offset -= prombase;
	/* skip as many bytes as necessary to reach requested start */
	if (offset < promstart) {
	    skip = (promstart - offset) * (width / nbytes);
	    if (skip >= ph->p_filesz)
		skip = ph->p_filesz;
	    offset += ((skip + width - 1) / width) * nbytes;
	}
    }

    if (skip < ph->p_filesz) {
	fseek (ifp, ph->p_offset + skip, SEEK_SET);
	n = ph->p_filesz - skip;

	/* convert to offset and size in prom */
	offset /= (width / nbytes);
	n = (n + (width / nbytes) - 1) / (width / nbytes);

	if (verbose) {
	  int i, j;
	  fprintf (stderr, "%s segment %08x:%08x -> %s %05x:%05x [",
		  phflags (ph), req, req + n,
		  fmt->name, offset, offset + n);
	  for (i = 0; i < width; i++) {
	    for (j = 0; j < nbytes; j++)
	      if (bytelist[j] == i)
		break;
	    if (j < nbytes)
	      fprintf (stderr, "%x", j);
	    else
	      fprintf (stderr, "-");
	  }
	  fprintf (stderr, "]\n");
	}

	while (n > 0 && offset < promend) {
	    int nr, i;
	    if ((nr = fread (buf, 1, width, ifp)) <= 0) {
		fprintf (stderr, "%s: bad file read\n", progname);
		exit (1);
	    }
	    for (i = 0; i < nbytes; i++) {
		int bn = bytelist[i];
		if (bn < nr)
		  (*fmt->output) (offset++, buf[bn]);
	    }
	    n -= nbytes;
	}
    }
}

static void
convertSymbols (FILE *ifp, Elf32_Ehdr *eh, const struct format *fmt)
{
    Elf32_Shdr *shtab, *sh;
    char *strtab;
    int i;

    shtab = (Elf32_Shdr *) xmalloc (eh->e_shnum * sizeof(Elf32_Shdr));

    fseek (ifp, eh->e_shoff, SEEK_SET);
    if (elfShdrRead (ifp, eh, shtab, eh->e_shnum) != eh->e_shnum) {
	fprintf (stderr,"%s: failed to read section headers\n", progname);
	exit (1);
    }

    for (i = 0; i < eh->e_shnum; i++)
      if (shtab[i].sh_type == SHT_SYMTAB)
	break;
    if (i >= eh->e_shnum) {
	free (shtab);
	return;
    }

    /* get string table */
    sh = &shtab[shtab[i].sh_link];
    strtab = (char *) xmalloc (sh->sh_size);
    fseek (ifp, sh->sh_offset, SEEK_SET);
    if (fread (strtab, 1, sh->sh_size, ifp) != sh->sh_size) {
	fprintf (stderr, "%s: cannot read string table\n", progname);
	exit (1);
    }

    /* scan symbol table */
    sh = &shtab[i];
    fseek (ifp, sh->sh_offset, SEEK_SET);
    for (i = sh->sh_size / sh->sh_entsize; i != 0; i--) {
	Elf32_Sym sym;
	int type;

	if (elfSymRead (ifp, eh, sh, &sym, 1) != 1) {
	    fprintf (stderr, "%s: cannot read symbol table\n", progname);
	    exit (1);
	}
	switch (sym.st_shndx) {
	case SHN_UNDEF:
	case SHN_COMMON:
	    continue;
	}

	type = ELF32_ST_TYPE (sym.st_info);
	switch (type) {
	case STT_SECTION:
	case STT_FILE:
	    continue;
	}

	/* only emit globals and functions */
	if (ELF32_ST_BIND (sym.st_info) != STB_GLOBAL && type != STT_FUNC)
	  continue;

	if (sym.st_other == 0xf0)
	    sym.st_value |= 1;

	(*fmt->sym) (strtab + sym.st_name, sym.st_value);
    }

    free (strtab);
    free (shtab);
}


/* comparison function for qsort() */
int
phcompar (const void *a, const void *b)
{
  const Elf32_Phdr *pha = a;
  const Elf32_Phdr *phb = b;
  unsigned long addra = pha->p_paddr ? pha->p_paddr : pha->p_vaddr;
  unsigned long addrb = phb->p_paddr ? phb->p_paddr : phb->p_vaddr;
  return (addra == addrb) ? 0
    : (addra < addrb) ? -1
    : 1;
}



#ifndef HAVE_STRTOUL
extern unsigned long int strtoul (const char *, char **, int);
#endif

static const struct option long_opts[] =
{
    { "version", no_argument, NULL, 'V' },
    { "help", no_argument, NULL, 'h' },
    { 0, 0, NULL, 0 }
};

int
main(int argc, char **argv)
{
    extern char *getenv ();
    FILE *ifp, *ofp, *cfp;
    Elf32_Ehdr ehdr;
    Elf32_Phdr *phtab, *ph;
    const struct format *fmt = 0;
    char *infile = "a.out";
    char *outfile = 0;
    char *fmtname;
    char *s;
    extern char *optarg;
    extern int optind;
    msize_t size = 0;
    int rawflag = 0;
    msize_t rawoffs = 0;
    int i, c;
    int bigendian = 0;

    progname = argv[0];
    if ((s = strrchr(progname, '/')) || (s = strrchr(progname, '\\')) ||
	(s = strrchr(progname, ':')))
      progname = s + 1;

    fmtname = getenv ("DLTYPE");
    while ((c = getopt_long(argc,argv,"prR:vylf:x:b:s:i:w:o:t:a:E:",
			    &long_opts[0], NULL)) != EOF)
      switch(c) {
      case 'p':
	  promflag = 1;
	  break;
      case 'r':
	  rawflag = 1;
	  break;
      case 'R':
	  rawflag = 1;
	  rawoffs = strtoul (optarg, &s, 16);
	  if (*s)
	    usage (1);
	  break;
      case 'v':
	  verbose = 1;
	  break;
      case 'y':
	  symflag = 1;
	  break;
      case 'l':
	  listflag = 1;
	  break;
      case 'f':
	  fmtname = optarg;
	  break;
      case 'b':
	  nbytes = parselist (optarg, bytelist, MAXWIDTH);
	  break;
      case 'x':
	  nomit = parselist (optarg, segomit, MAXSEGS);
	  break;
      case 'i':
	  promstart = strtoul (optarg, &s, 0);
	  if (*s)
	    usage (1);
	  break;
      case 's':
	  size = strtoul (optarg, &s, 0);
	  if (*s)
	    usage (1);
	  break;
      case 'a':
	  promflag = 1;
	  prombase = strtoul (optarg, &s, 16) & 0x1fffffff;
	  if (*s == '/' || *s == '#') {
	    promlimit = strtoul (++s, &s, 16) + prombase;
	  }
	  else if (*s == ':' || *s == '-') {
	    promlimit = strtoul (++s, &s, 16) & 0x1fffffff;
	    if (promlimit && (promlimit <= prombase)) {
		fprintf (stderr, "%s: ROM limit below ROM base\n", progname);
		usage (1);
	    }
	  }
	  if (*s)
	    usage (1);
	  break;
      case 'w':
	  width = strtoul (optarg, &s, 0);
	  if (*s || width % 8 != 0)
	    usage (1);
	  width /= 8;
	  break;
      case 'o':
	  outfile = optarg;
	  break;
      case 't':
	  fprintf (stderr, "%s: -t flag deprecated and ignored\n", progname);
	  break;
      case 'E':
	  if (optarg[0] == 'B' && optarg[1] == '\0')
	      bigendian = 1;
	  else if (optarg[0] == 'L' && optarg[1] == '\0')
	      bigendian = 0;
	  else {
	      fprintf (stderr, "%s: -E%s flag not recognised\n", progname,
		       optarg);
	      usage (1);
	  }
	  break;
      case 'h':
	  usage (0);
	  break;
      case 'V':
          printf ("MIPS convert " CONVERT_PKGVERSION "\n");
	  return 0;
      default:
	  usage (1);
      }

    if(optind < argc) {
	infile = argv[optind++];
	if (optind < argc)
	  usage (1);
    }

    if ((size || promstart) && !promflag) {
	fprintf (stderr, "%s: -s or -i specified without -p\n", progname);
	usage (1);
    }

    if (width == 0 || width > MAXWIDTH) {
	fprintf (stderr, "%s: bad width\n", progname);
	usage (1);
    }

    if (nbytes == 0) {
	for (i = 0; i < width; i++)
	  bytelist[i] = i;
	nbytes = width;
    } else {
	if (nbytes != (nbytes & -nbytes)) {
	    fprintf (stderr, "%s: number of selected bytes not power of two\n",
		     progname);
	    usage (1);
	}
	if (nbytes > width) {
	    fprintf (stderr, "%s: number of selected bytes > data width\n",
		     progname);
	    usage (1);
	}
	for (i = 0; i < nbytes; i++)
	  if (bytelist[i] >= width) {
	      fprintf (stderr, "%s: bad byte select %d (should be 0-%d)\n",
		       progname, bytelist[i], width-1);
	      usage (1);
	  }
    }

    /* invert seg omit list */
    for (i = 0; i < nomit; i++) {
	int o = segomit[i];
	if (o >= MAXSEGS) {
	    fprintf (stderr, "%s: bad segment number: %d\n",
		     progname, o);
	    usage (1);
	}
	omitseg[o] = 1;
    }

    if (fmtname) {
	for (fmt = formats; fmt->name; fmt++)
	  if (strcmp (fmtname, fmt->name) == 0)
	    break;
	if (!fmt->name) {
	    fprintf (stderr, "%s: unknown output format '%s'\n", fmtname,
		     progname);
	    usage (1);
	}
    } else {
	fmt = formats;
    }

    if (size == 0)
      promend = ULONG_MAX;
    else
      promend = promstart + size;

    if (promlimit == 0) {
      if (size == 0)
	promlimit = prombase + 0x400000;
      else
	promlimit = prombase + size / nbytes * width;
    }

    if (!(ifp = fopen (infile, FOPEN_RB))) {
	perror(infile);
	return 1;
    }

    if (outfile && strcmp(outfile, "-")) {
	if (!(ofp = fopen (outfile, fmt->binary ? FOPEN_WB : FOPEN_WT))) {
	    perror(outfile);
	    return 1;
	}
    }
    else {
      ofp = stdout;
#ifdef MSDOS
      if (fmt->binary)
	setmode (fileno (ofp), O_BINARY);
#endif
    }

    if (!rawflag) {
      if (elfEhdrRead (ifp, &ehdr) != 1) {
	fprintf (stderr,"%s: not an ELF object file: %s\n", progname, infile);
	return 1;
      }

      if (ehdr.e_type != ET_EXEC) {
	fprintf(stderr,"%s: warning: not an executable ELF file: %s\n",
		progname, infile);
      }

      bigendian = ehdr.e_ident[5] == ELFDATA2MSB;

      if (ehdr.e_phoff == 0 || ehdr.e_phnum == 0) {
	fprintf(stderr, "%s: missing ELF program header: %s\n",
		progname, infile);
	return 1;
      }

      if (ehdr.e_phnum > MAXSEGS) {
	fprintf(stderr, "%s: too many segments in ELF program header: %s\n",
		progname, infile);
	return 1;
      }

      phtab = (Elf32_Phdr *) xmalloc (ehdr.e_phnum * sizeof(Elf32_Phdr));

      fseek (ifp, ehdr.e_phoff, SEEK_SET);
      if (elfPhdrRead (ifp, &ehdr, phtab, ehdr.e_phnum) != ehdr.e_phnum) {
	fprintf (stderr,"%s: failed to read program header\n", progname);
	return 1;
      }
    }
    else {
      /* fake up the header etc */
      struct stat stb;

      ehdr.e_phnum = 1;
      ehdr.e_entry = prombase + rawoffs;
      ph = phtab = (Elf32_Phdr *) xmalloc (sizeof(Elf32_Phdr));
      ph->p_type = PT_LOAD;
      ph->p_flags = PF_R | PF_X;
      ph->p_vaddr = ph->p_paddr = prombase + rawoffs;
      fstat (fileno (ifp), &stb);
      ph->p_filesz = ph->p_memsz = stb.st_size;
    }

    if (listflag) {
	printf ("Segment List\n");
	printf ("Num Type       Vaddr    Paddr  MemSz FSize FOffs Align Flgs\n");
	for (ph = phtab; ph < &phtab[ehdr.e_phnum]; ph++) {
	    printf (" %1d  %8s %08x %08x %05x %05x %05x %05x %s%s\n",
		    ph-phtab, phtype(ph), ph->p_vaddr, ph->p_paddr,
		    ph->p_memsz, ph->p_filesz, ph->p_offset, ph->p_align,
		    phflags(ph), omitseg[ph-phtab]?" (omitted)":"");
	}
	exit (0);
    }

    (*fmt->start) (ofp, ehdr.e_entry, bigendian);

    /* sort program header table into physical address order */
    qsort (phtab, ehdr.e_phnum, sizeof (phtab[0]), phcompar);

    if (promflag) {
	/* do prom-resident segments first */
	for (ph = phtab; ph < &phtab[ehdr.e_phnum]; ph++)
	    if (ph->p_type == PT_LOAD) {
		unsigned int offset;
		offset = (ph->p_paddr ? ph->p_paddr : ph->p_vaddr)
		    & 0x1fffffff;
		if (offset >= prombase && offset < promlimit
		    && !omitseg[ph - phtab]) {
		    convertSegment (ifp, fmt, ph);
		    ph->p_type = PT_NULL;
		}
	    }
    }

    /* then do all other segments in order */
    for (ph = phtab; ph < &phtab[ehdr.e_phnum]; ph++)
      if (ph->p_type == PT_LOAD && ph->p_filesz && !omitseg[ph - phtab])
	convertSegment (ifp, fmt, ph);

    if (symflag && !rawflag && fmt->sym)
      convertSymbols (ifp, &ehdr, fmt);

    (*fmt->end) (ehdr.e_entry);

    free (phtab);

    return 0;
}
