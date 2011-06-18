/* Subroutines used for Microchip PIC32 MCU support
   Copyright (C) 1989, 1990, 1991, 1993, 1994, 1995, 1996, 1997, 1998,
   1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007
   Free Software Foundation, Inc.
   Contributed by J. Grosbach, james.grosbach@microchip.com, and
   T. Kuhrt, tracy.kuhrt@microchip.com
   Changes by J. Kajita, jason.kajita@microchip.com, and
   G. Loegel, george.loegel@microchip.com

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

#include "config.h"
#include "system.h"
#include "coretypes.h"
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
#include "integrate.h"
#include "langhooks.h"
#include "cfglayout.h"
#include "sched-int.h"
#include "gimple.h"
#include "bitmap.h"
#include "diagnostic.h"

#include <stdio.h>
#include "cpplib.h"
#include "mchp-protos.h"
#include "mchp-pragmas.h"
#include "c-common.h"
#include "c-pragma.h"
#include "c-tree.h"
#include "config/mips/mips-machine-function.h"
#ifdef __MINGW32__
void *alloca(size_t);
#else
#include <alloca.h>
#endif

#ifndef BITSET_P
/* True if bit BIT is set in VALUE.  */
#define BITSET_P(VALUE, BIT) (((VALUE) & (1 << (BIT))) != 0)
#endif

#define JOIN2(X,Y) (X ## Y)
#define JOIN(X,Y) JOIN2(X,Y)
#define MCHP_LL(X) JOIN2(X,LL)


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

typedef enum mchp_interesting_fn_info_ {
  info_invalid,
  info_I,
  info_O,
  info_O_v,
  info_dbl
} mchp_interesting_fn_info;

typedef enum mchp_conversion_status_ {
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

typedef struct mchp_intersting_fn_ {
  const char *name;
  const char *map_to;
  mchp_interesting_fn_info conversion_style;
  unsigned int interesting_arg;
  unsigned int function_convertable;
  mchp_conversion_status conv_flags;
  char *encoded_name;
} mchp_interesting_fn;

/* The nesting depth of .set noat */
extern struct mips_asm_switch mips_noat;

static HOST_WIDE_INT mchp_interrupt_priority;
static HOST_WIDE_INT mchp_isr_backcompat = 0;

static HOST_WIDE_INT mchp_offset_v0 = 0;
static HOST_WIDE_INT mchp_offset_v1 = 0;
static HOST_WIDE_INT mchp_offset_status = 0;
static HOST_WIDE_INT mchp_offset_epc = 0;
static HOST_WIDE_INT mchp_offset_srsctl = 0;

static HOST_WIDE_INT mchp_invalid_ipl_warning = 0;
HOST_WIDE_INT mchp_pic32_license_valid = 1;

static tree mchp_function_interrupt_p (tree decl);
static int mchp_function_naked_p (tree func);

static int mchp_vector_attribute_compound_expr (tree *node, tree expr,
    bool *no_add_attrs);
int mchp_epilogue_uses (unsigned regno);

static void mchp_output_configuration_words (void);
static void mchp_output_vector_dispatch_table (void);
static int mchp_vector_attribute_compound_expr (tree *node, tree expr,
    bool *no_add_attrs);
inline int
mchp_register_interrupt_context_p (unsigned regno);
static mchp_interesting_fn *mchp_match_conversion_fn(const char *name);

extern void mips_set_mips16_micromips_mode (int mips16_p, int micromips_p);
extern void mips_for_each_saved_gpr_and_fpr (HOST_WIDE_INT sp_offset, mchp_save_restore_fn fn);
extern void mips_restore_reg (rtx reg, rtx mem);

struct vector_dispatch_spec *vector_dispatch_list_head;

struct mchp_config_specification *mchp_configuration_values;

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

  /* Don't emit DWARF3/4 unless specifically selected. */
  if (dwarf_strict < 0)
    dwarf_strict = 1;
}

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
mchp_handle_configuration_setting (const char *name, const char *value_name)
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
                  if (strcmp (value->name, value_name) == 0)
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

static void
mchp_output_configuration_words (void)
{
  struct mchp_config_specification *spec;

  fputs ("# MCHP configuration words\n", asm_out_file);

  for (spec = mchp_configuration_values ; spec ; spec = spec->next)
    {
      /* if there are referenced bits in the word, output its value */
      if (spec->referenced_bits)
        {
          fprintf (asm_out_file, "# Configuration word @ 0x%08x\n", spec->word->address);
          fprintf (asm_out_file, "\t.section\t.config_%08X, \"a\",@progbits\n",
                   spec->word->address);
          fprintf (asm_out_file, "\t.type\t__config_%08X, @object\n",
                   spec->word->address);
          fprintf (asm_out_file, "\t.size\t__config_%08X, 4\n",
                   spec->word->address);
          fprintf (asm_out_file, "__config_%08X:\n", spec->word->address);
          fprintf (asm_out_file, "\t.word\t%u\n", spec->value);
        }
    }
}

void mchp_file_end (void)
{
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
  /* If this attribute isn't on the actual function declaration, we
     ignore it */
  if (TREE_CODE (decl) != FUNCTION_DECL)
    return NULL_TREE;

  /* The vector attribute has a comma delimited list of vector #'s as
     arguments. At least one must be present. */
  gcc_assert (args);
  while (args)
    {
      if (TREE_CODE (TREE_VALUE (args)) == INTEGER_CST)
        {
          /* The argument must be an integer constant between 0 and 63 */
          if ((int)TREE_INT_CST_LOW (TREE_VALUE (args)) < 0 ||
              (int)TREE_INT_CST_LOW (TREE_VALUE (args)) > 63)
            {
              *no_add_attrs = 1;

              error ("Vector number must be an integer between 0 and 63");
              return NULL_TREE;
            }
          /* add the vector to the list of dispatch functions to emit */
          mchp_add_vector_dispatch_entry (
            IDENTIFIER_POINTER (DECL_NAME (*node)),
            (int)TREE_INT_CST_LOW (TREE_VALUE (args)));
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
              error ("Vector number must be an integer between 0 and 63");
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

  if (expr == NULL_TREE)
    return 0;
  for (; TREE_CODE (expr) == COMPOUND_EXPR; expr = TREE_OPERAND (expr, 1))
    {
      len += mchp_vector_attribute_compound_expr (node, TREE_OPERAND (expr, 0),
             no_add_attrs);
    }
  if (TREE_CODE (expr) == INTEGER_CST)
    {
      /* The argument must be an integer constant between 0 and 63 */
      if ((int)TREE_INT_CST_LOW (expr) < 0 ||
          (int)TREE_INT_CST_LOW (expr) > 63)
        {
          *no_add_attrs = 1;

          error ("Vector number must be an integer between 0 and 63");
          return 0;
        }
      /* add the vector to the list of dispatch functions to emit */
      mchp_add_vector_dispatch_entry (
        IDENTIFIER_POINTER (DECL_NAME (*node)),
        (int)TREE_INT_CST_LOW (expr));
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
  char scn_name[11];

  /* If this attribute isn't on the actual function declaration, we
     ignore it */
  if (TREE_CODE (decl) != FUNCTION_DECL)
    return NULL_TREE;

  if (DECL_SECTION_NAME (decl) != NULL_TREE)
    {
      error ("the 'at_vector' attribute cannot be used with the 'section' attribute");
      *no_add_attrs = true;
      return NULL_TREE;
    }
  /* The argument must be an integer constant between 0 and 63 */
  if (TREE_CODE (TREE_VALUE (args)) != INTEGER_CST ||
      (int)TREE_INT_CST_LOW (TREE_VALUE (args)) < 0 ||
      (int)TREE_INT_CST_LOW (TREE_VALUE (args)) > 63)
    {
      *no_add_attrs = 1;

      error ("Interrupt vector number must be an integer between 0 and 63");
      return NULL_TREE;
    }
  /* now mark the decl as going into the section for the indicated vector */
  sprintf (scn_name, ".vector_%d", (int)TREE_INT_CST_LOW (TREE_VALUE (args)));
  DECL_SECTION_NAME (decl) = build_string (strlen (scn_name), scn_name);

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
      fprintf (asm_out_file, "\t.section\t.vector_%d,\"ax\",@progbits\n",
               dispatch_entry->vector_number);
      fprintf (asm_out_file, "\t.align\t2\n");
      fprintf (asm_out_file, "\t.set\tnomips16\n");
      fprintf (asm_out_file, "\t.ent\t__vector_dispatch_%d\n",
               dispatch_entry->vector_number);
      fprintf (asm_out_file, "__vector_dispatch_%d:\n",
               dispatch_entry->vector_number);
      fprintf (asm_out_file, "\tj\t%s\n", dispatch_entry->target);
      fprintf (asm_out_file, "\t.end\t__vector_dispatch_%d\n",
               dispatch_entry->vector_number);
      fprintf (asm_out_file, "\t.size\t__vector_dispatch_%d, .-"
               "__vector_dispatch_%d\n",
               dispatch_entry->vector_number,
               dispatch_entry->vector_number);
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
      /* Determine if an interrupt pragma is around for this function */
      fname = IDENTIFIER_POINTER (DECL_NAME (decl));
      gcc_assert (fname);
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
              char scn_name[11];
              gcc_assert (p->vect_number < 64);
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

  /* mips_set_mips16_mode (SYMBOL_REF_MIPS16_P (XEXP (DECL_RTL (fndecl), 0))); */

#ifdef TARGET_MCHP_PIC32MX
  /* If we're in MIPS16[e] mode for an interrupt function, that's an error */
  mchp_function_interrupt_p (fndecl);
  if ((current_function_type != NON_INTERRUPT) && TARGET_MIPS16)
    {
      warning (0, "interrupt handler functions cannot be mips16, "
               "assuming nomips16 attribute");
      mips_set_mips16_micromips_mode (0,0);
    }
#endif
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
      error ("Interrupt priority must be specified as 'single' or 'IPLn[AUTO|SOFT|SRS]', "
             "where n is in the range of 0..7, inclusive.");
      *no_add_attrs = 1;
      return NULL_TREE;
    }

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

void
mchp_expand_epilogue_return(rtx ret_insn)
{
  if (current_function_type == NON_INTERRUPT)
    {
      emit_jump_insn (ret_insn);
    }
  /* interrupt functions need to emit a placeholder instruction
     so that this pattern will return a non-empty expansion.
     we don't want anything below this (there shouldn't be any
     RTL following this, anyway) moved above it, so we'll use a
     blockage. */
  else
    {
      emit_insn (gen_blockage ());
    }
}

/* Expand the prologue into a bunch of separate insns at the end of the prologue.  */
void
mchp_expand_prologue_end (const struct mips_frame_info *frame)
{
  if ((current_function_type == AUTO_CONTEXT_SAVE) ||
      ((current_function_type != NON_INTERRUPT) && frame->has_hilo_context))
    {
      /* Don't allow instructions from the function to be moved into the
         software context saving code. */
      emit_insn (gen_blockage ());
    }
}

void
mchp_expand_epilogue_restoreregs (HOST_WIDE_INT step1,
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
              mips_for_each_saved_acc (frame->total_size - step2, mips_restore_reg);
              mips_for_each_saved_gpr_and_fpr (frame->total_size - step2,
                                               mips_restore_reg);
              if (!cfun->machine->keep_interrupts_masked_p
                  && (cfun->machine->interrupt_priority < 7))
                {
                  /* Load the original EPC.  */
                  gcc_assert (mchp_offset_epc != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (stack_pointer_rtx, mchp_offset_epc));
                  mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }

              /* Load the original Status.  */
              gcc_assert (mchp_offset_status >= 0);
              mem = gen_frame_mem (word_mode,
                                   plus_constant (stack_pointer_rtx, mchp_offset_status));
              mips_emit_move (gen_rtx_REG (word_mode, K1_REG_NUM), mem);
              offset -= UNITS_PER_WORD;

              if (!cfun->machine->keep_interrupts_masked_p
                  && (cfun->machine->interrupt_priority < 7))
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
                                       plus_constant (stack_pointer_rtx, mchp_offset_srsctl));
                  mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }
              /* If we don't use shoadow register set, we need to update SP.  */
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
              mips_for_each_saved_acc (frame->total_size - step2, mips_restore_reg);
              mips_for_each_saved_gpr_and_fpr (frame->total_size - step2,
                                               mips_restore_reg);
              if (7 == cfun->machine->interrupt_priority)
                {
                  if (mchp_save_srsctl)
                    {
                      /* Load the original SRSCTL.  */
                      /* Since we are in IPL7, we can use K0/K1 without executing a DI first. */
                      gcc_assert (mchp_offset_srsctl != 0);
                      mem = gen_frame_mem (word_mode,
                                           plus_constant (stack_pointer_rtx, mchp_offset_srsctl));
                      mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                      offset -= UNITS_PER_WORD;
                    }
                  /* Load the original Status.  */
                  gcc_assert (mchp_offset_status != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (stack_pointer_rtx, mchp_offset_status));
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
                                           plus_constant (stack_pointer_rtx, mchp_offset_srsctl));
                      mips_emit_move (gen_rtx_REG (word_mode, K1_REG_NUM), mem);
                      offset -= UNITS_PER_WORD;
                    }
                  if (!cfun->machine->keep_interrupts_masked_p)
                    {
                      /* Load the original EPC to K0.  */
                      gcc_assert (mchp_offset_epc != 0);
                      mem = gen_frame_mem (word_mode,
                                           plus_constant (stack_pointer_rtx, mchp_offset_epc));
                      mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                      offset -= UNITS_PER_WORD;
                    }
                  if (mchp_save_srsctl)
                    {
                      /* Restore previously loaded SRSCTL.  */
                      emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_SRSCTL_REG_NUM),
                                                gen_rtx_REG (SImode, K1_REG_NUM)));
                    }
                  /* Load the original Status.  */
                  gcc_assert (mchp_offset_status != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (stack_pointer_rtx, mchp_offset_status));
                  mips_emit_move (gen_rtx_REG (word_mode, K1_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                  if (!cfun->machine->keep_interrupts_masked_p)
                    {
                      /* Restore the original EPC.  */
                      emit_insn (gen_cop0_move (gen_rtx_REG (SImode, COP0_EPC_REG_NUM),
                                                gen_rtx_REG (SImode, K0_REG_NUM)));
                    }
                }
              else /* (interrupt_priority == 7) */
                {
                  /* Update SP.  */
                  if (step2 > 0)
                    {
                      emit_insn (gen_add3_insn (stack_pointer_rtx,
                                                stack_pointer_rtx,
                                                GEN_INT (step2)));
                    }
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
              rtx end_soft_save;
              /* Restore the registers.  */
              mips_for_each_saved_acc (frame->total_size - step2, mips_restore_reg);

              /* Load the original SRSCTL.  */
              gcc_assert (mchp_offset_srsctl != 0);
              mem = gen_frame_mem (word_mode,
                                   plus_constant (stack_pointer_rtx, mchp_offset_srsctl));
              mips_emit_move (gen_rtx_REG (word_mode, V0_REGNUM), mem);
              emit_insn (gen_andsi3 (gen_rtx_REG (SImode, V0_REGNUM),
                                     gen_rtx_REG (SImode, V0_REGNUM), GEN_INT((unsigned)0x0F) ));
              do_save_label = gen_label_rtx ();
              skip_save_label = gen_label_rtx ();

              cond = gen_rtx_NE(SImode,gen_rtx_REG(SImode, V0_REGNUM),const0_rtx);
              jumpskip =  emit_jump_insn (gen_condjump (cond, skip_save_label));
              JUMP_LABEL (jumpskip) = skip_save_label;

              JUMP_LABEL (jumpskip) = skip_save_label;
              LABEL_NUSES (skip_save_label) = 1;

              emit_label (do_save_label);
              mips_for_each_saved_gpr_and_fpr (frame->total_size - step2,
                                               mips_restore_reg);
              emit_label (skip_save_label);

              if (!cfun->machine->keep_interrupts_masked_p
                  && (cfun->machine->interrupt_priority < 7))
                {
                  /* Load the original EPC.  */
                  gcc_assert (mchp_offset_epc != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (stack_pointer_rtx, mchp_offset_epc));
                  mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }

              /* Load the original Status.  */
              gcc_assert (mchp_offset_status != 0);
              mem = gen_frame_mem (word_mode,
                                   plus_constant (stack_pointer_rtx, mchp_offset_status));
              mips_emit_move (gen_rtx_REG (word_mode, K1_REG_NUM), mem);
              offset -= UNITS_PER_WORD;

              if (!cfun->machine->keep_interrupts_masked_p
                  && (cfun->machine->interrupt_priority < 7))
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
                                       plus_constant (stack_pointer_rtx, mchp_offset_srsctl));
                  mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }
              /* Update SP.  */
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
              rtx end_soft_save;

              /* Restore the registers.  */
              mips_for_each_saved_acc (frame->total_size - step2, mips_restore_reg);

              /* Load the original SRSCTL.  */
              gcc_assert (mchp_offset_srsctl != 0);
              mem = gen_frame_mem (word_mode,
                                   plus_constant (stack_pointer_rtx, mchp_offset_srsctl));
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

              /* Branch over the software context saving only if it will
                 save cycles */
              if (savecount > 3)
                {
                  cond = gen_rtx_NE(SImode,gen_rtx_REG(SImode, V0_REGNUM),const0_rtx);
                  jumpskip =  emit_jump_insn (gen_condjump (cond, skip_save_label));
                  JUMP_LABEL (jumpskip) = skip_save_label;
                }
              LABEL_NUSES (skip_save_label) = 1;

              emit_label (do_save_label);
              mips_for_each_saved_gpr_and_fpr (frame->total_size - step2,
                                               mips_restore_reg);
              emit_label (skip_save_label);

              if (!cfun->machine->keep_interrupts_masked_p
                  && (cfun->machine->interrupt_priority < 7))
                {
                  /* Load the original EPC.  */
                  gcc_assert (mchp_offset_epc != 0);
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (stack_pointer_rtx, mchp_offset_epc));
                  mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }

              /* Load the original Status.  */
              gcc_assert (mchp_offset_status != 0);
              mem = gen_frame_mem (word_mode,
                                   plus_constant (stack_pointer_rtx, mchp_offset_status));
              mips_emit_move (gen_rtx_REG (word_mode, K1_REG_NUM), mem);
              offset -= UNITS_PER_WORD;

              if (!cfun->machine->keep_interrupts_masked_p
                  && (cfun->machine->interrupt_priority < 7))
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
                                       plus_constant (stack_pointer_rtx, mchp_offset_srsctl));
                  mips_emit_move (gen_rtx_REG (word_mode, K0_REG_NUM), mem);
                  offset -= UNITS_PER_WORD;
                }
              /* update SP.  */
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
                                           mips_restore_reg);
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
  HOST_WIDE_INT num_intreg;   /* # words needed for interrupt context regs */
  bool has_interrupt_context;   /* true if interrupt context is saved */
  bool has_hilo_context;        /* true if hi and lo registers are saved */

  tree ipl_tree;
  unsigned ipl_len;

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
             original MX3/MX4 family. The MX5, MX6, MX7 family introduces configurable
             shadow registers. You can map your shadow registers to an IPL level
             using configuration fuses. */
          ipl_len = strlen(IDENTIFIER_POINTER (TREE_VALUE (ipl_tree)));
          if (ipl_len == (sizeof("ipl7")-1))
            {
#if 1
              /* For chipKIT, assume AUTO if the context-save mechanism 
                 isn't specified */
              cfun->machine->current_function_type = 
              current_function_type = AUTO_CONTEXT_SAVE;
#else
              if (cfun->machine->interrupt_priority == 7)
                {
                  cfun->machine->current_function_type = 
                    current_function_type = SRS_CONTEXT_SAVE;
                  cfun->machine->use_shadow_register_set_p = true;
                  mchp_isr_backcompat = 1; /* No need to save SRSCTL */
                }
              else
                {
                  cfun->machine->current_function_type = current_function_type = SOFTWARE_CONTEXT_SAVE;
                  mchp_isr_backcompat = 1; /* No need to save SRSCTL */
                }
#endif
            }
          else if ((ipl_len > sizeof("ipl7")) && (ipl_len < (sizeof("ipl7software"))))
            {
              mchp_isr_backcompat = 0; /* Might need to save SRSCTL */
              if ((NULL != strstr(IDENTIFIER_POINTER (TREE_VALUE (ipl_tree)),"SRS")) ||
                  (NULL != strstr(IDENTIFIER_POINTER (TREE_VALUE (ipl_tree)),"srs")))
                {
                  cfun->machine->current_function_type = current_function_type = SRS_CONTEXT_SAVE;
                  cfun->machine->use_shadow_register_set_p = true;
                  if (ipl_len > 7)
                    {
                      if (0 == mchp_invalid_ipl_warning)
                        {
                          warning(0, "Invalid ISR context-saving mode, assuming IPL"
                                  HOST_WIDE_INT_PRINT_DEC "SRS ",
                                  cfun->machine->interrupt_priority);
                          mchp_invalid_ipl_warning++;
                        }
                    }
                }
              else if ((NULL != strstr(IDENTIFIER_POINTER
                                       (TREE_VALUE (ipl_tree)),"SOFT")) ||
                       (NULL != strstr(IDENTIFIER_POINTER
                                       (TREE_VALUE (ipl_tree)),"soft"))
                      )
                {
                  cfun->machine->current_function_type = current_function_type = SOFTWARE_CONTEXT_SAVE;
                  if (ipl_len > sizeof("IPLnSOFT"))
                    {
                      if (0 == mchp_invalid_ipl_warning)
                        {
                          warning(0, "Invalid ISR context-saving mode, assuming IPL"
                                  HOST_WIDE_INT_PRINT_DEC "SOFT ",
                                  cfun->machine->interrupt_priority);
                          mchp_invalid_ipl_warning++;
                        }
                    }
                }
              else if ((NULL != strstr(IDENTIFIER_POINTER
                                       (TREE_VALUE (ipl_tree)),"STACK")) ||
                       (NULL != strstr(IDENTIFIER_POINTER
                                       (TREE_VALUE (ipl_tree)),"stack"))
                      )
                {
                  cfun->machine->current_function_type = current_function_type = SOFTWARE_CONTEXT_SAVE;
                  if (ipl_len > sizeof("IPLnSTACK"))
                    {
                      if (0 == mchp_invalid_ipl_warning)
                        {
                          warning(0, "Invalid ISR context-saving mode, assuming IPL"
                                  HOST_WIDE_INT_PRINT_DEC "SOFT ", cfun->machine->interrupt_priority);
                          mchp_invalid_ipl_warning++;
                        }
                    }
                }
              else if ((NULL != strstr(IDENTIFIER_POINTER
                                       (TREE_VALUE (ipl_tree)),"AUTO")) ||
                       (NULL != strstr(IDENTIFIER_POINTER
                                       (TREE_VALUE (ipl_tree)),"auto"))
                      )
                {
                  /* Determine shadow or SRS context save at runtime. */
                  cfun->machine->current_function_type = current_function_type = AUTO_CONTEXT_SAVE;
                }
              else
                {
                  if (0 == mchp_invalid_ipl_warning)
                    {
                      warning(0, "Invalid ISR context-saving mode, assuming IPL"
                              HOST_WIDE_INT_PRINT_DEC "AUTO", cfun->machine->interrupt_priority);
                      mchp_invalid_ipl_warning++;
                    }
                  cfun->machine->current_function_type = current_function_type = AUTO_CONTEXT_SAVE;
                }
            }
          else
            {
              if (0 == mchp_invalid_ipl_warning)
                {
                  warning(0, "Invalid ISR context-saving mode, assuming IPL"
                          HOST_WIDE_INT_PRINT_DEC "AUTO", cfun->machine->interrupt_priority);
                  mchp_invalid_ipl_warning++;
                }
              cfun->machine->current_function_type = current_function_type = AUTO_CONTEXT_SAVE;
            }
        }
      if (mchp_function_naked_p (current_function_decl))
        error ("interrupt handler functions cannot also be naked functions");
    }

  /* has_interrupt_context tells us whether we're saving any interrupt
     specific data */
  has_interrupt_context = (cfun->machine->current_function_type != NON_INTERRUPT);
  /* has_hilo_context is true if we need to push/pop HI and LO */
  has_hilo_context = 0;
  num_intreg = 0;

  /* add in space for the interrupt context information */
  if (has_interrupt_context)
    {
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
          df_set_regs_ever_live (V1_REGNUM, true);
          fixed_regs[V1_REGNUM] = call_really_used_regs[V1_REGNUM] =
                                        call_used_regs[V1_REGNUM] = 1;
        }
      else if ((cfun->machine->current_function_type == SRS_CONTEXT_SAVE) ||
               (cfun->machine->current_function_type == SOFTWARE_CONTEXT_SAVE))
        {
          if ((cfun->machine->interrupt_priority < 7) && (0 == mchp_isr_backcompat))
            {
              /* If we are not in Daytona compatibility mode, save SRSCTL. */
              num_intreg++;
              df_set_regs_ever_live (V1_REGNUM, true);
              fixed_regs[V1_REGNUM] = call_really_used_regs[V1_REGNUM] =
                                        call_used_regs[V1_REGNUM] = 1;
            }
        }
      /* If HI/LO is defined in this function, we need to save them too.
         If the function is not a leaf function, we assume that the
         called function uses them. */
      if (df_regs_ever_live_p (LO_REGNUM) || df_regs_ever_live_p (HI_REGNUM)
          || !current_function_is_leaf)
        {
          has_hilo_context = 1;
        }
      /* Both AUTO and SOFTWARE reserve space to save v0/v1 if necessary */
      if ((cfun->machine->current_function_type == SOFTWARE_CONTEXT_SAVE) ||
          (cfun->machine->current_function_type == AUTO_CONTEXT_SAVE) ||
          (cfun->machine->current_function_type == DEFAULT_CONTEXT_SAVE) )
        {
          /* AUTO and SOFTWARE use v0/v1 to save hi/lo */
          if (has_hilo_context)
            {
              df_set_regs_ever_live (V0_REGNUM, true);
              fixed_regs[V0_REGNUM] = call_really_used_regs[V0_REGNUM] =
                                        call_used_regs[V0_REGNUM] = 1;
              df_set_regs_ever_live (V1_REGNUM, true);
              fixed_regs[V1_REGNUM] = call_really_used_regs[V1_REGNUM] =
                                        call_used_regs[V1_REGNUM] = 1;
            }
        }
    }

  cfun->machine->frame.has_interrupt_context = has_interrupt_context;
  cfun->machine->frame.has_hilo_context = has_hilo_context;

  if ((num_intreg % 2) != 0)
    num_intreg++;

  return num_intreg;
}

void
mchp_output_function_prologue (FILE *file, HOST_WIDE_INT tsize, HOST_WIDE_INT size ATTRIBUTE_UNUSED)
{
  fprintf (file, "# Begin mchp_output_function_prologue\n");
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

/* Utility function to add an entry to the vector dispatch list */
void
mchp_add_vector_dispatch_entry (const char *target_name, int vector_number)
{
  struct vector_dispatch_spec *dispatch_entry;

  /* add the vector to the list of dispatch functions to emit */
  dispatch_entry = ggc_alloc (sizeof (struct vector_dispatch_spec));
  dispatch_entry->next = vector_dispatch_list_head;
  dispatch_entry->target = target_name;
  dispatch_entry->vector_number = vector_number;
  vector_dispatch_list_head = dispatch_entry;
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
              if ((interrupt_priority < 7) && !cfun->machine->keep_interrupts_masked_p)
                {
                  /* Push EPC into its stack slot.  */
                  gcc_assert (offset > 0);
                  mchp_offset_epc = offset;
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (stack_pointer_rtx,
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
                                   plus_constant (stack_pointer_rtx,
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
                  /* Clear UM, ERL, EXL, IPL in STATUS */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (15),
                                         GEN_INT (SR_EXL),
                                         gen_rtx_REG (SImode, GP_REG_FIRST)));
                  /* Set the IPL */
                  emit_insn (gen_iorsi3 (gen_rtx_REG (SImode, K1_REG_NUM),
                                         gen_rtx_REG (SImode, K1_REG_NUM),GEN_INT((unsigned)interrupt_priority << SR_IPL)));
                }
              /* We will move K1 to STATUS later in the generic MIPS code */
              emit_insn (gen_blockage ());
              mips_for_each_saved_gpr_and_fpr (size, mips_save_reg);
              /* Save HI/LO as late as possible to minimize stalls */
              mips_for_each_saved_acc (size, mips_save_reg);

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
              if ((interrupt_priority < 7)
                  &&  !cfun->machine->keep_interrupts_masked_p)
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

              if ((interrupt_priority < 7)
                  &&  !cfun->machine->keep_interrupts_masked_p)
                {
                  /* Push SRSCTL into its stack slot.  */
                  /*
                  gcc_assert (offset > 0);
                  */
                  mchp_offset_srsctl = offset;
                  mips_save_restore_reg (word_mode, K0_REG_NUM, mchp_offset_srsctl, mips_save_reg);
                  offset -= UNITS_PER_WORD;
                }

              if ((interrupt_priority >= 0) || 1)
                {
                  gcc_assert (interrupt_priority >= 0);
                  gcc_assert (interrupt_priority <= 7);
                  /* Clear UM, ERL, EXL, IPL in STATUS (K1) */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (15),
                                         GEN_INT (SR_EXL),
                                         gen_rtx_REG (SImode, GP_REG_FIRST)));
                  /* Set the IPL */
                  emit_insn (gen_iorsi3 (gen_rtx_REG (SImode, K1_REG_NUM),
                                         gen_rtx_REG (SImode, K1_REG_NUM),GEN_INT((unsigned)interrupt_priority << SR_IPL)));
                }
              /* Should not save anything because we are using a shadow register set. */
              mips_for_each_saved_gpr_and_fpr (size, mips_save_reg);
              /* Save HI/LO as late as possible to minimize stalls */
              mips_for_each_saved_acc (size, mips_save_reg);
            }
          else if (AUTO_CONTEXT_SAVE == current_function_type)
            {
              rtx skip_save_label;
              rtx do_save_label;
              rtx jumpskip;
              rtx cond;
              rtx end_soft_save;
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
              if ((interrupt_priority < 7) && !cfun->machine->keep_interrupts_masked_p)
                {
                  /* Push EPC into its stack slot.  */
                  mchp_offset_epc = offset;
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (stack_pointer_rtx,
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
              mips_save_restore_reg (word_mode, K0_REG_NUM, mchp_offset_srsctl, mips_save_reg);
              offset -= UNITS_PER_WORD;

              /* TODO: Do we need to do this if interrupts are masked? */
              if (interrupt_priority < 0)
                {
                  /* Move from COP0 Cause to K0.  */
                  emit_insn (gen_cop0_move (gen_rtx_REG (SImode, K0_REG_NUM),
                                            gen_rtx_REG (SImode,
                                                         COP0_CAUSE_REG_NUM)));
                }


              /* Push STATUS into its stack slot.  */
              mchp_offset_status = offset;
              mem = gen_frame_mem (word_mode,
                                   plus_constant (stack_pointer_rtx,
                                                  mchp_offset_status));
              mips_emit_move (mem, gen_rtx_REG (word_mode, K1_REG_NUM));
              offset -= UNITS_PER_WORD;

              if (interrupt_priority < 0)
                {

                  if (!cfun->machine->keep_interrupts_masked_p)
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
                }

              if (interrupt_priority >= 0)
                {
                  gcc_assert (interrupt_priority <= 7);
                  /* Clear UM, ERL, EXL, IPL in STATUS */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (15),
                                         GEN_INT (SR_EXL),
                                         gen_rtx_REG (SImode, GP_REG_FIRST)));
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
                                   plus_constant (stack_pointer_rtx,
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
              mips_for_each_saved_gpr_and_fpr (size, mips_save_reg);
              emit_label (skip_save_label);
              emit_insn (gen_blockage ());

              /* Save HI/LO as late as possible to minimize stalls */
              mips_for_each_saved_acc (size, mips_save_reg);
            } /* AUTO_CONTEXT_SAVE */
          else if (DEFAULT_CONTEXT_SAVE == cfun->machine->current_function_type)
            {
              rtx skip_save_label;
              rtx do_save_label;
              rtx jumpskip;
              rtx cond;
              rtx end_soft_save;
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
              if ((interrupt_priority < 7) && !cfun->machine->keep_interrupts_masked_p)
                {
                  /* Push EPC into its stack slot.  */
                  mchp_offset_epc = offset;
                  mem = gen_frame_mem (word_mode,
                                       plus_constant (stack_pointer_rtx,
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
                                   plus_constant (stack_pointer_rtx,
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
                                   plus_constant (stack_pointer_rtx,
                                                  mchp_offset_status));
              mips_emit_move (mem, gen_rtx_REG (word_mode, K1_REG_NUM));
              offset -= UNITS_PER_WORD;

              /* Right justify the CAUSE RIPL in k0.  */
              if (!cfun->machine->keep_interrupts_masked_p)
                {
                  emit_insn (gen_lshrsi3 (gen_rtx_REG (SImode, K0_REG_NUM),
                                          gen_rtx_REG (SImode, K0_REG_NUM),
                                          GEN_INT (CAUSE_IPL)));

                  /* Insert the RIPL into our copy of SR (k1) as the new IPL.  */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (6),
                                         GEN_INT (SR_IPL),
                                         gen_rtx_REG (SImode, K0_REG_NUM)));

                  /* Enable interrupts by clearing the KSU ERL and EXL bits.
                     IE is already the correct value, so we don't have to do
                     anything explicit.  */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (4),
                                         GEN_INT (SR_EXL),
                                         gen_rtx_REG (SImode, GP_REG_FIRST)));
                  /* We will move K1 to STATUS later in the generic MIPS code */
                }
              else /* cfun->machine->keep_interrupts_masked_p */
                {
                  /* Disable interrupts by clearing the KSU, ERL, EXL,
                     and IE bits.  */
                  emit_insn (gen_insvsi (gen_rtx_REG (SImode, K1_REG_NUM),
                                         GEN_INT (5),
                                         GEN_INT (SR_IE),
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
                                   plus_constant (stack_pointer_rtx,
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
              mips_for_each_saved_gpr_and_fpr (size, mips_save_reg);
              emit_label (skip_save_label);

              emit_insn (gen_blockage ());

              /* Save HI/LO as late as possible to minimize stalls */
              mips_for_each_saved_acc (size, mips_save_reg);
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
          mips_for_each_saved_gpr_and_fpr (size, mips_save_reg);
        }
    }
  return size;
}

bool mchp_subtarget_mips16_enabled (const_tree decl)
{
  static tree first_disabled_decl = NULL;
  static tree last_disabled_decl = NULL;
  static bool suppress_further_warnings = false;

  if (mchp_pic32_license_valid < 0)
    {
      if ((decl == first_disabled_decl) ||
          (decl == last_disabled_decl))
        {
          suppress_further_warnings = true;
        }

      if (false == suppress_further_warnings)
        {
          if (-100 == mchp_pic32_license_valid)
            {
              warning (0, "The lite edition does not support the %<mips16%>"
                       " attribute on '%qs', attribute ignored",
                       IDENTIFIER_POINTER (DECL_NAME (decl)));
            }
          else
            {
              warning (0, "The current limited compiler license does not support the %<mips16%>"
                       " attribute on '%qs', attribute ignored",
                       IDENTIFIER_POINTER (DECL_NAME (decl)));
            }
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

#if defined(C32_SMARTIO_RULES) || 1

#define CCS_FLAG_MASK (~(conv_c-1))
#define CCS_STATE_MASK (conv_c-1)
#define CCS_FLAG(X) ((X) & CCS_FLAG_MASK)
#define CCS_STATE(X) ((X) & CCS_STATE_MASK)


static mchp_interesting_fn mchp_fn_list[] =
{
  /*  name         map_to        style          arg c, conv_flags */
  { "_dasprintf",  "_dasprintf", info_dbl,    5,  0, 0, NULL },
  { "_dfprintf",   "_dfprintf",  info_dbl,    5,  0, 0, NULL },
  { "_dfscanf",    "_dfscanf",   info_dbl,    5,  0, 0, NULL },
  { "_dprintf",    "_dprintf",   info_dbl,    4,  0, 0, NULL },
  { "_dscanf",     "_dscanf",    info_dbl,    4,  0, 0, NULL },
  { "_dsnprintf",  "_dsnprintf", info_dbl,    6,  0, 0, NULL },
  { "_dsprintf",   "_dsprintf",  info_dbl,    5,  0, 0, NULL },
  { "_dsscanf",    "_dsscanf",   info_dbl,    5,  0, 0, NULL },
  { "_dvasprintf", "_dvasprintf",info_dbl,    5,  0, 0, NULL },
  { "_dvfprintf",  "_dvfprintf", info_dbl,    5,  0, 0, NULL },
  { "_dvfscanf",   "_dvfscanf",  info_dbl,    5,  0, 0, NULL },
  { "_dvprintf",   "_dvprintf",  info_dbl,    4,  0, 0, NULL },
  { "_dvsprintf",  "_dvsprintf", info_dbl,    5,  0, 0, NULL },
  { "_dvsscanf",   "_dvsscanf",  info_dbl,    5,  0, 0, NULL },
  { "asprintf",  "_asprintf",  info_O,        5,  0, 0, NULL },
  { "fprintf",   "_fprintf",   info_O,        5,  0, 0, NULL },
  { "fscanf",    "_fscanf",    info_I,        5,  0, 0, NULL },
  { "printf",    "_printf",    info_O,        4,  0, 0, NULL },
  { "scanf",     "_scanf",     info_I,        4,  0, 0, NULL },
  { "snprintf",  "_snprintf",  info_O,        6,  0, 0, NULL },
  { "sprintf",   "_sprintf",   info_O,        5,  0, 0, NULL },
  { "sscanf",    "_sscanf",    info_I,        5,  0, 0, NULL },
  { "vasprintf", "_vasprintf", info_O,        5,  0, 0, NULL },
  { "vfprintf",  "_vfprintf",  info_O,        5,  0, 0, NULL },
  { "vfscanf",   "_vfscanf",   info_I,        5,  0, 0, NULL },
  { "vprintf",   "_vprintf",   info_O,        4,  0, 0, NULL },
  { "vsnprintf", "_vsnprintf", info_O,        6,  0, 0, NULL },
  { "vsprintf",  "_vsprintf",  info_O,        5,  0, 0, NULL },
  { "vsscanf",   "_vsscanf",   info_I,        5,  0, 0, NULL },
  { 0,           0,            0,            -1,  0, 0, NULL }
};

/*
 *  strings values are thrown away after they are generated, but the
 *    a reference to the string will always return the same rtx... keep
 *    track of them here and they're conversion state
 */
enum { status_output = 0,
       status_input = 1 };

typedef struct mchp_conversion_cache_
{
  rtx rtl;
  mchp_conversion_status valid[2];
  struct mchp_conversion_cache_ *l,*r;
} mchp_conversion_cache;

int mchp_clear_fn_list=1;

static mchp_conversion_cache *mchp_saved_conversion_info;

static const char *mchp_strip_name_encoding_helper(const char *str)
{
  return str + (*str == '*');
}

/* By default, we do nothing for encode_section_info, so we need not
   do anything but discard the '*' marker.  */

const char *
mchp_strip_name_encoding (const char *symbol_name)
{
  const char *var;
  mchp_interesting_fn *match;
  var = mchp_strip_name_encoding_helper(symbol_name);

  if (mchp_io_size_val > 0) {
    match = mchp_match_conversion_fn(var);
    while (match) {
      if (match->function_convertable) {

#define CCS_ADD_FLAG(FLAG) \
        if (match->conv_flags & JOIN(conv_,FLAG)) { \
          *f++=#FLAG[0]; \
          added |=  JOIN(conv_,FLAG); }

#define CCS_ADD_FLAG_ALT(FLAG,ALT) \
        if ((match->conv_flags & JOIN(conv_,FLAG)) && \
            ((added & JOIN(conv_,ALT)) == 0)) {\
          *f++=#ALT[0]; \
          added |=  JOIN(conv_,ALT); }

        if (match->encoded_name == 0) {
          char extra_flags[sizeof("_aAcdeEfFgGnopsuxX0")] = "_";
          char *f = &extra_flags[1];
          mchp_conversion_status added;
          /*
           * order is important here
           *  add new flags alphabetically with lower case preceding uppercase
           *    ie _aAcdEfgG not
           *       _acdfgAEG
           */

          added = 0;
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

          if (strlen(extra_flags) > 1) {
            match->encoded_name = (char*)xmalloc(strlen(match->map_to) +
                                          strlen(extra_flags) + 1);
            sprintf(match->encoded_name,"%s%s", match->map_to, extra_flags);
          } else {
            /* we have no flags */
            match->encoded_name = (char*)xmalloc(strlen(match->map_to) + 3);
            sprintf(match->encoded_name,"%s_0", match->map_to, extra_flags);
          }
        }
        if (match->encoded_name) return match->encoded_name;
      }
      if (match[1].name &&
          (strcmp(match[1].name,var) == 0)) match++; else match = 0;
    }
  }
  return var;
}

static int mchp_bsearch_compare(const void *va, const void *vb) {
  mchp_interesting_fn *a = (mchp_interesting_fn *)va;
  mchp_interesting_fn *b = (mchp_interesting_fn *)vb;

  return strcmp(a->name, b->name);
}

static mchp_interesting_fn *mchp_match_conversion_fn(const char *name) {
  mchp_interesting_fn a,*res;
  a.name = name;

  res = bsearch(&a, mchp_fn_list,
                sizeof(mchp_fn_list)/sizeof(mchp_interesting_fn)-1,
                sizeof(mchp_interesting_fn), mchp_bsearch_compare);
  while (res && (res != mchp_fn_list)  && (strcmp(name, res[-1].name) == 0))
    res--;
  return res;
}

static void conversion_info(mchp_conversion_status state,
                            mchp_interesting_fn *fn_id) {
  /* dependant upon the conversion status and the setting of the smart-io
     option, set up the mchp_fn_list table. */

  fn_id->conv_flags = CCS_FLAG(fn_id->conv_flags) | state;
  if (mchp_io_size_val== 0) {
    if (fn_id->encoded_name) free(fn_id->encoded_name);
    fn_id->encoded_name = 0;
    fn_id->function_convertable = 0;
  } else if ((mchp_io_size_val == 1) && (CCS_STATE(state) != conv_possible)) {
    if (fn_id->encoded_name) free(fn_id->encoded_name);
    fn_id->encoded_name = 0;
    fn_id->function_convertable = 0;
  } else if ((mchp_io_size_val == 2) &&
             (CCS_STATE(state) == conv_not_possible)) {
    if (fn_id->encoded_name) free(fn_id->encoded_name);
    fn_id->encoded_name = 0;
    fn_id->function_convertable = 0;
  }
}

static mchp_conversion_status
mchp_convertable_output_format_string(const char *string) {
  const char *c = string;
  enum mchp_conversion_status_ status = 0;

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
        case '0': c++; continue;
        default: break;
      }
      break;
    }
    /* optional field width or * */
    if (*c == '*') c++; else
    while (ISDIGIT(*c)) c++;
    /* optional precision or * */
    if (*c == '.') {
      c++;
      /* an illegal conversion sequence %.g, for example - give up and
         start looking from the g onwards */
      if (*c == '*') c++;
      else {
        if (!ISDIGIT(*c)) {
          c--;
        }
        while(ISDIGIT(*c)) c++;
      }
    }
    /* optional conversion modifier */
    switch (*c) {
      case 'h':
      case 'l':
      case 'L': c++; break;
      default: break;
    }
    /* c should point to the conversion character */
    switch (*c) {
      case 'a': status |= conv_a; break;
      case 'A': status |= conv_A; break;
      case 'c': status |= conv_c; break;
      case 'd': status |= conv_d; break;
      case 'i': status |= conv_d; break;
      case 'e': status |= conv_e; break;
      case 'E': status |= conv_E; break;
      case 'f': status |= conv_f; break;
      case 'F': status |= conv_F; break;
      case 'g': status |= conv_g; break;
      case 'G': status |= conv_G; break;
      case 'n': status |= conv_n; break;
      case 'o': status |= conv_o; break;
      case 'p': status |= conv_p; break;
      case 's': status |= conv_s; break;
      case 'u': status |= conv_u; break;
      case 'x': status |= conv_x; break;
      case 'X': status |= conv_X; break;
      default:   /* we aren't checking for legal format strings */
                 break;
    }
  }
  return conv_possible | status;
}

static mchp_conversion_status
mchp_convertable_input_format_string(const char *string) {
  const char *c = string;
  enum mchp_conversion_status_ status = 0;

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
    switch (*c) {
      case 'h':
      case 'l':
      case 'L': c++; break;
      default: break;
    }
    /* c should point to the conversion character */
    switch (*c) {
      case 'a': status |= conv_a; break;
      case 'A': status |= conv_A; break;
      case 'c': status |= conv_c; break;
      case 'd': status |= conv_d; break;
      case 'i': status |= conv_d; break;
      case 'e': status |= conv_e; break;
      case 'E': status |= conv_E; break;
      case 'f': status |= conv_f; break;
      case 'F': status |= conv_F; break;
      case 'g': status |= conv_g; break;
      case 'G': status |= conv_G; break;
      case 'n': status |= conv_n; break;
      case 'o': status |= conv_o; break;
      case 'p': status |= conv_p; break;
      case 'u': status |= conv_u; break;
      case 'x': status |= conv_x; break;
      case 'X': status |= conv_X; break;
      /* string selection expr */
      case '[': {
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
  return conv_possible | status;
}

/*
 *   Check or set the conversion status for a particular rtl -
 *     to check the current state pass conv_state_unknown (always 0)
 *     This will create an entry if it doesn't exist or return the current
 *     state.
 */
static mchp_conversion_status
cache_conversion_state(rtx val, int variant, mchp_conversion_status s) {
  mchp_conversion_cache *parent = 0;
  mchp_conversion_cache *save;

  save = mchp_saved_conversion_info;
  while (save && save->rtl != val) {
    parent = save;
    if ((int)val & sizeof(void *)) save = save->l; else save = save->r;
  }
  if (save) {

    /* we can only increase the current status */
    if (CCS_STATE(s) > CCS_STATE(save->valid[variant])) {
      save->valid[variant] &= CCS_FLAG_MASK;
      save->valid[variant] |= (s & CCS_STATE_MASK);
    }
    save->valid[variant] = save->valid[variant] | CCS_FLAG(s);
    return save->valid[variant];
  }
  save = (mchp_conversion_cache *) xcalloc(sizeof(mchp_conversion_cache),1);
  save->rtl = val;
  save->valid[variant] = s;
  if (parent) {
    if ((int)val & sizeof(void *)) parent->l = save; else parent->r = save;
  } else mchp_saved_conversion_info = save;
  return s;
}

/* call-back to make sure all constant strings get seen */
void mchp_cache_conversion_state(rtx val, tree sym) {
  mchp_conversion_status s;

  s = cache_conversion_state(val, status_output, conv_state_unknown);
  if (s == conv_state_unknown) {
    if (sym && STRING_CST_CHECK(sym)) {
      const char *string = TREE_STRING_POINTER(sym);

      s = mchp_convertable_output_format_string(string);
      cache_conversion_state(val, status_output, s);
    }
  }
  s = cache_conversion_state(val, status_input, conv_state_unknown);
  if (s == conv_state_unknown) {
    if (sym && STRING_CST_CHECK(sym)) {
      const char *string = TREE_STRING_POINTER(sym);

      s = mchp_convertable_input_format_string(string);
      cache_conversion_state(val, status_input, s);
    }
  }
}

/* return the DECL for a constant string denoted by x, if found */
/* this function has disappeared from later sources :( */
static tree constant_string(rtx x) {
  if (GET_CODE(x) == SYMBOL_REF) {
    if (TREE_CONSTANT_POOL_ADDRESS_P (x)) return SYMBOL_REF_DECL (x);
  }
  return 0;
}

/* given an rtx representing a possible string, validate that the string is
   convertable */
static void mchp_handle_conversion(rtx val,
                                    mchp_interesting_fn *matching_fn) {
  tree sym;
  int style;

  if (val == 0) {
    conversion_info(conv_indeterminate, matching_fn);
    return;
  }
  /* a constant string will be given a symbol name, and so will a
     symbol ... */
  sym = constant_string(val);
  if (!(sym && STRING_CST_CHECK(sym))) sym = 0;
  mchp_cache_conversion_state(val, sym);
  style = matching_fn->conversion_style == info_I ? status_input:status_output;
  conversion_info(cache_conversion_state(val, style, conv_state_unknown),
                  matching_fn);
}

static void mchp_handle_io_conversion(rtx call_insn,
                                       mchp_interesting_fn *matching_fn) {
  /* the info_I/O function calls are all varargs functions, with the format
     string pushed onto the stack as the anchor to the variable argument
     portion.  In short, the interesting_arg portion is not used.
     The format string is the last thing pushed onto the stack. */
  rtx format_arg;

  gcc_assert((matching_fn->conversion_style == info_I) ||
         (matching_fn->conversion_style == info_O));

  for (format_arg = PREV_INSN(call_insn);
       !(NOTE_INSN_BASIC_BLOCK_P(format_arg) ||
         NOTE_INSN_FUNCTION_BEG_P(format_arg) ||
         (INSN_P(format_arg) && (GET_CODE(PATTERN(format_arg)) == CALL_INSN)));
       format_arg = PREV_INSN(format_arg)) {
    if (INSN_P(format_arg)) {
      if ((GET_CODE(PATTERN(format_arg)) == SET) &&
          (GET_CODE(XEXP(PATTERN(format_arg),0)) == REG)) {

        int arg_reg = (REGNO(XEXP(PATTERN(format_arg),0)));
        rtx val = XEXP(PATTERN(format_arg),1);

        rtx assignment = format_arg;
        if (arg_reg == matching_fn->interesting_arg) {
          if ((GET_CODE(val) == LO_SUM)) {
            val = XEXP(val,1);
          } else if ((GET_CODE(val) == REG) || (GET_CODE(val) == SUBREG)) {
              val = find_last_value(val, &assignment, 0, /* allow hw reg */ 1);
          } else if (GET_CODE(val) == MEM) {
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


#if 0
static void mchp_handle_dbl_conversion(mchp_interesting_fn *matching_fn) {
  /* the info_dbl function calls must be converted to the _d version if
     -fno-short-double is enabled
  */

  gcc_assert(matching_fn->conversion_style == info_dbl);
  /*
  if (flag_short_double == 1) conversion_info(conv_not_possible, matching_fn);
  */
}
#endif

/*
 *  This function always returns true
 */
int mchp_check_for_conversion(rtx call_insn) {
  const char *name;
  const char *real_name;
  rtx fn_name;
  mchp_interesting_fn *match;

  if (GET_CODE(call_insn) != CALL_INSN) abort();
  /* (call_insn (set () (call (name) (size)))) for call returning value, and
     (call_insn (call (name) (size)))          for void call */
  if (GET_CODE(PATTERN(call_insn)) == PARALLEL) {
    if (GET_CODE(XVECEXP(PATTERN(call_insn),0,0)) == SET)
       fn_name = XEXP(XEXP(XVECEXP(PATTERN(call_insn),0,0),1),0);
    else fn_name = XEXP(XVECEXP(PATTERN(call_insn),0,0),0);
  }
  else {
    if (GET_CODE(PATTERN(call_insn)) == SET)
       fn_name = XEXP(XEXP(PATTERN(call_insn),1),0);
    else fn_name = XEXP(PATTERN(call_insn),0);
  }
  if (mchp_clear_fn_list) {
    int i;
    for (i = 0; mchp_fn_list[i].name; i++) {
      if (mchp_fn_list[i].encoded_name) free(mchp_fn_list[i].encoded_name);
      mchp_fn_list[i].encoded_name = 0;
      mchp_fn_list[i].function_convertable=1;
    }
    mchp_clear_fn_list = 0;
  }
  switch (GET_CODE(fn_name)) {
    default: return 1;

    case MEM: if (GET_CODE(XEXP(fn_name,0)) == SYMBOL_REF) {
                name = XSTR(XEXP(fn_name,0),0);
              } else {
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
  while (match) {
    switch (match->conversion_style) {
      default: abort();  /* illegal conversion style */

      case info_I:    mchp_handle_io_conversion(call_insn, match); break;
      case info_O:    mchp_handle_io_conversion(call_insn, match); break;
      case info_dbl:
#if 0
                      mchp_handle_dbl_conversion(match);
#endif
                      break;

    }
    if (match[1].name &&
        (strcmp(match[1].name, real_name) == 0)) match++; else match = 0;
  }
  return 1;
}
#endif  /* defined(C32_SMARTIO_RULES) */

#endif /* TODO */

