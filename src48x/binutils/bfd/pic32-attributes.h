#if !defined(ATTR)
#define ATTR(a,b,c)
#endif
#if !defined(ATTR_IS)
#define ATTR_IS(a,b)
#endif
#if !defined(MASK1)
//#define MASK1(a,b,c,d,e,f,g,h)
#define MASK1(a,b,c,d,e,f,g,h)
#endif
#if !defined(MASK2)
#define MASK2(a,b,c,d,e,f,g,h,i,j,k,l,m,n)
#endif
#if !defined(MASK3)
#define MASK3(a,b,c,d,e,f,g,h,i,j,k,l,m)
#endif
#if !defined(MASK4)
#define MASK4(a,b,c,d)
#endif

/*
 *
 * ATTR: Setting Section Attributes
 *
 *  ATTR(id, has_arg, set_it)
 *
 *  id:        attribute name
 *  has_arg:   (0 = no, 1 = yes)
 *  set_it:    C statement to set the attribute
 *             (refers to variables sec,arg)
 *
 */

#if NO_ARGS
#define PIC32_SET_ABSOLUTE_ATTR_ARG(a) {}
#define PIC32_SET_ALIGN_ATTR_ARG(a) {}
#define PIC32_SET_MERGE_ATTR_ARG(a) {}
#define PIC32_SET_MEMORY_ATTR_ARG(a) {}
#endif

/*
 * This is the list of "standard" attributes.
 * They are represented in ELF object
 * file as a symbol named__ext_attr_secname.
 *
 * Do not modify this list. New attributes
 * should be added to the end. Up to 32 extended
 * attributes are supported.
 */
ATTR( code,    0, PIC32_SET_CODE_ATTR(sec) )
ATTR( data,    0, PIC32_SET_DATA_ATTR(sec) )
ATTR( bss,     0, PIC32_SET_BSS_ATTR(sec)  )
ATTR( persist, 0, PIC32_SET_PERSIST_ATTR(sec) )
ATTR( memory,  1, PIC32_SET_MEMORY_ATTR(sec)
                  PIC32_SET_MEMORY_ATTR_ARG(arg) )

ATTR( address, 1, PIC32_SET_ABSOLUTE_ATTR(sec)
                  PIC32_SET_ABSOLUTE_ATTR_ARG(arg) )
ATTR( near,    0, PIC32_SET_NEAR_ATTR(sec) )
ATTR( reverse, 1, PIC32_SET_REVERSE_ATTR(sec)
                  PIC32_SET_ALIGN_ATTR_ARG(arg) )
ATTR( align,   1, PIC32_SET_ALIGN_ATTR_ARG(arg) )
ATTR( noload,  0, PIC32_SET_NOLOAD_ATTR(sec) )
ATTR( merge,   1, PIC32_SET_MERGE_ATTR(sec)
                  PIC32_SET_MERGE_ATTR_ARG(arg) )
ATTR( info,    0, PIC32_SET_INFO_ATTR(sec) )
ATTR( dma,     0, PIC32_SET_DMA_ATTR(sec) )

ATTR( heap,    0, PIC32_SET_HEAP_ATTR(sec) )
ATTR( stack,   0, PIC32_SET_STACK_ATTR(sec) )
ATTR( ramfunc, 0, PIC32_SET_RAMFUNC_ATTR(sec) )
ATTR( keep, 0, PIC32_SET_KEEP_ATTR(sec) ) 
ATTR( coherent, 0, PIC32_SET_COHERENT_ATTR(sec) )
ATTR( serial_mem, 0 , PIC32_SET_SERIAL_MEM_ATTR(sec))
/* lghica co-resident */
ATTR(shared,    0, PIC32_SET_SHARED_ATTR(sec))
ATTR(preserved, 0, PIC32_SET_PRESERVED_ATTR(sec))

/*
 * ATTR_IS: Testing Section Attributes
 *
 *  ATTR_IS(id, test_it)
 *
 *  id:        attribute name
 *  test_it:   C statement to test the attribute
 *
 */

ATTR_IS( code,    PIC32_IS_CODE_ATTR(sec) )
ATTR_IS( data,    PIC32_IS_DATA_ATTR(sec) )
ATTR_IS( bss,     PIC32_IS_BSS_ATTR(sec)  )
ATTR_IS( persist, PIC32_IS_PERSIST_ATTR(sec) )
ATTR_IS( memory,  PIC32_IS_MEMORY_ATTR(sec) )

ATTR_IS( address, PIC32_IS_ABSOLUTE_ATTR(sec) )
ATTR_IS( near,    PIC32_IS_NEAR_ATTR(sec) )
ATTR_IS( reverse, PIC32_IS_REVERSE_ATTR(sec) )
/* can't test for align */
ATTR_IS( noload,  PIC32_IS_NOLOAD_ATTR(sec) )
ATTR_IS( merge,   PIC32_IS_MERGE_ATTR(sec) )
ATTR_IS( info,    PIC32_IS_INFO_ATTR(sec) )
ATTR_IS( dma,     PIC32_IS_DMA_ATTR(sec) )

ATTR_IS( heap,    PIC32_IS_HEAP_ATTR(sec) )
ATTR_IS( stack,   PIC32_IS_STACK_ATTR(sec) )
ATTR_IS( ramfunc, PIC32_IS_RAMFUNC_ATTR(sec) )
ATTR_IS( keep, PIC32_IS_KEEP_ATTR(sec) )
ATTR_IS( coherent, PIC32_IS_COHERENT_ATTR(sec) )
ATTR_IS( serial_mem, PIC32_IS_SERIAL_MEM_ATTR(sec))
/* lghica co-resident */
ATTR_IS( shared,    PIC32_IS_SHARED_ATTR(sec))
ATTR_IS( preserved, PIC32_IS_PRESERVED_ATTR(sec))


/*
 * MASK1: Attributes that represent section types
 */
//MASK1( code, data, bss, persist, memory, heap, stack, ramfunc)
MASK1( code, data, bss, persist, heap, stack, ramfunc, serial_mem)


/*
 * MASK2: Attributes that modify section types
 *
 * MASK2( type, modifier1, ... modifier14 )
 */
MASK2( code, address, 0, 0, align, noload, merge, info, 0, keep, 0, memory, shared, preserved)
MASK2( data, address, near, reverse, align, noload, merge, info, dma, keep, coherent, memory, shared, preserved)
MASK2( bss, address, near, reverse, align, noload, 0, info, dma, keep, coherent, memory, shared, preserved)
MASK2( persist, address, near, reverse, align, noload, 0, 0, dma, keep, coherent, 0, shared, preserved)
//MASK2( memory, address, 0, reverse, align, noload, 0, 0, 0, keep, 0, 0)
MASK2( heap, address, 0, 0, align, 0, 0, 0, 0, keep, coherent, 0, shared, preserved)
MASK2( stack, address, 0, 0, align, 0, 0, 0, 0, keep, coherent, 0, shared, preserved)
MASK2( ramfunc, 0, 0, 0, align, noload, merge, info, 0, keep, 0, 0, shared, preserved)
MASK2( serial_mem, address, 0, 0, align, noload, merge, info, 0, keep, 0, memory, shared, preserved)


/*
 * MASK3: Attributes that may be used in combination
 *
 * MASK3( key, attr1, ... attr13)
 */
MASK3( address, near, 0, 0, noload, 0, 0, dma, keep, coherent, memory, shared, preserved)
MASK3( near, address, reverse, align, noload, merge, 0, 0, keep, coherent, memory, shared, preserved)
MASK3( reverse, 0, near, 0, noload, merge, 0, dma, keep, coherent, 0, shared, preserved)
MASK3( align, address, near, 0, noload, merge, 0, dma, keep, coherent, memory, shared, preserved)
MASK3( noload, address, near, reverse, align, 0, 0, dma, keep, coherent, memory, shared, preserved)
MASK3( merge, 0, near, reverse, align, 0, 0, info, keep, 0, 0, shared, preserved)
MASK3( info, 0, 0, 0, 0, merge, 0, 0, keep, 0, 0, shared, preserved)
MASK3( dma, address, 0, reverse, align, noload, 0, 0, keep, 0, memory, shared, preserved)
MASK3( keep, address, near, reverse, align, noload, merge, info, dma, coherent, memory, shared, preserved)
MASK3( coherent, address, near, reverse, align, noload, 0, 0, 0, keep, memory, shared, preserved)
MASK3( memory, address, near, 0, align, noload, 0, 0, dma, keep, coherent, shared, preserved)
/* lghica co-resident*/
MASK3( shared, address, near, reverse, align, noload, merge, info, dma, keep, coherent, memory, preserved)
MASK3( preserved, address, near, reverse, align, noload, merge, info, dma, keep, coherent, memory, shared)


/*
 * MASK4: Reserved section names with implied attributes
 *
 *  MASK4( name, quiet, type, modifier)
 *
 *  name:     reserved section name
 *  quiet:    set implied attribute(s) quietly?
 *            (0 = no, 1 = yes)
 *  type:     attribute that describes type
 *  modifier: attribute that modifies type
 *
 */
MASK4( .text,    1, code, 0 )
MASK4( .data,    1, data, 0 )
MASK4( .bss,     1, bss, 0 )
MASK4( .sbss,    1, bss, near )
MASK4( .nbss,    0, bss, near )
MASK4( .ndata,   0, data, near )
MASK4( .sdata,   1, data, near )
MASK4( .ndconst, 0, data, near )
MASK4( .pbss,    0, persist, 0 )
MASK4( .persist, 0, persist, 0 )
MASK4( .dconst,  0, data, 0 )
MASK4( .rdata,   1, code, 0 )
MASK4( .rodata,  1, code, 0 )
MASK4( .ramfunc, 1, ramfunc, 0 )
MASK4( .stack,    1, stack, 0 )

