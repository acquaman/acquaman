#!/bin/bash

source /home/beamline/tools/qt/startup/startup.qt-4.7.3.sh
export PATH=/home/beamline/tools/gstreamer-0.10.35/gstreamer-install/bin:/home/beamline/tools/gstreamer-0.10.35/deps/bin:$PATH
export LD_LIBRARY_PATH=/home/beamline/tools/gstreamer-0.10.35/gstreamer-install/lib:/home/beamline/tools/gstreamer-0.10.35/deps/lib:$LD_LIBRARY_PATH

$1 --enableCrashMonitor -graphicssystem raster "${@:2}"
