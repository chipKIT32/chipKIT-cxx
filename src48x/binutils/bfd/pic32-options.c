/*
** pic32-options.c
**
** This file is part of BFD, the Binary File Descriptor library.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
** USA.
**
** This file contains pic32-specifc options
** processing for the linker.
**
** It is included by the following target-specific
** emulation files:
**
**   ld/emultmpl/pic32_elf32.em
**
*/

#include <errno.h>

extern void pic32_processor_option(char *);
/*
** list_options()
**
** This function is called by the --help option.
*/
static void
gldelf32pic32mx_list_options (FILE * file)
{
/*  -D or --debug is intended for internal use. It is not for use by end customers.
    So, don't output the option in the help screen.
*/
  fprintf (file, _("  --hard-float         Use library objects utilizing FR64 floating-point"
                   "instructions\n"));
  fprintf (file, _("  --soft-float         Use library objects utilizing software floating-"
                   "point emulation\n"));
  fprintf (file, _("  -p,--processor PROC  Specify the target processor"
                   " (e.g., 32MX795F512L)\n"));
  fprintf (file, _("  --report-mem         Report memory usage to console\n"));
  fprintf (file, _("  --smart-io           Merge I/O library functions (default)\n"));
  fprintf (file, _("  --no-smart-io        Don't merge I/O library functions\n"));
  /* lghica co-resident */
#if 0 /* Disable the co-resident feature for now */
  fprintf (file, _("  --coresident         link for coresident application\n"));
#endif
} /* static void elf32pic32mx_list_options () */

static void pic32_init_fill_option_list (struct pic32_fill_option **lst)
{
  *lst = ((struct pic32_fill_option *)
         xmalloc(sizeof(struct pic32_fill_option)));
  (*lst)->next = 0;
  (*lst)->expr.values_range = NULL;
  (*lst)->expr.base_value = 0;
  (*lst)->expr.addened = 0;
  (*lst)->loc.unused = TRUE;
  (*lst)->loc.address = 0;
  (*lst)->loc.end_address = 0;
  (*lst)->fill_section_list = NULL;
}

static void pic32_create_fill_option (struct pic32_fill_option *option)
{
  option->next = 0;
  option->expr.values_range = 0;
  option->expr.base_value = 0;
  option->expr.addened = 0;
  option->loc.unused = TRUE;
  option->loc.address = 0;
  option->loc.end_address = 0;
  option->fill_section_list = NULL;
  option->fill_width = 4;
  option->range_size = 0;
}

static void pic32_append_option_to_list (struct pic32_fill_option *lst,
                                         struct pic32_fill_option *option)
{
  struct pic32_fill_option *o, *tail;
  if (!(lst))
    return;
  /* find the tail */
  for (o = lst; o != NULL; o = o->next)
    tail = o;
  /* append the new option */
  tail->next = option;
}

static void
pic32_process_fill_argument (char * fill_arg)
{
  char *expression = NULL;
  char *location = NULL;
  struct pic32_fill_option *option;
  char *LHS_operand = NULL;
  char *RHS_operand = NULL;
  char *addr = NULL;
  char *end_addr = NULL;
  char *width = NULL;
  long long value = 0;
  long long address = 0;
  const char *fill_warn = "Incorrect format for fill argument\n";
  
  /* check if there is no fill expression */ 
  if (strncmp(fill_arg, "@", 1) == 0)
   {
    einfo(_("%P: Warning: %s"), fill_warn);
    return;}

  expression = strtok(fill_arg, "@");
  location = strtok(NULL, "@");
  if (expression)
   {
    if (strchr(expression, ':'))
     {
      width = strtok(expression, ":");
      expression = strtok(NULL,":");
      }
    /* create the options list*/

    if (!pic32_fill_option_list)
     pic32_init_fill_option_list(&pic32_fill_option_list);
     option = ((struct pic32_fill_option *)
              xmalloc(sizeof(struct pic32_fill_option)));
     pic32_create_fill_option(option);
     if (width && (*width == 'w'))
     (void)sscanf((width+1), "%i", &(option->fill_width));

     if ((option->fill_width != 1) && (option->fill_width != 2) &&
         (option->fill_width != 4) && (option->fill_width != 8)){
       einfo(_("%P: Warning: You cannot use a width_value of %d."
               "You must specify only one byte, 2 bytes, 8 bytes, or"
               " one instruction word (4 bytes, the default)."
               " The default width will be used.\n"),
               option->fill_width);
       option->fill_width = 4;}

       pic32_init_section_list(&option->fill_section_list);

       /* expression is a changing value */
       char *inc = strstr(expression, "+=");
       char *dec = strstr(expression, "-=");
       if (inc || dec)
        {
         RHS_operand = inc? inc + 2 : dec + 2;
         if (inc) *inc = '\0'; else *dec = '\0';
         LHS_operand = expression;
         (void)sscanf(LHS_operand, "%lli", &(option->expr.base_value));
         (void)sscanf(RHS_operand, "%lli", &(option->expr.addened));
         if (dec)
           option->expr.addened = -option->expr.addened;
         }

       /* expression is a range of values */
       else if (strchr(expression, ','))
        {
         int count = 0;
         char *token = ((char *) malloc(strlen(expression)+1));
         char *value_token;
         unsigned char *buff;
         strcpy(token, (const char *)expression);
         for (value_token = strtok(token, ","); value_token;
              value_token = strtok(NULL, ","))
            count++; /*get the range size*/
         option->range_size = count;
         free(token);
         buff = malloc((count * option->fill_width) * sizeof(unsigned char));
         option->expr.values_range = buff;
         for (value_token = strtok(expression, ","); value_token;
              value_token = strtok(NULL, ","))
           {
            (void)sscanf(value_token, "%lli", &value);
            if (option->fill_width == 1)
              *buff++ = value & 0xFF;
            else if (option->fill_width == 2){
              *buff++ = value & 0xFF;
              *buff++ = (value >> 8) & 0xFF;
            }
            else if (option->fill_width == 8){
              *buff++ = value & 0xFF;
              *buff++ = (value >> 8) & 0xFF;
              *buff++ = (value >> 16) & 0xFF;
              *buff++ = (value >> 24) & 0xFF;
              *buff++ = (value >> 32) & 0xFF;
              *buff++ = (value >> 40) & 0xFF;
              *buff++ = (value >> 48) & 0xFF;
              *buff++ = (value >> 56) & 0xFF;
            }
            else{
              *buff++ = value & 0xFF;
              *buff++ = (value >> 8) & 0xFF;
              *buff++ = (value >> 16) & 0xFF;
              *buff++ = (value >> 24) & 0xFF;
            }
          }
        }
          /* expression is a single value */
      else 
        {
         (void)sscanf(expression, "%lli", &value);
         option->expr.base_value = value;
        }
    if (location)
      {
       int read_val = 0;
       if (strcmp(location, "unused") != 0)
         { option->loc.unused = FALSE;

           addr = strtok(location, ":");
           end_addr = strtok(NULL, ":");
           read_val = sscanf(addr, "%lli", &address);
           option->loc.address = address & 0xFFFFFFFFu;
           if (end_addr){
             read_val = sscanf(end_addr, "%lli", &address);
             option->loc.end_address = address & 0xFFFFFFFFu;
           }
           if (read_val == 0) {
             einfo(_("%P: Warning: Bad location specified for fill option. "
                    "Option will be ignored.\n"));
             return;
           }
         }
      }
   }
  else 
   {
    einfo(_("%P: Warning: %s"), fill_warn);
    return;
   }
  pic32_append_option_to_list(pic32_fill_option_list, option);
  return;
}

/*
** parse_args()
**
** This function is called for each command-line option.
*/
static int
gldelf32pic32mx_parse_args (int argc, char ** argv)
{
  int        longind;
  int        optc;
  int        prevoptind = optind;
  int        prevopterr = opterr;
  int        wanterror;
  static int lastoptind = -1;

  const char *smart_io_option_err  = "--smart-io and --no-smart-io";
  const char *option_err = " options can not be used together\n";
  const char *hardsoftfloat_option_err  = "--hard-float and --soft-float";
  const char *data_init_option_err = "--data-init and --no-data-init";
  const char *data_init_in_serial_mem_option_err = "--data-init_in_serial_mem and --no-data-init_in_serial_mem";
  const char *code_in_dinit_option_err = "--code-in-dinit and --no-code-in-dinit";

  if (lastoptind != optind)
    opterr = 0;

  wanterror  = opterr;
  lastoptind = optind;
  optc   = getopt_long_only (argc, argv, shortopts, longopts, & longind);
  opterr = prevopterr;
  switch (optc)
    {
    default:
      if (wanterror)
        xexit (1);
      optind =  prevoptind;
      return 0;
    case 'D':
      pic32_debug = TRUE;
      break;
    case SMART_IO_OPTION:
      if (pic32_has_smart_io_option && (!pic32_smart_io))
        einfo(_("%P%F: Error: %s%s"), smart_io_option_err, option_err);
      pic32_smart_io = TRUE;
      pic32_has_smart_io_option = TRUE;
      break;
    case NO_SMART_IO_OPTION:
      if (pic32_has_smart_io_option && (pic32_smart_io))
        einfo(_("%P%F: Error: %s%s"), smart_io_option_err, option_err);
      pic32_smart_io = FALSE;
      pic32_has_smart_io_option = TRUE;
      break;
    case REPORT_MEM_OPTION:
      pic32_report_mem = TRUE;
      break;
    case REPORT_MAFRLCSJ_OPTION:
      pic32_mafrlcsj = TRUE;
      break;
    case REPORT_MAFRLCSJ2_OPTION:
      pic32_mafrlcsj2 = TRUE;
      break;
    case MEMORY_SUMMARY:
      pic32_memory_summary= TRUE;
      memory_summary_arg = optarg;
      break;
    case DATA_INIT_OPTION:
      if (pic32_has_data_init_option && (!pic32_data_init))
        einfo(_("%P%F: Error: %s%s"), data_init_option_err, option_err);
      pic32_data_init = TRUE;
      pic32_has_data_init_option = TRUE;
      break;
    case NO_DATA_INIT_OPTION:
      if (pic32_has_data_init_option && (pic32_data_init))
        einfo(_("%P%F: Error: %s%s"), data_init_option_err, option_err);
      pic32_data_init = FALSE;
      pic32_has_data_init_option = TRUE;
      break;
            /* lghica co-resident */
    case MEMORY_USAGE:
        pic32_memory_usage = TRUE;
        break;
            
    case RESERVE_CONST: ///\ TODO
        pic32_reserve_const = TRUE;
        if (optarg)
        {
            if ((strstr(optarg, "0x") == 0) && (strstr(optarg, "0X") == 0))
                reserve_const_arg = atol(optarg);
            else
                (void) sscanf(optarg, "%x", &reserve_const_arg);
        }
        break;
    case PAD_FLASH:
        pic32_pad_flash_option = TRUE;
        if (optarg)
        {
            if ((strstr(optarg, "0x") == 0) && (strstr(optarg, "0X") == 0))
                pad_flash_arg = atol(optarg);
            else
                (void) sscanf(optarg, "%x", &pad_flash_arg);
        }
        break;
    case APPLICATION_ID:
        pic32_application_id = TRUE;
        application_id = optarg;
        break;
    case CORESIDENT:
        pic32_coresident_app = TRUE;
        break;
    case INHERIT_APPLICATION_INFO:
        pic32_inherit_application_info = TRUE;
        inherited_application = optarg;
        lang_add_input_file (optarg, lang_input_file_is_file_enum,
                                 (char *) NULL);
        break;
            
#ifdef TARGET_IS_PIC32MX
    case FILL_OPTION:
      pic32_process_fill_argument (optarg);
      pic32_has_fill_option = TRUE;
      break;
    case CRYPTO_OPTION:
      pic32_has_crypto_option = TRUE;
      crypto_file = (const char *) malloc(strlen(optarg)+1);
      strcpy(crypto_file, optarg);
      break;
    case MEMRESERVE_OPTION:
      /* This option has already been handled by the xc32-sh shell */
      break;
    case 'p':
      pic32_processor_option(optarg);
      break;
    case HARDFLOAT_OPTION:
      if (pic32_has_softfloat_option)
        einfo(_("%P%F: Error: %s%s"), hardsoftfloat_option_err, option_err);
      pic32_has_hardfloat_option = TRUE;
      break;
    case SOFTFLOAT_OPTION:
      if (pic32_has_hardfloat_option)
        einfo(_("%P%F: Error: %s%s"), hardsoftfloat_option_err, option_err);
      pic32_has_softfloat_option = TRUE;
      break;    
    case CODE_IN_DINIT_OPTION:
      if (pic32_has_code_in_dinit_option && !pic32_code_in_dinit)
        einfo(_("%P%F: Error: %s%s"), code_in_dinit_option_err,
                                      option_err);      
      pic32_code_in_dinit = TRUE;
      pic32_has_code_in_dinit_option = TRUE;
      break;
    case NO_CODE_IN_DINIT_OPTION:
      if (pic32_has_code_in_dinit_option && pic32_code_in_dinit)
        einfo(_("%P%F: Error: %s%s"), code_in_dinit_option_err,
                                      option_err);
      pic32_code_in_dinit = FALSE;
      pic32_has_code_in_dinit_option = TRUE;
      break;
    case DINIT_IN_SERIAL_MEM_OPTION:
      if (pic32_has_dinit_in_serial_mem_option && !pic32_dinit_in_serial_mem)
        einfo(_("%P%F: Error: %s%s"), data_init_in_serial_mem_option_err,
                                      option_err);
      pic32_dinit_in_serial_mem = TRUE;
      pic32_has_dinit_in_serial_mem_option = TRUE;
      break;
    case NO_DINIT_IN_SERIAL_MEM_OPTION:
      if (pic32_has_dinit_in_serial_mem_option && pic32_dinit_in_serial_mem)
        einfo(_("%P%F: Error: %s%s"), data_init_in_serial_mem_option_err,
                                      option_err);
      pic32_dinit_in_serial_mem = FALSE;
      pic32_has_dinit_in_serial_mem_option = TRUE;
      break;
    case DINIT_ADDRESS_OPTION:
      pic32_dinit_has_absolute_address = TRUE;
      if ((strstr(optarg, "0x") == 0) && (strstr(optarg, "0X") == 0))
        dinit_address = atol(optarg);
      else
        (void) sscanf(optarg, "%x", &dinit_address);
      break;


#endif /* TARGET_IS_PIC32MX */
    }

  return 1;
} /* static int gldelf32pic32mx_parse_args ()*/

