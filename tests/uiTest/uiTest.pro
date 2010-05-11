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

TARGET = test-ui
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
HEADERS +=
FORMS +=
SOURCES += tests.cpp

RESOURCES = source/icons/icons.qrc
