
#ifndef PIC30_SECTION
#error define PIC30_SECTION
#else

  PIC30_SECTION(".bss",\
                extern, bss_section, SECTION_BSS)
  PIC30_SECTION(".const",\
                static, const_section, SECTION_READ_ONLY | SECTION_PAGE)
  PIC30_SECTION(".boot_const",\
                static, boot_const_section, SECTION_READ_ONLY)
  PIC30_SECTION(".secure_const",\
                static, secure_const_section, SECTION_READ_ONLY)
  PIC30_SECTION(".data",\
                extern, data_section, SECTION_WRITE)
  PIC30_SECTION(".dconst",\
                static, dconst_section, SECTION_WRITE)
  PIC30_SECTION(".eedata",\
                static, eedata_section, SECTION_EEDATA)
  PIC30_SECTION(".nbss",\
                static, nbss_section, SECTION_BSS | SECTION_NEAR)
  PIC30_SECTION(".ndata",\
                static, ndata_section, SECTION_WRITE | SECTION_NEAR)
  PIC30_SECTION(".ndconst",\
                static, ndconst_section, SECTION_WRITE | SECTION_NEAR)
  PIC30_SECTION(".pbss",\
                static, pbss_section, SECTION_PERSIST)
  PIC30_SECTION(".text",\
                extern, text_section, SECTION_CODE)
  PIC30_SECTION(".xbss",\
                static, xbss_section, SECTION_BSS | SECTION_XMEMORY)
  PIC30_SECTION(".xdata",\
                static, xdata_section, SECTION_WRITE | SECTION_XMEMORY)
  PIC30_SECTION(".ybss",\
                static, ybss_section, SECTION_BSS | SECTION_YMEMORY)
  PIC30_SECTION(".ydata"\
        ,       static, ydata_section, SECTION_WRITE | SECTION_YMEMORY)
  PIC30_SECTION(0,\
                extern, bss_section, 0)

#undef PIC30_SECTION
#endif
