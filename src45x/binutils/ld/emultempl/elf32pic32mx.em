# This shell script emits a C file. -*- C -*-
#   Copyright 2006, 2007, 2008, 2009 Free Software Foundation, Inc.
#
# This file is part of the GNU Binutils.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
# MA 02110-1301, USA.

fragment <<EOF

#include "ldctor.h"
#include "elf/mips.h"
#include "elfxx-mips.h"

#include "elf/pic32.h"
#include "ctype.h"
#include "../bfd/pic32-options.h"
#include "../bfd/pic32-options.c"
#include "../bfd/pic32-utils.h"

#define is_mips_elf(bfd)				\
  (bfd_get_flavour (bfd) == bfd_target_elf_flavour	\
   && elf_tdata (bfd) != NULL				\
   && elf_object_id (bfd) == MIPS_ELF_DATA)

/* Fake input file for stubs.  */
static lang_input_statement_type *stub_file;
static bfd *stub_bfd;

struct traverse_hash_info
{
  bfd *abfd;
  asection *sec;
};

/* Memory Reporting Info */
struct magic_section_description_tag {
 char *name;
 char *description;
};

struct magic_sections_tag
{
  const struct magic_section_description_tag *sections;
  unsigned int count;
  unsigned int index;
  unsigned int start;
};

static void bfd_pic32_report_memory_usage
  PARAMS ((FILE *));

static void report_percent_used
  PARAMS ((bfd_vma, bfd_vma, FILE *));

static void pic32_build_section_list
  PARAMS ((bfd *, asection *, PTR));

static void pic32_init_section_list
  PARAMS ((struct pic32_section **));

static lang_memory_region_type *region_lookup
  PARAMS ((char *));

static bfd_size_type bfd_pic32_report_sections
  PARAMS ((asection *, const lang_memory_region_type *,
           struct magic_sections_tag *, FILE *));

struct bfd_link_hash_entry *bfd_mchp_is_defined_global_symbol
  PARAMS ((const char *const));

static bfd_boolean bfd_pic32_undefine_one_symbol_bfd
  PARAMS ((struct bfd_link_hash_entry *, PTR));

static void bfd_pic32_undefine_symbols_bfd
  PARAMS ((bfd *));

static void bfd_mchp_remove_archive_module
  PARAMS ((const char *const));

static const char * bfd_pic32_lookup_magic_section_description
  PARAMS ((const char *, struct magic_sections_tag *, const char **));

static bfd_boolean elf_link_check_archive_element
  PARAMS ((char *, bfd *, struct bfd_link_info *));

/* Declare functions used by various EXTRA_EM_FILEs.  */
#if 0
static void pic32_before_parse (void);
static void gld${EMULATION_NAME}_before_allocation (void);
static void pic32_after_open PARAMS ((void));
static lang_output_section_statement_type pic32_place_orphan
  (asection *, const char *, int);
static void pic32_finish (void);
#endif

unsigned force_keep_symbol
  PARAMS ((char *symbol, char *module_name));

/* Memory Reporting Info */
static const struct magic_section_description_tag magic_pmdescriptions[] =
   {{"", ""},
   {".text", "Application's executable code"},
   {".rodata", "Read-only constant data"},
   {".sdata2", "Small initialized, constant global and static data template"},
   {".sbss2", "Small uninitialized, constant global and static data"},
   {".eh_frame_hdr", "Stack unwind header"},
   {".eh_frame", "Stack unwind info"},
   {".data", "Data-initialization template"},
   {".got", "Global offset table"},
   {".sdata", "Small data-initialization template"},
   {".lit8", "8-byte constants"},
   {".lit4", "4-byte constants"},
   {".ramfunc", "RAM-function template"},
   {".startup", "C startup code"},
   {".app_excpt", "General-Exception handler"},
   {".reset", "Reset handler"},
   {".bev_excpt", "BEV-Exception handler"},
   {".vector_0", "Interrupt Vector 0"},
   {".vector_1", "Interrupt Vector 1"},
   {".vector_2", "Interrupt Vector 2"},
   {".vector_3", "Interrupt Vector 3"},
   {".vector_4", "Interrupt Vector 4"},
   {".vector_5", "Interrupt Vector 5"},
   {".vector_6", "Interrupt Vector 6"},
   {".vector_7", "Interrupt Vector 7"},
   {".vector_8", "Interrupt Vector 8"},
   {".vector_9", "Interrupt Vector 9"},
   {".vector_10", "Interrupt Vector 10"},
   {".vector_11", "Interrupt Vector 11"},
   {".vector_12", "Interrupt Vector 12"},
   {".vector_13", "Interrupt Vector 13"},
   {".vector_14", "Interrupt Vector 14"},
   {".vector_15", "Interrupt Vector 15"},
   {".vector_16", "Interrupt Vector 16"},
   {".vector_17", "Interrupt Vector 17"},
   {".vector_18", "Interrupt Vector 18"},
   {".vector_19", "Interrupt Vector 19"},
   {".vector_20", "Interrupt Vector 20"},
   {".vector_21", "Interrupt Vector 21"},
   {".vector_22", "Interrupt Vector 22"},
   {".vector_23", "Interrupt Vector 23"},
   {".vector_24", "Interrupt Vector 24"},
   {".vector_25", "Interrupt Vector 25"},
   {".vector_26", "Interrupt Vector 26"},
   {".vector_27", "Interrupt Vector 27"},
   {".vector_28", "Interrupt Vector 28"},
   {".vector_29", "Interrupt Vector 29"},
   {".vector_30", "Interrupt Vector 30"},
   {".vector_31", "Interrupt Vector 31"},
   {".vector_32", "Interrupt Vector 32"},
   {".vector_33", "Interrupt Vector 33"},
   {".vector_34", "Interrupt Vector 34"},
   {".vector_35", "Interrupt Vector 35"},
   {".vector_36", "Interrupt Vector 36"},
   {".vector_37", "Interrupt Vector 37"},
   {".vector_38", "Interrupt Vector 38"},
   {".vector_39", "Interrupt Vector 39"},
   {".vector_40", "Interrupt Vector 40"},
   {".vector_41", "Interrupt Vector 41"},
   {".vector_42", "Interrupt Vector 42"},
   {".vector_43", "Interrupt Vector 43"},
   {".vector_44", "Interrupt Vector 44"},
   {".vector_45", "Interrupt Vector 45"},
   {".vector_46", "Interrupt Vector 46"},
   {".vector_47", "Interrupt Vector 47"},
   {".vector_48", "Interrupt Vector 48"},
   {".vector_49", "Interrupt Vector 49"},
   {".vector_50", "Interrupt Vector 50"},
   {".vector_51", "Interrupt Vector 51"},
   {".vector_52", "Interrupt Vector 52"},
   {".vector_53", "Interrupt Vector 53"},
   {".vector_54", "Interrupt Vector 54"},
   {".vector_55", "Interrupt Vector 55"},
   {".vector_56", "Interrupt Vector 56"},
   {".vector_57", "Interrupt Vector 57"},
   {".vector_58", "Interrupt Vector 58"},
   {".vector_59", "Interrupt Vector 59"},
   {".vector_60", "Interrupt Vector 60"},
   {".vector_61", "Interrupt Vector 61"},
   {".vector_62", "Interrupt Vector 62"},
   {".vector_63", "Interrupt Vector 63"}
   };

static const struct magic_section_description_tag magic_dmdescriptions[] =
   {{"", ""},
   {".data", "Initialized data"},
   {".got", "Global offset table"},
   {".sdata", "Small initialized data"},
   {".lit8", "8-byte constants"},
   {".lit4", "4-byte constants"},
   {".sbss", "Small uninitialized data"},
   {".bss", "Uninitialized data"},
   {".heap", "Dynamic Memory heap"},
   {".stack", "Minimum space reserved for stack"},
   {".ramfunc", "RAM functions"}};

struct magic_sections_tag magic_dm =
{
  magic_dmdescriptions,
  0, 0, 0
};
struct magic_sections_tag magic_pm =
{
  magic_pmdescriptions,
  0, 0, 0
};

static bfd_size_type actual_prog_memory_used = 0;
static bfd_size_type data_memory_used = 0;
static bfd_size_type region_data_memory_used = 0;

/* Section Lists */
static struct pic32_section *pic32_section_list;


/*
** smart_io Data Structure
**
** The following data structure is used to identify
** I/O function pairs that have redundant functionality.
** The "full_set" function is a standard I/O function
** such as printf(). The "reduced_set" function has the
** same type and parameter list but supports a reduced
** feature set, such as the integer-only _iprintf().
**
** If both members of a function pair are defined,
** the reduced_set function is considered redundant.
** The --smart-io option causes the linker to merge
** the two functions in order to conserve memory.
**
** Function merging is performed by the _after_open()
** emulation entry point, after all the input BFDs
** have been opened, but before memory is allocated.
**
** Function merging consists of several steps:
**
** 1. Lookup both function names in the Global Linker
**    Hash Table and load pointers to the corresponding
**    hash table entries.
**
** 2. Copy the (value) and (section) fields from the
**    full_set hash table entry to the reduced_set hash
**    table entry. This causes the reduced_set function
**    name to be a synonym for the full_set function name.
**
**    This operation is valid because:
**
**    (section) is a pointer into a BFD data structure.
**    When a BFD is opened, it remains locked in memory
**    until it is closed at the completion of the link.
**    Pointers into a BFD data structure are guaranteed
**    not to move.
**
**    (value) is an offset into an input section. Offsets
**    within an input section are determined by the
**    assembler and do not change during the link.
**
** 3. Loop through each input statement in the linker script
**    and find the BFD associated with the reduced_set
**    function, based on its module_name.
**
** 4. Traverse the Global Linker Hash Table and change the
**    type of any symbols that are defined and owned by
**    the reduced_set BFD to "undefined".
**
** 5. For each section attached to the reduced_set BFD,
**    set the SEC_NEVER_LOAD section flag and set the
**    section size to zero. This causes the reduced_set
**    function to be omitted from the final link.
*/

struct reduced_set_list 
{
  char *reduced_set;
  char *module_name;
  struct reduced_set_list *next;
};

struct function_pair_type
{
  char *full_set;
  char *prefix;
  struct reduced_set_list *rsl;
};

  /* With RULES == 2 we put flags after the function name, almost like
     C++ encoding.  We need to select the function with the minimum set of
     extra flags at the end */
struct function_pair_type function_pairs[] =
  {
    /* iprintf() */
    { "printf",    "_printf",   0  },
    { "_printf",   "__printf",  0  },
    { "_putfld",   "__putfld",  0  },

    { "_dprintf",  "_printf",   0  },
    { "__dprintf", "__printf",  0  },
    { "__dputfld", "__putfld",  0  },

    /* ifprintf() */
    { "fprintf",   "_fprintf",  0  },
    { "_dfprintf", "_fprintf",  0  },

    /* isprintf() */
    { "sprintf",   "_sprintf",  0  },
    { "_dsprintf", "_sprintf",  0  },

    /* ivprintf() */
    { "vprintf",   "_vprintf",  0  },
    { "_dvprintf", "_vprintf",  0  },

    /* ivfprintf() */
    { "vfprintf",  "_vfprintf",  0 },
    { "_dvfprintf","_vfprintf",  0 },

    /* ivsprintf() */
    { "vsprintf",  "_vsprintf",  0 },
    { "_dvsprintf","_vsprintf",  0 },

    /* isnprintf() */
    { "snprintf",  "_snprintf",  0 },
    { "_dsnprintf","_snprintf",  0 },

    /* ivsnprintf() */
    { "vsnprintf", "_vsnprintf", 0 },
    { "_dvsnprintf","_vsnprintf",0 },

    /* isnprintf() */
    { "asprintf",  "_asprintf",  0 },
    { "_dasprintf","_asprintf",  0 },

    /* ivsnprintf() */
    { "vasprintf", "_vasprintf", 0 },
    { "_dvasprintf","_vasprintf",0 },

    /* iscanf() */
    { "scanf",     "_scanf",     0 },
    { "_scanf",    "__sscanf",   0 },
    { "_getfld",   "__getfld",   0 },

    { "_dscanf",   "_scanf",     0 },
    { "__dscanf",  "__sscanf",   0 },
    { "__dgetfld", "__getfld",   0 },

    /* ifscanf() */
    { "fscanf",    "_fscanf",    0 },
    { "_dfscanf",  "_fscanf",    0 },

    /* isscanf() */
    { "sscanf",    "_sscanf",    0 },
    { "_dsscanf",  "_sscanf",    0 },
    
    { "vsscanf",    "_vsscanf",    0 },
    { "_dvsscanf",  "_vsscanf",    0 },
    
    { "vscanf",    "_vscanf",    0 },
    { "_dvscanf",  "_vscanf",    0 },

    /* ldtob */
    { "_ldtob",   "__ldtob",    0 },
    { "_genld",   "__genld",    0 },

    /* null terminator */
    { 0, 0, 0 },
  };

static void
mips_after_parse (void)
{
  /* .gnu.hash and the MIPS ABI require .dynsym to be sorted in different
     ways.  .gnu.hash needs symbols to be grouped by hash code whereas the
     MIPS ABI requires a mapping between the GOT and the symbol table.  */
  if (link_info.emit_gnu_hash)
    {
      einfo ("%X%P: .gnu.hash is incompatible with the MIPS ABI\n");
      link_info.emit_hash = TRUE;
      link_info.emit_gnu_hash = FALSE;
    }
  after_parse_default ();
}

struct hook_stub_info
{
  lang_statement_list_type add;
  asection *input_section;
};

/* Traverse the linker tree to find the spot where the stub goes.  */

static bfd_boolean
hook_in_stub (struct hook_stub_info *info, lang_statement_union_type **lp)
{
  lang_statement_union_type *l;
  bfd_boolean ret;

  for (; (l = *lp) != NULL; lp = &l->header.next)
    {
      switch (l->header.type)
	{
	case lang_constructors_statement_enum:
	  ret = hook_in_stub (info, &constructor_list.head);
	  if (ret)
	    return ret;
	  break;

	case lang_output_section_statement_enum:
	  ret = hook_in_stub (info,
			      &l->output_section_statement.children.head);
	  if (ret)
	    return ret;
	  break;

	case lang_wild_statement_enum:
	  ret = hook_in_stub (info, &l->wild_statement.children.head);
	  if (ret)
	    return ret;
	  break;

	case lang_group_statement_enum:
	  ret = hook_in_stub (info, &l->group_statement.children.head);
	  if (ret)
	    return ret;
	  break;

	case lang_input_section_enum:
	  if (info->input_section == NULL
	      || l->input_section.section == info->input_section)
	    {
	      /* We've found our section.  Insert the stub immediately
		 before its associated input section.  */
	      *lp = info->add.head;
	      *(info->add.tail) = l;
	      return TRUE;
	    }
	  break;

	case lang_data_statement_enum:
	case lang_reloc_statement_enum:
	case lang_object_symbols_statement_enum:
	case lang_output_statement_enum:
	case lang_target_statement_enum:
	case lang_input_statement_enum:
	case lang_assignment_statement_enum:
	case lang_padding_statement_enum:
	case lang_address_statement_enum:
	case lang_fill_statement_enum:
	  break;

	default:
	  FAIL ();
	  break;
	}
    }
  return FALSE;
}

/* Create a new stub section called STUB_SEC_NAME and arrange for it to
   be linked in OUTPUT_SECTION.  The section should go at the beginning of
   OUTPUT_SECTION if INPUT_SECTION is null, otherwise it must go immediately
   before INPUT_SECTION.  */

static asection *
mips_add_stub_section (const char *stub_sec_name, asection *input_section,
		       asection *output_section)
{
  asection *stub_sec;
  flagword flags;
  const char *secname;
  lang_output_section_statement_type *os;
  struct hook_stub_info info;

  /* Create the stub file, if we haven't already.  */
  if (stub_file == NULL)
    {
      stub_file = lang_add_input_file ("linker stubs",
				       lang_input_file_is_fake_enum,
				       NULL);
      stub_bfd = bfd_create ("linker stubs", link_info.output_bfd);
      if (stub_bfd == NULL
	  || !bfd_set_arch_mach (stub_bfd,
				 bfd_get_arch (link_info.output_bfd),
				 bfd_get_mach (link_info.output_bfd)))
	{
	  einfo ("%F%P: can not create BFD %E\n");
	  return NULL;
	}
      stub_bfd->flags |= BFD_LINKER_CREATED;
      stub_file->the_bfd = stub_bfd;
      ldlang_add_file (stub_file);
    }

  /* Create the section.  */
  stub_sec = bfd_make_section_anyway (stub_bfd, stub_sec_name);
  if (stub_sec == NULL)
    goto err_ret;

  /* Set the flags.  */
  flags = (SEC_ALLOC | SEC_LOAD | SEC_READONLY | SEC_CODE
	   | SEC_HAS_CONTENTS | SEC_IN_MEMORY | SEC_KEEP);
  if (!bfd_set_section_flags (stub_bfd, stub_sec, flags))
    goto err_ret;

  /* Create an output section statement.  */
  secname = bfd_get_section_name (output_section->owner, output_section);
  os = lang_output_section_find (secname);

  /* Initialize a statement list that contains only the new statement.  */
  lang_list_init (&info.add);
  lang_add_section (&info.add, stub_sec, os);
  if (info.add.head == NULL)
    goto err_ret;

  /* Insert the new statement in the appropriate place.  */
  info.input_section = input_section;
  if (hook_in_stub (&info, &os->children.head))
    return stub_sec;

 err_ret:
  einfo ("%X%P: can not make stub section: %E\n");
  return NULL;
}

/* This is called before the input files are opened.  */

static void
mips_create_output_section_statements (void)
{
  if (is_mips_elf (link_info.output_bfd))
    _bfd_mips_elf_init_stubs (&link_info, mips_add_stub_section);
}

/* This is called after we have merged the private data of the input bfds.  */

static void
mips_before_allocation (void)
{
  flagword flags;

  flags = elf_elfheader (link_info.output_bfd)->e_flags;
  if (!link_info.shared
      && !link_info.nocopyreloc
      && (flags & (EF_MIPS_PIC | EF_MIPS_CPIC)) == EF_MIPS_CPIC)
    _bfd_mips_elf_use_plts_and_copy_relocs (&link_info);

  gld${EMULATION_NAME}_before_allocation ();
}

/* Avoid processing the fake stub_file in vercheck, stat_needed and
   check_needed routines.  */

static void (*real_func) (lang_input_statement_type *);

static void mips_for_each_input_file_wrapper (lang_input_statement_type *l)
{
  if (l != stub_file)
    (*real_func) (l);
}

static void
mips_lang_for_each_input_file (void (*func) (lang_input_statement_type *))
{
  real_func = func;
  lang_for_each_input_file (&mips_for_each_input_file_wrapper);
}

#define lang_for_each_input_file mips_lang_for_each_input_file

/*
** Utility routine: bfd_pic32_report_memory_usage
**
** - print a section chart to file *fp
**/
static void
bfd_pic32_report_memory_usage (fp)
     FILE *fp;
{
  bfd_size_type region_prog_memory_used;
  bfd_size_type total_prog_memory, total_data_memory;
  lang_memory_region_type *region;
  unsigned int region_index, region_count;
  struct pic32_section *s;
  struct region_report_tag {
    char *name;
    char *title;
    char *total;
  };
  struct region_report_tag pmregions_to_report[] =
    {{"kseg0_program_mem",
      "kseg0 Program-Memory Usage",
      "      Total kseg0_program_mem used"},
     {"kseg0_boot_mem",
      "kseg0 Boot-Memory Usage",
      "         Total kseg0_boot_mem used"},
     {"exception_mem",
      "Exception-Memory Usage ",
      "          Total exception_mem used"},
     {"kseg1_boot_mem",
      "kseg1 Boot-Memory Usage",
      "         Total kseg1_boot_mem used"}};

  struct region_report_tag dmregions_to_report[] =
    {{"kseg1_data_mem",
      "kseg1 Data-Memory Usage",
      "         Total kseg1_data_mem used"}};

  /* clear the counters */
  actual_prog_memory_used = 0;
  data_memory_used = 0;
  region_prog_memory_used = 0;
  region_data_memory_used = 0;
  total_data_memory = total_prog_memory = 0;
  if (pic32_debug)
    {
      fprintf (stdout, "DEBUG: Generating Memory-Usage Report\n");
    }

  fprintf( fp, "\nMicrochip PIC32 Memory-Usage Report");

  /* build an ordered list of output sections */
  pic32_init_section_list(&pic32_section_list);
  bfd_map_over_sections(link_info.output_bfd, &pic32_build_section_list, NULL);

  region_count = sizeof(pmregions_to_report)/sizeof(pmregions_to_report[0]);
  magic_pm.count = sizeof(magic_pmdescriptions)/sizeof(magic_pmdescriptions[0]);
  magic_pm.start = magic_pm.index = 0;
  for (region_index = 0; region_index < region_count; region_index++)
  {
     region = region_lookup(pmregions_to_report[region_index].name);

     /* print code header */
     fprintf( fp, "\n\n%s\n", pmregions_to_report[region_index].title);
     fprintf( fp, "section                    address  length [bytes]      (dec)  Description\n");
     fprintf( fp, "-------                 ----------  -------------------------  -----------\n");
     /* report code sections */
     for (s = pic32_section_list; s != NULL; s = s->next)
       if (s->sec)
       {
         region_prog_memory_used += bfd_pic32_report_sections (s->sec, region, &magic_pm,fp);
       }
     fprintf( fp, "%s  :  %#10lx  %10ld  ",
            pmregions_to_report[region_index].total,
            (unsigned long)region_prog_memory_used, (unsigned long)region_prog_memory_used);
     report_percent_used(region_prog_memory_used,region->length, fp);
     total_prog_memory += region->length;
     actual_prog_memory_used += region_prog_memory_used;
     region_prog_memory_used = 0;
   }

  /* -------------------- */
  /*  print code summary  */
  /* -------------------- */

  fprintf (fp, "\n        --------------------------------------------------------------------------\n");
  fprintf (fp,     "         Total Program Memory used  :  %#10lx  %10ld  ",
         actual_prog_memory_used, actual_prog_memory_used);
  report_percent_used (actual_prog_memory_used, total_prog_memory, fp);
  fprintf (fp, "\n        --------------------------------------------------------------------------\n");

  /* DATA MEMORY */

  region_count = sizeof(dmregions_to_report)/sizeof(dmregions_to_report[0]);
  magic_dm.count = sizeof(magic_dmdescriptions)/sizeof(magic_dmdescriptions[0]);
  magic_dm.start = magic_dm.index = 0;
  for (region_index = 0; region_index < region_count; region_index++)
  {
     region = region_lookup(dmregions_to_report[region_index].name);
     /* print code header */
     fprintf( fp, "\n\n%s\n", dmregions_to_report[region_index].title);
     fprintf( fp, "section                    address  length [bytes]      (dec)  Description\n");
     fprintf( fp, "-------                 ----------  -------------------------  -----------\n");
     /* report code sections */
     for (s = pic32_section_list; s != NULL; s = s->next)
       if (s->sec)
       {
         region_data_memory_used += bfd_pic32_report_sections (s->sec, region, &magic_dm, fp);
       }

     fprintf( fp, "%s  :  %#10lx  %10ld  ",
            dmregions_to_report[region_index].total,
            (unsigned long)region_data_memory_used, (unsigned long)region_data_memory_used);
     report_percent_used(region_data_memory_used,region->length, fp);
     total_data_memory += region->length;
     data_memory_used += region_data_memory_used;
     region_data_memory_used = 0;
   }

  /* -------------------- */
  /*  print data summary  */
  /* -------------------- */

  fprintf( fp, "\n        --------------------------------------------------------------------------\n");
  fprintf( fp,     "            Total Data Memory used  :  %#10lx  %10ld  ",
         data_memory_used, data_memory_used);
  report_percent_used (data_memory_used, total_data_memory, fp);
  fprintf( fp, "\n        --------------------------------------------------------------------------\n");

} /* static void bfd_pic32_report_memory_usage (...)*/

static void
report_percent_used (used, max, fp)
     bfd_vma used, max;
     FILE *fp;
{
  double percent;

  if (max > 0) {
     percent = 100 * ((float) used/max);

     if (percent)
       fprintf( fp, "%.1f%% of %#lx", percent, max);
     else
       fprintf( fp, "<1%% of %#lx", max);
   }
}

/*
** Return a pointer to bfd_link_hash_entry
** if a global symbol is defined;
** else return zero.
*/
struct bfd_link_hash_entry *
bfd_mchp_is_defined_global_symbol (name)
     const char *const name;
{
    struct bfd_link_hash_entry *h;
    h = bfd_link_hash_lookup (link_info.hash, name, FALSE, FALSE, TRUE);
    if ((h != (struct bfd_link_hash_entry *) NULL) &&
        (h->type == bfd_link_hash_defined)) {
      return h;
    }
    else
      return (struct bfd_link_hash_entry *) NULL;
}


/*
** Undefine one symbol by BFD
**
** This routine is called by bfd_link_hash_traverse()
*/
static bfd_boolean
bfd_pic32_undefine_one_symbol_bfd (h, p)
     struct bfd_link_hash_entry *h;
     PTR p;
{
  struct bfd_link_hash_entry *H = (struct bfd_link_hash_entry *) h;
  struct traverse_hash_info *info = ( struct traverse_hash_info *) p;

  /* if this symbol is defined, and owned by the BFD in question */
  if ((( H->type == bfd_link_hash_defined) ||
       ( H->type == bfd_link_hash_defweak)) &&
      ( H->u.def.section->owner == info->abfd))

    /* mark it undefined */
    H->type = bfd_link_hash_undefined;

  /* traverse the entire hash table */
  return TRUE;
} /* static bfd_boolean bfd_pic32_undefine_one_symbol_bfd (...)*/


/*
** Undefine all symbols owned by a bfd
*/
static void
bfd_pic32_undefine_symbols_bfd (target)
     bfd *target;
{
    struct traverse_hash_info info;
    info.abfd = target;

    /* traverse the global linker hash table */
    bfd_link_hash_traverse (link_info.hash,
       bfd_pic32_undefine_one_symbol_bfd, &info);

} /* static void bfd_pic32_undefine_symbols_bfd (...) */

/*
** Remove an archive module
*/
static void
bfd_mchp_remove_archive_module (name)
     const char *const name;
{
  LANG_FOR_EACH_INPUT_STATEMENT (is)
  {
      if (strcmp (is->the_bfd->filename, name) == 0)
      {
          if (pic32_debug)
          {
              printf("\nRemoving %s\n", name);
              printf("  %s had %d symbols\n", name , is->the_bfd->symcount);
          }

          /*
          ** remove the symbols
          */
          bfd_pic32_undefine_symbols_bfd (is->the_bfd);
          is->the_bfd->symcount = 0;

          /*
          ** remove the sections in this bfd
          */
          bfd_section_list_clear(is->the_bfd);
          break;
      }
  }
} /* static void bfd_mchp_remove_archive_module */


/*****************************************************************************
**
** Section List Routines
**
** Maintain a list of sections in order
** of ascending load address.
**
** Used for creating memory usage reports.
**
*/

/*
** Create a new list
**
** - first item is null
*/
static void
pic32_init_section_list(lst)
     struct pic32_section **lst;
{
  if (pic32_debug)
    {
      fprintf (stdout, "DEBUG: Init Section List\n");
    }
  *lst = ((struct pic32_section *)
         xmalloc(sizeof(struct pic32_section)));
  if (NULL == lst) {
    fprintf( stderr, "Fatal Error: Not enough memory to initialize section list\n");
    abort();
  }
  (*lst)->next = 0;
  (*lst)->sec = 0;
  (*lst)->attributes = 0;
  (*lst)->file = 0;
}


/*
** Add a section to the list
**
** - in order of increasing load address
** - this function is called via bfd_map_over_sections()
*/
static void
pic32_build_section_list (abfd, sect, fp)
     bfd *abfd ATTRIBUTE_UNUSED ;
     asection *sect;
     PTR fp ATTRIBUTE_UNUSED ;
{
  struct pic32_section *new, *s, *prev;
  int done = 0;
  if (pic32_debug)
    {
      fprintf (stderr, "DEBUG: Add section to list\n");
    }
  /* create a new element */
  new = ((struct pic32_section *)
         xmalloc(sizeof(struct pic32_section)));
  if (NULL == new) {
    fprintf( stderr, "Fatal Error: Could not build section list\n");
    abort();
  }
  new->sec  = sect;
  new->file = 0;

  /* insert it at the right spot */
  prev = pic32_section_list;
  for (s = prev; s != NULL; s = s->next)
    {
      if (s->sec && (new->sec->lma < s->sec->lma))
        {
          prev->next = new;
          new->next = s;
          done++;
          break;
        }
      prev = s;
    }

  if (!done)
    {
      prev->next = new;
      new->next = NULL;
    }
} /* static void pic32_build_section_list (...) */

/*
** region_lookup()
**
** Call this instead of lang_memory_region_lookup()
** if you want reasonable defaults set for the
** following regions:
**
** data
** program
**
** Note: Ideally, every linker script will include
** these definitions. But we don't want to fail
** if the user is running with a minimal script.
*/
#define NAME_IS(s) \
  ((strlen(name) == strlen(#s)) && \
           strcmp(name, (#s)) == 0)

static lang_memory_region_type *
region_lookup( name )
     char * name;
{
  lang_memory_region_type *region;
  bfd_vma upper_limit = 0;
  int use_default = 0;

  if (pic32_debug)
    {
      fprintf (stdout, "DEBUG: Region lookup\n");
    }

  region = lang_memory_region_lookup (name, FALSE);
  if (region->length == ~(bfd_size_type) 0) {
    if (NAME_IS(kseg0_program_mem)) {
      use_default = 1;
      region->origin = 0x9D000000;     /* works with 32MX320F512L */
      region->length = 0x80000;
      upper_limit    = (region->origin + region->length)-1;
    }
    else if (NAME_IS(kseg0_boot_mem)) {
      use_default = 1;
      region->origin = 0x9FC00490;     /* works with 32MX320F512L */
      region->length = 0x970;
      upper_limit    = (region->origin + region->length)-1;
    }
    else if (NAME_IS(exception_mem)) {
      use_default = 1;
      region->origin = 0x9FC01000;     /* works with 32MX320F512L */
      region->length = 0x1000;
      upper_limit    = (region->origin + region->length)-1;
    }
    else if (NAME_IS(kseg1_boot_mem)) {
      use_default = 1;
      region->origin = 0x9FC01000;     /* works with 32MX320F512L */
      region->length = 0x490;
      upper_limit    = (region->origin + region->length)-1;
    }
    else if (NAME_IS(kseg1_data_mem)) {
      use_default = 1;
      region->origin = 0xA0000000;     /* works with 32MX320F512L */
      region->length = 0x8000;
      upper_limit    = (region->origin + region->length)-1;
    }
    else
      einfo("Memory region %s does not exist\n", name);

    if (use_default) {
      einfo(_("%P: Warning: %s memory region not defined,"
              " using default range: 0x%v .. 0x%v\n"),
            name, region->origin, upper_limit);
      region->current = region->origin;
    }
  }
  return region;
}

/*
** Utility routine: bfd_pic32_report_sections()
**
** - show memory usage of SEC_ALLOC-flagged sections in memory
*/
static bfd_size_type
bfd_pic32_report_sections (sect, region, magic_sections, fp)
     asection *sect;
     const lang_memory_region_type *region;
     struct magic_sections_tag *magic_sections;
     FILE *fp;
{
  const char *description;
  bfd_size_type region_used = 0;
  unsigned long start = sect->vma;
  unsigned long load  = sect->lma;
  unsigned long actual = sect->size;

  /*
  ** report SEC_ALLOC sections in memory
  */
  if ((sect->flags & SEC_ALLOC)
  &&  (sect->size > 0))
    {
      char *name, *c;

      /* make a local copy of the name */
      name = xmalloc(strlen(sect->name) + 1);
      if (NULL == name) {
        fprintf( stderr, "Fatal Error: Could not copy section name\n");
        abort();
      }
      strcpy(name, sect->name);

      /* clean the name of %n */
      c = strchr(name, '%');
      if (c) *c = (char) '\0';

      if ((start >= region->origin)
        &&  ((start + actual) <= (region->origin + region->length)))
      {
         bfd_pic32_lookup_magic_section_description (name, magic_sections, &description);
         fprintf( fp, "%-24s%#10lx     %#10lx  %10ld  %s \n", name,
                 start, actual, actual, description);
         region_used = actual;
      }
      else if ((load >= region->origin)
        &&  ((load + actual) <= (region->origin + region->length))
        && (sect->flags & (SEC_HAS_CONTENTS)))
      {
         bfd_pic32_lookup_magic_section_description (name, magic_sections, &description);
         fprintf( fp, "%-24s%#10lx     %#10lx  %10ld  %s \n", name,
                  load, actual, actual, description);
         region_used = actual;
      }
      free(name);
    }
  return region_used;
} /* static bfd_size_type bfd_pic32_report_sections (...) */

static const char empty_string[] = "";
static const char * bfd_pic32_lookup_magic_section_description (name, magic_sections, description)
     const char *name;
     struct magic_sections_tag *magic_sections;
     const char **description;
{
  if (pic32_debug)
    {
      fprintf (stdout, "DEBUG: Lookup section description\n");
    }
      /* Assume that we don't have a description for the magic section name */
      *description = &empty_string[0];
      do
      {
        if (strcmp(name, magic_sections->sections[magic_sections->index].name))
        {
          /* Keep searching */
          magic_sections->index++;
          if (magic_sections->index == magic_sections->count)
            magic_sections->index = 0;
        }
        else
        {
          *description = magic_sections->sections[magic_sections->index].description;
          break;
        }
      } while(magic_sections->index != magic_sections->start);
      /* Start the next search where this one left off. */
      magic_sections->start = magic_sections->index;

      return *description;
} /* static void char * bfd_pic32_lookup_magic_section_description (...) */

/* match against mchp_smartio_symbol, sorry for the global variable :( */
char *mchp_smartio_symbol;

struct smartio_sym_list {
  struct bfd_link_hash_entry *h;
  unsigned long long cum_mask;
  struct smartio_sym_list *next;
} *smartio_fn_list, *smartio_free_list;

static bfd_boolean
mchp_smartio_fn_list(struct bfd_link_hash_entry *h, PTR p) {
  struct smartio_sym_list *l;

  if (strncmp(mchp_smartio_symbol, h->root.string,
              strlen(mchp_smartio_symbol)) == 0) {
    unsigned long long mask = 0;
    int bit;
    const char *c;

    for (c = &h->root.string[strlen(mchp_smartio_symbol)]; *c; c++) {
      if (*c == '_') continue;
      if (*c == '0') bit = 0; 
      else {
        bit = *c - 'A' + 1;
        if (bit > 26) bit = 27 + (*c - 'a');
      }
      mask |= (1ULL << bit);
    }
    if (smartio_free_list) {
      l = smartio_free_list;
      smartio_free_list = l->next;
    } else {
      l = xmalloc(sizeof(struct smartio_sym_list));
    }
    l->h = h;
    if (smartio_fn_list)  mask |= smartio_fn_list->cum_mask;
    l->cum_mask = mask;
    l->next = smartio_fn_list;
    smartio_fn_list = l;
  }
  p = p;
  return TRUE;  /* continue */
}

/* search the function pair type information and track special symbols -
   keep those symbols that match, noting the variant and object file name
   so that we can later force their removal

   a NULL module_name implies that we are just checking, and we should return
   trueness */

unsigned
force_keep_symbol(char *symbol, char *module_name) {
  struct function_pair_type *fp;
  struct reduced_set_list *rsl = 0;

  for (fp = function_pairs; fp->full_set; fp++) {
    if (strncmp(fp->prefix, symbol, strlen(fp->prefix)) == 0) {
      if (module_name == 0) return 1;
      /* boing */
      for (rsl = fp->rsl; rsl; rsl = rsl->next) {
        /* reduced set already noted, so return true */
        if (strcmp(rsl->reduced_set, symbol) == 0) return 1;
      }
      rsl = xmalloc(sizeof(struct reduced_set_list));
      rsl->reduced_set = symbol;
      rsl->module_name = module_name;
      rsl->next = fp->rsl;
      fp->rsl = rsl;
      return 1;
    }
  }
  return 0;
}

static void
mchp_release_kept_symbols(char *symbol) {
  struct function_pair_type *fp;
  struct reduced_set_list *rsl = 0, *next;

  for (fp = function_pairs; fp->full_set; fp++) {
    if (strncmp(fp->prefix, symbol, strlen(fp->prefix)) == 0) {
      for (rsl = fp->rsl; rsl; rsl = next) {
        next = rsl->next;
        if (strcmp(rsl->reduced_set, symbol) != 0) {
          /* this symbol was not used, so release it */
          bfd_mchp_remove_archive_module(rsl->module_name);
        }
        free(rsl);
      }
      fp->rsl = 0;
    }
  }
}

static void
smartio_symbols(struct bfd_link_info *info) {
  int i = 0;
  struct bfd_link_hash_entry *undefs = 0;

  /* format different for new rules - look for letter combinations
     for each function and choose the best match */
  while ( function_pairs[i].full_set )
    {  struct smartio_sym_list *l;
       struct bfd_link_hash_entry *u = 0;
       struct bfd_link_hash_entry *full;

       char suffix[] = "_aAcdeEfFgGnopsuxX";

       if (info) {
         undefs=info->hash->undefs;
         if (undefs == 0) return;
       }
       mchp_smartio_symbol = function_pairs[i].prefix;
       if (undefs) {
         /* gather all the stats on undefined functions */
           if (pic32_debug)
             {
               u = undefs;
               if (u->u.undef.next == NULL)
                 einfo(_("Warning: undef list incomplete for smart IO merging\n"));
             }
         for (u = undefs; u; u = u->u.undef.next) {
           if (pic32_debug && 0)
             {
               fprintf(stderr, "u->root.string == %s\n", u->root.string);
             }
             mchp_smartio_fn_list(u,0);
         }
       } else {
         bfd_link_hash_traverse(link_info.hash, mchp_smartio_fn_list,
                                  0 /* not used */);
       }
       if (smartio_fn_list) {
         int letter;
         int s = 1;
         char buffer_map_to[256] = "";

         if (smartio_fn_list->cum_mask > 1) {
           /* this means that we have an accumulated suffix that includes
              more than just '0' [the wild-card-suffix] */
           for (letter = 0; letter < 26; letter++) {
             if (smartio_fn_list->cum_mask & (1ULL << (letter+27)))
               suffix[s++] = 'a' + letter;
             if (smartio_fn_list->cum_mask & (1ULL << (letter+1)))
               suffix[s++] = 'A' + letter;
           }
           suffix[s] = 0;
         } else {
           sprintf(suffix, "_0");
         }
         sprintf(buffer_map_to, "%s%s", function_pairs[i].prefix,
                 suffix);
         if (undefs) {
           /* in this mode we are adding undefined symbols to the end of
              the list so that the regular mechanism can pull them in */
           if (bfd_link_hash_lookup(info->hash, buffer_map_to, 0, 0, 0) == 0) {
             struct bfd_link_hash_entry *new;

             if (pic32_debug) {
               fprintf(stderr,"Adding %s to undef\n", buffer_map_to);
               for (l = smartio_fn_list; l; l=l->next)
                 fprintf(stderr,"  because of: %s\n", l->h->root.string);
             }
             
             new = bfd_link_hash_lookup(info->hash, buffer_map_to, 1, 1 , 1);
             new->type =  bfd_link_hash_undefined;
             bfd_link_add_undef(info->hash, new);
           }
         } else {
           full = bfd_mchp_is_defined_global_symbol(buffer_map_to);
           if (!full) {
             if (pic32_debug)
               fprintf(stderr," no hash for %s\n", buffer_map_to);
           } else {
             for (l = smartio_fn_list; l; l=l->next) {
               if (pic32_debug)
                 printf("\nMapping %s to %s\n", l->h->root.string,
                                                buffer_map_to);
               l->h->u.def.value = full->u.def.value;
               l->h->u.def.section = full->u.def.section;
             }
             mchp_release_kept_symbols(buffer_map_to);
           }
         }
         /* free the list */
         if (!smartio_free_list) {
           for (l = smartio_fn_list; l->next; l = l->next);
           l->next = smartio_free_list;
         }
         smartio_free_list = smartio_fn_list;
         smartio_fn_list = 0;
       }
       i++;
    }
}

unsigned int (*mchp_force_keep_symbol)(char *, char *) = force_keep_symbol;
void (*mchp_smartio_symbols)(struct bfd_link_info *) = smartio_symbols;

/* This is called after all the input files have been opened.  */
static void
pic32_after_open(void)
{
 
  /*
  ** Merge full- and reduced-set I/O functions
  */
  if (pic32_smart_io)
    {
      if (pic32_debug)
        printf("\nMerging smart-io functions:\n");

      mchp_smartio_symbols(0);

#if 0
      while ( function_pairs[i].full_set )

        {
          struct bfd_link_hash_entry *full, *reduced;
          char *func1, *func2;

          func1 = function_pairs[i].full_set;
          func2 = function_pairs[i].reduced_set;
          if (pic32_debug)
            printf("\nLooking for (%s, %s) ...", func1, func2);

          if ((full = bfd_mchp_is_defined_global_symbol (func1)) &&
              (reduced = bfd_mchp_is_defined_global_symbol (func2)))
            {
              if (pic32_debug)
                printf("Found\n\nRedirecting %s -> %s\n", func2, func1);

              /* redirect reduced_set function to full_set function */
              reduced->u.def.value = full->u.def.value;
              reduced->u.def.section = full->u.def.section;

              /* remove the reduced_set module */
              bfd_pic32_remove_archive_module (function_pairs[i].module_name);
            }
          else
            if (pic32_debug)
              printf("Not Found\n");
          i++;
        }
#endif
    } /* pic32_smart_io */
    
    gld${EMULATION_NAME}_after_open();
}

/*
** finish()
**
** This function is called after symbol assignments
** are completed, but before relocations are resolved.
*/
static void
pic32_finish(void)
{

  bfd_boolean need_layout = bfd_elf_discard_info (link_info.output_bfd,
						  &link_info);

  if (config.map_file != NULL) {
    bfd_pic32_report_memory_usage (config.map_file);
  }
  if (pic32_report_mem)
    bfd_pic32_report_memory_usage (stdout);

  gld${EMULATION_NAME}_map_segments (need_layout);

  finish_default();
} /* static void pic32_finish ()*/


/* A variant of lang_output_section_find used by place_orphan.  */

static lang_output_section_statement_type *
output_rel_find (asection *sec, int isdyn)
{
  lang_output_section_statement_type *lookup;
  lang_output_section_statement_type *last = NULL;
  lang_output_section_statement_type *last_alloc = NULL;
  lang_output_section_statement_type *last_ro_alloc = NULL;
  lang_output_section_statement_type *last_rel = NULL;
  lang_output_section_statement_type *last_rel_alloc = NULL;
  int rela = sec->name[4] == 'a';

  for (lookup = &lang_output_section_statement.head->output_section_statement;
       lookup != NULL;
       lookup = lookup->next)
    {
      if (lookup->constraint >= 0
	  && CONST_STRNEQ (lookup->name, ".rel"))
	{
	  int lookrela = lookup->name[4] == 'a';

	  /* .rel.dyn must come before all other reloc sections, to suit
	     GNU ld.so.  */
	  if (isdyn)
	    break;

	  /* Don't place after .rel.plt as doing so results in wrong
	     dynamic tags.  */
	  if (strcmp (".plt", lookup->name + 4 + lookrela) == 0)
	    break;

	  if (rela == lookrela || last_rel == NULL)
	    last_rel = lookup;
	  if ((rela == lookrela || last_rel_alloc == NULL)
	      && lookup->bfd_section != NULL
	      && (lookup->bfd_section->flags & SEC_ALLOC) != 0)
	    last_rel_alloc = lookup;
	}

      last = lookup;
      if (lookup->bfd_section != NULL
	  && (lookup->bfd_section->flags & SEC_ALLOC) != 0)
	{
	  last_alloc = lookup;
	  if ((lookup->bfd_section->flags & SEC_READONLY) != 0)
	    last_ro_alloc = lookup;
	}
    }

  if (last_rel_alloc)
    return last_rel_alloc;

  if (last_rel)
    return last_rel;

  if (last_ro_alloc)
    return last_ro_alloc;

  if (last_alloc)
    return last_alloc;

  return last;
}

/* Place an orphan section.  We use this to put random SHF_ALLOC
   sections in the right segment.  */

static lang_output_section_statement_type *
gldelf32pic32mx_place_orphan (asection *s,
				   const char *secname,
				   int constraint)
{
  static struct orphan_save hold[] =
    {
      { ".text",
	SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_READONLY | SEC_CODE,
	0, 0, 0, 0 },
      { ".rodata",
	SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_READONLY | SEC_DATA,
	0, 0, 0, 0 },
      { ".persist",
	SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_DATA,
	0, 0, 0, 0 },
      { ".data",
	SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_DATA,
	0, 0, 0, 0 },
      { ".bss",
	SEC_ALLOC,
	0, 0, 0, 0 },
      { 0,
	SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_READONLY | SEC_DATA,
	0, 0, 0, 0 },
      { ".interp",
	SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_READONLY | SEC_DATA,
	0, 0, 0, 0 },
      { ".persist",
	SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_DATA | SEC_SMALL_DATA,
	0, 0, 0, 0 },
      { ".sdata",
	SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_DATA | SEC_SMALL_DATA,
	0, 0, 0, 0 },
      { 0,
	SEC_HAS_CONTENTS,
	0, 0, 0, 0 },
    };
  enum orphan_save_index
    {
      orphan_text = 0,
      orphan_rodata,
      orphan_persist,
      orphan_data,
      orphan_bss,
      orphan_rel,
      orphan_interp,
      orphan_spersist,
      orphan_sdata,
      orphan_nonalloc
    };
  static int orphan_init_done = 0;
  struct orphan_save *place;
  lang_output_section_statement_type *after;
  lang_output_section_statement_type *os;
  lang_output_section_statement_type *match_by_name = NULL;
  int isdyn = 0;
  int iself = s->owner->xvec->flavour == bfd_target_elf_flavour;
  unsigned int sh_type = iself ? elf_section_type (s) : SHT_NULL;

  if (! link_info.relocatable
      && link_info.combreloc
      && (s->flags & SEC_ALLOC))
    {
      if (iself)
	switch (sh_type)
	  {
	  case SHT_RELA:
	    secname = ".rela.dyn";
	    isdyn = 1;
	    break;
	  case SHT_REL:
	    secname = ".rel.dyn";
	    isdyn = 1;
	    break;
	  default:
	    break;
	  }
      else if (CONST_STRNEQ (secname, ".rel"))
	{
	  secname = secname[4] == 'a' ? ".rela.dyn" : ".rel.dyn";
	  isdyn = 1;
	}
    }

  /* Look through the script to see where to place this section.  */
  if (constraint == 0)
    for (os = lang_output_section_find (secname);
	 os != NULL;
	 os = next_matching_output_section_statement (os, 0))
      {
	/* If we don't match an existing output section, tell
	   lang_insert_orphan to create a new output section.  */
	constraint = SPECIAL;

	if (os->bfd_section != NULL
	    && (os->bfd_section->flags == 0
		|| (_bfd_elf_match_sections_by_type (link_info.output_bfd,
						     os->bfd_section,
						     s->owner, s)
		    && ((s->flags ^ os->bfd_section->flags)
			& (SEC_LOAD | SEC_ALLOC)) == 0)))
	  {
	    /* We already have an output section statement with this
	       name, and its bfd section has compatible flags.
	       If the section already exists but does not have any flags
	       set, then it has been created by the linker, probably as a
	       result of a --section-start command line switch.  */
	    lang_add_section (&os->children, s, os);
	    return os;
	  }

	/* Save unused output sections in case we can match them
	   against orphans later.  */
	if (os->bfd_section == NULL)
	  match_by_name = os;
      }

  /* If we didn't match an active output section, see if we matched an
     unused one and use that.  */
  if (match_by_name)
    {
      lang_add_section (&match_by_name->children, s, match_by_name);
      return match_by_name;
    }

  if (!orphan_init_done)
    {
      lang_output_section_statement_type *lookup;
      struct orphan_save *ho;

      for (ho = hold; ho < hold + sizeof (hold) / sizeof (hold[0]); ++ho)
	if (ho->name != NULL)
	  {
	    ho->os = lang_output_section_find (ho->name);
	    if (ho->os != NULL && ho->os->flags == 0)
	      ho->os->flags = ho->flags;
	  }
      lookup = hold[orphan_bss].os;
      if (lookup == NULL)
	lookup = &lang_output_section_statement.head->output_section_statement;
      for (; lookup != NULL; lookup = lookup->next)
	if ((lookup->bfd_section != NULL
	     && (lookup->bfd_section->flags & SEC_DEBUGGING) != 0)
	    || strcmp (lookup->name, ".comment") == 0)
	  break;
      hold[orphan_nonalloc].os = lookup ? lookup->prev : NULL;
      hold[orphan_nonalloc].name = ".comment";
      orphan_init_done = 1;
    }

  /* If this is a final link, then always put .gnu.warning.SYMBOL
     sections into the .text section to get them out of the way.  */
  if (link_info.executable
      && ! link_info.relocatable
      && CONST_STRNEQ (s->name, ".gnu.warning.")
      && hold[orphan_text].os != NULL)
    {
      os = hold[orphan_text].os;
      lang_add_section (&os->children, s, os);
      return os;
    }

  /* Decide which segment the section should go in based on the
     section name and section flags.  We put loadable .note sections
     right after the .interp section, so that the PT_NOTE segment is
     stored right after the program headers where the OS can read it
     in the first page.  */

  place = NULL;
  if ((s->flags & (SEC_ALLOC | SEC_DEBUGGING)) == 0)
    place = &hold[orphan_nonalloc];
  else if ((s->flags & SEC_ALLOC) == 0)
    ;
  else if ((s->flags & SEC_LOAD) != 0
	   && ((iself && sh_type == SHT_NOTE)
	       || (!iself && CONST_STRNEQ (secname, ".note"))))
    place = &hold[orphan_interp];
  else if ((s->flags & (SEC_LOAD | SEC_HAS_CONTENTS)) == 0)
    place = &hold[orphan_bss];
  else if ((s->flags & SEC_SMALL_DATA) != 0)
    place = &hold[orphan_spersist];
  else if ((s->flags & SEC_READONLY) == 0)
    place = &hold[orphan_persist];
  else if (((iself && (sh_type == SHT_RELA || sh_type == SHT_REL))
	    || (!iself && CONST_STRNEQ (secname, ".rel")))
	   && (s->flags & SEC_LOAD) != 0)
    place = &hold[orphan_rel];
  else if ((s->flags & SEC_CODE) == 0)
    place = &hold[orphan_rodata];
  else
    place = &hold[orphan_text];

  after = NULL;
  if (place != NULL)
    {
      if (place->os == NULL)
	{
	  if (place->name != NULL)
	    place->os = lang_output_section_find (place->name);
	  else
	    place->os = output_rel_find (s, isdyn);
	}
      after = place->os;
      if (after == NULL)
	after = lang_output_section_find_by_flags
	  (s, &place->os, _bfd_elf_match_sections_by_type);
      if (after == NULL)
	/* *ABS* is always the first output section statement.  */
	after = &lang_output_section_statement.head->output_section_statement;
    }

  return lang_insert_orphan (s, secname, constraint, after, place, NULL, NULL);
}

static bfd_boolean
elf_link_check_archive_element (name, abfd, info)
     char *name;
     bfd *abfd;
     struct bfd_link_info *info;
{
  /* we may need to pull this symbol in because it is a SMARTIO fn */
  if (mchp_force_keep_symbol)
      (void) mchp_force_keep_symbol(name, (char*)abfd->filename);

  info = info;

  return TRUE;
} 

bfd_boolean (*mchp_elf_link_check_archive_element)(char *name, bfd *abfd,
                                                    struct bfd_link_info *) = 
  elf_link_check_archive_element;


EOF


LDEMUL_AFTER_OPEN=pic32_after_open
LDEMUL_FINISH=pic32_finish
LDEMUL_PARSE_ARGS=gldelf32pic32mx_parse_args
LDEMUL_LIST_OPTIONS=gldelf32pic32mx_list_options
LDEMUL_AFTER_PARSE=mips_after_parse
LDEMUL_BEFORE_ALLOCATION=mips_before_allocation
LDEMUL_PLACE_ORPHAN=gldelf32pic32mx_place_orphan
LDEMUL_CREATE_OUTPUT_SECTION_STATEMENTS=mips_create_output_section_statements

