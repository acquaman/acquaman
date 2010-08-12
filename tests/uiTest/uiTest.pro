# #####################################################################
# QMake project file for reixsdaq.  			January 2010. mark.boots@usask.ca
# Note: Set EPICS_INCLUDE_DIRS, EPICS_LIB_DIR, and PHONON_INCLUDE_DIR correctly for platform
# #####################################################################
HOME_FOLDER = $$system(echo $HOME)

macx {
	EPICS_INCLUDE_DIRS = /Users/mboots/dev/epics/14-11/base/include \
		/Users/mboots/dev/epics/14-11/base/include/os/Darwin
	EPICS_LIB_DIR = /Users/mboots/dev/epics/14-11/base/lib/darwin-x86
	MPLOT_INCLUDE_DIR = /Users/mboots/dev/MPlot/src

	GSL_INCLUDE_DIR=/Users/mboots/dev/gsl-install/include
	GSL_LIB=-L/Users/mboots/dev/gsl-install/lib -lgsl
	GSL_CBLAS_LIB=-L/Users/mboots/dev/gsl-install/lib -lgslcblas
}
linux-g++ {
	EPICS_INCLUDE_DIRS = $$HOME_FOLDER/beamline/programming/epics/base/include \
			$$HOME_FOLDER/beamline/programming/epics/base/include/os/Linux
	EPICS_LIB_DIR = $$HOME_FOLDER/beamline/programming/epics/base/lib/linux-x86

	# include path for MPlot library (header-files only)
	MPLOT_INCLUDE_DIR = $$HOME_FOLDER/beamline/programming/MPlot/src

	GSL_LIB=-lgsl
	GSL_CBLAS_LIB=-lgslcblas
}
QT += core \
	phonon \
	network \
	sql \
	testlib
TARGET = test-ui
DESTDIR = build
DEPENDPATH += . \
	../../ \
	../../source
INCLUDEPATH += . \
	../../ \
	../../source
INCLUDEPATH += $$EPICS_INCLUDE_DIRS
INCLUDEPATH += $$MPLOT_INCLUDE_DIR
INCLUDEPATH += $$GSL_INCLUDE_DIR

LIBS += $$GSL_LIB
LIBS += $$GSL_CBLAS_LIB

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
HEADERS +=	../MPlot/src/MPlot/MPlot.h	\
	../MPlot/src/MPlot/MPlotAbstractTool.h	\
	../MPlot/src/MPlot/MPlotAxis.h	\
	../MPlot/src/MPlot/MPlotColorMap.h	\
	../MPlot/src/MPlot/MPlotImage.h	\
	../MPlot/src/MPlot/MPlotImageData.h	\
	../MPlot/src/MPlot/MPlotItem.h	\
	../MPlot/src/MPlot/MPlotLegend.h	\
	../MPlot/src/MPlot/MPlotMarker.h	\
	../MPlot/src/MPlot/MPlotObservable.h	\
	../MPlot/src/MPlot/MPlotObserver.h	\
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
	source/AMSettings.h	\
	source/AMUserOptions.h	\
	source/dataman/AMChannel.h	\
	source/dataman/AMDatabase.h	\
	source/dataman/AMDatabaseDefinition.h	\
	source/dataman/AMDataTree.h	\
	source/dataman/AMDataTreeColumn.h	\
	source/dataman/AMDbLoader.h	\
	source/dataman/AMDbObject.h	\
	source/dataman/AMDetectorInfo.h	\
	source/dataman/AMExperiment.h	\
	source/dataman/AMFirstTimeController.h	\
	source/dataman/AMRun.h	\
	source/dataman/AMSample.h	\
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
	source/ui/AMComboBox.h	\
	source/ui/AMNewRunDialog.h  \
	source/ui/AMCramBarHorizontal.h	\
	source/ui/AMFirstTimeWidget.h	\
	source/ui/AMScanView.h	\
	source/ui/AMSidebar.h

FORMS +=
SOURCES +=	source/AMErrorMonitor.cpp	\
	source/AMSettings.cpp	\
	source/dataman/AMChannel.cpp	\
	source/dataman/AMDatabase.cpp	\
	source/dataman/AMDatabaseDefinition.cpp	\
	source/dataman/AMDbObject.cpp	\
	source/dataman/AMDetectorInfo.cpp	\
	source/dataman/AMExperiment.cpp	\
	source/dataman/AMFirstTimeController.cpp	\
	source/dataman/AMRun.cpp	\
	source/dataman/AMSample.cpp	\
	source/dataman/AMScan.cpp	\
	source/dataman/AMScanSetModel.cpp	\
	source/dataman/AMXASScan.cpp	\
	source/dataman/SGMLegacyFileImporter.cpp	\
	source/muParser/muParser.cpp	\
	source/muParser/muParserBase.cpp	\
	source/muParser/muParserBytecode.cpp	\
	source/muParser/muParserCallback.cpp	\
	source/muParser/muParserError.cpp	\
	source/muParser/muParserTokenReader.cpp	\
	source/ui/AMComboBox.cpp	\
	source/ui/AMNewRunDialog.cpp  \
	source/ui/AMCramBarHorizontal.cpp	\
	source/ui/AMScanView.cpp	\
	source/ui/AMSidebar.cpp	\
	tests.cpp
RESOURCES =	source/icons/icons.qrc
