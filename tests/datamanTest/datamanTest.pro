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
INCLUDEPATH += $$MPLOT_INCLUDE_DIR

# Epics channel access linking:
LIBS += -L$$EPICS_LIB_DIR
LIBS += -lgsl
LIBS += -lgslcblas
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
HEADERS +=	../MPlot/src/MPlot/MPlot.h	\
	../MPlot/src/MPlot/MPlotAbstractTool.h	\
	../MPlot/src/MPlot/MPlotAxis.h	\
	../MPlot/src/MPlot/MPlotColorMap.h	\
	../MPlot/src/MPlot/MPlotImage.h	\
	../MPlot/src/MPlot/MPlotImageData.h	\
	../MPlot/src/MPlot/MPlotItem.h	\
	../MPlot/src/MPlot/MPlotLegend.h	\
	../MPlot/src/MPlot/MPlotMarker.h	\
	../MPlot/src/MPlot/MPlotPoint.h	\
	../MPlot/src/MPlot/MPlotSeries.h	\
	../MPlot/src/MPlot/MPlotSeriesData.h	\
	../MPlot/src/MPlot/MPlotTools.h	\
	../MPlot/src/MPlot/MPlotWidget.h	\
	source/AMBiHash.h	\
	source/AMErrorMonitor.h	\
	source/AMObservable.h	\
	source/AMObserver.h	\
	source/AMSettings.h	\
	source/beamline/AMAbstractDetector.h	\
	source/beamline/AMPVNames.h	\
	source/dataman/AMChannel.h	\
	source/dataman/AMDatabase.h	\
	source/dataman/AMDatabaseDefinition.h	\
	source/dataman/AMDataTree.h	\
	source/dataman/AMDataTreeColumn.h	\
	source/dataman/AMDbLoader.h	\
	source/dataman/AMDbObject.h	\
	source/dataman/AMFirstTimeController.h	\
	source/dataman/AMScan.h	\
	source/dataman/AMScanSetModel.h	\
	source/dataman/AMXASScan.h	\
	source/dataman/SGMLegacyFileImporter.h	\
	source/muParser/muParser.h	\
	source/muParser/muParserBase.h	\
	source/muParser/muParserBytecode.h	\
	source/muParser/muParserCallback.h	\
	source/muParser/muParserDef.h	\
	source/muParser/muParserError.h	\
	source/muParser/muParserFixes.h	\
	source/muParser/muParserStack.h	\
	source/muParser/muParserToken.h	\
	source/muParser/muParserTokenReader.h	\
	source/ui/AMFirstTimeWidget.h

SOURCES +=	source/dataman/AMFirstTimeController.cpp	\
	source/dataman/AMScanSetModel.cpp	\
	source/muParser/muParser.cpp	\
	source/muParser/muParserBase.cpp	\
	source/muParser/muParserBytecode.cpp	\
	source/muParser/muParserCallback.cpp	\
	source/muParser/muParserError.cpp	\
	source/muParser/muParserTokenReader.cpp	\
	source/AMErrorMonitor.cpp	\
	source/AMSettings.cpp	\
	source/beamline/AMAbstractDetector.cpp	\
	source/beamline/AMPVNames.cpp	\
	source/dataman/AMChannel.cpp	\
	source/dataman/AMDatabase.cpp	\
	source/dataman/AMDbObject.cpp	\
	source/dataman/AMScan.cpp	\
	source/dataman/AMXASScan.cpp	\
	source/dataman/SGMLegacyFileImporter.cpp	\
	tests.cpp
