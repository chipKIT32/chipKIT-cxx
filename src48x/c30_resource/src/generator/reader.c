/*
 *  resource file reader: Copyright 2006 Microchip Technology Inc
 *
 *  interprets for C30 if -c30 is provided
 *  (otherwise just dumps raw data )
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource_types.h"
#include "c30_flag_definitions.h"

struct resource_introduction_block *rib;
int target_c30 = 0;
int check = 0;
unsigned char *buffer;
int buffer_size;

int show_raw(unsigned char *buffer, int offset, int length, int width) {
   int i;
   int p = 0;
   
   for (i = 0; i < length; i++) {
     if (p == 0) {
       fprintf(stderr,"/* ");
     }
     p = p + 1;
     fprintf(stderr,"0x%2.2x", buffer[offset+i]);
     if ((p == 8) && (i+1 < length)) {
       fprintf(stderr," */\n");
       p = 0;
     } else {
       fprintf(stderr," ");
     }
   }
   while (p < 8) {
     fprintf(stderr,"     "); 
     p++;
   }
   if (length != 0) fprintf(stderr,"*/ ");
   else fprintf(stderr,"   /* ");
   return offset+length;
}

int main(int argc, char **argv) {

  int last_device_id;
  int devices_found=0;
  int filename_arg = 1;
  int i;
  int raw = 0;
  FILE *in;
  int total_record_size = 0;
  unsigned int record_start = 0;
  int error;
  int record=0;
  char * pic30_target_cpu = NULL;
  while (*argv[filename_arg] == '-') {
    if (strcmp(argv[filename_arg],"-c30") == 0) target_c30 = 1;
    if (strcmp(argv[filename_arg],"-check") == 0) check = 1;
    filename_arg++;
  }

  if (check && !target_c30) {
    fprintf(stderr,"**** -check only for C30\n");
    check=0;
  }

  rib = read_rib(argv[filename_arg]);
  in = fopen(argv[filename_arg],"rb");
  buffer_size = 256;
  buffer = malloc(buffer_size);
  if ((rib == 0) || (in == 0)) {
    fprintf(stderr,"Could not open file: %s\n", argv[filename_arg]);
    return 0;
  }
  fprintf(stderr,"\n");
  i = strlen(rib->tool_chain) + 1 +
      sizeof(rib->version) +
      sizeof(rib->resource_version_increment) +
      sizeof(rib->field_count) + 
      sizeof(int)*rib->field_count;
  fread(buffer, i, 1, in);

  raw = show_raw(buffer, raw, strlen(rib->tool_chain)+1, 8); 
  fprintf(stderr,"tool_chain = \"%s\"\n", rib->tool_chain);

  raw = show_raw(buffer, raw, 4, 8); 
  fprintf(stderr,"tool_version = %d.%2.2d\n", rib->version.major, 
                                              rib->version.minor);

  raw = show_raw(buffer, raw, 1, 8);
  fprintf(stderr,"resource_version = '%c'\n", rib->resource_version_increment);

  raw = show_raw(buffer, raw, 4, 8);
  fprintf(stderr,"field_count = %d\n", rib->field_count);

  raw = show_raw(buffer, raw, sizeof(int)*rib->field_count, 8);
  fprintf(stderr,"field_sizes = { ");
  total_record_size = 0;
  for (i = 0;i < rib->field_count;i++) {
    total_record_size += rib->field_sizes[i];
    fprintf(stderr,"%d%c", rib->field_sizes[i], 
                           i + 1 < rib->field_count ? ',' : ' ');
  }
  fprintf(stderr,"}\n");
  for (i = 0; record_start != 0xFFFFFFFF; i++) {
    if (i >= buffer_size) {
      char *new_buffer = realloc(buffer,buffer_size+256);

      buffer_size += 256;
      if (new_buffer != buffer) {
        free(buffer);
      }
      buffer = new_buffer;
    }
    fread(&buffer[i], 1, 1, in);
    record_start = (record_start << 8) | (buffer[i]);
  }
  raw = 0;
  raw = show_raw(buffer, 0, i, 8);
  fprintf(stderr,"resource_info_start\n");

  do {
    error = fread(buffer, total_record_size, 1, in);
    if (error == 0) break; /* read last one */
    fprintf(stderr,"\n");
    show_raw(0,0,0,8);
    fprintf(stderr,"Record %d */ {\n\n",record);
    raw = 0;
    if ((target_c30) && (rib->field_count >= 3) && (rib->field_count <=6)) {
      struct resource_data d;
      int device_id;
      int vector_id;
      int codeguard_id;

#define PRETTY \
        if (raw + o > 70) {                              \
          fprintf(stderr,"\n");                          \
          for (c = 0; c < raw+10; c++) fprintf(stderr," "); \
          o = 0;                                         \
        }

      move_to_record(record++);
    
      read_value(rik_string, &d);
      raw = show_raw(buffer, raw, rib->field_sizes[0], 8);
      fprintf(stderr,"\"%s\",\n", d.v.s);
      free(d.v.s);

      read_value(rik_int, &d);
      raw = show_raw(buffer, raw, rib->field_sizes[1], 8);
      device_id=0;
      vector_id=0;
      codeguard_id=0;
      if ((d.v.i & RECORD_TYPE_MASK) == IS_DEVICE_ID) {
        char *family = "UNK";
        int o = 0,c;

        device_id=1;
        switch (d.v.i & FAMILY_MASK) {
          case P30F:     family = "dsPIC30F"; break;
          case P30FSMPS: family = "dsPIC30F SMPS"; break;
          case P33F:     family = "dsPIC33F"; break;
          case P24F:     family = "PIC24F"; break;
          case P24H:     family = "PIC24H"; break;
          case P24FK:    family = "PIC24FK"; break;
          case P33E:     family = "dsPIC33E"; break;
          case P24E:     family = "PIC24E"; break;
          break;
        }
        fprintf(stderr,"FAMILY: %s\n", family);
        for (c = 0; c < raw+10; c++) fprintf(stderr," ");
	if (d.v.i & HAS_DSP) o += fprintf(stderr, "DSP ");

        PRETTY
        if (d.v.i & HAS_EEDATA) o += fprintf(stderr, "EE ");
        PRETTY
        if (d.v.i & HAS_DMA) o += fprintf(stderr, "DMA ");
        PRETTY
        if (d.v.i & HAS_CODEGUARD) o += fprintf(stderr, "CG ");
        PRETTY
        if (d.v.i & HAS_PMP) o += fprintf(stderr, "PMP ");
        PRETTY
        if (d.v.i & HAS_EDS) o += fprintf(stderr, "EDS ");
        PRETTY
        if (d.v.i & HAS_PMPV2) o += fprintf(stderr, "PMPV2 ");
        PRETTY
        if (d.v.i & HAS_ECORE) o += fprintf(stderr, "ECORE ");
        PRETTY
        if (d.v.i & HAS_DMAV2) o += fprintf(stderr, "DMAV2 ");
        PRETTY
        if (d.v.i & HAS_AUXFLASH) o += fprintf(stderr, "AUX ");
        PRETTY
        if (d.v.i & HAS_5VOLTS) o += fprintf(stderr, "5V ");
        fprintf(stderr,"\n");
      } else if ((d.v.i & RECORD_TYPE_MASK) == IS_VECTOR_ID) {
        int o = 0,c;

        vector_id=1;
        fprintf(stderr,"FAMILY: ");
        if (d.v.i & P30F)     o += fprintf(stderr,"dsPIC30F ");
        PRETTY
        if (d.v.i & P30FSMPS) o += fprintf(stderr,"dsPIC30F SMPS ");
        PRETTY
        if (d.v.i & P33F)     o += fprintf(stderr,"dsPIC33F ");
        PRETTY
        if (d.v.i & P24F)     o += fprintf(stderr,"PIC24F ");
        PRETTY
        if (d.v.i & P24H)     o += fprintf(stderr,"PIC24H ");
        PRETTY
        if (d.v.i & P24FK)    o += fprintf(stderr,"PIC24FK ");
        PRETTY
        if (d.v.i & P33E)     o += fprintf(stderr,"dsPIC33E ");
        PRETTY
        if (d.v.i & P24E)     o += fprintf(stderr,"PIC24E ");
        fprintf(stderr,"\n");
      } else if ((d.v.i & RECORD_TYPE_MASK) == IS_CODEGUARD_ID) {
        int o = 0,c;
        
        codeguard_id = 1;
#define CG \
        if (d.v.i & FLASH) o += fprintf(stderr,"FLASH "); \
        PRETTY \
        if (d.v.i & RAM) o += fprintf(stderr,"RAM "); \
        PRETTY \
        if (d.v.i & EEPROM) o += fprintf(stderr,"EEPROM "); \
        PRETTY \
        if (d.v.i & NONE) o += fprintf(stderr,"NONE "); \
        PRETTY \
        if (d.v.i & SMALL) o += fprintf(stderr,"SMALL "); \
        PRETTY \
        if (d.v.i & MEDIUM) o += fprintf(stderr,"MED "); \
        PRETTY \
        if (d.v.i & LARGE) o += fprintf(stderr,"LARGE "); \
        PRETTY \
        if (d.v.i & BOOT) o += fprintf(stderr,"BOOT "); \
        PRETTY \
        if (d.v.i & SECURE) o += fprintf(stderr,"SEC "); \
        PRETTY \
        if (d.v.i & GENERAL) o += fprintf(stderr,"GEN "); \
        PRETTY \
        if (d.v.i & CODE_PROTECT) o += fprintf(stderr,"CODEP "); \
        PRETTY \
        if (d.v.i & WRITE_PROTECT) o += fprintf(stderr,"WRITEP "); \
        PRETTY \
        if (d.v.i & STANDARD) o += fprintf(stderr,"STNDRD "); \
        PRETTY \
        if (d.v.i & HIGH) o += fprintf(stderr,"HIGH "); \
        PRETTY \
        if (d.v.i & ON) o += fprintf(stderr,"ON "); \
        PRETTY \
        if (d.v.i & OFF) o += fprintf(stderr,"OFF "); \
        fprintf(stderr,"\n");

        CG
        
      } else fprintf(stderr,"0x%8.8x,\n", d.v.i);

      read_value(rik_int, &d);
      raw = show_raw(buffer, raw, rib->field_sizes[2], 8);
      if (rib->field_count == 3) {
        fprintf(stderr,"0x%8.8x }\n", d.v.i);
      } else {
        if (device_id | vector_id | codeguard_id) {
          if (device_id) {
            last_device_id = d.v.i;
            devices_found++;
          } else {
            if (devices_found == 1) {
              // better be 0 or same device, otherwise mistake
              if ((d.v.i != 0) && (d.v.i != last_device_id)) 
                fprintf(stderr,"ERROR ");
            }
          }
          fprintf(stderr,"DEVICE_ID: %d\n", d.v.i);
        } else fprintf(stderr,"0x%8.8x\n", d.v.i);

        read_value(rik_int, &d);
        raw = show_raw(buffer, raw, rib->field_sizes[3], 8);
        fprintf(stderr,"0x%8.8x,\n", d.v.i);

        read_value(rik_int, &d);
        raw = show_raw(buffer, raw, rib->field_sizes[4], 8);
        fprintf(stderr,"0x%8.8x,\n", d.v.i);

        read_value(rik_int, &d);
        raw = show_raw(buffer, raw, rib->field_sizes[5], 8);
        fprintf(stderr,"0x%8.8x }\n", d.v.i);
      }
      fprintf(stderr,"\n");
    } else {
      raw = show_raw(buffer, raw, total_record_size, 8);
      fprintf(stderr,"}\n");
      record++;
    }
  } while(1);
  close_rib();
}
