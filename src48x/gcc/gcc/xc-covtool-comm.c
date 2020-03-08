#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "obstack.h"
#include "errors.h"
#include "bversion.h" /* for BUILDING_GCC_MAJOR */

/* GCC 5.x is a little crazy regarding header inclusion */
#if (BUILDING_GCC_MAJOR == 5)
#include "input.h"
#include "vec.h"
#include "predict.h"
#endif

#include "basic-block.h"
#include "expat.h"
#include "xc-covtool-comm.h"

static const char *XCCOV_XML_HEADER  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
static const char *XCCOV_XML_IN_EXT  = ".cfg.xml";
static const char *XCCOV_XML_OUT_EXT = ".xccov.xml";

#define INPUT_XML_ADD(str) obstack_grow (&input_xml_, (str), strlen((str)))
#define INPUT_XML_ADD_INT(x) do { \
  char buf[16]; \
  const int len = snprintf (buf, sizeof(buf), "%d", (x)); \
  obstack_grow (&input_xml_, buf, len); \
} while (0)

/* use obstacks to back up growing I/O data */
static struct obstack str_stack_;  /* storage for various strings (files and function names) */
static struct obstack input_xml_;  /* the content of the input XML is accumulated here */
static struct obstack covpaths_;   /* this stores the coverage paths (from the output XML) */
static struct obstack covfuncs_;   /* likewise, but for the covered functions */
static struct obstack vcovfuncs_;  /* the covered functions vector */

static char   *src_filename_ = NULL;
static size_t src_fname_len_ = 0;
static char   *in_filename_  = NULL;
static char   *out_filename_ = NULL;

/* incremented if multiple XMLs are generated per input source file */
static unsigned int seq_no_ = 0;

/* 1 if the temporary files should be kept, 0 if they should be removed */
static int save_temps_ = 0;

/* 1 if the XML parser is processing a <path> tag, 0 otherwise */
static int inside_path_ = 0;

/* 1 if the output obstacks (covpaths_, covfuncs_ and vcovfuncs_)
 * were initialized and need to be freed */
static int free_out_stacks = 0;

static void XCCovXML_InitFileNames (void)
{
  char buf[16];
  int  buf_len = 0;

  /* if not on the first call, free previous strings (both input and output filenames) */
  if (in_filename_) obstack_free (&str_stack_, in_filename_);

  if (save_temps_)
  {
    /* if "--save-temps" was passed then input/output filename for the
       Coverage tool will be <source_filename_base>[_SEQNO].cfg.xml (for input)
       and <source-filename_base>[_SEQNO].xccov.xml (for output) */
    if (seq_no_)
      buf_len = snprintf (buf, sizeof(buf), "_%u", seq_no_);

    obstack_grow (&str_stack_, src_filename_, src_fname_len_);
    if (buf_len)
      obstack_grow (&str_stack_, buf, buf_len);
    obstack_grow0 (&str_stack_, XCCOV_XML_IN_EXT, strlen (XCCOV_XML_IN_EXT));
    in_filename_ = (char *) obstack_finish (&str_stack_);

    obstack_grow (&str_stack_, src_filename_, src_fname_len_);
    if (buf_len)
      obstack_grow (&str_stack_, buf, buf_len);
    obstack_grow0 (&str_stack_, XCCOV_XML_OUT_EXT, strlen (XCCOV_XML_OUT_EXT));
    out_filename_ = (char *) obstack_finish (&str_stack_);
  }
  else
  {
    /* create a temporary file with extension ".cfg.xml" for Coverage input
       and ".xccov.xml" for Coverage output file */
    char *tmp_fname = make_temp_file (XCCOV_XML_IN_EXT);
    in_filename_ = (char *) obstack_copy0 (&str_stack_, tmp_fname, strlen (tmp_fname));
    free (tmp_fname);

    tmp_fname = make_temp_file (XCCOV_XML_OUT_EXT);
    out_filename_ = (char *) obstack_copy0 (&str_stack_, tmp_fname, strlen (tmp_fname));
    free (tmp_fname);
  }
}

void XCCovXML_Init (const char *src_filename, int save_temps)
{
  obstack_init (&str_stack_);
  obstack_init (&input_xml_);

  /* copy input params into static variables */
  gcc_assert (src_filename && *src_filename);
  src_fname_len_ = strlen (src_filename);
  src_filename_ = (char *) obstack_copy0 (&str_stack_, src_filename, src_fname_len_);
  save_temps_ = save_temps;

  XCCovXML_InitFileNames ();
}

void XCCovXML_NewInput (void)
{
  /* bump the sequence number and update the I/O filenames
     (only if "--save-temps" was specified) */
  if (save_temps_) {
    ++seq_no_;
    XCCovXML_InitFileNames ();
  }

  INPUT_XML_ADD (XCCOV_XML_HEADER);
  INPUT_XML_ADD ("<xccov>\n");
}

void XCCovXML_Fini (void)
{
  obstack_free (&str_stack_, NULL);
  obstack_free (&input_xml_, NULL);

  if (free_out_stacks) {
    obstack_free (&covpaths_,  NULL);
    obstack_free (&covfuncs_,  NULL);
    obstack_free (&vcovfuncs_, NULL);
    free_out_stacks = 0;
  }
}

const char *XCCovXML_InputFileName (void)
{
  return in_filename_;
}

const char *XCCovXML_OutputFileName (void)
{
  return out_filename_;
}

void XCCovXML_NewFunction (const char *name, const XCCovCFG *cfg)
{
  size_t i, j;
  int bb_idx;

  gcc_assert (cfg && cfg->size);

  INPUT_XML_ADD ("<fn name=\"");
  INPUT_XML_ADD (name);
  INPUT_XML_ADD ("\">\n");

  for (i = 0; i != cfg->size; ++i) {
    const XCCovBBVec *const succ = cfg->succ[i];
    gcc_assert (succ && succ->size);

    bb_idx = succ->bb[0];

    /* append bb element */
    INPUT_XML_ADD ("\t<bb");

    /* optionally the type attribute */
    if (bb_idx == ENTRY_BLOCK)
      INPUT_XML_ADD (" type=\"entry\" fake=\"true\">\n");
    else if (bb_idx == EXIT_BLOCK)
      INPUT_XML_ADD (" type=\"exit\" fake=\"true\">\n");
    else
      INPUT_XML_ADD (">\n");

    /* bb idx */
    INPUT_XML_ADD ("\t\t<idx>");
    INPUT_XML_ADD_INT (bb_idx);

    /* successors list in cfg */
    INPUT_XML_ADD ("</idx>\n\t\t<succs>");
    for (j = 1; j != succ->size; ++j) {
      INPUT_XML_ADD_INT (succ->bb[j]);
      if (j != succ->size - 1)
        INPUT_XML_ADD (" ");
    }
    INPUT_XML_ADD ("</succs>\n\t</bb>\n");
  }

  INPUT_XML_ADD ("</fn>\n");
}

void XCCovXML_EndInput (void)
{
  FILE   *f;
  void   *data;
  size_t size;
  int    rc;

  /* append ending tag */
  INPUT_XML_ADD ("</xccov>\n");

  /* size of the object (in bytes) */
  size = obstack_object_size (&input_xml_);

  /* the address */
  data = obstack_finish (&input_xml_);

  /* write file */
  gcc_assert (in_filename_);

  f = fopen (in_filename_, "w");
  if (!f) {
    error ("Error opening %s for writing: %s\n", in_filename_, xstrerror (errno));
    abort ();
  }

  if (size != fwrite (data, 1, size, f)) {
    error ("Error writing %u bytes to %s: %s\n", (unsigned)size, in_filename_, xstrerror (errno));
    abort ();
  }

  rc = fclose (f);
  if (rc) {
    error ("Error closing %s: %s\n", in_filename_, xstrerror (errno));
    abort ();
  }
}

static void XMLCALL
startElement (void           *userData ATTRIBUTE_UNUSED,
              const XML_Char *name,
              const XML_Char **atts)
{
  const size_t zero = 0;
  char *fn_name;

  if (!strcmp (name, "path")) {

    gcc_assert (!inside_path_);
    inside_path_ = 1;

    /* start a path (vector with BB indexes) by adding its initial */
    /* index and size (both 0) */
    obstack_grow (&covpaths_, &zero, sizeof(zero));
    obstack_grow (&covpaths_, &zero, sizeof(zero));
  }
  else if (!strcmp (name, "fn")) {
    gcc_assert (atts[0] && !strcmp (atts[0], "name") && atts[1] && *atts[1]);

    /* copy the function name to the strings obstack */
    fn_name = (char *) obstack_copy0 (&str_stack_, atts[1], strlen (atts[1]));

    /* and start a new "covered function" in the corresponding obstack */
    /* by adding the address of the string and the initial size/path count */
    obstack_ptr_grow (&covfuncs_, fn_name);
    obstack_grow (&covfuncs_, &zero, sizeof(zero));
  }
}

static void XMLCALL
endElement (void           *userData ATTRIBUTE_UNUSED,
            const XML_Char *name)
{
  XCCovPath *path;
  XCCovFunc *func;
  char      *str, *p, *end_p;
  long      bb;
  size_t    num_bbs;

  if (!strcmp(name, "path")) {
    gcc_assert (inside_path_);
    inside_path_ = 0;

    /* add a NUL terminator to the current string in the strings obstack */
    /* (the text contents of this <path> tag) */
    obstack_1grow (&str_stack_, '\0');
    str = p = (char *) obstack_finish (&str_stack_);

    /* parse all the integers in this string and add the to the current path */
    for (num_bbs = 0; ; p = end_p, ++num_bbs) {
      while (ISSPACE (*p)) ++p;
      if (!*p) break;
      bb = strtol (p, &end_p, 10);
      gcc_assert (bb >= 0 && errno != ERANGE);
      obstack_int_grow (&covpaths_, (int)bb);
    }

    /* we're done with the string; free it */
    obstack_free (&str_stack_, str);

    /* finish the current path */
    path = (XCCovPath *)obstack_finish (&covpaths_);
    path->size = num_bbs;

    /* add it to the current covered function */
    obstack_ptr_grow (&covfuncs_, path);
    ++((XCCovFunc *)obstack_base (&covfuncs_))->size;
  }
  else if (!strcmp(name, "fn")) {
    /* finish the current function */
    func = (XCCovFunc *)obstack_finish (&covfuncs_);

    /* add it to the vector of covered functions */
    obstack_ptr_grow (&vcovfuncs_, func);
    ++((XCCovFuncs *)obstack_base (&vcovfuncs_))->size;
  }
}

static void XMLCALL
textHandler (void           *userData ATTRIBUTE_UNUSED,
             const XML_Char *s,
             int            len)
{
  /* if the text pertains to a <path> tag */
  if (inside_path_) {
    /* accumulate it to the string obstack */
    obstack_grow (&str_stack_, s, len);
  }
}

const XCCovFuncs *XCCovXML_ParseOutput (void)
{
  const int BUFF_SIZE = 1024;
  const size_t zero = 0;
  FILE *f;
  int rc;

  XML_Parser parser = XML_ParserCreate (NULL);
  gcc_assert (parser);

  XML_SetElementHandler (parser, startElement, endElement);
  XML_SetCharacterDataHandler (parser, textHandler);

  gcc_assert (out_filename_);
  f = fopen (out_filename_, "r");
  if (!f) {
    error ("Error opening %s for reading: %s\n", out_filename_, xstrerror (errno));
    abort ();
  }

  /* setup the obstacks used (free the result of the previous call, if any) */
  if (free_out_stacks) {
    obstack_free (&covpaths_,  NULL);
    obstack_free (&covfuncs_,  NULL);
    obstack_free (&vcovfuncs_, NULL);
  }
  obstack_init (&covpaths_);
  obstack_init (&covfuncs_);
  obstack_init (&vcovfuncs_);
  free_out_stacks = 1;

  /* the initial function counter */
  obstack_grow (&vcovfuncs_, &zero, sizeof(zero));

  for (;;) {
    int bytes_read;
    void *buff = XML_GetBuffer (parser, BUFF_SIZE);
    if (!buff) {
      error ("The Expat library couldn't allocate a %d-bytes buffer\n", BUFF_SIZE);
      abort ();
    }

    bytes_read = fread (buff, 1, BUFF_SIZE, f);
    if (bytes_read != BUFF_SIZE && ferror (f)) {
      error ("Error reading from %s\n", out_filename_);
      abort ();
    }

    if (XML_STATUS_OK != XML_ParseBuffer (parser, bytes_read, bytes_read == 0)) {
      error ("Error %s parsing Coverage output file at line %lu\n",
             XML_ErrorString (XML_GetErrorCode (parser)),
             XML_GetCurrentLineNumber (parser));
      abort ();
    }

    if (!bytes_read)
      break;
  }

  rc = fclose (f);
  if (rc) {
    error ("Error closing %s: %s\n", in_filename_, xstrerror (errno));
    abort ();
  }

  if (!save_temps_) {
    remove (in_filename_);
    remove (out_filename_);
  }

  XML_ParserFree (parser);

  return (XCCovFuncs *)obstack_finish (&vcovfuncs_);
}
