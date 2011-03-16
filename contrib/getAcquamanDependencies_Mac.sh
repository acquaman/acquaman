#!/bin/bash

#edit this based on your development folder. Should point to the acquaman/contrib folder.
BASEDIR=$HOME/dev/acquaman/contrib
cd $BASEDIR

# GSL:
echo "Downloading and building GSL"

curl http://mirror.csclub.uwaterloo.ca/gnu/gsl/gsl-1.14.tar.gz > gsl.tar.gz
tar -zxvf gsl.tar.gz
cd gsl-1.14
./configure --disable-shared --prefix=$BASEDIR/gsl-install
make
make install
cd ../


# epics
echo "Downloading and building EPICS"

curl http://www.aps.anl.gov/epics/download/base/baseR3.14.12.tar.gz > epics.tar.gz
tar -zxvf epics.tar.gz
cd base-3.14.12
export EPICS_HOST_ARCH=darwin-x86

# NOTE! Must edit base-3.14.12/configure/os/CONFIG_SITE.Common.darwin-x86 to enable universal (i386 and x86_64) libs. (or just x86_64, if you prefer)
# ARCH_CLASS = i386 x86_64
#(note: to get iOs cross-compile libs, use CONFIG_SITE.darwin-x86.Common)
# Also, if you don't have readline available (in 64bit dev), comment out COMMANDLINE_LIBRARY=READLINE in configure/os/CONFIG.darwinCommon.darwinCommon

make
cd ../

# vlc
echo "Downloading and buidling VLC"

git clone git://git.videolan.org/vlc/vlc-1.1.git --depth=1
cd vlc-1.1
export CC=/Developer/usr/bin/llvm-gcc-4.2
export CXX=/Developer/usr/bin/llvm-g++-4.2
export OBJC=/Developer/usr/bin/llvm-gcc-4.2

cd extras/contrib
./bootstrap x86_64-apple-darwin10
make

cd ../../
./bootstrap

./configure --build=x86_64-apple-darwin10 --prefix=$BASEDIR/vlc-install
make
make install



# Temporary notes after struggling with VLC versions:

#Most recent VLC build:
#Missing BWToolkit in extras/contrib.
#Download from http://brandonwalkin.com/blog/bwtfiles/BWToolkit.zip and put into extras/contrib/hosts/x86_64-apple-darwin10


# Note: Alternative VLC method: download VLC 1.1 branch nightly build (ex: http://nightlies.videolan.org/build/macosx-intel/1.1-branch-intel64-20110309-0205.zip)
# and use alternative locations specified in acuamanCommon.pro.  This requires copying the lib and plugins folders from VLC.app/Contents/MacOS into Acquaman.app/Contents/MacOS and removing the plugin-path designation in AMVideoWidget_mac.mm.







#New approach to VLC: using phonon-backend-vlc and phonon...

#Latest stable phonon-backend-vlc: http://mirrors.mit.edu/kde/stable/phonon-backend-vlc/0.3.1/src/phonon-backend-vlc-0.3.1.tar.bz2

#VLC (libVLC): using git 1.1 branch as above

#Phonon: using default that comes with Qt 4.7.0 (version: )


#Trying Qt 4.7.2 built from source so that I can debug properly.  Configure with:

#./configure -arch x86_64

#(cocoa by default)