/* Macros for mips*-octeon-* target.
   Copyright (C) 2004, 2005, 2006 Cavium Networks.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.  */

#define CVMX_SHARED_BSS_FLAGS (SECTION_WRITE | SECTION_BSS)

#undef TARGET_ASM_SELECT_SECTION
#define TARGET_ASM_SELECT_SECTION octeon_select_section

#undef TARGET_ASM_UNIQUE_SECTION
#define TARGET_ASM_UNIQUE_SECTION octeon_unique_section

/* Implement ASM_OUTPUT_ALIGNED_DECL_LOCAL. This differs from the 
   generic version only in the use of cvmx_shared attribute.  */
 
#undef ASM_OUTPUT_ALIGNED_DECL_LOCAL
#define ASM_OUTPUT_ALIGNED_DECL_LOCAL(STREAM, DECL, NAME, SIZE, ALIGN) \
  do									\
    {									\
      if ((DECL) && TREE_CODE ((DECL)) == VAR_DECL 			\
	  && lookup_attribute ("cvmx_shared", DECL_ATTRIBUTES (DECL)))	\
	{  								\
           fprintf ((STREAM), "%s", LOCAL_ASM_OP);			\
           assemble_name ((STREAM), (NAME));				\
           fprintf ((STREAM), "\n");					\
	   octeon_output_shared_variable ((STREAM), (DECL), (NAME),     \
					  (SIZE), (ALIGN));             \
	 } 								\
      else 								\
	ASM_OUTPUT_ALIGNED_LOCAL (STREAM, NAME, SIZE, ALIGN);		\
    }									\
  while (0) 


/* Implement ASM_OUTPUT_ALIGNED_DECL_COMMON.  This differs from the mips 
   version only in the use of cvmx_shared attribute.  */

#undef ASM_OUTPUT_ALIGNED_DECL_COMMON
#define ASM_OUTPUT_ALIGNED_DECL_COMMON(STREAM, DECL, NAME, SIZE, ALIGN)	   \
  {									   \
    if (TREE_CODE ((DECL)) == VAR_DECL			   		   \
        && lookup_attribute ("cvmx_shared", DECL_ATTRIBUTES ((DECL))))	   \
      {									   \
	if (TREE_PUBLIC ((DECL)) && DECL_NAME ((DECL)))			   \
	  targetm.asm_out.globalize_label (asm_out_file, (NAME));	   \
	octeon_output_shared_variable ((STREAM), (DECL), (NAME),	   \
				       (SIZE), (ALIGN));		   \
      }									   \
    else								   \
      mips_output_aligned_decl_common ((STREAM), (DECL), (NAME), (SIZE),   \
				       (ALIGN));			   \
   }
