/*
 *  gramamar header file
 */
#include "resource_types.h"
#ifndef IN_GRAMMAR
#include "grammar.tab.h"
#endif

#ifndef _GRAMMAR_H
#define _GRAMMAR_H

#define error(X) \
   fprintf(stderr,"[%d] %s:%d error %s\n", ++errcnt, filename, lineno, X);

#define warning(X) \
   if (display_warnings) fprintf(stderr,"%s:%d warning %s\n", filename, lineno, X);

#define malloc(X) (total_alloc += X, malloc(X))

#define calloc(X,Y) (total_alloc += (X*Y), calloc(X,Y))

struct record {
  struct resource_data **fields;
  struct record *next;
};

extern int lineno;
extern char *filename;
extern int errcnt;
extern int display_warnings;
extern unsigned int total_alloc;

struct resource_introduction_block rib;

struct record *records;

#endif
