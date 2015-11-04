# If you change this file, please also look at files which source this one:
# elf32locteon.sh

EMBEDDED=yes
. ${srcdir}/emulparams/elf32btsmip.sh

# Entry point symbol.
ENTRY=__start

# Start of the .text section
TEXT_START_ADDR=0x10000000

# Data and .cmvx_shared are 32M pages. Unlike what the documentation says we 
# always use (ALIGN(MAXPAGESIZE) + (. & (MAXPAGESIZE - COMMONPAGESIZE))) to 
# align data. On-disk image will have at most 4K (ELF_MAXPAGESIZE) padding 
# before data.
MAXPAGESIZE=32M
COMMONPAGESIZE=32M
# Needed to disable demand paging.
EXTRA_EM_FILE=octeonelf

# If defined, the genscripts.sh script sets TEXT_START_ADDR to its value
# for the -n/-N options.
# Define this as the TEXT_START_ADDR is different to overright the one 
# defined in elf32bmip.sh
NONPAGED_TEXT_START_ADDR=0x10000000

# Create new macro to place .cvmx_shared at the end of _end.
OCTEON_SPECIAL_SECTIONS='
   . = ALIGN(32M);   /* RBF added alignment of data */
   __cvmx_shared_start = .;
   .cvmx_shared : { *(.cvmx_shared .cvmx_shared.linkonce.*) }
   .cvmx_shared_bss : { *(.cvmx_shared_bss .cvmx_shared_bss.linkonce.*) }
   __cvmx_shared_end = .;'

OTHER_END_SYMBOLS='
   /* All symbols above this (32 bit compat space) the simulator will attribute
      to the bootloader. */
    __bootloader = 0xFFFFFFF8;'

EXECUTABLE_SYMBOLS='
   PROVIDE(__cvmx_app_init = 0);
   PROVIDE(__cvmx_app_exit = 0);
   PROVIDE(__cvmx_pci_console_write = 0);
   PROVIDE(__cvmx_pci_console_read = 0);'
