/*
 *   Resource generator, Copyright 2006 Microchip Technology Inc
 */
static const char *version = "0.1 (alpha)";

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grammar.h"
#ifdef PIC32
#include "../xc32/xc32_flag_definitions.h"
#else
#include "../c30/c30_flag_definitions.h"
#endif

extern FILE *yyin;
extern int yydebug;

int lineno=1;
int errcnt;
char *filename;
int display_warnings;
int split_res = 0;
unsigned int total_alloc = 0;
int pic30_family_mask = FAMILY_MASK;

void write_data_item(struct resource_data *d, int width, FILE *f) {
  unsigned char buffer[sizeof(int)];
  char *s;
  char zero = 0;
  int len;

  switch (d->kind) {
    case rik_null:    while (width >= 1) {
                        fwrite(&zero, 1, 1, f);
                        width--;
                      }
                      break;

    case rik_char:    fwrite(&d->v.c, sizeof(char), 1, f);
                      if (width) {
                        width--;
                        while (width >= 1) {
                          fwrite(&zero, 1, 1, f);
                          width--;
                        }
                      }
                      break;

    case rik_int:     buffer[0] = (d->v.i & 0xFF);
                      buffer[1] = ((d->v.i >> 8) & 0xFF);
                      buffer[2] = ((d->v.i >> 16) & 0xFF);
                      buffer[3] = ((d->v.i >> 24) & 0xFF);
                      fwrite(buffer, 4, 1, f);
                      if (width) {
                        width -= 4;
                        while (width >= 1) {
                          fwrite(&zero, 1, 1, f);
                          width--;
                        }
                      }
                      break;

    case rik_version: buffer[0] = (d->v.version.major & 0xFF);
                      buffer[1] = ((d->v.version.major >> 8) & 0xFF);
                      buffer[2] = (d->v.version.minor & 0xFF);
                      buffer[3] = ((d->v.version.minor >> 8) & 0xFF);
                      fwrite(buffer, 4, 1, f);
                      if (width) {
                        width -= 4;
                        while (width > 0) {
                          fwrite(&zero, 1, 1, f);
                          width--;
                        }
                      }
                      break;

    case rik_string: 
 s = strdup(d->v.s);
                      resource_pack_string(s);
                      len = strlen(s)+1;
                      fwrite(s, len, 1, f);
                      if (width) {
                        width -= len;
                        while (width > 0) {
                          fwrite(&zero, 1, 1, f);
                          width--;
                        }
                      }
                      free(s);
                      break;
  }
}

int indent_fprintf(FILE *stream, char *format, ...) {
  static int indent_level=0;
  static char buffer[] = "%12345s";
  int indent_to = 0;
  va_list ap;
  char *c = format;
  char last_c;
  int result = 0;

  va_start(ap,format);
  last_c = 0;
  for (c = format; *c; c++) {
    switch (*c) {
      case '<': indent_to += 2;
                break;
      case '/': if (last_c == '<') {
                  indent_to = 0; 
                  indent_level -= 2;
                }
                else if (c[1] == '>') indent_to -= 2;
                break;
      default:  break;
    }
    last_c  = *c;
  }
  sprintf(buffer,"%%%ds", indent_level);
  fprintf(stream, buffer, "");
  result = vfprintf(stream, format, ap);
  va_end(ap);
  indent_level += indent_to;
  if (indent_level < 0) indent_level = 0;
  if (indent_level > 40) indent_level = 40;
  return result;
}

typedef struct tag_device_dict {
	char name[20];
  unsigned int  value;
  unsigned char family;
  struct tag_device_dict * next;
} device_dict;

int main(int argc, char **argv){

  int counter = 0;
  int input_arg;
  char *cmd_buffer;
  char *buffer;
  int buffer_size = 256;
  char tempfile[256];
  char *c;
  FILE *f;
  FILE *resource;
  FILE *xml;
  FILE *device_file;
  FILE *deviceSupportXML;
  struct resource_data d;
  struct record *r;
  int i,j;
  char *space=" ";
  int pic30_family_mask = FAMILY_MASK;

  fprintf(stderr,"Resource generation tool (Version %s)\n\n", version);

  buffer = calloc(1, buffer_size);
  i = 0;
  for (input_arg = 1; input_arg < argc-1; input_arg++) {
    if (*argv[input_arg] == '+') {
       switch (argv[input_arg][1]) {
         case 'd': yydebug=1;
                   break;
         case 'f': pic30_family_mask = strtol(argv[++input_arg],0,0);
                   break;
         case 'w': display_warnings=1;
                   break;
         case 's': split_res=1;
                   break;
         default:  fprintf(stderr,"Ignoring rg option %s\n", argv[input_arg]);
                   break;
       }
    } else if (*argv[input_arg] == '-') {
       /* option to the pre-processor */
       while (i + strlen(argv[input_arg]) + 1 >= buffer_size) {
         char *new_buffer;

         buffer_size += 256;
         new_buffer = realloc(buffer, buffer_size);
         if (new_buffer != buffer) free(buffer);
         buffer = new_buffer;
       }
       strcat(buffer, argv[input_arg]);
       strcat(buffer, space);
       i += strlen(argv[input_arg]) + 1;
    } else break;
  }

  sprintf(tempfile,argv[input_arg]);
  for (c = tempfile; (*c) && (*c != '.'); c++);
  if (*c == '.') {
    c[1] = 'i';
    c[2] = 0;
  } else {
    strcat(tempfile,".i");
  }

  fprintf(stderr,"All error lines refer to file: %s\n", tempfile);
  cmd_buffer = malloc(strlen(buffer) + 
                      strlen(argv[input_arg]) + 
                      strlen(tempfile) + 20);
  sprintf(cmd_buffer,"gcc %s -x c -E %s -o %s", buffer, argv[input_arg], 
                                                tempfile);
  fprintf(stderr,"Executing... %s\n", cmd_buffer);

  f = popen(cmd_buffer,"r");
  pclose(f);

  yyin = fopen(tempfile, "r");

  if (yyin == 0) {
     fprintf(stderr,"\nExiting due to pre-processing errors\n");
     exit(1);
  }

  filename = tempfile;
  yyparse();

  if (errcnt) {
    fprintf(stderr,"\nTotal allocation: %d\n",total_alloc);
    fprintf(stderr,"\nExiting due to errors\n");
    exit(1);
  }

  if (split_res==1) {
    int device_counter = 0;
#ifndef PIC32
    device_dict * first, * current;
    first = (device_dict *)malloc( sizeof(device_dict) );
    current = first;
#endif
    for (r = records; r; r = r->next) {
      /* Check if the record is a device */
#ifdef PIC32
      if ( strncmp(r->fields[0]->v.s,"32",2) == 0) {
#else
      if ( (strncmp(r->fields[0]->v.s,"24",2) == 0) ||
           (strncmp(r->fields[0]->v.s,"33",2) == 0) ||
           (strncmp(r->fields[0]->v.s,"30",2) == 0)){
#endif
        struct resource_data *last = 0;
        /*Create the device file*/
        char name[40];
        strcpy(name,r->fields[0]->v.s);
        strcat(name,".info");
        device_file = fopen(name,"w");
  
        if (!device_file) {
          fprintf(stderr,"Could not open file %s\n", argv[input_arg+1]);
          exit(1);
        }
  
        d.kind = rik_string;
        d.v.s = rib.tool_chain;
        write_data_item(&d, 0, device_file);

        d.kind = rik_version;
        d.v.version.major = rib.version.major;
        d.v.version.minor = rib.version.minor;
        write_data_item(&d, 0, device_file);

        d.kind = rik_char;
        d.v.c = rib.resource_version_increment;
        write_data_item(&d, 0, device_file);

        d.kind = rik_int;
        d.v.i = rib.field_count;
        write_data_item(&d, 0, device_file);
  
        for (i = 0; i < rib.field_count; i++) {
          d.v.i = rib.field_sizes[i];
          write_data_item(&d, 0, device_file);
        }
  
        d.v.i = 0xFFFFFFFF;
        write_data_item(&d,0,device_file);
#ifndef PIC32  
       current->value = r->fields[2]->v.i;
        strcpy(current->name, r->fields[0]->v.s);

#if 0
        fprintf(stderr,"%s %i\n",r->fields[0]->v.s, device_counter);
        fprintf(stderr,"%x %x\n",(unsigned char)r->fields[1]->v.i, 
                  r->fields[1]->v.i);
#endif
        current->family = (unsigned char)r->fields[1]->v.i;
  
        device_counter += 1; 
        current->next = (device_dict *) malloc( sizeof( device_dict ));
        current = current->next;
#endif        
        for (i = 0; i < rib.field_count; i++) {
          if (!last) {
            write_data_item(r->fields[i], rib.field_sizes[i], device_file);
            if (r->fields[i]->kind == rik_null) last = r->fields[i];
          } else {
            write_data_item(last, rib.field_sizes[i], device_file);
          }
        }
        fclose(device_file);
      }
#ifndef PIC32

      /* By the time we hit any of these else if's we should have
         recorded all the devices */

      else if (r->fields[2]->v.i != 0){
        /* This record affects one device that is specified by the 
           third parameter */
        struct resource_data *last = 0;

        /* Initialize i to point to the right instance of all_devices
         * so that we know where to record the info */
        for (current = first; current; current = current->next) { 
          if (current->value == r->fields[2]->v.i){
				    break;
          } 
        }
        char name[40];
        strcpy(name,current->name);
        strcat(name,".info");
        device_file = fopen(name,"a");
        if (!device_file) {
          fprintf(stderr,"Could not open file %s\n", argv[input_arg+1]);
          exit(1);
        }
        for (i = 0; i < rib.field_count; i++) {
          if (!last) {
            write_data_item(r->fields[i], rib.field_sizes[i], device_file);
            if (r->fields[i]->kind == rik_null) last = r->fields[i];
          } else {
            write_data_item(last, rib.field_sizes[i], device_file);
          }
        }
        fclose(device_file);
      }
      else if (r->fields[2]->v.i == 0){
#if 0
        fprintf(stderr,"******%s\n",r->fields[0]->v.s);
#endif

        /* If we get here this means that the record does not affect just
         * one device. This means that the flags for families should be set
         * and this record should be applied to every device in that family */
        for (current = first; current; current = current->next) {
          struct resource_data *last = 0;
          if ((current->family & (unsigned char)r->fields[1]->v.i) != 0){
#if 0
            fprintf(stderr,"%x %x\n", current->family, 
                 (unsigned char)r->fields[1]->v.i);
            fprintf(stderr,"%s %s\n",r->fields[0]->v.s, current->name);
#endif
            char name[40];
            strcpy(name,current->name);
            strcat(name,".info");
            device_file = fopen(name,"a");
            if (!device_file) {
              fprintf(stderr,"Could not open file %s\n", argv[input_arg+1]);
              exit(1);
            }
            for (i = 0; i < rib.field_count; i++) {
              if (!last) {
                write_data_item(r->fields[i], rib.field_sizes[i], device_file);
                if (r->fields[i]->kind == rik_null) last = r->fields[i];
              } else {
                write_data_item(last, rib.field_sizes[i], device_file);
              }
            }
            fclose(device_file);
          } 
        }
      }
#endif
    }
  }
  else {

    fprintf(stderr, "\nWriting resource file %s...\n", argv[input_arg+1]);
    resource = fopen(argv[input_arg+1],"wb");
  
    if (!resource) {
      fprintf(stderr,"Could not open file %s\n", argv[input_arg+1]);
      exit(1);
    }
  
    if (argv[input_arg+2]) {
      fprintf(stderr, "\nWriting xml file %s...\n", argv[input_arg+2]);
      xml = fopen(argv[input_arg+2],"wb");
  
      if (!xml) {
        fprintf(stderr,"Could not open file %s\n", argv[input_arg+2]);
        exit(1);
      }
    } else xml = 0;

    /* we should make this an option ... */
    if (argv[input_arg+3]) {
      fprintf(stderr, "\nWriting device support xml file %s...\n", 
              argv[input_arg+3]);
      deviceSupportXML = fopen(argv[input_arg+3],"wb");
  
      if (!deviceSupportXML) {
        fprintf(stderr,"Could not open file %s\n", argv[input_arg+2]);
        exit(1);
      }
    } else deviceSupportXML = 0;

    d.kind = rik_string;
    d.v.s = rib.tool_chain;
    write_data_item(&d, 0, resource);
  
    d.kind = rik_version;
    d.v.version.major = rib.version.major;
    d.v.version.minor = rib.version.minor;
    write_data_item(&d, 0, resource);
  
    d.kind = rik_char;
    d.v.c = rib.resource_version_increment;
    write_data_item(&d, 0, resource);
  
    d.kind = rik_int;
    d.v.i = rib.field_count;
    write_data_item(&d, 0, resource);
  
    for (i = 0; i < rib.field_count; i++) {
      d.v.i = rib.field_sizes[i];
      write_data_item(&d, 0, resource);
    }
  
    d.v.i = 0xFFFFFFFF;
    write_data_item(&d,0,resource);
  
    for (r = records; r; r = r->next) {
      struct resource_data *last = 0;
  
      for (i = 0; i < rib.field_count; i++) {
        if (!last) {
          write_data_item(r->fields[i], rib.field_sizes[i], resource);
          if (r->fields[i]->kind == rik_null) last = r->fields[i];
        } else {
          write_data_item(last, rib.field_sizes[i], resource);
        }
      }
    }
  
  
    fclose(resource);
    
    if (xml) {
#ifdef PIC32
      int families[] = {P32MX, P32MZ, 0};
      char *family_names[] = { "PIC32MX","PIC32MZ", };

      int f;

      fprintf(xml,"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                  "\n");
      indent_fprintf(xml,"<lti:LanguageToolSuite\n");
      indent_fprintf(xml,"lti:uuid=\"{ec0473c6-f276-4a29-9b4a-41f899a460a2}\""
                         "\n");
      indent_fprintf(xml,
                    "lti:name=\"MPLAB XC for PIC32MX/PIC32MZ\"\n");
      indent_fprintf(xml,"lti:version=\"%d.%d\"\n",
                     rib.version.major, rib.version.minor);
      indent_fprintf(xml,"xmlns:lti=\"http://www.microchip.com/lti\"\n");
      indent_fprintf(xml,
                     "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
      indent_fprintf(xml,"xsi:schemaLocation=\""
                     "http://www.microchip.com/lti lti/LanguageToolSuite.xsd\">"
                     "\n");
      fprintf(xml,"\n");

      indent_fprintf(xml,"<lti:BaseDir lti:dir=\"..\"/>\n\n");

      for (f = 0; families[f]; f++) {
        int header_dumped = 0;

        if ((families[f] & pic30_family_mask) == 0) continue;
        for (r = records; r; r = r->next) {
          struct resource_data *last = 0;

          if ((r->fields[1]->kind == rik_int) &&
              (r->fields[1]->v.i & families[f])) {
            /* device for the family we are currently dumping */
            if (!header_dumped) {
              header_dumped++;
              indent_fprintf(xml,"<lti:DeviceFamily lti:family=\"%s\">\n\n",
                             family_names[f]);
              indent_fprintf(xml,"<lti:IncludeDir lti:dir=\"pic32mx/include\"/>\n");
              fprintf(xml,"\n");
            }
            indent_fprintf(xml,"<lti:Device lti:device=\"%s\"/>\n",
                           r->fields[0]->v.s);
          }
        }
        if (header_dumped) {
          indent_fprintf(xml,"</lti:DeviceFamily>\n\n");
        }
      }
      indent_fprintf(xml,"</lti:LanguageToolSuite>\n");
#else
      int families[] = { 
        P30F | P30FSMPS, P33F, P24F, P24H, P33E, P24E, P24FK, 0 
      };
      char *family_names[] = { "dsPIC30F",
                               "dsPIC33F",
                               "PIC24F",
                               "PIC24H", 
                               "dsPIC33EP",
                               "PIC24EP",
                               "PIC24FK",
                             };
  
      int f;
  
      fprintf(xml,"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
                  "\n");
      indent_fprintf(xml,"<lti:LanguageToolSuite\n");
      indent_fprintf(xml,"lti:uuid=\"{de75cc75-76be-4a6a-9730-a13b54c87a86}\""
                         "\n");
      indent_fprintf(xml,
                    "lti:name=\"MPLAB C for PIC24F/PIC24H/dsPIC30/dsPIC33\"\n");
      indent_fprintf(xml,"lti:version=\"%d.%d\"\n", 
                     rib.version.major, rib.version.minor);
      indent_fprintf(xml,"xmlns:lti=\"http://www.microchip.com/lti\"\n");
      indent_fprintf(xml,
                     "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
      indent_fprintf(xml,"xsi:schemaLocation=\""
                     "http://www.microchip.com/lti lti/LanguageToolSuite.xsd\">"
                     "\n");
      fprintf(xml,"\n");
  
      indent_fprintf(xml,"<lti:BaseDir lti:dir=\"..\"/>\n\n");
  
      for (f = 0; families[f]; f++) {
        int header_dumped = 0;
  
        if ((families[f] & pic30_family_mask) == 0) continue;
        for (r = records; r; r = r->next) {
          struct resource_data *last = 0;
  
          if ((r->fields[1]->kind == rik_int) &&
              (r->fields[1]->v.i & IS_DEVICE_ID) &&
              (r->fields[1]->v.i & families[f])) {
            /* device for the family we are currently dumping */
            if (!header_dumped) {
              header_dumped++;
              indent_fprintf(xml,"<lti:DeviceFamily lti:family=\"%s\">\n\n",
                             family_names[f]);
              indent_fprintf(xml,"<lti:IncludeDir lti:dir=\"include\"/>\n");
              indent_fprintf(xml,"<lti:IncludeDir lti:dir=\"support/%s/h\"/>\n",
                             family_names[f]);
              if (families[f] == P24F) {
                indent_fprintf(xml,"<lti:IncludeDir "
                                   "lti:dir=\"support/peripheral_24F\"/>\n");
              } else {
                indent_fprintf(xml,"<lti:IncludeDir "
                                "lti:dir=\"support/peripheral_30F_24H_33F\"/>\n");
              }
              fprintf(xml,"\n");
              indent_fprintf(xml,"<lti:AssemblerIncludeDir "
                                 "lti:dir=\"support/%s/inc\"/>\n\n", 
                             family_names[f]);
              indent_fprintf(xml,"<lti:LibraryDir lti:dir=\"lib\"/>\n");
              indent_fprintf(xml,"<lti:LibraryDir "
                                 "lti:dir=\"support/%s/gld\"/>\n", 
                             family_names[f]);
              indent_fprintf(xml,"<lti:LibraryDir "
                                 "lti:dir=\"lib/%s\"/>\n\n", 
                             family_names[f]);
            }
            indent_fprintf(xml,"<lti:Device lti:device=\"%s%s\"/>\n",
                           r->fields[0]->v.s[0] == '2' ? "PIC" : "dsPIC",
                           r->fields[0]->v.s);
          }
        }
        if (header_dumped) {
          indent_fprintf(xml,"</lti:DeviceFamily>\n\n");
        }
      }
      indent_fprintf(xml,"</lti:LanguageToolSuite>\n");
#endif
      fclose(xml);
    }

    if (deviceSupportXML) {
      const char *header = 
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
        "<mp:deviceSupport xmlns:mp=\"http://crownking/mplab\"\n"
        "         xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
        "         xsi:schemaLocation=\"http://crownking/mplab%20mplab/deviceSupport\"\n"
        ">\n\n";

      const char *footer = "</mp:deviceSupport>";

#ifdef PIC32
      int families[] = {
        P32MX | P32MZ,
        0
      };

      char *family_names[] = { "32x",
                               "32x",
                             };
#else
      int families[] = {
        P24F | P24H | P24E | P24FK, P30F | P30FSMPS, P33F | P33E, 0
      };

      char *family_names[] = { "24x",
                               "30x",
                               "33x",
                             };
#endif

      int f;

      fprintf(deviceSupportXML, "%s", header);
      for (f = 0; families[f]; f++) {
        int header_dumped = 0;

        if ((families[f] & pic30_family_mask) == 0) continue;
        for (r = records; r; r = r->next) {
          struct resource_data *last = 0;

          if ((r->fields[1]->kind == rik_int) &&
              (r->fields[1]->v.i & IS_DEVICE_ID) &&
              (r->fields[1]->v.i & families[f])) {
            /* device for the family we are currently dumping */
            if (!header_dumped) {
              header_dumped++;
              indent_fprintf(deviceSupportXML,
                             "<mp:family mp:family=\"%s\">\n\n",
                             family_names[f]);
              fprintf(deviceSupportXML,"\n");
            }
#ifdef PIC32
            indent_fprintf(deviceSupportXML,
                           "<mp:device mp:name=\"%s%s\"/>\n",
                           "PIC",
                           r->fields[0]->v.s);
#else
            indent_fprintf(deviceSupportXML,
                           "<mp:device mp:name=\"%s%s\"/>\n",
                           r->fields[0]->v.s[0] == '2' ? "PIC" : "dsPIC",
                           r->fields[0]->v.s);
#endif
          }
        }
        if (header_dumped) {
          indent_fprintf(deviceSupportXML,"</mp:family>\n\n");
        }
      }
      fprintf(deviceSupportXML, "%s", footer);
      fclose(deviceSupportXML);
    }
  }

  fprintf(stderr,"Total allocation: %d\n",total_alloc);
  return EXIT_SUCCESS;
}
                
