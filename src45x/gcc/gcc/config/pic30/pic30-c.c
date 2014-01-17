/*
 *  C specific version of pic30.c.
 */


/* This file contains functions that cannot be part of pic30.c because they
 * refer to langauge specific features that would otherwise prevent the
 * compilation of the LTO chain
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "insn-config.h"
#include "regs.h"
#include "tree.h"
#include "c-tree.h"
#include "function.h"
#include "expr.h"
#include "recog.h"
#include "toplev.h"
#if !defined(HAVE_cc0)
#define HAVE_cc0
#endif
#include "conditions.h"
#include "real.h"
#include "hard-reg-set.h"
#include "insn-attr.h"
#include "output.h"
#include "flags.h"
#include "langhooks.h"
/* #include "ggc.h" */
#include "c-pragma.h"
/* #include "c-lex.h"  this stuff has been moved to c-praghma.h */
#include "cpplib.h"
#include "pic30-protos.h"
#include "target.h"
#include "target-def.h"
#include "insn-codes.h"
#include "basic-block.h"
#include "version.h"
/* splitting for lto/non-lto causes beacoup problems */
#include "../../../../../c30_resource/src/c30/resource_info.h"
#include "incpath.h"
#include "tm_p.h"
#include "cgraph.h"
#include "hashtab.h"
#include "tree-pass.h"
#ifdef LICENSE_MANAGER
#include "../../../../../pic30-lm/include/pic30-lm.h"
#endif
#include "config/mchp-cci/cci.h"

#ifndef C30_SMARTIO_RULES
/* make this the default */
#define C30_SMARTIO_RULES 2
#endif

#define JOIN2(X,Y) (X ## Y)
#define JOIN(X,Y) JOIN2(X,Y)


static int     pic30_parse_pragma_option(int pc);

void pic30_cpu_cpp_builtins(void *pfile_v) {
  char buffer[80];
  extern const char *pic30_it_option;
  extern const char *pic30_it_option_arg;
  extern int flag_lang_asm;
  struct cpp_reader *pfile = (struct cpp_reader *)pfile_v;

  sprintf(buffer,"__OPTIMIZATION_LEVEL__=%d", optimize);
  cpp_define(pfile,buffer);
  sprintf(buffer,"__LARGE_ARRAYS__=%d", TARGET_BIG != 0);
  cpp_define(pfile,buffer);
  if (pic30_target_family) cpp_define(pfile, pic30_target_family);
  if (pic30_target_cpu) cpp_define(pfile, pic30_target_cpu);
  sprintf(buffer,"__BUILTIN_ITTYPE");
  cpp_define(pfile,buffer);
  if (pic30_it_option) {
    sprintf(buffer,"__IT_TRANSPORT=%s",pic30_it_option);
    cpp_define(pfile,buffer);
    if (pic30_it_option_arg) {
      int i=1;
      char *s,*c;

      c = pic30_it_option_arg;
      do {
        s = c;
        for (; *c && *c != ','; c++);
        if (*c) *c++ = 0;
        sprintf(buffer,"__IT_TRANSPORT_OPTION%d=%s",i++,s);
        cpp_define(pfile,buffer);
      } while (*c);
    }
  }
  {
    sprintf(buffer,"__C30_VERSION__=%d", pic30_compiler_version);
    cpp_define(pfile,buffer);
    sprintf(buffer,"__XC16_VERSION=%d", pic30_compiler_version);
    cpp_define(pfile,buffer);
    sprintf(buffer,"__XC16_VERSION__=%d", pic30_compiler_version);
    cpp_define(pfile,buffer);
  }


  if ((cpp_get_options (pfile)->lang != CLK_ASM)) {
    cpp_define(pfile,"__XC16");
    cpp_define(pfile,"__C30");
    cpp_define(pfile,"__dsPIC30");
    cpp_define(pfile,"__C30__");
    cpp_define(pfile,"__XC16__");
    cpp_define(pfile,"__dsPIC30__");

#if PIC30_DWARF2
    cpp_define(pfile,"__XC16ELF");
    cpp_define(pfile,"__C30ELF");
    cpp_define(pfile,"__dsPIC30ELF");
    cpp_define(pfile,"__C30ELF__");
    cpp_define(pfile,"__XC16ELF__");
    cpp_define(pfile,"__dsPIC30ELF__");
    if (!flag_iso) {
      cpp_define(pfile,"C30");
      cpp_define(pfile,"XC16");
      cpp_define(pfile,"dsPIC30");
    }
#else
    cpp_define(pfile,"__XC16COFF");
    cpp_define(pfile,"__C30COFF");
    cpp_define(pfile,"__dsPIC30COFF");
    cpp_define(pfile,"__C30COFF__");
    cpp_define(pfile,"__XC16COFF__");
    cpp_define(pfile,"__dsPIC30COFF__");
    if (!flag_iso) {
      cpp_define(pfile,"C30");
      cpp_define(pfile,"XC16");
      cpp_define(pfile,"dsPIC30");
    }
#endif
  } else {
    cpp_define(pfile, "__ASM30");
    cpp_define(pfile, "__ASM30__");
    if (!flag_iso) {
      cpp_define(pfile, "ASM30");
    }
  }
  if (pic30_device_mask) {
    if (pic30_device_mask & HAS_DSP) cpp_define(pfile,"__HAS_DSP__");
    if (pic30_device_mask & HAS_EEDATA) cpp_define(pfile,"__HAS_EEDATA__");
    if (pic30_device_mask & HAS_DMA | HAS_DMAV2)
      cpp_define(pfile,"__HAS_DMA__");
    if (pic30_device_mask & HAS_AUXFLASH) cpp_define(pfile,"__HAS_AUXFLASH__");
    if (pic30_device_mask & HAS_DMAV2) cpp_define(pfile,"__HAS_DMAV2__");
    if (pic30_device_mask & HAS_CODEGUARD)
      cpp_define(pfile,"__HAS_CODEGUARD__");
    if (pic30_device_mask & HAS_PMP | HAS_PMPV2)
      cpp_define(pfile,"__HAS_PMP__");
    if (pic30_device_mask & HAS_PMPV2) cpp_define(pfile,"__HAS_PMPV2__");
    if (pic30_device_mask & HAS_EDS) cpp_define(pfile,"__HAS_EDS__");
    if ((pic30_device_mask & HAS_5VOLTS) || TARGET_ARCH(PIC30F))
      cpp_define(pfile,"__HAS_5VOLTS__");
  }

  mchp_init_cci(pfile_v);
}

/*
** Handle dsPIC30 specific pragmas for compatibility with existing
** compilers for the C17/C18.
**
** pragma                   attribute
** ----------------------------------------------------------
** interrupt function                      interrupt
** code section                    n/a
** idata section                n/a
** udata section                n/a
**
*/

/************************************************************************/
/*
** Parse the <#pragma <section> [section]> pragma.
*/
/************************************************************************/
static void pic30_handle_section_pragma(struct cpp_reader *pfile ATTRIBUTE_UNUSED,
                                        tree *pvalue) {
  int c;
  const char *pszScnName;
  tree x;

  c = pragma_lex(&x);
  if (c == CPP_EOF) *pvalue = NULL_TREE;
  else {
    /*
    ** User-specified <section-type> section name
    */
    for ( ; ; ) {
      if (pic30_parse_pragma_option(c) == 0) {
        error("Invalid or missing section name");
      }
      /*
       ** Check for data qualifiers; keep reading until we
       ** get an identifier... we read one extra token
       ** but consume it later
       */
      pszScnName = IDENTIFIER_POINTER(x);
      c = pragma_lex(&x);
      if ((strcmp("access", pszScnName) == 0) ||
          (strcmp("shared", pszScnName) == 0) ||
          (strcmp("overlay", pszScnName) == 0)) {
        warning(0,"data qualifier '%s' ignored", pszScnName);
        pszScnName = NULL;
      } else {
        break;
      }
    }
    *pvalue = build_string(strlen(pszScnName),pszScnName);
    /*
     ** Check for <location> specification
     */
    if (c == CPP_EQ) {
      warning(0,"absolute address specification ignored");
      do {
        c = pragma_lex(&x);
      } while (c != CPP_EOF);
    } else if (c != CPP_EOF) {
      if (pic30_parse_pragma_option(c) == 0) {
        error("Invalid location qualifier: '%s'", pszScnName);
        return;
      }
      pszScnName = IDENTIFIER_POINTER(x);
      /*
       ** Validate the location qualifier
       */
      if ((strcmp("gpr", pszScnName) == 0) || (strcmp("sfr", pszScnName) == 0)){
        warning(0,"location qualifier '%s' ignored", pszScnName);
        pszScnName = NULL;
      } else {
        error("Invalid location qualifier: '%s'", pszScnName);
      }
    }
  }
}

/************************************************************************/
/*
** Parse the <#pragma code [section]> pragma.
*/
/************************************************************************/
void pic30_handle_code_pragma(struct cpp_reader *pfile) {
    pic30_handle_section_pragma(pfile, &lTreeTextScnName);
}

/************************************************************************/
/*
** Parse the <#pragma idata [section]> pragma.
*/
/************************************************************************/
void pic30_handle_idata_pragma(struct cpp_reader *pfile) {
    pic30_handle_section_pragma(pfile, &lTreeIDataScnName);
}

/************************************************************************/
/*
** Parse the <#pragma udata [section]> pragma.
*/
/************************************************************************/
void pic30_handle_udata_pragma(struct cpp_reader *pfile) {
    pic30_handle_section_pragma(pfile, &lTreeUDataScnName);
}

/************************************************************************/
/*
** Parse a pragma option.
*/
/************************************************************************/
static int pic30_parse_pragma_option(int pc) {
    return (pc == CPP_NAME);
}

/************************************************************************/
/*
** Lookup a name in the current scope.
*/
/************************************************************************/
static tree pic30_lookup_vardecl(const char *pszName) {
    int i, len;
    tree decl;
    tree namelist;
    const char *pszDeclName;

    namelist = lang_hooks.decls.getdecls();
    len = list_length(namelist);
    for (i = 0, decl = namelist; i < len; ++i, decl = TREE_CHAIN(decl))
    {
        if (TREE_CODE(decl) == VAR_DECL)
        {
            pszDeclName = IDENTIFIER_POINTER(DECL_NAME(decl));
            if (strcmp(pszName, pszDeclName) == 0)
            {
                break;
            }
        }
    }
    return(decl);
}

/************************************************************************/
/*
** Parse the interrupt pragma:
**
** #pragma interrupt <functionname> [shadow] [save=<symbol-list>]
*/
/************************************************************************/
void pic30_handle_interrupt_pragma(struct cpp_reader *pfile ATTRIBUTE_UNUSED) {
    int c;
    tree treeOptName;
    tree treeFcnName;
    tree treeFcn;
    tree treeFcnCall = NULL_TREE;
    tree treeFcnType;
    tree treeFcnArgs = NULL_TREE;
    tree treeFcnSave = NULL_TREE;
    const char *pszFcnName;
    const char *pszOptName;
    tree x;

    if (!global_bindings_p())
    {
        error("interrupt pragma must have file scope");
        return;
    }
    c = pragma_lex(&x);
    /*
    ** Parse the interrupt function name
    */
    if (pic30_parse_pragma_option(c) == 0)
    {
          error("Invalid or missing function name from interrupt pragma");
          return;
        }
    pszFcnName = IDENTIFIER_POINTER(x);
    /*
    ** Parse the optional shadow/save parameter
    */
        c = pragma_lex(&x);
    while (c != CPP_EOF)
    {
        if (pic30_parse_pragma_option(c) == 0) {
                  error("Invalid option to interrupt pragma");
                  return;
            }
        pszOptName = IDENTIFIER_POINTER(x);
        if (strcmp(pszOptName, "shadow") == 0)
        {
            /*
            ** Add this function to the shadow function tree
            */
            treeFcnName = get_identifier(pszFcnName);
            treeFcn = build_tree_list(treeFcnName, NULL_TREE);
            lTreeShadow = chainon(lTreeShadow, treeFcn);
                        c = pragma_lex(&x);
        }
        else if (strcmp(pszOptName, "save") == 0)
        {
            c = pragma_lex(&x);
            if (c != CPP_EQ)
            {
                error("Missing '=' for 'save' in interrupt"
                                      " pragma");
            }
            c = pragma_lex(&x);
            /*
            ** Parse the symbol list
            */
            for ( ; ; )
            {
                /*
                ** Locate the symbol's declaration.
                */
                if (pic30_parse_pragma_option(c) == 0) {
                    error("Invalid save variable in "
                          "interrupt pragma");
                    return;
                }
                pszOptName = IDENTIFIER_POINTER(x);
                treeOptName = pic30_lookup_vardecl(pszOptName);
                if (treeOptName == NULL_TREE)
                {
                    error("symbol '%s' not defined",
                        pszOptName);
                    return;
                }
                /*
                ** Add this variable to the save(...) list
                */
                treeOptName = build_tree_list(NULL_TREE,
                                treeOptName);
                treeFcnArgs = chainon(treeFcnArgs, treeOptName);
                /*
                ** Check for continuation
                */
                c = pragma_lex(&x);
                if (c != CPP_COMMA)
                {
                    /*
                    ** End of list.
                    */
                    break;
                }
                /*
                ** Advance to the 1st token after the ','
                */
                c = pragma_lex(&x);
            }
            /*
            ** Create a pseduo-function call for
            ** the interrupt save(...) list.
            */
            if (treeFcnSave == NULL_TREE)
            {
                treeFcnSave = pic30_identSave[1];
                treeFcnType = build_pointer_type(
                      build_pointer_type(
                       build_pointer_type(void_type_node)));
                treeFcnType = build_function_type(treeFcnType,
                                  NULL_TREE);
                treeFcnSave = build_decl(UNKNOWN_LOCATION,
                            FUNCTION_DECL,
                            treeFcnSave,
                            treeFcnType);
            }
            treeFcnCall = build1(ADDR_EXPR,
                build_pointer_type(TREE_TYPE(treeFcnSave)),
                                treeFcnSave);
            treeFcnCall = build3(CALL_EXPR,
                    TREE_TYPE(TREE_TYPE(treeFcnSave)),
                    treeFcnCall, treeFcnArgs, NULL_TREE);

            treeFcnCall = build_tree_list(NULL_TREE, treeFcnCall);
        }
        else
        {
            error("Invalid option '%s' to interrupt pragma",
                  pszOptName);
                        c = pragma_lex(&x);
        }
    }
    /*
    ** Add this function to the interrupt function tree
    */
    treeFcnName = get_identifier(pszFcnName);
    treeFcn = build_tree_list(treeFcnName, treeFcnCall);
    lTreeInterrupt = chainon(lTreeInterrupt, treeFcn);
}

void pic30_handle_large_arrays_pragma(struct cpp_reader *pfile) {
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

void mchp_handle_section_pragma(struct cpp_reader *pfile) {
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
    warning(OPT_Wpragmas, "malformed section pragma; string literal expected");
    mchp_pragma_section = NULL_TREE;
    return;
  }
  name = TREE_STRING_POINTER(x);
  mchp_pragma_section = build_string(strlen(name), name);
  c = pragma_lex(&x);
  if (c == CPP_NUMBER) {
    // section alignment/
  }
}

void mchp_handle_printf_args_pragma(struct cpp_reader *pfile) {
  mchp_pragma_printf_args = 1;
}

void mchp_handle_scanf_args_pragma(struct cpp_reader *pfile) {
  mchp_pragma_scanf_args = 1;
}

void mchp_handle_keep_pragma(struct cpp_reader *pfile) {
  mchp_pragma_keep = 1;
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

    identifier = IDENTIFIER_POINTER(x);
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
            attrib = build_tree_list(pic30_identKeep[0], NULL_TREE);
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
  

void mchp_handle_optimize_pragma(struct cpp_reader *pfile) {
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

        keyword = IDENTIFIER_POINTER(x);
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


