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
    source/ui/NumericControl.h \
    source/dataman/Scan.h \
    source/dataman/Channel.h \
    source/acquaman/ScanController.h \
    source/acquaman/ScanConfiguration.h \
    source/acquaman/DacqScanController.h \
    source/acquaman/dacq3_2/epicsConnect.main.h \
    source/acquaman/dacq3_2/epicsConnect.h \
    source/acquaman/dacq3_2/acquisitionLib.main.h \
    source/acquaman/dacq3_2/acquisitionLib.internal.h \
    source/acquaman/dacq3_2/acquisitionLib.h \
    source/acquaman/dacq3_2/acqDataHandler.h \
    source/acquaman/dacq3_2/qepicsacqlocal.h \
    source/acquaman/dacq3_2/qepicsacqclass.h \
    source/acquaman/dacq3_2/OutputHandler/acqTextOutput.h \
    source/acquaman/dacq3_2/factoryQtTemplate.h \
    source/acquaman/dacq3_2/displayAlias.h \
    source/acquaman/dacq3_2/OutputHandler/acqBaseOutput.h \
    source/acquaman/dacq3_2/OutputHandler/acqProperties.h \
    source/acquaman/dacq3_2/OutputHandler/acqBaseStream.h \
    source/acquaman/dacq3_2/OutputHandler/acqFactory.h \
    source/acquaman/dacq3_2/OutputHandler/acqFileStream.h \
    source/acquaman/dacq3_2/qepicsadvacq.h \
    source/acquaman/dacq3_2/acqLibHelper.h
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
    source/ui/NumericControl.cpp \
    source/dataman/Scan.cpp \
    source/dataman/Channel.cpp \
    source/acquaman/ScanController.cpp \
    source/acquaman/ScanConfiguration.cpp \
    source/acquaman/DacqScanController.cpp \
    source/acquaman/dacq3_2/xmlWrite.cpp \
    source/acquaman/dacq3_2/xmlRead.cpp \
    source/acquaman/dacq3_2/update.c \
    source/acquaman/dacq3_2/macro.c \
    source/acquaman/dacq3_2/connector.c \
    source/acquaman/dacq3_2/channel_hash.c \
    source/acquaman/dacq3_2/channel.c \
    source/acquaman/dacq3_2/acqMotor.c \
    source/acquaman/dacq3_2/acqMonitor.c \
    source/acquaman/dacq3_2/acqMessage.c \
    source/acquaman/dacq3_2/acqLoad.c \
    source/acquaman/dacq3_2/acqExtern.c \
    source/acquaman/dacq3_2/acqActSetup.c \
    source/acquaman/dacq3_2/acqAction.c \
    source/acquaman/dacq3_2/qepicsacqlocal.cpp \
    source/acquaman/dacq3_2/qepicsacqclass.cpp \
    source/acquaman/dacq3_2/OutputHandler/acqTextOutput.cpp \
    source/acquaman/dacq3_2/displayAlias.cpp \
    source/acquaman/dacq3_2/OutputHandler/acqBaseOutput.cpp \
    source/acquaman/dacq3_2/OutputHandler/acqBaseStream.cpp \
    source/acquaman/dacq3_2/OutputHandler/acqFactory.cpp \
    source/acquaman/dacq3_2/OutputHandler/acqFileStream.cpp \
    source/acquaman/dacq3_2/qepicsadvacq.cpp \
    source/acquaman/dacq3_2/acqLibHelper.c
RESOURCES = source/icons/icons.qrc
