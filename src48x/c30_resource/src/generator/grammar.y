%{
#include <stdlib.h>
#include <stdio.h>
#define IN_GRAMMAR
#include "grammar.h"
#define YYMAXDEPTH 20000

/*
 *  resource grammar, Copyright 2006 Microchip Technology Inc
 */
static int field_no;
static struct resource_data *d;
static struct record *current_record = 0, *last_record = 0;

int yywrap() {
  return -1;
}

void yyerror(char *e) {
  error(e);
}

%}

%union {
  unsigned int   ival;
  char *sval;
  unsigned char  cval;
  struct resource_version version;
  struct resource_data *data;
}

%token <ival> INT_LIT  
%token <version> VERSION_LIT
%token <sval> STR_LIT
%token <cval> CHAR_LIT
%token T_TOOL_CHAIN T_TOOL_VERSION T_RESOURCE_VERSION T_FIELD_COUNT
%token T_FIELD_SIZES T_RESOURCE_INFO_START LSHIFT RSHIFT

%type <ival> integer_op_3 integer_op_2 integer_op_1 integer_op
%type <data> data_item

%%

start:  resource_info_block T_RESOURCE_INFO_START resource_data;

resource_info_block: 
    tool_chain tool_version resource_version field_count field_sizes;

tool_chain: T_TOOL_CHAIN '=' STR_LIT {
              if (rib.tool_chain != 0) {
                error("tool_chain already set!");
              } else rib.tool_chain = $3;
            };

tool_version: T_TOOL_VERSION '=' VERSION_LIT {
                if (rib.version.major | rib.version.minor) {
                  error("tool_version already set!");
                } else { 
                  rib.version.major = $3.major;
                  rib.version.minor = $3.minor;
                }
              };

resource_version: T_RESOURCE_VERSION '=' CHAR_LIT {
                    if (rib.resource_version_increment) {
                       error("resource_version already set!");
                    } else {
                      rib.resource_version_increment = $3;
                    }
                  } 
                | T_RESOURCE_VERSION '=' INT_LIT {
                    if (rib.resource_version_increment) {
                       error("resource_version already set!");
                    } else {
                      rib.resource_version_increment = $3;
                    }
                  }; 

field_count: T_FIELD_COUNT '=' INT_LIT {
               if (rib.field_count) {
                 error("field_count already set!");
               } else {
                 rib.field_count = $3;
                 rib.field_sizes = calloc(rib.field_count, sizeof(int));
               }
             };


field_int_lit_list: INT_LIT {
                      if (rib.field_sizes == 0) {
                        error("Number of fields not yet specified!");
                      } else {
                        field_no++;
                        rib.field_sizes[rib.field_count - field_no] = $1;
                      }
                    }
                  | INT_LIT ',' field_int_lit_list {
                      if (rib.field_sizes == 0) {
                        error("Number of fields not yet specified!");
                      } else {
                        field_no++;
                        if (field_no > rib.field_count) {
                          error("Too many field sizes defined!");
                        } else {
                          rib.field_sizes[rib.field_count - field_no] = $1;
                        }
                      }
                    }
                  ;

field_sizes: T_FIELD_SIZES '=' '{' field_int_lit_list '}' {
               if (field_no < rib.field_count) {
                 error("Too few field sizes defined!");
               }
             };

integer_op_3: INT_LIT                        { $$ = $1; }
            | INT_LIT '*' INT_LIT            { $$ = $1 * $3; }
            | INT_LIT '/' INT_LIT            { $$ = $1 / $3; }
            | '(' integer_op ')'             { $$ = $2; }
            ;

integer_op_2: integer_op_2 '+' integer_op_3    { $$ = $1 + $3; }
            | integer_op_2 '-' integer_op_3    { $$ = $1 - $3; }
            | integer_op_3                     { $$ = $1; }
            ;

integer_op_1: integer_op_1 LSHIFT integer_op_2   { $$ = $1 << $3; }
            | integer_op_1 RSHIFT integer_op_2   { $$ = $1 >> $3; }
            | integer_op_2                       { $$ = $1; }
            ;

integer_op:   integer_op '&' integer_op_1    { $$ = $1 & $3; }
            | integer_op '^' integer_op_1    { $$ = $1 ^ $3; }
            | integer_op '|' integer_op_1    { $$ = $1 | $3; }
            | integer_op_1                   { $$ = $1; }
            ;

data_item: STR_LIT     {
             d = malloc(sizeof(struct resource_data));
             d->kind = rik_string;
             d->v.s = $1;
             $$ = d;
           }
         | CHAR_LIT    {
             d = malloc(sizeof(struct resource_data));
             d->kind = rik_char;
             d->v.c = $1;
             $$ = d;
           }
         | VERSION_LIT {
             d = malloc(sizeof(struct resource_data));
             d->kind = rik_version;
             d->v.version.major = $1.major;
             d->v.version.minor = $1.minor;
             $$ = d;
           }
         | integer_op  { 
             d = malloc(sizeof(struct resource_data));
             d->kind = rik_int;
             d->v.i = $1;
             $$ = d;
           }
         ;

an_item: data_item {
           if (current_record == 0) {
             field_no = 0;
             current_record = calloc(1, sizeof(struct record));
             if (current_record == 0) error("Not enough memory - data_item");
             if (records == 0) records = current_record;
             current_record->fields = calloc(rib.field_count, 
                                             sizeof(struct resource_data *));
             if (current_record->fields == 0) 
               error("Not enough memory - data_item (2)");
             if (last_record) last_record->next = current_record;
           }
           if (field_no >= rib.field_count) {
             error("Too many fields in resource data!");
           } else {
             if (resource_data_size($1) > rib.field_sizes[field_no]) {
               error("Data item is too large for field!");
             }
             current_record->fields[field_no++] = $1;
           }
         }
       ;

data_list:  an_item
         |  data_list ',' an_item
         ;
   
resource_data_item: '{' data_list '}' {
                       if (field_no < rib.field_count) {
                         warning("Too few fields in resource data!");
                         d = malloc(sizeof(struct resource_data));
                         if (d) {
                           d->kind = rik_null;
                           current_record->fields[field_no] = d;
                         } else error("Not enough memory - resource_data_item");
                       }
                       last_record = current_record;
                       current_record = 0;
                     }
                  ;

resource_data: resource_data_item
             | resource_data resource_data_item;

