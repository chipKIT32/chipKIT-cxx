/* mchp-pragmas.c */
/* Subroutines used for MCHP PIC32 pragma handling.
   Copyright (C) 1989, 1990, 1991, 1993, 1994, 1995, 1996, 1997, 1998,
   1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009
   Free Software Foundation, Inc.
   Contributed by J. Grosbach, james.grosbach@microchip.com, and
   T. Kuhrt, tracy.kuhrt@microchip.com
   Changes by J. Kajita, jason.kajita@microchip.com, and
   G. Loegel, george.loegel@microchip.com

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef MCHP_PRAGMAS_H
#define MCHP_PRAGMAS_H

#define MCHP_CONFIGURATION_DATA_FILENAME "configuration.data"
#define MCHP_CONFIGURATION_HEADER_MARKER \
  "Daytona Configuration Word Definitions: "
#define MCHP_CONFIGURATION_HEADER_VERSION "0001"
#define MCHP_CONFIGURATION_HEADER_SIZE \
  (sizeof (MCHP_CONFIGURATION_HEADER_MARKER) + 5)

#define MCHP_WORD_MARKER        "CWORD:"
#define MCHP_SETTING_MARKER     "CSETTING:"
#define MCHP_VALUE_MARKER       "CVALUE:"
#define MCHP_WORD_MARKER_LEN    (sizeof (MCHP_WORD_MARKER) - 1)
#define MCHP_SETTING_MARKER_LEN (sizeof (MCHP_SETTING_MARKER) - 1)
#define MCHP_VALUE_MARKER_LEN   (sizeof (MCHP_VALUE_MARKER) - 1)

/* Pretty much arbitrary max line length for the config data file.
   Anything longer than this is either absurd or a bogus file. */
#define MCHP_MAX_CONFIG_LINE_LENGTH 1024

struct mchp_config_value
  {
    char *name;
    char *description;
    unsigned value;
    struct mchp_config_value *next;
  };
struct mchp_config_setting
  {
    char *name;
    char *description;
    unsigned mask;
    struct mchp_config_value *values;
    struct mchp_config_setting *next;
  };
struct mchp_config_word
  {
    unsigned address;
    unsigned mask;
    unsigned default_value;
    struct mchp_config_setting *settings;
  };

struct mchp_config_specification
  {
    struct mchp_config_word *word; /* the definition of the word this value
                                    is referencing */
    unsigned value;           /* the value of the word to put to the device */
    unsigned referenced_bits; /* the bits which have been explicitly specified
                                i.e., have had a setting = value pair in a
                                config pragma */
    struct mchp_config_specification *next;
  };

extern struct mchp_config_specification *mchp_configuration_values;

/* Microchip devices also allow interrupt vector dispactch functions
   to be defined via a pragma or an attribute (of the interrupt handler).
   We keep a list of the dispatch functions needed and emit them at
   the end of processing the translation unit. */
struct vector_dispatch_spec
  {
    const char *target; /* target function name */
    int vector_number;  /* exception vector table number */
    struct vector_dispatch_spec *next;
  };
extern struct vector_dispatch_spec *vector_dispatch_list_head;

/* Microchip devices allow an interrupt specification via pragma.
   We keep a list of the functions specified via pragma and apply the
   interrupt attribute to them when we see a declaration later. */
struct interrupt_pragma_spec
  {
    const char *name;     /* name of the interrupt function */
    tree ipl;             /* IPL */
    unsigned vect_number;  /* If placing the interrup at the actual
                           vector, which one? Zero means don't place the
                           handler at the vector. */
    struct interrupt_pragma_spec *next;
  };
extern struct interrupt_pragma_spec *interrupt_pragma_list_head;

#endif
