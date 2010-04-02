# #####################################################################
# QMake project file for reixsdaq.  			January 2010. mark.boots@usask.ca
# Note: Set EPICS_INCLUDE_DIRS, EPICS_LIB_DIR, and PHONON_INCLUDE_DIR correctly for platform
# #####################################################################
macx { 
    EPICS_INCLUDE_DIRS = /Users/mboots/dev/epics/14-11/base/include \
        /Users/mboots/dev/epics/14-11/base/include/os/Darwin
    EPICS_LIB_DIR = /Users/mboots/dev/epics/14-11/base/lib/darwin-x86
    PHONON_INCLUDE_DIR = /Library/Frameworks/phonon.framework/Versions/Current/Headers
}
linux-g++ { 
    EPICS_INCLUDE_DIRS = /home/reixs/beamline/programming/epics/base/include \
        /home/reixs/beamline/programming/epics/base/include/os/Linux
    EPICS_LIB_DIR = /home/reixs/beamline/programming/epics/base/lib/linux-x86
    PHONON_INCLUDE_DIR = /usr/include/qt4/phonon
}
QT += core \
    phonon \
    network \
    sql

CONFIG += qtestlib

TARGET = test-dataman
DESTDIR = ../../build
DEPENDPATH += . \
    ../../ \
    ../../source
INCLUDEPATH += . \
    ../../ \
    ../../source
INCLUDEPATH += $$EPICS_INCLUDE_DIRS
INCLUDEPATH += $$PHONON_INCLUDE_DIR

# Epics channel access linking:
LIBS += -L$$EPICS_LIB_DIR
LIBS += -lca \
    -lCom
macx:QMAKE_LFLAGS_RPATH += "$$EPICS_LIB_DIR"
linux-g++ { 
    QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$EPICS_LIB_DIR"
    QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$EPICS_LIB_DIR"
}

# include and library paths for libxml:
INCLUDEPATH += /usr/include/libxml2
LIBS += -lxml2

# Input
HEADERS += source/dataman/AMDbObject.h \
    source/dataman/AMScan.h \
    source/dataman/AMChannel.h \
    source/beamline/AMPVNames.h \
    source/dataman/AMDatabase.h \
    source/dataman/AMDbLoader.h \
    source/AMSettings.h \
	source/AMErrorMonitor.h

SOURCES += source/dataman/AMScan.cpp \
    source/dataman/AMChannel.cpp \
    source/beamline/AMPVNames.cpp \
    source/dataman/AMDatabase.cpp \
    source/dataman/AMDbObject.cpp \
    source/AMSettings.cpp \
	source/AMErrorMonitor.cpp \
    tests.cpp

