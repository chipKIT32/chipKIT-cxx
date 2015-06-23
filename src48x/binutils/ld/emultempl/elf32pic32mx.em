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

#define TARGET_IS_${EMULATION_NAME}
#ifndef TARGET_IS_PIC32
#define TARGET_IS_PIC32
#endif
#ifndef PIC32
#define PIC32
#endif

#include "ldctor.h"
#include "elf/mips.h"
#include "elfxx-mips.h"

#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"
#include "getopt.h"
#include "genlink.h"
#include "ld.h"
#include "ldmain.h"
#include "ldmisc.h"

#include "ldexp.h"
#include "ldlang.h"
#include "ldfile.h"

#include "ldemul.h"
#include "libbfd.h"
#include "elf-bfd.h"

#include "elf/pic32.h"
#include "ctype.h"
#include "../bfd/pic32-options.h"
//#include "../bfd/pic32-options.c"
#include "../bfd/pic32-utils.h"
#include <stdint.h>

/* defined in bfd/pic32-attributes.c */
char * pic32_section_size_string
  PARAMS ((asection *));
char * pic32_section_attr_string
  PARAMS ((asection *));


#define is_mips_elf(bfd)				\
  (bfd_get_flavour (bfd) == bfd_target_elf_flavour	\
   && elf_tdata (bfd) != NULL				\
   && elf_object_id (bfd) == MIPS_ELF_DATA)

/* Fake input file for stubs.  */
static lang_input_statement_type *stub_file;
static bfd *stub_bfd;

static bfd_boolean insn32;
static int eh_reloc_type = R_MIPS_EH;

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

/* Symbol Lists */
struct pic32_symbol
{
  char    *name;
  bfd_vma value;
};

bfd_boolean need_layout;

static void bfd_pic32_report_memory_sections
  PARAMS ((const char *, asection *, FILE *));

void bfd_pic32_report_memory_usage
  PARAMS ((FILE *));

static void report_percent_used
  PARAMS ((bfd_vma, bfd_vma, FILE *));

static void pic32_build_section_list
  PARAMS ((bfd *, asection *, PTR));

static void pic32_build_section_list_vma
  PARAMS ((bfd *, asection *, PTR));

static void pic32_init_section_list
  PARAMS ((struct pic32_section **));

static bfd_boolean pic32_name_in_section_list
  PARAMS ((struct pic32_section *, const char *));

static void bfd_pic32_finish
  PARAMS ((void));

void pic32_create_data_init_template (void);

void pic32_create_specific_fill_sections(void);

static bfd * bfd_pic32_create_data_init_bfd
  PARAMS ((bfd *));

static void bfd_pic32_add_bfd_to_link
  PARAMS ((bfd *, const char *));

static void bfd_pic32_scan_data_section
  PARAMS ((asection *, PTR));

static void bfd_pic32_skip_data_section
  PARAMS ((asection *, PTR));

static void pic32_strip_sections
 PARAMS ((bfd *));
 
static asection * bfd_pic32_create_section
  PARAMS ((bfd *, const char *, int, int));

static void pic32_append_section_to_list
  PARAMS ((struct pic32_section *, lang_input_statement_type *, asection *));

static void pic32_free_section_list
  PARAMS ((struct pic32_section **));

static int pic32_section_list_length
  PARAMS ((struct pic32_section *));

static void pic32_print_section_list
  PARAMS ((struct pic32_section *, const char *));

static void pic32_remove_from_section_list
  PARAMS ((struct pic32_section *, struct pic32_section *));

static void pic32_remove_group_from_section_list
  PARAMS ((struct pic32_section *));

static struct pic32_memory * pic32_static_assign_memory
  PARAMS ((struct pic32_memory *, bfd_vma, bfd_vma));

static void pic32_add_to_memory_list
  PARAMS ((struct pic32_memory *, bfd_vma, bfd_vma));

static void pic32_remove_from_memory_list
  PARAMS ((struct pic32_memory *, struct pic32_memory *));

static void build_alloc_section_list
  PARAMS ((unsigned int));

static void allocate_memory
  PARAMS ((void));

static int allocate_program_memory
  PARAMS ((void));

static int allocate_data_memory
  PARAMS ((void));

static int allocate_user_memory
  PARAMS ((void));

static void build_free_block_list
  PARAMS ((struct memory_region_struct *, unsigned int));

static int locate_sections
  PARAMS ((unsigned int, unsigned int, struct memory_region_struct *));

static void update_section_info
  PARAMS ((bfd_vma,
           struct pic32_section *, struct memory_region_struct *));

static void update_group_section_info
  PARAMS ((bfd_vma alloc_addr,
                    struct pic32_section *g,
                    struct memory_region_struct *region));

static struct pic32_memory * select_free_block
  PARAMS ((struct pic32_section *, unsigned int));

static void create_remainder_blocks
  PARAMS ((struct pic32_memory *, struct pic32_memory *, unsigned int));

static void remove_free_block
  PARAMS ((struct pic32_memory *));

static void pic32_free_memory_list
  PARAMS ((struct pic32_memory **));

static void pic32_init_memory_list
  PARAMS ((struct pic32_memory **));

static bfd_boolean pic32_unique_section
  PARAMS ((const char *s));

static void pic32_init_symbol_list
  PARAMS ((struct pic32_symbol **));

static void pic32_add_to_symbol_list
  PARAMS ((struct pic32_symbol **, const char *, bfd_vma));

static bfd_boolean bfd_pic32_process_bfd_after_open
  PARAMS ((bfd *, struct bfd_link_info *));

static lang_memory_region_type *region_lookup
  PARAMS ((char *));

static bfd_size_type bfd_pic32_report_sections
  PARAMS ((struct pic32_section *, const lang_memory_region_type *,
           struct magic_sections_tag *, FILE *));

static bfd_size_type bfd_pic32_collect_section_size
  PARAMS ((struct pic32_section *, const lang_memory_region_type *));

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

struct bfd_link_hash_entry *bfd_pic32_is_defined_global_symbol
  PARAMS ((const char *const));

static void allocate_default_stack
  PARAMS ((void));

#ifdef TARGET_IS_PIC32MX
  /* fill option definitions */
static bfd * bfd_pic32_create_fill_bfd
  PARAMS ((bfd *));

void pic32_create_fill_templates
  PARAMS ((void));

static void pic32_create_unused_program_sections
  PARAMS ((struct pic32_fill_option *));

void pic32_report_crypto_sections
  PARAMS((void));

void bfd_pic32_memory_summary
  PARAMS((char *));

int fill_section_count = 0;
int dinit_size = 0;
#endif

static void gldelf32pic32mx_after_allocation (void);
static struct pic32_section *memory_region_list;
struct pic32_section *unassigned_sections;
static struct pic32_section *alloc_section_list;
static struct pic32_section *pic32_section_list;
extern unsigned int pic32_attribute_map
    PARAMS ((asection *));
extern void pic32_set_attributes(asection *,
                                          unsigned int, unsigned char );
extern void * bfd_alloc (bfd *, bfd_size_type);

static struct pic32_symbol *pic32_symbol_list;
int pic32_symbol_count;
int pic32_max_symbols;
#define PIC32_INIT_SYM_COUNT 20
#define PIC32_GROW_SYM_COUNT 10

/* defined in ldlang.c */
extern lang_statement_list_type statement_list;

/* Dynamic Memory Info */
static unsigned int stack_base, stack_limit;
static unsigned int heap_base, heap_limit;
static bfd_boolean user_defined_stack = FALSE;
static bfd_boolean heap_section_defined = FALSE;
static ATTRIBUTE_UNUSED bfd_boolean stack_section_defined = FALSE;
static bfd *heap_bfd, *stack_bfd ATTRIBUTE_UNUSED;

extern bfd *init_bfd;
static bfd *stack_bfd;
extern unsigned char *init_data;
extern asection *init_template;
extern struct pic32_section *data_sections;

/* Symbol Tables */
static asymbol **symtab;
static int symptr;

static struct pic32_memory *free_blocks, *data_memory_free_blocks;

static bfd_boolean elf_link_check_archive_element
  PARAMS ((char *, bfd *, struct bfd_link_info *));

/* Declare functions used by various EXTRA_EM_FILEs.  */

static void pic32_after_open PARAMS ((void));

unsigned force_keep_symbol
  PARAMS ((char *symbol, char *module_name));

/* Memory Reporting Info */
static const struct magic_section_description_tag magic_pmdescriptions[] =
   {{"", ""},
   {".text", "App's exec code"},
   {".rodata", "Read-only const"},
   {".sdata2", "Small initialized, constant global and static data template"},
   {".sbss2", "Small uninitialized, constant global and static data"},
   {".eh_frame_hdr", "Stack unwind header"},
   {".eh_frame", "Stack unwind info"},
   {".data", "Data-init template"},
   {".got", "Global offset table"},
   {".sdata", "Small data-init template"},
   {".lit8", "8-byte constants"},
   {".lit4", "4-byte constants"},
   {".ramfunc", "RAM-funct template"},
   {".startup", "C startup code"},
   {".app_excpt", "General-Exception"},
   {".reset", "Reset handler"},
   {".bev_excpt", "BEV-Exception"},
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
   {".sdata", "Small init data"},
   {".lit8", "8-byte constants"},
   {".lit4", "4-byte constants"},
   {".sbss", "Small uninit data"},
   {".bss", "Uninitialized data"},
   {".heap", "Dynamic Memory heap"},
   {".stack", "Min reserved for stack"},
   {".ramfunc", "RAM functions"},
   {".dbg_data", "Memory reserved for debugger exec"}};

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

static bfd_size_type total_prog_memory = 0;
static bfd_size_type total_data_memory = 0;
static bfd_size_type actual_prog_memory_used = 0;
static bfd_size_type data_memory_used = 0;
static bfd_size_type region_data_memory_used = 0;
static bfd_size_type external_memory_used = 0;

/* User-defined memory regions */
static bfd_boolean has_user_defined_memory = FALSE;
const char *memory_region_prefix = "__memory_";
static struct pic32_section *memory_region_list;
static struct pic32_section *user_memory_sections;

/* Section Lists */
static struct pic32_section *pic32_section_list;
struct pic32_section *unassigned_sections;


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

#include "../bfd/pic32-options.c"

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

  /* PR 12845: If the input section has been garbage collected it will
     not have its output section set to *ABS*.  */
  if (bfd_is_abs_section (output_section))
    return NULL;

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
  lang_add_section (&info.add, stub_sec, NULL, os);
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
  struct elf_link_hash_table *htab;

  htab = elf_hash_table (&link_info);
  if (is_elf_hash_table (htab) && is_mips_elf (link_info.output_bfd))
    _bfd_mips_elf_linker_flags (&link_info, insn32, eh_reloc_type);

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
void
bfd_pic32_report_memory_usage (fp)
     FILE *fp;
{
  bfd_size_type region_prog_memory_used;
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
    {{"kseg0_data_mem",
      "kseg0 Data-Memory Usage",
      "         Total kseg0_data_mem used"},
      {"kseg1_data_mem",
      "kseg1 Data-Memory Usage",
      "         Total kseg1_data_mem used"}
    };

  /* clear the counters */
  actual_prog_memory_used = 0;
  data_memory_used = 0;
  region_prog_memory_used = 0;
  region_data_memory_used = 0;
  total_data_memory = total_prog_memory = 0;
  
  fflush (fp);

  fprintf (fp, "\nMicrochip PIC32 Memory-Usage Report");

  /* build an ordered list of output sections */
  pic32_init_section_list(&pic32_section_list);
  bfd_map_over_sections(link_info.output_bfd, &pic32_build_section_list, NULL);

  region_count = sizeof(pmregions_to_report)/sizeof(pmregions_to_report[0]);
  magic_pm.count = sizeof(magic_pmdescriptions)/sizeof(magic_pmdescriptions[0]);
  magic_pm.start = magic_pm.index = 0;
  for (region_index = 0; region_index < region_count; region_index++)
  {  
     if (!lang_memory_region_exist(pmregions_to_report[region_index].name))
       continue;
     region = region_lookup(pmregions_to_report[region_index].name);

     /* print code header */
     fprintf( fp, "\n\n%s\n", pmregions_to_report[region_index].title);
     fprintf( fp, "section                    address  length [bytes]      (dec)  Description\n");
     fprintf( fp, "-------                 ----------  -------------------------  -----------\n");
     /* report code sections */
     for (s = pic32_section_list; s != NULL; s = s->next)
       if (s->sec)
       {
         region_prog_memory_used += bfd_pic32_report_sections (s, region, &magic_pm,fp);
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
         (unsigned long)actual_prog_memory_used, (unsigned long)actual_prog_memory_used);
  report_percent_used (actual_prog_memory_used, total_prog_memory, fp);
  fprintf (fp, "\n        --------------------------------------------------------------------------\n");

  /* DATA MEMORY */

  region_count = sizeof(dmregions_to_report)/sizeof(dmregions_to_report[0]);
  magic_dm.count = sizeof(magic_dmdescriptions)/sizeof(magic_dmdescriptions[0]);
  magic_dm.start = magic_dm.index = 0;
  for (region_index = 0; region_index < region_count; region_index++)
  {
     if (!lang_memory_region_exist(dmregions_to_report[region_index].name))
       continue;
         
     region = region_lookup(dmregions_to_report[region_index].name);
     /* print data header */
     fprintf( fp, "\n\n%s\n", dmregions_to_report[region_index].title);
     fprintf( fp, "section                    address  length [bytes]      (dec)  Description\n");
     fprintf( fp, "-------                 ----------  -------------------------  -----------\n");
     /* report data sections */
     for (s = pic32_section_list; s != NULL; s = s->next)
       if (s->sec)
       {
         region_data_memory_used += bfd_pic32_report_sections (s, region, &magic_dm, fp);
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
         (unsigned long)data_memory_used, (unsigned long)data_memory_used);
  report_percent_used (data_memory_used, total_data_memory, fp);
  fprintf( fp, "\n        --------------------------------------------------------------------------\n");


  /* Dynamic Data Memory */
#define NAME_MAX_LEN 23
#define NAME_FIELD_LEN "24"
  fprintf( fp, "\n\n%s\n", "Dynamic Data-Memory Reservation");
  fprintf( fp, "section                    address  length [bytes]      (dec)  Description\n");
  fprintf( fp, "-------                 ----------  -------------------------  -----------\n");
  fprintf( fp, "%-" NAME_FIELD_LEN "s%#10lx     %#10lx  %10ld  %s \n", "heap",
             (unsigned long)heap_base, (unsigned long)(heap_limit-heap_base),
             (unsigned long)(heap_limit-heap_base), "Reserved for heap");
  fprintf( fp, "%-" NAME_FIELD_LEN "s%#10lx     %#10lx  %10ld  %s \n", "stack",
             (unsigned long)stack_base, (unsigned long)(stack_limit-stack_base),
             (unsigned long)(stack_limit-stack_base), "Reserved for stack");
  fprintf( fp, "\n        --------------------------------------------------------------------------\n");
  

  /* 
     Report user-defined memory sections...
     They require some extra effort to organize by
     external memory region 
   */
   
  if (has_user_defined_memory) {
    struct pic32_section *r, *rnext, *s;
    const char *region_name;
    
    /* Loop through any user-defined regions */
    for (r = memory_region_list; r != NULL; r = rnext) {
        rnext = r->next;

      if (r->sec == 0) continue;
    
      region_name = r->sec->name + strlen(memory_region_prefix);
      fprintf( fp, "\nExternal Memory %s"
               "  [Origin = 0x%lx, Length = 0x%lx]\n\n",
                region_name, r->sec->vma, r->sec->lma);


      fprintf( fp, "section                    address  length [bytes]      (dec)  Description\n");
      fprintf( fp, "-------                 ----------  -------------------------  -----------\n");


      external_memory_used = 0;
      for (s = pic32_section_list; s != NULL; s = s->next)
        if (s->sec)
          bfd_pic32_report_memory_sections (region_name, s->sec, fp);


      /* -------------------- */
      /*  print data summary  */
      /* -------------------- */

      fprintf( fp, "\n        --------------------------------------------------------------------------\n");
      fprintf( fp,     "         Total External Memory used :  %#10lx  %10ld  ",
                                   (unsigned long)external_memory_used, (unsigned long)external_memory_used);
      if (external_memory_used > 0)
        report_percent_used (external_memory_used, r->sec->lma, fp);
      fprintf( fp, "\n        --------------------------------------------------------------------------\n");
    }
  }

  fflush (fp);
#undef NAME_MAX_LEN
#undef NAME_FIELD_LEN
} /* static void bfd_pic32_report_memory_usage (...)*/


void pic32_report_crypto_sections()
{
   FILE *crypto;
   struct pic32_section *s;
   char *c;
   crypto = fopen(crypto_file, "w");
   if (!crypto){
     fprintf(stderr,"Error: crypto output file cannot be opened.\n");
     xexit(1);
   }
     for (s = pic32_section_list; s != NULL; s = s->next)
       if (s->sec && (strstr(s->sec->name,"XC32$_crypto") ||
                      (strncmp(s->sec->name, ".dinit%", 7) == 0)))
       {
          c = strchr(s->sec->name, '%');
          if (c) *c = (char) '\0';
	  fprintf( crypto, "%s\t%#x\t%#x\n", s->sec->name,
                 (unsigned int)s->sec->vma, (unsigned int)s->sec->size);
       }
    fclose(crypto);
    free(crypto_file);
}

void bfd_pic32_memory_summary(char *arg) 
{
  FILE *memory_summary_file;
  
  bfd_size_type region_prog_memory_used;
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
    {{"kseg0_data_mem",
      "kseg0 Data-Memory Usage",
      "         Total kseg0_data_mem used"},
      {"kseg1_data_mem",
      "kseg1 Data-Memory Usage",
      "         Total kseg1_data_mem used"}
    };


  memory_summary_file = fopen (arg, "w");
   
  if (memory_summary_file == NULL){
     fprintf(stderr,"Warning: Could not open %s.\n", arg );
     return;
   }
  else {
     /*Calculate and output memory summary file*/

     /* clear the counters */
     actual_prog_memory_used = 0;
     data_memory_used = 0;
     region_prog_memory_used = 0;
     region_data_memory_used = 0;
     total_data_memory = total_prog_memory = 0;
  
     /* build an ordered list of output sections */
     pic32_init_section_list(&pic32_section_list);
     bfd_map_over_sections(link_info.output_bfd, &pic32_build_section_list, NULL);

     region_count = sizeof(pmregions_to_report)/sizeof(pmregions_to_report[0]);
     magic_pm.count = sizeof(magic_pmdescriptions)/sizeof(magic_pmdescriptions[0]);
     magic_pm.start = magic_pm.index = 0;
     for (region_index = 0; region_index < region_count; region_index++)
     {  
        if (!lang_memory_region_exist(pmregions_to_report[region_index].name))
          continue;

        region = region_lookup(pmregions_to_report[region_index].name);

        /* collect code sections */
        for (s = pic32_section_list; s != NULL; s = s->next)
          if (s->sec)
          {
            region_prog_memory_used += bfd_pic32_collect_section_size (s, region);
          }
 
        total_prog_memory += region->length;
        actual_prog_memory_used += region_prog_memory_used;
        region_prog_memory_used = 0;
     }
 
 
     /* DATA MEMORY */
 
     region_count = sizeof(dmregions_to_report)/sizeof(dmregions_to_report[0]);
     magic_dm.count = sizeof(magic_dmdescriptions)/sizeof(magic_dmdescriptions[0]);
     magic_dm.start = magic_dm.index = 0;
     for (region_index = 0; region_index < region_count; region_index++)
     {
        if (!lang_memory_region_exist(dmregions_to_report[region_index].name))
          continue;
          
        region = region_lookup(dmregions_to_report[region_index].name);
 
        /* collect data sections */
        for (s = pic32_section_list; s != NULL; s = s->next)
          if (s->sec)
          {
            region_data_memory_used += bfd_pic32_collect_section_size (s, region);
          }
 
        total_data_memory += region->length;
        data_memory_used += region_data_memory_used;
        region_data_memory_used = 0;
     }

     /*Do not change the output xml format as it might throw the IDE off*/

     fprintf( memory_summary_file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n");
     fprintf( memory_summary_file, "<project>                                  \n");
     fprintf( memory_summary_file, "  <executable name=\"%s\">                 \n",
                                                          output_filename ); 
     fprintf( memory_summary_file, "    <memory name=\"program\">                 \n");
     fprintf( memory_summary_file, "      <units>bytes</units>                 \n");
     fprintf( memory_summary_file, "      <length>%ld</length>                 \n",
                                          (unsigned long)total_prog_memory);
     fprintf( memory_summary_file, "      <used>%ld</used>                     \n",
                                    (unsigned long)actual_prog_memory_used);
     fprintf( memory_summary_file, "      <free>%ld</free>                     \n",
              (unsigned long)(total_prog_memory - actual_prog_memory_used));
     fprintf( memory_summary_file, "    </memory>                              \n");
     fprintf( memory_summary_file, "    <memory name=\"data\">              \n");
     fprintf( memory_summary_file, "      <units>bytes</units>                 \n");
     fprintf( memory_summary_file, "      <length>%ld</length>                 \n",
                                          (unsigned long)total_data_memory);
     fprintf( memory_summary_file, "      <used>%ld</used>                     \n",
                                          (unsigned long)data_memory_used );
     fprintf( memory_summary_file, "      <free>%ld</free>                     \n",
                     (unsigned long)(total_data_memory - data_memory_used));
     fprintf( memory_summary_file, "    </memory>                              \n");
     fprintf( memory_summary_file, "  </executable>                            \n");
     fprintf( memory_summary_file, "</project>                                 \n");
  
     fclose(memory_summary_file);
  }
}


/*
** Utility routine: bfd_pic32_report_memory_sections()
**
** - show memory usage of sections in user-defined regions
*/ 
static void
bfd_pic32_report_memory_sections (region, sect, fp) 
     const char *region;
     asection *sect;
     FILE *fp;
{
  unsigned long start = sect->vma;
  unsigned int opb = bfd_octets_per_byte (link_info.output_bfd);
  bfd_size_type total = sect->rawsize / opb; /* ignore phantom bytes */

  if (PIC32_IS_MEMORY_ATTR(sect)) {
    const char *c1, *c2;
    struct pic32_section *s;
    unsigned int n;

    /* loop through the list of memory-type sections */
    for (s = user_memory_sections; s != NULL; s = s->next) {
      if (!s->file || !s->sec) continue;
      if (strcmp((char *)s->file, region) != 0) continue;
      
      c1 = (const char *) s->sec;  /* name is stored here! */
      c2 = strchr(c1, '%');        /* avoid the %n terminator */
      n = c2 ? c2 - c1 : strlen(c1);
      if (strncmp(c1, sect->name, n) == 0) {
        char *c, *name = xmalloc(strlen(sect->name) + 1);
        strcpy(name, sect->name);  /* make a local copy of the name */
        c = strchr(name, '%');
        if (c) *c = (char) NULL;   /* remove the %n terminator */
        
        fprintf( fp, "%-24s%#10lx     %#10lx  %10ld \n", name, start, total, total);
 
        external_memory_used += total;
        free(name);
        break;
      }
    }
  }
  return;
} /* static void bfd_pic32_report_memory_sections (...)*/

/*
** Utility routine: bfd_pic32_collect_section_size()
**
** - calculate memory usage of SEC_ALLOC-flagged sections in memory
*/
static bfd_size_type
bfd_pic32_collect_section_size (s, region )
     struct pic32_section *s;
     const lang_memory_region_type *region;
{
  bfd_size_type region_used = 0;
  unsigned long start = s->sec->vma;
  unsigned long load  = s->sec->lma;
  unsigned long actual = s->sec->size;

  if (PIC32_IS_COHERENT_ATTR(s->sec)) {
    start &= 0xdfffffff;
    load &= 0xdfffffff;
  }
    
  /*
  ** report SEC_ALLOC sections in memory
  */
  if ((s->sec->flags & SEC_ALLOC)
  &&  (s->sec->size > 0))
    {
      if ((start >= region->origin) &&
          ((start + actual) <= (region->origin + region->length)))
      {
         region_used = actual;
      }
      else if ((load >= region->origin) &&
          ((load + actual) <= (region->origin + region->length)) &&
           (s->sec->flags & (SEC_HAS_CONTENTS)))
      {
         region_used = actual;
      }
    }
  return region_used;
} /* static bfd_size_type bfd_pic32_collect_section_size (...) */


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
pic32_build_section_list_vma (abfd, sect, fp)
     bfd *abfd ATTRIBUTE_UNUSED ;
     asection *sect;
     PTR fp ATTRIBUTE_UNUSED ;
{
  struct pic32_section *new, *s, *prev;
  int done = 0;

  /* Already added due to lma */
  if (sect->lma == sect->vma)
    return;

  /* create a new element */
  new = ((struct pic32_section *)
         xmalloc(sizeof(struct pic32_section)));
  if (NULL == new) {
    fprintf( stderr, "Fatal Error: Could not build section list\n");
    abort();
  }
  new->sec  = sect;
  new->attributes = pic32_attribute_map(sect);
  new->file = 0;
  new->use_vma = 1;

  /* insert it at the right spot */
  prev = pic32_section_list;
  for (s = prev; s != NULL; s = s->next)
    {
      if (s->sec && (new->sec->vma < s->sec->lma))
        {
          prev->next = new;
          new->next = s;
          if (pic32_debug)
            printf("pic32_build_section_list_vma: Adding %s LMA:%x VMA:%x\n", new->sec->name, (unsigned int)new->sec->lma, (unsigned int)new->sec->vma);
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
} /* static void pic32_build_section_list_vma (...) */

static void
pic32_build_section_list (abfd, sect, fp)
     bfd *abfd ATTRIBUTE_UNUSED ;
     asection *sect;
     PTR fp ATTRIBUTE_UNUSED ;
{
  struct pic32_section *new, *s, *prev;
  int done = 0;

  /* create a new element */
  new = ((struct pic32_section *)
         xmalloc(sizeof(struct pic32_section)));
  if (NULL == new) {
    fprintf( stderr, "Fatal Error: Could not build section list\n");
    abort();
  }
  new->sec  = sect;
  new->attributes = pic32_attribute_map(sect);
  new->file = 0;
  new->use_vma = 0;

  /* insert it at the right spot */
  prev = pic32_section_list;
  for (s = prev; s != NULL; s = s->next)
    {
      if (s->sec && (new->sec->lma < s->sec->lma))
        {
          prev->next = new;
          new->next = s;
          if (pic32_debug)
            printf("pic32_build_section_list: Adding %s LMA:%x VMA:%x\n", new->sec->name, (unsigned int)new->sec->lma, (unsigned int)new->sec->vma);
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
** Search a section list by name
*/
static bfd_boolean
pic32_name_in_section_list (lst, name)
     struct pic32_section *lst;
     const char *name; 
{
  struct pic32_section *s, *next;
  for (s = lst; s != NULL; s = next) {
    next = s->next;
    if (s->sec && strcmp(s->sec->name, name) == 0)
      return TRUE;
  }
  return FALSE;
}


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
    else if (NAME_IS(kseg0_data_mem)) {
      use_default = 1;
      region->origin = 0x80000000;     /* works with 32MX320F512L */
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
bfd_pic32_report_sections (s, region, magic_sections, fp)
     struct pic32_section *s;
     const lang_memory_region_type *region;
     struct magic_sections_tag *magic_sections;
     FILE *fp;
{
  const char *description;
  bfd_size_type region_used = 0;
  unsigned long start = s->sec->vma;
  unsigned long load  = s->sec->lma;
  unsigned long actual = s->sec->size;
  size_t name_len = 0;

  if (PIC32_IS_COHERENT_ATTR(s->sec)) {
    start &= 0xdfffffff;
    load &= 0xdfffffff;
  }
    
  /*
  ** report SEC_ALLOC sections in memory
  */
  if ((s->sec->flags & SEC_ALLOC)
  &&  (s->sec->size > 0))
    {
      char *name, *c;
#define NAME_MAX_LEN 23
#define NAME_FIELD_LEN "24"
      if (strstr(s->sec->name, "fill$")) {
        name = xmalloc(7);
        if (PIC32_IS_ABSOLUTE_ATTR(s->sec))
          strcpy(name, "FILLED");
        else
          strcpy(name, "UNUSED");
      }
      else {
      /* make a local copy of the name */
      name_len = strlen(s->sec->name);
      name = xmalloc(name_len + 1);
      if (NULL == name) {
        fprintf( stderr, "Fatal Error: Could not copy section name\n");
        abort();
      }
      if (name_len > NAME_MAX_LEN)
        name_len = NAME_MAX_LEN;
      strncpy(name, s->sec->name, name_len);
      name[name_len] = '\0';
      }
      /* clean the name of %n */
      c = strchr(name, '%');
      if (c) *c = (char) '\0';

      if ((start >= region->origin) &&
          ((start + actual) <= (region->origin + region->length)))
      {
         bfd_pic32_lookup_magic_section_description (name, magic_sections, &description);
         fprintf( fp, "%-" NAME_FIELD_LEN "s%#10lx     %#10lx  %10ld  %s \n", name,
                 start, actual, actual, description);
         region_used = actual;
      }
      else if ((load >= region->origin) &&
          ((load + actual) <= (region->origin + region->length)) &&
           (s->sec->flags & (SEC_HAS_CONTENTS)))
      {
         bfd_pic32_lookup_magic_section_description (name, magic_sections, &description);
         fprintf( fp, "%-" NAME_FIELD_LEN "s%#10lx     %#10lx  %10ld  %s \n", name,
                  load, actual, actual, description);
         region_used = actual;
      }
      free(name);
#undef NAME_MAX_LEN
#undef NAME_FIELD_LEN
    }
  return region_used;
} /* static bfd_size_type bfd_pic32_report_sections (...) */

static const char empty_string[] = "";
static const char * bfd_pic32_lookup_magic_section_description (name, magic_sections, description)
     const char *name;
     struct magic_sections_tag *magic_sections;
     const char **description;
{
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
        if (strcmp(&symbol[strlen(symbol)-2],"_s") == 0) {
          /* _0 suffix == _s suffix, then don't remove the module because the
             symbols are defined in the same object file */
          if (strcmp(&rsl->reduced_set[strlen(rsl->reduced_set)-2],"_0") == 0)
          {
            if (pic32_debug)
              fprintf(stderr,"*** Not removing %s\n", rsl->module_name);
            continue;
          }
        }

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
smartio_symbols(struct bfd_link_info *sec_info) {
  int i = 0;
  struct bfd_link_hash_entry *undefs = 0;

  /* format different for new rules - look for letter combinations
     for each function and choose the best match */
  while ( function_pairs[i].full_set )
    {  struct smartio_sym_list *l;
       struct bfd_link_hash_entry *u = 0;
       struct bfd_link_hash_entry *full;

       char suffix[] = "_aAcdeEfFgGnopsuxX";

       if (sec_info) {
         undefs=sec_info->hash->undefs;
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
           if (bfd_link_hash_lookup(sec_info->hash, buffer_map_to, 0, 0, 0) == 0) {
             struct bfd_link_hash_entry *new;

             if (pic32_debug) {
               fprintf(stderr,"Adding %s to undef\n", buffer_map_to);
               for (l = smartio_fn_list; l; l=l->next)
                 fprintf(stderr,"  because of: %s\n", l->h->root.string);
             }

             new = bfd_link_hash_lookup(sec_info->hash, buffer_map_to, 1, 1 , 1);
             new->type =  bfd_link_hash_undefined;
             bfd_link_add_undef(sec_info->hash, new);
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

/*
** Create a new symbol list
*/
static void
pic32_init_symbol_list(lst)
     struct pic32_symbol **lst;
{
  *lst = ((struct pic32_symbol *)
         xmalloc(sizeof(struct pic32_symbol) * PIC32_INIT_SYM_COUNT));
  pic32_max_symbols = PIC32_INIT_SYM_COUNT;
  pic32_symbol_count = 0;
}

/*
** Append to a symbol list
*/
static void
pic32_add_to_symbol_list(lst, name, value)
     struct pic32_symbol **lst;
     const char *name;
     bfd_vma value;
{
  if (pic32_symbol_count >= pic32_max_symbols) {
    pic32_max_symbols += PIC32_GROW_SYM_COUNT;
    *lst = ((struct pic32_symbol *)
            xrealloc( *lst, sizeof(struct pic32_symbol) * pic32_max_symbols));
  }

  (*lst)[pic32_symbol_count].name = (char *) name;
  (*lst)[pic32_symbol_count].value = value;
  ++pic32_symbol_count;
}


static bfd_boolean
bfd_pic32_process_bfd_after_open (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info ATTRIBUTE_UNUSED;
{
  asection *sec;
  const char *sym_name;
  asymbol **symbols = 0;
  bfd_boolean has_extended_attributes = FALSE;

  /*
  ** check the input file name
  */
  if (!strcmp(abfd->filename, "sbrk.o"))
  {
      pic32_heap_required = 1;
  }

   /*
  ** loop through the symbols in this bfd
  */
  {
    long size, num,i;

    size = bfd_get_symtab_upper_bound (abfd);
    if (size < 0)
      abort();

    symbols = (asymbol **) xmalloc(size);
    if (!symbols)
      abort();

    num = bfd_canonicalize_symtab (abfd, symbols);
    if (num < 0)
      abort();


    for (i=0; i<num; i++) {

      char *ext_attr_prefix = "__ext_attr_";

      /*
       * Look for extended section attributes. If we find any,
       * store the info in a list for use below, where we loop
       * through the sections in this bfd.
       */
      sym_name = bfd_asymbol_name(symbols[i]);
      if (strstr(sym_name, ext_attr_prefix)) {
        char *sec_name = (char *) &sym_name[strlen(ext_attr_prefix)];
        bfd_vma attr = bfd_asymbol_value(symbols[i]);

        if (!has_extended_attributes) {
          pic32_init_symbol_list (&pic32_symbol_list);
          has_extended_attributes = TRUE;
        }

        pic32_add_to_symbol_list(&pic32_symbol_list, sec_name, attr);
        if (pic32_debug)
          printf("    extended attributes for section %s: 0x%lx\n",
                 sec_name, attr);
      }
  }

   /*
  ** loop through the sections in this bfd
  */
  for (sec = abfd->sections; sec != 0; sec = sec->next)
  {
    /* if section has extended attributes, apply them */
    if (has_extended_attributes) {
      int c;
      for (c=0; c < pic32_symbol_count; c++) {
        if (strcmp(sec->name, pic32_symbol_list[c].name) == 0)
          pic32_set_attributes(sec, pic32_symbol_list[c].value, 0);
      }
    }

    /* if section has HEAP attribute, record that fact */
    if (PIC32_IS_HEAP_ATTR(sec) && (sec->size > 0)) {
      if (heap_section_defined)
        einfo(_("%P%X: Error: Multiple heap sections defined\n"));
      else {
        heap_section_defined = TRUE;
        heap_bfd = sec->owner;
        if ((pic32_heap_size > 0) &&
            (pic32_heap_size != (sec->size)))
          einfo(_("%P%X: Error: Size of heap requested (%d bytes)"
                  " does not match heap defined in section \'%s\'"
                  "  (%d bytes)\n"), pic32_heap_size, sec->name,
                   sec->size );
      }
    }

    struct bfd_link_hash_entry *h;
    flagword old_flags = sec->flags;
    bfd_boolean is_memory_region;
    static unsigned int region_index = 0;
    const char *region_name = 0;

    /* If section represents a memory region, record it */

    is_memory_region = FALSE;
    if (PIC32_IS_INFO_ATTR(sec) &&
        strncmp(sec->name, memory_region_prefix, 
                strlen(memory_region_prefix)) == 0 &&
	strchr(sec->name, '@') == NULL) 
  {
      region_name = sec->name + strlen(memory_region_prefix);

      is_memory_region = TRUE;
      region_index++;
      h = bfd_pic32_is_defined_global_symbol(region_name);
      if (h == NULL) 
      {
        if (pic32_debug)
          printf("  ..creating global symbol for user-defined memory %s\n",
                 region_name);

        _bfd_generic_link_add_one_symbol (info, abfd,
                                          region_name, BSF_GLOBAL,
                                          bfd_abs_section_ptr, region_index,
                                          region_name, 1, 0, 0);
      }
    }
    
    /* process input section flags here, if necessary */
    if ((sec->flags & (SEC_ALLOC | SEC_LOAD | SEC_DATA)) ==
         (SEC_ALLOC | SEC_LOAD | SEC_DATA))
      sec->flags |= SEC_HAS_CONTENTS;  /* elf linker needs this */

    /* report if flags were changed */
    if ((pic32_debug) && (sec->flags != old_flags))
      printf("   adjust flags: %s %x --> %x\n", sec->name, old_flags, sec->flags);

    if (PIC32_IS_INFO_ATTR(sec) &&
        strncmp(sec->name, memory_region_prefix, 
                strlen(memory_region_prefix)) == 0 &&
	strchr(sec->name, '@') != NULL) 
    {
 	char *ext_mem_sec_region_name = xstrdup(sec->name);
 	size_t ext_mem_sec_region_len = strlen(ext_mem_sec_region_name);
 	
	const char *atdelim = strchr(ext_mem_sec_region_name,'@');
	
	int atdelim_index  = 0;
 	int mem_prefix_len = 0;
 	
	atdelim_index  = (int) (atdelim - ext_mem_sec_region_name) ;
	mem_prefix_len = strlen(memory_region_prefix);
	
	int ext_sec_name_len = 0 ;
	int ext_reg_name_len = 0 ;

	ext_reg_name_len = atdelim_index - mem_prefix_len;
	ext_sec_name_len = ext_mem_sec_region_len - atdelim_index - 1 ;

	char *region_name = xmalloc(ext_reg_name_len+1);
	strncpy(region_name, ext_mem_sec_region_name+mem_prefix_len, ext_reg_name_len);
	region_name[ext_reg_name_len] = '\0';
	
	char *sec_name    = xmalloc(ext_sec_name_len+1);
	strncpy(sec_name, ext_mem_sec_region_name+atdelim_index+1, ext_sec_name_len);
	sec_name[ext_sec_name_len] = '\0';

        pic32_append_section_to_list (user_memory_sections,
                                       (lang_input_statement_type *) region_name,
                                       (asection *) sec_name);

	free(ext_mem_sec_region_name); 
    }
  }
  if (symbols) free(symbols);
    return(TRUE);
 }
}


unsigned int (*mchp_force_keep_symbol)(char *, char *) = force_keep_symbol;
void (*mchp_smartio_symbols)(struct bfd_link_info *) = smartio_symbols;

/*
** Create a bfd for the data init template
**
** Uses the following global variables:
**   symtab
**   symptr
*/
static bfd *
bfd_pic32_create_data_init_bfd (parent)
     bfd *parent ATTRIBUTE_UNUSED;
{
  bfd_size_type size;
  bfd *abfd;
  asection *sec;
  char *oname;
  int flags, align;

  /* create a bare-bones bfd */
  oname = (char *) bfd_alloc (link_info.output_bfd, 20);
  snprintf (oname, 20, "data_init");
  abfd = bfd_create (oname, parent);
  bfd_find_target ("${OUTPUT_FORMAT}", abfd);
  bfd_make_writable (abfd);

  bfd_set_format (abfd, bfd_object);
  bfd_set_arch_mach (abfd, bfd_arch_mips, 0);

  /* create a symbol table (room for 1 entry) */
  symptr = 0;
  symtab = (asymbol **) bfd_alloc (link_info.output_bfd, 2 * sizeof (asymbol *));

  /*
  ** create a bare-bones section for .dinit
  */
  flags = SEC_CODE | SEC_HAS_CONTENTS | SEC_IN_MEMORY | SEC_KEEP |
          SEC_LINKER_CREATED | SEC_ALLOC;
  align = 2; /* 2^2 */
  sec = bfd_pic32_create_section (abfd, ".dinit", flags, align);
  size = 0; /* will update later */
  bfd_set_section_size (abfd, sec, size);

  /* put in the symbol table */
  bfd_set_symtab (abfd, symtab, symptr);

  /* will add section contents later */

  /* finish it */
  if (!bfd_make_readable (abfd))
    {
      fprintf( stderr, "Link Error: can't make data init readable\n");
      abort();
    }

  return abfd;
} /* static bfd * bfd_pic32_create_data_init_bfd (...)*/

void pic32_create_data_init_template(void)
{
  struct pic32_section *s;
  int total_data = 0;
  asection *sec;
  /*
  ** If data init support is enabled, create a BFD
  ** for section .dinit and add it to the link.
  */
  if (pic32_data_init)
    {
      init_bfd = bfd_pic32_create_data_init_bfd (link_info.output_bfd);
      bfd_pic32_add_bfd_to_link (init_bfd, init_bfd->filename);

       /* Compute size of data init template */
      for (s = data_sections; s != NULL; s = s->next)
        if ((s->sec) && ((s->sec->flags & SEC_EXCLUDE) == 0))
          bfd_pic32_scan_data_section(s->sec, &total_data);

      total_data += 4; /* zero terminated */

      if (total_data % 16)
        total_data += 16 - (total_data % 16);

      dinit_size = total_data;

      /* allocate memory for the template */
      init_data = (unsigned char *) bfd_alloc (link_info.output_bfd, total_data);
      if (!init_data)
        {
          fprintf( stderr, "Link Error: not enough memory for data template\n");
          abort();
        }

      /* fill the template with a default value */
      init_data = memset( init_data, 0x11, total_data);

      /* attach it to the input section */
      sec = bfd_get_section_by_name(init_bfd, ".dinit");
      if (sec)
        {
          sec->size = total_data;
          sec->flags |= (SEC_HAS_CONTENTS | SEC_IN_MEMORY | SEC_LOAD | SEC_CODE | SEC_KEEP);
          sec->contents = init_data;
          bfd_set_section_size (init_bfd, sec, total_data);
          init_template = sec;  /* save a copy for later */
        }
     else
        if (pic32_debug)
          printf("after_open: section .dinit not found\n");

    } /* if (pic32_data_init) */

  else
    {
      /* warn if initial data values will be ignored */
      for (s = data_sections; s != NULL; s = s->next)
        if (s->sec)
          bfd_pic32_skip_data_section(s->sec, &total_data);
    }
}

/*
** Create a bfd for the stack
**
** Uses the following global variables:
**   symtab
**   symptr
**   pic32_stack_size
*/
static bfd *
bfd_pic32_create_stack_bfd (bfd *parent) {
  bfd_size_type size;
  bfd *abfd;
  asection *sec;
  char *oname;
  int flags, align;

  /* create a bare-bones bfd */
  oname = (char *) xmalloc (20);
  sprintf (oname, "stack");
  abfd = bfd_create (oname, parent);
  bfd_find_target ("${OUTPUT_FORMAT}", abfd);
  bfd_make_writable (abfd);

  bfd_set_format (abfd, bfd_object);
  bfd_set_arch_mach (abfd, bfd_arch_mips, 0);

  /* create a symbol table (room for 1 entry) */
  symptr = 0;
  symtab = (asymbol **) xmalloc (2 * sizeof (asymbol *));

  /* create a bare-bones section */
  flags = SEC_LINKER_CREATED | SEC_KEEP;
  align = 1;
  sec = bfd_pic32_create_section (abfd, ".stack", flags, align);
  bfd_set_section_flags (abfd, sec, flags);
  size = 8;
  bfd_set_section_size (abfd, sec, size);

  /* put in the symbol table */
  bfd_set_symtab (abfd, symtab, symptr);

  /* finish it */
  if (!bfd_make_readable (abfd)) {
      fprintf( stderr, "Link Error: can't make stack readable\n");
      abort();
    }
  /* set section flags & attributes again, because the call to
     bfd_make_readable() loses them */
     sec = abfd->sections;
     flags = SEC_LINKER_CREATED | SEC_KEEP;
     bfd_set_section_flags (abfd, sec, flags);
  return abfd;
} /* static bfd * bfd_pic32_create_stack_bfd (...)*/

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

    } /* pic32_smart_io */

    gld${EMULATION_NAME}_after_open();

    /* Prepare a list for sections in user-defined regions */
    if (user_memory_sections)
      pic32_free_section_list(&user_memory_sections);
    pic32_init_section_list(&user_memory_sections);

  {
    /* loop through all of the input bfds */
    LANG_FOR_EACH_INPUT_STATEMENT (is)
      {
        if (!bfd_pic32_process_bfd_after_open(is->the_bfd, &link_info))
          {
            einfo("Errors encountered processing file %s\n", is->filename);
          }
      }
  }

   /* init list of input data sections */
  pic32_init_section_list(&data_sections);

  /*
   * Loop through all input sections and
   * build a list of DATA or BSS type.
   */
  {
    asection * sec;
    LANG_FOR_EACH_INPUT_STATEMENT (f)
      {
        for (sec = f->the_bfd->sections;
             sec != (asection *) NULL;
             sec = sec->next)
          if (((sec->size > 0) && (PIC32_IS_DATA_ATTR(sec)))
              || (PIC32_IS_BSS_ATTR(sec)) || PIC32_IS_RAMFUNC_ATTR(sec)
              || ((sec->size > 0) && (PIC32_IS_DATA_ATTR_WITH_MEMORY_ATTR(sec)))
              || (PIC32_IS_BSS_ATTR_WITH_MEMORY_ATTR(sec))) {
            /* fix for xc32-146 */
            if (PIC32_IS_DATA_ATTR(sec) &&
                (strcmp(sec->name, ".rodata") == 0)) {
              sec->flags &= ~SEC_DATA;
              PIC32_SET_CODE_ATTR(sec);
              if (sec->owner->my_archive) {
                einfo(_("%P: Warning: %s of %s needs to be compiled"
                        " with MPLAB XC32.\n"), 
                        sec->owner->filename, sec->owner->my_archive->filename);
              }
              else {
                einfo(_("%P: Warning: %s needs to be compiled"
                        " with MPLAB XC32.\n"), sec->owner->filename);
              }
            }
            else
              pic32_append_section_to_list(data_sections, f, sec);
          }
      }
  }

  /*
  ** If a heap is required, but not provided
  ** as an input section, create one.
  **
  ** FIXME: may want to test pic32_stack_required
  ** and create a default size stack
  */
  if (!stack_section_defined && (pic32_stack_size > 0)) {
    if (pic32_debug)
      printf("\nCreating stack of size %x\n", pic32_stack_size);
    stack_bfd = bfd_pic32_create_stack_bfd (link_info.output_bfd);
    bfd_pic32_add_bfd_to_link (stack_bfd, stack_bfd->filename);
    stack_section_defined = TRUE;
  }
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
  need_layout = bfd_elf_discard_info (link_info.output_bfd,
  				      &link_info);
  /* We need to map segments here so that we determine file positions for 
     sections added by best-fit-allocator */
  gld${EMULATION_NAME}_map_segments (need_layout);

  bfd_pic32_finish();

  if (config.map_file != NULL) {
    bfd_pic32_report_memory_usage (config.map_file);
  }
  if (pic32_report_mem)
    bfd_pic32_report_memory_usage (stdout);

  if (pic32_has_crypto_option)
    pic32_report_crypto_sections();

  /* Output memory descriptor file for IDE */
  if (pic32_memory_summary)
   bfd_pic32_memory_summary(memory_summary_arg);  

  finish_default();
} /* static void pic32_finish ()*/

/* Place an orphan section.  We use this to put random SHF_ALLOC
   sections in the right segment.  */

static int
gldelf32pic32mx_place_orphan (lang_input_statement_type *file,
                              asection *sec,
                              const char *secname ATTRIBUTE_UNUSED,
                              int constraint ATTRIBUTE_UNUSED)
{
  int was_placed = 0;

  if (pic32_allocate) {  /* if best-fit allocator enabled */

    /* if this section represents a memory region,
       we don't want it allocated or its relocations
       processed. */
    if ( PIC32_IS_INFO_ATTR(sec) &&
         strncmp(sec->name, memory_region_prefix, strlen(memory_region_prefix)) == 0 &&
         strchr(sec->name, '@') == NULL) {
      if (!memory_region_list)
        pic32_init_section_list(&memory_region_list);

      if (!pic32_name_in_section_list(memory_region_list, sec->name))
        pic32_append_section_to_list(memory_region_list, file, sec);
      has_user_defined_memory = TRUE;
      return 1;
    }

   if (((sec->flags & SEC_DEBUGGING) == SEC_DEBUGGING)
        || ((sec->flags & SEC_ALLOC) == 0))
      return 0; /* let the base code handle non-alloc sections */

    if (!unassigned_sections)
      pic32_init_section_list(&unassigned_sections);

    pic32_append_section_to_list(unassigned_sections, file, sec);

    return 1;  /* and exit */
  }

   return was_placed;
} /*static int gldelf32pic32mx_place_orphan () */

/*
** Append a section to a list
*/
static void
pic32_append_section_to_list (struct pic32_section *lst,
                              lang_input_statement_type *file,
                              asection *sec)
{
  struct pic32_section *s, *tail, *new;

  if (!(lst))
    return;
  /* create a new element */
  new = ((struct pic32_section *)
         xmalloc(sizeof(struct pic32_section)));
  new->next  = 0;
  new->sec  = sec;
  new->attributes = pic32_attribute_map(sec);
  new->file = (PTR) file;

  /* find the tail */
  for (s = lst; s != NULL; s = s->next)
    tail = s;

  tail->next = new;  /* add it */
}

/*
 * See if a section name matches
 * the unique section list. Truncate
 * section names at the % char, which
 * is used to differentiate orphan sections
 * with the same names.
 */
static bfd_boolean
pic32_unique_section(const char *s)
{
  struct unique_sections *unam;
  const char *us;

  for (unam = unique_section_list; unam; unam = unam->next) {
    us = unam->name;
    while (*us) {
      if (*us++ != *s++) continue;
      if ((*s == '%') || (*s == '0'))  return TRUE;
    }
  }
  return FALSE;
}

/*
** Free a section list
*/
static void
pic32_free_section_list(lst)
     struct pic32_section **lst;
{
  struct pic32_section *s, *next;

  if (!(*lst))
    return;

  for (s = *lst; s != NULL; s = next)
    {
      next = s->next;
      free(s);
    }

  *lst = NULL;
} /* static void pic32_free_section_list (...) */

/*
** Length of a section list
*/
static int
pic32_section_list_length (lst)
     struct pic32_section *lst;
{
  struct pic32_section *s;
  int count = -1; /* first element is null */

  for (s = lst; s != NULL; s = s->next)
      count++;

  return count;

} /* pic32_section_list_length (..) */

/*
** Print a section list
*/
static void
pic32_print_section_list (lst, name)
     struct pic32_section *lst;
      const char *name;
{
  unsigned int opb = bfd_octets_per_byte (link_info.output_bfd);
  struct pic32_section *s, *prev, *next;

  printf("\nContents of %s section list at %lx:\n", name, (bfd_vma) lst);
  prev = lst;
  for (s = prev; s != NULL; s = next)
    {
      next = s->next;
      printf("  name = %s, addr = %lx, len = %lx, attr = %x\n",
             s->sec ? s->sec->name : "(none)",
             s->sec ? s->sec->lma : 0,
             s->sec ? s->sec->rawsize / opb : 0,
             s->attributes);
      prev = s;
    }
  printf("\n");

} /* pic32_print_section_list (..) */

/*
** Add a node to a memory list
**  in order of increasing size
*/
static void
pic32_add_to_memory_list (lst, addr, size)
     struct pic32_memory *lst;
     bfd_vma addr, size;
{
  struct pic32_memory *new, *b, *prev;
  int done = 0;

  /* create a new element */
  new = ((struct pic32_memory *)
         xmalloc(sizeof(struct pic32_memory)));
  new->addr  = addr;
  new->size  = size;
  new->offset = 0;

  /* insert it at the right spot */
  prev = lst;
  for (b = prev; b != NULL; b = b->next)
    {
      if (new->size < b->size)
        {
          prev->next = new;
          new->next = b;
          done++;
          break;
        }
      prev = b;
    }

  if (!done)
    {
      prev->next = new;
      new->next = NULL;
    }
} /* static void pic32_add_to_memory_list (..) */

/*
** Remove an element from a section list
*/
static void
pic32_remove_from_section_list (lst, sec)
     struct pic32_section *lst;
     struct pic32_section *sec;
{
  struct pic32_section *s, *prev, *next;
  int done = 0;

  prev = lst;
  for (s = prev; s != NULL; s = next)
    {
      next = s->next;
      if (s == sec)
        {
          prev->next = next;
          free(s);
          done = 1;
          break;
        }
      prev = s;
    }

  /*
   * If we didn't find it, search for element->section.
   * This enables us to remove an element that was
   * identified from another list.
   *
   * Example: Remove a member of "unassigned sections"
   * that was identified from an allocation list.
   */
  if (!done)
    {
      prev = lst;
      for (s = prev; s != NULL; s = next)
        {
          next = s->next;
          if (s->sec == sec->sec)
            {
              prev->next = next;
              free(s);
              done = 1;
              break;
            }
          prev = s;
        }
    }

  if (pic32_debug && (done == 0))
    printf("    !Could not remove section \"%s\" from list at %lx\n",
           sec->sec->name, (long unsigned int) &lst);

} /* pic32_remove_from_section_list (..) */


/*
** Remove a group from a section list
**
** Sections in a group are related by name.
** We can't delete them, so we clear the
** attributes field so they won't match
** any more allocation stages.
*/
static void
pic32_remove_group_from_section_list (lst)
     struct pic32_section *lst;
{
  struct pic32_section *s, *prev, *next;

  prev = lst;
  for (s = prev; s != NULL; s = next)
    {
      next = s->next;
      if (s->sec && (PIC32_IS_RAMFUNC_ATTR(s->sec))) {
          s->attributes = 0;
        }
      prev = s;
    }
} /* pic32_remove_group_from_section_list() */


/*
** Find a suitable free block - static
**
** This function scans the free memory list
** looking for a buffer that will accomodate
** a static section (fixed address).
**
** If a suitable block is found, fill in the
** offset field to reflect the section starting
** address and return the block.
**
** If a suitable block can not be found, return NULL.
*/
static struct pic32_memory *
pic32_static_assign_memory (lst, size, sec_address)
     struct pic32_memory *lst;
     bfd_vma size, sec_address;
{
  struct pic32_memory *b, *prev;
  bfd_vma offset;

  prev = lst;
  for (b = prev; b != NULL; b = b->next)
    {
      if ((b->size < size) || (b->addr > sec_address))
        continue;

      offset = sec_address - b->addr;
      if (b->size >= (offset + size))
        {
          b->offset = offset;
          return b;
        }
    }

  /*
  ** if we get here, a suitable block could not be found
  */
  return (struct pic32_memory *) NULL;
} /* static struct pic32_memory * pic32_static_assign_memory (...) */

/*
** Remove a block from a memory list
*/
static void
pic32_remove_from_memory_list (lst, block)
     struct pic32_memory *lst;
     struct pic32_memory *block;
{
  struct pic32_memory *b, *prev, *next;

  prev = lst;
  for (b = prev; b != NULL; b = next)
    {
      next = b->next;
      if (b == block)
        {
          prev->next = next;
          free(b);
          break;
        }
      prev = b;
    }

} /* static void pic32_remove_from_memory_list(..) */

/*
** Free a memory list
*/
static void
pic32_free_memory_list(lst)
     struct pic32_memory **lst;
{
  struct pic32_memory *s, *next;

  if (!(*lst))
    return;

  for (s = *lst; s != NULL; s = next)
    {
      next = s->next;
      free(s);
    }

  *lst = NULL;
} /* static void pic32_free_memory_list (...) */

/*
** Create a list of free memory blocks
**
** - first item is null
*/
static void
pic32_init_memory_list(lst)
     struct pic32_memory **lst;
{
  *lst = ((struct pic32_memory *)
         xmalloc(sizeof(struct pic32_memory)));
  (*lst)->next = 0;
  (*lst)->addr = 0;
  (*lst)->size = 0;
  (*lst)->offset = 0;
}

/*
** Add a new bfd to the link
*/
static void
bfd_pic32_add_bfd_to_link (abfd, name)
     bfd *abfd;
     const char *name;
{
  lang_input_statement_type *fake_file;
  fake_file = lang_add_input_file (name,
                                   lang_input_file_is_fake_enum,
                                   NULL);
  fake_file->the_bfd = abfd;
  ldlang_add_file (fake_file);
  if (! bfd_link_add_symbols (abfd, &link_info))
    einfo (_("%F%B: could not add symbols: %E\n"), abfd);

  return;
} /* static void bfd_pic32_add_bfd_to_link (...)*/

/*
** This routine is called by before_allocation().
**
** Scan a DATA or BSS section and accumulate
** the data template size.
*/
static void
bfd_pic32_scan_data_section (sect, p)
     asection *sect;
     PTR p;
{
#define DATA_RECORD_HEADER_SIZE 12

   int *data_size = (int *) p;

  if (p == (int *) NULL)
    return;

  /*
  ** skip persistent or noload data sections
  */
  if (PIC32_IS_PERSIST_ATTR(sect) || PIC32_IS_NOLOAD_ATTR(sect))
  {
      /*
      ** issue a warning if DATA values are present
      */
      if ((sect->flags & SEC_DATA) && (sect->size > 0))
        einfo(_("%P: Warning: initial values were specified for a non-loadable"
                " data section (%s). These values will be ignored.\n"),
              sect->name);

      if (pic32_debug)
        printf("  %s (skipped), size = %x bytes\n",
               sect->name, (unsigned int) sect->size);
      return;
  }

   /*
  ** process BSS-type sections
  */
  if ((PIC32_SECTION_IS_BSS_TYPE(sect) || PIC32_IS_BSS_ATTR_WITH_MEMORY_ATTR(sect)) &&
      (sect->size > 0))
  {
      *data_size += DATA_RECORD_HEADER_SIZE;
      if (pic32_debug)
        printf("  %s (bss), size = %x bytes, template += %x bytes\n",
               sect->name, (unsigned int) sect->size,
               DATA_RECORD_HEADER_SIZE);
  }

  /*
  ** process DATA-type sections
  */
  if ((PIC32_IS_DATA_ATTR(sect) || PIC32_IS_DATA_ATTR_WITH_MEMORY_ATTR(sect) || PIC32_IS_RAMFUNC_ATTR(sect)) && (sect->size > 0))
  {
    /* Analyze initialization data now and find out what the after compression
       size of the data initialization template */
          /* account for 0-padding so that new dinit records always start at a 
          ** new memory location 
          */
	  int count = (sect->size % 4) ? (sect->size + (4 - sect->size % 4)) \
                                       : sect->size;
          int delta = DATA_RECORD_HEADER_SIZE + count;
          *data_size += delta;
      /*
      ** make section not LOADable
      */
      sect->flags &= ~ SEC_LOAD;

      if (pic32_debug)
        printf("  %s (data), size = %x bytes, template += %x bytes\n",
               sect->name, (unsigned int) sect->size, delta);
  }
} /*static void bfd_pic32_scan_data_section (...)*/

/*
** This routine is called by before_allocation()
** when the --no-data-init option is specified.
**
** Scan for loadable DATA sections...if found,
** mark them NEVER_LOAD and issue a warning.
*/
static void
bfd_pic32_skip_data_section (sect, p)
     asection *sect;
     PTR p ATTRIBUTE_UNUSED ;
{
  /*
  ** process DATA-type sections
  */
  if (((sect->flags & SEC_DATA) == SEC_DATA) && (sect->size > 0))
  {
      /*
      ** make section not LOADable
      */
      sect->flags &= ~ SEC_LOAD;
      sect->flags |= SEC_NEVER_LOAD;

      /*
      ** issue a warning
      */
      einfo(_("%P: Warning: data initialization has been turned off,"
              " therefore section %s will not be initialized using .dinit template.\n"), sect->name);

      if (pic32_debug)
        printf("  %s (skipped), size = %x\n",
               sect->name, (unsigned int) sect->size);
  }
}

/*
** Create a new section
*/
static asection *
bfd_pic32_create_section (abfd, name, flags, align)
     bfd *abfd;
     const char *name;
     int flags;
     int align;
{
  asection *sec;
  asymbol *sym;

  if (pic32_debug)
    printf("\nCreating input section \"%s\"\n", name);

  /* do NOT call bfd_make_section_old_way(), its buggy! */
  sec = bfd_make_section_anyway (abfd, name);
  bfd_set_section_flags (abfd, sec, flags | SEC_ALLOC | SEC_KEEP | SEC_LINKER_CREATED);
  bfd_set_section_alignment (abfd, sec, align);
  /* Set the gc mark to prevent the section from being removed by garbage
     collection, despite the fact that no relocs refer to this section.  */
  sec->gc_mark = 1;
  sec->output_section = sec;

  /* add a symbol for the section name */
  sym = bfd_make_empty_symbol (abfd);
  symtab[symptr++] = sym;
  sym->name = sec->name;
  sym->section = sec;
  sym->flags = BSF_LOCAL;
  sym->value = 0;

  return sec;
} /* static asection * bfd_pic32_create_section (...)*/

/*
** Strip zero-length sections
*/
static void
pic32_strip_sections (abfd)
     bfd *abfd;
{
  asection *sec, *prev;

  if (pic32_debug)
    printf("\nLooking for zero-length sections:\n");

  sec = abfd->sections;
  if ((sec == NULL) || (sec->next == NULL))
    return;

  if (sec->flags & (SEC_KEEP | SEC_LINKER_CREATED))
    return; 
    
  prev = sec;
  sec = sec->next; /* never strip the first section */
  /* loop through the sections in this bfd */
  for (; sec != NULL; sec = sec->next)
    {
      /* remove sections with size = 0 */
      if (sec->size == 0)
      {
        prev->next = sec->next;
        if (sec->next)
          sec->next->prev = prev; 
        else
          abfd->section_last = prev; /*update the bfd->section_last field
                                       if the removed section is the 
                                       last section.*/
        abfd->section_count -= 1;
        if (pic32_debug)
          printf("  Stripping section %s\n", sec->name);
        }
      else
          prev = sec;
    }
  return;
} /* static void pic32_strip_sections (...)*/

void
bfd_pic32_finish(void)
{
  struct bfd_link_hash_entry *h;
  asection *stack_sec;
  lang_output_section_statement_type *os;
  lang_memory_region_type *region;
  bfd_vma end_data_mem;

 /*
  ** remove output sections with size = 0
  */
  pic32_strip_sections(link_info.output_bfd);
  
  /* if we've encountered a fatal error, stop here */
  if (config.make_executable == FALSE)
    einfo("%P%F: Link terminated due to previous error(s).\n");

  /* check for _min_stack_size symbol -- this is an old way to specify
     a stack, and is really not preferred */
  if ((h = bfd_pic32_is_defined_global_symbol("_min_stack_size"))) {
    if (pic32_has_stack_option && (h->u.def.value != pic32_stack_size))
      fprintf (stderr, "Warning: --stack option overrides _min_stack_size symbol\n");
    else
      pic32_stack_size = h->u.def.value;
  }

   /* if heap is required, make sure one is specified */
  if (pic32_heap_required && !heap_section_defined && !pic32_has_heap_option &&
      !bfd_pic32_is_defined_global_symbol("_min_heap_size"))
      einfo("%P%X Error: A heap is required, but has not been specified\n");

  /* check for _min_stack_size symbol -- this is an old way to specify
     a stack, and is really not preferred */
  if ((h = bfd_pic32_is_defined_global_symbol("_min_heap_size"))) {
    if (pic32_has_heap_option && (h->u.def.value != pic32_heap_size))
      fprintf (stderr, "Warning: --heap option overrides _min_heap_size symbol\n");
    else
      pic32_heap_size = h->u.def.value;
  }

  /*
  ** Check for user-defined stack
  **
  ** note: symbol (_stack) must be defined
  */
  if ((h = bfd_pic32_is_defined_global_symbol("_stack"))) {
    user_defined_stack = TRUE;
    stack_base = h->u.def.value;
    /* if defined value is relative, add in the section base address */
    if (h->u.def.section && (h->u.def.section != bfd_abs_section_ptr))
      stack_base += h->u.def.section->vma;
  }
  else
    user_defined_stack = FALSE;

  /* If a stack was not defined as a section, or by symbols,
     allocate one from remaining memory */
  if (!user_defined_stack) {
    allocate_default_stack();

  /*
  ** Set stack symbols for the runtime library
  */
      if (pic32_debug)
        printf("Creating _heap = %x\n", heap_base);
      _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_heap",
                                        BSF_GLOBAL, bfd_abs_section_ptr,
                                        heap_base, "_heap", 1, 0, 0);
      if (pic32_debug)
        printf("Creating _eheap = %x\n", heap_limit);
      _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_eheap",
                                        BSF_GLOBAL, bfd_abs_section_ptr,
                                        heap_limit, "_eheap", 1, 0, 0);

      if (pic32_debug)
        printf("Creating _splim = %x\n", stack_base);
      _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_splim",
                                        BSF_GLOBAL, bfd_abs_section_ptr,
                                        stack_base, "_splim", 1, 0, 0);
      _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_SPLIM",
                                        BSF_GLOBAL, bfd_abs_section_ptr,
                                        stack_base, "_SPLIM", 1, 0, 0);

      stack_sec = bfd_get_section_by_name(stack_bfd, ".stack");
      if (stack_sec)
        {
          etree_type *addr_tree;
          os = lang_output_section_statement_lookup (".stack", 0, TRUE);
          stack_sec->size = pic32_stack_size;
          if (os->bfd_section != NULL)
          {
            bfd_set_section_size (stack_bfd, stack_sec, pic32_stack_size);
            bfd_set_section_vma (stack_bfd, stack_sec, stack_base);
            os->bfd_section->vma = stack_sec->vma;
            os->bfd_section->lma = stack_sec->lma;
            os->bfd_section->size = stack_sec->size;
            stack_sec->flags |= SEC_LINKER_CREATED;
            os->bfd_section->flags = stack_sec->flags;
            addr_tree = xmalloc(sizeof(etree_type));
            addr_tree->value.type.node_class = etree_value;
            addr_tree->value.value = stack_sec->vma;
            os->addr_tree = addr_tree;
            if (pic32_debug)
              printf("Creating _stack = %x\n", stack_limit);
            _bfd_generic_link_add_one_symbol (&link_info, stack_sec->owner, "_stack",
                                              BSF_GLOBAL, bfd_abs_section_ptr,
                                              stack_limit, "_stack", 1, 0, 0);
                                              
          }
          else
            fprintf( stderr, "Link Error: Unable to allocate stack\n");
        }

  }

  /* Range check the stack, no matter how it was created */
  if ( (int) (stack_limit - stack_base) < (int) pic32_stack_size)
    {
      einfo("%P%X Error: Not enough memory for stack"
            " (%d bytes needed, %d bytes available)\n",
            pic32_stack_size + pic32_stackguard_size,
            (int) (stack_limit - stack_base) + pic32_stackguard_size);
    }

  if (pic32_debug)
    printf("Creating __MIN_STACK_SIZE = %x\n", pic32_stack_size);
  _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "__MIN_STACK_SIZE",
                                    BSF_GLOBAL, bfd_abs_section_ptr,
                                    pic32_stack_size, "__MIN_STACK_SIZE", 1, 0, 0);
  if (pic32_debug)
    printf("Creating _min_stack_size = %x\n", pic32_stack_size);
  _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_min_stack_size",
                                    BSF_GLOBAL, bfd_abs_section_ptr,
                                    pic32_stack_size, "_min_stack_size", 1, 0, 0);

  if (pic32_debug)
    printf("Creating __MIN_HEAP_SIZE = %x\n", pic32_heap_size);
  _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "__MIN_HEAP_SIZE",
                                    BSF_GLOBAL, bfd_abs_section_ptr,
                                    pic32_heap_size, "__MIN_HEAP_SIZE", 1, 0, 0);
  if (pic32_debug)
    printf("Creating _min_heap_size = %x\n", pic32_heap_size);
  _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_min_heap_size",
                                    BSF_GLOBAL, bfd_abs_section_ptr,
                                    pic32_heap_size, "_min_heap_size", 1, 0, 0);

  end_data_mem = stack_limit;

  if (pic32_is_l1cache_machine(global_PROCESSOR))
  {
    /* Try to use kseg0_data_mem, but fall back to kseg1_data_mem. */
    if (lang_memory_region_exist("kseg0_data_mem"))
      region = region_lookup ("kseg0_data_mem");
    else if (lang_memory_region_exist("kseg1_data_mem"))
      region = region_lookup ("kseg1_data_mem");
    else
      region = region_lookup ("kseg0_data_mem");
  }
  else
  {
    /* Try to use kseg1_data_mem, but fall back to kseg0_data_mem. */
    if (lang_memory_region_exist("kseg1_data_mem"))
      region = region_lookup ("kseg1_data_mem");
    else if (lang_memory_region_exist("kseg0_data_mem"))
      region = region_lookup ("kseg0_data_mem");
    else
      region = region_lookup ("kseg1_data_mem");
  }

  if (region) {
    end_data_mem = region->origin + region->length;
  }

  if (!bfd_pic32_is_defined_global_symbol("_ramfunc_begin"))
  {
    /* If there are no ram fumctions, add the _ramfunc_begin symbol with value 0 */
    _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_ramfunc_begin",
    BSF_GLOBAL, bfd_abs_section_ptr,
    0, "_ramfunc_begin", 1, 0, 0);
  }
  if (!bfd_pic32_is_defined_global_symbol("_bmxdkpba_address"))
  {
    _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_bmxdkpba_address",
        BSF_GLOBAL, bfd_abs_section_ptr,
        end_data_mem - region->origin, "_bmxdkpba_address", 1, 0, 0);
  }
  if (!bfd_pic32_is_defined_global_symbol("_bmxdudba_address"))
  {
    _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_bmxdudba_address",
    BSF_GLOBAL, bfd_abs_section_ptr,
    region->length, "_bmxdudba_address", 1, 0, 0);

    _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_bmxdupba_address",
    BSF_GLOBAL, bfd_abs_section_ptr,
    region->length, "_bmxdupba_address", 1, 0, 0);
  }
  if (!bfd_pic32_is_defined_global_symbol("_bmxdupba_address"))
  {
    _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_bmxdupba_address",
    BSF_GLOBAL, bfd_abs_section_ptr,
    region->length, "_bmxdupba_address", 1, 0, 0);
  }

  /*
  ** Set _dinit_addr symbol for data init template
  **   so the C startup module can find it.
  */

  if (pic32_data_init)
    {
      asection *sec;
      sec = init_template->output_section;  /* find the template's output sec */

      if (sec)
        {
          bfd_vma dinit_addr = sec->lma + init_template->output_offset;
          unsigned int dinit_address = dinit_addr & 0xFFFFFFFFul;

          if (pic32_debug)
            printf("Creating _dinit_addr= %x\n", dinit_address);
          _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd,
                                            "_dinit_addr", BSF_GLOBAL,
                                            bfd_abs_section_ptr, dinit_address,
                                            "_dinit_addr", 1, 0, 0);
          if (pic32_debug)
            printf("Creating _dinit_size= %x\n", dinit_size);
          _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd,
                                            "_dinit_size", BSF_GLOBAL,
                                            bfd_abs_section_ptr, dinit_size,
                                            "_dinit_size", 1, 0, 0);
        }
    }
}

/* include the improved memory allocation functions */
#include "../bfd/pic32-allocate.c"

static bfd_boolean
elf_link_check_archive_element (name, abfd, sec_info)
     char *name;
     bfd *abfd;
     struct bfd_link_info *sec_info;
{
  obj_attribute *archive_attr;
  obj_attribute *out_attr;

  /* we may need to pull this symbol in because it is a SMARTIO fn */
  if (mchp_force_keep_symbol)
      (void) mchp_force_keep_symbol(name, (char*)abfd->filename);

  if (!elf_known_obj_attributes_proc (link_info.output_bfd)[0].i)
    {
      /* This is the first object.  Copy the attributes.  */
      _bfd_elf_copy_obj_attributes (abfd, link_info.output_bfd);

      /* Use the Tag_null value to indicate the attributes have been
	     initialized.  */
      elf_known_obj_attributes_proc (link_info.output_bfd)[0].i = 1;

      /* Check for conflicting Tag_GNU_MIPS_ABI_FP attributes and merge
         non-conflicting ones.  */
      archive_attr = elf_known_obj_attributes (abfd)[OBJ_ATTR_GNU];
      out_attr = elf_known_obj_attributes (link_info.output_bfd)[OBJ_ATTR_GNU];

      if (pic32_has_hardfloat_option || 
         (global_PROCESSOR && pic32_is_fltpt_machine(global_PROCESSOR) && !pic32_has_softfloat_option ))
      {
        out_attr[Tag_GNU_MIPS_ABI_FP].i = 4;
        if (4 != archive_attr[Tag_GNU_MIPS_ABI_FP].i)
          return FALSE;
      }

      if (pic32_has_softfloat_option || 
         (global_PROCESSOR && !pic32_is_fltpt_machine(global_PROCESSOR) && !pic32_has_hardfloat_option ))
      {
        out_attr[Tag_GNU_MIPS_ABI_FP].i = 3;
        if (3 != archive_attr[Tag_GNU_MIPS_ABI_FP].i)
          return FALSE;
      }
    }
  return TRUE;
}

static void
gldelf32pic32mx_after_allocation (void)
{
#ifdef TARGET_IS_PIC32MX
   /* If any sections are discarded then relocations of symbols referenced
      in sections like .eh_frame, .stab, .pdr will be removed. Also entries 
      these sections may be removed to eleminate duplication. 
      So the size will change. If we don't map_segments before 
      best-fit-allocation overlapping may occur. */
   need_layout = bfd_elf_discard_info (link_info.output_bfd,
                                                  &link_info);

   gld${EMULATION_NAME}_map_segments (need_layout);
#endif
/*
** Invoke the best-fit allocator
*/
  if (pic32_allocate) {
    allocate_memory();
  }

#ifdef TARGET_IS_PIC32MX
  if (pic32_has_fill_option)
    pic32_create_fill_templates();
#endif
}

/*
** Return a pointer to bfd_link_hash_entry
** if a global symbol is defined;
** else return zero.
*/
struct bfd_link_hash_entry *
bfd_pic32_is_defined_global_symbol (name)
     const char *const name;
{
    struct bfd_link_hash_entry *h;
    h = bfd_link_hash_lookup (link_info.hash, name, FALSE, FALSE, TRUE);

    if ((h != (struct bfd_link_hash_entry *) NULL) &&
        (h->type == bfd_link_hash_defined))
      return h;
    else
      return (struct bfd_link_hash_entry *) NULL;
}

bfd_boolean (*mchp_elf_link_check_archive_element)(char *name, bfd *abfd,
                                                    struct bfd_link_info *) =
  elf_link_check_archive_element;

/*
** Create a bfd for the fill templates
**
** Uses the following global variables:
**   symtab
**   symptr
*/
static bfd *
bfd_pic32_create_fill_bfd (parent)
     bfd *parent ATTRIBUTE_UNUSED;
{
  bfd_size_type size;
  bfd *abfd;
  asection *sec;
  char *oname;
  int flags, sec_align;
  const char *sname;

  /* create a bare-bones bfd */
  oname = (char *) bfd_alloc (link_info.output_bfd, 20);
  sprintf (oname, "fill%d", fill_section_count);
  abfd = bfd_create (oname, parent);
  bfd_find_target ("${OUTPUT_FORMAT}", abfd);
  bfd_make_writable (abfd);

  bfd_set_format (abfd, bfd_object);
  bfd_set_arch_mach (abfd, bfd_arch_mips, 0);

  /* create a symbol table (room for 1 entry) */
  symptr = 0;
  symtab = (asymbol **) bfd_alloc (link_info.output_bfd, 2 * sizeof (asymbol *));

  /*
  ** create a bare-bones section for
  */
  /* get unique section name */
  sname = bfd_get_unique_section_name (abfd, "fill$", &fill_section_count);
  flags = SEC_CODE | SEC_HAS_CONTENTS | SEC_IN_MEMORY | SEC_KEEP |
          SEC_LINKER_CREATED;
  sec_align = 0; 
  sec = bfd_pic32_create_section (abfd, sname, flags, sec_align);
  size = 0; /* will update later */
  bfd_set_section_size (abfd, sec, size);

  /* put in the symbol table */
  bfd_set_symtab (abfd, symtab, symptr);
  /* finish it */
  if (!bfd_make_readable (abfd))
    {
      fprintf( stderr, "Link Error: can't make fill readable\n");
      abort();
    }
  return abfd;
} /* static bfd * bfd_pic32_create_fill_bfd (...)*/

void pic32_create_fill_templates(void)
{
  struct pic32_fill_option *o;
  if (program_memory_free_blocks)
    {
      if (pic32_fill_option_list)
        {
         for (o = pic32_fill_option_list; o != NULL; o = o->next)
            {
              if (o == pic32_fill_option_list)
                continue;

              if (o->loc.unused)
                pic32_create_unused_program_sections(o);
            }

        }
     }
   else
     einfo(_("%P: Warning: There is not any free program memory to fill.\n"));
}

static void pic32_create_unused_program_sections
                      (struct pic32_fill_option *opt)
{
  asection * sec;
  struct pic32_memory *b, *next;
  bfd *fill_bfd;
  unsigned char *fill_data;
  struct pic32_section *s;
  struct memory_region_struct *region;
  bfd_vma size = 0;
  bfd_vma addr = 0;
  int sec_align = 2;

  for (b = program_memory_free_blocks; b != NULL; b = next)
    {
     next = b->next;
     if ((b->addr == 0) && (b->size == 0))
      continue;
     fill_bfd = bfd_pic32_create_fill_bfd(link_info.output_bfd);
     bfd_pic32_add_bfd_to_link (fill_bfd, fill_bfd->filename);
     
     addr = align_power(b->addr, sec_align); /*align to an instruction word*/
     size = b->size - (addr - b->addr);
    

     fill_bfd->sections->vma = fill_bfd->sections->lma = addr;
     fill_bfd->sections->rawsize = size;
     fill_bfd->sections->size = size;

       /* create a pic32_section */
     pic32_init_section_list(&s);
     s->sec = fill_bfd->sections;
     LANG_FOR_EACH_INPUT_STATEMENT (file)
      {
       if (strcmp(file->filename, fill_bfd->filename) == 0)
        s->file = (PTR) file;
      }
       /* allocate memory for the template */
     fill_data = (unsigned char *) bfd_alloc (link_info.output_bfd, size);
     if (!fill_data)
      {
       fprintf( stderr, "Link Error: not enough memory for fill template\n");
       abort();
      }

       /* fill the template with a default value */
     fill_data = memset(fill_data, 0x11, size);

       /* attach it to the input section */
     sec = bfd_get_section_by_name(fill_bfd, fill_bfd->sections->name);
     if (sec)
      {
       sec->rawsize = size;
       sec->flags |= (SEC_HAS_CONTENTS | SEC_IN_MEMORY | SEC_LOAD | SEC_CODE);
       sec->contents = fill_data;
       bfd_set_section_alignment(fill_bfd, sec, sec_align); /*align to an insn word*/
       bfd_set_section_size (fill_bfd, sec, size);
       fill_bfd->sections = sec;  /* save a copy for later */
      }
 else
       if (pic32_debug)
         printf("after_open: section %s not found\n", sec->name);

     region = region_lookup ("kseg0_program_mem");

     update_section_info(addr, s, region);

     pic32_append_section_to_list(opt->fill_section_list, 
                                  (lang_input_statement_type *)s->file, sec);
     pic32_remove_from_memory_list(program_memory_free_blocks, b);
    }

}

void pic32_create_specific_fill_sections(void)
{
  asection * sec;
  bfd *fill_bfd;
  unsigned char *fill_data;
  bfd_vma size;
  bfd_vma addr;
  struct pic32_fill_option *o;

  for (o = pic32_fill_option_list->next; o != NULL; o = o->next)
    {
       if (o->loc.unused)
         continue;

       addr = o->loc.address;

       if (o->loc.end_address)
        size = (o->loc.end_address - o->loc.address) + 1;
       else if (o->fill_width == 1)
        size = 1;
       else if (o->fill_width == 2)
        size = 2;
       else if (o->fill_width == 8)
        size = 8;
       else
        size = 4;

       fill_bfd = bfd_pic32_create_fill_bfd(link_info.output_bfd);
       bfd_pic32_add_bfd_to_link (fill_bfd, fill_bfd->filename);

       fill_bfd->sections->vma = fill_bfd->sections->lma = o->loc.address;
       fill_bfd->sections->rawsize = size;
       fill_bfd->sections->size = size;

        /* allocate memory for the template */
       fill_data = (unsigned char *) bfd_alloc (link_info.output_bfd, size);
       if (!fill_data)
        {
         fprintf( stderr, "Link Error: not enough memory for fill template\n");
         abort();
        }

        /* fill the template with a default value */
       fill_data = memset(fill_data, 0x11, size);

        /* attach it to the input section */
       sec = bfd_get_section_by_name(fill_bfd,fill_bfd->sections->name );
       if (sec)
       {
        sec->rawsize = size;
        sec->flags |= (SEC_HAS_CONTENTS | SEC_IN_MEMORY | SEC_LOAD | SEC_CODE);
        PIC32_SET_ABSOLUTE_ATTR(sec);
        sec->contents = fill_data;
        bfd_set_section_size (fill_bfd, sec, size);
        fill_bfd->sections = sec;  /* save a copy for later */
       }
      else
        if (pic32_debug)
          printf("after_open: section %s not found\n", sec->name);

      pic32_append_section_to_list(o->fill_section_list,
                                 (lang_input_statement_type *)NULL, sec);
   }
}

EOF


# Define some shell vars to insert bits of code into the standard elf
# parse_args and list_options functions.
#
PARSE_AND_LIST_PROLOGUE='
#define OPTION_INSN32			301
#define OPTION_NO_INSN32		(OPTION_INSN32 + 1)
#define OPTION_PCREL_EH_RELOC		303
'

PARSE_AND_LIST_LONGOPTS='
  { "insn32", no_argument, NULL, OPTION_INSN32 },
  { "no-insn32", no_argument, NULL, OPTION_NO_INSN32 },
  { "pcrel-eh-reloc", no_argument, NULL, OPTION_PCREL_EH_RELOC },
'

PARSE_AND_LIST_OPTIONS='
  fprintf (file, _("\
  --insn32                    Only generate 32-bit microMIPS instructions\n"
		   ));
  fprintf (file, _("\
  --no-insn32                 Generate all microMIPS instructions\n"
		   ));
'

PARSE_AND_LIST_ARGS_CASES='
    case OPTION_INSN32:
      insn32 = TRUE;
      break;

    case OPTION_NO_INSN32:
      insn32 = FALSE;
      break;

    case OPTION_PCREL_EH_RELOC:
      eh_reloc_type = R_MIPS_PC32;
      break;
'

LDEMUL_AFTER_OPEN=pic32_after_open
LDEMUL_FINISH=pic32_finish
LDEMUL_PARSE_ARGS=gldelf32pic32mx_parse_args
LDEMUL_LIST_OPTIONS=gldelf32pic32mx_list_options
LDEMUL_AFTER_PARSE=mips_after_parse
LDEMUL_BEFORE_ALLOCATION=mips_before_allocation
LDEMUL_PLACE_ORPHAN=gldelf32pic32mx_place_orphan
LDEMUL_CREATE_OUTPUT_SECTION_STATEMENTS=mips_create_output_section_statements
LDEMUL_AFTER_ALLOCATION=gldelf32pic32mx_after_allocation

