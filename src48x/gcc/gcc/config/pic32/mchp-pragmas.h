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


/* Microchip devices also allow interrupt vector dispactch functions
   to be defined via a pragma or an attribute (of the interrupt handler).
   We keep a list of the dispatch functions needed and emit them at
   the end of processing the translation unit. */
struct vector_dispatch_spec
  {
    const char *target;                 /* target function name */
    enum pic32_isa_mode isr_isa_mode;   /* isa mode of the target function */
    bool longcall;                      /* Is a longcall required? */
    int vector_number;                  /* exception vector table number */
    int emit_dispatch;                  /* nonzero to emit a dispatch function */
    const_tree *node;
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

void mchp_add_vector_dispatch_entry (const char *,int,bool,enum pic32_isa_mode,int);

#endif
