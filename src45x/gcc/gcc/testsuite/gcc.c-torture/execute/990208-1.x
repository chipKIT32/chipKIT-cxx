
# On ARM, with -Os, some of the functions that this test
# expects to be inlined are not inlined for code size
# reasons.  This is not a bug, it's intentional,
# so stop this test from running.
set torture_eval_before_compile {
  if { [istarget "arm-*-*"] && [string match {*-Os*} "$option"] } {
    continue
  }
}

return 0