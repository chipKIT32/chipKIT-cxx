/* Subroutines used for Microchip PIC32 MCU support
   Copyright (C) 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998
   1999, 2000, 2001, 2002, 2003, 2004, 2005, 2007, 2008, 2009
   Free Software Foundation, Inc.
   Contributed by J. Grosbach, james.grosbach@microchip.com, and
   T. Kuhrt, tracy.kuhrt@microchip.com
   Changes by J. Kajita, jason.kajita@microchip.com,
   G. Loegel, george.loegel@microchip.com and
   S. Bekal, swaroopkumar.bekal@microchip.com

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef TARGET_IS_PIC32MX 
#define TARGET_IS_PIC32MX 
#endif

#ifndef PIC32
#define PIC32
#endif

#ifndef _BUILD_C32_
#define _BUILD_C32_
#endif

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "cppdefault.h"
#include "tm.h"
#include <signal.h>
#include "rtl.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "insn-attr.h"
#include "recog.h"
#include "toplev.h"
#include "output.h"
#include "tree.h"
#include "function.h"
#include "expr.h"
#include "optabs.h"
#include "libfuncs.h"
#include "flags.h"
#include "reload.h"
#include "tm_p.h"
#include "ggc.h"
#include "gstab.h"
#include "hashtab.h"
#include "debug.h"
#include "target.h"
#include "target-def.h"
#include "langhooks.h"
#include "sched-int.h"
#include "gimple.h"
#include "bitmap.h"
#include "diagnostic.h"
#include "params.h"
#include "prefix.h"
#include "bversion.h"
#include "opts.h"

#include <stdio.h>
#include <string.h>
#include "cpplib.h"
#include "mchp-protos.h"
#include "mchp-pragmas.h"
#include "c-family/c-pragma.h"
#include "incpath.h"
#include "cppdefault.h"
#include "hwint.h"
#include "config/mips/mips-machine-function.h"
#include "version.h"

/* splitting for lto/non-lto causes beacoup problems */
#include "../../../../c30_resource/src/xc32/resource_info.h"
#include "config/mchp-cci/cci.h"
#if !defined(SKIP_LICENSE_MANAGER)
/* include file */
#include "config/mchp-cci/xclm_public.h"
#include "config/mchp-cci/mchp_sha.h"
#endif

#ifdef __MINGW32__
void *alloca(size_t);
#else
#include <alloca.h>
#endif

#define xstr(s) str(s)
#define str(s) #s

#define const_section readonly_data_section
#define dconst_section readonly_data_section

/* Global Variables */
 //const char *mchp_resource_file = NULL;
 char *mchp_resource_file_f;  /* stoopid options.c always makes this const */
//extern const char * mchp_text_scn;
SECTION_FLAGS_INT mchp_text_flags = SECTION_CODE;
extern bool user_defined_section_attribute;

extern bool bss_initializer_p (const_tree decl);

extern cpp_options *cpp_opts;

#ifndef MCHP_XCLM_FREE_LICENSE
 #define MCHP_XCLM_FREE_LICENSE 0x0
 #warning MCHP_XCLM_FREE_LICENSE not defined by API
#endif

int          mchp_profile_option = 0;

unsigned int mchp_pragma_align = 0;
unsigned int mchp_pragma_inline = 0;
unsigned int mchp_pragma_keep = 0;
unsigned int mchp_pragma_coherent = 0;
unsigned int mchp_pragma_printf_args = 0;
unsigned int mchp_pragma_scanf_args = 0;
tree mchp_pragma_section = NULL_TREE;
typedef struct pic32_external_memory {
  tree decl;
  char *name;
  int size;
  unsigned int origin;
  struct pic32_external_memory *next;
} pic32_external_memory;

static pic32_external_memory *pic32_external_memory_head,
                             *pic32_external_memory_tail;


/* Local Variables */

#ifndef BITSET_P
/* True if bit BIT is set in VALUE.  */
#define BITSET_P(VALUE, BIT) (((VALUE) & (1 << (BIT))) != 0)
#endif

#define SECTION_FLAG_EXEC       "x"
#define SECTION_FLAG_DATA       "d"
#define SECTION_FLAG_BSS        "b"
#define SECTION_FLAG_READONLY   "r"

#define SECTION_NAME_BSS          ".bss"
#define SECTION_NAME_NBSS         ".sbss"
#define SECTION_NAME_SBSS         ".sbss"
#define SECTION_NAME_DATA         ".data"
#define SECTION_NAME_NDATA        ".sdata"
#define SECTION_NAME_SDATA        ".sdata"
#define SECTION_NAME_CONST        ".rodata"
#define SECTION_NAME_RAMFUNC      ".ramfunc"
#define SECTION_NAME_PBSS         ".pbss"
#define SECTION_NAME_INIT         ".init"
#define SECTION_NAME_FINI         ".fini"
#define SECTION_NAME_CTORS        ".ctors"
#define SECTION_NAME_DTORS        ".dtors"
#define SECTION_NAME_INIT_ARRAY   ".init_array"
#define SECTION_NAME_FINI_ARRAY   ".fini_array"

#define JOIN2(X,Y) (X ## Y)
#define JOIN(X,Y) JOIN2(X,Y)
#define MCHP_ULL(X) JOIN2(X,ULL)

/* 0x100000 */
#define SECTION_READ_ONLY       (MCHP_ULL(SECTION_MACH_DEP))
#define SECTION_CONST           (MCHP_ULL(SECTION_MACH_DEP))
#define SECTION_RAMFUNC         (MCHP_ULL(SECTION_MACH_DEP) << 1ull)
#define SECTION_NEAR            (MCHP_ULL(SECTION_MACH_DEP) << 2ull)
#define SECTION_PERSIST         (MCHP_ULL(SECTION_MACH_DEP) << 3ull)
#define SECTION_NOLOAD          (MCHP_ULL(SECTION_MACH_DEP) << 4ull)
#define SECTION_INFO            (MCHP_ULL(SECTION_MACH_DEP) << 5ull)
#define SECTION_ADDRESS         (MCHP_ULL(SECTION_MACH_DEP) << 6ull)
#define SECTION_ALIGN           (MCHP_ULL(SECTION_MACH_DEP) << 7ull)
#define SECTION_KEEP            (MCHP_ULL(SECTION_MACH_DEP) << 8ull)
#define SECTION_COHERENT        (MCHP_ULL(SECTION_MACH_DEP) << 9ull)
#define SECTION_REGION          (MCHP_ULL(SECTION_MACH_DEP) << 10ull)

/* the attribute names from the assemblers point of view */
#define SECTION_ATTR_ADDRESS   "address"
#define SECTION_ATTR_ALIGN     "align"
#define SECTION_ATTR_BSS       "bss"
#define SECTION_ATTR_CODE      "code"
#define SECTION_ATTR_CONST     "code"
#define SECTION_ATTR_DATA      "data"
#define SECTION_ATTR_INFO      "info"
#define SECTION_ATTR_NEAR      "near"
#define SECTION_ATTR_NOLOAD    "noload"
#define SECTION_ATTR_PERSIST   "persist"
#define SECTION_ATTR_RAMFUNC   "ramfunc"
#define SECTION_ATTR_DEFAULT   "unused"
#define SECTION_ATTR_KEEP      "keep"
#define SECTION_ATTR_COHERENT  "coherent"
#define SECTION_ATTR_REGION    "memory"

struct valid_section_flags_
{
  const char *flag_name;
  char single_letter_equiv;
  unsigned long long flag_mask;
  unsigned long long incompatible_with;
} valid_section_flags[] =
{
  {
    SECTION_ATTR_ADDRESS, 0,
    SECTION_ADDRESS, SECTION_ALIGN | SECTION_INFO
  },
  {
    SECTION_ATTR_ALIGN, 0,
    SECTION_ALIGN,   SECTION_ADDRESS |
    SECTION_INFO
  },
  {
    SECTION_ATTR_BSS, 'b',
    SECTION_BSS,     SECTION_CODE | SECTION_WRITE | SECTION_PERSIST |
    SECTION_READ_ONLY
  },
  {
    SECTION_ATTR_CODE, 'x',
    SECTION_CODE,     SECTION_WRITE | SECTION_BSS |
    SECTION_NEAR |
    SECTION_PERSIST |
    SECTION_READ_ONLY
  },
  {
    SECTION_ATTR_CONST, 'r',
    SECTION_READ_ONLY,SECTION_CODE | SECTION_WRITE | SECTION_BSS |
    SECTION_NEAR |
    SECTION_INFO
  },
  {
    SECTION_ATTR_COHERENT, 0,
    SECTION_COHERENT,
    0
  },
  {
    SECTION_ATTR_DATA, 'd',
    SECTION_WRITE,    SECTION_BSS | SECTION_PERSIST |
    SECTION_READ_ONLY
  },

  {
    SECTION_ATTR_INFO, 0,
    SECTION_INFO,     SECTION_PERSIST |
    SECTION_ADDRESS | SECTION_NEAR |
    SECTION_ALIGN |
    SECTION_NOLOAD | SECTION_MERGE |
    SECTION_READ_ONLY
  },
  {
    SECTION_ATTR_KEEP, 0,
    SECTION_KEEP,
    0
  },
  {
    SECTION_ATTR_REGION, 0,
    SECTION_REGION,
    0
  },
  {
    SECTION_ATTR_NOLOAD, 0,
    SECTION_NOLOAD,   SECTION_MERGE | SECTION_INFO
  },
  {
    SECTION_ATTR_PERSIST, 'b',
    SECTION_PERSIST,  SECTION_CODE | SECTION_WRITE | SECTION_BSS |
    SECTION_MERGE |
    SECTION_INFO | SECTION_READ_ONLY
  },
  {
    SECTION_ATTR_RAMFUNC, 0,
    SECTION_RAMFUNC,     SECTION_WRITE | SECTION_BSS |
    SECTION_NEAR |
    SECTION_PERSIST |
    SECTION_READ_ONLY
  },
  { 0, 0, 0, 0},
};

struct reserved_section_names_
{
  const char *section_name;
  SECTION_FLAGS_INT mask;
} reserved_section_names[] =
{
  { ".bss",    SECTION_BSS },
  { ".rodata",  SECTION_READ_ONLY },
  { ".rodata1",  SECTION_READ_ONLY },
  { ".data",   SECTION_WRITE },
  { ".dconst", SECTION_WRITE },
  { ".sbss",   SECTION_BSS | SECTION_NEAR },
  { ".sdata",  SECTION_WRITE | SECTION_NEAR },
  { ".sdconst",SECTION_WRITE | SECTION_NEAR },
  { ".pbss",   SECTION_PERSIST },
  { ".text",   SECTION_CODE },
  { ".ramfunc", SECTION_RAMFUNC },
  { ".gnu.linkonce.d", SECTION_WRITE },
  { ".gnu.linkonce.t", SECTION_CODE },
  { ".gnu.linkonce.r", SECTION_READ_ONLY },
  { ".gnu.linkonce.s2", SECTION_READ_ONLY },
  { ".gnu.linkonce.sb2", SECTION_READ_ONLY },
  { ".got", SECTION_WRITE },
  { ".got.plt", SECTION_WRITE },
  { ".gnu.linkonce.s", SECTION_WRITE | SECTION_NEAR },
  { ".gnu.linkonce.sb", SECTION_BSS | SECTION_NEAR },
  { ".dynsbss", SECTION_BSS | SECTION_NEAR },
  { ".scommon", SECTION_BSS | SECTION_NEAR },
  { ".dynbss", SECTION_BSS },
  { 0, 0},
};

static const char *mchp_default_section = "*";
static char this_default_name[sizeof("*_012345670123456701234567")];
static time_t current_time = 0;
static int lfInExecutableSection = FALSE;

static int pic32_num_register_sets = 2;
static unsigned int pic32_procid = 0;
static bool pic32_loaded_device_mask = false;
static unsigned int pic32_device_mask = HAS_MIPS32R2 | HAS_MIPS16 | HAS_MICROMIPS;

const char * mchp_target_cpu = NULL;
char * mchp_target_cpu_id;

enum
{
  ss_pushed = 0,     /* section stack was pushed */
  ss_set = 1,        /* section stack was set */
  ss_should_pop = 2  /* popped, but wait til we see what is pushed next */
};

typedef struct sectionStack_
{
  const char * pszName;
  unsigned int pszFlag;
  struct sectionStack_ *pop;
  SECTION_FLAGS_INT flags;
} sectionStack;

enum css
{
  css_pop = 0,
  css_push = 1,
  css_activate = 2,
  css_tos = 3
};

static sectionStack default_section =
{
  ".text.default_section", SECTION_CODE , 0x0, 0x0
};

static sectionStack *lSectionStack = &default_section;
static sectionStack *freeSectionStack;

/* The maximum distance between the top of the stack frame and the
   value $sp has when we save and restore registers.

   The value for normal-mode code must be a SMALL_OPERAND and must
   preserve the maximum stack alignment.  We therefore use a value
   of 0x7ff0 in this case.

   MIPS16e SAVE and RESTORE instructions can adjust the stack pointer by
   up to 0x7f8 bytes and can usually save or restore all the registers
   that we need to save or restore.  (Note that we can only use these
   instructions for o32, for which the stack alignment is 8 bytes.)

   We use a maximum gap of 0x100 or 0x400 for MIPS16 code when SAVE and
   RESTORE are not available.  We can then use unextended instructions
   to save and restore registers, and to allocate and deallocate the top
   part of the frame.  */
#ifndef MIPS_MAX_FIRST_STACK_STEP
#define MIPS_MAX_FIRST_STACK_STEP     \
  (!TARGET_MIPS16 ? 0x7ff0      \
   : GENERATE_MIPS16E_SAVE_RESTORE ? 0x7f8    \
   : TARGET_64BIT ? 0x100 : 0x400)
#endif

#if 1 /* TODO */

enum mips_function_type_tag current_function_type;

typedef struct cheap_rtx_list {
  tree t;
  rtx x;
  int flag;
  struct cheap_rtx_list *next;
} cheap_rtx_list;

typedef enum mchp_interesting_fn_info_
{
  info_invalid,
  info_I,
  info_O,
  info_O_v,
  info_dbl
} mchp_interesting_fn_info;

typedef enum mchp_conversion_status_
{
  conv_state_unknown,
  conv_possible,
  conv_indeterminate,
  conv_not_possible,
  conv_c =  0x000080,
  conv_d =  0x000100,
  conv_i =  0x000100,
  conv_e =  0x000200,
  conv_E =  0x000400,
  conv_f =  0x000800,
  conv_g =  0x001000,
  conv_G =  0x002000,
  conv_n =  0x004000,
  conv_o =  0x008000,
  conv_p =  0x010000,
  conv_s =  0x020000,
  conv_u =  0x040000,
  conv_x =  0x080000,
  conv_X =  0x100000,
  conv_a =  0x200000,
  conv_A =  0x400000,
  conv_F =  0x800000
} mchp_conversion_status;

typedef struct mchp_intersting_fn_
{
  const char *name;
  const char *map_to;
  mchp_interesting_fn_info conversion_style;
  int interesting_arg;
  unsigned int function_convertable;
  mchp_conversion_status conv_flags;
  char *encoded_name;
} mchp_interesting_fn;

/* The nesting depth of .set noat */
extern struct mips_asm_switch mips_noat;

static HOST_WIDE_INT mchp_interrupt_priority;
static HOST_WIDE_INT mchp_isr_backcompat = 0;

static HOST_WIDE_INT mchp_offset_status = 0;
static HOST_WIDE_INT mchp_offset_epc = 0;
static HOST_WIDE_INT mchp_offset_srsctl = 0;
static HOST_WIDE_INT mchp_offset_dspcontrol = 0;
static HOST_WIDE_INT mchp_offset_fcsr = 0;

static HOST_WIDE_INT mchp_invalid_ipl_warning = 0;
#ifdef SKIP_LICENSE_MANAGER
#if defined(MCHP_XCLM_VALID_CPP_FULL)
HOST_WIDE_INT mchp_pic32_license_valid = MCHP_XCLM_VALID_CPP_FULL;
#else
HOST_WIDE_INT mchp_pic32_license_valid = 0x6;
#endif
#else
HOST_WIDE_INT mchp_pic32_license_valid = -1;
#endif

static void push_cheap_rtx(cheap_rtx_list **l, rtx x, tree t, int flag);
static rtx pop_cheap_rtx(cheap_rtx_list **l, tree *t, int *flag);

static tree mchp_function_interrupt_p (tree decl);
static int mchp_function_naked_p (tree func);
static int mchp_persistent_p (tree func);
static int mchp_keep_p (tree decl);
static int mchp_coherent_p (tree decl);

static int mchp_vector_attribute_compound_expr (tree *node, tree expr,
    bool *no_add_attrs);
int mchp_epilogue_uses (unsigned regno);

static void mchp_output_configuration_words (void);
static void mchp_output_vector_dispatch_table (void);
static void mchp_output_ext_region_memory_info (void);
static int mchp_vector_attribute_compound_expr (tree *node, tree expr,
    bool *no_add_attrs);
inline int
mchp_register_interrupt_context_p (unsigned regno);
static mchp_interesting_fn *mchp_match_conversion_fn(const char *name);
static const char *default_section_name(tree decl, SECTION_FLAGS_INT flags);
static void    mchp_merged_asm_named_section(const char *name,
    SECTION_FLAGS_INT flags);
static
 char * mchp_get_named_section_flags (const char *pszSectionName,
    SECTION_FLAGS_INT flags);
int set_section_stack(const char *pszSectionName,
                      SECTION_FLAGS_INT pszSectionFlag);
#if 1
void mchp_push_section_name(const char *pszSectionName,
                            SECTION_FLAGS_INT pszSectionFlag);
#endif
#if 1
static tree mchp_push_pop_constant_section(tree decl, enum css push,
    const char **section_name);
static void mchp_pop_section_name(void);
#endif
static tree get_mchp_space_attribute (tree decl);
static tree get_mchp_region_attribute (tree decl);

extern unsigned int mips_get_compress_mode (tree decl);
extern void mips_set_compression_mode (unsigned int compression_mode);
extern void mips_for_each_saved_gpr_and_fpr (HOST_WIDE_INT sp_offset, mchp_save_restore_fn fn, rtx skip_label);
extern void mips_restore_reg (rtx reg, rtx mem);
extern bool mips_interrupt_type_p (tree type);

SECTION_FLAGS_INT mchp_section_type_flags(tree decl, const char *name, int reloc ATTRIBUTE_UNUSED);
void mchp_add_vector_dispatch_entry (const char *target_name, int vector_number, bool fartype, 
                                     enum pic32_isa_mode isamode, int emit);

struct vector_dispatch_spec *vector_dispatch_list_head;

struct mchp_config_specification *mchp_configuration_values;

#if !defined(MCHP_SKIP_RESOURCE_FILE)
/* This will be called by the spec parser in gcc.c when it sees
   a %:local_cpu_detect(args) construct.
 */

static const char* mchp_get_resource_file_path(void);
static const char* mchp_get_resource_file_path(void)
{
  extern struct cl_decoded_option *save_decoded_options;
  /* TODO Correct this path */
#if 0
  char* path = make_relative_prefix(save_argv[0], "/bin/bin", "/bin");
#endif
  char* path = make_relative_prefix(save_decoded_options[0].arg,
                                  "/pic32mx/bin/gcc/pic32mx/"
                                  str(BUILDING_GCC_MAJOR) "."
                                  str(BUILDING_GCC_MINOR) "."
                                  str(BUILDING_GCC_PATCHLEVEL),
                                  "/bin");

#if defined(__MINGW32__)
  {
    char *convert;
    convert = path;
    while (*convert != '\0')
      {
        if (*convert == '\\')
          *convert = '/';
        convert++;
      }
  }
#endif
  
  return path;
}

#if 1
static unsigned int mchp_load_resource_info(char *id, char **matched_id, const char* mchp_resource_file_in);
static unsigned int mchp_load_resource_info(char *id, char **matched_id, const char* mchp_resource_file_in) {
  struct resource_introduction_block *rib;
  struct resource_data d;
  int version;
  unsigned int mask;
  char *Microchip;
  char *new_version = xstrdup(version_string);
  char *version_part1;
  char *version_part2;
  int mismatch=0;
  char *match;
  char *device_buf;
  char *mchp_resource_file_generic;
  int buffer_size;
  
  /* No device specified, use defaults */
  if (id == NULL)
    return 0;
  
  if (mchp_resource_file_in == 0) {
    if (mchp_resource_file != 0)
      /* specified on the command line */
      mchp_resource_file_in = mchp_resource_file;
    else
      /* use the default */
      mchp_resource_file_in = mchp_get_resource_file_path();
  }

  mchp_resource_file_generic = (char *)mchp_resource_file_in+strlen(mchp_resource_file_in)-strlen("xc32_device.info");

  /* read_device_rib assumes that the resource file name is xc32_device.inf. Ensure that it is. */
  if (strcmp (mchp_resource_file_generic, "xc32_device.info") != 0)
  {
    buffer_size = strlen(mchp_resource_file_in)+strlen("/xc32_device.info")+1;
    mchp_resource_file_generic = (char *)xcalloc (buffer_size,1);
    snprintf (mchp_resource_file_generic, buffer_size, "%s/xc32_device.info", mchp_resource_file_in);
  }
  else
  {
    buffer_size = strlen(mchp_resource_file_in)+1;
    mchp_resource_file_generic = (char *)xcalloc (buffer_size,1);
    snprintf (mchp_resource_file_generic, buffer_size, "%s", mchp_resource_file_in);
  }
  
  rib = read_device_rib (mchp_resource_file_generic, id);
  if (rib == 0) {
    error("Could not open resource file for: %qs at %qs", id, mchp_resource_file_generic);
    return 0;
  }

  if (strcmp(rib->tool_chain,"XC32")) {
    error("Invalid resource file\n");
    close_rib();
    return -1;
  }  
  
  if (rib->field_count >= 4) {
    int record;
    int isr_names_max=0;
    int isr_names_idx=0;

    for (record = 0; move_to_record(record); record++) {
      read_value(rik_string, &d);
      if (strcmp(d.v.s, id) == 0) {
        /* match */
        *matched_id = d.v.s;
        read_value(rik_int, &d);
        if (d.v.i & IS_DEVICE_ID) {
          pic32_device_mask = (d.v.i & (~IS_DEVICE_ID));
          read_value(rik_int, &d);
          pic32_num_register_sets = d.v.i;
          read_value(rik_int, &d);
          pic32_procid = d.v.i;
          break;
        }
      }
    }
  }
  pic32_loaded_device_mask = true;
  return pic32_device_mask;
}
#endif

unsigned int validate_device_mask (char *id, char **matched_id, const char* mchp_resource_file_in);
unsigned int validate_device_mask (char *id, char **matched_id, const char* mchp_resource_file_in)
{
  mchp_load_resource_info (id, matched_id, mchp_resource_file_in);
  /* Disable options that the target does not support */
  if (TARGET_MIPS16 && !(pic32_device_mask & HAS_MIPS16) && pic32_loaded_device_mask)
    {
       error ("The selected %qs device does not support the MIPS16 ISA mode (-mips16)", mchp_processor_string);
    }
  if (TARGET_MICROMIPS && !(pic32_device_mask & HAS_MICROMIPS) && pic32_loaded_device_mask)
    {
       error ("The selected %qs device does not support the microMIPS ISA mode (-mmicromips)", mchp_processor_string);
    }
  if ((pic32_device_mask & HAS_MICROMIPS) && !(pic32_device_mask & HAS_MIPS32R2) && pic32_loaded_device_mask)
    {
      /* This device supports _only_ the microMIPS ISA. Force MicroMIPS mode. */
        target_flags |= MASK_MICROMIPS;
    }
    
  if (TARGET_SMALL_ISA && (pic32_device_mask & HAS_MICROMIPS) && pic32_loaded_device_mask)
    {
      target_flags |= MASK_MICROMIPS;
    }
  else if (TARGET_SMALL_ISA && (pic32_device_mask & HAS_MIPS16) && pic32_loaded_device_mask)
    {
      target_flags |= MASK_MIPS16;
    }
    
  if (TARGET_DSPR2 && !(pic32_device_mask & HAS_DSPR2) && pic32_loaded_device_mask)
    {
      warning (0, "The selected %qs device does not support the DSPr2 ASE (-mdspr2)", mchp_processor_string);
      target_flags &= ~MASK_DSPR2;
      target_flags &= ~MASK_DSP;
    }
  if (TARGET_MCU && !(pic32_device_mask & HAS_MCU) && pic32_loaded_device_mask)
    {
      warning (0, "The selected %qs device does not support the MCU ASE (-mmcu)", mchp_processor_string);
      target_flags &= ~MASK_MCU;
    }

  /* Enable ASE features that the target does support */
  if (!TARGET_DSPR2 && (pic32_device_mask & HAS_DSPR2))
    {
       /* Enable DSPr2 ASE by default when the target supports it */
       target_flags |= MASK_DSP;
       target_flags |= MASK_DSPR2;
    }
  if (TARGET_NO_DSPR2)
    {
       /* Override device defaults and disable DSPr2 ASE */
       target_flags &= ~MASK_DSP;
       target_flags &= ~MASK_DSPR2;
    }
  if (!TARGET_MCU && (pic32_device_mask & HAS_MCU))
    {
       /* Enable MCU ASE by default when the target supports it */
       target_flags |= MASK_MCU;
    }
  if (TARGET_NO_MCU)
    {
       /* Override device defaults and disable MCU ASE */
       target_flags &= ~MASK_MCU;
    }
    
  return pic32_device_mask;
}
#endif /* (MCHP_SKIP_RESOURCE_FILE) */

void 
mchp_subtarget_override_options(void)
{
#if !defined(MCHP_SKIP_RESOURCE_FILE)
  int mask;
  
  /* DSP and microMIPS cannot coexist  */
  if (TARGET_DSP && TARGET_MIPS16)
    error ("unsupported combination: %s", "-mips16 -mdsp");
  
  /* DSPr2 and microMIPS cannot coexist  */
  if (TARGET_DSPR2 && TARGET_MIPS16)
    error ("unsupported combination: %s", "-mips16 -mdspr2");
  
  mask = validate_device_mask ((char*)mchp_processor_string, &mchp_target_cpu_id,
                              mchp_resource_file);
#endif
}


/* Validate the Microchip-specific command-line options.  */
void
mchp_subtarget_override_options1 (void)
{
  mips_code_readable = CODE_READABLE_PCREL;

  /* If smart-io is explicitly disabled, make the size value 0 */
  if (!TARGET_MCHP_SMARTIO)
    {
      mchp_io_size_val = 0;
    }
  if ((mchp_io_size_val < 0) || (mchp_io_size_val > 2))
    {
      warning (0, "Invalid smart-io level %d, assuming 1", mchp_io_size_val);
      mchp_io_size_val = 1;
    }

  if (TARGET_LEGACY_LIBC || TARGET_LONG_CALLS)
    {
      TARGET_MCHP_SMARTIO = 0;
      mchp_io_size_val = 0;
    }

}

#ifdef MCHP_USE_LICENSE_CONF
/* get a line, and remove any line-ending \n or \r\n */
static char *
get_line (char *buf, size_t n, FILE *fptr)
{
  if (fgets (buf, n, fptr) == NULL)
    return NULL;
  while (buf [strlen (buf) - 1] == '\n'
         || buf [strlen (buf) - 1] == '\r')
    buf [strlen (buf) - 1] = '\0';
  return buf;
}
#endif

#ifndef SKIP_LICENSE_MANAGER

#ifdef MCHP_USE_LICENSE_CONF
#define MCHP_LICENSE_CONF_FILENAME "license.conf"
#define MCHP_LICENSEPATH_MARKER "license_dir"
#endif /* MCHP_USE_LICENSE_CONF */

#ifdef __MINGW32__
#define MCHP_MAX_LICENSEPATH_LINE_LENGTH 1024
#define MCHP_XCLM_FILENAME "xclm.exe"
#else
#define MCHP_XCLM_FILENAME "xclm"
#endif

static char*
get_license_manager_path (void)
{
  extern struct cl_decoded_option *save_decoded_options;
  char *xclmpath = NULL;
  FILE *fptr = NULL;
  struct stat filestat;
  int xclmpath_length;

#ifdef MCHP_USE_LICENSE_CONF
  char *conf_dir, *conf_fname;
  FILE *fptr = NULL;
  char line [MCHP_MAX_LICENSEPATH_LINE_LENGTH] = {0};

  /* MCHP_LICENSE_CONF_FILENAME must reside in the same directory as pic32-gcc */
  conf_dir = make_relative_prefix(save_decoded_options[0].arg,
                                  "/pic32mx/bin/gcc/pic32mx/"
                                  str(BUILDING_GCC_MAJOR) "."
                                  str(BUILDING_GCC_MINOR) "."
                                  str(BUILDING_GCC_PATCHLEVEL),
                                  "/bin");

  /* alloc space for the filename: directory + '/' + MCHP_LICENSE_CONF_FILENAME
   */
  conf_fname = (char*)alloca (strlen (conf_dir) + 1 +
                              strlen (MCHP_LICENSE_CONF_FILENAME) + 1);
  strcpy (conf_fname, conf_dir);
  if (conf_fname [strlen (conf_fname) - 1] != '/'
      && conf_fname [strlen (conf_fname) - 1] != '\\')
    strcat (conf_fname, "/");
  strcat (conf_fname, MCHP_LICENSE_CONF_FILENAME);

  if ((fptr = fopen (conf_fname, "rb")) != NULL)
    {
      while (get_line (line, sizeof (line), fptr) != NULL)
        {
          char *pch0, *pch1;
          /* Find the line with the license directory */
          if (strstr (line, MCHP_LICENSEPATH_MARKER))
            {
              /* Find the quoted string on that line */
              pch0 = strchr (line,'"') +1;
              pch1 = strrchr (line,'"');
              if ((pch1-pch0) > 2)
                strncpy (xclmpath, pch0, pch1-pch0);
              break;
            }
        }
      /* Append the xclm executable name to the directory. */
      if (xclmpath [strlen (xclmpath) - 1] != '/'
          && xclmpath [strlen (xclmpath) - 1] != '\\')
        strcat (xclmpath, "/");
      strcat (xclmpath, MCHP_XCLM_FILENAME);

    }

  if (fptr != NULL)
    {
      fclose (fptr);
      fptr = NULL;
    }
#endif /* MCHP_USE_LICENSE_CONF */

#ifdef MCHP_USE_LICENSE_CONF
  if (-1 == stat (xclmpath, &filestat))
#endif /* MCHP_USE_LICENSE_CONF */
    {
      /*  Try the compiler bin directory.
       *
       */
      char* bindir = make_relative_prefix(save_decoded_options[0].arg,
                                          "/pic32mx/bin/gcc/pic32mx/"
                                          str(BUILDING_GCC_MAJOR) "."
                                          str(BUILDING_GCC_MINOR) "."
                                          str(BUILDING_GCC_PATCHLEVEL),
                                          "/bin");
      xclmpath_length = strlen(bindir) + 1 + strlen(MCHP_XCLM_FILENAME) + 1;
      xclmpath = (char*)xcalloc(xclmpath_length+1,sizeof(char));
      strncpy (xclmpath, bindir, xclmpath_length);
      /* Append the xclm executable name to the directory. */
      if (xclmpath [strlen (xclmpath) - 1] != '/'
          && xclmpath [strlen (xclmpath) - 1] != '\\')
        strcat (xclmpath, "/");
      strcat (xclmpath, MCHP_XCLM_FILENAME);

      if (-1 == stat (xclmpath, &filestat))
        {
          free (xclmpath);
          /*  Try the old common directory
           */
          xclmpath_length = strlen("/opt/Microchip/xclm/bin/") + strlen(MCHP_XCLM_FILENAME) + 1;
          xclmpath = (char*)xcalloc(xclmpath_length+1,sizeof(char));
          strncpy (xclmpath, "/opt/Microchip/xclm/bin/", xclmpath_length);
          /* Append the xclm executable name to the directory. */
          strcat (xclmpath, MCHP_XCLM_FILENAME);
        }
      if (-1 == stat (xclmpath, &filestat))
        {
          /*  Try the build directory
           */
          strncpy (xclmpath, "/build/xc32/xclm/bin/", xclmpath_length);
          /* Append the xclm executable name to the directory. */
          strcat (xclmpath, MCHP_XCLM_FILENAME);
        }
    }

#if defined(__MINGW32__)
  {
    char *convert;
    convert = xclmpath;
    while (*convert != '\0')
      {
        if (*convert == '\\')
          *convert = '/';
        convert++;
      }
  }
#endif
  if (-1 == stat (xclmpath, &filestat))
    return NULL;

#if defined(MCHP_DEBUG)
  fprintf (stderr, "%d Final xclmpath: %s\n", __LINE__, xclmpath);
#endif
  return xclmpath;
}
#ifdef MCHP_USE_LICENSE_CONF
#undef MCHP_MAX_LICENSEPATH_LINE_LENGTH
#undef MCHP_LICENSE_CONF_FILENAME
#undef MCHP_LICENSEPATH_MARKER
#endif /* MCHP_USE_LICENSE_CONF */
#undef MCHP_XCLM_FILENAME
#endif

static int
pic32_get_license (int require_cpp)
{
  /*
   *  On systems where we have a licence manager, call it
   */
#ifdef TARGET_MCHP_PIC32MX

/* Misc. Return Codes */
#ifndef MCHP_XCLM_EXPIRED_DEMO
#define MCHP_XCLM_EXPIRED_DEMO 0x10
#endif
#if defined(MCHP_XCLM_VALID_CPP_FREE)
#define PIC32_EXPIRED_LICENSE MCHP_XCLM_EXPIRED_DEMO
#define PIC32_FREE_LICENSE MCHP_XCLM_FREE_LICENSE
#define PIC32_VALID_STANDARD_LICENSE MCHP_XCLM_VALID_STANDARD_LICENSE
#define PIC32_VALID_PRO_LICENSE MCHP_XCLM_VALID_PRO_LICENSE
#define PIC32_NO_CPP_LICENSE MCHP_XCLM_NO_CPP_LICENSE
#define PIC32_VALID_CPP_FREE MCHP_XCLM_VALID_CPP_FREE
#define PIC32_VALID_CPP_FULL MCHP_XCLM_VALID_CPP_FULL
#else
#define PIC32_EXPIRED_LICENSE MCHP_XCLM_EXPIRED_DEMO
#define PIC32_FREE_LICENSE 0
#define PIC32_VALID_STANDARD_LICENSE 1
#define PIC32_VALID_PRO_LICENSE 2
#define PIC32_NO_CPP_LICENSE 4
#define PIC32_VALID_CPP_FREE 5
#define PIC32_VALID_CPP_FULL 6

#endif
#ifndef SKIP_LICENSE_MANAGER
  {
    char *exec;
#if XCLM_FULL_CHECKOUT
    char kopt[] = "-fcfc";
#else
    char kopt[] = "-checkout";
#endif
    char productc[]   = "swxc32";
    char productcpp[] = "swxcpp32";
    char version[9] = "";
    char date[] = __DATE__;

#if XCLM_FULL_CHECKOUT
    char * args[] = { NULL, NULL, NULL, NULL, NULL, NULL};
#else
    char * args[] = { NULL, NULL, NULL, NULL, NULL};
#endif

    char *err_msg=(char*)"", *err_arg=(char*)"";
    const char *failure = NULL;
    int status = 0;
    int err = 0;
    int major_ver =0, minor_ver=0;
    extern struct cl_decoded_option *save_decoded_options;
    struct stat filestat;
    int found_xclm = 0, xclm_tampered = 1;

    /* Get the version number string from the entire version string */
    if ((version_string != NULL) && *version_string)
      {
        char *Microchip;
        gcc_assert(strlen(version_string) < 80);
        Microchip = strrchr ((char*)version_string, 'v');
        if (Microchip)
          {
            while ((*Microchip) &&
                   ((*Microchip < '0') ||
                    (*Microchip > '9')))
              {
                Microchip++;
              }
            if (*Microchip)
              {
                major_ver = strtol (Microchip, &Microchip, 0);
              }
            if ((*Microchip) &&
                ((*Microchip=='_') || (*Microchip=='.')))
              {
                Microchip++;
                minor_ver = strtol(Microchip, &Microchip, 0);
              }
          }
        snprintf (version, 6, "%d.%02d", major_ver, minor_ver);
      }

    /* Arguments to pass to xclm */
    args[1] = kopt;
    if (require_cpp)
      args[2] = productcpp;
    else
      args[2] = productc;
    args[3] = version;
#if XCLM_FULL_CHECKOUT
    args[4] = date;
#endif
    /* Get a path to the license manager to try */
    exec = get_license_manager_path();
    if (exec == NULL)
      {
         /*Set free edition if the license manager isn't available.*/ 
        mchp_pic32_license_valid=PIC32_FREE_LICENSE;
        warning (0, "Could not retrieve compiler license");
        inform (input_location, "Please reinstall the compiler");
      }
    else if (-1 == stat (exec, &filestat))
      {
         /*Set free edition if the license manager execution fails. */
        mchp_pic32_license_valid=PIC32_FREE_LICENSE;
        warning (0, "Could not retrieve compiler license");
        inform (input_location, "Please reinstall the compiler");
      }
    else 
      {
        /* Found xclm */
          found_xclm = 1;
      }

    /* Verify SHA sum and call xclm to determine the license */
    if (found_xclm && mchp_pic32_license_valid==-1)
      {

        /* Check if xclm executable is tampered */
        xclm_tampered = mchp_sha256_validate(exec, (const unsigned char*)MCHP_XCLM_SHA256_DIGEST);
        
        if (xclm_tampered != 0)
          {
            /* Set free edition if the license manager SHA digest does not match. */
            /* The free edition disables optimization options without an eval period. */
            mchp_pic32_license_valid=PIC32_FREE_LICENSE;
            warning (0, "Detected corrupt executable file");
            inform (input_location, "Please reinstall the compiler");
          }
        else
          {
            args[0] = exec;
            failure = pex_one(0, exec, args, "MPLAB XC32 Compiler", 0, 0, &status, &err);

            if (failure != NULL)
              {
                /* Set free edition if the license manager isn't available. */
                /* The free edition disables optimization options without an eval period. */
                mchp_pic32_license_valid=PIC32_FREE_LICENSE;
                warning (0, "Could not retrieve compiler license (%s)", failure);
                inform (input_location, "Please reinstall the compiler");
              }
            else if (WIFEXITED(status))
              {
                mchp_pic32_license_valid = WEXITSTATUS(status);
              }
          }
      }
  }
#endif /* SKIP_LICENSE_MANAGER */
  return mchp_pic32_license_valid;
}

static const char *disabled_option_message = NULL;
static int message_displayed = 0;
static int message_purchase_display = 0;
static const char *invalid_license = "due to an invalid XC32 license";
#define NULLIFY(X,S) \
    if (X) { \
      if ((S != NULL) && (disabled_option_message == NULL)) { \
          disabled_option_message = S; \
          message_displayed++;         \
        } \
    } \
    X

static int require_cpp = 0;
static int nullify_O2 = 0;
static int nullify_Os = 0;
static int nullify_O3 = 0;
static int nullify_mips16 = 0;
static int nullify_lto = 0;
static char *invalid = (char*) "invalid";

void mchp_override_options_after_change(void) {
    if (nullify_Os)
      {
        /* Disable -Os optimization(s) */
        /* flag_web and flag_inline_functions already disabled */
        if (optimize_size)
          {
            optimize = 2;
          }
        NULLIFY(optimize_size, "Optimize for size") = 0;
      }
    if (nullify_O2)
      {
        int opt1_max;
        /* Disable -O2 optimizations */
        if (optimize > 1)
          {
            NULLIFY(optimize, "Optimization level > 1") = 1;
          }
        NULLIFY(flag_indirect_inlining, "indirect inlining") = 0;
        NULLIFY(flag_thread_jumps, "thread jumps") = 0;
        NULLIFY(flag_crossjumping, "crossjumping") = 0;
        NULLIFY(flag_optimize_sibling_calls, "optimize sibling calls") = 0;
        NULLIFY(flag_cse_follow_jumps, "cse follow jumps") = 0;
        NULLIFY(flag_gcse, "gcse") = 0;
        NULLIFY(flag_expensive_optimizations, "expensive optimizations") = 0;
        NULLIFY(flag_rerun_cse_after_loop, "cse after loop") = 0;
        NULLIFY(flag_caller_saves, "caller saves") = 0;
        NULLIFY(flag_peephole2, "peephole2") = 0;
#ifdef INSN_SCHEDULING
        NULLIFY(flag_schedule_insns, "schedule insns") = 0;
        NULLIFY(flag_schedule_insns_after_reload, "schedule insns after reload") = 0;
#endif
        NULLIFY(flag_regmove, "regmove") = 0;
        NULLIFY(flag_strict_aliasing, "strict aliasing") = 0;
        NULLIFY(flag_strict_overflow, "strict overflow") = 0;
        NULLIFY(flag_reorder_blocks, "reorder blocks") = 0;
        NULLIFY(flag_reorder_functions, "reorder functions") = 0;
        NULLIFY(flag_tree_vrp, "tree vrp") = 0;
        NULLIFY(flag_tree_builtin_call_dce, "tree builtin call dce") = 0;
        NULLIFY(flag_tree_pre, "tree pre") = 0;
        NULLIFY(flag_tree_switch_conversion, "tree switch conversion") = 0;
        NULLIFY(flag_ipa_cp, "ipa cp") = 0;
        NULLIFY(flag_ipa_sra, "ipa sra") = 0;

        /* Just -O1/-O0 optimizations.  */
        opt1_max = (optimize <= 1);
        align_loops = opt1_max;
        align_jumps = opt1_max;
        align_labels = opt1_max;
        align_functions = opt1_max;
      }
    if (nullify_O3)
      {
        if (optimize >= 3)
          {
            NULLIFY(optimize, "Optimization level > 2") = 2;
          }
        /* Disable -O3 optimizations */
        NULLIFY(flag_predictive_commoning, "predictive commoning") = 0;
        NULLIFY(flag_inline_functions, "inline functions") = 0;
        NULLIFY(flag_unswitch_loops, "unswitch loops") = 0;
        NULLIFY(flag_gcse_after_reload, "gcse after reload") = 0;
        NULLIFY(flag_tree_vectorize, "tree vectorize") = 0;
        NULLIFY(flag_ipa_cp_clone, "ipa cp clone") = 0;
        flag_ipa_cp = 0;
       }
    if (nullify_mips16)
      {
        /* Disable -mips16 and -mips16e */
        if ((mips_base_compression_flags & (MASK_MIPS16)) != 0)
          {
            /* Disable -mips16 and -mips16e */
            NULLIFY(mips_base_compression_flags, "mips16 mode") = 0;
          }
        if ((mips_base_compression_flags & (MASK_MICROMIPS)) != 0 && (strncmp (mchp_processor_string, "32MM", 4) != 0))
          {
            /* Disable -mmicromips */
            NULLIFY(mips_base_compression_flags, "micromips mode") = 0;
          }
      }
    if (nullify_lto)
      {
        NULLIFY(flag_lto, "Link-time optimization") = 0;
        NULLIFY(flag_whole_program, "Whole-program optimizations") = 0;
        NULLIFY(flag_generate_lto, "Link-time optimization") = 0;
      }
}

static void mchp_print_license_warning (void)
{
    switch (mchp_pic32_license_valid)
      {
      case PIC32_EXPIRED_LICENSE:
        invalid_license = "because the XC32 evaluation period has expired";
        break;
      case PIC32_FREE_LICENSE:
        invalid_license = "because the free XC32 C compiler does not support this feature.";
        break;
      case PIC32_VALID_CPP_FREE:
        invalid_license = "because the free XC32 C++ compiler does not support this feature.";
        break;
      case PIC32_VALID_STANDARD_LICENSE:
        invalid_license = "because this feature requires the MPLAB XC32 PRO compiler";
        break;
      default:
        invalid_license = "due to an invalid XC32 license";
        break;
      }

    if (message_displayed && TARGET_LICENSE_WARNING)
      {
        /* Display a warning for the Standard option first */
        if (disabled_option_message != NULL)
          warning (0,"Compiler option (%s) ignored %s",
                   disabled_option_message, invalid_license);
        disabled_option_message = NULL;
        message_purchase_display++;
      }
#endif /* SKIP_LICENSE_MANAGER */
}

void
mchp_subtarget_override_options2 (void)
{
  extern struct cl_decoded_option *save_decoded_options;
  require_cpp    = 0;

    if (mchp_it_transport && *mchp_it_transport) { 
      if (strcasecmp(mchp_it_transport,"profile") == 0) {
            mchp_profile_option = 1;
      }
    }

#ifndef SKIP_LICENSE_MANAGER
  nullify_O2     = 1;
  nullify_O3     = 1;
  nullify_Os     = 1;
  nullify_mips16 = 1;
  nullify_lto    = 1;
#endif /* SKIP_LICENSE_MANAGER */

  if (mchp_profile_option) {
    flag_inline_small_functions = 0;
    flag_inline_functions = 0;
    flag_no_inline = 1;
    flag_optimize_sibling_calls = 0;
  }

  require_cpp = (strstr (save_decoded_options[0].arg, "cc1plus")!=NULL);
  mchp_pic32_license_valid = pic32_get_license (require_cpp);

    if (require_cpp && !((mchp_pic32_license_valid == PIC32_VALID_CPP_FREE) ||
                         (mchp_pic32_license_valid == PIC32_VALID_CPP_FULL)))
      {
        error  ("MPLAB XC32 C++ license not activated");
        inform (input_location, "Visit http://www.microchip.com/MPLABXCcompilers to acquire a "
               "free C++ license");
       }
    if ((mchp_pic32_license_valid == PIC32_VALID_PRO_LICENSE) ||
        (mchp_pic32_license_valid == PIC32_VALID_CPP_FULL))
      {
        nullify_lto = nullify_mips16 = nullify_O2 =  nullify_O3 = nullify_Os = 0;
      }
    else if (mchp_pic32_license_valid == PIC32_VALID_STANDARD_LICENSE)
      {
        nullify_O2 = 0;
      }
  /*
   *  On systems where we have a licence manager, call it
   */
#ifdef TARGET_MCHP_PIC32MX
  {
    /*
     * Prior to v1.40 (gcc upgrade from 4.5.2 to 4.8.3),  
     * mchp_override_options_after_change() was called from optimization_option()
     * through OPTIMIZATION_OPTION target macro which is poisoned in 4.8.3. Hence
     * optimization_option() was removed. Call to  mchp_override_options_after_change()
     * added here to fix XC32-546
     */
    mchp_override_options_after_change();
    mchp_print_license_warning(); 
 
    if (nullify_lto)
      {
        if (optimize_size)
          {
            optimize = 2;
          }
        if (optimize >= 3)
          {
            NULLIFY(optimize, "Optimization level > 2") = 2;
          }
        NULLIFY(optimize_size, "Optimize for size") = 0;

        NULLIFY(flag_predictive_commoning, "predictive commoning") = 0;
        NULLIFY(flag_inline_functions, "inline functions") = 0;
        NULLIFY(flag_unswitch_loops, "unswitch loops") = 0;
        NULLIFY(flag_gcse_after_reload, "gcse after reload") = 0;
        NULLIFY(flag_tree_vectorize, "tree vectorize") = 0;
        NULLIFY(flag_ipa_cp_clone, "ipa cp clone") = 0;
        flag_ipa_cp = 0;

        NULLIFY(flag_lto, "Link-time optimization") = 0;
        NULLIFY(flag_whole_program, "Whole-program optimizations") = 0;
        NULLIFY(flag_generate_lto, "Link-time optimization") = 0;

        /* Disable -mips16 and -mips16e */
        if ((mips_base_compression_flags & (MASK_MIPS16)) != 0)
          {
            /* Disable -mips16 and -mips16e */
            NULLIFY(mips_base_compression_flags, "mips16 mode") = 0;
          }

        if ((mips_base_compression_flags & (MASK_MICROMIPS)) != 0 && mchp_subtarget_mips32_enabled()) 
          {
            /* If this device also supports mips32, disable -mmicromips */
              NULLIFY(mips_base_compression_flags, "micromips mode") = 0;
          }
      }
    if (message_displayed && TARGET_LICENSE_WARNING)
      {
         /*Now display a warning for the Pro option */
        if (disabled_option_message != NULL)
          warning (0,"Pro Compiler option (%s) ignored %s", disabled_option_message,
                   invalid_license);
        message_purchase_display++;
        message_displayed=0;
      }
    if ((message_purchase_display > 0) && (TARGET_LICENSE_WARNING))
      {
        inform (0, "Disable the option or visit http://www.microchip.com/MPLABXCcompilers "
                "to purchase a new MPLAB XC compiler license.");

         /*If the --nofallback option was specified, abort compilation. */
        if (TARGET_NO_FALLBACKLICENSE)
          error ("Unable to find a valid license, aborting");

        message_purchase_display = 0;
      }

    if (TARGET_LEGACY_LIBC)
      {
        TARGET_MCHP_SMARTIO = 0;
        mchp_io_size_val = 0;
      }

     /*Require a Standard or Pro license */
    if (TARGET_NO_FALLBACKLICENSE && (mchp_pic32_license_valid == PIC32_FREE_LICENSE))
      error ("Unable to find a valid license, aborting"); 
  }
#undef PIC32_EXPIRED_LICENSE
#undef PIC32_ACADEMIC_LICENSE
#undef PIC32_VALID_STANDARD_LICENSE
#undef PIC32_VALID_PRO_LICENSE
#endif /* TARGET_MCHP_PIC32MX */
}
#if defined(TARGET_MCHP_PIC32MX)
#undef NULLIFY
#endif

#if 0
/* Moved to cci.c */

/* Verify the header record for the configuration data file
 */
static int
verify_configuration_header_record (FILE *fptr)
{
  char header_record[MCHP_CONFIGURATION_HEADER_SIZE + 1];
  /* the first record of the file is a string identifying
     file and its format version number. */
  if (get_line (header_record, MCHP_CONFIGURATION_HEADER_SIZE + 1, fptr)
      == NULL)
    {
      warning (0, "malformed configuration word definition file.");
      return 1;
    }
  /* verify that this file is a daytona configuration word file */
  if (strncmp (header_record, MCHP_CONFIGURATION_HEADER_MARKER,
               sizeof (MCHP_CONFIGURATION_HEADER_MARKER) - 1) != 0)
    {
      warning (0, "malformed configuration word definition file.");
      return 1;
    }
  /* verify that the version number is one we can deal with */
  if (strncmp (header_record + sizeof (MCHP_CONFIGURATION_HEADER_MARKER) - 1,
               MCHP_CONFIGURATION_HEADER_VERSION,
               sizeof (MCHP_CONFIGURATION_HEADER_VERSION) - 1))
    {
      warning (0, "configuration word definition file version mismatch.");
      return 1;
    }
  return 0;
}

/* Load the configuration word definitions from the data file
 */
int
mchp_load_configuration_definition (const char *fname)
{
  int retval = 0;
  FILE *fptr;
  char line [MCHP_MAX_CONFIG_LINE_LENGTH];

  if ((fptr = fopen (fname, "rb")) == NULL)
    return 1;

  if (verify_configuration_header_record (fptr))
    return 1;

  while (get_line (line, sizeof (line), fptr) != NULL)
    {
      /* parsing the file is very straightforward. We check the record
         type and transition our state based on it:

          CWORD       Add a new word to the word list and make it the
                        current word
          SETTING     If there is no current word, diagnostic and abort
                      Add a new setting to the current word and make
                        it the current setting
          VALUE       If there is no current setting, diagnostic and abort
                      Add a new value to the current setting
          other       Diagnostic and abort
        */
      if (!strncmp (MCHP_WORD_MARKER, line, MCHP_WORD_MARKER_LEN))
        {
          struct mchp_config_specification *spec;

          /* This is a fixed length record. we validate the following:
              - total record length
              - delimiters in the expected locations */
          if (strlen (line) != (MCHP_WORD_MARKER_LEN
                                + 24   /* two 8-byte hex value fields */
                                + 2)   /* two ':' delimiters */
              || line [MCHP_WORD_MARKER_LEN + 8] != ':'
              || line [MCHP_WORD_MARKER_LEN + 17] != ':')
            {
              warning (0, "malformed configuration word definition file. bad config word record");
              break;
            }

          spec = (struct mchp_config_specification *)xmalloc (sizeof (struct mchp_config_specification));
          spec->next = mchp_configuration_values;

          spec->word = (struct mchp_config_word *)xcalloc (sizeof (struct mchp_config_word), 1);
          spec->word->address = strtoul (line + MCHP_WORD_MARKER_LEN, NULL, 16);
          spec->word->mask = strtoul (line + MCHP_WORD_MARKER_LEN + 9, NULL, 16);
          spec->word->default_value =
            strtoul (line + MCHP_WORD_MARKER_LEN + 18, NULL, 16);

          /* initialize the value to the default with no bits referenced */
          spec->value = spec->word->default_value;
          spec->referenced_bits = 0;

          mchp_configuration_values = spec;
        }
      else if (!strncmp (MCHP_SETTING_MARKER, line, MCHP_SETTING_MARKER_LEN))
        {
          struct mchp_config_setting *setting;
          size_t len;

          if (!mchp_configuration_values)
            {
              warning (0, "malformed configuration word definition file. setting record without preceding word record");
              break;
            }

          /* Validate the fixed length portion of the record by checking
             that the record length is >= the size of the minimum valid
             record (empty description and one character name) and that the
             ':' delimiter following the mask is present. */
          if (strlen (line) < (MCHP_SETTING_MARKER_LEN
                               + 8     /* 8-byte hex mask field */
                               + 2     /* two ':' delimiters */
                               + 1)    /* non-empty setting name */
              || line [MCHP_SETTING_MARKER_LEN + 8] != ':')
            {
              warning (0, "malformed configuration word definition file. bad setting record");
              break;
            }

          setting = (struct mchp_config_setting *)xcalloc (sizeof (struct mchp_config_setting), 1);
          setting->next = mchp_configuration_values->word->settings;

          setting->mask = strtoul (line + MCHP_SETTING_MARKER_LEN, NULL, 16);
          len = strcspn (line + MCHP_SETTING_MARKER_LEN + 9, ":");
          /* Validate that the name is not empty */
          if (len == 0)
            {
              warning (0, "malformed configuration word definition file. bad setting record");
              break;
            }
          setting->name = (char*)xmalloc (len + 1);
          strncpy (setting->name, line + MCHP_SETTING_MARKER_LEN + 9, len);
          setting->name [len] = '\0';
          setting->description =
            (char*)xmalloc (strlen (line + MCHP_SETTING_MARKER_LEN + len + 10) + 2);
          strcpy (setting->description,
                  line + MCHP_SETTING_MARKER_LEN + len + 10);

          mchp_configuration_values->word->settings = setting;
        }
      else if (!strncmp (MCHP_VALUE_MARKER, line, MCHP_VALUE_MARKER_LEN))
        {
          struct mchp_config_value *value;
          size_t len;
          if (!mchp_configuration_values
              || !mchp_configuration_values->word->settings)
            {
              warning (0, "malformed configuration word definition file.");
              break;
            }
          /* Validate the fixed length portion of the record by checking
             that the record length is >= the size of the minimum valid
             record (empty description and one character name) and that the
             ':' delimiter following the mask is present. */
          if (strlen (line) < (MCHP_VALUE_MARKER_LEN
                               + 8     /* 8-byte hex mask field */
                               + 2     /* two ':' delimiters */
                               + 1)    /* non-empty setting name */
              || line [MCHP_VALUE_MARKER_LEN + 8] != ':')
            {
              warning (0, "malformed configuration word definition file. bad value record");
              break;
            }

          value = (struct mchp_config_value *)xcalloc (sizeof (struct mchp_config_value), 1);
          value->next = mchp_configuration_values->word->settings->values;

          value->value = strtoul (line + MCHP_VALUE_MARKER_LEN, NULL, 16);
          len = strcspn (line + MCHP_VALUE_MARKER_LEN + 9, ":");
          /* Validate that the name is not empty */
          if (len == 0)
            {
              warning (0, "malformed configuration word definition file. bad setting record");
              break;
            }
          value->name = (char*)xmalloc (len + 1);
          strncpy (value->name, line + MCHP_VALUE_MARKER_LEN + 9, len);
          value->name [len] = '\0';
          value->description =
            (char*)xmalloc (strlen (line + MCHP_VALUE_MARKER_LEN + len + 10) + 2);
          strcpy (value->description,
                  line + MCHP_VALUE_MARKER_LEN + len + 10);

          mchp_configuration_values->word->settings->values = value;
        }
      else
        {
          warning (0, "malformed configuration word definition file.");
          break;
        }
    }
  /* if we didn't exit the loop because of end of file, we have an
     error of some sort. */
  if (!feof (fptr))
    {
      warning (0, "malformed configuration word definition file.");
      retval = 1;
    }


  fclose (fptr);
  return retval;
}

void
mchp_handle_configuration_setting (const char *name, const unsigned char *value_name)
{
  struct mchp_config_specification *spec;

  /* Look up setting in the definitions for the configuration words */
  for (spec = mchp_configuration_values ; spec ; spec = spec->next)
    {
      struct mchp_config_setting *setting;
      for (setting = spec->word->settings ; setting ; setting = setting->next)
        {
          if (strcmp (setting->name, name) == 0)
            {
              struct mchp_config_value *value;

              /* If we've already specified this setting, that's an
                 error, even if the new value and the old value match */
              if (spec->referenced_bits & setting->mask)
                {
                  error ("multiple definitions for configuration setting '%s'",
                         name);
                  return;
                }

              /* look up the value */
              for (value = setting->values ;
                   value ;
                   value = value->next)
                {
                  if (strcmp (value->name, (const char*)value_name) == 0)
                    {
                      /* mark this setting as having been specified */
                      spec->referenced_bits |= setting->mask;
                      /* update the value of the word with the value
                         indicated */
                      spec->value = (spec->value & ~setting->mask)
                                    | value->value;
                      return;
                    }
                }
              /* If we got here, we didn't match the value name */
              error ("unknown value for configuration setting '%s': '%s'",
                     name, value_name);
              return;
            }
        }
    }
  /* if we got here, we didn't find the setting, which is an error */
  error ("unknown configuration setting: '%s'", name);
}

#endif /* Moved to cci */

static void
mchp_output_configuration_words (void)
{
  struct mchp_config_specification *spec;

  fputs ("# Microchip Technology PIC32 MCU configuration words\n", asm_out_file);

  for (spec = mchp_configuration_values ; spec ; spec = spec->next)
    {
      /* if there are referenced bits in the word, output its value */
      if (spec->referenced_bits)
        {
          fprintf (asm_out_file, "# Configuration word @ 0x%08x\n", spec->word->address);
          fprintf (asm_out_file, "\t.section\t.config_%08X, code, keep, address(0x%08X)\n",
                   spec->word->address, spec->word->address);
          fprintf (asm_out_file, "\t.type\t__config_%08X, @object\n",
                   spec->word->address);
          fprintf (asm_out_file, "\t.size\t__config_%08X, 4\n",
                   spec->word->address);
          fprintf (asm_out_file, "__config_%08X:\n", spec->word->address);
          fprintf (asm_out_file, "\t.word\t0x%08X\n", spec->value);
        }
    }
}

static void
mchp_output_ext_region_memory_info (void)
{
    pic32_external_memory *m;
    char printed = 0;

    for (m = pic32_external_memory_head; m; m = m ->next) 
    {
      fprintf(asm_out_file, "\n\t.memory _%s, origin(0x%x), size(0x%x)", m->name, m->origin, m->size);
      printed = 1 ;
    }
  
    if (printed) 
    {
      fprintf(asm_out_file, "\n\n");
    }
}
void mchp_file_end (void)
{
  mchp_output_ext_region_memory_info();
  mchp_output_vector_dispatch_table();
  mchp_output_configuration_words ();
}


/* Callback when a vector attribute is recognized. 'node' points
   to the node to which the attribute is applied (which may not be a DECL).
   If it is a decl, we modify in place, otherwise we need to create a copy
   for any changes. 'name' is the identifier node specifying the attribute.
   'args' is a TREE_LIST of the arguments to the attribute. 'flags' gives
   us context information. We use 'no_add_attrs' to indicate whether the
   attribute should be added to the decl/type or not; in this case, we
   never want to keep the attribute since it's just an indicator to us to
   add the vector.

   Return is NULL_TREE since we're not modifying the attribute for other
   decls/types */
tree
mchp_vector_attribute (tree *node, tree name ATTRIBUTE_UNUSED, tree args,
                       int flags ATTRIBUTE_UNUSED, bool *no_add_attrs)
{
  tree decl = *node;
  enum pic32_isa_mode isamode = pic32_isa_mips32r2;
  
  /* If this attribute isn't on the actual function declaration, we
     ignore it */
  if (TREE_CODE (decl) != FUNCTION_DECL)
    return NULL_TREE;

  if (flag_generate_lto)
    {
      warning (0, "Attribute vector not supported with -flto option, this file will not participate in LTO");
      flag_generate_lto = 0;
    }

  /* The vector attribute has a comma delimited list of vector #'s as
     arguments. At least one must be present. */
  gcc_assert (args);
  while (args)
    {
      if (TREE_CODE (TREE_VALUE (args)) == INTEGER_CST)
        {
          /* The argument must be an integer constant between 0 and 255 */
          if ((int)TREE_INT_CST_LOW (TREE_VALUE (args)) < 0 ||
              (int)TREE_INT_CST_LOW (TREE_VALUE (args)) > 255)
            {
              *no_add_attrs = 1;

              error ("Vector number must be an integer between 0 and 255");
              return NULL_TREE;
            }
            
          if (mips_base_compression_flags & (MASK_MICROMIPS))
            isamode = pic32_isa_micromips;
            
          /* add the vector to the list of dispatch functions to emit */
          mchp_add_vector_dispatch_entry (
            IDENTIFIER_POINTER (DECL_NAME (*node)),
            (int)TREE_INT_CST_LOW (TREE_VALUE (args)),
            mips_far_type_p (TREE_TYPE (*node)),
            isamode,
            1);
        }
      else
        {
          if (TREE_CODE (TREE_VALUE (args)) == COMPOUND_EXPR)
            {
              mchp_vector_attribute_compound_expr (node, TREE_VALUE(args),
                                                   no_add_attrs);
            }
          else
            {
              *no_add_attrs = 1;
              error ("Vector number must be an integer between 0 and 255");
              return NULL_TREE;
            }
        }
      args = TREE_CHAIN (args);
    }

  return NULL_TREE;
}

/* Handles compound expressions within the vector attribute list.
   Called from mchp_vector_attribute(). */
static int
mchp_vector_attribute_compound_expr (tree *node, tree expr,
                                     bool *no_add_attrs)
{
  int len = 0;
  enum pic32_isa_mode isamode = pic32_isa_mips32r2;
  
  if (expr == NULL_TREE)
    return 0;
  for (; TREE_CODE (expr) == COMPOUND_EXPR; expr = TREE_OPERAND (expr, 1))
    {
      len += mchp_vector_attribute_compound_expr (node, TREE_OPERAND (expr, 0),
             no_add_attrs);
    }
  if (TREE_CODE (expr) == INTEGER_CST)
    {
      /* The argument must be an integer constant between 0 and 255 */
      if ((int)TREE_INT_CST_LOW (expr) < 0 ||
          (int)TREE_INT_CST_LOW (expr) > 255)
        {
          *no_add_attrs = 1;

          error ("Vector number must be an integer between 0 and 255");
          return 0;
        }
        
      if (mips_base_compression_flags & (MASK_MICROMIPS))
        isamode = pic32_isa_micromips;
            
      /* add the vector to the list of dispatch functions to emit */
      mchp_add_vector_dispatch_entry (
        IDENTIFIER_POINTER (DECL_NAME (*node)),
        (int)TREE_INT_CST_LOW (expr),
        mips_far_type_p (TREE_TYPE (*node)),
        isamode,
        1);
    }
  ++len;
  return len;
}

/* Callback when a vector attribute is recognized. 'node' points
   to the node to which the attribute is applied (which may not be a DECL).
   If it is a decl, we modify in place, otherwise we need to create a copy
   for any changes. 'name' is the identifier node specifying the attribute.
   'args' is a TREE_LIST of the arguments to the attribute. 'flags' gives
   us context information. We use 'no_add_attrs' to indicate whether the
   attribute should be added to the decl/type or not; in this case, we
   never want to keep the attribute since it's just an indicator to us to
   add the vector.

   In effect, this is just a section attribute with a pre-defined section
   name.
   */
tree
mchp_at_vector_attribute (tree *node, tree name ATTRIBUTE_UNUSED, tree args,
                          int flags ATTRIBUTE_UNUSED, bool *no_add_attrs)
{
  tree decl = *node;
  enum pic32_isa_mode isamode = pic32_isa_mips32r2;
  char scn_name[12];

  /* If this attribute isn't on the actual function declaration, we
     ignore it */
  if (TREE_CODE (decl) != FUNCTION_DECL)
    return NULL_TREE;

  if (DECL_SECTION_NAME (decl) != NULL)
    {
      error ("the 'at_vector' attribute cannot be used with the 'section' attribute");
      *no_add_attrs = true;
      return NULL_TREE;
    }
  /* The argument must be an integer constant between 0 and 255 */
  if (TREE_CODE (TREE_VALUE (args)) != INTEGER_CST ||
      (int)TREE_INT_CST_LOW (TREE_VALUE (args)) < 0 ||
      (int)TREE_INT_CST_LOW (TREE_VALUE (args)) > 255)
    {
      *no_add_attrs = 1;

      error ("Interrupt vector number must be an integer between 0 and 255");
      return NULL_TREE;
    }
  /* now mark the decl as going into the section for the indicated vector */
  sprintf (scn_name, ".vector_%d", (int)TREE_INT_CST_LOW (TREE_VALUE (args)));
  DECL_SECTION_NAME (decl) = build_string (strlen (scn_name), scn_name);
  
  if (mips_base_compression_flags & (MASK_MICROMIPS))
    isamode = pic32_isa_micromips;
  
  /* add the vector to the list of dispatch functions to emit */
  mchp_add_vector_dispatch_entry (
    IDENTIFIER_POINTER (DECL_NAME (*node)),
    (int)TREE_INT_CST_LOW (TREE_VALUE(args)),
    mips_far_type_p (TREE_TYPE (*node)),
    isamode,
    0);

  return NULL_TREE;
}


static void
mchp_output_vector_dispatch_table (void)
{
  struct vector_dispatch_spec *dispatch_entry;

  fputs ("# Begin MCHP vector dispatch table\n", asm_out_file);

  /* output the vector dispatch functions specified in this translation
     unit, if any */
  for (dispatch_entry = vector_dispatch_list_head ;
       dispatch_entry ;
       dispatch_entry = dispatch_entry->next)
    {
      fprintf (asm_out_file, "\t.globl\t__vector_dispatch_%d\n",
               dispatch_entry->vector_number);
      if (dispatch_entry->emit_dispatch != 0)
      {
        fprintf (asm_out_file, "\t.section\t.vector_%d,code,keep\n",
                 dispatch_entry->vector_number);
        fprintf (asm_out_file, "\t.set\tnomips16\n");
        if ((mips_base_compression_flags & (MASK_MICROMIPS)) && (pic32_device_mask & HAS_MICROMIPS))
          {
            fprintf (asm_out_file, "\t.set\tmicromips\n");
          }
        fprintf (asm_out_file, "\t.set noreorder\n");
        fprintf (asm_out_file, "\t.align\t2\n");
        fprintf (asm_out_file, "\t.ent\t__vector_dispatch_%d\n",
                 dispatch_entry->vector_number);
                 
        fprintf (asm_out_file, "__vector_dispatch_%d:\n",
                 dispatch_entry->vector_number);
        if ((dispatch_entry->longcall) || 
            ((mips_base_compression_flags & (MASK_MICROMIPS)) && (dispatch_entry->isr_isa_mode != pic32_isa_micromips)) ||
            (!(mips_base_compression_flags & (MASK_MICROMIPS)) && (dispatch_entry->isr_isa_mode == pic32_isa_micromips)) ||
            TARGET_LONG_CALLS)
          {
            fprintf (asm_out_file, "\tla\t$26, %s\n", dispatch_entry->target);
            fprintf (asm_out_file, "\tjr\t$26\n");
          }
        else
          {
            fprintf (asm_out_file, "\tj\t%s\n", dispatch_entry->target);
          }

        /* Branch delay slot */
        fprintf (asm_out_file, "\tnop\n");
        fprintf (asm_out_file, "\t.set reorder\n");
        fprintf (asm_out_file, "\t.end\t__vector_dispatch_%d\n",
                 dispatch_entry->vector_number);
        fprintf (asm_out_file, "\t.size\t__vector_dispatch_%d, .-"
                 "__vector_dispatch_%d\n",
                 dispatch_entry->vector_number,
                 dispatch_entry->vector_number);
      }
#if 1
      else
        fprintf (asm_out_file, "\t__vector_dispatch_%d = 0\n",
                 dispatch_entry->vector_number);
#endif
    }
  fputs ("# End MCHP vector dispatch table\n", asm_out_file);
}

struct interrupt_pragma_spec *interrupt_pragma_list_head;

/* Called when a decl is being created.

   This allows us to add attributes based on pragmas or other target
   specific stuff. In this case, we want to apply the interrupt attribute
   to decls of functions specified as interrupts via #pragma. If the
   handler function was specified to reside at the vector address, we
   will also apply a section attribute to the function to do that. */
void
mchp_target_insert_attributes (tree decl, tree *attr_ptr)
{
  if (TREE_CODE (decl) == FUNCTION_DECL)
    {
      const char *fname;
      struct interrupt_pragma_spec *p;
      struct vector_dispatch_spec *dispatch_entry;
      enum pic32_isa_mode isamode = pic32_isa_mips32r2;

      bool micromips_p, nomicromips_p, vector_p, longcall_p;
      micromips_p = lookup_attribute ("micromips", *attr_ptr) != NULL;
      nomicromips_p = lookup_attribute ("nomicromips", *attr_ptr) != NULL;
      vector_p = lookup_attribute ("vector", *attr_ptr) != NULL;
      longcall_p = (lookup_attribute ("long_call", *attr_ptr) != NULL
          || lookup_attribute ("far", *attr_ptr) != NULL
          || lookup_attribute ("longcall", *attr_ptr) != NULL);

      /* Determine if an interrupt pragma is around for this function */
      fname = IDENTIFIER_POINTER (DECL_NAME (decl));
      gcc_assert (fname);
      
      if (vector_p && (micromips_p || longcall_p))
      {
        if (micromips_p)
          isamode = pic32_isa_micromips;

        mchp_add_vector_dispatch_entry(fname, -1, longcall_p, isamode, 1);
      }

      for (p = interrupt_pragma_list_head ;
           p && strcmp (p->name, fname) ;
           p = p->next)
        ;
      /* if p != NULL, we have a match */
      if (p)
        {
          /* Apply the interrupt attribute to the decl.

             The attribute list is a TREE_LIST. We need to create a new
             TREE_LIST node with the existing attribute list as its
             TREE_CHAIN entry.

             The attribute itself has the purpose field being the name
             of the attribute and the parameters of the attribute being
             another TREE_LIST as the value of the attribute tree.  */
          *attr_ptr = tree_cons (get_identifier ("interrupt"),
                                 build_tree_list (NULL_TREE, p->ipl),
                                 *attr_ptr);
          /* If the interrupt goes directly at the vector address,
             add a section attribute to the function to place it in the
             dedicated vector section */
          if (p->vect_number)
            {
              char scn_name[12];
              gcc_assert (p->vect_number < 256);
              sprintf (scn_name, ".vector_%d", p->vect_number);
              *attr_ptr = tree_cons (get_identifier ("section"),
                                     build_tree_list (NULL_TREE,
                                                      build_string (strlen (scn_name), scn_name)),
                                     *attr_ptr);
            }
        }
    }
}

void
mchp_prepare_function_start (tree fndecl)
{
  tree x;

  if (!fndecl)
    return;

  /* Get the DECL of the top-most enclosing function. */
  /* XXX do we still need to do this? */
  while ((x = decl_function_context (fndecl)))
    fndecl = x;
}
/* Check if the interrupt attribute is set for a function. If it is, return
   the IPL identifier, else NULL */

static tree
mchp_function_interrupt_p (tree decl)
{
  tree attrlist = TYPE_ATTRIBUTES(TREE_TYPE(decl));
  tree attr;
  attr = lookup_attribute ("interrupt", attrlist);
  if (attr)
    {
      /* Behavior for default (unspecified) IPL value */
      if ((TREE_VALUE(attr)) == NULL )
        {
          cfun->machine->current_function_type = current_function_type = DEFAULT_CONTEXT_SAVE;
          mchp_interrupt_priority = -1;
          cfun->machine->interrupt_priority = -1;
        }
      else
        {
          current_function_type = UNKNOWN_CONTEXT_SAVE;
        }
      attr = TREE_VALUE (attr);
    }
  else
    {
      current_function_type = NON_INTERRUPT;
    }
  return attr;
}

static int ignore_attribute(const char *attribute, const char *attached_to,
                            tree node)
{
  tree scope;

  if (TREE_CODE(node) == PARM_DECL)
    {
      warning(0, "ignoring %s attribute applied to parameter %s", attribute,
              attached_to);
      return 1;
    }
  else if (TREE_CODE(node) == FIELD_DECL)
    {
      warning(0, "ignoring %s attribute applied to member %s", attribute,
              attached_to);
      return 1;
    }

  /* DECL_CONTEXT is not set up by the time we process the attributes */
  scope = current_function_decl;

  if (scope && (TREE_CODE(scope) == FUNCTION_DECL) &&
      (!TREE_STATIC(node)) &&
      ((TREE_CODE(node) != VAR_DECL) || !TREE_PUBLIC(node)))
    {
      warning(0, "ignoring %s attribute applied to automatic %s", attribute,
              attached_to);
      return 1;
    }
  return 0;
}

/* Callback when an interrupt attribute is recognized. 'node' points
   to the node to which the attribute is applied (which may not be a DECL).
   If it is a decl, we modify in place, otherwise we need to create a copy
   for any changes. 'name' is the identifier node specifying the attribute.
   'args' is a TREE_LIST of the arguments to the attribute. 'flags' gives
   us context information. We use 'no_add_attrs' to indicate whether the
   attribute should be added to the decl/type or not; in this case, on error.

   Return is NULL_TREE since we're not modifying the attribute for other
   decls/types */
tree
mchp_interrupt_attribute (tree *node ATTRIBUTE_UNUSED,
                          tree name ATTRIBUTE_UNUSED, tree args,
                          int flags ATTRIBUTE_UNUSED, bool *no_add_attrs)
{
  /* we want to validate that the argument isn't bogus. There should
     be one and only one argument in the args TREE_LIST and it should
     be an identifier of the form "ipl[0-7]". */

  /* we can assert one argument since that should be enforced by
     the parser from the attribute table */

  if (args == NULL)
    {
      return NULL_TREE;
    }

  gcc_assert (TREE_CHAIN (args) == NULL);

  if (TREE_CODE (TREE_VALUE (args)) != IDENTIFIER_NODE
      || (((strcmp ("single", IDENTIFIER_POINTER (TREE_VALUE (args))) != 0)
           && (strcmp ("SINGLE", IDENTIFIER_POINTER (TREE_VALUE (args))) != 0)
           && (strcmp ("ripl", IDENTIFIER_POINTER (TREE_VALUE (args))) != 0)
           && (strcmp ("RIPL", IDENTIFIER_POINTER (TREE_VALUE (args))) != 0))
          && (((strncmp ("ipl", IDENTIFIER_POINTER (TREE_VALUE (args)),3) != 0)
               && (strncmp ("IPL", IDENTIFIER_POINTER (TREE_VALUE(args)),3)!= 0))
              || (IDENTIFIER_POINTER (TREE_VALUE (args))[3] > '7')
              || (IDENTIFIER_POINTER (TREE_VALUE (args))[3] < '0'))))
    {
      error ("Interrupt priority must be specified as 'single' or 'IPLn{AUTO|SOFT|SRS}', "
             "where n is in the range of 0..7, inclusive.");
      *no_add_attrs = 1;
      return NULL_TREE;
    }

  if ((strlen (IDENTIFIER_POINTER (TREE_VALUE (args))) > strlen("ipl7")) &&
    (ISDIGIT(IDENTIFIER_POINTER (TREE_VALUE (args))[3]) &&
     ISDIGIT(IDENTIFIER_POINTER (TREE_VALUE (args))[4])))
    {
       error ("Interrupt priority must be specified as 'single' or 'IPLn{AUTO|SOFT|SRS}', "
              "where n is in the range of 0..7, inclusive.");
       *no_add_attrs = 1;
       return NULL_TREE;
    }

   if ((strncasecmp ("IPL", IDENTIFIER_POINTER (TREE_VALUE (args)), 3) == 0) && 
       (strlen(IDENTIFIER_POINTER (TREE_VALUE (args)))==4))
      {
        warning (0, "Interrupt priority IPL%c is deprecated. Specify as 'IPL%c{AUTO|SOFT|SRS}' instead.", 
                 IDENTIFIER_POINTER (TREE_VALUE (args))[3],
                 IDENTIFIER_POINTER (TREE_VALUE (args))[3]);
      }

  return NULL_TREE;
}

/*
** Return nonzero if IDENTIFIER is a valid attribute.
*/
tree mchp_address_attribute(tree *decl, tree identifier ATTRIBUTE_UNUSED,
                            tree args, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs)
{
  const char *attached_to = 0;
  tree address = NULL_TREE;
  if (args == NULL)
    {
      return NULL_TREE;
    }

  if (DECL_P(*decl))
    {
      attached_to = IDENTIFIER_POINTER(DECL_NAME(*decl));
    }

  /* we can assert one argument since that should be enforced by
     the parser from the attribute table */

  gcc_assert (TREE_CHAIN (args) == NULL);

  if (DECL_P(*decl))
    {
      if (ignore_attribute("address", attached_to, *decl))
        {
          *no_add_attrs = 1;
          return NULL_TREE;
        }
      address = TREE_VALUE(args);
      if (TREE_CODE(address) != INTEGER_CST)
        {
          error("invalid address argument for '%s'", attached_to);
          *no_add_attrs = 1;
        }
      else
        /* currently the assembler will not accept an unaligned address */
        if (TREE_INT_CST_LOW(address) % 4)
          {
            warning(0, "invalid address argument for '%s'", attached_to);
            warning(0, "unaligned addresses are not yet permitted, ignoring attribute");
            *no_add_attrs = 1;
          }
      {
        unsigned long long address_val;
        address_val = (unsigned long long)TREE_INT_CST_LOW(address);
        if ((address_val &0xFFFFFFFFul) < 0x20000000ul)
          { 
            error ("Physical-memory address used with address attribute. Use a virtual-memory address for '%s'",
                   attached_to);
            *no_add_attrs = 1;
          }
        if (address_val > 0xFFFFFFFFul)
          { 
            error ("Address of '%s' is outside of the 32-bit address range", attached_to);
            *no_add_attrs = 1;
          }
       }
    }

  /* Address attribute implies noinline and noclone */
  if (TREE_CODE(*decl) == FUNCTION_DECL)
    {
      tree attrib_noinline, attrib_noclone;
      long unsigned int address_val;
      address_val = (long unsigned int)TREE_INT_CST_LOW(address) & 0xFFFFFFFFul;
      
      if (lookup_attribute ("noinline", DECL_ATTRIBUTES (*decl)) == NULL)
        {
          attrib_noinline = build_tree_list(get_identifier("noinline"), NULL_TREE);
          attrib_noinline = chainon(DECL_ATTRIBUTES(*decl), attrib_noinline);
          decl_attributes(decl, attrib_noinline, 0);
        }
      if (lookup_attribute ("noclone", DECL_ATTRIBUTES (*decl)) == NULL)
        {
          attrib_noclone = build_tree_list(get_identifier("noclone"), NULL_TREE);
          attrib_noclone = chainon(DECL_ATTRIBUTES(*decl), attrib_noclone);
          decl_attributes(decl, attrib_noclone, 0);
        }
      if ((pic32_device_mask & HAS_L1CACHE) && 
          ((address_val < 0xC0000000ul) && (address_val > 0xA0000000ul)))
        { 
          warning(0, "Specified address of function '%s' is in the kseg1 region of an L1-cache target device", attached_to);
          inform (input_location, "Use a kseg0-region address for this function");
          /* Convert the kseg1 address to a kseg0 address. */
          address_val &= ~0x20000000ul;
          TREE_INT_CST_LOW(address) = address_val;
        }
     }

  if (TREE_CODE(*decl) == VAR_DECL)
    {
      long unsigned int address_val;
      address_val = (long unsigned int)TREE_INT_CST_LOW(address) & 0xFFFFFFFFul;
      if ((pic32_device_mask & HAS_L1CACHE) &&
          ((address_val >= 0xA0000000ul) && (address_val < 0xB0000000)))
        { 
          warning(0, "Specified address of variable '%s' is in the kseg1 region of an L1-cache target device", attached_to);
          inform (input_location, "Use the 'coherent' attribute with a kseg0-region address to designate an uncached variable");
          /* Convert the kseg1 address to a kseg0 address and apply the coherent attribute. */
          address_val &= ~0x20000000ul;
          TREE_INT_CST_LOW(address) = address_val;
          tree attrib_coherent = build_tree_list (get_identifier ("coherent"), NULL_TREE);
          attrib_coherent = chainon (DECL_ATTRIBUTES(*decl), attrib_coherent);
          decl_attributes (decl, attrib_coherent, 0);
          DECL_COMMON (*decl) = 0;
        }
      else if (!(pic32_device_mask & HAS_L1CACHE) &&
          ((address_val >= 0x80000000ul) && (address_val < 0x90000000ul)))
        { 
          warning(0, "Data-variable address in kseg0 region of an uncached target device");
        }
    }

  return NULL_TREE;
}

/*
** Return nonzero if IDENTIFIER is a valid attribute.
*/
tree mchp_ramfunc_attribute(tree *decl, tree identifier ATTRIBUTE_UNUSED,
                            tree args ATTRIBUTE_UNUSED, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs ATTRIBUTE_UNUSED)
{
  /* Ramfunc attribute implies noinline and noclone */
  if (TREE_CODE(*decl) == FUNCTION_DECL)
    {
      tree attrib_noinline, attrib_noclone;
      if (lookup_attribute ("noinline", DECL_ATTRIBUTES (*decl)) == NULL)
        {
          attrib_noinline = build_tree_list(get_identifier("noinline"), NULL_TREE);
          attrib_noinline = chainon(DECL_ATTRIBUTES(*decl), attrib_noinline);
          decl_attributes(decl, attrib_noinline, 0);
        }
      if (lookup_attribute ("noclone", DECL_ATTRIBUTES (*decl)) == NULL)
        {
          attrib_noclone = build_tree_list(get_identifier("noclone"), NULL_TREE);
          attrib_noclone = chainon(DECL_ATTRIBUTES(*decl), attrib_noclone);
          decl_attributes(decl, attrib_noclone, 0);
        }
    }

  return NULL_TREE;
}

/*
** Return nonzero if IDENTIFIER is a valid attribute.
*/
tree mchp_naked_attribute(tree *decl, tree identifier ATTRIBUTE_UNUSED,
                            tree args ATTRIBUTE_UNUSED, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs ATTRIBUTE_UNUSED)
{
  /* Naked attribute implies noinline and noclone */
  if (TREE_CODE(*decl) == FUNCTION_DECL)
    {
      tree attrib_noinline, attrib_noclone;
      if (lookup_attribute ("noinline", DECL_ATTRIBUTES (*decl)) == NULL)
        {
          attrib_noinline = build_tree_list(get_identifier("noinline"), NULL_TREE);
          attrib_noinline = chainon(DECL_ATTRIBUTES(*decl), attrib_noinline);
          decl_attributes(decl, attrib_noinline, 0);
        }
      if (lookup_attribute ("noclone", DECL_ATTRIBUTES (*decl)) == NULL)
        {
          attrib_noclone = build_tree_list(get_identifier("noclone"), NULL_TREE);
          attrib_noclone = chainon(DECL_ATTRIBUTES(*decl), attrib_noclone);
          decl_attributes(decl, attrib_noclone, 0);
        }
    }

  return NULL_TREE;
}

/* Handle a "unique_section" attribute; arguments as in
   struct attribute_spec.handler.  
*/
tree
mchp_unique_section_attribute (tree *node, tree name ATTRIBUTE_UNUSED,
                          tree args ATTRIBUTE_UNUSED, int flags ATTRIBUTE_UNUSED,
                          bool *no_add_attrs)
{
  tree decl = *node;

      if ((TREE_CODE (decl) == FUNCTION_DECL
	   || TREE_CODE (decl) == VAR_DECL))
	{
	  if (TREE_CODE (decl) == VAR_DECL
	      && current_function_decl != NULL_TREE
	      && ! TREE_STATIC (decl))
	    {
	      error ("%Junique_section attribute cannot be specified for "
                     "local variables", decl);
	      *no_add_attrs = true;
	    }

	    DECL_UNIQUE_SECTION (decl) = 1;
	    DECL_COMMON (decl) = 0;
	}
      else
	{
	  error ("%Junique_section attribute not allowed for '%D'", *node, *node);
	  *no_add_attrs = true;
	}

  return NULL_TREE;
}


/*
** Return nonzero if IDENTIFIER is a valid attribute.
*/
tree mchp_nomicromips_attribute(tree *decl, tree identifier ATTRIBUTE_UNUSED,
                            tree args ATTRIBUTE_UNUSED, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs ATTRIBUTE_UNUSED)
{
  /* Naked attribute implies noinline and noclone */
    if (!mchp_subtarget_mips32_enabled())
    {
      error ("The %qs target device does not support the %qs attribute on %qs", 
             mchp_processor_string, 
             "nomicromips", 
             IDENTIFIER_POINTER (DECL_NAME (*decl)));
    }

  return NULL_TREE;
}

/*
** Return nonzero if IDENTIFIER is a valid attribute.
*/
tree mchp_unsupported_attribute(tree *node, tree identifier ATTRIBUTE_UNUSED,
                                tree args, int flags ATTRIBUTE_UNUSED,
                                bool *no_add_attrs)
{
  tree type = NULL_TREE;
  int warn = 0;
  tree what = NULL_TREE;
  const char *attached_to = 0;

  if (DECL_P(*node))
    {
      attached_to = IDENTIFIER_POINTER(DECL_NAME(*node));
    }

  if (!args)
    *no_add_attrs = true;
  else if (TREE_CODE (TREE_VALUE (args)) != STRING_CST)
    {
      error("invalid argument to 'unsupported' attribute applied to '%s',"
            " literal string expected", attached_to);
      *no_add_attrs = true;
    }

  if (DECL_P (*node))
    {
      tree decl = *node;
      type = TREE_TYPE (decl);

      if (TREE_CODE (decl) == TYPE_DECL
          || TREE_CODE (decl) == PARM_DECL
          || TREE_CODE (decl) == VAR_DECL
          || TREE_CODE (decl) == FUNCTION_DECL
          || TREE_CODE (decl) == FIELD_DECL)
        TREE_DEPRECATED (decl) = 1;
      else
        warn = 1;
    }
  else if (TYPE_P (*node))
    {
      if (!(flags & (int) ATTR_FLAG_TYPE_IN_PLACE))
        *node = build_variant_type_copy (*node);
      TREE_DEPRECATED (*node) = 1;
      type = *node;
    }
  else
    warn = 1;

  if (warn)
    {
      *no_add_attrs = true;
      if (type && TYPE_NAME (type))
        {
          if (TREE_CODE (TYPE_NAME (type)) == IDENTIFIER_NODE)
            what = TYPE_NAME (*node);
          else if (TREE_CODE (TYPE_NAME (type)) == TYPE_DECL
                   && DECL_NAME (TYPE_NAME (type)))
            what = DECL_NAME (TYPE_NAME (type));
        }
      if (what)
        warning (OPT_Wattributes, "%qE attribute ignored for %qE", identifier, what);
      else
        warning (OPT_Wattributes, "%qE attribute ignored", identifier);
    }

  return NULL_TREE;
}

/*
** Return nonzero if IDENTIFIER is a valid space attribute.
*/
tree mchp_space_attribute(tree *decl, tree identifier ATTRIBUTE_UNUSED,
                          tree args, int flags ATTRIBUTE_UNUSED,
                          bool *no_add_attrs)
{
  const char *attached_to = 0;
  const char *ident = NULL
                      ;
  if (args == NULL)
    {
      return NULL_TREE;
    }

  if (DECL_P(*decl))
    {
      attached_to = IDENTIFIER_POINTER(DECL_NAME(*decl));
    }

  /* we can assert one argument since that should be enforced by
     the parser from the attribute table */

  gcc_assert (TREE_CHAIN (args) == NULL);

  ident = IDENTIFIER_POINTER(DECL_NAME(*decl));

  if (DECL_P(*decl))
    {
      tree space;

      if (ignore_attribute("space", attached_to, *decl))
        {
          *no_add_attrs = 1;
          return NULL_TREE;
        }
      space = lookup_attribute("space",
                               DECL_ATTRIBUTES(*decl));

      if (space)
        {
          warning(0, "ignoring previous space attribute");
        }

        /*
	   space(data) attribute can be specified for functions.
           It will be treated as ramfunc.
           If region() attribute is specified, space(data) can be
           used to qualify the function to be copied to external
           data memory.
	  */
#if 0		
      if (TREE_CODE(*decl) == FUNCTION_DECL)
        {
          warning (0, "space attribute does not apply to functions");
        }
#endif		
      if (TREE_VALUE(args) == get_identifier("prog"))
        {
          return space;
        }
      else if (TREE_VALUE(args) == get_identifier("data"))
        {
          return space;
        }
      else if ((TREE_VALUE(args) == get_identifier("psv")) ||
               (TREE_VALUE(args) == get_identifier("auto_psv")))
        {
          /* Accept for C30 compatibility */
          warning (0, "Ignoring unavailable space(psv) attribute for '%s'. ", ident);
          inform (input_location, "Suggest space(prog) for this device instead.");
          TREE_VALUE(args) = get_identifier("prog");
          return NULL_TREE;
        }
    }
  if (TYPE_P(*decl))
    {
      if (TREE_VALUE(args) == get_identifier("prog"))
        {
          return NULL_TREE;
        }
      else if (TREE_VALUE(args) == get_identifier("data"))
        {
          return NULL_TREE;
        }
      else if ((TREE_VALUE(args) == get_identifier("psv")) ||
               (TREE_VALUE(args) == get_identifier("auto_psv")))
        {
          /* Accept for C30 compatibility */
          warning (0, "Ignoring unavailable space(psv) attribute for '%s'. ", ident);
          inform (input_location, "Suggest space(prog) for this device instead.");
          TREE_VALUE(args) = get_identifier("prog");
          return NULL_TREE;
        }
    }
  warning (0, "invalid space argument for '%s', ignoring attribute", attached_to);
  *no_add_attrs = 1;
  return NULL_TREE;
}

/*
  This function process the region attribute.
  The region attribute can be specified by any of the following method:

    #pragma region name="reg1" origin=0x08000000 size=0x200000
    int var __attribute__(( region("reg1") )) ;

*/

tree mchp_region_attribute( tree *decl, tree identifier ATTRIBUTE_UNUSED,
                            tree args, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs)
{
  const char *attached_to = 0;
  tree region;
  tree sub_arglist;
  tree sub_arg,sub_arg_arg;
  pic32_external_memory *memory = 0;

  if (args == NULL)
  {
    return NULL_TREE;
  }

/* 
  If LTO is enabled, then we ignore -flto and issue a warning.
*/
  if (flag_generate_lto)
  {
     flag_generate_lto = 0;
  }

  if (ignore_attribute("region", attached_to, *decl))
  {
    *no_add_attrs = 1;
    return NULL_TREE;
  }

  region = lookup_attribute("region", DECL_ATTRIBUTES(*decl));

  if (region)
  {
    warning(0, "ignoring previous region attribute");
  }

  if (DECL_P(*decl))
  {
    attached_to = IDENTIFIER_POINTER(DECL_NAME(*decl));
  }

  if (TREE_CODE(TREE_VALUE(args)) == STRING_CST)
  {
      pic32_external_memory *d;

      for (d = pic32_external_memory_head; d; d = d->next)
      {
        if (strcmp (d->name, TREE_STRING_POINTER(TREE_VALUE (args))) == 0) break;
      }

      if (d == 0)
      {
          error("sub argument to region() is not a region definition");
          *no_add_attrs=1;
      }

      return NULL_TREE;
  }
  else
  {
      error("invalid sub argument to region()");
      if (memory) free(memory);
      *no_add_attrs=1;
      return NULL_TREE;
  }
}



void
pic32_update_external_memory_info(const char* region_name, unsigned int region_origin, int region_size)
{
  pic32_external_memory *memory = 0;
  unsigned int origin;
  int size;

  if ((region_origin & 3) != 0)
  {
    error("invalid argument to origin; origin must be word aligned");
    origin = 0;
  }
  else
  {
    origin = region_origin;
  }

  if (region_size <= 0)
  {
    error("Invalid argument to size");
    size = 0;
  }
  else if ((region_size & 1) == 1)
  {
    error("invalid argument to size; size must be even");
    size = 0;
  }
  else
  {
    size = region_size;
  }

  memory = (pic32_external_memory *)
              xcalloc(sizeof(pic32_external_memory),1);
  memory->name = xstrndup(region_name, strlen(region_name)+1);
  memory->decl = NULL;
  memory->origin = origin;
  memory->size = size;

  if (pic32_external_memory_head == 0)
  {
    pic32_external_memory_head = memory;
    pic32_external_memory_tail = memory;
  }
  else
  {
    pic32_external_memory_tail->next = memory;
    pic32_external_memory_tail = memory;
  }
}

/*
** Return nonzero if IDENTIFIER is a valid attribute.
*/
tree mchp_target_error_attribute(tree *node, tree identifier ATTRIBUTE_UNUSED,
                                tree args, int flags ATTRIBUTE_UNUSED,
                                bool *no_add_attrs)
{
  const char *attached_to = IDENTIFIER_POINTER(DECL_NAME(*node));
  if (TREE_CODE(TREE_VALUE(args)) != STRING_CST) {
    error("invalid argument to 'target_error' attribute applied to '%s',"
          " literal string expected", attached_to);
    return NULL_TREE;
  }
  else
  {
    TREE_DEPRECATED(*node) = 1;
    return NULL_TREE;
  }
}

tree mchp_keep_attribute(tree *node, tree identifier ATTRIBUTE_UNUSED,
                                tree args, int flags ATTRIBUTE_UNUSED,
                                bool *no_add_attrs)
{
  DECL_COMMON (*node) = 0;
  DECL_UNIQUE_SECTION (*node) = 1;
  return NULL_TREE;
}


tree mchp_persistent_attribute(tree *node, tree identifier ATTRIBUTE_UNUSED,
                                tree args, int flags ATTRIBUTE_UNUSED,
                                bool *no_add_attrs)
{
  const char *attached_to = 0;
  
  DECL_UNIQUE_SECTION (*node) = 1;
  
  if (DECL_P(*node))
    {
      attached_to = IDENTIFIER_POINTER(DECL_NAME(*node));
    }
  /* The persistent attribute implies coherent for variables. */
  if ((TREE_CODE(*node) == VAR_DECL) &&
       !DECL_INITIAL(*node) &&
      (lookup_attribute ("coherent", DECL_ATTRIBUTES(*node)) == NULL) )
    {
      tree attrib_coherent = build_tree_list (get_identifier ("coherent"), NULL_TREE);
      attrib_coherent = chainon (DECL_ATTRIBUTES(*node), attrib_coherent);
      decl_attributes (node, attrib_coherent, 0);
      DECL_COMMON (*node) = 0;
    }   

  return NULL_TREE;
}

tree mchp_coherent_attribute(tree *node, tree identifier ATTRIBUTE_UNUSED,
                                tree args, int flags ATTRIBUTE_UNUSED,
                                bool *no_add_attrs)
{ 
  DECL_UNIQUE_SECTION (*node) = 1;
  DECL_COMMON (*node) = 0;

  if (TREE_CODE (*node) == VAR_DECL
      && current_function_decl != NULL_TREE
      && ! TREE_STATIC (*node))
    {
      error ("coherent attribute cannot be specified for "
             "local variables", *node);
      *no_add_attrs = true;
    }

  return NULL_TREE;
}

tree mchp_crypto_attribute(tree *node, tree identifier ATTRIBUTE_UNUSED,
                                tree args, int flags ATTRIBUTE_UNUSED,
                                bool *no_add_attrs)
{

    if (TREE_CODE (*node) != VAR_DECL || !TREE_READONLY(*node))
      error ("Attribute 'crypto' applies to const variables only");
    DECL_UNIQUE_SECTION (*node) = 1;
    DECL_COMMON (*node) = 0;

  return NULL_TREE;

}

/* Don't emit a function prologue. */
bool
mchp_suppress_prologue (void)
{
  /* if this function is specified as a naked function, just return without
     expanding any code */
  if (mchp_function_naked_p (current_function_decl))
    {
      return true;
    }
  return false;
}

/* Don't emit a function epilogue. */
bool
mchp_suppress_epilogue(void)
{
  /* if this function is specified as a naked function, just return without
     expanding any code */
  if (mchp_function_naked_p (current_function_decl))
    {
      return true;
    }
  return false;
}

bool
mchp_function_profiling_epilogue (bool sibcall_p)
{
  if (mchp_profile_option) {
      rtx insn, funct_sym;
      funct_sym = init_one_libfunc ("__function_level_profiling_long_zero");
        {
          rtx v0, v1, a0;
          a0 = gen_rtx_REG (Pmode, GP_ARG_FIRST);
          v1 = gen_rtx_REG (Pmode, V1_REGNUM);
          v0 = gen_rtx_REG (Pmode, V0_REGNUM);
          emit_insn (gen_blockage());
          insn = mips_expand_call (MIPS_CALL_EPILOGUE, NULL_RTX, funct_sym,
                                   const0_rtx, NULL_RTX, false);
          use_reg (&CALL_INSN_FUNCTION_USAGE (insn), a0);
          use_reg (&CALL_INSN_FUNCTION_USAGE (insn), v0);
          use_reg (&CALL_INSN_FUNCTION_USAGE (insn), v1);
          emit_insn (gen_blockage());
        }
  }
}

static bool
mchp_uses_fpu (void)
{
  enum machine_mode fpr_mode;
  int regno;
  fpr_mode = (TARGET_SINGLE_FLOAT ? SFmode : DFmode);
  for (regno = FP_REG_LAST - MAX_FPRS_PER_FMT + 1;
       regno >= FP_REG_FIRST;
       regno -= MAX_FPRS_PER_FMT)
    if (BITSET_P (cfun->machine->frame.fmask, regno - FP_REG_FIRST))
      {
        return true;
      }
  return false;
}

/* Expand the prologue into a bunch of separate insns at the end of the prologue.  */
void
mchp_expand_prologue_end (const struct mips_frame_info *frame)
{
  if (mchp_profile_option) {
      rtx insn, funct_sym;
      rtx v0, v1, a0;
      emit_insn (gen_blockage());
      a0 = gen_rtx_REG (Pmode, GP_ARG_FIRST);
      v1 = gen_rtx_REG (Pmode, V1_REGNUM);
      v0 = gen_rtx_REG (Pmode, V0_REGNUM);
      funct_sym = init_one_libfunc ("__function_level_profiling_long");
      insn = mips_expand_call (MIPS_CALL_PROLOGUE, NULL_RTX, funct_sym,
                               const0_rtx, NULL_RTX, false);
      use_reg (&CALL_INSN_FUNCTION_USAGE (insn), a0);
      use_reg (&CALL_INSN_FUNCTION_USAGE (insn), v1);
      use_reg (&CALL_INSN_FUNCTION_USAGE (insn), v0);
      emit_insn (gen_blockage());
    }
  if ((current_function_type == AUTO_CONTEXT_SAVE) ||
      ((current_function_type != NON_INTERRUPT) && frame->has_hilo_context))
    {
      /* Don't allow instructions from the function to be moved into the
         software context saving code. */
      emit_insn (gen_blockage ());
    }
}

void
mchp_expand_epilogue_restoreregs (HOST_WIDE_INT step1 ATTRIBUTE_UNUSED,
                                  HOST_WIDE_INT step2)
{
  const struct mips_frame_info *frame;
  int savecount = 0;
  unsigned int regno;

  HOST_WIDE_INT mchp_save_srsctl;
  
  mchp_save_srsctl = (((cfun->machine->interrupt_priority < 7)
                       && (0 == mchp_isr_backcompat))
                      || (current_function_type == AUTO_CONTEXT_SAVE)
                      || (current_function_type == DEFAULT_CONTEXT_SAVE));
  
  if (pic32_num_register_sets > 2)
    {
      mchp_save_srsctl    = 1;
      mchp_isr_backcompat = 0;
    }

  frame = &cfun->machine->frame;
  cfun->machine->frame.savedgpr = 0;

  if (GENERATE_MIPS16E_SAVE_RESTORE && frame->mask != 0)
    {
      unsigned int mask;
      HOST_WIDE_INT offset;
      rtx restore;

      /* Generate the restore instruction.  */
      mask = frame->mask;
      restore = mips16e_build_save_restore (true, &mask, &offset, 0, step2);

      /* Restore any other registers manually.  */
      for (regno = GP_REG_FIRST; regno < GP_REG_LAST; regno++)
        if (BITSET_P (mask, regno - GP_REG_FIRST))
          {
            offset -= UNITS_PER_WORD;
            mips_save_restore_reg (word_mode, regno, offset, mips_restore_reg);
          }

      /* Restore the remaining registers and deallocate the final bit
      of the frame.  */
      emit_insn (restore);
    }
  else
    {
      if (cfun->machine->interrupt_handler_p)
        {
          HOST_WIDE_INT offset;
          rtx mem;

          emit_insn (gen_blockage ());

          offset = frame->cop0_sp_offset - (frame->total_size - step2);

          if (SOFTWARE_CONTEXT_SAVE == cfun->machine->current_function_type)
            {
              /* Restore the registers.  */
              if (TARGET_HARD_FLOAT && cfun->machine->frame.fmask)
                {
                  /* Restore the FPU FCSR control and status register */
                  rtx insn;
                  gcc_assert(mchp_offset_fcsr != 0);
                  mips_save_restore_reg (SImode, V0_REGNUM, mchp_offset_fcsr, mips_restore_reg);
                  insn = gen_mips_set_fcsr (gen_rtx_REG (SImode, V0_REGNUM));
                  emit_insn (insn);
                  offset -= UNITS_PER_WORD;
                }
              if (TARGET_DSPR2 && cfun->machine->frame.acc_mask)
                {
                  rtx insn;
                  /* Restore the DSPControl register */
                  gcc_assert (mchp_offset_dspcontrol != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx, mchp_offset_dspcontrol));
                  mips_emit_move (gen_rtx_REG (word_mode, V1_REGNUM), mem);
                  insn = gen_mips_wrdsp (gen_rtx_REG (SImode, V1_REGNUM), GEN_INT (0x3Fu));
                  emit_insn (insn);
                  offset -= UNITS_PER_WORD;
                }
              mips_for_each_saved_acc (frame->total_size - step2, mips_restore_reg);
              mips_for_each_saved_gpr_and_fpr (frame->total_size - step2,
                                               mips_restore_reg, NULL_RTX);
              if (cfun->machine->interrupt_priority < 7)
                {
                  /* Load the original EPC.  */
                  gcc_assert (mchp_offset_epc != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx, mchp_offset_epc));
                  mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }

              /* Load the original Status.  */
              gcc_assert (mchp_offset_status >= 0);
              mem = gen_frame_mem (word_mode,
                                   plus_constant (word_mode,stack_pointer_rtx, mchp_offset_status));
              mips_emit_move (gen_rtx_REG (word_mode, K1_REG_NUM), mem);
              offset -= UNITS_PER_WORD;

              if (cfun->machine->interrupt_priority < 7)
                {
                  /* Restore the original EPC.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_EPC_REG_NUM),
                                            gen_rtx_REG (SImode, K0_REG_NUM)));
                }

              if (mchp_save_srsctl)
                {
                  /* Load the original SRSCTL.  */
                  gcc_assert (mchp_offset_srsctl != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx, mchp_offset_srsctl));
                  mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }
              /* If we don't use shadow register set, we need to update SP.  */
              if (step2 > 0)
                {
                  emit_insn (gen_add3_insn (stack_pointer_rtx,
                                            stack_pointer_rtx,
                                            GEN_INT (step2)));
                }

              if (mchp_save_srsctl)
                {
                  /* Restore previously loaded SRSCTL.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_SRSCTL_REG_NUM),
                                            gen_rtx_REG (SImode, K0_REG_NUM)));
                }
              emit_insn (gen_mips_wrpgpr (stack_pointer_rtx, stack_pointer_rtx));
              /* Restore previously loaded Status.  */
              emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_STATUS_REG_NUM),
                                        gen_rtx_REG (SImode, K1_REG_NUM)));

            } /* SOFTWARE_CONTEXT_SAVE */

          else if (SRS_CONTEXT_SAVE == cfun->machine->current_function_type)
            {
              gcc_assert (true == cfun->machine->use_shadow_register_set_p);
              /* Restore the registers.  */
              if (TARGET_HARD_FLOAT && cfun->machine->frame.fmask)
                {
                  /* Restore the FPU FCSR control and status register */
                  rtx insn;
                  gcc_assert(mchp_offset_fcsr != 0);
                  mips_save_restore_reg (SImode, V0_REGNUM, mchp_offset_fcsr, mips_restore_reg);
                  insn = gen_mips_set_fcsr (gen_rtx_REG (SImode, V0_REGNUM));
                  emit_insn (insn);
                  offset -= UNITS_PER_WORD;
                }
              if (TARGET_DSPR2 && cfun->machine->frame.acc_mask)
                {
                  rtx insn;
                  /* Restore the DSPControl register */
                  gcc_assert (mchp_offset_dspcontrol != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx, mchp_offset_dspcontrol));
                  mips_emit_move (gen_rtx_REG (word_mode, V1_REGNUM), mem);
                  insn = gen_mips_wrdsp (gen_rtx_REG (SImode, V1_REGNUM), GEN_INT (0x3Fu));
                  emit_insn (insn);
                  offset -= UNITS_PER_WORD;
                }
              mips_for_each_saved_acc (frame->total_size - step2, mips_restore_reg);
              mips_for_each_saved_gpr_and_fpr (frame->total_size - step2,
                                               mips_restore_reg, NULL_RTX);
              if (7 == cfun->machine->interrupt_priority)
                {
                  if (mchp_save_srsctl)
                    {
                      /* Load the original SRSCTL.  */
                      /* Since we are in IPL7, we can use K0/K1 without executing a DI first. */
                      gcc_assert (mchp_offset_srsctl != 0);
                      mem = gen_frame_mem (word_mode,
                                           plus_constant (word_mode,stack_pointer_rtx, mchp_offset_srsctl));
                      mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                      offset -= UNITS_PER_WORD;
                    }
                  /* Load the original Status.  */
                  gcc_assert (mchp_offset_status != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx, mchp_offset_status));
                  mips_emit_move (gen_rtx_REG (word_mode, K1_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }
              /* We will insert a DI here later. */
              if (cfun->machine->interrupt_priority < 7)
                {
                  if (mchp_save_srsctl)
                    {
                      /* Load the original SRSCTL to K1.  */
                      gcc_assert (mchp_offset_srsctl != 0);
                      mem = gen_frame_mem (word_mode,
                                           plus_constant (word_mode,stack_pointer_rtx, mchp_offset_srsctl));
                      mips_emit_move (gen_rtx_REG (word_mode, K1_REG_NUM), mem);
                      offset -= UNITS_PER_WORD;
                    }

                    /* Load the original EPC to K0.  */
                    gcc_assert (mchp_offset_epc != 0);
                    mem = gen_frame_mem (word_mode,
                                         plus_constant (word_mode,stack_pointer_rtx, mchp_offset_epc));
                    mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                    offset -= UNITS_PER_WORD;

                  if (mchp_save_srsctl)
                    {
                      /* Restore previously loaded SRSCTL.  */
                      emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_SRSCTL_REG_NUM),
                                                gen_rtx_REG (SImode, K1_REG_NUM)));
                    }
                  /* Load the original Status.  */
                  gcc_assert (mchp_offset_status != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx, mchp_offset_status));
                  mips_emit_move (gen_rtx_REG (word_mode, K1_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                  /* Restore the original EPC.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_EPC_REG_NUM),
                                            gen_rtx_REG (SImode, K0_REG_NUM)));
                }
              else /* (interrupt_priority == 7) */
                {
                  /* Do not preserve EPC for IPL7 */
                  /* k1 already holds STATUS */
                  /* k0 already holds SRSCTL */
                  if (mchp_save_srsctl)
                    {
                      /* Restore the original SRSCTL, which was loaded into $k0 above. */
                      emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_SRSCTL_REG_NUM),
                                                gen_rtx_REG (SImode, K0_REG_NUM)));
                    }
                }
              /* Update SP.  */
              if (step2 > 0)
                {
                  emit_insn (gen_add3_insn (stack_pointer_rtx,
                                            stack_pointer_rtx,
                                            GEN_INT (step2)));
                }
              emit_insn (gen_mips_wrpgpr (stack_pointer_rtx, stack_pointer_rtx));
              /* Restore previously loaded Status.  */
              emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_STATUS_REG_NUM),
                                        gen_rtx_REG (SImode, K1_REG_NUM)));
            } /* SRS_CONTEXT_SAVE */
          else if (AUTO_CONTEXT_SAVE == cfun->machine->current_function_type)
            {
              rtx skip_save_label;
              rtx do_save_label;
              rtx jumpskip;
              rtx cond;
              rtx insns;

              /* Restore the registers.  */
              if (TARGET_HARD_FLOAT && cfun->machine->frame.fmask)
                {
                  /* Restore the FPU FCSR control and status register */
                  rtx insn;
                  gcc_assert(mchp_offset_fcsr != 0);
                  mips_save_restore_reg (SImode, V0_REGNUM, mchp_offset_fcsr, mips_restore_reg);
                  insn = gen_mips_set_fcsr (gen_rtx_REG (SImode, V0_REGNUM));
                  emit_insn (insn);
                  offset -= UNITS_PER_WORD;
                }
              if (TARGET_DSPR2 && cfun->machine->frame.acc_mask)
                {
                  rtx insn;
                  /* Restore the DSPControl register */
                  gcc_assert (mchp_offset_dspcontrol != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx, mchp_offset_dspcontrol));
                  mips_emit_move (gen_rtx_REG (word_mode, V1_REGNUM), mem);
                  insn = gen_mips_wrdsp (gen_rtx_REG (SImode, V1_REGNUM), GEN_INT (0x3Fu));
                  emit_insn (insn);
                  offset -= UNITS_PER_WORD;
                }
              mips_for_each_saved_acc (frame->total_size - step2, mips_restore_reg);

              /* Load the original SRSCTL.  */
              gcc_assert (mchp_offset_srsctl != 0);
              mem = gen_frame_mem (word_mode,
                                   plus_constant (word_mode,stack_pointer_rtx, mchp_offset_srsctl));
              mips_emit_move (gen_rtx_REG (word_mode, V0_REGNUM), mem);
              emit_insn (gen_andsi3 (gen_rtx_REG (SImode, V0_REGNUM),
                                     gen_rtx_REG (SImode, V0_REGNUM), GEN_INT((unsigned)0x0F) ));
#if 1
              do_save_label = gen_label_rtx ();
              skip_save_label = gen_label_rtx ();
              emit_insn (gen_blockage());
              cond = gen_rtx_NE(SImode,gen_rtx_REG(SImode, V0_REGNUM),const0_rtx);
              jumpskip =  emit_jump_insn (gen_condjump (cond, skip_save_label));
              emit_insn (gen_blockage());
              JUMP_LABEL (jumpskip) = skip_save_label;

              LABEL_NUSES (skip_save_label) = 1;
              LABEL_NUSES (do_save_label) = 1;
              emit_label (do_save_label);
              mips_for_each_saved_gpr_and_fpr (frame->total_size - step2,
                                               mips_restore_reg, skip_save_label);
#endif
              if (cfun->machine->interrupt_priority < 7)
                {
                  /* Load the original EPC.  */
                  gcc_assert (mchp_offset_epc != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx, mchp_offset_epc));
                  mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }

              /* Load the original Status.  */
              gcc_assert (mchp_offset_status != 0);
              mem = gen_frame_mem (word_mode,
                                   plus_constant (word_mode,stack_pointer_rtx, mchp_offset_status));
              mips_emit_move (gen_rtx_REG (word_mode, K1_REG_NUM), mem);
              offset -= UNITS_PER_WORD;

              if (cfun->machine->interrupt_priority < 7)
                {
                  /* Restore the original EPC.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_EPC_REG_NUM),
                                            gen_rtx_REG (SImode, K0_REG_NUM)));
                }

              if (mchp_save_srsctl)
                {
                  /* Load the original SRSCTL.  */
                  gcc_assert (mchp_offset_srsctl != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx, mchp_offset_srsctl));
                  mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }
              /* we need to update SP.  */
              if (step2 > 0)
                {
                  emit_insn (gen_add3_insn (stack_pointer_rtx,
                                            stack_pointer_rtx,
                                            GEN_INT (step2)));
                }

              if (mchp_save_srsctl)
                {
                  /* Restore previously loaded SRSCTL.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_SRSCTL_REG_NUM),
                                            gen_rtx_REG (SImode, K0_REG_NUM)));
                }
              emit_insn (gen_mips_wrpgpr (stack_pointer_rtx, stack_pointer_rtx));
              /* Restore previously loaded Status.  */
              emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_STATUS_REG_NUM),
                                        gen_rtx_REG (SImode, K1_REG_NUM)));

            } /* AUTO_CONTEXT_SAVE */
          else if (DEFAULT_CONTEXT_SAVE == cfun->machine->current_function_type)
            {
              /* TODO Determine IPL at runtime */
              rtx skip_save_label;
              rtx do_save_label;
              rtx jumpskip;
              rtx cond;

              /* Restore the registers.  */
              if (TARGET_HARD_FLOAT && cfun->machine->frame.fmask)
                {
                  /* Restore the FPU FCSR control and status register */
                  rtx insn;
                  gcc_assert(mchp_offset_fcsr != 0);
                  mips_save_restore_reg (SImode, V0_REGNUM, mchp_offset_fcsr, mips_restore_reg);
                  insn = gen_mips_set_fcsr (gen_rtx_REG (SImode, V0_REGNUM));
                  emit_insn (insn);
                  offset -= UNITS_PER_WORD;
                }
              if (TARGET_DSPR2 && cfun->machine->frame.acc_mask)
                {
                  rtx insn;
                  /* Restore the DSPControl register */
                  gcc_assert (mchp_offset_dspcontrol != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx, mchp_offset_dspcontrol));
                  mips_emit_move (gen_rtx_REG (word_mode, V1_REGNUM), mem);
                  insn = gen_mips_wrdsp (gen_rtx_REG (SImode, V1_REGNUM), GEN_INT (0x3Fu));
                  emit_insn (insn);
                  offset -= UNITS_PER_WORD;
                }
              mips_for_each_saved_acc (frame->total_size - step2, mips_restore_reg);

              /* Load the original SRSCTL.  */
              gcc_assert (mchp_offset_srsctl != 0);
              mem = gen_frame_mem (word_mode,
                                   plus_constant (word_mode,stack_pointer_rtx, mchp_offset_srsctl));
              mips_emit_move (gen_rtx_REG (word_mode, V0_REGNUM), mem);
              emit_insn (gen_andsi3 (gen_rtx_REG (SImode, V0_REGNUM),
                                     gen_rtx_REG (SImode, V0_REGNUM), GEN_INT((unsigned)0x0F) ));
              do_save_label = gen_label_rtx ();
              skip_save_label = gen_label_rtx ();

              /* Count the number of GP registers to save */
              savecount = 0;
              for (regno = GP_REG_LAST; regno > GP_REG_FIRST; regno--)
                if (BITSET_P (cfun->machine->frame.mask, regno - GP_REG_FIRST))
                  {
                    savecount++;
                  }
#if 1
              /* Branch over the software context saving only if it will
                 save cycles */
              if (savecount > 3)
                {
                emit_insn (gen_blockage());
                  cond = gen_rtx_NE(SImode,gen_rtx_REG(SImode, V0_REGNUM),const0_rtx);
                  jumpskip =  emit_jump_insn (gen_condjump (cond, skip_save_label));
                  JUMP_LABEL (jumpskip) = skip_save_label;
                }
              LABEL_NUSES (skip_save_label) = 1;
              LABEL_NUSES (do_save_label) = 1;
              emit_label (do_save_label);
              mips_for_each_saved_gpr_and_fpr (frame->total_size - step2,
                                               mips_restore_reg, skip_save_label);
#endif
              if (cfun->machine->interrupt_priority < 7)
                {
                  /* Load the original EPC.  */
                  gcc_assert (mchp_offset_epc != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx, mchp_offset_epc));
                  mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }

              /* Load the original Status.  */
              gcc_assert (mchp_offset_status != 0);
              mem = gen_frame_mem (word_mode,
                                   plus_constant (word_mode,stack_pointer_rtx, mchp_offset_status));
              mips_emit_move (gen_rtx_REG (word_mode, K1_REG_NUM), mem);
              offset -= UNITS_PER_WORD;

              if (cfun->machine->interrupt_priority < 7)
                {
                  /* Restore the original EPC.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_EPC_REG_NUM),
                                            gen_rtx_REG (SImode, K0_REG_NUM)));
                }

              if (mchp_save_srsctl)
                {
                  /* Load the original SRSCTL.  */
                  gcc_assert (mchp_offset_srsctl != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx, mchp_offset_srsctl));
                  mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }
              /* If we don't use shadow register set, we need to update SP.  */
              if (step2 > 0)
                {
                  emit_insn (gen_add3_insn (stack_pointer_rtx,
                                            stack_pointer_rtx,
                                            GEN_INT (step2)));
                }

              if (mchp_save_srsctl)
                {
                  /* Restore previously loaded SRSCTL.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_SRSCTL_REG_NUM),
                                            gen_rtx_REG (SImode, K0_REG_NUM)));
                }
              emit_insn (gen_mips_wrpgpr (stack_pointer_rtx, stack_pointer_rtx));
              /* Restore previously loaded Status.  */
              emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_STATUS_REG_NUM),
                                        gen_rtx_REG (SImode, K1_REG_NUM)));
            }
          else /* unknown context-saving mechanism */
            {
              gcc_assert(0);
            }

        } /* (cfun->machine->interrupt_handler_p) */
      else /* not an interrupt */
        {
          /* Restore the registers.  */
          mips_for_each_saved_acc (frame->total_size - step2, mips_restore_reg);
          mips_for_each_saved_gpr_and_fpr (frame->total_size - step2,
                                           mips_restore_reg, NULL_RTX);
          /* Deallocate the final bit of the frame.  */
          if (step2 > 0)
            emit_insn (gen_add3_insn (stack_pointer_rtx,
                                      stack_pointer_rtx,
                                      GEN_INT (step2)));
        }
    }

  return;
}

void
mchp_output_function_epilogue (FILE *file ATTRIBUTE_UNUSED,
                               HOST_WIDE_INT size ATTRIBUTE_UNUSED)
{
  fprintf (file, "# Begin mchp_output_function_epilogue\n");
  if (cfun->machine->interrupt_handler_p && (0 < mips_noat.nesting_level))
    {
      mips_pop_asm_switch (&mips_noat);
    }

  fprintf (file, "# End mchp_output_function_epilogue\n");
}

/* Returns true if regno is a register ordinarilly not callee saved which
   must nevertheless be preserved by an interrupt handler function. */
inline int
mchp_register_interrupt_context_p (unsigned regno)
{
  if ((current_function_type == AUTO_CONTEXT_SAVE) ||
      (current_function_type == DEFAULT_CONTEXT_SAVE))
    {
      /* return true for at, a0-a3, t0-t9 and ra */
      /* Don't save v0-v1 because we save these in asm */
      if ((regno >= 4 && regno <= 15) /* a0-a3, t0-t7 */
          || regno == 1  /* at */
          || regno == 24 /* t8 */
          || regno == 25 /* t9 */
          || regno == 31) /* ra */
        {
          return 1;
        }
    }
  else if (current_function_type == SOFTWARE_CONTEXT_SAVE)
    {
      /* return true for at, a0-a3, t0-t9 and ra */
      /* Don't save v0-v1 because we save these in asm */
      if ((regno >= 4 && regno <= 15) /* a0-a3, t0-t7 */
          || regno == 1  /* at */
          || regno == 24 /* t8 */
          || regno == 25 /* t9 */
          || regno == 31) /* ra */
        {
          return 1;
        }
    }

  else
    {
      /* return true for v0, v1, a0-a3, t0-t9 and ra */
      if ((regno >= 1 && regno <= 15) /* v0, v1, a0-a3, t0-t7 */
          || regno == 24 /* t8 */
          || regno == 25 /* t9 */
          || regno == 31) /* ra */
        {
          return 1;
        }
    }

  return 0;
}

HOST_WIDE_INT
mchp_compute_frame_info (void)
{
  HOST_WIDE_INT num_intreg;     /* # words needed for interrupt context regs */
  bool use_v0v1_reg = false;    /* do we need to use v0/v1 to save other registers? */
  bool has_interrupt_context;   /* true if interrupt context is saved */
  bool has_hilo_context;        /* true if hi and lo registers are saved */
  static bool printed_error = false;
  tree ipl_tree;
  unsigned ipl_len;

  if (mchp_function_nofpu_p (TREE_TYPE (current_function_decl)) &&
     !mips_interrupt_type_p (TREE_TYPE (current_function_decl)))
    {
      if (!printed_error)
        error ("the %<no_fpu%> attribute requires the %<interrupt%> attribute");
      printed_error = true;
    }
  
  /* Reset warning counter when we move to a new function */
  static struct function *prevfun = NULL;
  if (prevfun != cfun)
    {
       mchp_invalid_ipl_warning = 0;
    }
  prevfun = cfun;

  /* Determine what type of interrupt was specified. */
  ipl_tree = mchp_function_interrupt_p (current_function_decl);
  if (current_function_type == UNKNOWN_CONTEXT_SAVE)
    {
      cfun->machine->use_shadow_register_set_p = false;
      cfun->machine->interrupt_handler_p = true;
      /* the priority can be either "single" or "ipl[0..7]"
         When the interrupt handler is for single interrupt mode, we
         treat it as a software context save handler. */
      gcc_assert (ipl_tree != NULL);

      if ((strcmp (IDENTIFIER_POINTER (TREE_VALUE (ipl_tree)), "single") == 0)
          || (strcmp (IDENTIFIER_POINTER (TREE_VALUE (ipl_tree)), "SINGLE") == 0)
          || (strcmp (IDENTIFIER_POINTER (TREE_VALUE (ipl_tree)), "ripl") == 0)
          || (strcmp (IDENTIFIER_POINTER (TREE_VALUE (ipl_tree)), "RIPL") == 0))
        {
          cfun->machine->current_function_type = current_function_type = DEFAULT_CONTEXT_SAVE;
          mchp_interrupt_priority = -1; /* Need to set IPL from RIPL */
          cfun->machine->interrupt_priority = -1;
          mchp_isr_backcompat = 0;
        }
      else
        {
          cfun->machine->interrupt_priority = mchp_interrupt_priority =
                                                IDENTIFIER_POINTER (TREE_VALUE (ipl_tree))[3] - '0';

          /* When the context-saving mode is not specified, assume that IPL7 uses
             SRS and other IPL levels use software. This assumption comes from the
             original Daytona family. The Talladega family introduces configurable
             shadow registers. You can map your shadow registers to an IPL level
             using configuration fuses. */
          ipl_len = strlen(IDENTIFIER_POINTER (TREE_VALUE (ipl_tree)));
          if (ipl_len == (sizeof("ipl7")-1))
            {
              if (cfun->machine->interrupt_priority == 7)
                {
                  cfun->machine->current_function_type = current_function_type = SRS_CONTEXT_SAVE;
                  cfun->machine->use_shadow_register_set_p = true;
                  mchp_isr_backcompat = 1; /* No need to save SRSCTL */
                }
              else
                {
                  cfun->machine->current_function_type = current_function_type = SOFTWARE_CONTEXT_SAVE;
                  mchp_isr_backcompat = 1; /* No need to save SRSCTL */
                }
            }
          else if ((ipl_len > sizeof("ipl7")) && (ipl_len < (sizeof("ipl7software"))))
            {
              mchp_isr_backcompat = 0; /* Might need to save SRSCTL */
              if ((NULL != strstr(IDENTIFIER_POINTER (TREE_VALUE (ipl_tree)),"SRS")) ||
                  (NULL != strstr(IDENTIFIER_POINTER (TREE_VALUE (ipl_tree)),"srs")))
                {
                  /* Must have a general register set + at least 1 shadow register set */
                  if (pic32_num_register_sets >= (1+1))
                    {
                      cfun->machine->current_function_type = current_function_type = SRS_CONTEXT_SAVE;
                      cfun->machine->use_shadow_register_set_p = true;
                      if (ipl_len > 7)
                        {
                          if (0 == mchp_invalid_ipl_warning)
                          {
                            warning(0, "Invalid ISR context-saving mode, assuming IPL%dSRS",
                              cfun->machine->interrupt_priority);
                            mchp_invalid_ipl_warning++;
                          }
                        }
                    }
                  else /* No shadow register sets */
                    {
                      if (0 == mchp_invalid_ipl_warning)
                        {
                          warning (0, "No Shadow Register Set available on selected %qs device, "
                                   "assuming IPL%d"  "SOFT",
                                   mchp_processor_string,
                                   cfun->machine->interrupt_priority);
                          mchp_invalid_ipl_warning++;
                        }
                      cfun->machine->use_shadow_register_set_p = false;
                      cfun->machine->current_function_type = current_function_type = SOFTWARE_CONTEXT_SAVE;
                    }
                }
              else if ((NULL != strstr(IDENTIFIER_POINTER
                                       (TREE_VALUE (ipl_tree)),"SOFT")) ||
                       (NULL != strstr(IDENTIFIER_POINTER
                                       (TREE_VALUE (ipl_tree)),"soft"))
                      )
                {
                  cfun->machine->current_function_type = current_function_type = SOFTWARE_CONTEXT_SAVE;
                  if ((0 == mchp_invalid_ipl_warning) && (pic32_num_register_sets == 8))
                    {
                      warning (0, "IPLnSOFT context saving used on a selected %qs device with 8 Shadow Register Sets, consider IPLnSRS\n", 
                               mchp_processor_string);
                      mchp_invalid_ipl_warning++;
                    }
                  if (ipl_len > sizeof("IPLnSOFT"))
                    {
                      if (0 == mchp_invalid_ipl_warning)
                        {
                          warning(0, "Invalid ISR context-saving mode, assuming IPL%dSOFT",
                                  cfun->machine->interrupt_priority);
                          mchp_invalid_ipl_warning++;
                        }
                    }
                }
              else if ((NULL != strstr(IDENTIFIER_POINTER
                                       (TREE_VALUE (ipl_tree)),"STACK")) ||
                       (NULL != strstr(IDENTIFIER_POINTER
                                       (TREE_VALUE (ipl_tree)),"stack")) )
                {
                  cfun->machine->current_function_type = current_function_type = SOFTWARE_CONTEXT_SAVE;
                  if (ipl_len > sizeof("IPLnSTACK"))
                    {
                      if (0 == mchp_invalid_ipl_warning)
                        {
                          warning(0, "Invalid ISR context-saving mode, assuming IPL%dSOFT ",
                           cfun->machine->interrupt_priority);
                          mchp_invalid_ipl_warning++;
                        }
                    }
                }
              else if ((NULL != strstr(IDENTIFIER_POINTER
                                       (TREE_VALUE (ipl_tree)),"AUTO")) ||
                       (NULL != strstr(IDENTIFIER_POINTER
                                       (TREE_VALUE (ipl_tree)),"auto")) )
                {
                  /* Determine shadow or SRS context save at runtime. */
                  cfun->machine->current_function_type = current_function_type = AUTO_CONTEXT_SAVE;
                }
              else
                {
                  if (0 == mchp_invalid_ipl_warning)
                    {
                      warning(0, "Invalid ISR context-saving mode, assuming IPL%dAUTO",
                       cfun->machine->interrupt_priority);
                      mchp_invalid_ipl_warning++;
                    }
                  /* Determine shadow or SRS context save at runtime. */
                  cfun->machine->current_function_type = current_function_type = AUTO_CONTEXT_SAVE;
                }
            }
          else
            {
              if (0 == mchp_invalid_ipl_warning)
                {
                  warning(0, "Invalid ISR context-saving mode, assuming IPL%dAUTO",
                          cfun->machine->interrupt_priority);
                  mchp_invalid_ipl_warning++;
                }
              /* Determine shadow or SRS context save at runtime. */
              cfun->machine->current_function_type = current_function_type = AUTO_CONTEXT_SAVE;
            }
        }
      if (mchp_function_naked_p (current_function_decl))
        error ("interrupt handler functions cannot also be naked functions");
    }
    
    if (current_function_type == SRS_CONTEXT_SAVE)
      cfun->machine->use_shadow_register_set_p = true;

  /* has_interrupt_context tells us whether we're saving any interrupt
     specific data */
  has_interrupt_context = (cfun->machine->current_function_type != NON_INTERRUPT);
  /* has_hilo_context is true if we need to push/pop HI and LO */
  has_hilo_context = 0;
  num_intreg = 0;
  use_v0v1_reg = false;

  /* add in space for the interrupt context information */
  if (has_interrupt_context)
    {
    
      if (pic32_num_register_sets > 2)
        {
          mchp_isr_backcompat = 0;
        }
    
      gcc_assert (cfun->machine->current_function_type != UNKNOWN_CONTEXT_SAVE);
      df_set_regs_ever_live (K0_REGNUM, true);
      fixed_regs[K0_REGNUM] = call_really_used_regs[K0_REGNUM] =
                                call_used_regs[K0_REGNUM] = 1;
      df_set_regs_ever_live (K1_REGNUM, true);
      fixed_regs[K1_REGNUM] = call_really_used_regs[K1_REGNUM] =
                                call_used_regs[K1_REGNUM] = 1;

      /* all AUTO and SRS interrupt context functions need space
         to preserve SRSCTL. */
      if ((current_function_type == AUTO_CONTEXT_SAVE) ||
          (current_function_type == DEFAULT_CONTEXT_SAVE))
        {
          num_intreg++;
          use_v0v1_reg = true;
          df_set_regs_ever_live (V1_REGNUM, true);
          fixed_regs[V1_REGNUM] = call_really_used_regs[V1_REGNUM] =
                                    call_used_regs[V1_REGNUM] = 1;
        }
      else if ((cfun->machine->current_function_type == SRS_CONTEXT_SAVE) ||
               (cfun->machine->current_function_type == SOFTWARE_CONTEXT_SAVE))
        {
          if (((cfun->machine->interrupt_priority < 7) && (0 == mchp_isr_backcompat)) ||
              (pic32_num_register_sets > 2))
            {
              /* If we are not in Daytona compatibility mode, save SRSCTL. */
              num_intreg++;
              use_v0v1_reg = true;
              df_set_regs_ever_live (V1_REGNUM, true);
              fixed_regs[V1_REGNUM] = call_really_used_regs[V1_REGNUM] =
                                        call_used_regs[V1_REGNUM] = 1;
            }
        }
      /* If HI/LO is defined in this function, we need to save them too. Also check the 
         DSPr2 accumulator pairs.
         If the function is not a leaf function, we assume that the
         called function uses them. */
      if (df_regs_ever_live_p (LO_REGNUM)       || df_regs_ever_live_p (HI_REGNUM)
          || df_regs_ever_live_p (AC1LO_REGNUM) || df_regs_ever_live_p (AC1HI_REGNUM)
          || df_regs_ever_live_p (AC2LO_REGNUM) || df_regs_ever_live_p (AC2HI_REGNUM)
          || df_regs_ever_live_p (AC3LO_REGNUM) || df_regs_ever_live_p (AC3HI_REGNUM)
          || !crtl->is_leaf)
        {
          has_hilo_context = 1;
        }
      /* Both AUTO and SOFTWARE reserve space to save v0/v1 if necessary */
      if ((cfun->machine->current_function_type == SRS_CONTEXT_SAVE) ||
          (cfun->machine->current_function_type == SOFTWARE_CONTEXT_SAVE) ||
          (cfun->machine->current_function_type == AUTO_CONTEXT_SAVE) ||
          (cfun->machine->current_function_type == DEFAULT_CONTEXT_SAVE) )
        {
          /* AUTO and SOFTWARE use v0/v1 to save hi/lo */
          if (has_hilo_context)
            {
              use_v0v1_reg = true;
              df_set_regs_ever_live (V0_REGNUM, true);
              fixed_regs[V0_REGNUM] = call_really_used_regs[V0_REGNUM] =
                                        call_used_regs[V0_REGNUM] = 1;
              df_set_regs_ever_live (V1_REGNUM, true);
              fixed_regs[V1_REGNUM] = call_really_used_regs[V1_REGNUM] =
                                        call_used_regs[V1_REGNUM] = 1;
            }
          if (TARGET_DSPR2 && has_hilo_context)
            {
              /* If we have the DSPr2 ASE, save DSPControl. */
              num_intreg++;
              use_v0v1_reg = true;
              df_set_regs_ever_live (V1_REGNUM, true);
              fixed_regs[V1_REGNUM] = call_really_used_regs[V1_REGNUM] =
                                      call_used_regs[V1_REGNUM] = 1;
             }
          if (TARGET_HARD_FLOAT && mchp_uses_fpu())
             {
               use_v0v1_reg = true;
               df_set_regs_ever_live (V0_REGNUM, true);
               fixed_regs[V0_REGNUM] = call_really_used_regs[V0_REGNUM] =
                                        call_used_regs[V0_REGNUM] = 1;
             }
        }
    }

  cfun->machine->frame.has_interrupt_context = has_interrupt_context;
  cfun->machine->frame.has_hilo_context = has_hilo_context;

  if (use_v0v1_reg == true)
    num_intreg = 2;

  if ((num_intreg % 2) != 0)
    num_intreg++;

  return num_intreg;
}

bool
mchp_subtarget_save_reg_p (unsigned int regno)
{
  /* When we are profiling, we need to preserve the RA register. */
  if (mchp_profile_option && (regno == RETURN_ADDR_REGNUM))
    return true;
  
  return false;
}

void
mchp_output_function_prologue (FILE *file, HOST_WIDE_INT tsize ATTRIBUTE_UNUSED, HOST_WIDE_INT size ATTRIBUTE_UNUSED)
{
  if (cfun->machine->current_function_type == SRS_CONTEXT_SAVE)
    fprintf (file, "# Begin mchp_output_function_prologue: SRS_CONTEXT_SAVE\n");
  else if (cfun->machine->current_function_type == SOFTWARE_CONTEXT_SAVE)
    fprintf (file, "# Begin mchp_output_function_prologue: SOFTWARE_CONTEXT_SAVE\n");
  else if (cfun->machine->current_function_type == AUTO_CONTEXT_SAVE)
    fprintf (file, "# Begin mchp_output_function_prologue: AUTO_CONTEXT_SAVE\n");
  else if (cfun->machine->current_function_type == DEFAULT_CONTEXT_SAVE)
    fprintf (file, "# Begin mchp_output_function_prologue: DEFAULT_CONTEXT_SAVE\n");
    
  if (cfun->machine->interrupt_handler_p && (0 == mips_noat.nesting_level))
    {
      mips_push_asm_switch (&mips_noat);
    }

  fprintf (file, "# End mchp_output_function_prologue\n");
}


static int
mchp_function_naked_p (tree func)
{
  tree a;
  /* according to the specification of the attribute, we shouldn't ever
     have it on a non-function declaration */
  gcc_assert (TREE_CODE (func) == FUNCTION_DECL);

  a = lookup_attribute ("naked", DECL_ATTRIBUTES (func));
  return a != NULL_TREE;
}

static int
mchp_persistent_p (tree decl)
{
  tree a;
  a = lookup_attribute ("persistent", DECL_ATTRIBUTES (decl));
  return a != NULL_TREE;
}

static int
mchp_target_error_p (tree decl)
{
  tree a;
  a = lookup_attribute ("target_error", DECL_ATTRIBUTES (decl));
  return a != NULL_TREE;
}

static int
mchp_keep_p (tree decl)
{
  tree a;
  a = lookup_attribute ("keep", DECL_ATTRIBUTES (decl));
  return (a != NULL_TREE);
}

static int
mchp_coherent_p (tree decl)
{
  tree a;
  a = lookup_attribute ("coherent", DECL_ATTRIBUTES (decl));
  return (a != NULL_TREE);
}

static int
mchp_crypto_p (tree decl)
{
  tree a;
  a = lookup_attribute ("crypto", DECL_ATTRIBUTES (decl));
  return a != NULL_TREE;
}

static tree
get_mchp_absolute_address (tree decl)
{
  tree retval;
  retval = lookup_attribute ("address", DECL_ATTRIBUTES (decl));

  return retval;
}

static tree
get_mchp_region_attribute (tree decl)
{
  return lookup_attribute ("region", DECL_ATTRIBUTES (decl));
}

static tree
get_mchp_space_attribute (tree decl)
{
  return lookup_attribute ("space", DECL_ATTRIBUTES (decl));
}

bool
mchp_ramfunc_type_p (tree decl)
{
  bool retval = FALSE;
  tree space = 0;
  tree region = 0;
  if (TREE_CODE(decl) == FUNCTION_DECL)
    {
      if (lookup_attribute ("ramfunc", TYPE_ATTRIBUTES (TREE_TYPE (decl))) != NULL)
        {
          retval = TRUE;
        }

      region = lookup_attribute("region", DECL_ATTRIBUTES(decl));
      space = lookup_attribute("space", DECL_ATTRIBUTES(decl));
      if (region)
      {
		  retval = FALSE ;
      }
      else if (space)
        {
          if (get_identifier("data") == (TREE_VALUE(TREE_VALUE(space))))
            retval = TRUE;
        }
    }
  return retval;
}

int
mchp_function_nofpu_p (tree type)
{
  tree a;

  a = lookup_attribute ("no_fpu", TYPE_ATTRIBUTES (type));
  return a != NULL_TREE;
}

/* Utility function to add an entry to the vector dispatch list */
void
mchp_add_vector_dispatch_entry (const char *target_name, int vector_number, bool fartype, 
                                enum pic32_isa_mode isamode, int emit)
{
  struct vector_dispatch_spec *dispatch_entry;

  for (dispatch_entry = vector_dispatch_list_head ;
       dispatch_entry ;
       dispatch_entry = dispatch_entry->next)
    {
          if (strcmp(dispatch_entry->target, target_name)==0)
            break;
    }

  if (dispatch_entry == NULL)
    {
      /* add the vector to the list of dispatch functions to emit */
      dispatch_entry = (struct vector_dispatch_spec *)xmalloc (sizeof (struct vector_dispatch_spec));
      dispatch_entry->next = vector_dispatch_list_head;
      dispatch_entry->target = target_name;
      dispatch_entry->emit_dispatch = emit;
      dispatch_entry->isr_isa_mode = pic32_isa_mips32r2;
      dispatch_entry->longcall = false;
    }
  if (fartype)
    dispatch_entry->longcall = true;
  if (vector_number >= 0)
    dispatch_entry->vector_number = vector_number;
  vector_dispatch_list_head = dispatch_entry;
  dispatch_entry->isr_isa_mode = isamode;
}

/* Expand the register-saving code in the function prologue */
HOST_WIDE_INT
mchp_expand_prologue_saveregs (HOST_WIDE_INT size, HOST_WIDE_INT step1)
{
  const struct mips_frame_info *frame;
  HOST_WIDE_INT nargs;
  rtx insn;
  int regno;
  frame = &cfun->machine->frame;
  cfun->machine->frame.savedgpr = 0;

  if (GENERATE_MIPS16E_SAVE_RESTORE)
    {
      HOST_WIDE_INT offset;
      unsigned int mask;

      /* Try to merge argument stores into the save instruction.  */
      nargs = mips16e_collect_argument_saves ();

      /* Build the save instruction.  */
      mask = frame->mask;
      insn = mips16e_build_save_restore (false, &mask, &offset,
                                         nargs, step1);
      RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
      size -= step1;

      /* Check that we need to save other registers.  */
      for (regno = GP_REG_FIRST; regno < GP_REG_LAST; regno++)
        if (BITSET_P (mask, regno - GP_REG_FIRST))
          {
            offset -= UNITS_PER_WORD;
            mips_save_restore_reg (word_mode, regno,
                                   offset, mips_save_reg);
          }
    }
  else
    {
      if (cfun->machine->interrupt_handler_p)
        {
          HOST_WIDE_INT offset;
          rtx mem;
          int interrupt_priority;
          enum mips_function_type_tag current_function_type;
          HOST_WIDE_INT mchp_save_srsctl;

          frame = &cfun->machine->frame;
          current_function_type = cfun->machine->current_function_type;
          interrupt_priority = cfun->machine->interrupt_priority;
          mchp_save_srsctl = (((interrupt_priority < 7)
                               && (0 == mchp_isr_backcompat))
                              || (current_function_type == AUTO_CONTEXT_SAVE)
                              || (current_function_type == DEFAULT_CONTEXT_SAVE));

          if (pic32_num_register_sets > 2)
            {
              mchp_save_srsctl    = 1;
              mchp_isr_backcompat = 0;
            }

          /* If this interrupt is using a shadow register set, we need to
          get the stack pointer from the previous register set. We want the
          first four instructions of the interrupt handler to be the same for
          all handler functions. This let's there be cache lines locked to
          those instructions, lowering the latency. */
          emit_insn (gen_mips_rdpgpr (stack_pointer_rtx,
                                      stack_pointer_rtx));

          if (SOFTWARE_CONTEXT_SAVE == current_function_type)
            {
              if (interrupt_priority < 7 || 1) /* Leave for cache purposes */
                {
                  /* Move from COP0 EPC to K0.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K0_REG_NUM),
                                            gen_rtx_REG (SImode,
                                                         COP0_EPC_REG_NUM)));
                }
              /* Move from COP0 STATUS to K1.  */
              emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K1_REG_NUM),
                                        gen_rtx_REG (SImode,
                                                     COP0_STATUS_REG_NUM)));
              /* Allocate the first part of the frame.  */
              insn = gen_add3_insn (stack_pointer_rtx, stack_pointer_rtx,
                                    GEN_INT (-step1));
              RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
              size -= step1;

              /* Start at the uppermost location for saving.  */
              offset = frame->cop0_sp_offset - size;
              gcc_assert (offset > 0);

              /* Don't save EPC if we know we won't get a nested interrupt. */
              if (interrupt_priority < 7)
                {
                  /* Push EPC into its stack slot.  */
                  gcc_assert (offset > 0);
                  mchp_offset_epc = offset;
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx,
                                                      mchp_offset_epc));
                  mips_emit_move (mem, gen_rtx_REG (word_mode, K0_REG_NUM));
                  offset -= UNITS_PER_WORD;
                }

              if (mchp_save_srsctl)
                {
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K0_REG_NUM),
                                            gen_rtx_REG (SImode,
                                                         COP0_SRSCTL_REG_NUM)));
                }
              /* Push STATUS into its stack slot.  */
              gcc_assert (offset >= 0);
              mchp_offset_status = offset;
              mem = gen_frame_mem (word_mode,
                                   plus_constant (word_mode,stack_pointer_rtx,
                                                  mchp_offset_status));
              mips_emit_move (mem, gen_rtx_REG (word_mode, K1_REG_NUM));
              offset -= UNITS_PER_WORD;

              if (mchp_save_srsctl)
                {
                  /* Push SRSCTL into its stack slot.  */
                  gcc_assert (offset > 0);
                  mchp_offset_srsctl = offset;
                  mips_save_restore_reg (word_mode, K0_REG_NUM, mchp_offset_srsctl, mips_save_reg);
                  offset -= UNITS_PER_WORD;
                }

              if ((interrupt_priority >= 0) || 1)
                {
                  gcc_assert (interrupt_priority >= 0);
                  gcc_assert (interrupt_priority <= 7);
                  
                  if (cfun->machine->keep_interrupts_masked_p)
                    {
                      /* Disable interrupts by clearing the KSU, ERL, EXL,
                         and IE bits.  */
                      emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                             GEN_INT (16),
                                             GEN_INT (SR_IE),
                                             gen_rtx_REG (SImode, GP_REG_FIRST)));
                    }
                  else
                    {
                      /* Clear UM, ERL, EXL, IPL in STATUS (K1) */
                      emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                             GEN_INT (15),
                                             GEN_INT (SR_EXL),
                                             gen_rtx_REG (SImode, GP_REG_FIRST)));
                    }
                  if (mchp_function_nofpu_p (TREE_TYPE (current_function_decl)))
                    {
                      /* Clear CU1 */
                      emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                             GEN_INT (1),
                                             GEN_INT (SR_CU1),
                                             gen_rtx_REG (SImode, GP_REG_FIRST)));
                    }
                  /* Set the IPL */
                  emit_insn (gen_iorsi3 (gen_rtx_REG (SImode, K1_REG_NUM),
                                         gen_rtx_REG (SImode, K1_REG_NUM),GEN_INT((unsigned)interrupt_priority << SR_IPL)));
                }
              /* We will move K1 to STATUS later in the generic MIPS code */
              emit_insn (gen_blockage ());
              mips_for_each_saved_gpr_and_fpr (size, mips_save_reg, NULL_RTX);
              /* Save HI/LO as late as possible to minimize stalls */
              mips_for_each_saved_acc (size, mips_save_reg);
              
              if (TARGET_DSPR2 && cfun->machine->frame.acc_mask)
                {
                  /* Save the DSPControl register */
                  rtx insn;
                  insn = gen_mips_rddsp (gen_rtx_REG (SImode, V1_REGNUM), GEN_INT (0x3Fu));
                  emit_insn (insn);
                  mchp_offset_dspcontrol = offset;
                  mips_save_restore_reg (SImode, V1_REGNUM, mchp_offset_dspcontrol, mips_save_reg);
                  offset -= UNITS_PER_WORD;
                }
              if (TARGET_HARD_FLOAT)
                {
                  rtx insn;
                  mchp_offset_fcsr = frame->fcsr_sp_offset;
                  if (mchp_uses_fpu()) {
                      insn = gen_mips_get_fcsr (gen_rtx_REG (SImode, V0_REGNUM));
                      emit_insn (insn);                 
                      mips_save_restore_reg (SImode, V0_REGNUM, mchp_offset_fcsr, mips_save_reg);
                    }
                  offset -= UNITS_PER_WORD;
                }
            } /* End SOFT context save */
          else if (SRS_CONTEXT_SAVE == current_function_type)
            {
              cfun->machine->use_shadow_register_set_p = true;

              if (interrupt_priority < 7 || 1) /* Leave for cache purposes */
                {
                  /* Move from COP0 EPC to K0.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K0_REG_NUM),
                                            gen_rtx_REG (SImode,
                                                         COP0_EPC_REG_NUM)));
                }
              /* Move from COP0 STATUS to K1.  */
              emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K1_REG_NUM),
                                        gen_rtx_REG (SImode,
                                                     COP0_STATUS_REG_NUM)));
              /* Allocate the first part of the frame.  */
              insn = gen_add3_insn (stack_pointer_rtx, stack_pointer_rtx,
                                    GEN_INT (-step1));
              RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
              size -= step1;

              /* Start at the uppermost location for saving.  */
              offset = frame->cop0_sp_offset - size;
              gcc_assert (offset > 0);
              if (mchp_save_srsctl)
                {
                  /* Push EPC into its stack slot.  */
                  gcc_assert (offset > 0);
                  mchp_offset_epc = offset;
                  mips_save_restore_reg (word_mode, K0_REG_NUM, mchp_offset_epc, mips_save_reg);
                  offset -= UNITS_PER_WORD;

                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K0_REG_NUM),
                                            gen_rtx_REG (SImode,
                                                         COP0_SRSCTL_REG_NUM)));
                }

              /* Push STATUS into its stack slot.  */
              gcc_assert (offset > 0);
              mchp_offset_status = offset;
              mips_save_restore_reg (word_mode, K1_REG_NUM, mchp_offset_status, mips_save_reg);
              offset -= UNITS_PER_WORD;

              if (mchp_save_srsctl)
                {
                  /* Push SRSCTL into its stack slot.  */
                  gcc_assert (offset > 0);
                  mchp_offset_srsctl = offset;
                  mips_save_restore_reg (word_mode, K0_REG_NUM, mchp_offset_srsctl, mips_save_reg);
                  offset -= UNITS_PER_WORD;
                }

              if ((interrupt_priority >= 0) || 1)
                {
                  gcc_assert (interrupt_priority >= 0);
                  gcc_assert (interrupt_priority <= 7);
                  if (cfun->machine->keep_interrupts_masked_p)
                    {
                      /* Disable interrupts by clearing the KSU, ERL, EXL,
                         and IE bits.  */
                      emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                             GEN_INT (16),
                                             GEN_INT (SR_IE),
                                             gen_rtx_REG (SImode, GP_REG_FIRST)));
                    }
                  else
                    {
                      /* Clear UM, ERL, EXL, IPL in STATUS (K1) */
                      emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                             GEN_INT (15),
                                             GEN_INT (SR_EXL),
                                             gen_rtx_REG (SImode, GP_REG_FIRST)));
                    }
                  if (mchp_function_nofpu_p (TREE_TYPE (current_function_decl)))
                    {
                      /* Clear CU1 */
                      emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                             GEN_INT (1),
                                             GEN_INT (SR_CU1),
                                             gen_rtx_REG (SImode, GP_REG_FIRST)));
                    }
                  /* Set the IPL */
                  emit_insn (gen_iorsi3 (gen_rtx_REG (SImode, K1_REG_NUM),
                                         gen_rtx_REG (SImode, K1_REG_NUM),GEN_INT((unsigned)interrupt_priority << SR_IPL)));
                }
              /* Should not save anything because we are using a shadow register set. */
              mips_for_each_saved_gpr_and_fpr (size, mips_save_reg, NULL_RTX);
              /* Save HI/LO as late as possible to minimize stalls */
              mips_for_each_saved_acc (size, mips_save_reg);
              
              if (TARGET_DSPR2 && cfun->machine->frame.acc_mask)
                {
                  /* Save the DSPControl register */
                  rtx insn;
                  insn =  gen_mips_rddsp (gen_rtx_REG (SImode, V1_REGNUM), GEN_INT (0x3Fu));
                  emit_insn (insn);
                  mchp_offset_dspcontrol = offset;
                  mips_save_restore_reg (SImode, V1_REGNUM, mchp_offset_dspcontrol, mips_save_reg);
                  offset -= UNITS_PER_WORD;
                }
              if (TARGET_HARD_FLOAT)
                {
                  /* Save the FPU FCSR control and status register */
                  rtx insn;
                  mchp_offset_fcsr = frame->fcsr_sp_offset;
                  if (mchp_uses_fpu()) {
                      insn = gen_mips_get_fcsr (gen_rtx_REG (SImode, V0_REGNUM));
                      emit_insn (insn);                 
                      mips_save_restore_reg (SImode, V0_REGNUM, mchp_offset_fcsr, mips_save_reg);
                    }
                  offset -= UNITS_PER_WORD;
                }
            }
          else if (AUTO_CONTEXT_SAVE == current_function_type)
            {
              rtx skip_save_label;
              rtx do_save_label;
              rtx jumpskip;
              rtx cond;
              int savecount = 0;

              if (interrupt_priority < 7 || 1) /* Leave for cache purposes */
                {
                  /* Move from COP0 EPC to K1.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K1_REG_NUM),
                                            gen_rtx_REG (SImode,
                                                         COP0_EPC_REG_NUM)));
                }
              /* Move from COP0 SRSCTL to K0.  */
              emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K0_REG_NUM),
                                        gen_rtx_REG (SImode,
                                                     COP0_SRSCTL_REG_NUM)));

              /* Allocate the first part of the frame.  */
              insn = gen_add3_insn (stack_pointer_rtx, stack_pointer_rtx,
                                    GEN_INT (-step1));
              RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
              size -= step1;

              /* Start at the uppermost location for saving.  */
              offset = frame->cop0_sp_offset - size;

              /* Don't save EPC if we know we won't get a nested interrupt. */
              if (interrupt_priority < 7)
                {
                  /* Push EPC into its stack slot.  */
                  mchp_offset_epc = offset;
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx,
                                                      mchp_offset_epc));
                  mips_emit_move (mem, gen_rtx_REG (word_mode, K1_REG_NUM));
                  offset -= UNITS_PER_WORD;
                }

              /* Move from COP0 STATUS to K1.  */
              emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K1_REG_NUM),
                                        gen_rtx_REG (SImode,
                                                     COP0_STATUS_REG_NUM)));

              /* Calculate offsets of status and srsctl so that that match the offsets
                 for SOFTWARE_CONTEXT_SAVE */
              mchp_offset_status = offset;
              offset -= UNITS_PER_WORD;
              mchp_offset_srsctl = offset;
              offset -= UNITS_PER_WORD;

              /* Push SRSCTL into its stack slot.  */
              mips_save_restore_reg (word_mode, K0_REG_NUM, mchp_offset_srsctl, mips_save_reg);


              /* TODO: Do we need to do this if interrupts are masked? */
              if (interrupt_priority < 0)
                {
                  /* Move from COP0 Cause to K0.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K0_REG_NUM),
                                            gen_rtx_REG (SImode,
                                                         COP0_CAUSE_REG_NUM)));
                }


              /* Push STATUS into its stack slot.  */

              mem = gen_frame_mem (word_mode,
                                   plus_constant (word_mode,stack_pointer_rtx,
                                                  mchp_offset_status));
              mips_emit_move (mem, gen_rtx_REG (word_mode, K1_REG_NUM));


              if (interrupt_priority < 0)
                {
                  /* Right justify the CAUSE RIPL in k0.  */
                  emit_insn (gen_lshrsi3 (gen_rtx_REG (SImode, K0_REG_NUM),
                                          gen_rtx_REG (SImode, K0_REG_NUM),
                                          GEN_INT (CAUSE_IPL)));
                  /* Insert the RIPL into our copy of SR (k1) as the new IPL.  */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (6),
                                         GEN_INT (SR_IPL),
                                         gen_rtx_REG (SImode, K0_REG_NUM)));
                }

              if (interrupt_priority >= 0)
                {
                  gcc_assert (interrupt_priority <= 7);
                  
                  if (cfun->machine->keep_interrupts_masked_p)
                    {
                      /* Disable interrupts by clearing the KSU, ERL, EXL,
                         and IE bits.  */
                      emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                             GEN_INT (16),
                                             GEN_INT (SR_IE),
                                             gen_rtx_REG (SImode, GP_REG_FIRST)));
                    }
                  else
                    {
                      /* Clear UM, ERL, EXL, IPL in STATUS (K1) */
                      emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                             GEN_INT (15),
                                             GEN_INT (SR_EXL),
                                             gen_rtx_REG (SImode, GP_REG_FIRST)));
                    }
                  if (mchp_function_nofpu_p (TREE_TYPE (current_function_decl)))
                    {
                      /* Clear CU1 */
                      emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                             GEN_INT (1),
                                             GEN_INT (SR_CU1),
                                             gen_rtx_REG (SImode, GP_REG_FIRST)));
                    }
                  /* Set the IPL */
                  emit_insn (gen_iorsi3 (gen_rtx_REG (SImode, K1_REG_NUM),
                                         gen_rtx_REG (SImode, K1_REG_NUM),GEN_INT((unsigned)interrupt_priority << SR_IPL)));
                }
              else
                {
                  /* Clear UM, ERL, EXL, IPL in STATUS */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (4),
                                         GEN_INT (SR_EXL),
                                         gen_rtx_REG (SImode, GP_REG_FIRST)));
                }
              if (mchp_function_nofpu_p (TREE_TYPE (current_function_decl)))
                {
                  /* Clear CU1 */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (1),
                                         GEN_INT (SR_CU1),
                                         gen_rtx_REG (SImode, GP_REG_FIRST)));
                }
              /* We will move K1 to STATUS later in the generic MIPS code */
              emit_insn (gen_blockage ());

              /* Save v0 and v1 here, but on the proper stack location */
              cfun->machine->frame.mask |= (1 << V0_REGNUM);
              cfun->machine->frame.mask |= (1 << V1_REGNUM);

              offset = cfun->machine->frame.gp_sp_offset - size;
              savecount = 0;
              for (regno = GP_REG_LAST; regno > GP_REG_FIRST; regno--)
                if (BITSET_P (cfun->machine->frame.mask, regno - GP_REG_FIRST))
                  {
                    if ((regno == V1_REGNUM) || (regno == V0_REGNUM))
                      {
                        mips_save_restore_reg (word_mode, regno,
                                               offset, mips_save_reg);
                        cfun->machine->frame.savedgpr |= (1 << regno);
                      }
                    savecount++;
                    offset -= UNITS_PER_WORD;
                  }

              mem = gen_frame_mem (word_mode,
                                   plus_constant (word_mode,stack_pointer_rtx,
                                                  mchp_offset_srsctl));
              mips_emit_move (gen_rtx_REG (word_mode, V1_REGNUM), mem);
              emit_insn (gen_andsi3 (gen_rtx_REG (SImode, V1_REGNUM), gen_rtx_REG (SImode, V1_REGNUM), GEN_INT((unsigned)0x0F) ));
              do_save_label = gen_label_rtx ();
              skip_save_label = gen_label_rtx ();

              if (savecount > 3)
                {
                  cond = gen_rtx_NE(SImode,gen_rtx_REG(SImode, V1_REGNUM),const0_rtx);
                  jumpskip =  emit_jump_insn (gen_condjump (cond, skip_save_label));
                  JUMP_LABEL (jumpskip) = skip_save_label;
                }
              LABEL_NUSES (skip_save_label) = 1;

              emit_insn (gen_blockage ());
              emit_label (do_save_label);
              emit_insn (gen_blockage ());
              mips_for_each_saved_gpr_and_fpr (size, mips_save_reg, skip_save_label);

              /* Save HI/LO as late as possible to minimize stalls */
              mips_for_each_saved_acc (size, mips_save_reg);
              
              if (TARGET_DSPR2 && cfun->machine->frame.acc_mask)
                {
                  /* Save the DSPControl register */
                  rtx insn;
                  insn = gen_mips_rddsp (gen_rtx_REG (SImode, V1_REGNUM), GEN_INT (0x3Fu));
                  emit_insn (insn);
                  mchp_offset_dspcontrol = offset;
                  mips_save_restore_reg (SImode, V1_REGNUM, mchp_offset_dspcontrol, mips_save_reg);
                  offset -= UNITS_PER_WORD;
                }
              if (TARGET_HARD_FLOAT)
                {
                  /* Save the FPU FCSR control and status register */
                  rtx insn;
                  mchp_offset_fcsr = frame->fcsr_sp_offset;
                  if (mchp_uses_fpu()) {
                      insn = gen_mips_get_fcsr (gen_rtx_REG (SImode, V0_REGNUM));
                      emit_insn (insn);                 
                      mips_save_restore_reg (SImode, V0_REGNUM, mchp_offset_fcsr, mips_save_reg);
                    }
                  offset -= UNITS_PER_WORD;
                }
              
            } /* AUTO_CONTEXT_SAVE */
          else if (DEFAULT_CONTEXT_SAVE == cfun->machine->current_function_type)
            {
              rtx skip_save_label;
              rtx do_save_label;
              rtx jumpskip;
              rtx cond;
              int savecount = 0;

              if (interrupt_priority < 7 || 1) /* Leave for cache purposes */
                {
                  /* Move from COP0 EPC to K1.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K1_REG_NUM),
                                            gen_rtx_REG (SImode,
                                                         COP0_EPC_REG_NUM)));
                }

              /* Move from COP0 SRSCTL to K0.  */
              emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K0_REG_NUM),
                                        gen_rtx_REG (SImode,
                                                     COP0_SRSCTL_REG_NUM)));


              /* Allocate the first part of the frame.  */
              insn = gen_add3_insn (stack_pointer_rtx, stack_pointer_rtx,
                                    GEN_INT (-step1));
              RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
              size -= step1;

              /* Start at the uppermost location for saving.  */
              offset = frame->cop0_sp_offset - size;

              /* Don't save EPC if we know we won't get a nested interrupt. */
              if (interrupt_priority < 7)
                {
                  /* Push EPC into its stack slot.  */
                  mchp_offset_epc = offset;
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (word_mode,stack_pointer_rtx,
                                                      mchp_offset_epc));
                  mips_emit_move (mem, gen_rtx_REG (word_mode, K1_REG_NUM));
                  offset -= UNITS_PER_WORD;
                }

              /* Move from COP0 STATUS to K1.  */
              emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K1_REG_NUM),
                                        gen_rtx_REG (SImode,
                                                     COP0_STATUS_REG_NUM)));

              /* Push SRSCTL into its stack slot.  */
              mchp_offset_srsctl = offset;
              mem = gen_frame_mem (word_mode,
                                   plus_constant (word_mode,stack_pointer_rtx,
                                                  mchp_offset_srsctl));
              mips_emit_move (mem, gen_rtx_REG (word_mode, K0_REG_NUM));
              offset -= UNITS_PER_WORD;

              /* Move from COP0 Cause to K0.  */
              emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K0_REG_NUM),
                                        gen_rtx_REG (SImode,
                                                     COP0_CAUSE_REG_NUM)));

              /* Push STATUS into its stack slot.  */
              mchp_offset_status = offset;
              mem = gen_frame_mem (word_mode,
                                   plus_constant (word_mode,stack_pointer_rtx,
                                                  mchp_offset_status));
              mips_emit_move (mem, gen_rtx_REG (word_mode, K1_REG_NUM));
              offset -= UNITS_PER_WORD;

              /* Right justify the CAUSE RIPL in k0.  */
              emit_insn (gen_lshrsi3 (gen_rtx_REG (SImode, K0_REG_NUM),
                                      gen_rtx_REG (SImode, K0_REG_NUM),
                                      GEN_INT (CAUSE_IPL)));

                  /* Insert the RIPL into our copy of SR (k1) as the new IPL.  */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (6),
                                         GEN_INT (SR_IPL),
                                         gen_rtx_REG (SImode, K0_REG_NUM)));

              if (cfun->machine->keep_interrupts_masked_p)
                {
                  /* Disable interrupts by clearing the KSU, ERL, EXL,
                     and IE bits.  */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (5),
                                         GEN_INT (SR_IE),
                                         gen_rtx_REG (SImode, GP_REG_FIRST)));
                  /* We will move K1 to STATUS later in the generic MIPS code */
                }
              else /* cfun->machine->keep_interrupts_masked_p */
                {
                  /* Enable interrupts by clearing the KSU ERL and EXL bits.
                     IE is already the correct value, so we don't have to do
                     anything explicit.  */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (4),
                                         GEN_INT (SR_EXL),
                                         gen_rtx_REG (SImode, GP_REG_FIRST)));
                  /* We will move K1 to STATUS later in the generic MIPS code */
                }
              if (mchp_function_nofpu_p (TREE_TYPE (current_function_decl)))
                {
                  /* Clear CU1 */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (1),
                                         GEN_INT (SR_CU1),
                                         gen_rtx_REG (SImode, GP_REG_FIRST)));
                }
              /* We will move K1 to STATUS later in the generic MIPS code */
              emit_insn (gen_blockage ());

              /* Save v0 and v1 here, but on the proper stack location */
              cfun->machine->frame.mask |= (1 << V0_REGNUM);
              cfun->machine->frame.mask |= (1 << V1_REGNUM);

              offset = cfun->machine->frame.gp_sp_offset - size;
              savecount = 0;
              for (regno = GP_REG_LAST; regno > GP_REG_FIRST; regno--)
                if (BITSET_P (cfun->machine->frame.mask, regno - GP_REG_FIRST))
                  {
                    if ((regno == V1_REGNUM) || (regno == V0_REGNUM))
                      {
                        mips_save_restore_reg (word_mode, regno,
                                               offset, mips_save_reg);
                        cfun->machine->frame.savedgpr |= (1 << regno);
                      }
                    savecount++;
                    offset -= UNITS_PER_WORD;
                  }

              mem = gen_frame_mem (word_mode,
                                   plus_constant (word_mode,stack_pointer_rtx,
                                                  mchp_offset_srsctl));
              mips_emit_move (gen_rtx_REG (word_mode, V1_REGNUM), mem);
              emit_insn (gen_andsi3 (gen_rtx_REG (SImode, V1_REGNUM), gen_rtx_REG (SImode, V1_REGNUM), GEN_INT((unsigned)0x0F) ));
              do_save_label = gen_label_rtx ();
              skip_save_label = gen_label_rtx ();

              if (savecount > 3)
                {
                  cond = gen_rtx_NE(SImode,gen_rtx_REG(SImode, V1_REGNUM),const0_rtx);
                  jumpskip =  emit_jump_insn (gen_condjump (cond, skip_save_label));
                  JUMP_LABEL (jumpskip) = skip_save_label;
                }

              emit_insn (gen_blockage ());
              emit_label (do_save_label);
              mips_for_each_saved_gpr_and_fpr (size, mips_save_reg, skip_save_label);

              emit_insn (gen_blockage ());

              /* Save HI/LO as late as possible to minimize stalls */
              mips_for_each_saved_acc (size, mips_save_reg);
              
              if (TARGET_DSPR2 && cfun->machine->frame.acc_mask)
                {
                  /* Save the DSPControl register */
                  rtx insn;
                  insn = gen_mips_rddsp (gen_rtx_REG (SImode, V1_REGNUM), GEN_INT (0x3Fu));
                  emit_insn (insn);
                  mchp_offset_dspcontrol = offset;
                  mips_save_restore_reg (SImode, V1_REGNUM, mchp_offset_dspcontrol, mips_save_reg);
                  offset -= UNITS_PER_WORD;
                }
              if (TARGET_HARD_FLOAT)
                {
                  /* Save the FPU FCSR control and status register */
                  rtx insn;
                  mchp_offset_fcsr = frame->fcsr_sp_offset;
                  if (mchp_uses_fpu()) {
                      insn = gen_mips_get_fcsr (gen_rtx_REG (SImode, V0_REGNUM));
                      emit_insn (insn);                 
                      mips_save_restore_reg (SImode, V0_REGNUM, mchp_offset_fcsr, mips_save_reg);
                    }
                  offset -= UNITS_PER_WORD;
                }
            }
        }
      else /* not an interrupt */
        {
          insn = gen_add3_insn (stack_pointer_rtx,
                                stack_pointer_rtx,
                                GEN_INT (-step1));
          RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
          size -= step1;


          mips_for_each_saved_acc (size, mips_save_reg);
          mips_for_each_saved_gpr_and_fpr (size, mips_save_reg, NULL_RTX);
        }
    }
  return size;
}

bool mchp_subtarget_mips16_enabled (const_tree decl)
{
  static const_tree first_disabled_decl = NULL;
  static const_tree last_disabled_decl = NULL;
  static bool suppress_further_warnings = false;
  bool disable_mips16;

  disable_mips16 = (mchp_pic32_license_valid < 2) || !(pic32_device_mask & HAS_MIPS16);

  if (disable_mips16)
    {
      if ((decl == first_disabled_decl) ||
          (decl == last_disabled_decl))
        {
          suppress_further_warnings = true;
        }

      if (false == suppress_further_warnings)
        {
          if (mchp_pic32_license_valid < 2)
            warning (0, "The current compiler license does not support the %<mips16%>"
                         " attribute on %qs, attribute ignored",
                         IDENTIFIER_POINTER (DECL_NAME (decl)));
          else if (!(pic32_device_mask & HAS_MIPS16))
            error ("The %qs target device does not support the %<mips16%>"
                         " attribute on %qs",
                         mchp_processor_string,
                         IDENTIFIER_POINTER (DECL_NAME (decl)));

          if (NULL == first_disabled_decl)
            {
              first_disabled_decl = decl;
            }
          last_disabled_decl = decl;
        }

      return false;
    }
  else
    {
      return true;
    }
}

bool mchp_subtarget_micromips_enabled (const_tree decl)
{
  static const_tree first_disabled_decl = NULL;
  static const_tree last_disabled_decl = NULL;
  static bool suppress_further_warnings = false;
  bool disable_micromips;

  disable_micromips = (mchp_pic32_license_valid < 2) || !(pic32_device_mask & HAS_MICROMIPS);
  /* If the device does not support MIPS32R2, don't disable microMIPS */
  if (!(pic32_device_mask & HAS_MIPS32R2))
    {
      disable_micromips = false;
    }

  if (disable_micromips)
    {
      if ((decl == first_disabled_decl) ||
          (decl == last_disabled_decl))
        {
          suppress_further_warnings = true;
        }

      if (false == suppress_further_warnings)
        {
          if (mchp_pic32_license_valid < 2)
            warning (0, "The current compiler license does not support the %<micromips%>"
                         " attribute on %qs, attribute ignored",
                         IDENTIFIER_POINTER (DECL_NAME (decl)));
          else if (!(pic32_device_mask & HAS_MICROMIPS))
            error ("The %qs target device does not support the %<micromips%>"
                         " attribute on %qs",
                         mchp_processor_string,
                         IDENTIFIER_POINTER (DECL_NAME (decl)));

          if (NULL == first_disabled_decl)
            {
              first_disabled_decl = decl;
            }
          last_disabled_decl = decl;
        }

      return false;
    }
  else
    {
      return true;
    }
}

bool mchp_subtarget_mips32_enabled ()
{
  if (pic32_loaded_device_mask && !(pic32_device_mask & HAS_MIPS32R2))
    {
      return false;
    }
  return true;
}

#if defined(C32_SMARTIO_RULES) || 1

#define CCS_FLAG_MASK (~(conv_c-1))
#define CCS_STATE_MASK (conv_c-1)
#define CCS_FLAG(X) ((X) & CCS_FLAG_MASK)
#define CCS_STATE(X) ((X) & CCS_STATE_MASK)

/* Must be sorted */
static mchp_interesting_fn mchp_fn_list[] =
{
  /*  name         map_to        style          arg c, conv_flags */
  { "_dasprintf",  "_dasprintf", info_dbl,    5,  0, (mchp_conversion_status)0, NULL },
  { "_dfprintf",   "_dfprintf",  info_dbl,    5,  0, (mchp_conversion_status)0, NULL },
  { "_dfscanf",    "_dfscanf",   info_dbl,    5,  0, (mchp_conversion_status)0, NULL },
  { "_dprintf",    "_dprintf",   info_dbl,    4,  0, (mchp_conversion_status)0, NULL },
  { "_dscanf",     "_dscanf",    info_dbl,    4,  0, (mchp_conversion_status)0, NULL },
  { "_dsnprintf",  "_dsnprintf", info_dbl,    6,  0, (mchp_conversion_status)0, NULL },
  { "_dsprintf",   "_dsprintf",  info_dbl,    5,  0, (mchp_conversion_status)0, NULL },
  { "_dsscanf",    "_dsscanf",   info_dbl,    5,  0, (mchp_conversion_status)0, NULL },
  { "_dvasprintf", "_dvasprintf",info_dbl,    5,  0, (mchp_conversion_status)0, NULL },
  { "_dvfprintf",  "_dvfprintf", info_dbl,    5,  0, (mchp_conversion_status)0, NULL },
  { "_dvfscanf",   "_dvfscanf",  info_dbl,    5,  0, (mchp_conversion_status)0, NULL },
  { "_dvprintf",   "_dvprintf",  info_dbl,    4,  0, (mchp_conversion_status)0, NULL },
  { "_dvsprintf",  "_dvsprintf", info_dbl,    5,  0, (mchp_conversion_status)0, NULL },
  { "_dvsscanf",   "_dvsscanf",  info_dbl,    5,  0, (mchp_conversion_status)0, NULL },
  { "asprintf",  "_asprintf",  info_O,        5,  0, (mchp_conversion_status)0, NULL },
  { "fprintf",   "_fprintf",   info_O,        5,  0, (mchp_conversion_status)0, NULL },
  { "fscanf",    "_fscanf",    info_I,        5,  0, (mchp_conversion_status)0, NULL },
  { "printf",    "_printf",    info_O,        4,  0, (mchp_conversion_status)0, NULL },
  { "scanf",     "_scanf",     info_I,        4,  0, (mchp_conversion_status)0, NULL },
  { "snprintf",  "_snprintf",  info_O,        6,  0, (mchp_conversion_status)0, NULL },
  { "sprintf",   "_sprintf",   info_O,        5,  0, (mchp_conversion_status)0, NULL },
  { "sscanf",    "_sscanf",    info_I,        5,  0, (mchp_conversion_status)0, NULL },
  { "vasprintf", "_vasprintf", info_O,        5,  0, (mchp_conversion_status)0, NULL },
  { "vfprintf",  "_vfprintf",  info_O,        5,  0, (mchp_conversion_status)0, NULL },
  { "vfscanf",   "_vfscanf",   info_I,        5,  0, (mchp_conversion_status)0, NULL },
  { "vprintf",   "_vprintf",   info_O,        4,  0, (mchp_conversion_status)0, NULL },
  { "vsnprintf", "_vsnprintf", info_O,        6,  0, (mchp_conversion_status)0, NULL },
  { "vsprintf",  "_vsprintf",  info_O,        5,  0, (mchp_conversion_status)0, NULL },
  { "vsscanf",   "_vsscanf",   info_I,        5,  0, (mchp_conversion_status)0, NULL },
  { 0,           0,            (mchp_interesting_fn_info)0,            -1,  0, (mchp_conversion_status)0, NULL }
};

/*
 *  strings values are thrown away after they are generated, but the
 *    a reference to the string will always return the same rtx... keep
 *    track of them here and they're conversion state
 */
enum { status_output = 0,
       status_input = 1
     };

typedef struct mchp_conversion_cache_
{
  rtx rtl;
  mchp_conversion_status valid[2];
  struct mchp_conversion_cache_ *l,*r;
} mchp_conversion_cache;

int mchp_clear_fn_list=1;

static mchp_conversion_cache *mchp_saved_conversion_info;

static const char *mchp_strip_name_encoding_helper(const char *symbol_name)
{
  const char *var;
  int sz = 0;

  var = symbol_name;
  while ((sz = ENCODED_NAME_P(var)))
    {
      var = var + ENCODED_NAME_P(symbol_name);
      var += (*var == '*');
    }
  return var;
}

/* By default, we do nothing for encode_section_info, so we need not
   do anything but discard the '*' marker.  */

const char *
mchp_strip_name_encoding (const char *symbol_name)
{
  const char *var;
  mchp_interesting_fn *match;
  var = mchp_strip_name_encoding_helper(symbol_name);

  if (mchp_io_size_val > 0)
    {
      match = mchp_match_conversion_fn(var);
      while (match)
        {
          if (match->function_convertable)
            {

#define CCS_ADD_FLAG(FLAG) \
        if (match->conv_flags & JOIN(conv_,FLAG)) { \
          *f++=#FLAG[0]; \
          added = (mchp_conversion_status) (added | JOIN(conv_,FLAG)); }

#define CCS_ADD_FLAG_ALT(FLAG,ALT) \
        if ((match->conv_flags & JOIN(conv_,FLAG)) && \
            ((added & JOIN(conv_,ALT)) == 0)) {\
          *f++=#ALT[0]; \
          added =(mchp_conversion_status) (added |  (mchp_conversion_status)JOIN(conv_,ALT)); }

              {
                char extra_flags[sizeof("_aAcdeEfFgGnopsuxX0")] = "_";
                char *f = &extra_flags[1];
                mchp_conversion_status added;
                /*
                 * order is important here
                 *  add new flags alphabetically with lower case preceding uppercase
                 *    ie _aAcdEfgG not
                 *       _acdfgAEG
                 */

                added = (mchp_conversion_status)0;
                /*
                 * we don't implement all 131K unique combinations, only
                 * a subset...
                */

                /* a | A -> aA */
                CCS_ADD_FLAG(a);
                CCS_ADD_FLAG_ALT(A,a);
                CCS_ADD_FLAG(A);
                CCS_ADD_FLAG_ALT(a,A);

                /* c | d | n | o | p | u | x | X -> cdnopuxX */
                CCS_ADD_FLAG(c);
                CCS_ADD_FLAG_ALT(d,c);
                CCS_ADD_FLAG_ALT(n,c);
                CCS_ADD_FLAG_ALT(o,c);
                CCS_ADD_FLAG_ALT(p,c);
                CCS_ADD_FLAG_ALT(u,c);
                CCS_ADD_FLAG_ALT(x,c);
                CCS_ADD_FLAG_ALT(X,c);

                /* c | d | n | o | p | u | x | X -> cdnopuxX */
                CCS_ADD_FLAG(d);
                CCS_ADD_FLAG_ALT(c,d);
                CCS_ADD_FLAG_ALT(n,d);
                CCS_ADD_FLAG_ALT(o,d);
                CCS_ADD_FLAG_ALT(p,d);
                CCS_ADD_FLAG_ALT(u,d);
                CCS_ADD_FLAG_ALT(x,d);
                CCS_ADD_FLAG_ALT(X,d);

                /* e | E -> eE */
                CCS_ADD_FLAG(e);
                CCS_ADD_FLAG_ALT(E,e);
                CCS_ADD_FLAG(E);
                CCS_ADD_FLAG_ALT(e,E);

                /* f | F -> fF */
                CCS_ADD_FLAG(f);
                CCS_ADD_FLAG_ALT(F,f);
                CCS_ADD_FLAG(F);
                CCS_ADD_FLAG_ALT(f,F);

                /* g | G -> gG */
                CCS_ADD_FLAG(g);
                CCS_ADD_FLAG_ALT(G,g);
                CCS_ADD_FLAG(G);
                CCS_ADD_FLAG_ALT(g,G);

                /* c | d | n | o | p | u | x | X -> cdnopuxX */
                CCS_ADD_FLAG(n);
                CCS_ADD_FLAG_ALT(c,n);
                CCS_ADD_FLAG_ALT(d,n);
                CCS_ADD_FLAG_ALT(n,n);
                CCS_ADD_FLAG_ALT(o,n);
                CCS_ADD_FLAG_ALT(p,n);
                CCS_ADD_FLAG_ALT(u,n);
                CCS_ADD_FLAG_ALT(x,n);
                CCS_ADD_FLAG_ALT(X,n);

                /* c | d | n | o | p | u | x | X -> cdnopuxX */
                CCS_ADD_FLAG(o);
                CCS_ADD_FLAG_ALT(c,o);
                CCS_ADD_FLAG_ALT(d,o);
                CCS_ADD_FLAG_ALT(n,o);
                CCS_ADD_FLAG_ALT(o,o);
                CCS_ADD_FLAG_ALT(p,o);
                CCS_ADD_FLAG_ALT(u,o);
                CCS_ADD_FLAG_ALT(x,o);
                CCS_ADD_FLAG_ALT(X,o);

                CCS_ADD_FLAG(p);
                CCS_ADD_FLAG_ALT(c,p);
                CCS_ADD_FLAG_ALT(d,p);
                CCS_ADD_FLAG_ALT(n,p);
                CCS_ADD_FLAG_ALT(o,p);
                CCS_ADD_FLAG_ALT(p,p);
                CCS_ADD_FLAG_ALT(u,p);
                CCS_ADD_FLAG_ALT(x,p);
                CCS_ADD_FLAG_ALT(X,p);

                CCS_ADD_FLAG(s);

                CCS_ADD_FLAG(u);
                CCS_ADD_FLAG_ALT(c,u);
                CCS_ADD_FLAG_ALT(d,u);
                CCS_ADD_FLAG_ALT(n,u);
                CCS_ADD_FLAG_ALT(o,u);
                CCS_ADD_FLAG_ALT(p,u);
                CCS_ADD_FLAG_ALT(u,u);
                CCS_ADD_FLAG_ALT(x,u);
                CCS_ADD_FLAG_ALT(X,u);

                CCS_ADD_FLAG(x);
                CCS_ADD_FLAG_ALT(c,x);
                CCS_ADD_FLAG_ALT(d,x);
                CCS_ADD_FLAG_ALT(n,x);
                CCS_ADD_FLAG_ALT(o,x);
                CCS_ADD_FLAG_ALT(p,x);
                CCS_ADD_FLAG_ALT(u,x);
                CCS_ADD_FLAG_ALT(x,x);
                CCS_ADD_FLAG_ALT(X,x);

                CCS_ADD_FLAG(X);
                CCS_ADD_FLAG_ALT(c,X);
                CCS_ADD_FLAG_ALT(d,X);
                CCS_ADD_FLAG_ALT(n,X);
                CCS_ADD_FLAG_ALT(o,X);
                CCS_ADD_FLAG_ALT(p,X);
                CCS_ADD_FLAG_ALT(u,X);
                CCS_ADD_FLAG_ALT(x,X);
                CCS_ADD_FLAG_ALT(X,X);
                *f++=0;

                if (strlen(extra_flags) > 1)
                  {
                    if (match->encoded_name = NULL)
                      free(match->encoded_name);
                    match->encoded_name = (char*)xmalloc(strlen(match->map_to) +
                    strlen(extra_flags) + 1);
                    sprintf(match->encoded_name,"%s%s", match->map_to, extra_flags);
                  }
                else
                  {
                    /* we have no flags */
                    match->encoded_name = (char*)xmalloc(strlen(match->map_to) + 3);
                    sprintf(match->encoded_name,"%s_0", match->map_to);
                  }
              }
              if (match->encoded_name) return match->encoded_name;
            }
          if (match[1].name &&
              (strcmp(match[1].name,var) == 0)) match++;
          else match = 0;
        }
    }
  return var;
}

static int mchp_bsearch_compare(const void *va, const void *vb)
{
  const mchp_interesting_fn *a = (const mchp_interesting_fn *)va;
  const mchp_interesting_fn *b = (const mchp_interesting_fn *)vb;

  return strcmp(a->name, b->name);
}

static mchp_interesting_fn *mchp_match_conversion_fn(const char *name)
{
  mchp_interesting_fn a,*res;
  a.name = name;

  res = (mchp_interesting_fn*)
        bsearch(&a, mchp_fn_list,
                sizeof(mchp_fn_list)/sizeof(mchp_interesting_fn)-1,
                sizeof(mchp_interesting_fn), mchp_bsearch_compare);
  while (res && (res != mchp_fn_list)  && (strcmp(name, res[-1].name) == 0))
    res--;
  return res;
}

static void conversion_info(mchp_conversion_status state,
                            mchp_interesting_fn *fn_id)
{
  /* dependant upon the conversion status and the setting of the smart-io
     option, set up the mchp_fn_list table. */

  fn_id->conv_flags = (mchp_conversion_status) (CCS_FLAG(fn_id->conv_flags) | state);
  if (mchp_io_size_val== 0)
    {
      if (fn_id->encoded_name) free(fn_id->encoded_name);
      fn_id->encoded_name = 0;
      fn_id->function_convertable = 0;
    }
  else if ((mchp_io_size_val == 1) && (CCS_STATE(state) != conv_possible))
    {
      if (fn_id->encoded_name) free(fn_id->encoded_name);
      fn_id->encoded_name = 0;
      fn_id->function_convertable = 0;
    }
  else if ((mchp_io_size_val == 2) &&
           (CCS_STATE(state) == conv_not_possible))
    {
      if (fn_id->encoded_name) free(fn_id->encoded_name);
      fn_id->encoded_name = 0;
      fn_id->function_convertable = 0;
    }
}

static mchp_conversion_status
mchp_convertable_output_format_string(const char *string)
{
  const char *c = string;
  enum mchp_conversion_status_ status = (mchp_conversion_status_)0;

  for ( ; *c; c++)
    {
      /* quickly deal with the un-interesting cases */
      if (*c != '%') continue;
      if (*(++c) == '%')
        {
          continue;
        }
      /* zero or more flags */
      while (1)
        {
          switch (*c)
            {
            case '-':
            case '+':
            case ' ':
            case '#':
            case '0':
              c++;
              continue;
            default:
              break;
            }
          break;
        }
      /* optional field width or * */
      if (*c == '*') c++;
      else
        while (ISDIGIT(*c)) c++;
      /* optional precision or * */
      if (*c == '.')
        {
          c++;
          /* an illegal conversion sequence %.g, for example - give up and
             start looking from the g onwards */
          if (*c == '*') c++;
          else
            {
              if (!ISDIGIT(*c))
                {
                  c--;
                }
              while (ISDIGIT(*c)) c++;
            }
        }
      /* optional conversion modifier */
      switch (*c)
        {
        case 'h':
        case 'l':
        case 'L':
          c++;
          break;
        default:
          break;
        }
      /* c should point to the conversion character */
      switch (*c)
        {
        case 'a':
          status = (mchp_conversion_status_) (status | conv_a);
          break;
        case 'A':
          status = (mchp_conversion_status_) (status |conv_A);
          break;
        case 'c':
          status = (mchp_conversion_status_) (status |conv_c);
          break;
        case 'd':
          status = (mchp_conversion_status_) (status |conv_d);
          break;
        case 'i':
          status = (mchp_conversion_status_) (status |conv_d);
          break;
        case 'e':
          status = (mchp_conversion_status_) (status |conv_e);
          break;
        case 'E':
          status = (mchp_conversion_status_) (status |conv_E);
          break;
        case 'f':
          status = (mchp_conversion_status_) (status |conv_f);
          break;
        case 'F':
          status = (mchp_conversion_status_) (status |conv_F);
          break;
        case 'g':
          status = (mchp_conversion_status_) (status |conv_g);
          break;
        case 'G':
          status = (mchp_conversion_status_) (status |conv_G);
          break;
        case 'n':
          status = (mchp_conversion_status_) (status |conv_n);
          break;
        case 'o':
          status = (mchp_conversion_status_) (status |conv_o);
          break;
        case 'p':
          status = (mchp_conversion_status_) (status |conv_p);
          break;
        case 's':
          status = (mchp_conversion_status_) (status |conv_s);
          break;
        case 'u':
          status = (mchp_conversion_status_) (status |conv_u);
          break;
        case 'x':
          status = (mchp_conversion_status_) (status |conv_x);
          break;
        case 'X':
          status = (mchp_conversion_status_) (status |conv_X);
          break;
        default:   /* we aren't checking for legal format strings */
          break;
        }
    }
  return (mchp_conversion_status)(conv_possible | status);
}

static mchp_conversion_status
mchp_convertable_input_format_string(const char *string)
{
  const char *c = string;
  enum mchp_conversion_status_ status = (mchp_conversion_status_)0;

  for ( ; *c; c++)
    {
      /* quickly deal with the un-interesting cases */
      if (*c != '%') continue;
      if (*(++c) == '%')
        {
          continue;
        }
      /* optional assignment suppression */
      if (*c == '*') c++;
      /* optional field width */
      while (ISDIGIT(*c)) c++;
      /* optional conversion modifier */
      switch (*c)
        {
        case 'h':
        case 'l':
        case 'L':
          c++;
          break;
        default:
          break;
        }
      /* c should point to the conversion character */
      switch (*c)
        {
        case 'a':
          status = (mchp_conversion_status_)(status | conv_a);
          break;
        case 'A':
          status = (mchp_conversion_status_)(status |conv_A);
          break;
        case 'c':
          status = (mchp_conversion_status_)(status |conv_c);
          break;
        case 'd':
          status = (mchp_conversion_status_)(status |conv_d);
          break;
        case 'i':
          status = (mchp_conversion_status_)(status |conv_d);
          break;
        case 'e':
          status = (mchp_conversion_status_)(status |conv_e);
          break;
        case 'E':
          status = (mchp_conversion_status_)(status |conv_E);
          break;
        case 'f':
          status = (mchp_conversion_status_)(status |conv_f);
          break;
        case 'F':
          status = (mchp_conversion_status_)(status |conv_F);
          break;
        case 'g':
          status = (mchp_conversion_status_)(status |conv_g);
          break;
        case 'G':
          status = (mchp_conversion_status_)(status |conv_G);
          break;
        case 'n':
          status = (mchp_conversion_status_)(status |conv_n);
          break;
        case 'o':
          status = (mchp_conversion_status_)(status |conv_o);
          break;
        case 'p':
          status = (mchp_conversion_status_)(status |conv_p);
          break;
        case 's':
          status = (mchp_conversion_status_)(status |conv_s);
          break;
        case 'u':
          status = (mchp_conversion_status_)(status |conv_u);
          break;
        case 'x':
          status = (mchp_conversion_status_)(status |conv_x);
          break;
        case 'X':
          status = (mchp_conversion_status_)(status |conv_X);
          break;
          /* string selection expr */
        case '[':
        {
          /* [^]...] or []...] or [...] ; get to the end of the conversion */
          c++;
          if (*c == '^') c++;
          if (*c == ']') c++;
          while (*c++ != ']');
        }
        default:   /* we aren't checking for legal format strings */
          break;
        }
    }
  return (mchp_conversion_status)(conv_possible | status);
}

/*
 *   Check or set the conversion status for a particular rtl -
 *     to check the current state pass conv_state_unknown (always 0)
 *     This will create an entry if it doesn't exist or return the current
 *     state.
 */
static mchp_conversion_status
cache_conversion_state(rtx val, int variant, mchp_conversion_status s)
{
  mchp_conversion_cache *parent = 0;
  mchp_conversion_cache *save;

  save = mchp_saved_conversion_info;
  while (save && save->rtl != val)
    {
      parent = save;
      if ((HOST_WIDE_INT)val & sizeof(void *)) save = save->l;
      else save = save->r;
    }
  if (save)
    {

      /* we can only increase the current status */
      if (CCS_STATE(s) > CCS_STATE(save->valid[variant]))
        {
          save->valid[variant] = (mchp_conversion_status) (save->valid[variant] & CCS_FLAG_MASK);
          save->valid[variant] = (mchp_conversion_status) (save->valid[variant] | (s & CCS_STATE_MASK));
        }
      save->valid[variant] = (mchp_conversion_status) (save->valid[variant] | CCS_FLAG(s));
      return save->valid[variant];
    }
  save = (mchp_conversion_cache *) xcalloc(sizeof(mchp_conversion_cache),1);
  save->rtl = val;
  save->valid[variant] = s;
  if (parent)
    {
      if ((HOST_WIDE_INT)val & sizeof(void *)) parent->l = save;
      else parent->r = save;
    }
  else mchp_saved_conversion_info = save;
  return s;
}

/* call-back to make sure all constant strings get seen */
void mchp_cache_conversion_state(rtx val, tree sym)
{
  mchp_conversion_status s;

  s = cache_conversion_state(val, status_output, conv_state_unknown);
  if (s == conv_state_unknown)
    {
      if (sym && (TREE_CODE(sym)==STRING_CST) && STRING_CST_CHECK(sym))
        {
          const char *string = TREE_STRING_POINTER(sym);

          s = mchp_convertable_output_format_string(string);
          cache_conversion_state(val, status_output, s);
        }
    }
  s = cache_conversion_state(val, status_input, conv_state_unknown);
  if (s == conv_state_unknown)
    {
      if (sym && STRING_CST_CHECK(sym))
        {
          const char *string = TREE_STRING_POINTER(sym);

          s = mchp_convertable_input_format_string(string);
          cache_conversion_state(val, status_input, s);
        }
    }
}

/* return the DECL for a constant string denoted by x, if found */
/* this function has disappeared from later sources :( */
static tree constant_string(rtx x)
{
  if (GET_CODE(x) == SYMBOL_REF)
    {
      if (TREE_CONSTANT_POOL_ADDRESS_P (x)) return SYMBOL_REF_DECL (x);
    }
  return 0;
}

/* given an rtx representing a possible string, validate that the string is
   convertable */
static void mchp_handle_conversion(rtx val,
                                   mchp_interesting_fn *matching_fn)
{
  tree sym,var_decl;
  int style;
 
  if (val == 0)
    {
      conversion_info(conv_indeterminate, matching_fn);
      return;
    }
  sym = var_decl = NULL;
  /* a constant string will be given a symbol name, and so will a
     symbol ... */
  var_decl = constant_string(val);
  if(var_decl != NULL)
	sym = DECL_INITIAL (var_decl);

  if (!(sym && (TREE_CODE(sym)==STRING_CST) && STRING_CST_CHECK(sym))) sym = 0;
  mchp_cache_conversion_state(val, sym);
  style = matching_fn->conversion_style == info_I ? status_input:status_output;
  conversion_info(cache_conversion_state(val, style, conv_state_unknown),
                  matching_fn);
}

static void mchp_handle_io_conversion(rtx call_insn,
                                      mchp_interesting_fn *matching_fn)
{
  rtx format_arg;

  gcc_assert((matching_fn->conversion_style == info_I) ||
             (matching_fn->conversion_style == info_O));

  format_arg = PREV_INSN(call_insn);

  if (format_arg == NULL)
    return;

  for (format_arg = PREV_INSN(call_insn);
       !(NOTE_INSN_BASIC_BLOCK_P(format_arg) ||
         NOTE_INSN_FUNCTION_BEG_P(format_arg) ||
         (INSN_P(format_arg) && (GET_CODE(PATTERN(format_arg)) == CALL_INSN)));
       format_arg = PREV_INSN(format_arg))
    {
      if (INSN_P(format_arg))
        {
          if ((GET_CODE(PATTERN(format_arg)) == SET) &&
              (GET_CODE(XEXP(PATTERN(format_arg),0)) == REG))
            {

              int arg_reg = (REGNO(XEXP(PATTERN(format_arg),0)));
              rtx val = XEXP(PATTERN(format_arg),1);

              rtx assignment = format_arg;
              if (arg_reg == matching_fn->interesting_arg)
                {
                  if ((GET_CODE(val) == LO_SUM))
                    {
                      val = XEXP(val,1);
                    }
                  else if ((GET_CODE(val) == REG) || (GET_CODE(val) == SUBREG))
                    {
                      val = find_last_value(val, &assignment, 0, /* allow hw reg */ 1);
                    }
                  else if (GET_CODE(val) == MEM)
                    {
                      val = XEXP(val,0);
                    }
                  mchp_handle_conversion(val, matching_fn);
                  return;
                }
            }
        }
    }
  conversion_info(conv_indeterminate, matching_fn);
}

/*
 *  This function always returns true
 */
int mchp_check_for_conversion(rtx call_insn)
{
  const char *name;
  const char *real_name;
  rtx fn_name;
  mchp_interesting_fn *match;

  if (GET_CODE(call_insn) != CALL_INSN) abort();
  /* (call_insn (set () (call (name) (size)))) for call returning value, and
     (call_insn (call (name) (size)))          for void call */
  if (GET_CODE(PATTERN(call_insn)) == PARALLEL)
    {
      if (GET_CODE(XVECEXP(PATTERN(call_insn),0,0)) == SET)
        fn_name = XEXP(XEXP(XVECEXP(PATTERN(call_insn),0,0),1),0);
      else fn_name = XEXP(XVECEXP(PATTERN(call_insn),0,0),0);
    }
  else
    {
      if (GET_CODE(PATTERN(call_insn)) == SET)
        fn_name = XEXP(XEXP(PATTERN(call_insn),1),0);
      else fn_name = XEXP(PATTERN(call_insn),0);
    }
  if (mchp_clear_fn_list)
    {
      int i;
      for (i = 0; mchp_fn_list[i].name; i++)
        {
          if (mchp_fn_list[i].encoded_name) free(mchp_fn_list[i].encoded_name);
          mchp_fn_list[i].encoded_name = 0;
          mchp_fn_list[i].function_convertable=1;
        }
      mchp_clear_fn_list = 0;
    }
  switch (GET_CODE(fn_name))
    {
    default:
      return 1;

    case MEM:
      if (GET_CODE(XEXP(fn_name,0)) == SYMBOL_REF)
        {
          name = XSTR(XEXP(fn_name,0),0);
        }
      else
        {
          /* not calling a function directly, fn pointer or other such
             - give up */
          return 1;
        }
      real_name = mchp_strip_name_encoding_helper(name);
      match = mchp_match_conversion_fn(real_name);
      break;
    }
  /* function name not interesting or it is already proven to
     be not-convertable */
  while (match)
    {
      switch (match->conversion_style)
        {
        default:
          abort();  /* illegal conversion style */

        case info_I:
          mchp_handle_io_conversion(call_insn, match);
          break;
        case info_O:
          mchp_handle_io_conversion(call_insn, match);
          break;
        case info_dbl:
          break;

        }
      if (match[1].name &&
          (strcmp(match[1].name, real_name) == 0)) match++;
      else match = 0;
    }
  return 1;
}
#endif  /* defined(C32_SMARTIO_RULES) */

static int mchp_build_prefix(tree decl, int fnear, char *prefix)
{
  char *f = prefix;

  tree address_attr = 0;
  tree space_attr = 0;
  tree region_attr = 0;
  bool is_ramfunc = FALSE;
  int const_rodata = 0;

  SECTION_FLAGS_INT flags = 0;
  const char *ident;
  int section_type_set = 0;
  tree paramtype;
  if (fnear == -1)
    {
      section_type_set = 1;
      fnear = 0;
    }

  address_attr = get_mchp_absolute_address (decl);
  space_attr = get_mchp_space_attribute (decl);
  region_attr = get_mchp_region_attribute (decl);
  is_ramfunc = mchp_ramfunc_type_p (decl);

  if (space_attr)
    {
      if (get_identifier("psv") == (TREE_VALUE(TREE_VALUE(space_attr))))
        {
          const_rodata++;
        }
      if (get_identifier("auto_psv") == (TREE_VALUE(TREE_VALUE(space_attr))))
        {
          const_rodata++;
        }
    }

  if (DECL_SECTION_NAME(decl))
    {
      const char *name = TREE_STRING_POINTER(DECL_SECTION_NAME(decl));
      flags = mchp_section_type_flags(0, name, 1);
      if (((flags & SECTION_WRITE) && (!DECL_INITIAL(decl))) ||
          ((flags & SECTION_WRITE) && bss_initializer_p(decl)))
        {
          flags &= ~(SECTION_FLAGS_INT)SECTION_WRITE;
          flags |= (SECTION_FLAGS_INT)SECTION_BSS;
        }
    }
  else
    {
      if ((address_attr) /* || (reverse_attr) */)
        {
          DECL_SECTION_NAME(decl) = build_string(2,"*");
          if (TREE_CODE (decl) == VAR_DECL
              && DECL_SECTION_NAME (decl) != NULL_TREE
              && DECL_INITIAL (decl) == NULL_TREE
              && DECL_COMMON (decl))
            DECL_COMMON (decl) = 0;
        }
      if (TREE_CODE(decl) == STRING_CST)
        flags |= SECTION_READ_ONLY;
    }
  if (TREE_CODE(decl) == VAR_DECL)
    {
      if (!space_attr && TREE_READONLY(decl) && TARGET_EMBEDDED_DATA &&
          (DECL_INITIAL(decl) || (DECL_EXTERNAL(decl))))
        {
          flags |= SECTION_READ_ONLY;
        }
    }
  if (address_attr)
    {
      if (flags & SECTION_ADDRESS)
        {
            if (DECL_NAME(decl) != NULL_TREE)
              {
                ident = IDENTIFIER_POINTER(DECL_NAME(decl));
                warning(0, "address attribute conflicts with section attribute for '%s'",
                        ident);
              }
            else
              {
                warning(0, "address attribute conflicts with section attribute");
              }
        }
      else f += sprintf(f, MCHP_ADDR_FLAG);
    }
  if ((TREE_CODE(decl) == VAR_DECL) &&
      ((flags & SECTION_PERSIST)  || (mchp_persistent_p(decl))))
    {
      f += sprintf(f, MCHP_PRST_FLAG);
      section_type_set = 1;
      DECL_COMMON (decl) = 0;
      if (DECL_INITIAL(decl))
        {
            if (DECL_NAME(decl) != NULL_TREE)
              {
                ident = IDENTIFIER_POINTER(DECL_NAME(decl));
                warning(0, "Persistent variable '%s' will not be initialized",
                        ident);
              }
            else
              {
                warning(0, "Persistent variable will not be initialized");
              }
        }
    }
  if ((flags & SECTION_KEEP) || mchp_keep_p(decl)) {
      DECL_COMMON (decl) = 0;
      f += sprintf(f, MCHP_KEEP_FLAG);
    }
  if ((flags & SECTION_COHERENT) || mchp_coherent_p(decl)) {
      DECL_COMMON (decl) = 0;
      f += sprintf(f, MCHP_COHERENT_FLAG);
      
    }
#if defined(PIC32_SUPPORT_CRYPTO_ATTRIBUTE)
  if (mchp_crypto_p(decl))
    {
      DECL_COMMON (decl) = 0;
      if (!(DECL_INITIAL(decl)))
        {
            if (DECL_NAME(decl) != NULL_TREE)
              {
                ident = IDENTIFIER_POINTER(DECL_NAME(decl));
                warning(0, "Attribute 'crypto' applied to uninitialized '%s'",
                        ident);
              }
        }
    }
#endif
  if (1)
    {
      fnear |= (flags & SECTION_NEAR);
      if ((flags & SECTION_CODE) ||
          (space_attr && (get_identifier("prog") == (TREE_VALUE(TREE_VALUE(space_attr))))))
        {
          if (TREE_CODE(decl) == VAR_DECL)
            f += sprintf(f, MCHP_CONST_FLAG);
          else
            f += sprintf(f, MCHP_PROG_FLAG);
          fnear = 0;
          DECL_COMMON (decl) = 0;
          section_type_set = 1;
        }
      if ((flags & SECTION_WRITE) ||
          is_ramfunc ||
          (space_attr && (get_identifier("data") ==
                          (TREE_VALUE(TREE_VALUE(space_attr))))))
        {
          if (TREE_CODE(decl) == FUNCTION_DECL)
          {
            if (!region_attr)
            {
            f += sprintf(f, MCHP_RAMFUNC_FLAG);
            }
            else 
            {
              f += sprintf(f, MCHP_REGION_FLAG);	
              f += sprintf(f, MCHP_DATA_FLAG);
            }
          }  
          else
          {
            f += sprintf(f, MCHP_DATA_FLAG);
          }
          section_type_set = 1;
        }
      /* we can't ask for a BSS section apart from by using the old naming
       convention or flags in a section directive - but if it is there, we
       should honour it */
      if (flags & SECTION_BSS)
        {
          f += sprintf(f, MCHP_BSS_FLAG);
          section_type_set = 1;
        }
      if ((flags & SECTION_READ_ONLY) || const_rodata)
        {
          f += sprintf(f, MCHP_CONST_FLAG);
          fnear = 0;
          section_type_set = 1;
        }

      if ((!section_type_set) && (!DECL_EXTERNAL(decl)))
        {
          if (!DECL_INITIAL(decl) || bss_initializer_p(decl))
            f += sprintf(f, MCHP_BSS_FLAG);
          else
            f += sprintf(f, MCHP_DATA_FLAG);
        }
      if ((fnear) && !(flags & (SECTION_INFO)))
        {
          f += sprintf(f, MCHP_NEAR_FLAG);
        }
    }
  if (mchp_keep_p(decl)) {
    f += sprintf(f, MCHP_KEEP_FLAG);
  }
  if (mchp_coherent_p(decl)) {
    f += sprintf(f, MCHP_COHERENT_FLAG);
  }
  return fnear;
}

static SECTION_FLAGS_INT validate_identifier_flags(const char *id);

void mchp_subtarget_encode_section_info (tree decl, rtx rtl,
    int first_seen ATTRIBUTE_UNUSED)
{
  char prefix[120] = { 0 };
  int fNear;
  const char *fn_name;
  char *f = prefix;

  switch (TREE_CODE(decl))
    {
    case FUNCTION_DECL:
      fNear = 0;
      fn_name = IDENTIFIER_POINTER(DECL_NAME(decl));
      f += sprintf(f,MCHP_FCNN_FLAG);
      mchp_build_prefix (decl, -1, f);
      break;
    case VAR_DECL:
      fNear = 0;
      if (TREE_READONLY(decl))
        {
          /*
          ** If this is a constant declaration,
          ** and constants are located in code space,
          ** then it cannot be a near declaration.
          */
          fNear = 0;
        }
#if 0 /* Don't use 'near' for small data yet */
      else
        fNear = mips_in_small_data_p(decl);
#endif
      SYMBOL_REF_FLAG(XEXP(rtl, 0)) = mchp_build_prefix(decl, fNear, prefix);
      break;

    default:
      break;
    }
  if ((prefix[0] != 0) == 0) fNear = 0;
  {
    const char *str = XSTR(XEXP (rtl, 0), 0);
    int len = strlen(str);
    const char *stripped_str = mchp_strip_name_encoding_helper(str);
    char *newstr = (char*)xmalloc(len + strlen(prefix) + 1);
    sprintf(newstr, "%s%s", prefix, stripped_str);
    XSTR(XEXP(rtl, 0), 0) = newstr;
    /* previously allocated ? */
    if (stripped_str != str) free((void*)str);
  }
}

/* Return a nonzero value if DECL has a section attribute.  */
#ifndef IN_NAMED_SECTION
#define IN_NAMED_SECTION(DECL) \
  ((TREE_CODE (DECL) == FUNCTION_DECL || TREE_CODE (DECL) == VAR_DECL) \
   && DECL_SECTION_NAME (DECL) != NULL)
#endif
/*
** A C statement or statements to switch to the appropriate
** section for output of DECL.  DECL is either a `VAR_DECL' node
** or a constant of some sort.  RELOC indicates whether forming
** the initial value of DECL requires link-time relocations.
*/
section *
mchp_select_section (tree decl, int reloc,
                     unsigned HOST_WIDE_INT align ATTRIBUTE_UNUSED)
{
  extern section *
  mergeable_string_section (tree decl ATTRIBUTE_UNUSED,
                            unsigned HOST_WIDE_INT align ATTRIBUTE_UNUSED,
                            unsigned int flags ATTRIBUTE_UNUSED);
  const char *sname;
  const char *ident = 0; /* compiler can't tell that this is intialized */
  rtx rtl;
  SECTION_FLAGS_INT flags = 0;

  if ((TREE_CODE(decl) == FUNCTION_DECL) || (TREE_CODE(decl) == VAR_DECL))
    {
      if (IN_NAMED_SECTION (decl) ||
          get_mchp_absolute_address(decl) ||
          lookup_attribute("space", DECL_ATTRIBUTES(decl)) ||
          lookup_attribute("persistent", DECL_ATTRIBUTES(decl)) ||
          lookup_attribute("region", DECL_ATTRIBUTES(decl)) ||
          mchp_ramfunc_type_p(decl)
#if defined(PIC32_SUPPORT_CRYPTO_ATTRIBUTE)
           || mchp_crypto_p(decl)
#endif
         )
        {
          rtl = DECL_RTL(decl);
          ident = XSTR(XEXP(rtl, 0), 0);
          flags = validate_identifier_flags(ident);
          sname = default_section_name(decl, flags);
          if (flags)
            {
              if (sname)
                {
                  if (!DECL_P (decl))
                    decl = NULL_TREE;
                  return get_named_section (decl, sname, reloc);
                }
            }
        }
    }

  switch (categorize_decl_for_section (decl, reloc))
    {
    case SECCAT_TEXT:
      return text_section;
    case SECCAT_RODATA:
      return readonly_data_section;
    case SECCAT_RODATA_MERGE_STR:
      return mergeable_string_section (decl, align, 0);
    case SECCAT_RODATA_MERGE_STR_INIT:
      return mergeable_string_section (DECL_INITIAL (decl), align, 0);
    case SECCAT_RODATA_MERGE_CONST:
      return mergeable_constant_section (DECL_MODE (decl), align, 0);
    case SECCAT_SRODATA:
      sname = ".sdata2";
      break;
    case SECCAT_DATA:
      return data_section;
    case SECCAT_DATA_REL:
      sname = ".data.rel";
      break;
    case SECCAT_DATA_REL_LOCAL:
      sname = ".data.rel.local";
      break;
    case SECCAT_DATA_REL_RO:
      sname = ".data.rel.ro";
      break;
    case SECCAT_DATA_REL_RO_LOCAL:
      sname = ".data.rel.ro.local";
      break;
    case SECCAT_SDATA:
      sname = ".sdata";
      break;
    case SECCAT_TDATA:
      sname = ".tdata";
      break;
    case SECCAT_BSS:
      if (bss_section)
        return bss_section;
      sname = ".bss";
      break;
    case SECCAT_SBSS:
      sname = ".sbss";
      break;
    case SECCAT_TBSS:
      sname = ".tbss";
      break;
    default:
      gcc_unreachable ();
    }

  if (!DECL_P (decl))
    decl = NULL_TREE;
  return get_named_section (decl, sname, reloc);
}

static const char *default_section_name(tree decl, SECTION_FLAGS_INT flags)
{
  static char result[1024];
  char *f;
  int i;
  tree a,is_aligned;
  tree p;
  tree region, memory=0;
  bool is_rf;

  const char *pszSectionName=0;
  int is_default = 0;
  int len_this_default_name;

  if (current_time == 0) current_time = time(0);
  /*
   * if we have a decl, use its address as the unique section name
   */
  if (decl)
    {
      len_this_default_name = sprintf(this_default_name,"*_%8.8lx%lx",
                                      (unsigned long) decl, current_time);
    }
  else
    {
      len_this_default_name = 1;
      this_default_name[0]='*';
      this_default_name[1]=0;
    }
  f = result;
  *f = 0;
  if (decl)
    {
      is_aligned = lookup_attribute("aligned",
                                    DECL_ATTRIBUTES(decl));
      a = get_mchp_absolute_address (decl);
      p = get_mchp_space_attribute (decl);
      region = get_mchp_region_attribute (decl);
      is_rf = mchp_ramfunc_type_p (decl);

      if (DECL_SECTION_NAME (decl))
        {
          pszSectionName = TREE_STRING_POINTER(DECL_SECTION_NAME(decl));
        }
#if defined(PIC32_SUPPORT_CRYPTO_ATTRIBUTE)
      if (mchp_crypto_p(decl))
        {
           if (a)
                f += sprintf(result, ".$XC32$_crypto.%s,%s(0x%lx)",
                             IDENTIFIER_POINTER(DECL_NAME(decl)),
                             SECTION_ATTR_ADDRESS,
                             (long unsigned int)TREE_INT_CST_LOW(TREE_VALUE(TREE_VALUE(a))));
            else
                f += sprintf(result, ".$XC32$_crypto.%s",
                             IDENTIFIER_POINTER(DECL_NAME(decl)));
        } else
#endif
      if (a)
        {
          if (!pszSectionName||(strcmp(pszSectionName,mchp_default_section) == 0))
            f += sprintf(result, "%s,%s(0x%lx)",
                         this_default_name,
                         SECTION_ATTR_ADDRESS,
                         (long unsigned int)TREE_INT_CST_LOW(TREE_VALUE(TREE_VALUE(a))));
          else
            {
              if (((TREE_CODE(decl) == VAR_DECL) && flag_data_sections) ||
                  ((TREE_CODE(decl) == FUNCTION_DECL) && flag_function_sections))
                f += sprintf(result, "%s.%s,%s(0x%lx)",
                             pszSectionName,
                             IDENTIFIER_POINTER(DECL_NAME(decl)),
                             SECTION_ATTR_ADDRESS,
                             (long unsigned int)TREE_INT_CST_LOW(TREE_VALUE(TREE_VALUE(a))));
              else
                f += sprintf(result, "%s,%s(0x%lx)",
                             pszSectionName,
                             SECTION_ATTR_ADDRESS,
                             (long unsigned int)TREE_INT_CST_LOW(TREE_VALUE(TREE_VALUE(a))));
            }
        }
      else if (TREE_CODE(decl) == VAR_DECL)
        {
          if ((!mchp_coherent_p(decl)) && mchp_persistent_p(decl)) /* persist*/
            {
              pszSectionName = SECTION_NAME_PBSS;
            }
          else if (mips_in_small_data_p(decl))
            {
              if (pszSectionName == NULL)
                {
                  if (flags)
                    {
                      if (flags & SECTION_BSS)
                        {
                              pszSectionName = SECTION_NAME_SBSS;
                        }
                      else if (flags & SECTION_WRITE)
                        {
                              pszSectionName = SECTION_NAME_SDATA;
                        }
                    }
                  else
                    pszSectionName = this_default_name;
                }
            }
          if (pszSectionName)
            {
              if (flag_data_sections)
                {
                  f += sprintf (result, "%s.%s", pszSectionName,
                                IDENTIFIER_POINTER(DECL_NAME(decl)));
                }
              else
                {
                  f += sprintf(result, "%s", pszSectionName);
                }
            }
          else
            {
              if (!is_aligned) is_default = 1;
              f+= sprintf(result,"%s", this_default_name);
            }
        }
      else if (TREE_CODE(decl) == FUNCTION_DECL)
        {
          if (pszSectionName)
            {
#if 1
              if (flag_function_sections && is_rf)
                {
                  f += sprintf(result, "%s.%s",
                               pszSectionName, IDENTIFIER_POINTER(DECL_NAME(decl)));
                }
              else
                {
                  f += sprintf(result, "%s",
                               pszSectionName);
                }
#else
              f += sprintf(result, "%s",
                           pszSectionName);
#endif
            }
          else
            {
              if (!is_aligned) is_default = 1;
              f+= sprintf(result,"%s", this_default_name);
            }
        }
      else
        {
          if (!is_aligned) is_default = 1;
          f+= sprintf(result,"%s", this_default_name);
        }

         if (region)
         {
            if (TREE_CODE(TREE_VALUE(TREE_VALUE(region))) == STRING_CST) 
            {
                 memory = TREE_VALUE(TREE_VALUE(region));
            }
         }

         if (memory) 
         {
           pic32_external_memory *m;
           is_default = 0;

           for (m = pic32_external_memory_head; m; m = m->next) {
              if (strcmp (m->name, TREE_STRING_POINTER (memory)) == 0) break;
           }
      
           if (m == 0) error("invalid external memory attribute");
      
           f+= sprintf(f,",memory(_%s)", m->name);
#if 0      
           if (!(flags & SECTION_NOLOAD)) 
           {
              f+= sprintf(f, "," SECTION_ATTR_NOLOAD);
           }
#endif
         }

      if ((!is_default) ||
          (strncmp(result,this_default_name,len_this_default_name)))
        return result;
    }
  if (flags)
    {
      i = 0;
      if ((flags & SECTION_READ_ONLY) && (!TARGET_EMBEDDED_DATA))
        {
          if ((TREE_CODE(decl) == STRING_CST) ||
              (DECL_INITIAL(decl) && TREE_CONSTANT(DECL_INITIAL(decl))))
            {
              const char *name;
#if defined(PIC32_SUPPORT_CRYPTO_ATTRIBUTE)
              if (mchp_crypto_p(decl)) /* crypto */
              {
                name = ".$XC32$_crypto";
              }
              else
                name = SECTION_NAME_CONST;
#else
              name = SECTION_NAME_CONST;
#endif
              while (reserved_section_names[i].section_name)
                {
                  if (((flags ^ reserved_section_names[i].mask) == 0) &&
                      strstr(name, reserved_section_names[i].section_name) == 0)
                    return name;
                  i++;
                }
            }
        }
      else if (flags & SECTION_RAMFUNC)
        {
          if (TREE_CODE(decl) == FUNCTION_DECL)
            {
              const char *name;
              name = SECTION_NAME_RAMFUNC;
              while (reserved_section_names[i].section_name)
                {
                  if (((flags ^ reserved_section_names[i].mask) == 0) &&
                      strstr(name, reserved_section_names[i].section_name) == 0)
                    return name;
                  i++;
                }
            }
        }
      i = 0;
      while (reserved_section_names[i].section_name)
        {
          if ((flags ^ reserved_section_names[i].mask) == 0)
            {
              if (strcmp(reserved_section_names[i].section_name, ".text") == 0)
                {
                  const char *txt = mchp_text_scn;

                  if (!txt) txt = reserved_section_names[i].section_name;
                  if (!flag_function_sections || !decl)  return txt;
#if 1
                  else
                    {
                      char *retval;
                      retval = (char*)xmalloc(strlen(txt)+3+
                                              strlen(IDENTIFIER_POINTER(DECL_NAME(decl))));
                      /* if we are prepending .isr use that instead of the default name
                         of .text until keep attribute is implemented */
                      f +=sprintf(retval, "%s.%s", txt,
                                  IDENTIFIER_POINTER(DECL_NAME(decl)));
                      return retval;
                    }
#endif
                }
              else return reserved_section_names[i].section_name;
            }
          i++;
        }
    }
  return this_default_name;
}


/************************************************************************/
/* TARGET_ASM_NAMED_SECTION target hook.                */
/* Output assembly directives to switch to section pszSectionName.    */
/* The section name will have any user-specifed flags appended.        */
/* The section should have attributes as specified by flags, which is a    */
/* bit mask of the SECTION_* flags defined in output.h.         */
/************************************************************************/
void mchp_asm_named_section(const char *pszSectionName,
                            SECTION_FLAGS_INT flags,
                            tree decl ATTRIBUTE_UNUSED)
{
  if (set_section_stack(pszSectionName, flags) == 0) return;
  mchp_merged_asm_named_section(pszSectionName, flags);
}

static 
char * mchp_get_named_section_flags (const char *pszSectionName,
    SECTION_FLAGS_INT flags)
{
  char pszSectionFlag[100] = " # Invalid Section Attributes";
  char *f;

  if (pszSectionName == 0) return NULL;
#if 1
  if (strcmp("*", pszSectionName) == 0) return NULL;
#endif
  f = pszSectionFlag;
  lfInExecutableSection = FALSE;
  if (flags & SECTION_BSS)
    {
      f += sprintf(f, "," SECTION_ATTR_BSS);
    }
  if (flags & SECTION_WRITE)
    {
      f += sprintf(f, "," SECTION_ATTR_DATA);
    }
  if (flags & SECTION_CODE)
    {
      f += sprintf(f, "," SECTION_ATTR_CODE);
      lfInExecutableSection = TRUE;
    }
  if (flags & SECTION_READ_ONLY)
    {
      f += sprintf(f, "," SECTION_ATTR_CONST);
    }
  if (flags & SECTION_RAMFUNC)
    {
      f += sprintf(f, "," SECTION_ATTR_RAMFUNC);
    }
  if (flags & SECTION_NEAR)
    {
      f += sprintf(f, "," SECTION_ATTR_NEAR);
    }
  if (flags & SECTION_PERSIST)
    {
      f += sprintf(f, "," SECTION_ATTR_PERSIST);
    }
  if (flags & SECTION_NOLOAD)
    {
      f += sprintf(f, "," SECTION_ATTR_NOLOAD);
    }
  if (flags & SECTION_DEBUG)
    {
      f += sprintf(f, "," SECTION_ATTR_INFO);
    }
  if (flags & SECTION_KEEP)
    {
      f += sprintf(f, "," SECTION_ATTR_KEEP);
    }
  if (flags & SECTION_COHERENT)
    {
      f += sprintf(f, "," SECTION_ATTR_COHERENT);
    }
  return xstrdup(pszSectionFlag);
}

static void mchp_merged_asm_named_section(const char *pszSectionName,
    SECTION_FLAGS_INT flags)
{
  char* pszSectionFlag;
  pszSectionFlag = mchp_get_named_section_flags (pszSectionName, flags);
#if 1
  fprintf(asm_out_file, "\t.section\t%s%s\n", pszSectionName, pszSectionFlag);
#endif
}


static int mchp_bsearch_rsn_compare(const void *va, const void *vb)
{
  const char *a = (const char *)va;
  const struct reserved_section_names_ *n = (const struct reserved_section_names_ *)vb;

  if (n) return strcmp(a, n->section_name);
  else return 0;
}

static int mchp_bsearch_vsf_compare(const void *va, const void *vb)
{
  const char *a = (const char *)va;
  const struct valid_section_flags_ *f = (const struct valid_section_flags_ *)vb;

  if (f) return strncmp(a, f->flag_name, strlen(f->flag_name));
  else return 0;
}

/* validates a section declaration based on its name and any flags */
static 
SECTION_FLAGS_INT validate_section_flags(const char *name,
    SECTION_FLAGS_INT attr_flags)
{
  SECTION_FLAGS_INT set_flags = attr_flags;
  struct reserved_section_names_ *r_section = 0;
  struct valid_section_flags_ *v_flags = 0;
  char *f,*fe,comma=0;
  char *flags = NULL;
  int first_flag = 1;

  f = 0;
  flags = (char*)strchr(name, ',');
  if (flags)
    {
      *flags = 0;
      f = flags+1;
      comma = ',';
    }
  if (name) r_section = (struct reserved_section_names_*)
                          bsearch(name, reserved_section_names,
                                  (sizeof(reserved_section_names) /
                                   sizeof(struct reserved_section_names_)) - 1,
                                  sizeof(struct reserved_section_names_),
                                  mchp_bsearch_rsn_compare);
  if (r_section)
    {
      set_flags |= r_section->mask;
    }
  if (f) do
      {
        fe = strchr(f, ',');
        if (fe)
          {
            *fe = 0;
          }
        /* nasty safe-ctype.h means that we can't use isspace */
        while (*f && ISSPACE(*f)) f++;
        if (*f)
          {
            v_flags = (struct valid_section_flags_ *)
                      bsearch(f, valid_section_flags,
                              (sizeof(valid_section_flags) /
                               sizeof(struct valid_section_flags_)) -1,
                              sizeof(struct valid_section_flags_),
                              mchp_bsearch_vsf_compare);
            if (!v_flags)
              {
                if (first_flag)
                  {
                    char *s;

                    for (s = f; *s; s++)
                      {
                        for (v_flags = valid_section_flags; v_flags->flag_name; v_flags++)
                          {
                            if (*s == v_flags->single_letter_equiv)
                              {
                                if (v_flags->single_letter_equiv == 'b')
                                  {
                                    /* this may be .pbss,b for persistent */
                                    if ((strncmp(name, SECTION_NAME_PBSS, 5) == 0) &&
                                        ((v_flags->flag_mask & SECTION_PERSIST) == 0)) continue;
                                  }
                                else if (v_flags->single_letter_equiv == 'r')
                                  {
                                    /* 'r' used to be used for .eedata - don't set READ_ONLY for
                                           .eedata section */
                                    if (strncmp(name, ".eedata", 7) == 0) break;
                                  }
                                set_flags |= v_flags->flag_mask;
                                break;
                              }
                          }
                        if (!v_flags->flag_name)
                          {
                            warning(0, "'%c': unrecognized old-style section flag", *s);
                            break;
                          }
                        *s = ' ';
                        comma=' ';
                      }
                    first_flag = 0;
                  }
                else warning(0, "'%s': unrecognized section flag", f);
              }
            else
              {
                set_flags |= v_flags->flag_mask;
              }
            if (fe)
              {
                *fe = ',';
                f = fe+1;
              }
            else break;
          }
        else break;
      }
    while (1);
  if (flags) *flags = comma;
  for (v_flags = valid_section_flags; v_flags->flag_name; v_flags++)
    {
      if ((set_flags & v_flags->flag_mask) &&
          (set_flags & v_flags->incompatible_with))
        {
          error("incompatible section flags for section '%s'", name);
          return set_flags;
        }
    }
  return set_flags;
}

/*
** unsigned int
** TARGET_SECTION_TYPE_FLAGS(tree decl, const char *name, int reloc);
**
** A target hook to choose a set of section attributes for use by
** TARGET_ASM_NAMED_SECTION based on a variable or function decl,
** a section name, and whether or not the declaration's initializer
** may contain runtime relocations.
**
** <decl> is either a FUNCTION_DECL, a VAR_DECL or NULL_TREE.
** If <decl> is null, read-write data should be assumed.
** <reloc> indicates whether the initial value of exp requires
** link-time relocations.
*/
SECTION_FLAGS_INT
mchp_section_type_flags(tree decl, const char *name,
                        int reloc ATTRIBUTE_UNUSED)
{
  SECTION_FLAGS_INT flag = 0;
  rtx rtl;

  if (decl)
    {
      rtl = DECL_RTL(decl);
      if (rtl && XSTR(XEXP(rtl, 0), 0))
        flag = validate_identifier_flags(XSTR(XEXP(rtl, 0), 0));
    }
  if (name) flag = validate_section_flags(name,flag);
  return flag ;
}

/************************************************************************/
/* Save the current section name.                    */
/************************************************************************/
int set_section_stack(const char *pszSectionName,
                      SECTION_FLAGS_INT pszSectionFlag)
{
  if (!lSectionStack)
    {
      if (freeSectionStack)
        {
          lSectionStack = freeSectionStack;
          freeSectionStack = freeSectionStack->pop;
        }
      else lSectionStack =  (sectionStack *)xcalloc(sizeof(sectionStack),1);
      lSectionStack->pop = 0;
    }
  else if ((lSectionStack->pszFlag == pszSectionFlag) &&
           (pszSectionName[0] != '*') &&
           (strcmp(lSectionStack->pszName, pszSectionName) == 0))
    {
      if (lSectionStack->flags & ss_should_pop) lSectionStack->flags = ss_set;
      return 0;
    }
  else if (lSectionStack->flags & ss_should_pop)
    {
      sectionStack *s;

      s = lSectionStack;
      lSectionStack = s->pop;
      s->pop = freeSectionStack;
      freeSectionStack = s;
    }
  lSectionStack->pszName = xstrdup(pszSectionName);
  lSectionStack->pszFlag = pszSectionFlag;
  lSectionStack->flags |= ss_set;
  return 1;
}

#if 1
void mchp_push_section_name(const char *pszSectionName,
                            SECTION_FLAGS_INT pszSectionFlag)
{
  sectionStack *s;

  if (lSectionStack->flags & ss_should_pop)
    {
      if ((lSectionStack->pszFlag == pszSectionFlag) &&
          (pszSectionName[0] != '*') &&
          (strcmp(lSectionStack->pszName, pszSectionName) == 0))
        {
          /* pushing the section just popped */
          lSectionStack->flags &= ~ss_should_pop;
          return;
        }
      else s = lSectionStack;
    }
  else if (freeSectionStack)
    {
      s = freeSectionStack;
      if (s->pszName) free((void *)s->pszName);  /* avoid leak */
      s->pszName = 0;
      freeSectionStack = s->pop;
    }
  else
    {
      s = (sectionStack *) xcalloc(sizeof(sectionStack),1);
    }
  if (s != lSectionStack) s->pop = lSectionStack;
  s->pszName = xstrdup(pszSectionName);
  s->pszFlag = pszSectionFlag;
  s->flags = 0;
  lSectionStack = s;
  mchp_merged_asm_named_section(pszSectionName, pszSectionFlag);
}
#endif

#if 1
static tree mchp_push_pop_constant_section(tree decl, enum css push,
    const char **section_name)
{
  struct decl_stack
  {
    tree decl;
    struct decl_stack *next;
    int activated;
  };
  static struct decl_stack *my_decl_stack = 0;
  int activated = 0;

  if (push == css_push)
    {
      struct decl_stack *p;

      p = (struct decl_stack *)xcalloc(sizeof(struct decl_stack),1);
      p->next = my_decl_stack;
      p->decl = decl;
      my_decl_stack = p;
    }
  else if (push == css_activate /*activate */)
    {
      /* this can happen iff we are creating code to initialize */
      decl = my_decl_stack ? my_decl_stack->decl : 0;
    }
  else if (push == css_pop /*pop */)
    {
      if (my_decl_stack == 0) abort();
      if (decl != my_decl_stack->decl) abort();
      activated = my_decl_stack->activated;
      my_decl_stack = my_decl_stack->next;
    }
  else if (push == css_tos)
    {
      decl = my_decl_stack ? my_decl_stack->decl : 0;
    }
  if (push == css_push) return decl;
  if (!decl || !DECL_SECTION_NAME(decl))
    {
      const char *force_named_section = 0;
      SECTION_FLAGS_INT flags;
      /*
       * perhaps force a named section based upon boot or secure attributes
       */
      flags = SECTION_READ_ONLY;

      if (force_named_section)
        {
          if (push == css_activate)
            {
              mchp_push_section_name(force_named_section, flags);
              if (my_decl_stack) my_decl_stack->activated = 1;
            }
          else if ((push == css_pop) && (activated)) mchp_pop_section_name();
          else if ((push == css_tos) && (*section_name))
            *section_name = force_named_section;
          return decl;
        }
    }
  if (decl && DECL_SECTION_NAME(decl))
    {
      SECTION_FLAGS_INT flags;
      const char *name = TREE_STRING_POINTER(DECL_SECTION_NAME(decl));

      flags = mchp_section_type_flags(decl, name, 1);
      if (push == css_activate)
        {
          mchp_push_section_name(name, flags);
          my_decl_stack->activated = 1;
        }
      else if ((push == css_pop) && (activated))  mchp_pop_section_name();
      else if ((push == css_tos) && (section_name)) *section_name = name;
      return decl;
    }

  if (push == css_activate)
    {
      if (1)
        switch_to_section(const_section);
    }
  else if ((push == css_tos) && *section_name)
    {
      if (1)
        *section_name = SECTION_NAME_CONST;
    }
  return decl;
}

const char *mchp_pushed_constant_section(void)
{
  const char *name = 0;

  if (mchp_push_pop_constant_section(0, css_tos, &name) == 0)
    {
      name= lSectionStack->pszFlag & SECTION_CONST ? lSectionStack->pszName : name;
    }
  return name;
}

void mchp_push_constant_section(tree decl)
{
  (void) mchp_push_pop_constant_section(decl,css_push,0);
}

void mchp_pop_constant_section(tree decl)
{
  (void) mchp_push_pop_constant_section(decl,css_pop,0);
}


#endif
#if 1
/************************************************************************/
/* Restore a saved section name.                    */
/************************************************************************/
static void mchp_pop_section_name(void)
{
  if (lSectionStack->flags & ss_set)
    {
      /* popping back to a sectionStack item that was set using
         set_section_stack() as a result of a named section ... convert it
         back to the default section */
      lSectionStack->pszName = default_section.pszName;
      lSectionStack->pszFlag = default_section.pszFlag;
      lSectionStack->flags = default_section.flags;
      mchp_merged_asm_named_section(lSectionStack->pszName,
                                    lSectionStack->pszFlag);
    }
  else lSectionStack->flags |= ss_should_pop;
}
#endif

/************************************************************************/
/*
** A C statement (sans semicolon) to output to the stdio stream FILE the
** assembler definition of a local-common-label named NAME whose size is SIZE
** bytes. The variable ROUNDED is the size rounded up to whatever alignment the
** caller wants. Use the expression assemble_name (FILE, NAME) to output the
** name itself; before and after that, output the additional assembler syntax
** for defining the name, and a newline. This macro controls how the assembler
** definitions of uninitialized static variables are output.
*/
/************************************************************************/
void mchp_asm_output_local(FILE *file, char *name, int size ATTRIBUTE_UNUSED,
                           int rounded)
{
  const char *pszSectionName;

  SECTION_FLAGS_INT flags = validate_identifier_flags(name);

  /* if this is a data section, this is now a BSS section */
  if (flags & SECTION_WRITE)
    {
      flags &= ~SECTION_WRITE;
      flags |= SECTION_BSS;
    }
  pszSectionName = default_section_name(0, flags);
  mchp_push_section_name(pszSectionName, flags);

  assemble_name(file, name);
  fputs(":\t.space\t", file);
  fprintf(file, "%u\n", rounded);
  mchp_pop_section_name();
}

/************************************************************************/
/*
** Like ASM_OUTPUT_LOCAL except takes the required alignment as a separate,
** explicit argument. If you define this macro, it is used in place of
** ASM_OUTPUT_LOCAL, and gives you more flexibility in handling the required
** alignment of the variable. The alignment is specified as the number of bits.
**
** NB: this function is used in preference to mchp_asm_output_local
*/
/************************************************************************/
void mchp_asm_output_aligned_decl_local(FILE *file, tree decl, char *name,
                                        int size, int alignment)
{
  const char *pszSectionName;
  SECTION_FLAGS_INT flags = validate_identifier_flags(name);

  /* if this is a data sectino, this is now a BSS section */
  if (flags & SECTION_WRITE)
    {
      flags &= ~SECTION_WRITE;
      flags |= SECTION_BSS;
    }
  pszSectionName = default_section_name(decl, flags);
  mchp_push_section_name(pszSectionName, flags);

  if ((alignment > BITS_PER_UNIT))
    {
      fprintf(file, "\t.align\t%d\n", alignment / BITS_PER_UNIT);
    }
  assemble_name(file, name);
  fputs(":\t.space\t", file);
  fprintf(file, "%u\n", size);
  mchp_pop_section_name();
}

/************************************************************************/
/*
** mchp_text_section_asm_op()
** A C expression whose value is a string, including spacing, containing
** the assembler operation that should precede instructions and read-only
** data. Normally "\t.text" is right.
*/
/************************************************************************/
const char * mchp_text_section_asm_op(void)
{
  static char *pszSection;
  static unsigned int pszSection_len;
  const char *section_name;
  int ramfunc = 0;
  tree space;

  /* TODO */
  if (current_function_decl)
    {
      space = get_mchp_space_attribute(current_function_decl);
      ramfunc = ((get_identifier("data") == (TREE_VALUE(TREE_VALUE(space)))) ||
                 mchp_ramfunc_type_p(current_function_decl)) ;
      section_name = default_section_name(current_function_decl,
                                          ramfunc ? SECTION_RAMFUNC :
                                          SECTION_CODE);
      DECL_SECTION_NAME(current_function_decl) = build_string (strlen (section_name), section_name);
    }
  else
    {
      section_name = mchp_text_scn ? mchp_text_scn : ".text";
    }
  set_section_stack(section_name, ramfunc ? SECTION_RAMFUNC : SECTION_CODE);
  lfInExecutableSection = TRUE;
  if (pszSection == NULL)
    {
      pszSection_len = strlen(section_name);
      pszSection = (char *) xmalloc(pszSection_len+100);
    }
  else if (strlen(section_name) > pszSection_len)
    {
      pszSection_len = strlen(section_name);
      free(pszSection);
      pszSection = (char *)xmalloc(pszSection_len+100);
    }
  sprintf(pszSection, "\t.section\t%s,%s", lSectionStack->pszName,
          ramfunc ? SECTION_ATTR_RAMFUNC :
          SECTION_ATTR_CODE);
  return  (const char *)pszSection;
}

/************************************************************************/
/*
** mchp_data_section_asm_op()
** A C expression whose value is a string, including spacing, containing
** the assembler operation to identify the following data as writable
** initialized data. Normally "\t.data" is right.
*/
/************************************************************************/
const char * mchp_data_section_asm_op(void)
{
  set_section_stack(SECTION_NAME_DATA,SECTION_WRITE);
  lfInExecutableSection = FALSE;

  return((char *)"\t.section\t.data, data");
}

const char * mchp_bss_section_asm_op(void)
{
  set_section_stack(SECTION_NAME_BSS,SECTION_WRITE);
  lfInExecutableSection = FALSE;

  return((char *)"\t.section\t.bss,bss");
}

const char * mchp_sbss_section_asm_op(void)
{
  set_section_stack(SECTION_NAME_SBSS,SECTION_WRITE);
  lfInExecutableSection = FALSE;

  return((char *)"\t.section\t.sbss,bss,near");
}

const char * mchp_sdata_section_asm_op(void)
{
  set_section_stack(SECTION_NAME_SDATA,SECTION_WRITE);
  lfInExecutableSection = FALSE;

  return((char *)"\t.section\t.sdata,data,near");
}

/* validate prefix before an identifier */
static SECTION_FLAGS_INT validate_identifier_flags(const char *id)
{
  const char *f = id;
  SECTION_FLAGS_INT flags = 0;
  int add_section_code_flag = 0;
  struct valid_section_flags_ *v_flags = 0;
  
  gcc_assert (sizeof(SECTION_FLAGS_INT) >= sizeof(unsigned long long));

  while (f && *f == MCHP_EXTENDED_FLAG[0])
    {
      if (strncmp(f, MCHP_PROG_FLAG, sizeof(MCHP_PROG_FLAG)-1) == 0)
        {
          flags |= SECTION_CODE;
          f += sizeof(MCHP_PROG_FLAG)-1;
        }
      else if (strncmp(f, MCHP_RAMFUNC_FLAG, sizeof(MCHP_RAMFUNC_FLAG)-1) == 0)
        {
          flags |= SECTION_RAMFUNC;
          f += sizeof(MCHP_RAMFUNC_FLAG)-1;
        }
      else if (strncmp(f, MCHP_FCNN_FLAG, sizeof(MCHP_FCNN_FLAG)-1) == 0)
        {
          add_section_code_flag++;
          f += sizeof(MCHP_FCNN_FLAG)-1;
        }
      else if (strncmp(f, MCHP_FCNS_FLAG, sizeof(MCHP_FCNS_FLAG)-1) == 0)
        {
          add_section_code_flag++;
          f += sizeof(MCHP_FCNS_FLAG)-1;
        }
      else if (strncmp(f, MCHP_DATA_FLAG, sizeof(MCHP_DATA_FLAG)-1) == 0)
        {
          flags |= SECTION_WRITE;
          f += sizeof(MCHP_DATA_FLAG)-1;
        }
      else if (strncmp(f, MCHP_CONST_FLAG, sizeof(MCHP_CONST_FLAG)-1) == 0)
        {
          flags |= SECTION_READ_ONLY;
          f += sizeof(MCHP_DATA_FLAG)-1;
        }
      else if (strncmp(f, MCHP_PRST_FLAG, sizeof(MCHP_PRST_FLAG)-1) == 0)
        {
          flags |= SECTION_PERSIST;
          f += sizeof(MCHP_PRST_FLAG)-1;
        }
      else if (strncmp(f, MCHP_NEAR_FLAG, sizeof(MCHP_NEAR_FLAG)-1) == 0)
        {
          flags |= SECTION_NEAR;
          f += sizeof(MCHP_NEAR_FLAG)-1;
        }
      else if (strncmp(f, MCHP_ADDR_FLAG, sizeof(MCHP_ADDR_FLAG)-1) == 0)
        {
          flags |= SECTION_ADDRESS;
          f += sizeof(MCHP_ADDR_FLAG)-1;
        }
      else if (strncmp(f, MCHP_NOLOAD_FLAG, sizeof(MCHP_NOLOAD_FLAG)-1) == 0)
        {
          flags |= SECTION_NOLOAD;
          f += sizeof(MCHP_NOLOAD_FLAG)-1;
        }
      else if (strncmp(f, MCHP_BSS_FLAG, sizeof(MCHP_BSS_FLAG)-1) == 0)
        {
          flags |= SECTION_BSS;
          f += sizeof(MCHP_BSS_FLAG)-1;
        }
      else if (strncmp(f, MCHP_KEEP_FLAG, sizeof(MCHP_KEEP_FLAG)-1) == 0)
        {
          flags |= SECTION_KEEP;
          f += sizeof(MCHP_KEEP_FLAG)-1;
        }
      else if (strncmp(f, MCHP_COHERENT_FLAG, sizeof(MCHP_COHERENT_FLAG)-1) == 0)
        {
          flags |= SECTION_COHERENT;
          f += sizeof(MCHP_COHERENT_FLAG)-1;
        }
      else if (strncmp(f, MCHP_REGION_FLAG, sizeof(MCHP_REGION_FLAG)-1) == 0)
        {
          flags |= SECTION_REGION;
          f += sizeof(MCHP_REGION_FLAG)-1;
        }
      else
        {
          error("Could not determine flags for: '%s'", id);
          return flags;
        }
    }
  if (add_section_code_flag && !(flags & SECTION_RAMFUNC) && !(flags & SECTION_REGION))
    flags |= SECTION_CODE;
  for (v_flags = valid_section_flags; v_flags->flag_name; v_flags++)
    {
      if ((flags & v_flags->flag_mask) && (flags & v_flags->incompatible_with))
        {
          error("incompatible section flags for identifier '%s'",
                mchp_strip_name_encoding(id));
          return flags;
        }
    }
  return flags;
}

void mchp_apply_pragmas(tree decl) {

  int relayout=0;

  if (TREE_CODE(decl) == VAR_DECL) {
    if (mchp_pragma_align > 0) {
      DECL_ALIGN(decl) = (mchp_pragma_align) * BITS_PER_UNIT;
      DECL_USER_ALIGN (decl) = 1;
      relayout=1;
      mchp_pragma_align = 0;
    }
  } else if (TREE_CODE(decl) == FUNCTION_DECL) {

    if (mchp_pragma_scanf_args) {
      /* add format_arg attribute */
      tree attrib;
      tree args;

      args = build_tree_list(NULL_TREE, get_identifier("scanf"));
      args = chainon(args,
                     build_tree_list(NULL_TREE,build_int_cst(NULL_TREE,1)));
      args = chainon(args,
                     build_tree_list(NULL_TREE,build_int_cst(NULL_TREE,2)));
      attrib = build_tree_list(get_identifier("__format__"), args);
      attrib = chainon(DECL_ATTRIBUTES(decl), attrib);
#if 0
      attrib = chainon(DECL_ATTRIBUTES(decl), attrib);
      DECL_ATTRIBUTES(decl) = attrib;
#else
      decl_attributes(&decl, attrib, 0);
#endif
      mchp_pragma_scanf_args = 0;
    }
    if (mchp_pragma_printf_args) {
      /* add format_arg attribute */
      tree attrib;
      tree args;

      args = build_tree_list(NULL_TREE, get_identifier("printf"));
      args = chainon(args,
                     build_tree_list(NULL_TREE,build_int_cst(NULL_TREE,1)));
      args = chainon(args,
                     build_tree_list(NULL_TREE,build_int_cst(NULL_TREE,2)));
      attrib = build_tree_list(get_identifier("__format__"), args);
      attrib = chainon(DECL_ATTRIBUTES(decl), attrib);
#if 0
      attrib = chainon(DECL_ATTRIBUTES(decl), attrib);
      DECL_ATTRIBUTES(decl) = attrib;
#else
      decl_attributes(&decl, attrib, 0);
#endif
      mchp_pragma_printf_args = 0;
    }
  }

  if (mchp_pragma_section) {
    DECL_SECTION_NAME(decl) = mchp_pragma_section;
    mchp_pragma_section = NULL_TREE;
  }

  if (mchp_pragma_inline) {
#if 0
      DECL_DISREGARD_INLINE_LIMITS(decl) = 1;
#endif
      DECL_DECLARED_INLINE_P(decl) = 1;
      mchp_pragma_inline = 0;
  }

  if (mchp_pragma_keep) {
    tree attrib;

    /* make a fake KEEP attribute and add it to the decl */
    attrib = build_tree_list(get_identifier ("keep"), NULL_TREE);
    attrib = chainon(DECL_ATTRIBUTES(decl), attrib);
    decl_attributes(&decl, attrib, 0);

    DECL_UNIQUE_SECTION(decl) = 1;
    mchp_pragma_keep = 0;
  }

  if (mchp_pragma_coherent) {
    tree attrib;

    /* make a fake coherent attribute and add it to the decl */
    attrib = build_tree_list(get_identifier ("coherent"), NULL_TREE);
    attrib = chainon(DECL_ATTRIBUTES(decl), attrib);
    decl_attributes(&decl, attrib, 0);

    DECL_UNIQUE_SECTION(decl) = 1;
    mchp_pragma_coherent = 0;
  }

  if (relayout) relayout_decl(decl);

  if (mchp_pragma_align) {
    warning(OPT_Wpragmas, "ignoring #pragma align");
    mchp_pragma_align = 0;
  }
  if (mchp_pragma_scanf_args) {
    warning(OPT_Wpragmas, "ignoring #pragma scanf_args");
    mchp_pragma_scanf_args = 0;
  }
  if (mchp_pragma_printf_args) {
    warning(OPT_Wpragmas, "ignoring #pragma printf_args");
    mchp_pragma_printf_args = 0;
  }
}


/************************************************************************/
/*
** mchp_const_section_asm_op()
**
** Constants go in the code/data window, hence the .const section
** is marked as executable or data (depending on the command-line),
** so that the assembler knows the word width.
*/
/************************************************************************/
const char *mchp_rdata_section_asm_op(void)
{
  static char szSection[32];
#if 1
  lfInExecutableSection = TRUE;

  set_section_stack(SECTION_NAME_CONST, SECTION_READ_ONLY);

  sprintf(szSection, "\t.section\t%s,%s", lSectionStack->pszName,
          SECTION_ATTR_CODE);
#else
  sprintf(szSection, "\t.section\t%s,%s", ".rodata",
          SECTION_ATTR_CODE);
#endif
  return(szSection);
}

void pic32_system_include_paths (const char *sysroot, const char *iprefix,
                                 int cxx_stdinc)
{
  const struct default_include *p;
  size_t len;

  if (!TARGET_LEGACY_LIBC && !TARGET_XC32_LIBCPP)
    return;

  if (iprefix && (len = cpp_GCC_INCLUDE_DIR_len) != 0)
    {
      /* Look for directories that start with the standard prefix.
      "Translate" them, ie. replace /usr/local/lib/gcc... with
      IPREFIX and search them first.  */
      for (p = cpp_include_defaults; p->fname; p++)
        {
          if (/* !p->cplusplus || */ cxx_stdinc)
            {
              /* Should we be translating sysrooted dirs too?  Assume
              that iprefix and sysroot are mutually exclusive, for
              now.  */
              if (sysroot && p->add_sysroot)
                continue;
              if (!strncmp (p->fname, cpp_GCC_INCLUDE_DIR, len))
                {
                  char *str;
                  char *newfname;

                  if (TARGET_XC32_LIBCPP)
                    {
                      newfname = concat (p->fname, "/Cpp/c", NULL);
                      str = concat (iprefix, newfname + len, NULL);
                      free(newfname);
                      add_path (str, SYSTEM, p->cxx_aware, false);
                      newfname = concat (p->fname, "/Cpp", NULL);
                      str = concat (iprefix, newfname + len, NULL);
                      free(newfname);
                      add_path (str, SYSTEM, p->cxx_aware, false);
                    }
                  else if (TARGET_LEGACY_LIBC)
                    {
                      newfname = concat (p->fname, "/lega-c", NULL);
                      str = concat (iprefix, newfname + len, NULL);
                      free(newfname);
                      add_path (str, SYSTEM, p->cxx_aware, false);
                    }

                  str = concat (iprefix, p->fname + len, NULL);
                  add_path (str, SYSTEM, p->cxx_aware, false);

                }
            }
        }
    }

  for (p = cpp_include_defaults; p->fname; p++)
    {
      if (/* !p->cplusplus || */ cxx_stdinc)
        {
          char *str;
          char *str2;
          char *newfname;
          char *newfname2;

          /* Should this directory start with the sysroot?  */
          if (sysroot && p->add_sysroot)
            {
              if (TARGET_XC32_LIBCPP)
                {
                  newfname2 = concat (p->fname, "/Cpp/c", NULL);
                  str2 = concat (sysroot, newfname2, NULL);
                  add_path (str2, SYSTEM, p->cxx_aware, false);
                  free(newfname2);
                  newfname = concat (p->fname, "/Cpp", NULL);
                  str = concat (sysroot, newfname, NULL);
                  add_path (str, SYSTEM, p->cxx_aware, false);
                  free(newfname);
                }
              else if (TARGET_LEGACY_LIBC)
                {
                  newfname = concat (p->fname, "/lega-c", NULL);
                  str = concat (sysroot, newfname, NULL);
                  add_path (str, SYSTEM, p->cxx_aware, false);
                  free(newfname);
                }
              else
                {
                  str = concat (sysroot, p->fname, NULL);
                }
            }
          else
            {
              if (TARGET_XC32_LIBCPP)
                {
                  newfname2 = concat (p->fname, "/Cpp/c", NULL);
                  str2 = update_path (newfname2, p->component);
                  add_path (str2, SYSTEM, p->cxx_aware, false);
                  free(newfname2);
                  newfname = concat (p->fname, "/Cpp", NULL);
                  str = update_path (newfname, p->component);
                  add_path (str, SYSTEM, p->cxx_aware, false);
                  free(newfname);
                }
              else if (TARGET_LEGACY_LIBC)
                {
                  newfname = concat (p->fname, "/lega-c", NULL);
                  str = update_path (newfname, p->component);
                  add_path (str, SYSTEM, p->cxx_aware, false);
                  free(newfname);
                }
              else
                {
                  str = update_path (p->fname, p->component);
                  add_path (str, SYSTEM, p->cxx_aware, false);
                }
            }
        }
    }
}


void push_cheap_rtx(cheap_rtx_list **l, rtx x, tree t, int flag) {
  cheap_rtx_list *item;

  if (l == 0) return;
  item = (struct cheap_rtx_list*) xmalloc(sizeof(cheap_rtx_list));
  gcc_assert(item != NULL);
  item->x = x;
  item->t = t;
  item->flag = flag;
  item->next = *l;

  *l = item;
}

rtx pop_cheap_rtx(cheap_rtx_list **l, tree *t, int *flag) {
  cheap_rtx_list *item;
  rtx result;

  if (l == 0) return 0;
  item = (*l);
  if (item == 0) return 0;
  *l = item->next;
  if (t) *t = item->t;
  if (flag) *flag = item->flag;
  result = item->x;
  free(item);
  item = NULL;
  return result;
}

#endif
