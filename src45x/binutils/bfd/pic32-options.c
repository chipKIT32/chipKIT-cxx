/*
** pic32-options.c
**
** This file is part of BFD, the Binary File Descriptor library.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
** USA.
**
** This file contains pic32-specifc options
** processing for the linker.
**
** It is included by the following target-specific
** emulation files:
**
**   ld/emultmpl/pic32_elf32.em
**
*/

#include <errno.h>

/*
** list_options()
**
** This function is called by the --help option.
*/
static void
gldelf32pic32mx_list_options (FILE * file)
{
  fprintf (file, _("  -D,--debug           Produce linker debugging messages\n"));
  fprintf (file, _("  --report-mem         Report memory usage to console\n"));
  fprintf (file, _("  --smart-io           Merge I/O library functions (default)\n"));
  fprintf (file, _("  --no-smart-io        Don't merge I/O library functions\n"));
} /* static void elf32pic32mx_list_options () */

/*
** parse_args()
**
** This function is called for each command-line option.
*/
static int
gldelf32pic32mx_parse_args (int argc, char ** argv)
{
  int        longind;
  int        optc;
  int        prevoptind = optind;
  int        prevopterr = opterr;
  int        wanterror;
  static int lastoptind = -1;

  const char *smart_io_option_err  = "--smart-io and --no-smart-io";
  const char *option_err = " options can not be used together\n";

  if (lastoptind != optind)
    opterr = 0;

  wanterror  = opterr;
  lastoptind = optind;
  optc   = getopt_long_only (argc, argv, shortopts, longopts, & longind);
  opterr = prevopterr;
  switch (optc)
    {
    default:
      if (wanterror)
        xexit (1);
      optind =  prevoptind;
      return 0;
    case 'D':
      pic32_debug = TRUE;
      break;
    case SMART_IO_OPTION:
      if (pic32_has_smart_io_option && (!pic32_smart_io))
        einfo(_("%P%F: Error: %s%s"), smart_io_option_err, option_err);
      pic32_smart_io = TRUE;
      pic32_has_smart_io_option = TRUE;
      break;
    case NO_SMART_IO_OPTION:
      if (pic32_has_smart_io_option && (pic32_smart_io))
        einfo(_("%P%F: Error: %s%s"), smart_io_option_err, option_err);
      pic32_smart_io = FALSE;
      pic32_has_smart_io_option = TRUE;
      break;
    case REPORT_MEM_OPTION:
      pic32_report_mem = TRUE;
      break;
    }

  return 1;
} /* static int gldelf32pic32mx_parse_args ()*/

