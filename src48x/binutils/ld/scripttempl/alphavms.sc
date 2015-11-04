# Linker script for Alpha VMS systems.
# Tristan Gingold <gingold@adacore.com>.

PAGESIZE=0x10000

cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
${LIB_SEARCH_DIRS}

SECTIONS
{
  ${RELOCATING+. = ${PAGESIZE};}

  \$DATA\$ ALIGN (${PAGESIZE}) : {
    *(\$DATA\$)
    *(\$LINK\$)
  }
  \$BSS\$ ALIGN (${PAGESIZE}) : {
    *(\$BSS\$)
  }
  \$CODE\$ ALIGN (${PAGESIZE}) : {
    *(\$CODE\$ *\$CODE*)
  }
  \$LITERAL\$ ALIGN (${PAGESIZE}) : {
    *(\$LITERAL\$)
    *(\$READONLY\$)
    *(\$READONLY_ADDR\$)
  }

  \$DWARF\$ ALIGN (${PAGESIZE}) : {
    \$dwarf2.debug_pubtypes = .;
    *(debug_pubtypes)
    \$dwarf2.debug_ranges = .;
    *(debug_ranges)

    \$dwarf2.debug_abbrev = .;
    *(debug_abbrev)
    \$dwarf2.debug_aranges = .;
    *(debug_aranges)
    \$dwarf2.debug_frame = .;
    *(debug_frame)
    \$dwarf2.debug_info = .;
    *(debug_info)
    \$dwarf2.debug_line = .;
    *(debug_line)
    \$dwarf2.debug_loc = .;
    *(debug_loc)
    \$dwarf2.debug_macinfo = .;
    *(debug_macinfo)
    \$dwarf2.debug_macro = .;
    *(debug_macro)
    \$dwarf2.debug_pubnames = .;
    *(debug_pubnames)
    \$dwarf2.debug_str = .;
    *(debug_str)
    \$dwarf2.debug_zzzzzz = .;
  }

  \$DST\$ 0 : {
    *(\$DST\$)
  }
}
EOF
