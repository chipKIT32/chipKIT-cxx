/* 
 *  Resource structure defintions, Copyright 2006 Microchip Technology Inc
 */

#ifndef _RESOURCE_TYPES_H
#define _RESOURCE_TYPES_H

struct resource_version {
  unsigned short major;
  unsigned short minor;
};

struct resource_introduction_block {
  char *tool_chain;
  char resource_version_increment;
  struct resource_version version;
  unsigned int field_count;
  unsigned int *field_sizes;
  void *target_specific_data;
};

enum resource_information_kind {
  rik_error =0,
  rik_string,
  rik_char,
  rik_version,
  rik_int,
  rik_null                        /* short resource data list */
};

struct resource_data {
  enum resource_information_kind kind;
  union {
    char *s;
    unsigned char c;
    struct resource_version version;
    unsigned int i;
  } v;
};

enum error_codes {
  ec_none = 0,
  ec_cannot_open_file,       /* file didn't open */
  ec_not_in_resource_data,   /* cannot read_value until first move_to_record()*/
  ec_end_of_record,          /* no more fields to read in record */
  ec_field_too_small,        /* field too small to read_value of this type */
  ec_not_at_field_start,     /* read_byte used and we are half-way finished */
  ec_record_overrun,         /* read_byte overran record */
  ec_end_of_file             /* end of file */
};

extern enum error_codes resource_error;
  
extern int resource_data_size(struct resource_data *);
extern char *resource_pack_string(char *s);
extern struct resource_introduction_block *read_rib(const char *name);
extern struct resource_introduction_block *read_device_rib(const char *name,
                                                           char * device);
extern void close_rib(void);
extern int read_byte(unsigned char *byte);
extern int read_value(enum resource_information_kind kind,
                      struct resource_data *data);
extern int move_to_record(int n);

#endif
