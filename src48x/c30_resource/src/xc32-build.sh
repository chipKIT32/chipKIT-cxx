:
echo "Building generator tools"

CMD_VERSION=${1:-"0_00"};
CMD_RVERSION=${2:-0x7a};

(cd generator; make clean; make PIC32=-DPIC32)


VERSION_ID=`echo ${MCHP_VERSION:-$CMD_VERSION} | perl -n -e 'if (/.*(\d+)[\._](\d\d?).*/) { print "$1.$2" }'`

echo "Building xc32 resource data files for version [$VERSION_ID]"


echo "(cd xc32; ../generator/rg +v $VERSION_ID (`echo $MCHP_RESOURCE | egrep -o \"[[:alnum:]]\"`) -DMCHP_VERSION=${VERSION_ID} -DMCHP_RESOURCE=${MCHP_RESOURCE:-$CMD_RVERSION} xc32_devices.res xc32_device.info .LanguageToolSuite deviceSupport.xml)"

(cd xc32; ../generator/rg +v "$VERSION_ID (`echo $MCHP_RESOURCE | egrep -o \"[[:alnum:]]\"`)" -DMCHP_VERSION=${VERSION_ID} -DMCHP_RESOURCE=${MCHP_RESOURCE:-$CMD_RVERSION} xc32_devices.res xc32_device.info .LanguageToolSuite deviceSupport.xml)

echo "(cd xc32; ../generator/rg +s -DMCHP_VERSION=${VERSION_ID} -DMCHP_RESOURCE=${MCHP_RESOURCE:-$CMD_RVERSION} xc32_devices.res xc32_device.info)"
(cd xc32; ../generator/rg +s -DMCHP_VERSION=${VERSION_ID} -DMCHP_RESOURCE=${MCHP_RESOURCE:-$CMD_RVERSION} xc32_devices.res xc32_device.info)
