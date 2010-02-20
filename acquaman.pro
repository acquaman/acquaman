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
    network
TARGET = 
DEPENDPATH += . \
    source
INCLUDEPATH += . \
    source
INCLUDEPATH += $$EPICS_INCLUDE_DIRS
INCLUDEPATH += $$PHONON_INCLUDE_DIR

# Epics channel access linking:
LIBS += -L$$EPICS_LIB_DIR
LIBS += -lca -lCom

macx {
	QMAKE_LFLAGS_RPATH +="$$EPICS_LIB_DIR"
}
linux-g++ {
	QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$EPICS_LIB_DIR"
	QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$EPICS_LIB_DIR"
}

# Input
HEADERS += source/beamline/DiagnosticPaddle.h \
    source/beamline/LoadLock.h \
    source/beamline/SampleHolder.h \
    source/beamline/AmpDetector.h \
    source/beamline/Spectrometer.h \
    source/beamline/Mono.h \
    source/beamline/VariableAperture.h \
    source/beamline/InsertionDevice.h \
    source/ui/AbsorptionScanController.h \
    source/ui/ConnectionSettings.h \
    source/ui/EmissionScanController.h \
    source/ui/ExpAlbum.h \
    source/ui/GratingResolution.h \
    source/ui/MainWindow.h \
    source/ui/PeriodicTable.h \
    source/ui/ProtocolViewer.h \
    source/ui/SamplePositions.h \
    source/ui/Scheduler.h \
    source/ui/BottomBar.h \
    source/ui/Sidebar.h \
    source/ui/CamWidget.h \
    source/beamline/ProcessVariable.h \
    source/beamline/Beamline.h \
    source/beamline/Control.h \
    source/ui/NumericControl.h
FORMS += source/ui/AbsorptionScanController.ui \
    source/ui/ConnectionSettings.ui \
    source/ui/EmissionScanController.ui \
    source/ui/ExpAlbum.ui \
    source/ui/GratingResolution.ui \
    source/ui/PeriodicTable.ui \
    source/ui/ProtocolViewer.ui \
    source/ui/SamplePositions.ui \
    source/ui/Scheduler.ui \
    source/ui/BottomBar.ui
SOURCES += source/beamline/DiagnosticPaddle.cpp \
    source/beamline/LoadLock.cpp \
    source/beamline/SampleHolder.cpp \
    source/beamline/AmpDetector.cpp \
    source/beamline/Spectrometer.cpp \
    source/beamline/Mono.cpp \
    source/beamline/VariableAperture.cpp \
    source/beamline/InsertionDevice.cpp \
    source/ui/SamplePositions.cpp \
    source/ui/BottomBar.cpp \
    source/ui/CamWidget.cpp \
    source/ui/MainWindow.cpp \
    source/main.cpp \
    source/beamline/ProcessVariable.cpp \
    source/beamline/Beamline.cpp \
    source/beamline/Control.cpp \
    source/ui/NumericControl.cpp
RESOURCES = source/icons/icons.qrc
