#!/bin/bash

# 1. Setup: Edit this based on your development folder. Should point to the acquaman/contrib folder.
BASEDIR=$HOME/dev/acquaman/contrib
cd $BASEDIR

# 2. GSL:
echo "Downloading and building GSL"

curl http://mirror.csclub.uwaterloo.ca/gnu/gsl/gsl-1.14.tar.gz > gsl.tar.gz
tar -zxvf gsl.tar.gz
cd gsl-1.14
./configure --disable-shared --prefix=$BASEDIR/gsl-install
make
make install
cd ../


# 3. EPICS:
echo "Downloading and building EPICS"

curl http://www.aps.anl.gov/epics/download/base/baseR3.14.12.3.tar.gz > epics.tar.gz
tar -zxvf epics.tar.gz
cd base-3.14.12
export EPICS_HOST_ARCH=darwin-x86

# NOTE! Must edit base-3.14.12/configure/os/CONFIG_SITE.Common.darwin-x86 to enable universal (i386 and x86_64) libs. (or just x86_64, if you prefer)
# ARCH_CLASS = i386 x86_64
#(note: to get iOs cross-compile libs, use CONFIG_SITE.darwin-x86.Common)
# Also, if you don't have readline available (in 64bit dev), comment out COMMANDLINE_LIBRARY=READLINE in configure/os/CONFIG.darwinCommon.darwinCommon

# NOTE! This patch is no longer necessary if you take baseR3.14.12.3.tar.gz!!!!!
# On Mac OS X Lion, you need to apply Jeff Hill's patch to remove some variable argument + multiple inheritance problems that don't work with the compiler on OS X 10.7
curl https://launchpadlibrarian.net/77048945/816678.patch > epicsMacOSXLionPatch.patch
patch -p0 < epicsMacOSXLionPatch.patch

make
cd ../


# 4: QtMobility Multimedia module (http://doc.qt.nokia.com/qtmobility/installation.html)

echo "Downloading and building QMultimediaKit from Qt Mobility"

curl http://get.qt.nokia.com/qt/add-ons/qt-mobility-opensource-src-1.1.3.tar.gz > qt-mobility.tar.gz
tar -zxf qt-mobility.tar.gz
cd qt-mobility-opensource-src-1.1.3
./configure -modules multimedia -prefix $BASEDIR/qt-mobility-install
make
sudo make install

# add to project file: QT += mobility
# MOBILITY += multimedia

# 5: CDF library:

echo "Downloading the CDF (NASA Common Data Format) library"

curl http://cdaweb.gsfc.nasa.gov/pub/cdf/dist/cdf34_0/macosX/cdf34_0-setup_universal_binary.tar.gz > cdf34_0.tar.gz
tar -zxf cdf34_0.tar.gz
echo "Please install the CDF library using the installer package."
open CDF3400ub.pkg

# 6: You may have to compile Qt, if so
	*** NOTE: Make sure you configure Qt with support for OpenSSL, other github features won't work***
