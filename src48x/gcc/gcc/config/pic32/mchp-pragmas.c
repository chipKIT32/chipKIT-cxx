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
#undef  IN_GCC_FRONTEND
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
//#include "integrate.h"
#include "langhooks.h"
//#include "cfglayout.h"
#include "sched-int.h"
#include "gimple.h"
#include "bitmap.h"
//#include "diagnostic.h"

#ifndef _BUILD_MCHP_
#define _BUILD_MCHP_
#endif
#ifndef _BUILD_C32_
#define _BUILD_C32_
#endif

#include <stdio.h>
#include "cpplib.h"
#include "mchp-pragmas.h"
#include "mchp-protos.h"
#include "c-family/c-common.h"
#include "c-family/c-pragma.h"
#include "c/c-tree.h"
#include "config/mips/mips-machine-function.h"

#define MCHP_DONT_DEFINE_RESOURCES
#include "../../../../c30_resource/src/xc32/resource_info.h"
#undef MCHP_DONT_DEFINE_RESOURCES
 
#ifdef __MINGW32__
void *alloca(size_t);
#else
#include <alloca.h>
#endif

#include "config/mchp-cci/cci.c"  /* ack */

#ifndef CLEAR_REST_OF_INPUT_LINE
#define CLEAR_REST_OF_INPUT_LINE() do{int t;tree tv;do{t=pragma_lex(&tv);}while(t!=CPP_EOF);}while(0);
#endif

/* parse the vector pragma */
void
mchp_handle_vector_pragma (struct cpp_reader *pfile ATTRIBUTE_UNUSED)
{
  int tok;
  tree tok_value;
  const char *fname;

  /* syntax:
     vector-pragma: # pragma vector target-name irq-num [ , irq-num ]...
   */

  if (flag_generate_lto)
    {
      warning (0, "#pragma vector not supported with -flto option, this file will not participate in LTO");
      flag_generate_lto = 0;
    }

  /* Recognize the syntax. */
  /* First we have the name of the function which is an interrupt handler */
  tok = pragma_lex (&tok_value);
  if (tok != CPP_NAME)
    {
      error ("function name not found in vector #pragma");
      return;
    }
  fname = IDENTIFIER_POINTER (tok_value);
  gcc_assert (fname);

  /* The first address is mandatory since we have a vector clause */
  tok = pragma_lex (&tok_value);
  if (tok != CPP_NUMBER ||
      ((int)TREE_INT_CST_LOW (tok_value) < 0 ||
       (int)TREE_INT_CST_LOW (tok_value) > 255))
    {
      error ("IRQ number must be an integer between 0 and 255");
      return;
    }
  /* add the vector to the list of dispatch functions to emit */
  mchp_add_vector_dispatch_entry (fname, (int)TREE_INT_CST_LOW (tok_value),
                                  false, pic32_isa_unknown, 1);

  /* There are optionally more addresses, comma separated */
  tok = pragma_lex (&tok_value);
  while (tok == CPP_COMMA)
    {
      tok = pragma_lex (&tok_value);
      if (tok != CPP_NUMBER)
        {
          error ("address constant expected for vector function specifier");
          return;
        }
      /* add the vector to the list of dispatch functions to emit */
      mchp_add_vector_dispatch_entry (fname,
                                      (int)TREE_INT_CST_LOW (tok_value),
                                      false, pic32_isa_unknown, 1);

      tok = pragma_lex (&tok_value);
    }

  /* No further input is valid. We should have end of line here. */
  if (tok != CPP_EOF)
    {
      error ("extraneous data at end of line of #pragma vector");
    }
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
  p = (struct interrupt_pragma_spec *)ggc_alloc_atomic (sizeof (struct interrupt_pragma_spec));
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
  tok = pragma_lex (&tok_value);

  if (tok == CPP_ATSIGN)
    {
      char scn_name[12];
      /* '@' found */
      tok = pragma_lex (&tok_value);

      /* To place the handler function at the vector address, we
         put the function in the special section located at that
         address. */
      if (tok != CPP_NUMBER ||
          ((int)TREE_INT_CST_LOW (tok_value) < 0 ||
           (int)TREE_INT_CST_LOW (tok_value) > 255))
        {
          error ("Vector number must be an integer between 0 and 255");
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
      /* add the vector to the list of dispatch functions */
      mchp_add_vector_dispatch_entry (fname,
                                      (int)TREE_INT_CST_LOW (tok_value),
                                      false, pic32_isa_unknown,
                                      0);
    }
  else
    {
      /* it wasn't an '@' sign, so it's either a number or an error.
       */

      if (tok != CPP_NUMBER ||
          ((int)TREE_INT_CST_LOW (tok_value) < 0 ||
           (int)TREE_INT_CST_LOW (tok_value) > 255))
        {
          error ("Vector number must be an integer between 0 and 255");
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
                                      (int)TREE_INT_CST_LOW (tok_value),
                                      false, pic32_isa_unknown,
                                      1);
    }

  /* There are optionally more addresses, comma separated */
  tok = pragma_lex (&tok_value);
  while (tok == CPP_COMMA)
    {
      tok = pragma_lex (&tok_value);
      if (tok != CPP_NUMBER ||
          ((int)TREE_INT_CST_LOW (tok_value) < 0 ||
           (int)TREE_INT_CST_LOW (tok_value) > 255))
        {
          error ("Vector number must be an integer between 0 and 255");
          return;
        }
      /* add the vector to the list of dispatch functions to emit */
      mchp_add_vector_dispatch_entry (fname,
                                      (int)TREE_INT_CST_LOW (tok_value),
                                      false, pic32_isa_unknown,
                                      1);
      tok = pragma_lex (&tok_value);
    }

  /* No further input is valid. We should have end of line here. */
  if (tok != CPP_EOF)
    {
      error ("extraneous data at end of line of #pragma interrupt");
    }
}


/*
 * #pragma align <alignment>
 * #pragam align = <alignment>
 *
 * Not quite the same as #pragma pack, this aligns the start of the variable
 *   as #pragma pack only affeects structures
 *
 */

void mchp_handle_align_pragma(struct cpp_reader *pfile) {
  int c;
  tree x;

  c = pragma_lex(&x);
  if (c == CPP_EOF) {
    /* reset to default */
    mchp_pragma_align = 0;
    return;
  }
  if (c == CPP_EQ) c = pragma_lex(&x);
  if (c == CPP_NUMBER) {
    if (TREE_CODE(x) != INTEGER_CST) {
      warning(OPT_Wpragmas, "requested alignment is not a constant");
      mchp_pragma_align = 0;
      return;
    }
    mchp_pragma_align = TREE_INT_CST_LOW(x);
    if (mchp_pragma_align != 0) {
      if ((mchp_pragma_align & (mchp_pragma_align - 1)) != 0) {
        warning(OPT_Wpragmas, "requested alignment is not a power of 2");
        mchp_pragma_align = 0;
        return;
      }
    }
  }
}

void mchp_handle_section_pragma (struct cpp_reader *pfile) {
  int c;
  tree x;
  char *name;

  c = pragma_lex(&x);
  if (c == CPP_EOF) {
    /* reset to default */
    mchp_pragma_section = NULL_TREE;
    return;
  }
  if (c == CPP_EQ) c = pragma_lex(&x);
  if (c != CPP_STRING) {
    warning(OPT_Wpragmas, "malformed section pragma, string literal expected");
    mchp_pragma_section = NULL_TREE;
    return;
  }
  name = (char *)TREE_STRING_POINTER(x);
  mchp_pragma_section = build_string(strlen(name), name);
  c = pragma_lex(&x);
  if (c == CPP_NUMBER) {
    // section alignment/
  }
}

void mchp_handle_printf_args_pragma (struct cpp_reader *pfile) {
  mchp_pragma_printf_args = 1;
}

void mchp_handle_scanf_args_pragma (struct cpp_reader *pfile) {
  mchp_pragma_scanf_args = 1;
}

void mchp_handle_inline_pragma (struct cpp_reader *pfile) {
  int c;
  tree x;

  mchp_pragma_inline = 1;
  c = pragma_lex(&x);
  if (c == EOF) {
    return;
  } else if (c == CPP_EQ) {
    /* forced is not yet supported */
    warning(OPT_Wpragmas, "arguments to inline pragma are not supported.");
    return;
  }
}

void mchp_handle_keep_pragma (struct cpp_reader *pfile) {
  mchp_pragma_keep = 1;
}

void mchp_handle_coherent_pragma (struct cpp_reader *pfile) {
  mchp_pragma_coherent = 1;
}

void mchp_handle_required_pragma(struct cpp_reader *pfile) {
  int c;
  tree x;

  c = pragma_lex(&x);
  if (c == CPP_EOF) {
    /* behave just like #pragma keep */
    mchp_pragma_keep = 1;
    return;
  } else if (c == CPP_EQ) {
    c = pragma_lex(&x);
  }

  if (c == CPP_NAME) {
    char *identifier;
    tree sym;

    identifier = (char *)IDENTIFIER_POINTER(x);
    sym = maybe_get_identifier(identifier);
    if (sym) {
      sym = lookup_name(sym);
      if (sym) {
        switch (TREE_CODE(sym)) {
          default:  warning(OPT_Wpragmas,"malformed required pragma; "
                                         "unknown identifier '%s'", identifier);
                    return;
          case VAR_DECL:
          case FUNCTION_DECL: {
            tree attrib;
            attrib = build_tree_list(get_identifier("keep"), NULL_TREE);
            decl_attributes(&sym, attrib, 0);
            return;
          }
        }
      } else {
        warning(OPT_Wpragmas, "malformed required pragma; "
                              "identifier '%s' must be declared", identifier);
        return;
      }
    }
  }
  warning(OPT_Wpragmas, "malformed required pragma; '= symbol' expected ");
}

void mchp_handle_optimize_pragma (struct cpp_reader *pfile) {
  /* almost the same as the GCC optimize pragma, just more 'friendly' */
  int c;
  tree x;
  tree args = NULL_TREE;
  tree optimization_previous_node = optimization_current_node;

  c = pragma_lex(&x);
  if (c == CPP_EQ) c = pragma_lex(&x);  // options =

  do {
    switch (c) {
      case CPP_STRING:
        if (TREE_STRING_LENGTH(x) > 0)
          args = tree_cons(NULL_TREE,x,args);
        break;

      case CPP_NUMBER:
        args = tree_cons(NULL_TREE,x,args);
        break;

      case CPP_NAME: {
        char *keyword;
        char *equivalent=0;

        keyword = (char *)IDENTIFIER_POINTER(x);
        if (!strcmp(keyword,"balanced")) {
          equivalent = "-O2";
        } else if (!strcmp(keyword,"size")) {
          equivalent = "-Os";
        } else if (!strcmp(keyword,"speed")) {
          equivalent = "-O3";
        } else if (!strcmp(keyword,"none")) {
          equivalent = "-O0";
        } else if (!strcmp(keyword,"low")) {
          equivalent = "-O1";
        } else if (!strcmp(keyword,"medium")) {
          equivalent = "-O2";
        } else if (!strcmp(keyword,"high")) {
          equivalent = "-O3";
        } else {
          warning(OPT_Wpragmas, "ignoring optimize setting: %s", keyword);
        }

        args = tree_cons(NULL_TREE,
                         build_string(strlen(equivalent), equivalent), args);
        break;
      }

      default:  warning(OPT_Wpragmas, "malformed optimize pragma");
                return;
    }
    c = pragma_lex(&x);
  } while (c != CPP_EOF);

  /* put arguments in the order the user typed them.  */
  args = nreverse (args);

  parse_optimize_options (args, false);
  current_optimize_pragma = chainon (current_optimize_pragma, args);
  optimization_current_node = build_optimization_node ();
  c_cpp_builtins_optimize_pragma (parse_in,
                                      optimization_previous_node,
                                      optimization_current_node);
}


void mchp_handle_region_pragma (struct cpp_reader *pfile) {

  int c;
  tree region;
  const char *region_name;
  int region_origin;
  int region_size;
  enum cpp_ttype type;
  tree x ;

   if (flag_generate_lto)
   {
     warning (0, "#pragma region not supported with -flto option, this file will not participate in LTO" );
     flag_generate_lto = 0;
   }

   type = pragma_lex (&x);

   if ( (type == CPP_NAME) && (strncmp (IDENTIFIER_POINTER(x), "name", 4) == 0) ) { 
   }
   else
   {
    warning (OPT_Wpragmas, "malformed #pragma region, expected 'name'" );
    return;
   }
   
  type = pragma_lex(&x);
  
  if (type != CPP_EQ) 
  {
    warning (OPT_Wpragmas, "malformed #pragma region, expected '=' after 'name'");
    return;
  }
  
  type = pragma_lex(&x);

  if (type != CPP_STRING) 
  {
    warning (OPT_Wpragmas, "malformed #pragma region, string literal expected for region name");
    return;
  }
    
  region_name = TREE_STRING_POINTER(x);
  
  type = pragma_lex(&x);
 
   if ( (type == CPP_NAME) && (strncmp (IDENTIFIER_POINTER(x), "origin", 6) == 0) ) { 
   }
   else
   {
    warning (OPT_Wpragmas, "malformed #pragma region, expected 'origin'" );
    return;
   }
  
  type = pragma_lex(&x);
  
  if (type != CPP_EQ) 
  {
    warning (OPT_Wpragmas, "malformed #pragma region, expected '=' after 'origin'");
    return;
  }
  
  type = pragma_lex(&x);
  
  if ( (type != CPP_NUMBER) || (TREE_CODE(x) != INTEGER_CST) )
  {
    warning (OPT_Wpragmas, "malformed #pragma region, expected constant value for 'origin'");
    return;
  }
  
  region_origin = TREE_INT_CST_LOW(x);
  
  type = pragma_lex(&x);
  
   if ( (type == CPP_NAME) && (strncmp (IDENTIFIER_POINTER(x), "size", 4) == 0) ) { 
   }
   else
   {
    warning (OPT_Wpragmas, "malformed #pragma region, expected 'size'" );
    return;
   }
  
  type = pragma_lex(&x);
  
  if (type != CPP_EQ) 
  {
    warning (OPT_Wpragmas, "malformed #pragma region, expected '=' after 'size'");
    return;
  }
  
  type = pragma_lex(&x);
  
  if ( (type != CPP_NUMBER) || (TREE_CODE(x) != INTEGER_CST) )
  {
    warning (OPT_Wpragmas, "malformed #pragma region, expected constant value for 'size'");
    return;
  }
  
  region_size = TREE_INT_CST_LOW(x);

  type = pragma_lex(&x);

   if (type != CPP_EOF) {
    /* return */
    error ("extraneous data at end of line of #pragma region");
    CLEAR_REST_OF_INPUT_LINE();
    return;
  }
  
  pic32_update_external_memory_info(region_name, region_origin, region_size);
  return;
}
