#ifndef XCCOVTOOL_COMM_H_
#define XCCOVTOOL_COMM_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Helper 'class'/module to ease the communication with
 * the Code Coverage tool via XML files
 */

/*
 * A vector containing basic block indexes (used for represent an adjacency list in
 * the input CFGs as well as a coverage path in the output of the CodeCov tool)
 */
typedef struct XCCovBBVec_ {
  size_t     size;     /* number of BB indexes following in the 'bb' array */
  int        bb[1];    /* this uses the trailing array idiom */
} XCCovBBVec;

/*
 * A CFG (control flow graph) structure (adjacency lists) - input for the CodeCov tool
 */
typedef struct XCCovCFG_ {
  size_t     size;     /* the number of basic blocks in the CFG */
  XCCovBBVec *succ[1]; /* by convention, in each successor list, elem #0 is the index of */
                       /* the source BB index and the rest (#1,#2 etc/could be none) */
} XCCovCFG;            /* are its successors */

/*
 * A "coverage path" structure - output of the CodeCov tool
 */
typedef struct XCCovPath_ {
  size_t     index;    /* a value that uniquely identifies this path; indicates the bit */
                       /* that holds the path "covered" status in the cc_bits array */
  size_t     size;     /* number of BB indexes following in the 'bb' array */
  int        bb[1];    /* the indexes of the BBs forming this path; by convention, bb[0] */
                       /* is the BB to be instrumented and the rest (#1,#2 etc/could be none) */
} XCCovPath;           /* are the ones to be considered covered when bb[0] is covered */

/*
 * A "covered function" structure - holds a function's coverage paths
 */
typedef struct XCCovFunc_ {
  const char *name;    /* the name of the function - matches exactly a function name */
                       /* passed as input to XCCovXML_NewFunction() */
  size_t     size;     /* the number of "coverage paths" necessary for this function */
  XCCovPath  *path[1]; /* the coverage paths */
} XCCovFunc;

/*
 * If multiple functions are specified in the input XML file (N/A for GCC RTL CodeCov pass),
 * the CodeCov tool output will contain coverage paths for the same number of functions
 */
typedef struct XCCovFuncs_ {
  size_t      size;    /* number of functions (should always be 1 for the GCC RTL CodeCov pass) */
  XCCovFunc  *func[1]; /* the coverage information for each of the functions */
} XCCovFuncs;

/*
 * Inits the module (e.g. determines the input and output file names)
 *   src_base_name - base name of source file
 *   save_temps    - save input/output files flag
 */
void XCCovXML_Init (const char *src_filename, int save_temps);

/* Getters for filenames
 *   Input file:  <filename>.cfg.xml
 *   Output file: <filename>.xccov.xml
 */
const char *XCCovXML_InputFileName (void);
const char *XCCovXML_OutputFileName (void);

/*
 * Starts building a new input XML file for the CodeCov tool
 * (the input filename and associated output filename are determined here)
 */
void XCCovXML_NewInput (void);

/*
 * Adds a function (name and CFG) to the current input file
 *   name - function name/id (used to match the corresponding part in the output XML)
 *   cfg  - the function's CFG (as adjacency lists)
 */
void XCCovXML_NewFunction (const char *name, const XCCovCFG *cfg);

/*
 * Creates/writes the XML file (input for the CodeCov tool) with the gathered information
 */
void XCCovXML_EndInput (void);

/*
 * Called after the CodeCov tool was executed to parse its (XML file) output.
 *   return value - the address of a structure containing the "coverage paths" for
 *     the functions specified in the input XML file (the memory is managed by the
 *     XCCovXML module / the client shouldn't attempt to free it)
 */
const XCCovFuncs *XCCovXML_ParseOutput (void);

/*
 * Frees the module resources
 */
void XCCovXML_Fini (void);

#ifdef __cplusplus
}
#endif

#endif /* XCCOVTOOL_COMM_H_ */
