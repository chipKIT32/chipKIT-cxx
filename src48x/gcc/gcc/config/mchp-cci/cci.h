/*
 *  CCI support for Microchip Compilers
 */

#ifndef CCI
/*
 * General defines....
 *
 */

enum CCI_KIND {
  CCI_unknown,     /* should never happen, I don't like 0 being a valid enum */
  CCI_set_value,   /* For KEYWORD pass the address of the value you wish to set
                    *
                    *  KEYWORD = N;
                    *
                    * Great for setting flags or options...
                    */
  CCI_define,      /* straight define:
                    *
                    * #define KEYWORD=CCI_VAL
                    */
  CCI_attribute,   /* attribute define:
                    *
                    * #define KEYWORD=__attribute__((CCI_VAL)))
                    */
  CCI_attribute_n, /* attribute define:
                    *
                    * #define KEYWORD(P1,P2,...Pn)= \
                    *   __attribute__((CCI_VAL(P1,P2,...Pn)))
                    *
                    * if P1 (all or none) is specified in the conversion, then
                    *   that will be used otherwise the compiler will generate
                    *   the parenthetical expression after CCI_VAL
                    */
  CCI_attribute_v, /* attribute define:
                    *
                    * #define KEYWORD(...)=__attribute__((CCI_VAL(__VA_ARGS__)))
                    */
  CCI_attribute_nv, /* attribute define:
                    *
                    * #define KEYWORD(P1,P2,...Pn,...)= \
                    *   __attribute__((CCI_VAL(P1,P2,...Pn,__VA_ARGS__)))
                    *
                    * if P1 (all or none) is specified in the conversion, then
                    *   that will be used otherwise the compiler will generate
                    *   the parenthetical expression after CCI_VAL
                    */
  CCI_pragma       /* pragma define, maps a pragma to an already handled
                    *   native pragma, the CCI_for_xxx should be a function
                    *   name
                    */
};

extern void mchp_init_cci_builtins(void);
extern void mchp_init_cci(void *pfile_v);
extern void mchp_handle_config_pragma(struct cpp_reader *pfile);
extern void mchp_handle_section_pragma (struct cpp_reader *pfile);
extern void mchp_handle_printf_args_pragma (struct cpp_reader *pfile);
extern void mchp_handle_scanf_args_pragma (struct cpp_reader *pfile);
extern void mchp_handle_keep_pragma (struct cpp_reader *pfile);
extern void mchp_handle_optimize_pragma (struct cpp_reader *pfile);
extern void mchp_handle_align_pragma (struct cpp_reader *pfile);
#if defined(_BUILD_C32_) || defined(TARGET_IS_PIC32MX)
extern void mchp_handle_coherent_pragma (struct cpp_reader *pfile);
extern void mchp_handle_configset_pragma(struct cpp_reader *pfile, const char *set);
extern void mchp_handle_config_alt_pragma(struct cpp_reader *pfile);
extern void mchp_handle_config_bf1_pragma(struct cpp_reader *pfile);
extern void mchp_handle_config_abf1_pragma(struct cpp_reader *pfile);
extern void mchp_handle_config_bf2_pragma(struct cpp_reader *pfile);
extern void mchp_handle_config_abf2_pragma(struct cpp_reader *pfile);
#endif /* _BUILD_C32_ */
/*
 * #pragma config support
 */

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


#else

/* CCI mapping */

/* for use as NULL define */
#define EMPTY 0

/*
 * For brevity...
 *
 */
#define IMPORT_C30(V)  ((TARGET_C30) && mchp_import && ((strcasecmp(mchp_import,(V)) == 0)))
#define IMPORT_C32(V)  ((TARGET_C32) && mchp_import && ((strcasecmp(mchp_import,(V)) == 0)))
#define IMPORT_MCHP(V) ((mchp_import && strcasecmp(mchp_import,(V)) == 0))

#define CCI_C30  ((TARGET_C30) && (TARGET_CCI || IMPORT_C30("cci")))
#define CCI_C32  ((TARGET_C32) && (TARGET_CCI || IMPORT_C32("cci")))
#define CCI_MCHP (TARGET_CCI || IMPORT_MCHP("cci"))

/*  CCI,        CCI_KIND          CCI Keyword,    CCI for C3x,     n      */
/*  -------------------------------------------------------------------------*/
CCI(CCI_C30,    CCI_attribute_v,  "__interrupt",  "interrupt",     0)
CCI(CCI_C32,    CCI_attribute_nv, "__interrupt", \
    "vector(P1), interrupt(__VA_ARGS__), nomips16",                1)

CCI(CCI_MCHP,   CCI_attribute,    "__persistent", "persistent",    0)

CCI(CCI_C30,    CCI_attribute,    "__near",       "near",          0)
CCI(CCI_C32,    CCI_define,       "__near",       EMPTY,           0)

CCI(CCI_MCHP,   CCI_attribute,    "__far",        "far",           0)

CCI(CCI_MCHP,   CCI_attribute_n,  "__at",         "address",       1)

CCI(CCI_C30,    CCI_attribute,    "__eeprom",     "space(eedata)", 0)
CCI(CCI_C32,    CCI_attribute,    "__eeprom",
    "unsupported(\"__eeprom is unsupported on this platform\")",   0)

CCI(CCI_C30,    CCI_attribute,    "__xdata",      "space(xmemory)",0)
CCI(CCI_C30,    CCI_attribute,    "__ydata",      "space(ymemory)",0)
CCI(CCI_C32,    CCI_define,       "__xdata",      EMPTY,           0)
CCI(CCI_C32,    CCI_define,       "__ydata",      EMPTY,           0)
CCI(CCI_MCHP,   CCI_define,       "__bank(X)",    EMPTY,           0)

CCI(CCI_C30,    CCI_define,       "__abi(...)",   EMPTY,           0)
CCI(CCI_C32,    CCI_attribute_n,  "__abi",        "P1",            1)

CCI(CCI_MCHP,   CCI_attribute,    "__deprecate",  "deprecated",    0)

CCI(CCI_MCHP,   CCI_attribute,    "__pack",       "packed",        0)

CCI(CCI_MCHP,   CCI_attribute_n,  "__align",      "aligned",       1)

/* __pack_upper_byte is already defined for C30 */
CCI(CCI_C32,    CCI_define,       "__pack_upper_byte",  EMPTY,     0)

CCI(CCI_MCHP,   CCI_attribute_n,  "__section",    "section",       1)

CCI(CCI_MCHP,   CCI_define,       "__CCI__",      "1",             0)

/* CCI for IAR */
/*  CCI,                CCI_KIND       CCI Keyword,   CCI for C3x,    n */
/*  -------------------------------------------------------------------------*/
CCI(IMPORT_MCHP("iar"), CCI_define,    "__section_begin", "__builtin_section_begin", 0)
CCI(IMPORT_MCHP("iar"), CCI_define,    "__section_end",   "__builtin_section_end", 0)
CCI(IMPORT_MCHP("iar"), CCI_define,    "__section_size",  "__builtin_section_size", 0)
CCI(IMPORT_MCHP("iar"), CCI_define,    "__segment_begin", "__builtin_section_begin", 0)
CCI(IMPORT_MCHP("iar"), CCI_define,    "__segment_end",   "__builtin_section_end", 0)
CCI(IMPORT_MCHP("iar"), CCI_define,    "__segment_size",  "__builtin_section_size", 0)
CCI(IMPORT_MCHP("iar"), CCI_define,    "__sfb",       "__builtin_section_begin", 0)
CCI(IMPORT_MCHP("iar"), CCI_define,    "__sfe",       "__builtin_section_end", 0)
CCI(IMPORT_MCHP("iar"), CCI_define,    "__sfs",       "__builtin_section_size", 0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__root",        "keep",      0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__arm", \
      "unsupported(\"__arm is unsupported on this platform\")",      0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__intrinsic", \
      "unsupported(\"__intrinsic is unsupported on this platform\")",0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__interwork", \
      "unsupported(\"__interwork is unsupported on this platform\")",0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__little_endian", \
      "unsupported(\"__little_endian is unsupported on this platform\")", 0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__nested", \
      "unsupported(\"__nested is unsupported on this platform\")",   0)

CCI(IMPORT_C30("iar"), CCI_attribute, "__ramfunc", \
      "unsupported(\"__ramfunc is unsupported on this platform\")",  0)

CCI(IMPORT_C32("iar"),  CCI_attribute, "__ramfunc", "ramfunc,unique_section",  0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__swi", \
      "unsupported(\"__swi is unsupported on this platform\")",      0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__thumb", \
      "unsupported(\"__thumb is unsupported on this platform\")",    0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__farfunc", \
      "unsupported(\"__farfunc is unsupported on this platform\")",  0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__huge", \
      "unsupported(\"__huge is unsupported on this platform\")",     0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__nearfunc", \
      "unsupported(\"__nearfunc is unsupported on this platform\")", 0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__big_endian", \
      "target_error(\"__big_endian is unsupported on this platform\")", 0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__fiq", \
      "target_error(\"__fiq is unsupported on this platform\")", 0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__irq", \
      "target_error(\"__irq is unsupported on this platform\")", 0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__no_init", "persistent", 0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__noreturn", "noreturn", 0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__task", "naked", 0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__weak", "weak", 0)

CCI(IMPORT_MCHP("iar"), CCI_attribute, "__packed", "packed", 0)

CCI(IMPORT_MCHP("iar"), CCI_define,    "__istate_t", "unsigned int", 0)

CCI(IMPORT_MCHP("iar"), CCI_define,    "__disable_interrupt", \
                                       "__builtin_disable_interrupts", 0)

CCI(IMPORT_MCHP("iar"), CCI_define,    "__enable_interrupt", \
                                       "__builtin_enable_interrupts", 0)

CCI(IMPORT_MCHP("iar"), CCI_define,    "__get_interrupt_state", \
                                       "__builtin_get_isr_state", 0)

CCI(IMPORT_MCHP("iar"), CCI_define,    "__set_interrupt_state", \
                                       "__builtin_set_isr_state", 0)

CCI(IMPORT_MCHP("iar"), CCI_pragma,    "inline",  \
                                       mchp_handle_inline_pragma, 0)

CCI(IMPORT_MCHP("iar"), CCI_pragma,    "segment",  \
                                       mchp_handle_section_pragma, 0)

CCI(IMPORT_MCHP("iar"), CCI_pragma,    "data_alignment",  \
                                       mchp_handle_align_pragma, 0)

CCI(IMPORT_MCHP("iar"), CCI_pragma,    "__printf_args",  \
                                       mchp_handle_printf_args_pragma, 0)

CCI(IMPORT_MCHP("iar"), CCI_pragma,    "__scanf_args",  \
                                       mchp_handle_scanf_args_pragma, 0)

CCI(IMPORT_MCHP("iar"), CCI_pragma,    "required",  \
                                       mchp_handle_required_pragma, 0)

#undef CCI
#endif
