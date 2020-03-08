#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "target.h"
#include "bversion.h" /* for BUILDING_GCC_MAJOR, BUILDING_GCC_VERSION */

/* this header madness affects only GCC 5.x, it seems */
#if (BUILDING_GCC_MAJOR == 5)
#include "hash-set.h"
#include "vec.h"
#include "double-int.h"
#include "input.h"
#include "alias.h"
#include "symtab.h"
#include "inchash.h"
#include "ipa-ref.h"
#include "tree-core.h"
#include "tree-ssa-alias.h"
#include "is-a.h"
#include "predict.h"
#include "function.h"
#include "basic-block.h"
#include "tree.h"
#include "gimple-expr.h"
#include "fold-const.h"
#include "gimple.h"
#include "lto-streamer.h"
#include "dominance.h"
#else
#include "tree.h"
#include "function.h"
#include "basic-block.h"
#endif

#include "tree-pass.h"
#include "cgraph.h"
#include "toplev.h" /* error() prototype is here on older GCCs */
#include "diagnostic.h" /* for error() on newer versions */
#include "langhooks.h"
#include "cppdefault.h"
#include "rtl.h"
#include "emit-rtl.h"
#include "bitmap.h"
#if (BUILDING_GCC_VERSION >= 5000)
#include "cfg.h"
#endif
#include "df.h"
#include "tm_p.h"
#include "output.h"
#include "dwarf2asm.h"

#include "xc-covtool-comm.h"
#include "xc-coverage.h"

/* the name of the Code Coverage tool/external executable */
#if defined(_WIN32)
#define MCHP_XCCOV_FILENAME "xc-ccov.exe"
#else
#define MCHP_XCCOV_FILENAME "xc-ccov"
#endif

/* (base) name of the symbol for the array of bits used to record the code coverage */
#define CC_BITS_NAME "__cc_bits"

/* how many bits to use to hold the value (covered/not covered) of a coverage point */
#if !defined(TARGET_XCCOV_POINTSIZE)
#define TARGET_XCCOV_POINTSIZE 1
#elif (TARGET_XCCOV_POINTSIZE != 1)
#error Only TARGET_XCCOV_POINTSIZE==1 is supported in this version.
#endif

/* the number of bits of the elements of the 'cc_bits' array */
#if !defined(TARGET_XCCOV_UNITSIZE)
#define TARGET_XCCOV_UNITSIZE 8
#elif (TARGET_XCCOV_UNITSIZE != 8)
#error Only TARGET_XCCOV_UNITSIZE==8 is supported in this version.
#endif

/* the value used for a 'covered' coverage point */
#if !defined(TARGET_XCCOV_COVERED_VAL)
#define TARGET_XCCOV_COVERED_VAL 1
#elif (TARGET_XCCOV_COVERED_VAL != 1)
#error Only TARGET_XCCOV_COVERED_VAL==1 is supported in this version.
#endif

/* no TARGET_XCCOV_CC_BITS_OFS without TARGET_XCCOV_LD_CC_BITS */
#if defined(TARGET_XCCOV_CC_BITS_OFS) && !defined(TARGET_XCCOV_LD_CC_BITS)
#error TARGET_XCCOV_CC_BITS_OFS makes sense only if TARGET_XCCOV_LD_CC_BITS is defined.
#endif

/* older basic-block.h versions were a little more verbose */
#if !defined(ENTRY_BLOCK_PTR_FOR_FN)
#define ENTRY_BLOCK_PTR_FOR_FN ENTRY_BLOCK_PTR_FOR_FUNCTION
#define BASIC_BLOCK_FOR_FN     BASIC_BLOCK_FOR_FUNCTION
#endif


/* struct to hold label numbers for start & end of a basic block */
typedef struct BBLabels_ {
  size_t     start_lbl;      /* index of the label at the beginning of the BB */
  size_t     end_lbl;        /* index of the label at the end of the BB */
  const char *fn_name;       /* the function name and the basic block index */
  size_t     idx;            /* are kept for debugging purposes only */
} BBLabels;

/* struct to hold label numbers for all BBs in a coverage path */
typedef struct PathLabels_ {
  size_t     size;           /* the number of BBLabels following */
  BBLabels   bbLabels[1];    /* the BBLabels (the trailing array idiom) */
} PathLabels;

/* struct to hold label numbers for all cov paths of the compilation unit */
typedef struct AllLbls_ {
  size_t     size;           /* the number of PathLabels following */
  PathLabels *pathLabels[1]; /* their BB labels */
} AllLabels;


/* variables for holding the coverage bits to asm labels mapping */
static struct obstack fn_names_;
static struct obstack path_labels_;
static struct obstack all_labels_;

/* var for code coverage symbol name */
static char *cc_bits_name = NULL;

/* var holding the number of code coverage paths/bits for the RTL case */
/* (where we need to generate the cc_bits array manually along with the CC info) */
static unsigned num_rtl_cc_bits;

/* 1 if the persistent data for the codecov pass was initialized; 0, otherwise */
static int pdata_inited = 0;

/* 1 if XCLM reports a valid Code Coverage license; 0, otherwise */
static int licensed_mode = 0;

/* getter for code coverage symbol name */
const char *xccov_cc_bits_name (void)
{
  return cc_bits_name;
}

#if (BUILDING_GCC_VERSION < 4008) /* < GCC 4.8 (approx.) */

/* Returns the name of function declared by FNDECL.  */
static const char *
fndecl_name (tree fndecl)
{
  if (fndecl == NULL)
    return "(nofn)";
  return lang_hooks.decl_printable_name (fndecl, 2);
}

/* Returns the name of function FN.  */
static const char *
function_name (struct function *fn)
{
  tree fndecl = (fn == NULL) ? NULL : fn->decl;
  return fndecl_name (fndecl);
}

extern const char **save_argv; /* this should be *hacked* in toplev.c to not be static */
#define SAVE_ARGV_0 save_argv[0]

#else /* GCC 4.8+ */

#include "opts.h"
extern struct cl_decoded_option *save_decoded_options;
#define SAVE_ARGV_0 save_decoded_options[0].arg

#endif /* BUILDING_GCC_VERSION < 4008 */

#if (BUILDING_GCC_VERSION < 4009) /* < GCC 4.9? */

#define RTX_INSN rtx

#else

#define RTX_INSN rtx_insn *
#define cgraph_get_node cgraph_node::get
#define cgraph_node_name(node) function_name (DECL_STRUCT_FUNCTION (node->decl));

#endif /* BUILDING_GCC_VERSION < 4009 */

/* the basic block that contains the prologue code (usually this is BB #2 i.e.
 * the first real BB of the function / the next BB after the fake ENTRY_BLOCK) */
static basic_block prologue_bb = NULL;

/* self-explanatory - inits the 'prologue_bb' variable above */
static void
find_prologue_end_bb (void)
{
  if (epilogue_completed) {
    /* usually, this is found in the first real BB (i.e. BB #2)
       but on -O3, this is not always the case so we scan all the insns */
    RTX_INSN insn;
    for (insn = get_insns (); insn; insn = NEXT_INSN (insn))
      /* look only at the notes */
      if (NOTE_P (insn)) {
        /* if a BB note, note the current BB */
        if (NOTE_KIND (insn) == NOTE_INSN_BASIC_BLOCK)
          prologue_bb = NOTE_BASIC_BLOCK (insn);
        /* otherwise, check for the prologue end note (and stop when find it) */
        else if (NOTE_KIND (insn) == NOTE_INSN_PROLOGUE_END)
          break;
      }
    /* assert that we found the prologue end note
       TODO: is there just one such note, always? */
    gcc_assert (insn);
  } else {
    /* if this pass is run before the one generating the prologue/epilogue code... */
    prologue_bb = ENTRY_BLOCK_PTR_FOR_FN (cfun)->next_bb;
  }
}

/* where to insert the instrumentation instructions: generally at the beginning of */
/* each BB (past the possible notes); special case for the BB that contains the */
/* prologue code where we'll add the instrumentation insns just after that code */
static RTX_INSN
get_insertion_point (basic_block bb)
{
  RTX_INSN const end = NEXT_INSN (BB_END (bb));
  RTX_INSN insn = BB_HEAD (bb);

  /* if this is the BB that contains the prologue of the function... */
  if (epilogue_completed && bb == prologue_bb)
    {
      /* ...skip the prologue code */
      while ((insn != end) &&
            (!NOTE_P (insn) || NOTE_KIND (insn) != NOTE_INSN_PROLOGUE_END))
        insn = NEXT_INSN (insn);
      /* and it better be there or something's not kosher */
      gcc_assert (insn != end);
      /* return the next insn after the note, regardless of its type
        (we want to insert our code exactly past the prologue end) */
      insn = NEXT_INSN (insn);
    }
  else
    {
      /* look for the first real, non-debug instruction */
      while ((insn != end) &&
            (!INSN_P (insn) || DEBUG_INSN_P (insn)))
        insn = NEXT_INSN (insn);
    }

  /* NULL indicates that the insertion point is past BB_END (bb) */
  if (insn == end)
    insn = NULL;

  /* let the target adjust this position, if it wants */
#if defined(TARGET_XCCOV_ADJ_INS_POS)
  insn = TARGET_XCCOV_ADJ_INS_POS (bb,
                                   epilogue_completed && bb == prologue_bb,
                                   insn);
#endif
  return insn;
}

static inline void
emit_at_position (rtx pattern, basic_block bb, RTX_INSN where)
{
  /* I don't know exactly when the insns are marked as belonging to a BB
   * (most of them aren't marked) but if the reference instruction is
   * marked, that would transfer to the new instructions as well (and it
   * would take care of updating BB_END (bb), when needed) */
  if (where) {
    set_block_for_insn (where, bb);
    emit_insn_before (pattern, where);
  } else {
    set_block_for_insn (BB_END (bb), bb);
    emit_insn_after (pattern, BB_END (bb));
  }
}

/* returns an unique name for a cgraph node by appending its address */
/* NOTE: the returned string must be freed with XDELETEVEC */
static char *
cgraph_node_uname (struct cgraph_node *node)
{
  const char *func_name = cgraph_node_name (node);

  /* the length of the result; "%p" in (sn)printf is implementation defined, */
  /* so figure it out with a dry run on an empty buffer */
  const size_t len = strlen (func_name) + 1 + snprintf (NULL, 0, "%p", (void*)node);

  /* allocate the memory */
  char *uname = XNEWVEC (char, len + 1);

  /* format the name and check that the length is matching */
  const int nchars = snprintf (uname, len + 1, "%s_%p", func_name, (void *)node);
  gcc_assert (nchars == (int)len);
  return uname;
}

/* the inverse of cgraph_node_uname(); retrieves the cgraph node from its (u)name */
static struct cgraph_node *
cgraph_node_from_uname (const char *uname)
{
  /* ignore the name - just look for the address part */
  const char *pos = NULL;
  void *ptr = NULL;
  int nargs;

  pos = strrchr (uname, '_');
  gcc_assert (pos && pos[1]);

  /* use sscanf to get the pointer value */
  nargs = sscanf (pos + 1, "%p", &ptr);
  gcc_assert (nargs == 1 && ptr);

  return (struct cgraph_node *)ptr;
}

/* Returns 1 if "--save-temps" is detected in the COLLECT_GCC_OPTIONS env var */
static int
get_save_temps_flag (void)
{
  char *p = getenv ("COLLECT_GCC_OPTIONS");
  if (!p)
    return 0;
  return (strstr(p, "save-temps") != NULL);
}

/* Init (once) whatever data/resources are needed across pass instances */
static void
xccov_init_pdata (void)
{
  if (!pdata_inited) {
    const size_t zero = 0;
    XCCovXML_Init (aux_base_name, get_save_temps_flag ());
    obstack_init (&fn_names_);
    obstack_init (&path_labels_);
    obstack_init (&all_labels_);
    obstack_grow (&all_labels_, &zero, sizeof(zero)); /* the number of paths so far */
    licensed_mode = TARGET_XCCOV_LICENSED(); /* a valid ccov license is found? */
    pdata_inited = 1;
  }
}

/* Dispose of the static data/resources */
static void
xccov_fini_pdata (void)
{
  if (pdata_inited) {
    XCCovXML_Fini ();
    obstack_free (&fn_names_, NULL);
    obstack_free (&path_labels_, NULL);
    obstack_free (&all_labels_, NULL);
    XDELETEVEC (cc_bits_name);
    pdata_inited = 0;
  }
}

static void
xccov_write_tool_input_file (void)
{
  basic_block bb;
  edge e;
  edge_iterator ei;
  struct cgraph_node *node;
  char *node_uname;

  XCCovCFG *cfg;
  struct obstack cfg_obstack;
  struct obstack succ_obstack;
  size_t num_bbs = 0;

  obstack_init (&cfg_obstack);
  obstack_init (&succ_obstack);

  obstack_grow (&cfg_obstack, &num_bbs, sizeof(num_bbs));

  /* build the cfg map (include the fake entry and exit blocks) */
  FOR_ALL_BB_FN(bb, cfun)
  {
    size_t num_succs = 1;
    XCCovBBVec *succs;

    obstack_grow (&succ_obstack, &num_succs, sizeof(num_succs));
    obstack_int_grow (&succ_obstack, bb->index);

    FOR_EACH_EDGE(e, ei, bb->succs)
    {
      obstack_int_grow (&succ_obstack, e->dest->index);
      ++num_succs;
    }

    succs = (XCCovBBVec *)obstack_finish (&succ_obstack);
    succs->size = num_succs;

    obstack_ptr_grow (&cfg_obstack, succs);
    ++num_bbs;
  }

  cfg = (XCCovCFG *)obstack_finish (&cfg_obstack);
  cfg->size = num_bbs;

  /* a new input for the XCCovXML module */
  XCCovXML_NewInput ();

  /* append a function element */
  node = cgraph_get_node (cfun->decl);
  gcc_assert (node);

  node_uname = cgraph_node_uname (node);
  XCCovXML_NewFunction (node_uname, cfg);
  XDELETEVEC (node_uname);

  /* done (this will write the input XML file) */
  XCCovXML_EndInput ();

  obstack_free (&succ_obstack, NULL);
  obstack_free (&cfg_obstack, NULL);
}

/* Runs the Code Coverage tool and returns its exit code if it terminates */
static int
run_coverage_tool (const char *in_filename, const char *out_filename, bool use_sb)
{
  char *xccov_path = NULL;
  char *bindir = NULL;
  char **args = NULL;
  const char *failure = NULL;
  struct stat fs;
  int status = 0;
  int err = 0;
  int exit_code = -1;
  int num_args, i;
  int bindir_len, total_len, nchars;

  /* get bin directory path (the CodeCov tool is expected to reside here) */
  bindir = make_relative_prefix (SAVE_ARGV_0,
                                 TARGET_XCCOV_LIBEXEC_PATH,
                                 "/bin");

  /* build path to Coverage tool */
  bindir_len = (int) strlen (bindir);
  if (IS_DIR_SEPARATOR (bindir[bindir_len - 1]))
    bindir_len -= 1;
  total_len = bindir_len + strlen (MCHP_XCCOV_FILENAME) + 1;
  xccov_path = (char *) XNEWVEC (char, total_len + 1);
  nchars = snprintf (xccov_path, total_len + 1, "%.*s%c%s", bindir_len, bindir,
                     DIR_SEPARATOR, MCHP_XCCOV_FILENAME);
  gcc_assert (nchars == total_len);

  /* check Coverage exec exists */
  if (-1 == stat (xccov_path, &fs)) {
    error ("XC Coverage: could not find XC coverage tool at %s\n", xccov_path);
    abort();
  }

  /* build args array */
  num_args = 5 + use_sb;
  args = XCNEWVEC (char *, num_args);
  args[0] = xccov_path; /* NB: no copy here + it will be freed as the others */
  args[1] = xstrdup (in_filename);
  args[2] = xstrdup ("-o");
  args[3] = xstrdup (out_filename);
  if (use_sb)
    args[4] = xstrdup ("-sb");
  args[4 + use_sb] = NULL;

  /* launch Coverage tool */
  failure = pex_one (PEX_SEARCH,
      args[0],
      args,
      "XC Code Coverage tool",
      0,
      0,
      &status,
      &err);

  if (failure != NULL) {
    error ("Error running %s: %s\n", xccov_path, failure);
    exit_code = -1;
  }
  else if (WIFEXITED(status)) {
    exit_code = WEXITSTATUS(status);
  }

  for (i = 0; i < num_args; i++) {
    free (args[i]);
    args[i] = NULL;
  }
  free (args);
  args = NULL;

  return exit_code;
}

/* gets the coverage paths from the external CC tool;
 * supports multiple functions although there will always be a single one for the RTL pass */
static const XCCovFuncs *
xccov_get_coverage_paths (void)
{
  const char *in_filename = NULL;
  const char *out_filename = NULL;
  int exit_status;
  struct stat fs;

  /* run Code Coverage tool */
  in_filename = XCCovXML_InputFileName ();
  out_filename = XCCovXML_OutputFileName();

  /* allow for a 'sb' first parameter to -mcodecov= to ask the tool to use the SB algorithm */
  exit_status = run_coverage_tool (in_filename, out_filename,
                                   (mchp_codecov[0] == 's' && mchp_codecov[1] == 'b'
                                    && !ISALNUM (mchp_codecov[2])));
  if (exit_status) {
    error ("XC Coverage : error running Code Coverage tool, exit code %d!", exit_status);
    abort();
  }

  /* check output file was created by Code Coverage tool */
  gcc_assert(out_filename);
  if (-1 == stat(out_filename, &fs)) {
    error ("XC Coverage : error reading file!");
    abort();
  }

  /* get coverage paths from output file */
  return XCCovXML_ParseOutput ();
}

static void
xccov_init_cc_bits_name (void)
{
  /* annotate the name to make it unique across the various compilation units: */
  /* append the input file name and a hex hash value to CC_BITS_NAME */
  /* and replace any non-alphanumeric characters with underscores */
  if (!cc_bits_name) {
    const char *base_name;
    char       *abs_main_input_filename;
    hashval_t  hval;
    size_t     len, i;
    int        nchars;

    /* get a hash value for the absolute path of the input file */
    abs_main_input_filename = lrealpath (main_input_filename);
    gcc_assert (abs_main_input_filename);
    hval = htab_hash_string (abs_main_input_filename);
    free (abs_main_input_filename);

    /* the legible part is based on CC_BITS_NAME and the base name of the input file */
    base_name = strrchr (dump_base_name, '/');
    base_name = base_name ? base_name + 1 : dump_base_name;

    /* the total length of the identifier - except the ending NUL */
    len = strlen (CC_BITS_NAME) + strlen (base_name) + 2  /* two underscores */
                                      + sizeof(hval) * 2; /* hex string for the hash value */
    /* allocate the memory */
    cc_bits_name = XNEWVEC (char, len + 1);

    /* format the name and verify that we've done our calculations right */
    nchars = snprintf (cc_bits_name, len + 1, CC_BITS_NAME "_%s_%0*x",
                       base_name, (int)(sizeof(hval) * 2), hval);
    gcc_assert (nchars == (int)len);

    /* finally, sweep over all the characters and replace any non-alphanumeric ones with */
    /* underscores (normally, this should be done only on the 'base_name' part but...) */
    for (i = 0; i != len; ++i)
      if (!ISALNUM (cc_bits_name[i]))
        cc_bits_name[i] = '_';
  }
}

/* load the address of the "cc_bits" array (+ an offset, in bytes) somehow (if needed) */
/* NOTE: the offset is used only if the TARGET_XCCOV_CC_BITS_OFS macro is defined, */
/*       otherwise, it can be ignored (will always be zero in such a case) */
static void
xccov_load_cc_bits (unsigned offset ATTRIBUTE_UNUSED)
{
  /* this is optional (might not be defined/needed on some targets */
#if defined(TARGET_XCCOV_LD_CC_BITS)
  RTX_INSN where = get_insertion_point (prologue_bb);
  RTX_INSN insns;
  start_sequence ();
    TARGET_XCCOV_LD_CC_BITS (offset); /* target emit instructions */
    insns = get_insns ();
    gcc_assert (insns);
  end_sequence ();
  /* insert the insns (if any) at the beginning of the function */
  emit_at_position (insns, prologue_bb, where);
#endif
}

/* insert instrumentation code to mark the specified bit in the "cc_bits" array */
/* NOTE: cc_bit_no is relative to the offset passed to xccov_load_cc_bits_address () */
static void
xccov_add_instrumentation_point (basic_block bb,
                                 unsigned cc_bit_no ATTRIBUTE_UNUSED)
{
#if defined(TARGET_XCCOV_SET_CC_BIT)
  RTX_INSN where = get_insertion_point (bb);
  RTX_INSN insns;
  start_sequence ();
    TARGET_XCCOV_SET_CC_BIT (cc_bit_no); /* target emit instructions */
    insns = get_insns ();
    gcc_assert (insns);
  end_sequence ();
  /* insert the insn(s) at the beginning of the specified basic block */
  emit_at_position (insns, bb, where);
#else
  /* this one is not optional - it must be defined */
#error TARGET_XCCOV_SET_CC_BIT must be defined
#endif
}

static void
xccov_add_instrumentation_code (const XCCovFuncs *covfuncs)
{
  basic_block bb;
  const XCCovFunc *func;
  unsigned startPathNo;
  int num_bbs;
  size_t i;
  int *bb2PathIdx;
  unsigned cc_bits_ofs;

  /* in the RTL case, there should be paths for exactly one function here: the current func. */
  gcc_assert (covfuncs && covfuncs->size == 1);
  func = covfuncs->func[0];

  gcc_assert (cgraph_node_from_uname (func->name) == cgraph_get_node (cfun->decl));

#if defined(TARGET_XCCOV_BEGIN_INSTMT)
  TARGET_XCCOV_BEGIN_INSTMT ();
#endif

  /* iterate through the CC paths and assign indexes / bit numbers */
  startPathNo = num_rtl_cc_bits;

  for (i = 0; i != func->size; ++i)
    func->path[i]->index = num_rtl_cc_bits++;

  /* at least one path should exist for this function */
  gcc_assert (num_rtl_cc_bits != startPathNo);

  /* normally the BB indexes should be contiguous at this point but let's not take chances
     (determine 'num_bbs' as the maximum BB index + 1) */
  num_bbs = EXIT_BLOCK;
  FOR_EACH_BB_FN (bb, cfun)
    if (bb->index > num_bbs)
      num_bbs = bb->index;
  ++num_bbs;

  /* (temporary) map from BB indexes to path indexes/numbers (the "cc_bits" element
     to mark when the instrumented BB will be executed);
     bb2PathIdx[bb] == -1 indicates that 'bb' shouldn't be instrumented */
  bb2PathIdx = XNEWVEC (int, num_bbs);
  for (i = 0; i != (size_t)num_bbs; ++i)
    bb2PathIdx[i] = -1;

  for (i = 0; i != func->size; ++i) {
    const XCCovPath *const path = func->path[i];
    int bb_to_instrument;
    gcc_assert (path && path->size);

    /* the 1st element in the coverage path is the bb to be instrumented */
    bb_to_instrument = path->bb[0];
    gcc_assert (bb_to_instrument > EXIT_BLOCK && bb_to_instrument < num_bbs);
    bb2PathIdx[bb_to_instrument] = path->index;
  }

  /* where's the prologue code? */
  find_prologue_end_bb ();

  /* no cc_bits offset by default */
  cc_bits_ofs = 0;

  /* if TARGET_XCCOV_CC_BITS_OFS is defined */
#if defined(TARGET_XCCOV_CC_BITS_OFS)
  /* and it evaluates (it might be a C expression, not merely a constant) as non-zero */
  if (TARGET_XCCOV_CC_BITS_OFS) {
    /* each function will have a constant .word cc_bits + cc_bits_ofs that the instrum.
      code will use as address to the "cc_bits" array */
    cc_bits_ofs = startPathNo * TARGET_XCCOV_POINTSIZE / 8;
  }
#endif

  /* make sure the name of the cc_bits array is initialized as it may be needed in
   * the target implementation of xccov_add_instrumentation_point () */
  xccov_init_cc_bits_name ();

  /* for all the basic blocks of the function */
  FOR_EACH_BB_FN (bb, cfun) {
    /* look up the BB index in 'bitMap' */
    int path_idx = bb2PathIdx[bb->index];
    /* if found */
    if (path_idx >= 0) {
      /* insert the instrumentation insns */
      xccov_add_instrumentation_point (bb, path_idx - cc_bits_ofs * 8 / TARGET_XCCOV_POINTSIZE);
    }
  }

  XDELETEVEC (bb2PathIdx);

  /* possibly load the cc_bits addr into a register at the beginning of the function */
  xccov_load_cc_bits (cc_bits_ofs);

#if defined(TARGET_XCCOV_END_INSTMT)
  TARGET_XCCOV_END_INSTMT ();
#endif
}

static void
xccov_insert_bb_labels (void)
{
  basic_block bb;
  FOR_EACH_BB_FN (bb, cfun) {
    RTX_INSN head = BB_HEAD (bb);
    RTX_INSN end;
    if (!LABEL_P (head)) {
      /* if the bb doesn't have a start label generate and emit one */
      RTX_INSN code_lbl = emit_label_before (gen_label_rtx (), head);
      head = BB_HEAD (bb) = code_lbl;
      set_block_for_insn (code_lbl, bb);
    }
    ++LABEL_NUSES (head);
    /* the same for the end of the bb */
    end = BB_END (bb);
    if (!LABEL_P (end)) {
      RTX_INSN code_lbl = emit_label_after (gen_label_rtx (), end);
      end = BB_END (bb) = code_lbl;
      set_block_for_insn (code_lbl, bb);
    }
    ++LABEL_NUSES (end);
    /* the target may need to add a (fake or not) insn at the end of the BB */
#ifdef TARGET_XCCOV_ADD_EOB_INSN
    TARGET_XCCOV_ADD_EOB_INSN (bb);
#endif
  }
}

/* generate coverage bit to address ranges mapping */
/* and accumulate it in 'all_labels_' */
static void
xccov_acc_ancilliary_info (const XCCovFuncs *covfuncs)
{
  basic_block bb;
  const XCCovFunc *func;
  const char *fn_name;
  size_t i, j;

  gcc_assert (covfuncs && covfuncs->size && covfuncs->func[0]);

  func = covfuncs->func[0];

  /* add the function name to the corresponding obstack */
  fn_name = function_name (cfun);
  fn_name = (const char *)obstack_copy0 (&fn_names_, fn_name, strlen (fn_name));

  /* update the number of total paths up-front */
  ((AllLabels *)obstack_base (&all_labels_))->size += func->size;

  /* for each path */
  for (i = 0; i != func->size; ++i) {
    const XCCovPath *const path = func->path[i];
    size_t tmp;

    /* the number of BBs in this path */
    obstack_grow (&path_labels_, &path->size, sizeof(path->size));

    /* iterate path BBs */
    for (j = 0; j != path->size; ++j) {
      rtx head, end;
      /* get the basic_block from its index */
      bb = BASIC_BLOCK_FOR_FN (cfun, path->bb[j]);
      /* then the start and end labels */
      head = BB_HEAD (bb);
      gcc_assert (LABEL_P (head));
      end = BB_END (bb);
      /* if an extra insn could have been added */
#ifdef TARGET_XCCOV_ADD_EOB_INSN
      /* back off one position to get the label */
      if (!LABEL_P (end))
        end = PREV_INSN (end);
#endif
      gcc_assert (LABEL_P (end));
      /* add a corresponding BBLabels struct on the path_labels_ stack */
      tmp = CODE_LABEL_NUMBER (head);
      obstack_grow (&path_labels_, &tmp, sizeof(tmp)); /* start_lbl */
      tmp = CODE_LABEL_NUMBER (end);
      obstack_grow (&path_labels_, &tmp, sizeof(tmp)); /* end_lbl */
      obstack_ptr_grow (&path_labels_, fn_name);       /* fn_name */
      tmp = bb->index;
      obstack_grow (&path_labels_, &tmp, sizeof(tmp)); /* idx */
    }

    /* finish the current path and add its address to 'all_labels_' */
    obstack_ptr_grow (&all_labels_,
                      obstack_finish (&path_labels_));
  }
}

/* symbolic constants used in the codecov info/hdr sections' fields */
#define CCINFO_HDR_SIZE            16
#define CCINFO_HDR_VERSION         1

#define CCINFO_DATA_SPACE          0
#define CCINFO_PROGRAM_SPACE       1
#define CCINFO_RESTRICTED_SPACE   -1

/* Emits the "cc_bits" definition */
static void
xccov_define_cc_bits (void)
{
  /* emit .section statement */
  TARGET_XCCOV_EMIT_SECTION (CODECOV_SECTION);

  /* size in bytes (rounded up from the no. of bits) */
  const unsigned long long size = (num_rtl_cc_bits * TARGET_XCCOV_POINTSIZE + 7) / 8;
  ASM_OUTPUT_ALIGNED_LOCAL (asm_out_file, cc_bits_name, size, 8);
}

/* Code to write code coverage ancilliary information to asm.
   This file contains code that writes the code coverage ancilliary information
   to asm, into ".codecov_info", ".codecov_info.hdr" sections.
   e.g.:
    asm output (-dA gcc option)
      .section        .codecov_info.hdr,info
      .byte   0x10    @ size
      .4byte  0x1     @ version
      .byte   0x1     @ pointsize
      .byte   0x8     @ unitsize
      .byte   0x1     @ flags
      .4byte  0       @ reserved
      .4byte  0       @ reserved
      .section        .codecov_info,info
      .4byte  0       @ CC_ADDR_SPACE
      .4byte  __cc_bits_basic_c_e9218c68      @ CC_ADDR
      .4byte  0x1     @ total num points
      .4byte  0       @ OFFSET
      .4byte  0       @ RESERVED
      .4byte  0x1     @ RANGE_SPACE
      .4byte  0x2     @ num bbs covered
      .4byte  .L2     @ fn main bb 3 label start
      .4byte  .L6     @ fn main bb 3 label end
      .4byte  .L4     @ fn main bb 2 label start
      .4byte  .L5     @ fn main bb 2 label end
*/
static void
xccov_output_coverage_info (void)
{
  char buff[64];
  size_t p, i;

  /* finish the open object in 'all_labels_' */
  AllLabels *cc_labels = (AllLabels *)obstack_finish (&all_labels_);

  /* emit .section .codecov_info.hdr */
  TARGET_XCCOV_EMIT_SECTION (CODECOV_INFO_HDR);

  /* emit header */
  dw2_asm_output_data (1, CCINFO_HDR_SIZE, "size");
  dw2_asm_output_data (4, CCINFO_HDR_VERSION, "version");
  dw2_asm_output_data (1, TARGET_XCCOV_POINTSIZE, "pointsize");
  dw2_asm_output_data (1, TARGET_XCCOV_UNITSIZE, "unitsize");
  dw2_asm_output_data (1, (TARGET_XCCOV_COVERED_VAL & 1), "flags");
  dw2_asm_output_data (4, 0, "reserved");
  dw2_asm_output_data (4, 0, "reserved");

  /* emit .section .codecov_info */
  TARGET_XCCOV_EMIT_SECTION (CODECOV_INFO);

  /* output memory address space for the code coverage bits
      0 - default data memory (RAM by default on SAM/PIC32C targets) */
  dw2_asm_output_data (4, CCINFO_DATA_SPACE, "CC_ADDR_SPACE");

  /* output __cc_bits* address */
  dw2_asm_output_addr (4, cc_bits_name, "CC_ADDR");

  /* output total number of bits in this __cc_bits entry */
  dw2_asm_output_data (4, cc_labels->size, "total num points");

  /* output offset where coverage bits start from CC_ADDR */
  dw2_asm_output_data (4, 0, "OFFSET");

  /* reserved bytes */
  dw2_asm_output_data (4, 0, "RESERVED");

  for (p = 0; p != cc_labels->size; ++p) {
    const PathLabels *const lbls = cc_labels->pathLabels[p];

    /* output memory address space for code addresses
        1 - default program memory (FLASH by default on SAM/PIC32C targets)
       -1 - for restricted/unlicensed mode */
    dw2_asm_output_data (4, licensed_mode ? CCINFO_PROGRAM_SPACE
                                          : CCINFO_RESTRICTED_SPACE, "RANGE_SPACE");

    /* output number of basic blocks covered by the current bit */
    dw2_asm_output_data (4, lbls->size, "num bbs covered");

    for (i = 0; i != lbls->size; ++i) {
      const BBLabels *const bbLbls = &lbls->bbLabels[i];

      /* prepend ".L" to the start label number */
      snprintf (buff, sizeof(buff), ".L%u", (unsigned)bbLbls->start_lbl);

      /* output the label to asm */
      dw2_asm_output_addr (4, buff, "fn %s bb %u label start",
                            bbLbls->fn_name, (unsigned)bbLbls->idx);

      /* same for the end label */
      snprintf (buff, sizeof(buff), ".L%u", (unsigned)bbLbls->end_lbl);

      dw2_asm_output_addr (4, buff, "fn %s bb %u label end",
                            bbLbls->fn_name, (unsigned)bbLbls->idx);
    }
  }
}

/* called from TARGET_ASM_CODE_END so that the 'cc_bits' and the coverage
 * info section(s) are added to the assembly output */
void
xccov_code_end (void)
{
  /* any cc bits? */
  if (num_rtl_cc_bits) {
    xccov_define_cc_bits ();
    xccov_output_coverage_info ();
  }
  xccov_fini_pdata ();
}

/* entry point of the RTL code coverage pass */
static unsigned int
xc_code_coverage (void)
{
  const XCCovFuncs *covfuncs = NULL;

  /* make sure the static pass data (persistent over the pass instances) is initialized */
  xccov_init_pdata ();

  /* write input file for Coverage tool */
  xccov_write_tool_input_file ();

  /* get coverage paths from output file */
  covfuncs = xccov_get_coverage_paths ();

  /* add the instrumentation code */
  xccov_add_instrumentation_code (covfuncs);

  /* needed b/c of the newly inserted instructions (otherwise arm_reorg() will ICE) */
  df_finish_pass (true);
  df_scan_alloc (NULL);
  df_scan_blocks ();

  /* add labels at the start and end of each basic block */
  xccov_insert_bb_labels ();

  /* generate ancilliary information and accumulate it output from xccov_asm_code_end () */
  xccov_acc_ancilliary_info (covfuncs);
  return 0;
}

bool
xccov_enabled (void)
{
  /* if -mcodecov is specified and the function type doesn't have the 'nocodecov' attribute */
  return mchp_codecov != 0 &&
    lookup_attribute ("nocodecov",
                      TYPE_ATTRIBUTES (TREE_TYPE (current_function_decl))) == NULL_TREE;
}

#if (BUILDING_GCC_VERSION < 4009) /* the passes are C++ objects from GCC 4.9 ? */

struct rtl_opt_pass pass_xc_coverage =
{
 {
  RTL_PASS,                  /* type */
  "xc_coverage",             /* name */
#if (BUILDING_GCC_VERSION >= 4008) /* GCC 4.8 approx. */
  OPTGROUP_NONE,             /* optinfo_flags */
#endif
  xccov_enabled,             /* gate */
  xc_code_coverage,          /* execute */
  NULL,                      /* sub */
  NULL,                      /* next */
  0,                         /* static_pass_number */
  TV_NONE,                   /* tv_id */
  0,                         /* properties_required */
  0,                         /* properties_provided */
  0,                         /* properties_destroyed */
  0,                         /* todo_flags_start */
  0                          /* todo_flags_finish */
 }
};

#else /* GCC 4.9+ per my calculations :-| */

namespace {

const pass_data pass_data_xc_coverage =
{
  RTL_PASS,         /* type */
  "xc_coverage",    /* name */
  OPTGROUP_NONE,    /* optinfo_flags */
  TV_NONE,          /* tv_id */
  0,                /* properties_required */
  0,                /* properties_provided */
  0,                /* properties_destroyed */
  0,                /* todo_flags_start */
  0                 /* todo_flags_finish */
};

class pass_xc_coverage : public rtl_opt_pass {
public:
  pass_xc_coverage (gcc::context *ctxt): rtl_opt_pass (pass_data_xc_coverage, ctxt) {}
  virtual bool gate (function *) { return xccov_enabled (); }
  virtual unsigned int execute (function *) { return xc_code_coverage (); }
};

} /* anon namespace */

rtl_opt_pass *
make_pass_xc_coverage (gcc::context *ctxt)
{
  return new pass_xc_coverage (ctxt);
}

#endif
