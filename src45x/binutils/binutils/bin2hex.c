/* bin2hex - convert an executable object file to HEX format using the
             BFD library. */

#include "sysdep.h"
#include "bfd.h"
#include "bfdver.h"
#include "getopt.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "bucomm.h"
#include "libiberty.h"

struct section_list
{
  struct section_list *next;
  PTR *file;
  asection *sec;
};

/* prototypes */
int main (int argc, char ** argv);
static void write_extended_address_record (file_ptr base_address, PTR fp);
static void write_section (bfd *abfd, asection *sect, PTR fp);
static int write_hex_file (char *name, bfd *abfd);
static void usage (FILE *stream, int status);
static void nonfatal (const char *msg);
static void hexify_file (char *filename, char *target);

/* needed for sorting */
static void init_section_list (struct section_list **);
static void build_section_list (bfd *, asection *, PTR);
static void write_section_list (bfd *, PTR, struct section_list *);
static void free_section_list (struct section_list **);

static int exit_status = 0;

/* The BFD object file format.  */
static char *default_target = NULL;

static bfd_size_type actual_prog_used;

static int verbose = 0;
static int sort_by_address = 0;
static struct section_list *outputs;
static int physical_addresses = 1;
static int a0_address_mapping = 0;
static int show_usage = 0;

static struct option long_options[] =
{
  {"sort", no_argument, NULL, 'a'},
  {"virtual", no_argument, NULL, 'i'},
  {"physical", no_argument, NULL, 'p'},
  {"a0-mapping", no_argument, NULL, '0'},
  {"std-mapping", no_argument, NULL, 's'},
  {"verbose", no_argument, NULL, 'v'},
  {"target", required_argument, NULL, 'b'},
  {"help", no_argument, NULL, '?'},
  {0, no_argument, 0, 0}
};

int
main (int argc, char **argv)
{
  int c;
  char *target = default_target;

  program_name = *argv;
  xmalloc_set_program_name (program_name);

  bfd_init ();
  set_default_bfd_target ();

  while ((c = getopt_long (argc, argv, "aip0svb?",
                           long_options, (int *) 0)) != EOF)
    switch (c)
      {
      case 'a' :
        sort_by_address = 1;
        break;
      case 'v' :
        verbose = 1;
        break;
      case 'p' :
        physical_addresses = 1;
        break;
      case 'i' :
        physical_addresses = 0;
        break;
      case '0' :
        a0_address_mapping = 1;
        break;
      case 's' :
        a0_address_mapping = 0;
        break;
      case 'b' :
        target = optarg;
        break;
      case '?' :
        show_usage = 1;
        break;
      default :
        show_usage = 1;
	break;
      }

  if (show_usage)
    usage (stdout, 0);

  if (optind == argc)
  {
    char *file = strdup ("a.out");
    hexify_file (file, target);
    free (file);
  }
  else
    for (; optind < argc;)
      hexify_file (argv[optind++], target);

  return exit_status;
}

static void
nonfatal (const char *msg)
{
  bfd_nonfatal (msg);
  exit_status = 1;
}

static void
hexify_file (char *filename, char *target)
{
  bfd *abfd;
  char *dot;
  char *hex_file;

  if (get_file_size (filename) < 1)
    return;

  abfd = bfd_openr (filename, target);
  if (abfd == NULL)
    {
      nonfatal (filename);
      return;
    }

  if (bfd_check_format (abfd, bfd_object))
    {
      if (verbose)
        printf ("\n");

      /* strip extension from filename if present */
      dot = strrchr (filename, '.');
      if (dot)
        dot[0] = '\0';

      /* create a new name with .hex extension */
      hex_file = malloc (strlen (filename) + strlen (".hex") + 1);
      sprintf (hex_file, "%s%s", filename, ".hex");

      exit_status = write_hex_file (hex_file, abfd);

      if (verbose)
        printf ("\n");

      free (hex_file);
    }

  bfd_close (abfd);
}

static void
usage (FILE *stream, int status)
{
  fprintf (stream,
           "%s %s\n\tConverts an ELF file to Intel hex format\n\n",
           program_name, BFD_VERSION_STRING);
  fprintf (stream,
           "USAGE: %s [options] [file]\n"
           "  -a, --sort         sort sections by address\n"
           "  -i, --virtual      use virtual addresses\n"
           "  -p, --physical     use physical addresses (default)\n"
#ifdef PIC32_SHOW_A0MAPPINGOPTION
           /* After A0, we default to standard address mapping */
           "  -0, --a0-mapping   use A0 address mapping\n"
           "  -s, --std-mapping  use standard address mapping (default)\n"
#endif
           "  -v, --verbose      print verbose messages\n"
           "  -?, --help         print this screen\n",
           program_name);
  exit (status);
}

static int
write_hex_file (char *name, bfd *abfd)
{
  FILE *fp;
  int rc = 0;

  fp = fopen (name, "w");
  if (!fp)
    {
      printf ("Error: could not open file %s for writing!\n", name);
      return 1;
    }
  
  if (verbose)
    {
      printf ("writing %s\n\n", name);
      printf ("section                       byte address     length    (dec)\n");
      printf ("--------------------------------------------------------------\n");
    }

  actual_prog_used = 0;

  /* write each section */
  if (sort_by_address)
    {
      init_section_list (&outputs);
      bfd_map_over_sections (abfd, &build_section_list, outputs);
      write_section_list (abfd, fp, outputs);
      free_section_list (&outputs);
    }
  else
    bfd_map_over_sections (abfd, &write_section, fp);

  /* write the end of file marker */
  fprintf (fp, ":00000001FF\n");
  fclose (fp);

  /* write summary info */
  if (verbose)
    printf ("\nTotal program memory used (bytes): %#18lx    (%ld)\n",
           actual_prog_used, actual_prog_used);

  return rc;
}

static void
init_section_list (struct section_list **lst)
{
  *lst = ((struct section_list *)
         xmalloc (sizeof (struct section_list)));
  (*lst)->next = 0;
  (*lst)->sec = 0;
  (*lst)->file = 0;
}

/*
** Build a list of output sections sorted by address.
**
** - called via bfd_map_over_sections ()
*/
void
build_section_list (bfd *abfd ATTRIBUTE_UNUSED,
                    asection *sect,
                    PTR p)
{
  struct section_list *new, *s, *prev;
  int done = 0;

  /* create a new element */
  new = ((struct section_list *) xmalloc (sizeof (struct section_list)));
  new->sec  = sect;

  prev = (struct section_list *) p;
  for (s = prev; s != NULL; s = s->next)
    {
      /* insert it at the correct spot */
      if (s->sec && (new->sec->lma < s->sec->lma))
        {
          prev->next = new;
          new->next = s;
          done++;
          break;
        }
      prev = s;
    }

  /* if still not placed, add it to the end */
  if (!done)
    {
      prev->next = new;
      new->next = NULL;
    }
}

static void
write_section_list (bfd *abfd, PTR fp, struct section_list *p)
{
  struct section_list *s;

  for (s = p; s != NULL; s = s->next)
    {
      if (s->sec)
        write_section (abfd, s->sec, fp);
    }
}


static void
free_section_list (struct section_list **lst)
{
  struct section_list *s, *next;

  if (!(*lst))
    return;

  for (s = *lst; s != NULL; s = next)
    {
      next = s->next;
      free (s);
    }
  *lst = NULL;
}

static file_ptr
decode_address (file_ptr addr)
{
  /* A0 Silicon has a bug whereby the address mapping is incorrect */
  if (a0_address_mapping)
    addr &= 0x1FFFFFFF;
  else if (addr & 0x80000000)
    {
      if (addr & 0x40000000)
        /* KSEG2/KSEG3 address - to decode do nothing */
        addr = addr;
      else
        /* KSEG0/KSEG1 address - to decode strip upper 3 bits */
        addr &= 0x1FFFFFFF;
    }
  else
    /* USEG/KUSEG address - to decode add 0x40000000 to the address */
    addr += 0x40000000;

  return addr;
}
static void
write_section (bfd *abfd, asection *sect, PTR fp)
{
  const bfd_size_type row_size=0x10;

  FILE *hexfile;
  unsigned char dat[row_size];
  unsigned char *p;
  file_ptr  start, offset;
  file_ptr  upper_addr, base_addr;  /* for Extended Linear Address Records */
  bfd_size_type total;
  unsigned int   num_rows, last_row, i, j, sum;
  unsigned short low_addr;

  base_addr = 0;
  hexfile = fp;
  p = &dat[0];
  offset = 0;
  total = (unsigned long) bfd_section_size (abfd, sect)
        / bfd_octets_per_byte (abfd);

  /* if section is load-able and has contents */
  if ((sect->flags & SEC_LOAD) &&
      (sect->flags & SEC_HAS_CONTENTS) &&
      (total > 0))
  {
    start = sect->lma;
    /* To convert to a physical address, the top 3 bits are masked off */
    if (physical_addresses)
      start = decode_address (start);

    /* print section header */
    if (verbose)
      printf ("%-25s     %#12lx     %#6lx    (%ld)\n", sect->name,
             (unsigned long) start, total, total);

    actual_prog_used += total;  /* actual program memory used */
    num_rows = total / row_size;
    last_row = total % row_size;

    /* start with extended address = 0 */
    write_extended_address_record (0, fp);

    /* write the full records */
    for (i = 0; i < num_rows; i++)
      {
        sum = 0;
        low_addr = (unsigned short) (start + offset);
        upper_addr = (start + offset) - low_addr;
        /* if address overflows */
        if (upper_addr > base_addr)
          {
            base_addr = upper_addr;
            write_extended_address_record (base_addr, fp);
          }

      bfd_get_section_contents (abfd, sect, p, offset, row_size);
      fprintf (hexfile, ":10%4.4x00", low_addr);  /* begin record */
      sum -= (0x10 + (low_addr >> 8) + (low_addr & 0xFF));
      offset += row_size;
      /* fill in data */
      for (j = 0; j < row_size; j++)
        {
          fprintf (hexfile, "%2.2x", dat[j]);
          sum -= dat[j];
        }
      fprintf (hexfile, "%2.2x\n", (unsigned char) sum);     /* add checksum */
    }

    /* write the last record */
    if (last_row)
      {
        sum = 0;
        low_addr = (unsigned short) (start + offset);
        upper_addr = (start + offset) - low_addr;
        /* if address overflows */
        if (upper_addr > base_addr)
          {
            base_addr = upper_addr;
            write_extended_address_record (base_addr, fp);
          }
        /* begin record */
        fprintf (hexfile, ":%2.2x%4.4x00", last_row, low_addr);
        sum -= (last_row + (low_addr>>8) + (low_addr&0xFF));
        bfd_get_section_contents (abfd, sect, p, offset, last_row);
        /* fill in data */
        for (j = 0; j < last_row; j++)
          {
            fprintf (hexfile, "%2.2x", dat[j]);
            sum -= dat[j];
          }
        /* add checksum */
        fprintf (hexfile, "%2.2x\n", (unsigned char) sum);
      }    
    }
}

static void
write_extended_address_record (file_ptr base_address, PTR fp)
{
  FILE *hexfile;
  unsigned int sum;
  unsigned int upper_addr;

  sum = 0;
  hexfile = fp;
  upper_addr = (unsigned long) base_address >> 16;
  sum -= (2 + 0 + 4 + (upper_addr >> 8) + (upper_addr & 0xFF));
  fprintf (hexfile, ":02000004%4.4x%2.2x\n", upper_addr, (unsigned char) sum);
}


