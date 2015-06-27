/*
 *  Resource support file, Copyright 2006 Micorchip Technology Inc
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "resource_types.h"

#ifndef IN_GCC
#define xcalloc calloc
#define xrealloc realloc
#define TOUPPER toupper
#endif

#define res_error(X) {resource_error = X; return 0;}

#ifndef IN_GCC
#define xcalloc calloc
#define xrealloc realloc
#endif

static FILE *input_file;
static struct resource_introduction_block *rib;
static unsigned int resource_start_read = 0;
static int read_short(unsigned short *);
static int read_word(unsigned int *);
static int current_record = 0;
static unsigned int current_field = 0;
static unsigned int read_byte_used = 0;
static int total_record_size = 0;
static struct stat fileinfo;

enum error_codes resource_error = ec_none;

int resource_data_size(struct resource_data *d) {

  switch (d->kind) {
    case rik_string:  return strlen(d->v.s)+1;
    case rik_char:    return sizeof(char);
    case rik_version: return sizeof(struct resource_version);
    case rik_int:     return sizeof(int);
    default: return 0x7FFFFFFF;
  }
}

char *resource_pack_string(char *s) {
  unsigned char *c;
  unsigned char l;

  for (c = (unsigned char *)s; *c; c++) {
    l = *c;
    l = (((l<<1) & 0xAA) | ((l>>1) & 0x55));  /* swap every bit */
    l = (l >>4) | (l << 4);                   /* swap every nybble */
    *c = l;
  }
  return s;
}

struct resource_introduction_block *read_device_rib(const char *name, 
                                                    char * device) {
  unsigned int buffer_size = 80;
  unsigned char *buffer;
  unsigned int i;
  int status = -1;
  
  char *match;
  char *device_buf = 0;
  if (device != NULL) { 
    device_buf = (char *)xcalloc(strlen(name)+strlen("device_files/")+
                                 strlen(device)+strlen(".info")+1,1);
    match = (char *)xcalloc(80,1);
    strcpy(device_buf, name);
#if defined(TARGET_IS_PIC32MX) || defined(_BUILD_C32_)
    match = strstr(device_buf,"xc32_device.info");
#else
    match = strstr(device_buf,"c30_device.info");
#endif

    strcpy(match, "device_files/");
    { 
      char * temp, * place;
      place = (char *)xcalloc(80,1);
      temp = place;
      for (*temp = TOUPPER(*device++);*temp++;) 
        *temp = TOUPPER(*device++);
      strcat(match, place);
      free(place);
    }
    strcat(match, ".info");
    status = stat(device_buf, &fileinfo);
  
    if (status == -1) {
      /* device_buf cannot be statted */
      /* this could be because of the reoganization of the install directory 
       * some of the binaries are in the same directory as the resource file,
       * some are one directory below it... try moving one up and looking again
       */
      char *c;
      char *ultimate = (char *)name,
           *penultimate = (char *)name;
      char *temp;
  
      for (c = (char *)name; *c; c++) {
        if ((*c == '/') || (*c == '\\')) {
          penultimate = ultimate;
          ultimate = c;
          while (c[1] == *c) c++;  /* ignore duplicate slashes */
        }
      }
      temp = (char *)xcalloc(strlen(ultimate)+1,1);
      strcpy(temp, ultimate);
      strcpy(penultimate,ultimate);
#if 0
      status = stat(name,&fileinfo);
#endif
    } else {
      name = device_buf;
    }
  }
  status = stat(name,&fileinfo);
  if ((status == -1) && 
      ((device_buf == 0) || (stat(device_buf,&fileinfo) == -1))) {
    /* device_buf cannot be statted */
    /* try the default device file */
    char *c;
    char *ultimate = (char *)name,
         *penultimate = (char *)name;
    char *temp;

    for (c = (char *)name; *c; c++) {
      if ((*c == '/') || (*c == '\\')) {
        penultimate = ultimate;
        ultimate = c;
        while (c[1] == *c) c++;  /* ignore duplicate slashes */
      }
    }
    temp = (char *)xcalloc(strlen(ultimate)+1,1);
    strcpy(temp, ultimate);
    strcpy(penultimate,temp);
    stat(name,&fileinfo);
  } 

  /* under WINDOZE, the "b" is required because text files are subject to
     translation and ftell and fseek might not agree */
  input_file = fopen(name,"rb");
  if (input_file == 0) {
    rib = 0;
    res_error(ec_cannot_open_file);
  }
  resource_start_read = 0;
  buffer = (unsigned char *)xcalloc(1, buffer_size);
  rib = (struct resource_introduction_block *)xcalloc(1, 
                                    sizeof(struct resource_introduction_block));

  /* tool_chain */
  i = 0;
  do {
    if (i >= buffer_size) {
      unsigned char *new_buffer = (unsigned char *)xrealloc(buffer, 
                                                            buffer_size+80);
      buffer_size += 80;
      if (buffer != new_buffer) {
        buffer= new_buffer;
      }
    }
    read_byte(&buffer[i]);
  } while (buffer[i++]);
  resource_pack_string((char *)buffer);
  rib->tool_chain = (char *)buffer;

  /* version */
  read_short(&rib->version.major);
  read_short(&rib->version.minor);
  
  /* increment */
  read_byte((unsigned char *)&rib->resource_version_increment);

  /* field_count */
  read_word(&rib->field_count);

  /* field_sizes */
  rib->field_sizes = (unsigned int *)xcalloc(rib->field_count, sizeof(int));
  total_record_size = 0;
  for (i = 0; i < rib->field_count; i++) {
    read_word(&rib->field_sizes[i]);
    total_record_size += rib->field_sizes[i];
  }
  return rib;
}  

struct resource_introduction_block *read_rib(const char *name) {
  return read_device_rib(name,0);
}

void close_rib(void) {
  if (rib) {
    fclose(input_file);
    free(rib->tool_chain);
    free(rib->field_sizes);
    free(rib);
  }
}

int read_byte(unsigned char *byte) {
  int result;

  if (input_file == 0) {
    return 0;
  }
  read_byte_used++;
  result = fread(byte, 1, 1, input_file);
  if (result == 0) res_error(ec_end_of_file);
  return result;
}

static int read_short(unsigned short *s) {
  unsigned short val;
  unsigned char byte;

  if (read_byte(&byte)) {
    val = byte;
    if (read_byte(&byte)) {
      val += (byte << 8);
      *s = val;
      return 1;
    }
  }
  return 0;
}

static int read_word(unsigned int *w) {
  unsigned int val;
  unsigned char byte;

  if (read_byte(&byte)) {
    val = byte;
    if (read_byte(&byte)) {
      val += (byte << 8);
      if (read_byte(&byte)) {
        val += (byte << 16);
        if (read_byte(&byte)) {
          val += (byte << 24);
          *w = val;
          return 1;
        }
      }
    }
  }
  return 0;
}

/* what a stupid warning */
static int adjust_for_readbyte(void);
static int adjust_for_readbyte(void) {
  if (read_byte_used) {
    while (read_byte_used) {
      if (read_byte_used >= rib->field_sizes[current_field]) {
        read_byte_used -= rib->field_sizes[current_field];
        current_field++;
        if (current_field >= rib->field_count) res_error(ec_record_overrun);
      } else break;
    }
  }
  return 1;
}

int read_value(enum resource_information_kind kind,
               struct resource_data *data) {
  int size;
  char *c;
  unsigned char dump;

  if (read_byte_used) {
    int e;

    e = adjust_for_readbyte();
    if (e == 0) return 0;
  }
  if (resource_start_read == 0) res_error(ec_not_in_resource_data);
  if (current_field >= rib->field_count) res_error(ec_end_of_record);
  if (read_byte_used) res_error(ec_not_at_field_start);
  data->kind = kind;
  size = rib->field_sizes[current_field];
  if (kind != rik_string) {
    if (resource_data_size(data) > size) res_error(ec_field_too_small);
  }
  switch (kind) {
    case rik_char:    if (read_byte(&data->v.c) == 0) return 0;
                      size--;  
                      break;
    case rik_int:     if (read_word(&data->v.i) == 0) return 0; 
                      size-=4; 
                      break;
    case rik_version: if (read_short(&data->v.version.major)) return 0;
                      if (read_short(&data->v.version.minor)) return 0;
                      size-=4; 
                      break;
    case rik_string:  data->v.s = (char *)xcalloc(size, 1);
                      c = data->v.s;
                      while (size) {
                        if (read_byte((unsigned char *)c) == 0) return 0;
                        size--;
                        if (*c == 0) break;
                        c++;
                      }
                      resource_pack_string(data->v.s);
                      break;
    default:          break;
  }
  while (size) {
    read_byte(&dump);
    size--;
  }
  read_byte_used = 0;
  current_field++;
  return 1;
}

int move_to_record(int n) {
  unsigned char dump;
  int offset;

  if (resource_start_read == 0) {
    while (resource_start_read != 0xFFFFFFFF) {
      if (read_byte(&dump) == 0) return 0;
      resource_start_read = (resource_start_read << 8) | dump;
    }
    current_record = 0;
    current_field = 0;
    read_byte_used = 0;
  } 
  if (read_byte_used) {
    resource_start_read = 0;
    fseek(input_file, 0L, SEEK_SET);
    return move_to_record(n);
  }

  if (current_field == rib->field_count) {
    current_field = 0;
    current_record++;
  }
  if (current_field != 0) {
    unsigned int i;
    int size = 0;

    for (i = current_field; i < rib->field_count; i++) 
      size += rib->field_sizes[i];
    fseek(input_file, size, SEEK_CUR);
    current_record++;
    current_field = 0;
  }
  if (current_record == n) return 1;
  offset = (n - current_record) * total_record_size;
  if (ftell(input_file) + offset < fileinfo.st_size) {
    fseek(input_file,offset,SEEK_CUR);
    current_record = n;
    current_field = 0;
    return 1;
  }

  return 0;
}

#undef res_error
