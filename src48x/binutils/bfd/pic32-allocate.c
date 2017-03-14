/*
** pic32-allocate.c
**
** Copyright (c) 2009 Microchip Technology, Inc.
**
** This file contains pic32-specifc memory allocation
** processing for the linker.
**
** It is included by the following target-specific
** emulation files:
**
**   ld/emultmpl/elf32pic32mx.em
**
*/

/*****************************************************************************
**
** Improved Memory Allocation Routines
**
** These functions implement the improved
** memory allocator .
**
*/

/* useful constants and macros */
#define NEAR_BOUNDARY   0xa0010000

#define VALID_NEAR(addr, len) ((bfd_vma)(addr)+(len) <= NEAR_BOUNDARY)

#define VALID_LOW_ADDR(addr, len)  ((bfd_vma)(addr)+(len) <= exclude_addr)
#define VALID_HIGH_ADDR(addr, len)  ((bfd_vma)(addr) >= exclude_addr)

/* Create an id for each section attribute */
#define ATTR(a,b,c)  _##a,
 enum {
#include "pic32-attributes.h"
 } ;

/* Create a bit mask for each section attribute */
#undef ATTR
#define ATTR(a,b,c)  a = (1<<_##a),
 enum {
#include "pic32-attributes.h"
 } ;

/* Create a bit mask to match any attribute */
#define all_attr 0xFFFFFFFF

/* \lghica coresident*/
extern bfd_boolean pic32_coresident_app;
///\ prev_shared_dinit holds the address of the previous linked .shared.dinit section
extern bfd_vma prev_shared_dinit;
extern bfd_vma crt_shared_dinit;


/* Memory allocator options  */
enum {
  NO_LOCATE_OPTION,
  FAVOR_LOW_ADDR,     /* scan free blocks from low addr only  */
  FAVOR_HIGH_ADDR,    /* scan free blocks from high addr only */
  EXCLUDE_LOW_ADDR,   /* avoid memory below LIMIT             */
  EXCLUDE_HIGH_ADDR,  /* avoid memory above LIMIT - 1         */
  LOCATE_HIGHEST,
};

/* Data structure for free program memory blocks */
extern struct pic32_memory *program_memory_free_blocks;

static void
finish_section_info(struct pic32_section *s, lang_output_section_statement_type *os);

static int locate_options = 0;
static bfd_vma exclude_addr = 0;
static bfd_vma ramfunc_begin = 0;

static void
set_locate_options(int optionvalue, bfd_vma addr) {
  locate_options = optionvalue;
  exclude_addr = addr;
}

static void
reset_locate_options(void) {
  locate_options = NO_LOCATE_OPTION;
  exclude_addr = 0;
}

#define IS_LOCATE_OPTION(a)  ((locate_options & (a)) == (a))

static void
remove_section_from_bfd(bfd *abfd, asection *sec)
{
  asection *s, *prev;

  s = abfd->sections;
  if ((s == NULL) || (s->next == NULL)) return;

  prev = s;
  s = s->next; /* never remove the first section */
  for (; s != NULL; s = s->next) {
      if (s == sec) {
        prev->next = s->next;
        abfd->section_count -= 1;
        if (pic32_debug)
          printf("    removing section %s\n", s->name);
      }
      else
        prev = s;
    }
  return;
} /* static void remove_section_from_bfd (...)*/

static void
report_allocation_error(struct pic32_section *s) {
#define PREFIX "/tmp"
    char *colon = "";
    char *filename = "";
    char *secname = "*";
    char *s1 = pic32_section_size_string(s->sec);
    char *s2 = pic32_section_attr_string(s->sec);

    /* report the file name, unless its a temp file */
    if (strncmp(s->sec->owner->filename, PREFIX, strlen(PREFIX)) != 0) {
      colon = ":";
      filename = (char *) s->sec->owner->filename;
    }

    /* report the section name, unless its based on a temp file */
    if (strncmp(s->sec->name, PREFIX, strlen(PREFIX)) != 0)
      secname = (char *) s->sec->name;

    einfo(_("%X%s%s Link Error: Could not allocate section %s, %s, %s\n"),
          filename, colon, secname, s1 ? s1 : "", s2 ? s2 : "");

    if (s1) free(s1);  /* free the malloc'ed strings */
    if (s2) free(s2);

    if (pic32_debug)
      printf("\n    Error: Could not allocate section %s\n", s->sec->name);
}

extern asection* init_shared_template;


/*
 * update_shared_dinit_addr()
 *
 * This function updates __shared_dinit_addr. It is used in coresident context, 
 * when shared variables are used. If shared variables are used in 2 linking steps,
 * the value hold by __shared_dinit_addr at this linking phase is the one 
 * set in the previous step. This function updates the value, but also save the 
 * old value in a variable, as it is needed for .shared.dinit section update
 *
 * Called by: allocate_memory()
 *
 * Calls:     bfd_link_hash_lookup()
 *
 */
static void update_shared_dinit_addr()
{
    asection*                   shared_dinit_sec;
    struct bfd_link_hash_entry  *h_sym;
    bfd_vma                     new_shared_addr = 0;
    
    h_sym = bfd_link_hash_lookup(link_info.hash, "__shared_dinit_addr", FALSE, FALSE, TRUE);
    

    shared_dinit_sec = init_shared_template->output_section;
    
    if (shared_dinit_sec != NULL)
    {
        //if (pic32_debug)
        //{
        //    printf ("LG --- .shared.dinit LMA 0x%x\n", shared_dinit_sec->lma);
        //}
        new_shared_addr = shared_dinit_sec->lma;
    }
    
    if ((h_sym != NULL)
        && (h_sym->u.def.section == bfd_abs_section_ptr))
    {
        //if (pic32_debug)
        //    printf("LG --- shared_dinit_addr - 0x%x\n", h_sym->u.def.value);
        
        if ((new_shared_addr != 0) && (new_shared_addr != h_sym->u.def.value))
        {
            ///\ update the content of the previous .shared.dinit section
            prev_shared_dinit = h_sym->u.def.value;
            crt_shared_dinit = new_shared_addr;
            h_sym->u.def.value = new_shared_addr;
        }
    }
}

/*
 * allocate_memory()
 *
 * This function attempts to locate sections using
 * a best-fit algorithm. The term "best-fit" implies
 * that the smallest suitable free block will be used
 * for each section. This approach makes efficient use
 * of the free block list and reduces fragmentation.
 *
 * Called by: ldemul_after_allocation()
 *
 * Calls:     allocate_program_memory()
 *            allocate_data_memory()
 *            bfd_map_over_sections()
 *
 * If any of the sub-processes fail, report it
 * and continue. This helps to suppress misleading
 * messages, and follows the general philosophy
 * of doing as much work as we can, despite the
 * occurrence of fatal errors.
 */
static void
allocate_memory() {
  int result;
  lang_output_section_statement_type **last_os;

#define ERR_STR " Link Error: Could not allocate "

  /* save the last output section statement
     after sequential allocation */
  last_os = (lang_output_section_statement_type **) statement_list.tail;

  /*
   * Build an ordered list of output sections
   * that were placed by sequential allocator.
   * It will help identify any gaps between
   * output sections that are available.
   */
  pic32_init_section_list (&pic32_section_list);

  bfd_map_over_sections (link_info.output_bfd, &pic32_build_section_list, NULL);
  bfd_map_over_sections (link_info.output_bfd, &pic32_build_section_list_vma, NULL);

  if (pic32_debug) {
    pic32_print_section_list(unassigned_sections, "unassigned");
    pic32_print_section_list(memory_region_list, "memory region");
  }

  result = allocate_data_memory();
  if (result != 0)
    einfo(_("%F%sdata memory\n"), ERR_STR );

  result = allocate_serial_memory();
  if (result != 0)
    einfo(_("%F%sserial memory\n"), ERR_STR );

  result = allocate_program_memory();
  if (result != 0)
    einfo(_("%F%sprogram memory\n"), ERR_STR );
#if 1
  if (has_user_defined_memory) {
    result = allocate_user_memory();
    if (result != 0)
      einfo(_("%F%suser-defined memory region\n"), ERR_STR );
  }
#endif
  /* allocate the heap, if required */

  /* allocate the stack, unless the user has defined one */

  /* free the output section list */
  pic32_free_section_list(&pic32_section_list);

  /*
   * Scan over the output section statements
   * and merge any contiguous sections
   * with the same name, unless the
   * --unique option tells us not to.
   *
   * We start where the sequential allocator
   * finished, so that any merges are well understood.
   * For example, all of the best-fit output sections
   * contain a single input section.
   */
  {
    lang_statement_union_type *os, *next;
    asection *sec, *next_sec;
    unsigned int len, match, merge_sec = 0;
    bfd_vma sec_len, next_len = 0, merge_len = 0;
    char *p,*p2;

    if (pic32_debug)
      printf("\nScanning the output statements\n");

    for (os = (lang_statement_union_type *) *last_os;
         os != (lang_statement_union_type *) NULL;
         os = next) {

      /* clear the accumulator, if we didn't just merge */
      if (!merge_sec) merge_len = 0;

      merge_sec = 0;
      next = os->header.next;
      if (os->header.type == lang_output_section_statement_enum) {

        if (os->output_section_statement.bfd_section == NULL)
          /* --gc-sections has discarded this section */
          continue;

        sec = os->output_section_statement.bfd_section;
        if (os->output_section_statement.children.head->input_section.section->rawsize)
          sec_len = os->output_section_statement.children.head
                    ->input_section.section->rawsize;
        else
          sec_len = os->output_section_statement.children.head
                    ->input_section.section->size;

        if (!sec || !sec->name) continue;

        if (next && (next->header.type == lang_output_section_statement_enum)) {
          next_sec = next->output_section_statement.bfd_section;
	  if (next->output_section_statement.children.head) {
            if (next->output_section_statement.children.head->
                         input_section.section->rawsize)
              next_len = next->output_section_statement.children.head->
                         input_section.section->rawsize;
            else
              next_len = next->output_section_statement.children.head->
                         input_section.section->size;
          }

          if (!next_sec || !next_sec->name) continue;

          if (next->output_section_statement.children.head->
                input_section.section->size == 0) continue;

          /* if section address and len don't match, continue */
          if ((sec->lma + sec_len + merge_len) != next_sec->lma) continue;

          p = strchr(sec->name, '%');
          p2 = strchr(next_sec->name, '%');
          if (p && p2) {
            len = p - sec->name;
            if (len != (unsigned) (p2 - next_sec->name))
              continue;
            match = (strncmp(sec->name, next_sec->name, len) == 0);
          } else
            match = (strcmp(sec->name, next_sec->name) == 0);

          if (match && !config.unique_orphan_sections &&
              !pic32_unique_section(sec->name) &&
              (pic32_attribute_map(sec) == pic32_attribute_map(next_sec))) {

            if (pic32_debug) {
              printf("  Merging output sections %s and %s\n",
                      sec->name, next_sec->name);
              printf("    %s: addr = %lx, len = %lx\n",
                     sec->name, sec->lma, sec_len + merge_len);
              printf("    %s: addr = %lx, len = %lx\n",
                     next_sec->name, next_sec->lma, next_len);
            }

            merge_sec = 1; /* set a flag to indicate we're merging */
            merge_len += next_len;
            next->output_section_statement.children.head->
              input_section.section->output_section = NULL;

            lang_add_section (&os->output_section_statement.children,
                              next->output_section_statement.children.head
                              ->input_section.section,
                              NULL,
                              &os->output_section_statement);

            /* remove the merged section from output_bfd */
            remove_section_from_bfd(link_info.output_bfd, next_sec);
          }
        }
      }

      if (merge_sec) {
        os->header.next = next->header.next;  /* unlink the merged statement */
        next = os;                            /* try to merge another one */
      }
    }
  }
    
  ///\ after allocating memory, update __shared_dinit_addr
  ///\ needed only in coresident context
    if (init_shared_template != NULL)
    update_shared_dinit_addr();

} /* allocate_memory() */


/*
 * allocate_program_memory()
 *
 * This function attempts to allocate program memory sections.
 *
 * Called by: allocate_memory()
 *
 * Calls:     build_alloc_section_list()
 *            build_free_block_list()
 *            locate_sections()
 *            pic32_section_list_length()
 *
 * Returns:   status code
 *            (0 = success)
 */
static int
allocate_program_memory() {
  struct memory_region_struct *region;
  unsigned int mask = code;
  int result = 0;

  if (pic32_debug)
    printf("\nBuilding allocation list for region \"program\"\n"
           "  attribute mask = %x\n", mask);

  build_alloc_section_list(mask);
  if (pic32_section_list_length(alloc_section_list) == 0)
    return result;

  region = region_lookup ("kseg0_program_mem");
  build_free_block_list(region, mask);

  if (pic32_debug) {
    pic32_print_section_list(alloc_section_list, "allocation");
  }

#if 0
  if (pic32_debug)
    pic32_print_section_list(unassigned_sections, "unassigned");
#endif

  reset_locate_options();
  result |= locate_sections(address, 0, region);   /* most restrictive  */
  result |= locate_sections(all_attr, 0, region);  /* least restrictive */

  /* save the free blocks list */
  program_memory_free_blocks = free_blocks;
  free_blocks = 0;
    
  return result;
} /* allocate_program_memory() */

static int
allocate_serial_memory() {
  struct memory_region_struct *region;
  unsigned int mask = serial_mem;
  int result = 0;

  if (pic32_debug)
    printf("\nBuilding allocation list for region \"serial memory\"\n"
           "  attribute mask = %x\n", mask);

  build_alloc_section_list(mask);

  if (pic32_section_list_length(alloc_section_list) == 0)
    return result;

  region = region_lookup ("serial_mem");
  build_free_block_list(region, mask);

  if (pic32_debug) {
    pic32_print_section_list(alloc_section_list, "allocation");
  }

#if 0
  if (pic32_debug)
    pic32_print_section_list(unassigned_sections, "unassigned");
#endif

  reset_locate_options();
  result |= locate_sections(address, 0, region);   /* most restrictive  */
  result |= locate_sections(all_attr, 0, region);  /* least restrictive */

  return result;
} /* allocate_serial_memory() */


/*
 * allocate_data_memory()
 *
 * This function attempts to allocate data memory sections.
 *
 * Called by: allocate_memory()
 *
 * Calls:     build_section_list()
 *            build_free_block_list()
 *            locate_sections()
 *
 * Returns:   status code
 *            (0 = success)
 *
 * Notes: List "data_memory_free_blocks" is needed by
 *        bfd_pic322_finish() to allocate the stack
 *        and heap, so don't exit this function early.
 *
 *        EDS allocation is tricky. Although these sections
 *        can be allocated anywhere, we need to preserve
 *        low memory for the stack. In the first pass,
 *        we exclude memory below the stack limit altogether.
 *        In the second pass, we scan free blocks in reverse
 *        only, which helps when a block crosses the
 *        stack limit boundary. Eventually we'll take
 *        whatever memory we need to, and leave the rest
 *        for the stack and heap.
 *
 *        We also support allocation of stack and heap
 *        that are defined with section attributes.
 *
 *        See comments below for additional info
 *        about stack allocation.
 */
static int
allocate_data_memory() {
  struct memory_region_struct *region = NULL;
  struct pic32_section *s;
  unsigned int mask = data|bss|persist|stack|heap|ramfunc;
  int result = 0;

  if (pic32_debug)
    printf("\nBuilding allocation list for region \"data\"\n"
           "  attribute mask = %x\n", mask);

  build_alloc_section_list(mask);

  if (pic32_is_l1cache_machine(global_PROCESSOR))
  {
    /* Try to use kseg0_data_mem, but fall back to kseg1_data_mem. */
    if (lang_memory_region_exist("kseg0_data_mem"))
      region = region_lookup ("kseg0_data_mem");
    else if (lang_memory_region_exist("kseg1_data_mem"))
      region = region_lookup ("kseg1_data_mem");
    else
      region = region_lookup ("kseg0_data_mem");
  }
  else
  {
    struct memory_region_struct *region1 = NULL, *region0 = NULL;
    /* Use the largest kseg0/1_data_mem region */
    if (lang_memory_region_exist("kseg1_data_mem"))
      region1 = region_lookup ("kseg1_data_mem");
    if (lang_memory_region_exist("kseg0_data_mem"))
      region0 = region_lookup ("kseg0_data_mem");
    
    if (region0 && region1)
    {
      if (pic32_debug)
        printf ("region0->length = %lx, region1->length = %lx\n");
      if ((region0->length) > (region1->length))
        region = region0;
      else
        region = region1;
    }
    else
    {
      if (region1)
        region = region1;
      else
        region = region0;
    }
  }
  if (region == NULL)
    einfo (_(" Link Error: kseg0_data_mem or kseg1_data_mem region must be defined for this device"));

  build_free_block_list(region, mask);

  if (pic32_debug) {
    pic32_print_section_list(alloc_section_list, "allocation");
  }
    
  /* lghica co-resident */
#if 1
    /*
     ** an initially empty list of blocks that can be shared in data memory
     */
    pic32_init_memory_list (&shared_data_memory_blocks);
#endif

  reset_locate_options();
  result |= locate_sections(ramfunc, 0, region);    /* most restrictive  */
  
  if (!bfd_pic32_is_defined_global_symbol("_ramfunc_begin"))
  {
    /* If there are no ram fumctions, add the _ramfunc_begin symbol with value 0 */
    _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_ramfunc_begin",
    BSF_GLOBAL, bfd_abs_section_ptr,
    0, "_ramfunc_begin", 1, 0, 0);
  }
  
  if (ramfunc_begin != 0) {
    set_locate_options(EXCLUDE_HIGH_ADDR, ramfunc_begin);
  }
  result |= locate_sections(address, 0, region);
    
  coherent_section_count = 0;
  result |= locate_coherent_sections(data, region);  // allocate coherent data sections
    
  coherent_section_count = 0;
  result |= locate_coherent_sections(bss, region);	 // allocate coherent bss sections
    
  coherent_section_count = 0;
  result |= locate_coherent_sections(persist, region);	 // allocate coherent persist sections
    
    
  result |= locate_sections(near, 0, region);       /* less restrictive  */
  result |= locate_sections(all_attr, stack|heap, region);

#if 0
  /* user-defined heap */
  if (ramfunc_begin != 0) {
    set_locate_options(EXCLUDE_HIGH_ADDR, ramfunc_begin);
  }
  result |= locate_sections(heap, 0, region);

  /* user-defined stack */
  if (ramfunc_begin != 0) {
    set_locate_options(EXCLUDE_HIGH_ADDR, ramfunc_begin);
  }
  result |= locate_sections(stack, 0, region);
#else
  /* Don't support user-defined stack sections yet */
  for (s = alloc_section_list; s != NULL; s = s->next) {
      /* lghica co-resident */
#if 1
      if(s && (s->sec != NULL) && (s->sec->linked))
          continue;
#endif
    if (s->sec && (PIC32_IS_STACK_ATTR(s->sec) || PIC32_IS_HEAP_ATTR(s->sec))) {
      lang_output_section_statement_type *os;
      s->sec->lma = 0;
      s->sec->vma = 0;
      os = lang_output_section_statement_lookup (".stack", 0, TRUE);
      /* lang_add_section() will call init_os() if needed */
      lang_add_section (&os->children, s->sec, NULL, os);
      finish_section_info(s, os);
      pic32_remove_from_section_list(alloc_section_list,s);
      os->bfd_section->flags = s->sec->flags;
    }
  }

#endif

  /* if any sections are left in the allocation list, report an error */
  for (s = alloc_section_list; s != NULL; s = s->next) {
  if ((s->attributes != 0) && (s->sec->linked == 0) /* lghica co-resident*/)
  {
    report_allocation_error(s);
    result = 1;
    break;
  }
  }

  /* save the free blocks list */
  data_memory_free_blocks = free_blocks;
  free_blocks = 0;

  return result;
} /* allocate_data_memory() */

static bfd_vma
group_section_size(struct pic32_section *g)
{
  struct pic32_section *s,*next;
  bfd_vma result = 0;

  for (s = g; s != NULL; s = next) {
    next = s->next;
    if (s->sec == 0)
      continue;
    if (s->sec && (PIC32_IS_RAMFUNC_ATTR(s->sec)))
      result += s->sec->size;
  }
  return result;
}

#define OUTSIDE_REGION(addr, len, region)          \
  (((addr + len) <= region->origin)                \
   || (addr >= (region->origin + region->length)))

#define ACROSS_REGION(addr, len, region)                            \
  (((addr < region->origin) && ((addr + len) > region->origin)) ||  \
   ((addr < (region->origin + region->length)) &&                   \
    ((addr + len) > (region->origin + region->length))))


/*
 * locate_group_section()
 *
 * Helper function to locate a grouped section
 * using the free_blocks list.
 *
 * Called by: locate_sections()
 *
 * Returns:   status code
 *            (0 = success)
 */

 static int
 locate_group_section(struct pic32_section *s,
                        struct memory_region_struct *region) {

  struct pic32_memory *b;
  bfd_vma len = group_section_size(s);
  bfd_vma addr = s->sec->lma;
  int result = 0;

  /* DEBUG */
  if (pic32_debug)
    printf("  group section \"%s\", total size = %lx\n",
           s->sec->name, len);

  /* look for tricky user error */
  if (PIC32_IS_ABSOLUTE_ATTR(s->sec) && ACROSS_REGION(addr, len, region))
        einfo(_(" Link Warning: absolute section \'%s\' crosses"
                " the boundary of region %s.\n"),
              s->sec->name, region->name_list.name);

  if (len == 0)
    update_group_section_info(0,s,region); /* trivial case */
  else if (PIC32_IS_ABSOLUTE_ATTR(s->sec) &&
           OUTSIDE_REGION(addr, len, region)) {
    update_group_section_info(addr, s, region);  /* falls outside region */
  }
  else {                          /* locate using free_blocks list */
    b = select_free_block(s, len);
    if (b) {
      addr = b->addr + b->offset;
      /* Translate the address from kseg0 to kseg1 */
      if (PIC32_IS_COHERENT_ATTR(s->sec))
        addr |= 0x20000000u;
      update_group_section_info(addr,s,region);
      create_remainder_blocks(free_blocks,b,len);
      remove_free_block(b);
    } else {
      if (locate_options != NO_LOCATE_OPTION) {
        if (pic32_debug)
          printf("    \"%s\" location declined\n", s->sec->name);
        return 0;
      }
      result |= 1;
    }
  }

  if (pic32_debug)
    printf("    removing group from allocation list\n");
  pic32_remove_group_from_section_list(alloc_section_list);

  return result;
} /* locate_group_section() */

/*
 * get_section_duplicate(asection *)
 *
 * Helper function to locate a section with a given name
 *      - it is called only for specific pic32 sections
 *      - In an application linked in one step this function should never get called
 *      - added for coresident feature, for multiple steps linking
 *
 * Called by: locate_single_section()
 *
 * Returns:  the section with the specified name, or NULL
 */
static asection* get_section_duplicate(asection * const sec_orig)
{
    asection *sec = NULL;
    
    /* find an output section with the same name */
    sec = bfd_get_section_by_name(link_info.output_bfd, sec_orig->name);
    
    if ((sec!= NULL) && (sec_orig != sec))
    {
        if (PIC32_IS_ABSOLUTE_ATTR(sec))
            return sec_orig;
        else
            return sec;
    }
    return NULL;
}

/*
 * locate_single_section()
 *
 * Helper function to locate a single section
 * using the free_blocks list.
 *
 * Called by: locate_sections()
 *
 * Returns:   status code
 *            (0 = success)
 */

 static int
 locate_single_section(struct pic32_section *s,
                       struct memory_region_struct *region) {

  struct pic32_memory *b;
  bfd_vma len = s->sec->rawsize? s->sec->rawsize : s->sec->size;
  bfd_vma addr = s->sec->lma;
  int result = 0;
     
     
     if (pic32_debug && s->sec)
     {
         printf ("  %s with len %d & addr %x\n", s->sec->name, len, addr);
     }
     
     /* lghica co-resident*/
     asection* dup_sec;

     ///\ lookup in the input sections if there is another one with the same name
     if (s->sec && ( ( (strcmp(s->sec->name, ".text.version") == 0)
                        || (strcmp(s->sec->name, ".tlb_init_values") == 0)
                        || (strcmp(s->sec->name, ".simple_tlb_refill_excpt") == 0)
                        || (strcmp(s->sec->name,".cache_err_excpt")== 0)
                        || (strcmp(s->sec->name,".app_excpt")== 0)
                        || (strcmp(s->sec->name,".bev_excpt")== 0)
                        || (strstr(s->sec->name, "vector_offset") != NULL)
                      )
                    && (dup_sec = get_section_duplicate(s->sec))))
     {
         ///\ if the other section is allocated, just link to it
         s->sec->flags |= SEC_EXCLUDE;

         update_section_info(0, s, region);
         pic32_remove_from_section_list(alloc_section_list,s);
         return 0;
     }
     else if (s->sec && (strcmp(s->sec->name, ".text.version") == 0))
     {
         s->sec->flags |= SEC_EXCLUDE;
         pic32_remove_from_section_list(alloc_section_list,s);
         return 0;
     }
     
  /* look for tricky user error */
  if (PIC32_IS_ABSOLUTE_ATTR(s->sec) && ACROSS_REGION(addr, len, region))
        einfo(_(" Link Warning: absolute section \'%s\' crosses"
                " the boundary of region %s.\n"),
              s->sec->name, region->name_list.name);
  
  if (len == 0)
    update_section_info(0,s,region);       /* trivial case */
  else if (PIC32_IS_ABSOLUTE_ATTR(s->sec) &&
           OUTSIDE_REGION(addr, len, region)) {
    update_section_info(addr, s, region);  /* falls outside region */
  }
  else{                          /* locate using free_blocks list */
    b = select_free_block(s, len);
    if (b) {
      addr = b->addr + b->offset;
      if (PIC32_IS_COHERENT_ATTR(s->sec))
        addr |= 0x20000000;
#if 1
        /* lghica co-resident */
      if (PIC32_IS_SHARED_ATTR(s->sec) && PIC32_IS_ABSOLUTE_ATTR(s->sec)
               && (s->sec->linked == 1))
      {
          struct pic32_memory * mem_reg;
          
          mem_reg = pic32_add_shared_to_memory_list(shared_data_memory_blocks,
                                            b->addr+b->offset, len, s->sec);
          
          if (pic32_debug)
              printf("LG --- SECTION in shared_data_memory %s (0x%x - 0x%x)\n",
                     s->sec->name, mem_reg->addr, mem_reg->size);
      }
#endif
      update_section_info(addr,s,region);
      create_remainder_blocks(free_blocks,b,len);
      remove_free_block(b);
    } else {
      if (locate_options != NO_LOCATE_OPTION) {
        if (pic32_debug)
          printf("    \"%s\" location declined\n", s->sec->name);
        return 0;
      }
      result |= 1;
    }
  }

  if (pic32_debug)
    printf("    removing from allocation list\n");
  pic32_remove_from_section_list(alloc_section_list,s);

  return result;
} /* locate_single_section() */


/*
 * locate_sections()
 *
 * This function attempts to locate all sections
 * in the section list that match a specified
 * attribute mask. The block argument specifies
 * attributes that disqualify a section.
 *
 * Called by: allocate_program_memory()
 *            allocate_data_memory()
 *            allocate_eedata_memory()
 *            allocate_auxflash_memory()
 *
 * Calls:     select_free_block()
 *            update_section_info()
 *            create_remainder_blocks()
 *            remove_free_block()
 *            remove_alloc_section()
 *
 * Returns:   status code
 *            (0 = success)
 */
static int
locate_sections(unsigned int mask, unsigned int block,
                struct memory_region_struct *region) {
  struct pic32_section *s,*next;
  int result = 0;
  static int ramfunc_section_count = 0;

  if (pic32_debug) {
    printf("\nLocating sections with mask %x, but not %x\n", mask, block);
    printf("  locate_options = %x, exclude_addr = %lx\n",
           locate_options, exclude_addr);
  }

  for (s = alloc_section_list; s != NULL; s = next) {

      next = s->next;
      /* lghica co-resident */
#if 1
      if (s->sec && (PIC32_IS_DATA_ATTR(s->sec) || PIC32_IS_BSS_ATTR(s->sec))
            && !PIC32_IS_ABSOLUTE_ATTR(s->sec) && (s->sec->linked == 1))
      {
          update_section_info(s->sec->lma, s, region);
      }
#endif
      
    if (s->sec && (s->attributes & mask) &&
        ((s->attributes & block) == 0)) {
      bfd_vma len = s->sec->rawsize? s->sec->rawsize: s->sec->size;

      if (pic32_debug) {
        char *attr = pic32_section_attr_string(s->sec);

        printf("  b \"%s\", len = %lx, %s\n",
               s->sec->name, (unsigned long)len, attr ? attr : "");
        if (attr) free(attr);
      }

      if (PIC32_IS_RAMFUNC_ATTR(s->sec)) {
          if (ramfunc_section_count == 0) {
              set_locate_options(FAVOR_HIGH_ADDR,0);
              s->sec->alignment_power = 11;
              ramfunc_section_count++;
              result |= locate_group_section(s, region);
              if (s->sec->vma == 0)
                {
                  report_allocation_error(s);
                }
              ramfunc_begin = s->sec->vma;
              if (!bfd_pic32_is_defined_global_symbol("_ramfunc_begin")) {
                _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_ramfunc_begin",
                                    BSF_GLOBAL, bfd_abs_section_ptr,
                                    ramfunc_begin, "_ramfunc_begin", 1, 0, 0);
              } else {
                einfo(_("Link Warning: Cannot define '_ramfunc_begin' due to existing definition; check linker script\n"));
              }
              if (pic32_debug)
                printf("  _ramfunc_begin = %x\n", (unsigned int)ramfunc_begin);
              if (!bfd_pic32_is_defined_global_symbol("_bmxdkpba_address")) {
                _bfd_generic_link_add_one_symbol (&link_info, link_info.output_bfd, "_bmxdkpba_address",
                                    BSF_GLOBAL, bfd_abs_section_ptr,
                                    ramfunc_begin - region->origin, "_bmxdkpba_address", 1, 0, 0);
              } else {
                einfo(_("Link Warning: Cannot define '_bmxdkpba_address' due to existing definition; check linker script\n"));
              }
              if (pic32_debug)
                printf("  _bmxdkpba_address = %x\n", (unsigned int)(ramfunc_begin - region->origin));
            }
        }
      else {
          result |= locate_single_section(s, region);
        }
      }
  }
  return result;
} /* locate_sections() */


/*
 * next_aligned_address()
 */
static bfd_vma
next_aligned_address( bfd_vma start, unsigned int align_power) {

  bfd_vma mask = ~((~0) << align_power);
  return (bfd_vma) ((start + 1 + mask) & (~mask));
}


/*
 * prev_aligned_address()
 */
static bfd_vma
prev_aligned_address( bfd_vma start, unsigned int align_power) {

  bfd_vma mask = ~((~0) << align_power);
  if (start < mask)
    return (bfd_vma) 0;
  else
    return (bfd_vma) ((start - 1 - mask) & (~mask));
}


/*
 * select_free_block()
 *
 * This function attempts to select a suitable
 * free block for a specified section. For each
 * free block, the section is positioned two ways:
 *  (1) as close to the starting address as possible,
 *  (2) as close to the ending address as possible.
 *
 * The position that leaves the largest unused
 * remainder space is selected. If a suitable free
 * block is found, the block offset is written into
 * the free block structure and the structure is returned.
 *
 * The selection algorithm can be fine-tuned by previous
 * calls to set_locate_options().
 *
 * Called by: locate_sections()
 *            locate_group_sections()
 *
 * Calls:     pic32_static_assign_memory()
 *            next_aligned_address()
 *            prev_aligned_address()
 *
 * Returns:   suitable free block, or 0
 */

#if 0
static void
confirm_dma_range_defined() {
  struct bfd_link_hash_entry *h;
  static bfd_boolean base_err_reported = FALSE;
  static bfd_boolean end_err_reported = FALSE;

  if ((!dma_base_defined) || (!dma_end_defined)) {

    h = bfd_pic32_is_defined_global_symbol(str3);
    if (h) {
      dma_base = h->u.def.value;
      dma_base_defined = TRUE;
    }
    else {
      if (!base_err_reported) {
        einfo(_(str1), str3);
        base_err_reported = TRUE;
      }
    }

    h = bfd_pic32_is_defined_global_symbol(str4);
    if (h) {
      dma_end = h->u.def.value;
      dma_end_defined = TRUE;
    }
    else {
      if (!end_err_reported) {
        einfo(_(str1), str4);
        end_err_reported = TRUE;
      }
    }
  }
}
#endif


static struct pic32_memory *
select_free_block(struct pic32_section *s, unsigned int len) {

  unsigned int align_power = s->sec->alignment_power;

  const char *err_str1 = "Link Error: Could not allocate section";

  struct pic32_memory *b = 0;
  bfd_vma option1, limit1;
  bfd_vma option2, limit2;
  bfd_boolean option1_valid, option2_valid;
/* lghica co-resident */
    unsigned int    opb = bfd_octets_per_byte (link_info.output_bfd);
    bfd_vma         old_option1, new_option1;
    bfd_vma         old_option2, new_option2;
    
    
  /*
   * If the section is absolute, call the static function
   */
  if (PIC32_IS_ABSOLUTE_ATTR(s->sec))
  {
          b = pic32_static_assign_memory(free_blocks, len, s->sec->lma);
    if (!b) {
        if ((s->sec->flags & SEC_NEVER_LOAD) ||
            (command_line.check_section_addresses == FALSE))
          /* OK, don't report as error */;
        else
        {
            einfo(_("%X %s \'%s\' at 0x%v SIZE %d\n"),
                err_str1, s->sec->name, s->sec->lma, s->sec->size);
        }
        return (struct pic32_memory *) NULL;
    } else
      return b;
  }
#if 1
    /* lghica co-resident shared but not linked yet!!! -  to check*/
   else if (PIC32_IS_SHARED_ATTR(s->sec) && !PIC32_IS_ABSOLUTE_ATTR(s->sec))
    {
        if (pic32_debug && (s->sec->linked == 0))
            printf("LG - is shared & abs %s\n", s->sec->name);
        b = pic32_assign_shared_memory(shared_data_memory_blocks, len, s);
        if (b)
        {
            s->sec->lma = b->addr;
            return b;
        }
    }
#endif

#if 0
  /*
   * If DMA attribute is set, make sure
   * we have a definition for __DMA_BASE and __DMA_END
   * Usually this appears in the linker script.
   */
  if (PIC32_IS_DMA_ATTR(s->sec)) {
    confirm_dma_range_defined();
  }
#endif

  /*
   * Loop through the free blocks list
   */
  for (b = free_blocks; b != NULL; b = b->next) {

    if ((b->addr == 0) && (b->size == 0))
      continue;

    if (pic32_debug)
      printf("    consider block at %lx, len = %lx\n", b->addr, b->size);

    /*
     * Qualify the block first, so we don't waste
     *  time scanning for alignment.
     */
    if (b->size < len)
      continue;


    /* qualify NEAR with leftmost position in free block */
    if (PIC32_IS_NEAR_ATTR(s->sec) && !VALID_NEAR(b->addr, len))
      continue;

    /* qualify LOW_ADDR with leftmost position in free block */
    if (IS_LOCATE_OPTION(EXCLUDE_HIGH_ADDR) &&
        !VALID_LOW_ADDR(b->addr, len))
      continue;

    /* qualify HIGH_ADDR with rightmost position in free block */
    if (IS_LOCATE_OPTION(EXCLUDE_LOW_ADDR) &&
        !VALID_HIGH_ADDR((b->addr + b->size - len), len))
      continue;

    /* fall through if block passes pre-qual tests */

    /* Compute option1: scan from the start of the block */
    option1 = b->addr - 1 ;
    limit1  = b->addr + (b->size - len);
    option1_valid = TRUE; /* set default */
    if (pic32_debug)
      printf("    option1 [advancing from %lx]\n", option1 + 2);
    while (1) {

      if (IS_LOCATE_OPTION(FAVOR_HIGH_ADDR)) {
        option1_valid = FALSE;
        break;
      }

      /* find the next aligned address */
      if (PIC32_IS_REVERSE_ATTR(s->sec)) {
        option1 = next_aligned_address(option1 + len, align_power);
        option1 -= len;
      }
      else
        option1 = next_aligned_address(option1, align_power);

      if (option1 > limit1) {
        option1_valid = FALSE;
        break;
      }


      if (pic32_debug)
        printf("    option1 aligned at %lx\n", option1);

      /* aligned address is valid, check other attributes */


        if (PIC32_IS_NEAR_ATTR(s->sec)  && !VALID_NEAR(option1, len)) {
        option1_valid = FALSE;  /* scanning forward won't help */
        break;
      }
        
      /* lghica co-resident */
#if 1
        /* this check is necessary for Co-resident applications */
        /* to make sure persistent data is not over written at any time */
        
        if (PIC32_IS_PERSIST_ATTR(s->sec) || PIC32_IS_SHARED_ATTR(s->sec))
        {
            old_option1 = option1;
            new_option1 = option1;
            struct pic32_section *ls;
            for (ls = alloc_section_list; ls != NULL; ls = ls->next)
            {
                if (ls->sec && (ls->sec->linked == 1))
                {
                    if (((ls->sec->lma <= old_option1)
                         && (old_option1 < (ls->sec->lma + (ls->sec->rawsize / opb)))))
                    {
                        new_option1 = ls->sec->lma + (ls->sec->rawsize / opb);
                        break;
                    }
                }
            }
            if (new_option1 != old_option1)
                continue;
            else
                option1= new_option1;
        }
#endif
        
      if (IS_LOCATE_OPTION(EXCLUDE_LOW_ADDR) &&
          !VALID_HIGH_ADDR(option1, len)) {
        option1 = exclude_addr - 1;  /* skip ahead */
        if (pic32_debug)
          printf("    approaching EXCLUDE boundary from %lx\n", option1);
        continue;
      }

      /* if we get here, the option is valid */
      break;
    } /* while (!done) */

    /* Compute option2: scan from the end of the block       */
    /*                  unless the section is BOOT or SECURE */
    option2 = limit1 + 1;
    limit2  = b->addr;
      option2_valid = TRUE; /* set default */
      if (pic32_debug)
        printf("    option2 [descending from %lx]\n", option2 - 2);
      while (1) {

        if (IS_LOCATE_OPTION(FAVOR_LOW_ADDR)) {
          option2_valid = FALSE;
          break;
        }

        /* find the next aligned address */
        if (PIC32_IS_REVERSE_ATTR(s->sec)) {
          option2 = prev_aligned_address(option2 + len, align_power);
          option2 -= len;
        }
        else
          option2 = prev_aligned_address(option2, align_power);

        if (option2 < limit2) {
          option2_valid = FALSE;
          break;
        }

        if (pic32_debug)
          printf("    option2 aligned at %lx\n", option2);

          /* lghica co-resident */
#if 1
          /* this check is necessary for Co-resident applications */
          /* to make sure persistent data is not over written at any time */
          if (PIC32_IS_PERSIST_ATTR(s->sec) || PIC32_IS_SHARED_ATTR(s->sec)) {
              old_option2 = option2;
              new_option2 = option2;
              struct pic32_section *ls;
              for (ls = alloc_section_list; ls != NULL; ls = ls->next)
              {
                  if (ls->sec && (ls->sec->linked == 1))
                  {
                      if (((ls->sec->lma <= old_option2)
                           && (old_option2 < (ls->sec->lma + (ls->sec->rawsize / opb)))))
                      {
                          new_option2 = ls->sec->lma - len;
                          break;
                      }
                  }
              }
              if (new_option2 != old_option2) 
                  continue;
              else
                  option2= new_option2;
          }
#endif
        if  (PIC32_IS_NEAR_ATTR(s->sec)    && !VALID_NEAR(option2, len)) {
          option2 = NEAR_BOUNDARY - len + 1;  /* skip back */
          if (pic32_debug)
            printf("    approaching Near boundary from %lx\n", option2);
          continue;
        }


        if (IS_LOCATE_OPTION(EXCLUDE_HIGH_ADDR) &&
            !VALID_LOW_ADDR(option2, len)) {
          option2 = exclude_addr - len + 1;  /* skip back */
          if (pic32_debug)
            printf("    approaching EXCLUDE boundary from %lx\n", option2);
          continue;
        }

        /* if we get here, the option is valid */
        break;
      } /* while (!done) */

    if ((option1_valid == FALSE) && (option2_valid == FALSE))
      continue;  /* try the next free block */

    if (pic32_debug) {
      printf("    ");
      if (option1_valid)
        printf("option1 = %lx, ", option1);
      if (option2_valid)
        printf("option2 = %lx", option2);
      printf("\n");
    }

    /* we have 1 or 2 valid options */
    if (option1_valid == FALSE)
      b->offset = option2 - b->addr;
    else if (option2_valid == FALSE)
      b->offset = option1 - b->addr;
    else {
      /* compare the two "outer" remainders */
      bfd_vma rem1 = option1 - b->addr;
      bfd_vma rem2 = (b->addr + b->size) - option2 - len;

      b->offset = (rem1 > rem2) ?
                  (option2 - b->addr) : (option1 - b->addr);
    }

    return b;
  } 

  /* If we get here, a suitable block could not be found */
  if (locate_options != NO_LOCATE_OPTION)
    return (struct pic32_memory *) NULL; /* exit quietly */
  else
    report_allocation_error(s);

#if 0
  {
#define PREFIX "/tmp"
    char *colon = "";
    char *filename = "";
    char *secname = "*";
    char *s1 = pic32_section_size_string(s->sec);
    char *s2 = pic32_section_attr_string(s->sec);

    /* report the file name, unless its a temp file */
    if (strncmp(s->sec->owner->filename, PREFIX, strlen(PREFIX)) != 0) {
      colon = ":";
      filename = (char *) s->sec->owner->filename;
    }

    /* report the section name, unless its based on a temp file */
    if (strncmp(s->sec->name, PREFIX, strlen(PREFIX)) != 0)
      secname = (char *) s->sec->name;

    einfo(_("%X%s%s Link Error: Could not allocate section %s, %s, %s\n"),
          filename, colon, secname, s1 ? s1 : "", s2 ? s2 : "");

    if (s1) free(s1);  /* free the malloc'ed strings */
    if (s2) free(s2);

    if (pic32_debug)
      printf("\n    Error: Could not allocate section %s\n", s->sec->name);
  }
#endif

  return (struct pic32_memory *) NULL;
} /* select_free_block() */

static char *
unique_section_name(const char *s) {
  static int auto_sec_cnt = 0;
  char *name;
  int len;

  len = strlen(s) + 1 + 5;
  name = xmalloc (len + 1);
  snprintf(name, len, "%s%s%d", s, "%", ++auto_sec_cnt);
  return name;
}


static char *
unique_zero_length_section_name(const char *s) {
  static int auto_zero_sec_cnt = 0;
  char *name;
  int len;

  len = strlen(s) + 2 + 5;
  name = xmalloc (len + 1);
  snprintf(name, len, "%s%s%d", s, "%z", ++auto_zero_sec_cnt);
  return name;
}

/*
 * finish_section_info()
 *
 * called by:
 *  update_section_info()
 *  update_grouped_section_info()
 *
 * Note that we don't copy in the section size,
 * because lang_process() will calculate the
 * output section size after the emulation
 * returns control.
 */
static void
finish_section_info(struct pic32_section *s, lang_output_section_statement_type *os)
{
  etree_type *addr_tree;

  if (os->bfd_section == NULL) /* --gc-sections discarded this, so done */
    return;
  os->bfd_section->vma = s->sec->vma;
  os->bfd_section->lma = s->sec->lma;
  addr_tree = xmalloc(sizeof(etree_type));
  addr_tree->value.type.node_class = etree_value;
  addr_tree->value.value = s->sec->vma;
  os->addr_tree = addr_tree;

} /* finish_section_info() */


#define update_section_addr(s,addr)                     \
  {                                                     \
  s->lma = addr;                                        \
  s->vma = addr;                                        \
  }

/*
 * update_section_info()
 *
 * Update section info after allocation
 *
 * If section was a heap or stack, set some globals.
 *
 * Note: argument region is currently unused.
 *
 */
static void
update_section_info(bfd_vma alloc_addr,
                    struct pic32_section *s,
                    struct memory_region_struct *region) {

  lang_output_section_statement_type *os;
  char *name;

  update_section_addr(s->sec, alloc_addr);

  if (pic32_debug) {
    printf("    updating section info:"
           "  vma = %lx, lma = %lx\n", s->sec->vma, s->sec->lma);
  }

  /* create a unique name for the output section */
    name = (s->sec->size > 0) ?
    unique_section_name(s->sec->name) :
    unique_zero_length_section_name(s->sec->name);

  /* create an output section (statement) */
  os = lang_output_section_statement_lookup (name, 0, TRUE);


  if (pic32_debug)
    printf("    creating output section statement 2\"%s\"\n\n", os->name);

  /* lang_add_section() will call init_os() if needed */
  lang_add_section (&os->children, s->sec, NULL, os);

  finish_section_info(s, os);
    
    ///\lghica coresident - make sections absolute sections - always
    ///\ needed when bootloader is linked first
    {
            if(pic32_debug)
                printf("LG Section %s set abs & linked", s->sec->name);

            PIC32_SET_ABSOLUTE_ATTR(s->sec->output_section);
            s->sec->output_section->linked = 1;
    }
    
  region = region;
} /* update_section_info() */

/*
 * update_group_section_info()
 *
 * Update group section info after allocation
 *
 * Note: argument region is currently unused.
 */
static void
update_group_section_info(bfd_vma alloc_addr,
                    struct pic32_section *g,
                    struct memory_region_struct *region ATTRIBUTE_UNUSED) {

  struct pic32_section *s, *next;
  asection *sec;
  char *name;
  bfd_vma addr = alloc_addr;
  lang_output_section_statement_type *os;

  /* create a unique name for the output section, if necessary */
     sec = bfd_get_section_by_name(link_info.output_bfd, g->sec->name);
     if (sec) {
       name = ( g->sec->size > 0) ?
               unique_section_name(g->sec->name) :
               unique_zero_length_section_name(g->sec->name);
     }
     else {
       name = (char *) g->sec->name;
       if (PIC32_IS_RAMFUNC_ATTR(g->sec))
         strcpy(name, ".RAMFUNC$");
     }
  /* create an output section (statement) */
  os = lang_output_section_statement_lookup (name, 0, TRUE);
  if (pic32_debug)
    printf("    creating output section statement \"%s\"\n", os->name);

  /* loop through the input sections in this group */
  for (s = g; s != NULL; s = next) {
    next = s->next;
    if (s->sec && (PIC32_IS_RAMFUNC_ATTR(s->sec))) {
      update_section_addr(s->sec, addr);
      addr += (s->sec->size);

      /* lang_add_section() will call init_os() if needed */
      lang_add_section (&os->children, s->sec, NULL, os);

      if (pic32_debug)
        printf("    updating grouped section info:"
               "  vma = %lx, lma = %lx\n", s->sec->vma, s->sec->lma);
    }
  }
  finish_section_info(g, os);
} /* update_group_section_info() */


/*
 * create_remainder_blocks()
 *
 * Create [0,1,2] remainder blocks based on
 * block->offset and section->size.
 */
static void
create_remainder_blocks(struct pic32_memory *lst,
                        struct pic32_memory *b, unsigned int len) {

  bfd_vma remainder = b->size - (len + b->offset);

  if (pic32_debug)
    printf("    creating remainder blocks: %lx, %lx\n",
           b->offset, remainder);

  if (b->offset > 0)  /* gap at beginning */
    pic32_add_to_memory_list(lst, b->addr, b->offset);

  if (remainder > 0)  /* gap at end */
    pic32_add_to_memory_list(lst,
                             b->addr + b->offset + len, remainder);
} /* create_remainder_blocks() */


/*
 * insert_alloc_section()
 *
 * Insert a pic32_section into the list
 *  in order of decreasing size or alignment power.
 *
 * Note: a new copy of the pic32_section is made
 *
 */
static void
insert_alloc_section (struct pic32_section *lst,
                      struct pic32_section *as)
{
  struct pic32_section *new, *prev, *s;
  int done = 0;

  /* create a new element */
  new = ((struct pic32_section *)
         xmalloc(sizeof(struct pic32_section)));
  new->sec  = as->sec;
  new->attributes = as->attributes;
  new->file = as->file;

  /* insert it at the right spot */
  prev = lst;
  for (s = prev; s != NULL; s = s->next)
    {
      if (s->sec && ((new->sec->size > s->sec->size) ||
          ((bfd_vma)(1 << new->sec->alignment_power) > s->sec->size)) ) {
        prev->next = new;
        new->next = s;
        done++;
        break;
      }
      prev = s;
    }

  if (!done)
    {
      prev->next = new;
      new->next = NULL;
    }
} /* insert_alloc_section() */


/*
 * build_alloc_section_list()
 *
 * Build alloc_section_list from the
 * list of unassigned_sections, based on
 * an attribute mask.
 *
 * Sort the list in order of decreasing S,
 * where S is the greater of size and
 * alignment requirement.
 *
 * Note: If a section has length zero,
 * we allocate it anyway, although it
 * is stripped from the output file.
 * The linker needs to have an output
 * section statement created for it.
 *
 * Future enhancement:
 *   If --re-order-sections is set, also
 *   add explicity mapped output sections
 *   unless they have an absolute address.
 */
static void
build_alloc_section_list(unsigned int mask) {
  struct pic32_section *s, *prev, *next;

  if (alloc_section_list)
    pic32_free_section_list(&alloc_section_list);

  pic32_init_section_list(&alloc_section_list);

  /* insert the unassigned sections */
  prev = unassigned_sections;
  for (s = prev; s != NULL; s = next) {

    next = s->next;
    /* Don't add gc-sections to the alloc list */
    if (s->sec && (s->sec->flags & SEC_EXCLUDE))
      continue;
    if (s->attributes & mask) {
      if (pic32_debug)
        printf("  input section \"%s\", len = %lx, flags = %x, attr = %x\n",
               s->sec->name, s->sec->rawsize? s->sec->rawsize :
               s->sec->size, s->sec->flags, s->attributes);
        insert_alloc_section(alloc_section_list, s);
        prev->next = next; /* unlink it from unassigned_sections */        
    } else
      prev = s;
  }
} /* build_alloc_section_list() */


/*
 * build_free_block_list()
 *
 * Build a list of free memory blocks
 * in a specified region.
 *
 * Sort the list in order of increasing size.
 *
 * Future enhancement:
 *   If --re-order-sections is set, add free blocks
 *   for explicity mapped output sections unless
 *   they have an absolute address.
 */
static void
build_free_block_list(struct memory_region_struct *region,
                      unsigned int mask ATTRIBUTE_UNUSED) {
  int cnt = 0;
  bfd_vma len, dot, limit, ma;
  struct pic32_section *s;

  if (free_blocks)
    pic32_free_memory_list(&free_blocks);

  pic32_init_memory_list(&free_blocks);

  if (pic32_debug)
    printf("\nBuilding free block list for region \"%s\"\n"
           "  origin = %lx, length = %lx, current = %lx\n",
           region->name_list.name, region->origin, region->length, region->current );

  /* find any gaps left by sequential allocator */
        dot = region->origin;
    
  limit = dot + region->length;
  for (s = pic32_section_list; s != NULL; s = s->next) {

      if (pic32_debug && s->sec)
        printf("    Checking (section \"%s\", lma = %lx, vma = %lx, len = %lx, attr = %x)\n",
               s->sec->name, s->sec->lma, s->sec->vma,
               s->sec->rawsize? s->sec->rawsize : s->sec->size,
               s->attributes);

    if (s->sec && (s->use_vma==0) &&
        (s->sec->lma >= region->origin) &&
           (s->sec->lma <= (region->origin + region->length)))
        ma = s->sec->lma;
    else if (s->sec && (s->use_vma) &&
        (s->sec->vma >= region->origin) &&
           (s->sec->vma <= (region->origin + region->length)))
        ma = s->sec->vma;
    else
        ma = 0;

    if ((s->sec) && ((s->sec->rawsize | s->sec->size) > 0) && ma) {
      if (pic32_debug)
        printf("    (section \"%s\", addr = %lx, len = %lx, attr = %x)\n",
               s->sec->name, ma,
               s->sec->rawsize? s->sec->rawsize : s->sec->size,
               s->attributes);
      if (ma == dot)                   /* no gap, just advance dot */
        dot += s->sec->rawsize? s->sec->rawsize : s->sec->size;
      else if (ma >= limit)   /* section belongs to another region */
        continue;
      else if (ma > dot) {               /* a gap preceeds section */
        len = ma - dot;                          /* compute length */
        if (pic32_debug)
          printf("  block %d, addr = %lx, len = %lx\n",
                 ++cnt, dot, len);
        pic32_add_to_memory_list(free_blocks, dot, len);  /* add free block */
        dot += len + (s->sec->rawsize? s->sec->rawsize : s->sec->size);           /* advance dot    */
      }
    }
  }

  /* add a block for any free space remaining in this region */
  /* .. use dot, because region->current may have been set for CodeGuard */
  len = region->length - (dot - region->origin);
  if (len > 0) {
    if (pic32_debug)
      printf("  block %d, addr = %lx, len = %lx\n",
             ++cnt, dot, len);
    pic32_add_to_memory_list(free_blocks, dot, len);
  }
} /* build_free_block_list() */


/*
 * remove_free_block()
 *
 * Remove an item from the free memory blocks list.
 *
 */
static void
remove_free_block(struct pic32_memory *b) {
  pic32_remove_from_memory_list(free_blocks, b);
}


/*
 * allocate_default_stack()
 *
 * If a stack section was not explicitly defined,
 * and the stack init symbols were not defined,
 * then this function is called to allocate the
 * largest stack possible from data_memory_free_blocks.
 *
 * Called by: bfd_pic32_finish()
 *
 * Calls:     pic32_add_to_memory_list()
 *            pic32_remove_from_memory_list()
 *
 * Returns:   sets global vars stack_base, stack_limit
 *
 * Notes: The free blocks list is sorted in order
 *        of increasing size
 *
 *        The calling function will confirm that
 *        the resulting stack satisfies the minimum
 *        size requirements.
 *
 *
 */

static void
allocate_default_stack() {
  struct pic32_memory *big_block = 0;
  struct pic32_memory *b, *next;
  bfd_vma under,over;
#define KSEG1_DATA_MEM_LIMIT 0xFFFFFFFFul;
  bfd_vma max_addr = KSEG1_DATA_MEM_LIMIT;

  /* if a free block straddles the upper limit, divide it */
  for (b = data_memory_free_blocks; b != NULL; b = next) {
    next = b->next;
    if ((b->addr < max_addr) && ((b->addr + b->size) > max_addr)) {

      if (pic32_debug)
        printf("  free block at %lx crosses upper stack limit (%lx)\n",
               b->addr, max_addr);

      under = max_addr - b->addr;
      over  = (b->addr + b->size) - max_addr;

      pic32_add_to_memory_list(data_memory_free_blocks, b->addr, under);
      pic32_add_to_memory_list(data_memory_free_blocks, max_addr, over);
      pic32_remove_from_memory_list(data_memory_free_blocks, b);
      break;
    }
  }

  /* find the largest block that qualifies */
  for (b = data_memory_free_blocks; b != NULL; b = next) {
    if (b->addr < max_addr)
      big_block = b;
    next = b->next;
  }

  if ((int)(pic32_heap_size) >
      (int)big_block->size) {
    heap_base = (big_block->addr + 1);
    heap_limit = (heap_base + (big_block->size) - 0x80u);
    einfo("%P%X Error: Not enough memory for heap"
          " (%d bytes needed, %d bytes available)\n",
            (int)(pic32_heap_size),
            (int)(heap_limit - heap_base));
  } else {
    heap_base = (big_block->addr + 1);
    heap_limit = (heap_base + pic32_heap_size);
  }
  while (heap_base % 8 != 0)
    heap_base++;
  while (heap_limit % 8 != 0)
    heap_limit++;

  stack_base  = (heap_limit + 1 + pic32_stackguard_size);
  stack_limit = (heap_base + big_block->size
                 - pic32_stackguard_size + 1);
  /* Stack must be aligned by 8 */
  while (stack_base % 8 != 0)
    stack_base++;
  while (stack_limit % 8 != 0)
    stack_limit--;

  if (pic32_debug) {
    printf("  selecting block at %lx\n", big_block->addr);
    printf("  heap base = %x, heap limit = %x\n", heap_base, heap_limit);
    printf("  stack base = %x, stack limit = %x\n", stack_base, stack_limit);
  }

  /* remove the block that we just used */
  pic32_remove_from_memory_list(data_memory_free_blocks, big_block);
}




static int
compare_memory_region_origin (const void *a, const void *b)
{
  const asection *sec1 = *(const asection **) a;
  const asection *sec2 = *(const asection **) b;

 
  /* If there's no section for some reason, compare equal.  */
  if (!sec1 || !sec2)
    return 0;
  
  if (sec1->vma < sec2->vma)
    return -1;
  else if (sec1->vma > sec2->vma)
    return 1;
  
  return 0;
}

static int
check_overlapping_memory_regions (asection **asec_list, unsigned int asec_count)
{
  int i ;
  asection* a = NULL ;
  asection* b = NULL ;
  const char *asec_name = NULL ;
  const char *bsec_name = NULL ;
  
  for (i=0; i<(asec_count-1); i++)
  {
    a = asec_list[i];
    b = asec_list[i+1];
    
    if ( (a->vma + a->lma) >= b->vma )
    {
        asec_name = a->name + strlen(memory_region_prefix) + 1;
        bsec_name = b->name + strlen(memory_region_prefix) + 1;

        einfo(_(" Link Error: user defined memory region \'%s\' crosses"
                " the boundary of region '%s'.\n"),
                asec_name, bsec_name);
    }
  }
}


static int 
validate_memory_region_list()
{
  /* Build a sorted list of memory regions, then use that to validate */
  struct pic32_section *r, *rnext;
  unsigned int list_size = 10;
  unsigned int sec_count = 0;

  asection **sec_list = (asection **)
    xmalloc (list_size * sizeof (asection *));

  for (r = memory_region_list; r != NULL; r = rnext) 
  {
    rnext = r->next;
    asection *asec = r->sec;
    if (asec) 
    {
      if (sec_count == list_size) 
      {
        list_size *= 2;
        sec_list = (asection **) 
        xrealloc (sec_list, list_size * sizeof (asection *));
      }

      sec_list[sec_count++] = asec;
    }
  }

  qsort (sec_list, sec_count, sizeof (asection *), &compare_memory_region_origin);
  check_overlapping_memory_regions(sec_list, sec_count);
  free (sec_list);

  return 0;
}



/*
 * allocate_user_memory()
 *
 * This function attempts to allocate sections
 * in user-defined memory regions.
 *
 * Called by: allocate_memory()
 *
 * Calls:     build_section_list()
 *            build_free_block_list()
 *            locate_sections() 
 *
 * Returns:   status code
 *            (0 = success)
 */
static int
allocate_user_memory() {
  struct memory_region_struct temp_region;
  struct pic32_section *r, *rnext, *s, *snext, *ss, *ssnext, *ssprev;
  const char *region_name;
  int result = 0;

  /* Validate that the memory region list don't overlapping*/
  validate_memory_region_list();

  /* loop through any user-defined regions */
  for (r = memory_region_list; r != NULL; r = rnext) {
  
    rnext = r->next;
    if (r->sec == 0) continue;
    
    region_name = r->sec->name + strlen(memory_region_prefix);
    if (pic32_debug)
      printf("\nBuilding allocation list for user-defined region \"%s\""
             " origin = %lx, length = %lx\n",
             region_name, r->sec->vma, r->sec->lma);

    /* the assembler will not accept a memory region
       definition with length 0... if we see one here,
       it indicates that a section has referenced a
       region that was never defined */
    if (r->sec->lma == 0)
      einfo(_("%s: Link Error: Memory region \'%s\' has been referenced"
              " but not defined\n"),
              r->sec->owner->filename, region_name);

    /* loop through the list of memory-type sections..
       note that only two fields of this list are
       used (file, sec) and they are both (char *) */
    for (s = user_memory_sections; s != NULL; s = snext) {

      snext = s->next;
      if (!s->file || !s->sec) continue;
      
#if 0
      printf("s->file =%s, s->sec = %s \n", (char *) s->file, (char *) s->sec);
#endif
      if (strcmp((char *)s->file, region_name) == 0) {
        /* if this section has been assigned to the current region,
           add it to the allocation list */
        ssprev = unassigned_sections;
        for (ss = ssprev; ss != NULL; ss = ssnext) {
          ssnext = ss->next;
          if (ss->sec && strcmp((char *) s->sec, ss->sec->name) == 0) {
            if (pic32_debug)
              printf("  input section \"%s\", len = %lx, flags = %x, attr = %x\n",
                     ss->sec->name, ss->sec->size,
                     ss->sec->flags, ss->attributes);

            insert_alloc_section(alloc_section_list, ss);
            ssprev->next = ssnext; /* unlink it from unassigned_sections */
          } else
            ssprev = ss;
        }
      }
    }
    
    if (pic32_section_list_length(alloc_section_list) == 0) {
      if (pic32_debug)
        printf("\n  (none)\n");
      continue;
    }
      
//    temp_region.name = (char *) region_name;
    temp_region.next = &temp_region;
    temp_region.origin = r->sec->vma;
    temp_region.length = r->sec->lma;

    /* don't expect to need these.. */
    temp_region.current = r->sec->vma;
//    temp_region.old_length = 0;
    temp_region.flags = 0;
    temp_region.not_flags = 0;
    temp_region.had_full_message = FALSE;

    if (free_blocks)
      pic32_free_memory_list(&free_blocks);
    pic32_init_memory_list(&free_blocks);
  
    /* start with entire region as a free block */
    pic32_add_to_memory_list(free_blocks, temp_region.origin,
                             temp_region.length);

    if (pic32_debug) {
      pic32_print_section_list(alloc_section_list, "allocation");
    }
  
    result |= locate_sections(address, 0, &temp_region);   /* most restrictive  */
    result |= locate_sections(all_attr, 0, &temp_region);  /* least restrictive */
  }

  return result;
} /* allocate_user_memory() */


// Coherent sections processing functions

static void
update_coherent_group_section_info(bfd_vma alloc_addr,
                                   struct pic32_section *g,
                                   unsigned int mask,
                                   struct memory_region_struct *region ATTRIBUTE_UNUSED) {
    
    struct pic32_section *s, *next;
    asection *sec;
    char *name;
    bfd_vma addr = alloc_addr;
    lang_output_section_statement_type *os;
    
    /* create a unique name for the output section, if necessary */
    sec = bfd_get_section_by_name(link_info.output_bfd, g->sec->name);
    if (sec) {
        name = ( g->sec->size > 0) ?
        unique_section_name(g->sec->name) :
        unique_zero_length_section_name(g->sec->name);
    }
    else {
        name = (char *) g->sec->name;
        if ( PIC32_IS_COHERENT_ATTR(g->sec) &&
            ((g->attributes & mask) == (coherent|data))
            )
        {
            strcpy(name, ".COHERENTDATA$");
        }
        else if ( PIC32_IS_COHERENT_ATTR(g->sec) &&
                 ((g->attributes & mask) == (coherent|bss))
                 )
        {
            strcpy(name, ".COHERENTBSS$");
        }
        else if ( PIC32_IS_COHERENT_ATTR(g->sec) &&
                 ((g->attributes & mask) == (coherent|persist))
                 )
        {
            strcpy(name, ".COHERENTPERSIST$");
        }
        else
        {}
    }
    /* create an output section (statement) */
    os = lang_output_section_statement_lookup (name, 0, TRUE);
    
    /* loop through the input sections in this group */
    for (s = g; s != NULL; s = next) {
        next = s->next;
        if (s->sec && (PIC32_IS_COHERENT_ATTR(s->sec)) && ((s->attributes & mask) == mask) ) {
            update_section_addr(s->sec, addr);
            addr += (s->sec->size);
            
            /* lang_add_section() will call init_os() if needed */
            lang_add_section (&os->children, s->sec, NULL, os);
            
        }
    }
    finish_section_info(g, os);
} /* update_coherent_group_section_info() */



static bfd_vma
group_coherent_section_size(struct pic32_section *g, unsigned int mask)
{
    struct pic32_section *s,*next;
    bfd_vma result = 0;
    
    for (s = g; s != NULL; s = next) {
        next = s->next;
        if (s->sec == 0)
            continue;
        if (s->sec && (PIC32_IS_COHERENT_ATTR(s->sec)) && ((s->attributes & mask) == mask))
            result += s->sec->size;
    }
    return result;
}

static int
locate_coherent_group_section(struct pic32_section *s,
                              struct memory_region_struct *region, unsigned int mask) {
    
    struct pic32_memory *b;
    bfd_vma len = 0;
    bfd_vma addr = s->sec->lma;
    int result = 0;
    unsigned int pad_req;
    
    len = group_coherent_section_size(s, mask);
    
    // Set len to 16 byte padding.
    pad_req = len%16 ;
    if (pad_req)
        len = len + (16-pad_req);
    
    
    /* look for tricky user error */
    if (PIC32_IS_ABSOLUTE_ATTR(s->sec) && ACROSS_REGION(addr, len, region))
        einfo(_(" Link Warning: absolute section \'%s\' crosses"
                " the boundary of region %s.\n"),
              s->sec->name, region->name_list.name);
    
    if (len == 0)
        update_coherent_group_section_info(0,s,mask,region); /* trivial case */
    else if (PIC32_IS_ABSOLUTE_ATTR(s->sec) &&
             OUTSIDE_REGION(addr, len, region)) {
        update_coherent_group_section_info(addr,s,mask,region);  /* falls outside region */
    }
    else {                          /* locate using free_blocks list */
        b = select_free_block(s, len);
        if (b) {
            addr = b->addr + b->offset;
            /* Translate the address from kseg0 to kseg1 */
            if (PIC32_IS_COHERENT_ATTR(s->sec))
                addr |= 0x20000000u;
            update_coherent_group_section_info(addr,s,mask,region);
            create_remainder_blocks(free_blocks,b,len);
            remove_free_block(b);
        } else {
            if (locate_options != NO_LOCATE_OPTION) {
                return 0;
            }
            result |= 1;
        }
    }
    
    pic32_remove_coherent_group_from_section_list(alloc_section_list, mask);
    
    return result;
} /* locate_group_section() */



static int
locate_coherent_sections (unsigned int mask,
                          struct memory_region_struct *region) {
    struct pic32_section *s,*next;
    int result = 0;
    
    mask = coherent | mask ;
    
    for (s = alloc_section_list; s != NULL; s = next) {
        next = s->next;
        if (s->sec && (s->attributes & mask) == mask) {
            bfd_vma len = s->sec->rawsize? s->sec->rawsize: s->sec->size;
            
            if (PIC32_IS_COHERENT_ATTR(s->sec)) {
                if (coherent_section_count == 0) {
                    // Set the alignment to cache line boundary (16 byte)
                    s->sec->alignment_power = 4;
                    coherent_section_count++;
                    result |= locate_coherent_group_section(s, region, mask);
                    if (s->sec->vma == 0)
                    {
                        report_allocation_error(s);
                    }
                }
            }
        }
    }
    return result;
} /* locate_coherent_sections */
