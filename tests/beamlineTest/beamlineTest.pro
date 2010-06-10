# #####################################################################
# QMake project file for reixsdaq.  			January 2010. mark.boots@usask.ca
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
TARGET = test-beamline
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
	source/beamline/AMPVNames.h \
	source/dataman/AMDatabase.h \
	source/dataman/AMDbLoader.h \
	source/AMSettings.h \
	source/beamline/SGMBeamline.h \
	source/beamline/AMControlState.h \
	source/AMErrorMonitor.h \
	source/beamline/AMControlSet.h \
	source/acquaman/AMRegion.h \
	source/acquaman/AMRegionsList.h \
	source/dataman/AMXASScan.h \
	source/dataman/AMScan.h \
	source/dataman/AMDbObject.h \
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
	../../source/dataman/AMDataTree.h \
	../../source/AMObserver.h \
	../../source/AMObservable.h \
	../../../MPlot/src/MPlot/MPlotSceneAndView.h \
	source/beamline/AMDetector.h \
	source/beamline/AMAbstractDetector.h
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
	source/beamline/SGMBeamline.cpp \
	source/beamline/AMControlState.cpp \
	source/AMErrorMonitor.cpp \
	source/beamline/AMControlSet.cpp \
	tests.cpp \
	source/dataman/AMXASScan.cpp \
	source/acquaman/AMRegion.cpp \
	source/acquaman/AMRegionsList.cpp \
	source/dataman/AMScan.cpp \
	source/dataman/AMDbObject.cpp \
	../../source/dataman/AMDatabase.cpp \
	../../source/dataman/AMChannel.cpp \
	../../source/muParser/muParserTokenReader.cpp \
	../../source/muParser/muParserError.cpp \
	../../source/muParser/muParserCallback.cpp \
	../../source/muParser/muParserBytecode.cpp \
	../../source/muParser/muParserBase.cpp \
	../../source/muParser/muParser.cpp \
	source/beamline/AMDetector.cpp \
	source/beamline/AMAbstractDetector.cpp

