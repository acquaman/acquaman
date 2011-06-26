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
