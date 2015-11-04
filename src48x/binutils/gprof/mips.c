/*
 * Copyright (c) 1983, 1993, 1998
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "gprof.h"
#include "search_list.h"
#include "source.h"
#include "symtab.h"
#include "cg_arcs.h"
#include "corefile.h"
#include "hist.h"

static Sym indirect_child;

void mips_find_call (Sym *, bfd_vma, bfd_vma);

void
mips_find_call (Sym *parent, bfd_vma p_lowpc, bfd_vma p_highpc)
{
  bfd_vma pc, dest_pc;
  unsigned int op;
  int offset;
  Sym *child;
  static bfd_boolean inited = FALSE;
  int mips16;

  if (!inited)
    {
      inited = TRUE;
      sym_init (&indirect_child);
      indirect_child.name = _("<indirect child>");
      indirect_child.cg.prop.fract = 1.0;
      indirect_child.cg.cyc.head = &indirect_child;
    }
  mips16 = (p_lowpc & 1) != 0;
  p_lowpc &= ~1;
  p_highpc &= ~1;

  DBG (CALLDEBUG, printf (_("[find_call] %s: 0x%lx to 0x%lx%s\n"),
			  parent->name, (unsigned long) p_lowpc,
			  (unsigned long) p_highpc,
			  mips16 ? " (mips16)" : ""));
  for (pc = p_lowpc; pc < p_highpc;)
    {
      if (!mips16)
	{
	  op = bfd_get_32 (core_bfd, &((char *)core_text_space)[pc - p_lowpc]);
	  pc += 4;
	  if ((op & 0xfc000000) == 0x0c000000)		/* jal */
	    {
	      DBG (CALLDEBUG,
		   printf (_("[find_call] 0x%lx: jal"), (unsigned long) pc-4));
	      offset = (op & 0x03ffffff) << 2;
	      dest_pc = (pc & ~(bfd_vma) 0xfffffff) | offset;
	      pc += 4; /* skip delay slot */
	    }
	  else if ((op & 0xfc000000) == 0x74000000) 	/* jalx */
	    {
	      DBG (CALLDEBUG,
		   printf (_("[find_call] 0x%lx: jalx"), (unsigned long) pc-4));
	      offset = (op & 0x03ffffff) << 2;
	      dest_pc = (pc & ~(bfd_vma) 0xfffffff) | offset | 1;
	      pc += 4; /* skip delay slot */
	    }
	  else if ((op & 0xfc1f07ff) == 0x00000009)	/* jalr */
	    {
	      DBG (CALLDEBUG, printf ("[find_call]\t0x%lx:jalr\n" ,
				      (unsigned long)pc-4));
	      arc_add (parent, &indirect_child, (long)0);
	      pc += 4; /* skip delay slot */
	      continue;
	    }
	  else if ((op & 0xfc1c0000) == 0x04100000)	/* bxxzal */
	    {
	      DBG (CALLDEBUG, printf ("[find_call]\t0x%lx:bal" ,
				      (unsigned long)pc-4));
	      dest_pc = pc + ((short)op << 2) + 4;
	      pc += 4; /* skip delay slot */
	    }
	  else
	    continue;
	}
      else
	{
	  /* MIPS16 function */
	  op = bfd_get_16 (core_bfd, &((char *)core_text_space)[pc - p_lowpc]);
	  pc += 2;
	  if ((op & 0xfc00) == 0x1800			/* jal */
	      || (op & 0xfc00) == 0x1c00)		/* jalx */
	    {
	      DBG (CALLDEBUG, printf ("[find_call]\t0x%lx:jal%s",
				      (unsigned long)pc-2,
				      (op & 0xfc00) == 0x1c00 ? "x" : ""));
	      offset = bfd_get_16 (core_bfd,
				   &((char *)core_text_space)[pc - p_lowpc]);
	      pc += 2;
	      offset |= (op & 0x1f) << 21;
	      offset |= (op & 0x3e0) << (16 - 5);
	      dest_pc = (pc & ~(bfd_vma)0x0fffffff)
		+ ((offset & 0x3ffffff) << 2);
	      if ((op & 0xfc00) == 0x1800)		/* jal */
		dest_pc |= 1;
	      pc += 2;		/* skip delay slot */
	    }
	  else if ((op & 0xf8ff) == 0xe840)		/* jalr */
	    {
	      DBG (CALLDEBUG, printf ("[find_call]\t0x%lx:jalr\n" ,
				      (unsigned long)pc-2));
	      arc_add (parent, &indirect_child, (long)0);
	      pc += 2;		/* skip delay slot */
	      continue;
	    }
	  else if ((op & 0xf8ff) == 0xe8c0)		/* jalrc */
	    {
	      DBG (CALLDEBUG, printf ("[find_call]\t0x%lx:jalrc\n" ,
				      (unsigned long)pc-2));
	      arc_add (parent, &indirect_child, (long)0);
	      continue;
	    }
	  else if ((op & 0xff7f) == 0xe820		/* jr/jrc $31 */
		   || (op & 0xff7f) == 0xef00		/* jr/jrc $7 */
		   || (op & 0xff80) == 0x6400		/* restore */
		   || (op & 0xff1f) == 0xed09		/* exit */
		   || (op & 0xff1f) == 0xee09		/* exit */
		   || (op & 0xff1f) == 0xef09)		/* exit */
	    {
	      /* Skip constants after the end of the function. */
	      DBG (CALLDEBUG, printf ("[find_call]\t0x%lx:end\n" ,
				      (unsigned long)pc-2));
	      return;
	    }
	  else
	    continue;
	}

      if (dest_pc >= p_lowpc && dest_pc <= p_highpc)
	{
	  child = sym_lookup (&symtab, dest_pc);
	  if (child)
	    {
	      DBG (CALLDEBUG,
		   printf (" 0x%lx\t; name=%s, addr=0x%lx",
			   (unsigned long) dest_pc, child->name,
			   (unsigned long) child->addr));
	      if (child->addr == dest_pc)
		{
		  DBG (CALLDEBUG, printf ("\n"));
		  /* a hit:  */
		  arc_add (parent, child, (unsigned long) 0);
		  continue;
		}
	    }
	}
      /* Something funny going on.  */
      DBG (CALLDEBUG, printf ("\tbut it's a botch\n"));
    }
}
