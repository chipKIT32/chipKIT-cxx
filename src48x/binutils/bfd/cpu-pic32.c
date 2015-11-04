/*
 * cpu-pic32.c -- BFD library support routines for the PIC30 architecture.
 * Copyright (C) 2000 Free Software Foundation, Inc.
 * Contributed by Microchip Corporation.
 * Written by Tracy A. Kuhrt
 *
 * This file is part of BFD, the Binary File Descriptor library.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "bfd.h"
#include "sysdep.h"
#include "pic32-utils.h"
#include "libiberty.h"

#include "../../c30_resource/src/xc32/resource_info.h"

#ifndef DIR_SEPARATOR
#define DIR_SEPARATOR '/'
#endif

#if defined (_WIN32) || defined (__MSDOS__) || defined (__DJGPP__) || \
  defined (__OS2__)
#ifndef DIR_SEPARATOR_2
#define HAVE_DOS_BASED_FILE_SYSTEM
#define DIR_SEPARATOR_2 '\\\\'
#define EXE_SUFFIX ".exe"
#define PATH_SEPARATOR ';'
#define DIR_SEPARATOR_STR "\\"
#endif
#endif

/* Define IS_DIR_SEPARATOR.  */
#ifndef IS_DIR_SEPARATOR
#ifndef DIR_SEPARATOR_2
# define IS_DIR_SEPARATOR(ch) ((ch) == DIR_SEPARATOR)
#else /* DIR_SEPARATOR_2 */
# define IS_DIR_SEPARATOR(ch) \
        (((ch) == DIR_SEPARATOR) || ((ch) == DIR_SEPARATOR_2))
#endif /* DIR_SEPARATOR_2 */
#endif

#ifndef DIR_SEPARATOR_2 
#define PATH_SEPARATOR ':'
#define DIR_SEPARATOR_2 DIR_SEPARATOR
#define EXE_SUFFIX ""
#define DIR_SEPARATOR_STR "/"
#endif

/******************************************************************************/

/* prototypes */


extern char *program_name;

#define EOL ,
#define ARCH(machine, printable_name, default_machine, next, flags) \
{ 32,                     \
  32,                     \
  8,                      \
  bfd_arch_mips,         \
  machine,                \
  "mips",                \
  printable_name,         \
  1,                      \
  default_machine,        \
  bfd_default_compatible,  \
  bfd_default_scan,       \
  next }

const bfd_arch_info_type * pic32_get_machine (const char * const name);
int pic32_proc_family(const bfd_arch_info_type *);
void pic32_update_resource(const char *resource);
int pic32_is_l1cache_machine(const bfd_arch_info_type *);
int pic32_is_fltpt_machine(const bfd_arch_info_type *);
void pic32_processor_option(char *);

void process_resource_file(unsigned int, int);

const bfd_arch_info_type * global_PROCESSOR = NULL;
const char *device_name = 0;

static char *pic32_resource_file = 0;

struct pic32_resource_info {
  unsigned int flags;
  bfd_arch_info_type *arch_info;
  struct pic32_resource_info *next;
};


/* This is the "last" of the generic devices, and must not
 * be declared const, because the table of devices read
 * from the resource file will be appended to it. */

bfd_arch_info_type bfd_pic32_arch =
   ARCH ( 0, "32MXGENERIC", TRUE, 0, P32MX | HAS_MIPS32R2 | HAS_MIPS16);

static struct pic32_resource_info arch_flags_head[] = {
   { P32MX | HAS_MIPS32R2 | HAS_MIPS16, (bfd_arch_info_type *) &bfd_pic32_arch, 0}
};

static int pic32_tool_version;
char *pic32_resource_version;

static char *version_part1;
static char *version_part2;

#define QUOTE2(X) #X
#define QUOTE(X) QUOTE2(X)

int
pic32_is_generic_machine(unsigned int machine) {
  if (machine == 0) return 1;
  return 0;
}


/* stupid prototype */ static void get_resource_path(const char *resource);
static void get_resource_path(const char *resource) {

  char *tool_name = (char *) resource;
  char *c;

  /*
  ** In some command shells, no path information
  ** is provided as part of argv[0].
  */
  if (!IS_DIR_SEPARATOR(resource[0]))
  {
    /*
    ** No DOS path information for the program:
    ** search for the executable using the
    ** PATH environment variable.
    */
    struct stat fs;
    char *separator;
    const char *PATH;
    char *dosPATH = 0;
    int tool_len;
    int len;
    char *command;

    tool_name = xmalloc(strlen(resource) + 4 + 40);
    sprintf(tool_name, "%s", resource);
    tool_len = strlen(tool_name);

#if defined(HAVE_DOS_BASED_FILE_SYSTEM)
    /*
    ** Append ".exe" to the executable name, if necessary
    */
    if ((tolower(tool_name[tool_len-1]) != 'e') ||
        (tolower(tool_name[tool_len-2]) != 'x') ||
        (tolower(tool_name[tool_len-3]) != 'e') ||
        (       (tool_name[tool_len-4]) != '.'))
    {
      strcat(tool_name, ".exe");
      tool_len += 4;
    }
#endif
    /*
    ** Get the current PATH
    */
    PATH = getenv("PATH");
    if (PATH) {
      /*
       *  stupid gnu will print (null) to the string if PATH==0
       */
      dosPATH = xcalloc(1, strlen(PATH) + 3);
      sprintf(dosPATH,"%s%c.",PATH, PATH_SEPARATOR);
      PATH = dosPATH;
    } else {
      PATH=".";
    }

    len = 256;
    command = xmalloc(len);
    do
    { int path_len;

      separator = strchr(PATH, PATH_SEPARATOR);
      path_len = separator ? (unsigned)(separator-PATH) : strlen(PATH);
      if ((path_len + tool_len + 2)  > len) {
        len = path_len + tool_len + 80;
        free(command);
        command = xmalloc(len);
      }
      command[0] = 0;
      strncpy(command, PATH, path_len);
      command[path_len] = 0;
      if (!IS_DIR_SEPARATOR(command[path_len-1])) {
        strcat(command, DIR_SEPARATOR_STR);
      }
      strcat(command, tool_name);

      if (stat(command, &fs) == 0) break;
      PATH = 0;
    } while (separator && (PATH = separator+1));
    if (dosPATH) free(dosPATH);
    if (PATH) tool_name = command;
  }
  pic32_resource_file = xmalloc(strlen(tool_name) +
                                sizeof("/../xc32_device.info") + 1);
  sprintf(pic32_resource_file,"%s", tool_name);
  for (c = pic32_resource_file + strlen(pic32_resource_file);
       !IS_DIR_SEPARATOR(*c); c--);
  *c = 0;
  strcat(pic32_resource_file,"/../xc32_device.info");/* Keep this to be able 
                                                     to use same
                                                     read_device_rib as XC16 */ 
}

void pic32_update_resource(const char *resource) {
  /* The tool_version is defined in the string MCHP_VERSION */
  char *Microchip;
#ifndef MCHP_VERSION
  char *new_version = xstrdup("undefined1_21");
#warning MCHP_VERSION not defined
#else
  char *new_version = xstrdup(QUOTE(MCHP_VERSION));
#endif
  int major = 0, minor = 0;

  Microchip = new_version;
  if (Microchip) {
    for (; (*Microchip) && ((*Microchip < '0') || (*Microchip > '9')); 
         Microchip++);
    if (*Microchip) {
      major = strtol(Microchip, &Microchip, 0);
      if ((*Microchip) && ((*Microchip == '_') || (*Microchip == '.'))) {
         Microchip++;
         minor = strtol(Microchip, &Microchip, 0);
         for (; *Microchip && *Microchip != ' '; Microchip++);
      }
      pic32_tool_version = major *100 + minor;
      version_part1 = new_version;
      *Microchip = 0;
      version_part2 = Microchip+1;
    }
  }
  get_resource_path(resource);
  process_resource_file(0, 0);
}

static struct pic32_resource_info *selected_processor;

void process_resource_file(unsigned int procID, int debug) {
  struct resource_introduction_block *rib;
  struct resource_data d;
  int version;

  if (pic32_resource_file == 0) {
    fprintf(stderr,"Provide a resource file");
    return;
  }
  rib = read_device_rib(pic32_resource_file,
                        (device_name) ? device_name : 0);
  if (rib == 0) {
    fprintf(stderr,"Could not open resource file: %s\n", pic32_resource_file);
    return;
  }

  if (strcmp(rib->tool_chain,"XC32")) {
    fprintf(stderr,"Invalid resource resource file\n");
    close_rib();
    return;
  } 

  if (!pic32_resource_version && version_part1 && version_part2) {
    pic32_resource_version = xmalloc(strlen(version_part1) + 
                                     strlen(version_part2) + 40);
    version = rib->version.major * 100 + rib->version.minor;
    sprintf(pic32_resource_version,"Part Supprt version (%c) ",
              rib->resource_version_increment);
  }
  if (device_name) {
    bfd_arch_info_type *last_generic = arch_flags_head[sizeof(arch_flags_head)/sizeof(struct pic32_resource_info)-1].arch_info;
    struct pic32_resource_info *last_arch_flags = &arch_flags_head[sizeof(arch_flags_head)/sizeof(struct pic32_resource_info)-1];
    int record = 0;

   record = move_to_record(record);
      bfd_arch_info_type *next;
      struct pic32_resource_info *next_flags; 
      struct resource_data d2;
      struct resource_data d3;
      struct resource_data d4;

      read_value(rik_string, &d);
      read_value(rik_int, &d2);
        next = xmalloc(sizeof(bfd_arch_info_type));
        memcpy(next, last_generic, sizeof(bfd_arch_info_type));
        last_generic->next = next;
        last_generic = next;
        next_flags = xmalloc(sizeof(struct pic32_resource_info));
        next->printable_name = d.v.s;
        next_flags->flags = d2.v.i;
        read_value(rik_int, &d3);
        read_value(rik_int, &d4);
        next->mach = d4.v.i;
        next_flags->arch_info = next;
        last_arch_flags->next = next_flags;
        last_arch_flags = next_flags;
    last_arch_flags->next = 0;
    global_PROCESSOR = pic32_get_machine(device_name);
     if (global_PROCESSOR == 0) {
       fprintf(stderr,"Error: Unknown Processor %s\n", device_name);
       exit(1);
     }
  }
  close_rib();

}


/*
** Lookup a machine description
**
*/
const bfd_arch_info_type *
pic32_get_machine (name)
   const char * const name;
{

   const bfd_arch_info_type * p;
   struct pic32_resource_info *f;

   f = arch_flags_head[0].next;
   
   for (p = bfd_pic32_arch.next; p != NULL; f = f->next, p = p->next)
   {
      if (strcasecmp (name, p->printable_name) == 0)
         break;
   } /* for p */
   if (p) {
     if (f->arch_info != p) {
       fprintf(stderr,"Architecture description mis-match!\n");
     } else selected_processor = f;
   }
   return p;
}


/*
** Lookup a processor family
*/
int
pic32_proc_family(const bfd_arch_info_type *proc)
{
  int rc = -1;
  struct pic32_resource_info *f;

  for (f = arch_flags_head[0].next; f != NULL; f = f->next)
    if (proc == f->arch_info) {
      rc = f->flags & FAMILY_MASK;
    }

  return rc;
}

/*
** Query machine for CACHEL1 support
*/
int
pic32_is_l1cache_machine(const bfd_arch_info_type *proc)
{
  int rc = 0;
  struct pic32_resource_info *f;

  if (proc == NULL)
    return rc;

  for (f = arch_flags_head[0].next; f != NULL; f = f->next)
    if (proc == f->arch_info) {
      rc = 0;
      if (f->flags & HAS_L1CACHE) rc = 1;
    }

  return rc;
}

/*
** Query machine for Hardware Floating-Point support
*/
int
pic32_is_fltpt_machine(const bfd_arch_info_type *proc)
{
  int rc = 0;
  struct pic32_resource_info *f;

  if (proc == NULL)
    return rc;

  for (f = arch_flags_head[0].next; f != NULL; f = f->next)
    if (proc == f->arch_info) {
      rc = 0;
      if (f->flags & HAS_FLTPT) rc = 1;
    }

  return rc;
}

bfd_boolean pic32_is_known_proc(char *opt_arg)
{
  char *c;
  static struct stat fileinfo;
  char * place;
  char *device_file;

  device_file = xmalloc(strlen(optarg) + strlen(program_name)
                        + strlen("/../device_files/")+ strlen(".info") + 1);
  sprintf(device_file, "%s", program_name);
  for (c = device_file + strlen(device_file);
     !IS_DIR_SEPARATOR(*c); c--);
  *c = 0;
  strcat(device_file,"/../device_files/");

  for (place = opt_arg; *place; place++)
     *place = TOUPPER(*place);

   strcat(device_file,opt_arg);
   strcat(device_file,".info");
   if (stat(device_file, &fileinfo) == 0) {
     free(device_file);
     return TRUE;
   }
  free(device_file);
  return FALSE;
}

void pic32_processor_option(char *optarg)
{
   const char *proc_option_err = "unknown processor";
  const char *proc_option_warn = "multiple processor options specified\n";
  if (pic32_is_known_proc(optarg)) {
        if (pic32_has_processor_option && strcmp(device_name, optarg))
          fprintf(stderr,"Warning: %s\n", proc_option_warn);
        device_name = xmalloc(strlen(optarg) + 1);
        sprintf(device_name, "%s", optarg);
        pic32_has_processor_option = TRUE;
        pic32_update_resource(program_name);
      }
      else {
            fprintf(stderr,"Error: %s %s\n", proc_option_err, optarg);
            exit(1);
      }
   
}
