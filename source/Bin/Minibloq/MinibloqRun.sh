#!/bin/bash

OS=`uname`
KERNEL=`uname -r`
MACH=`uname -m`

PWD=`dirname $0`

cd $PWD

function set_execute_permission {
#find ./ -type f -perm +111 | xargs -n 1 -I {} echo chmod +x {}
#find ./ -type l -perm +111 | xargs -n 1 -I {} echo chmod +x {}
#find ./ -type l -perm +111 -exec ls -l {} \; | awk '{print "ln -sf "$11" "$9}'

#echo set_execute_permission

chmod +x ./Components/Minibloq/v1.0/Minibloq-amd64
chmod +x ./Components/Minibloq/v1.0/Minibloq-i386
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-gprof
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-gcc-4.3.5
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-readelf
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-size
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-gcc
chmod +x ./Components/avrlinux/i386/v1.0/bin/avrdude
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-ld
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-ar
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-ranlib
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-objcopy
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-c++filt
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-gcov
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-as
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-cpp
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-gccbug
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-man
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-g++
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-strings
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-addr2line
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-nm
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-objdump
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-strip
chmod +x ./Components/avrlinux/i386/v1.0/bin/avr-c++
chmod +x ./Components/avrlinux/i386/v1.0/lib/gcc/avr/4.3.5/cc1plus
chmod +x ./Components/avrlinux/i386/v1.0/lib/gcc/avr/4.3.5/collect2
chmod +x ./Components/avrlinux/i386/v1.0/lib/gcc/avr/4.3.5/cc1
chmod +x ./Components/avrlinux/i386/v1.0/lib/gcc/avr/4.3.5/install-tools/fixproto
chmod +x ./Components/avrlinux/i386/v1.0/lib/gcc/avr/4.3.5/install-tools/mkinstalldirs
chmod +x ./Components/avrlinux/i386/v1.0/lib/gcc/avr/4.3.5/install-tools/fix-header
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-gprof
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-gcc-4.3.5
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-readelf
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-size
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-gcc
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avrdude
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-ld
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-ar
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-ranlib
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-objcopy
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-c++filt
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-gcov
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-as
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-cpp
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-gccbug
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-man
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-g++
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-strings
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-addr2line
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-nm
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-objdump
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-strip
chmod +x ./Components/avrlinux/amd64/v1.0/bin/avr-c++
chmod +x ./Components/avrlinux/amd64/v1.0/lib/gcc/avr/4.3.5/cc1plus
chmod +x ./Components/avrlinux/amd64/v1.0/lib/gcc/avr/4.3.5/collect2
chmod +x ./Components/avrlinux/amd64/v1.0/lib/gcc/avr/4.3.5/cc1
chmod +x ./Components/avrlinux/amd64/v1.0/lib/gcc/avr/4.3.5/install-tools/fixproto
chmod +x ./Components/avrlinux/amd64/v1.0/lib/gcc/avr/4.3.5/install-tools/mkinstalldirs
chmod +x ./Components/avrlinux/amd64/v1.0/lib/gcc/avr/4.3.5/install-tools/fix-header

chmod +x ./Components/avrlinux/i386/v1.0/lib/avr/bin/objdump
chmod +x ./Components/avrlinux/i386/v1.0/lib/avr/bin/ranlib
chmod +x ./Components/avrlinux/i386/v1.0/lib/avr/bin/as
chmod +x ./Components/avrlinux/i386/v1.0/lib/avr/bin/ld
chmod +x ./Components/avrlinux/i386/v1.0/lib/avr/bin/strip
chmod +x ./Components/avrlinux/i386/v1.0/lib/avr/bin/objcopy
chmod +x ./Components/avrlinux/i386/v1.0/lib/avr/bin/ar
chmod +x ./Components/avrlinux/i386/v1.0/lib/avr/bin/nm
chmod +x ./Components/avrlinux/amd64/v1.0/lib/avr/bin/objdump
chmod +x ./Components/avrlinux/amd64/v1.0/lib/avr/bin/ranlib
chmod +x ./Components/avrlinux/amd64/v1.0/lib/avr/bin/as
chmod +x ./Components/avrlinux/amd64/v1.0/lib/avr/bin/ld
chmod +x ./Components/avrlinux/amd64/v1.0/lib/avr/bin/strip
chmod +x ./Components/avrlinux/amd64/v1.0/lib/avr/bin/objcopy
chmod +x ./Components/avrlinux/amd64/v1.0/lib/avr/bin/ar
chmod +x ./Components/avrlinux/amd64/v1.0/lib/avr/bin/nm
chmod +x ./Components/hid_bootloader_cli/v1.0/hid_bootloader_cli.Linux


ln -sf ../../../bin/avr-objdump ./Components/avrlinux/i386/v1.0/lib/avr/bin/objdump
ln -sf ../../../bin/avr-ranlib ./Components/avrlinux/i386/v1.0/lib/avr/bin/ranlib
ln -sf ../../../bin/avr-as ./Components/avrlinux/i386/v1.0/lib/avr/bin/as
ln -sf ../../../bin/avr-ld ./Components/avrlinux/i386/v1.0/lib/avr/bin/ld
ln -sf ../../../bin/avr-strip ./Components/avrlinux/i386/v1.0/lib/avr/bin/strip
ln -sf ../../../bin/avr-objdump ./Components/avrlinux/i386/v1.0/lib/avr/bin/objdump2
ln -sf ../../../bin/avr-objcopy ./Components/avrlinux/i386/v1.0/lib/avr/bin/objcopy
ln -sf ../../../bin/avr-ar ./Components/avrlinux/i386/v1.0/lib/avr/bin/ar
ln -sf ../../../bin/avr-nm ./Components/avrlinux/i386/v1.0/lib/avr/bin/nm
ln -sf ../../../bin/avr-objdump ./Components/avrlinux/amd64/v1.0/lib/avr/bin/objdump
ln -sf ../../../bin/avr-ranlib ./Components/avrlinux/amd64/v1.0/lib/avr/bin/ranlib
ln -sf ../../../bin/avr-as ./Components/avrlinux/amd64/v1.0/lib/avr/bin/as
ln -sf ../../../bin/avr-ld ./Components/avrlinux/amd64/v1.0/lib/avr/bin/ld
ln -sf ../../../bin/avr-strip ./Components/avrlinux/amd64/v1.0/lib/avr/bin/strip
ln -sf ../../../bin/avr-objcopy ./Components/avrlinux/amd64/v1.0/lib/avr/bin/objcopy
ln -sf ../../../bin/avr-ar ./Components/avrlinux/amd64/v1.0/lib/avr/bin/ar
ln -sf ../../../bin/avr-nm ./Components/avrlinux/amd64/v1.0/lib/avr/bin/nm

echo set_execute_permission

}

set_execute_permission

if [ "${OS}" = "Linux" ] ; then
    echo $OS
    echo $KERNEL
    echo $MACH
    if [ "${MACH}" = "x86_64" ] ; then
        export LD_LIBRARY_PATH=$PWD/Components/XDFRunTime/Linux/v1.0/amd64
        $PWD/Components/Minibloq/v1.0/Minibloq-amd64
    else
#    if [ "${MACH}" = "i386" ] || [ "${MACH}" = "i486" ] || [ "${MACH}" = "i586" ] || [ "${MACH}" = "i686" ] ; then
        export LD_LIBRARY_PATH=$PWD/Components/XDFRunTime/Linux/v1.0/i386
        $PWD/Components/Minibloq/v1.0/Minibloq-i386
    fi
else
    xmessage "Unsupported operating system"
fi


