:
echo "Building generator tools"

CMD_VERSION=${1:-"0_00"};
CMD_RVERSION=${2:-0x7a};

(cd generator; make)

(
  # for some reason these come out of cvs read only
  chmod a+w c30/*.res
  if [ -d ../../XC16_ps_staging ]; then
    cd c30
    for f in *.res; do
      if [ "`diff -q ../../../XC16_ps_staging/resource/$f $f`" != "" ]; then
         echo "*** $f in XC16_ps_staging differs; copying from staging area"
         cp -f ../../../XC16_ps_staging/resource/$f .
      fi
    done
  fi
)

VERSION_ID=`echo ${MCHP_VERSION:-$CMD_VERSION} | perl -n -e 'if (/.*(\d+)[\._](\d\d?).*/) { print "$1.$2" }'`

echo "Building C30 resource data files for version [$VERSION_ID]"

#(cd c30; ../generator/rg +f 0x7 -DMCHP_VERSION=${VERSION_ID} -DMCHP_RESOURCE=${MCHP_RESOURCE:-$CMD_RVERSION} c30_devices.res c30_device.info dsPIC.LanguageToolSuite)
#(cd c30; ../generator/rg +f 24 -DMCHP_VERSION=${VERSION_ID} -DMCHP_RESOURCE=${MCHP_RESOURCE:-$CMD_RVERSION} c30_devices.res c30_device.info PIC24.LanguageToolSuite)
(cd c30; ../generator/rg -DMCHP_VERSION=${VERSION_ID} -DMCHP_RESOURCE=${MCHP_RESOURCE:-$CMD_RVERSION} c30_devices.res c30_device.info combo.LanguageToolSuite deviceSupport.xml)
(cd c30; ../generator/rg +s -DMCHP_VERSION=${VERSION_ID} -DMCHP_RESOURCE=${MCHP_RESOURCE:-$CMD_RVERSION} c30_devices.res c30_device.info)

