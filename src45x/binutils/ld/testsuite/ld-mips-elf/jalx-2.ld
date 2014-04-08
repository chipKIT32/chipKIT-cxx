ENTRY (internal_function)
_start_text_phys = 0x4400000;
_start_text = _start_text_phys;

SECTIONS
{
  .text _start_text : AT (ADDR (.text)) { *(.text) }
  .plt : { *(.plt) }
  .interp : { *(.interp) }
  .got : { *(.got) }
  .got.plt : { *(.got.plt) }
  .rld_map : { *(.rld_map) }
  .dynsym : { *(.dynsym) }
  .dynstr : { *(.dynstr) }
  .dynamic : { *(.dynamic) }
  .hash : { *(.hash) }
  .reginfo : { *(.reginfo) }
  .rodata : { *(.rodata) }
  .rel.plt : { *(.rel.plt) }
}
