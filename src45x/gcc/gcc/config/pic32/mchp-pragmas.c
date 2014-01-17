/* mchp-pragmas.c */
/* Subroutines used for MCHP PIC32 pragma handling.
   Copyright (C) 1989, 1990, 1991, 1993, 1994, 1995, 1996, 1997, 1998,
   1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009
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

/* TODO: Clean up these includes since refactoring out of mchp.c */
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
#include "mchp-pragmas.h"
#include "mchp-protos.h"
#include "c-common.h"
#include "c-pragma.h"
#include "c-tree.h"
#include "config/mips/mips-machine-function.h"
#ifdef __MINGW32__
void *alloca(size_t);
#else
#include <alloca.h>
#endif

#define CLEAR_REST_OF_INPUT_LINE() do{int t;tree tv;do{t=pragma_lex(&tv);}while(t!=CPP_EOF);}while(0);

#if 1 /* TODO */

/* handler function for the config pragma */
void
mchp_handle_config_pragma (struct cpp_reader *pfile)
{
  enum cpp_ttype tok;
  tree tok_value;
  static int shown_no_config_warning = 0;

  /* If we're compiling for the default device, we don't process
     configuration words */
  if (!mchp_processor_string)
    {
      error ("#pragma config directive not available for the default generic device, %s", mchp_processor_string);
      CLEAR_REST_OF_INPUT_LINE();
      return;
    }

  if (!mchp_config_data_dir)
    {
      error ("Configuration-word data directory not specified "
             "but required for #pragma config directive");
      CLEAR_REST_OF_INPUT_LINE();
      return;
    }

  /* the first time we see a config pragma, we need to load the
     configuration word data from the definition file. */
  if (!mchp_configuration_values)
    {
      /* alloc space for the filename: directory + '/' + "configuration.data"
       */
      char *fname = (char*)alloca (strlen (mchp_config_data_dir) + 1 +
                            strlen (MCHP_CONFIGURATION_DATA_FILENAME));
      strcpy (fname, mchp_config_data_dir);
      if (fname [strlen (fname) - 1] != '/'
          && fname [strlen (fname) - 1] != '\\')
        strcat (fname, "/");
      strcat (fname, MCHP_CONFIGURATION_DATA_FILENAME);

      if (mchp_load_configuration_definition (fname))
        {
          if (!shown_no_config_warning)
            {
              shown_no_config_warning = 1;
              warning (0, "configuration word information not available for "
                       "this processor. #pragma config is ignored.");
            }
          CLEAR_REST_OF_INPUT_LINE();
          return;
        }
    }

  /* The payload for the config pragma is a comma delimited list of
     "setting = value" pairs. Both the setting and the value must
     be valid C identifiers. */
  tok = pragma_lex (&tok_value);
  while (1)
    {
      const cpp_token *raw_token;
      const char *setting_name;
      const char *value_name;

      /* the current token should be the setting name */
      if (tok != CPP_NAME)
        {
          error ("configuration setting name expected in configuration pragma");
          break;
        }

      setting_name = IDENTIFIER_POINTER (tok_value);
      /* the next token should be the '=' */
      tok = pragma_lex (&tok_value);
      if (tok != CPP_EQ)
        {
          error ("'=' expected in configuration pragma");
          break;
        }
      /* now we have the value name. We don't use pragma_lex() to get this one
         since we don't want the additional interpretation going on there.
         i.e., converting integers from the string. */
      raw_token = cpp_get_token (pfile);

      if (raw_token->type == CPP_NAME)
        {
        /*
          value_name = IDENTIFIER_POINTER (
                         HT_IDENT_TO_GCC_IDENT (
                         HT_NODE (raw_token->val.node)));
           space?              */
          value_name = cpp_token_as_text (pfile, raw_token);

        }
      else if (raw_token->type == CPP_NUMBER)
        {
          value_name = (char*)raw_token->val.str.text;
        }
      else
        {
          error ("configuration value name expected in configuration pragma");
          break;
        }

      mchp_handle_configuration_setting (setting_name, value_name);

      /* if the next token is ',' then we have another setting. */
      tok = pragma_lex (&tok_value);
      if (tok == CPP_COMMA)
        tok = pragma_lex (&tok_value);
      /* if it's EOF, we're done */
      else if (tok == CPP_EOF)
        break;
      /* otherwise, we have spurious input */
      else
        {
          error ("',' or end of line expected in configuration pragma");
          break;
        }
    }
  /* if we ended for any reason other than end of line, we have an error.
     Any needed diagnostic should have already been issued, so just
     clear the rest of the data on the line. */
  if (tok != CPP_EOF)
    CLEAR_REST_OF_INPUT_LINE();
}


/* parse the interrupt pragma */
void
mchp_handle_interrupt_pragma (struct cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  struct interrupt_pragma_spec *p;
  const cpp_token *raw_tok;
  int tok;
  tree tok_value;
  const char *fname;
  tree ipl;

  /* Make sure our pragma is specified at file scope */
  if (!lang_hooks.decls.global_bindings_p ())
    {
      error ("interrupt pragma must have file scope");
      return;
    }
  /* syntax:
     interrupt-pragma: # pragma interrupt function-name ipl-specifier [ vector [[@]vect-num [ , vect-num ]... ] ]

     ipl-specifier: IPL0AUTO ... IPL7AUTO |
                    IPL0SOFT ... IPL7SOFT |
                    IPL0SRS  ... IPL7SRS  |
                    single                |
                    ripl
     */

  /* Recognize the syntax. */
  /* First we have the name of the function which is an interrupt handler */
  tok = pragma_lex (&tok_value);
  if (tok != CPP_NAME)
    {
      error ("function name not found in interrupt #pragma");
      return;
    }
  fname = IDENTIFIER_POINTER (tok_value);
  gcc_assert (fname);
  /* Second we have the IPL */
  tok = pragma_lex (&tok_value);
  if (tok != CPP_NAME
      || ((strcmp ("single", IDENTIFIER_POINTER (tok_value)) != 0)
          && (strcmp ("SINGLE", IDENTIFIER_POINTER (tok_value)) != 0)
          && (strcmp ("ripl", IDENTIFIER_POINTER (tok_value)) != 0)
          && (strcmp ("RIPL", IDENTIFIER_POINTER (tok_value)) != 0)
          && (((strncmp ("ipl", IDENTIFIER_POINTER (tok_value), 3) != 0)
               && (strncmp ("IPL", IDENTIFIER_POINTER (tok_value), 3) != 0))
              || (IDENTIFIER_POINTER (tok_value)[3] > '7')
              || (IDENTIFIER_POINTER (tok_value)[3] < '0'))))
    {
      error ("priority level specifier not found for interrupt #pragma %s",
             fname);
      return;
    }
  ipl = tok_value;

  /* add the interrupt designation to the list of interrupt pragmas */
  p = ggc_alloc (sizeof (struct interrupt_pragma_spec));
  p->next = interrupt_pragma_list_head;
  p->name = fname;
  p->ipl = ipl;
  p->vect_number = 0;
  interrupt_pragma_list_head = p;

  /* We can also have a comma delimited list of vector functions to add
     targetting this handler. Each of these vectors is functionally equivalent
     to a separate #pragma vector targetting this function. Allowing the
     specification here is syntactic convenience. */
  tok = pragma_lex (&tok_value);
  /* the specifier is optional, so check for end of line explicitly */
  if (tok == CPP_EOF)
    return;

  /* We have some non-EOF token, so check that it's "vector" */
  if (tok != CPP_NAME || strcmp ("vector", IDENTIFIER_POINTER (tok_value)))
    {
      error ("'vector' expected in #pragma interrupt specification");
      return;
    }
  /* The first address is mandatory since we have a vector clause */
  /* We need to recognize the '@' sign here, but the pragma_lex function
     will issue an error if it sees it ('@' is only a valid token in
     Objective-C. So we need to use the lower-level functions to get
     this token in order to be able to see the '@' sign if it's there. */
  do
    raw_tok = cpp_get_token (parse_in);
  while (raw_tok->type == CPP_PADDING);
  if (raw_tok->type == CPP_ATSIGN)
    {
      /* '@' found */
      tok = pragma_lex (&tok_value);

      /* To place the handler function at the vector address, we
         put the function in the special section located at that
         address. */
      if (tok != CPP_NUMBER ||
          ((int)TREE_INT_CST_LOW (tok_value) < 0 ||
           (int)TREE_INT_CST_LOW (tok_value) > 63))
        {
          error ("Vector number must be an integer between 0 and 63");
          return;
        }
      /* if this is a "single" handler, the only valid vector is zero */
      if (((strcmp ("single", IDENTIFIER_POINTER (ipl)) == 0)
           || (strcmp ("SINGLE", IDENTIFIER_POINTER (ipl)) == 0))
          && ((int)TREE_INT_CST_LOW (tok_value) != 0))
        {
          error ("'single' interrupt handlers work with only vector 0");
          return;
        }

      p->vect_number = (unsigned)TREE_INT_CST_LOW (tok_value);
    }
  else
    {
      /* it wasn't an '@' sign, so it's either a number or an error.
         handle that by backing up a token and re-lexing it with the
         normal pragma_lex() function. */
      _cpp_backup_tokens (parse_in, 1);
      tok = pragma_lex (&tok_value);

      if (tok != CPP_NUMBER ||
          ((int)TREE_INT_CST_LOW (tok_value) < 0 ||
           (int)TREE_INT_CST_LOW (tok_value) > 63))
        {
          error ("Vector number must be an integer between 0 and 63");
          return;
        }
      /* if this is a "single" handler, the only valid vector is zero */
      if (((strcmp ("single", IDENTIFIER_POINTER (ipl)) == 0)
           || (strcmp ("SINGLE", IDENTIFIER_POINTER (ipl)) == 0))
          && (int)TREE_INT_CST_LOW (tok_value) != 0)
        {
          error ("'single' interrupt handlers require vector 0");
          return;
        }
      /* add the vector to the list of dispatch functions to emit */
      mchp_add_vector_dispatch_entry (fname,
                                      (int)TREE_INT_CST_LOW (tok_value));
    }

  /* There are optionally more addresses, comma separated */
  tok = pragma_lex (&tok_value);
  while (tok == CPP_COMMA)
    {
      tok = pragma_lex (&tok_value);
      if (tok != CPP_NUMBER ||
          ((int)TREE_INT_CST_LOW (tok_value) < 0 ||
           (int)TREE_INT_CST_LOW (tok_value) > 63))
        {
          error ("Vector number must be an integer between 0 and 63");
          return;
        }
      /* add the vector to the list of dispatch functions to emit */
      mchp_add_vector_dispatch_entry (fname,
                                      (int)TREE_INT_CST_LOW (tok_value));
      tok = pragma_lex (&tok_value);
    }

  /* No further input is valid. We should have end of line here. */
  if (tok != CPP_EOF)
    {
      error ("extraneous data at end of line of #pragma interrupt");
    }
}
#endif /* TODO */

