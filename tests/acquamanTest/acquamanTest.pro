# #####################################################################
# QMake project file for acquaman-module unit tests.  			March 2010. mark.boots@usask.ca
# Note: Set EPICS_INCLUDE_DIRS, EPICS_LIB_DIR, and PHONON_INCLUDE_DIR correctly for platform
# #####################################################################
macx {
	EPICS_INCLUDE_DIRS = /Users/mboots/dev/epics/14-11/base/include \
		/Users/mboots/dev/epics/14-11/base/include/os/Darwin
	EPICS_LIB_DIR = /Users/mboots/dev/epics/14-11/base/lib/darwin-x86
	PHONON_INCLUDE_DIR = /Library/Frameworks/phonon.framework/Versions/Current/Headers
	MPLOT_INCLUDE_DIR = /Users/mboots/dev/MPlot/src
}
linux-g++ {
	EPICS_INCLUDE_DIRS = /home/reixs/beamline/programming/epics/base/include \
		/home/reixs/beamline/programming/epics/base/include/os/Linux
	EPICS_LIB_DIR = /home/reixs/beamline/programming/epics/base/lib/linux-x86
	PHONON_INCLUDE_DIR = /usr/include/qt4/phonon

	# include path for MPlot library (header-files only)
	MPLOT_INCLUDE_DIR = /home/reixs/beamline/programming/MPlot/src
}
QT += core \
	phonon \
	network \
	sql
CONFIG += qtestlib
TARGET = test-acquaman
DESTDIR = ../../build
DEPENDPATH += . \
	../../ \
	../../source
INCLUDEPATH += . \
	../../ \
	../../source
INCLUDEPATH += $$EPICS_INCLUDE_DIRS
INCLUDEPATH += $$PHONON_INCLUDE_DIR
INCLUDEPATH += $$MPLOT_INCLUDE_DIR

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
HEADERS += source/beamline/AMDiagnosticPaddle.h \
	source/beamline/AMLoadLock.h \
	source/beamline/AMSampleHolder.h \
	source/beamline/AMAmpDetector.h \
	source/beamline/AMSpectrometer.h \
	source/beamline/AMMono.h \
	source/beamline/AMVariableAperture.h \
	source/beamline/AMInsertionDevice.h \
	source/beamline/AMProcessVariable.h \
	source/beamline/AMBeamline.h \
	source/beamline/AMControl.h \
	source/AMSettings.h \
	source/acquaman/AMScanController.h \
	source/acquaman/AMScanConfiguration.h \
	source/acquaman/AMDacqScanController.h \
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
	source/acquaman/dacq3_2/OutputHandler/acqTextSpectrumOutput.h \
	source/acquaman/dacq3_2/qepicsadvacq.h \
	source/acquaman/dacq3_2/acqLibHelper.h \
	source/acquaman/AMXASScanConfiguration.h \
	source/beamline/SGMBeamline.h \
	source/beamline/AMControlState.h \
	source/acquaman/SGM/SGMXASDacqScanController.h \
	source/AMErrorMonitor.h \
	source/beamline/AMControlSet.h \
	source/acquaman/AMRegion.h \
	source/acquaman/AMRegionsList.h \
	source/acquaman/SGM/SGMXASScanController.h \
	source/acquaman/SGM/SGMXASScanConfiguration.h \
	source/acquaman/SGM/SGMScanConfiguration.h \
	../../source/dataman/AMXASScan.h \
	../../source/dataman/AMScan.h \
	../../source/dataman/AMDbObject.h \
	../../source/dataman/AMDatabase.h \
	../../source/dataman/AMChannel.h \
	../../source/muParser/muParserTokenReader.h \
	../../source/muParser/muParserToken.h \
	../../source/muParser/muParserStack.h \
	../../source/muParser/muParserFixes.h \
	../../source/muParser/muParserError.h \
	../../source/muParser/muParserDef.h \
	../../source/muParser/muParserCallback.h \
	../../source/muParser/muParserBytecode.h \
	../../source/muParser/muParserBase.h \
	../../source/muParser/muParser.h \
	../../../MPlot/src/MPlot/MPlotWidget.h \
	../../../MPlot/src/MPlot/MPlotTools.h \
	../../../MPlot/src/MPlot/MPlotSeriesData.h \
	../../../MPlot/src/MPlot/MPlotSeries.h \
	../../../MPlot/src/MPlot/MPlotPoint.h \
	../../../MPlot/src/MPlot/MPlotMarker.h \
	../../../MPlot/src/MPlot/MPlotLegend.h \
	../../../MPlot/src/MPlot/MPlotItem.h \
	../../../MPlot/src/MPlot/MPlotImageData.h \
	../../../MPlot/src/MPlot/MPlotImage.h \
	../../../MPlot/src/MPlot/MPlotColorMap.h \
	../../../MPlot/src/MPlot/MPlotAxis.h \
	../../../MPlot/src/MPlot/MPlotAbstractTool.h \
	../../../MPlot/src/MPlot/MPlot.h \
	../../../MPlot/src/MPlot/MPlotSceneAndView.h \
	../../source/dataman/AMDataTree.h \
	../../source/AMObserver.h \
	../../source/AMObservable.h \
	../../../MPlot/src/MPlot/MPlotSceneAndView.h \
	source/beamline/AMDetector.h \
	source/acquaman/AMAcqScanSpectrumOutput.h \
	source/ui/AMScanView.h \
	source/dataman/AMScanSetModel.h \
	source/beamline/AMAbstractDetector.h

# FORMS   +=
SOURCES += source/beamline/AMDiagnosticPaddle.cpp \
	source/beamline/AMLoadLock.cpp \
	source/beamline/AMSampleHolder.cpp \
	source/beamline/AMAmpDetector.cpp \
	source/beamline/AMSpectrometer.cpp \
	source/beamline/AMMono.cpp \
	source/beamline/AMVariableAperture.cpp \
	source/beamline/AMInsertionDevice.cpp \
	source/beamline/AMProcessVariable.cpp \
	source/beamline/AMBeamline.cpp \
	source/beamline/AMControl.cpp \
	source/beamline/AMPVNames.cpp \
	source/AMSettings.cpp \
	source/acquaman/AMScanController.cpp \
	source/acquaman/AMScanConfiguration.cpp \
	source/acquaman/AMDacqScanController.cpp \
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
	source/acquaman/dacq3_2/OutputHandler/acqTextSpectrumOutput.cpp \
	source/acquaman/dacq3_2/qepicsadvacq.cpp \
	source/acquaman/dacq3_2/acqLibHelper.c \
	source/acquaman/AMXASScanConfiguration.cpp \
	source/beamline/SGMBeamline.cpp \
	source/beamline/AMControlState.cpp \
	source/acquaman/SGM/SGMXASDacqScanController.cpp \
	source/AMErrorMonitor.cpp \
	source/beamline/AMControlSet.cpp \
	source/acquaman/AMRegion.cpp \
	source/acquaman/AMRegionsList.cpp \
	source/acquaman/SGM/SGMXASScanController.cpp \
	source/acquaman/SGM/SGMXASScanConfiguration.cpp \
	source/acquaman/SGM/SGMScanConfiguration.cpp \
	tests.cpp \
	../../source/dataman/AMXASScan.cpp \
	../../source/dataman/AMScan.cpp \
	../../source/dataman/AMDbObject.cpp \
	../../source/dataman/AMDatabase.cpp \
	../../source/dataman/AMChannel.cpp \
	../../source/muParser/muParserTokenReader.cpp \
	../../source/muParser/muParserError.cpp \
	../../source/muParser/muParserCallback.cpp \
	../../source/muParser/muParserBytecode.cpp \
	../../source/muParser/muParserBase.cpp \
	../../source/muParser/muParser.cpp \
	source/beamline/AMDetector.cpp \
	source/acquaman/AMAcqScanSpectrumOutput.cpp \
	source/ui/AMScanView.cpp \
	source/dataman/AMScanSetModel.cpp \
	source/beamline/AMAbstractDetector.cpp
