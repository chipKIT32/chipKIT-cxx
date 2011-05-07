/* Override the N32 ABI default from linux64.h.  */

/* Force the default endianness and ABI flags onto the command line
   in order to make the other specs easier to write.  */
#undef DRIVER_SELF_SPECS
#define DRIVER_SELF_SPECS \
  BASE_DRIVER_SELF_SPECS, \
  LINUX_DRIVER_SELF_SPECS \
  " %{!EB:%{!EL:%(endian_spec)}}" \
  " %{!mabi=*: -mabi=32}" \
  " %{funwind-tables|fno-unwind-tables|ffreestanding|nostdlib:;: -funwind-tables}"

#undef SUBTARGET_CC1_SPEC
#define SUBTARGET_CC1_SPEC "%{O3:-funroll-loops --param max-unroll-times=2}"

