HOME_FOLDER = $$system(echo $HOME)

macx {

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = dev

		# Where the acquaman source is
		AM_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/source

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/base-3.14.12/include \
				$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/base-3.14.12/include/os/Darwin
		EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/base-3.14.12/lib/darwin-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/src
}
linux-g++ {

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# Where the acquaman source is
		AM_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/source

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/epics/base/include \
				$$HOME_FOLDER/$$DEV_PATH/epics/base/include/os/Linux
		EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/epics/base/lib/linux-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/src
}
linux-g++-32 {

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# Where the acquaman source is
		AM_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/source

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/epics/base/include \
				$$HOME_FOLDER/$$DEV_PATH/epics/base/include/os/Linux
		EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/epics/base/lib/linux-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/src
}
# The following works well for CLS beamline OPI machines, built using VMSL54.cs.clsi.ca

linux-g++-64 {

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = Sandbox/Acquaman2011/dev

		# Where the acquaman source is
		AM_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/source

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = /home/epics/src/R3.14.12/base/include \
				/home/epics/src/R3.14.12/base/include/os/Linux
		EPICS_LIB_DIR = /home/epics/src/R3.14.12/base/lib/linux-x86_64

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/src
}

# Special build paths and options for running on the Jenkins auto-build server (currently at http://beamteam.usask.ca:8080)
CONFIG(jenkins_build) {

		message("Detected Jenkins auto-build... Specifying dependency paths for the build server.")

		# Where the acquaman source is
		AM_INCLUDE_DIR = "/var/lib/jenkins/jobs/Acquaman on Linux - master branch/workspace/source"

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = /home/mark/dev/epics/base/include \
				/home/mark/dev/epics/base/include/os/Linux
		EPICS_LIB_DIR = /home/mark/dev/epics/base/lib/linux-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = "/var/lib/jenkins/jobs/MPlotOnLinux_MasterBranch/workspace/src"
}

QT +=		sql
INCLUDEPATH    += $$AM_INCLUDE_DIR \
		$$MPLOT_INCLUDE_DIR
HEADERS         = \
		$$AM_INCLUDE_DIR/dataman/AMAnalysisBlockInterface.h \
		$$AM_INCLUDE_DIR/dataman/database/AMDbObject.h \
		$$AM_INCLUDE_DIR/dataman/database/AMDbObjectSupport.h \
		$$AM_INCLUDE_DIR/dataman/database/AMDatabase.h \
		$$AM_INCLUDE_DIR/dataman/AMAnalysisBlock.h \
		$$AM_INCLUDE_DIR/dataman/AMnDIndex.h \
		$$AM_INCLUDE_DIR/dataman/AMAxisInfo.h \
		$$AM_INCLUDE_DIR/dataman/AMNumber.h \
		$$AM_INCLUDE_DIR/dataman/datasource/AMDataSource.h \
		$$AM_INCLUDE_DIR/dataman/datasource/AMDataSourceImageData.h \
		$$AM_INCLUDE_DIR/analysis/AMStandardAnalysisBlock.h \
		$$AM_INCLUDE_DIR/util/AMSettings.h \
		$$AM_INCLUDE_DIR/util/AMErrorMonitor.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlot.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotItem.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotWidget.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotAxis.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotAxisScale.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotSeries.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotPoint.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotAbstractTool.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotTools.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotMarker.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotRectangle.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotImage.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotImageData.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotColorMap.h \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotLegend.h
SOURCES         = \
		$$AM_INCLUDE_DIR/dataman/database/AMDbObject.cpp \
		$$AM_INCLUDE_DIR/dataman/database/AMDbObjectSupport.cpp \
		$$AM_INCLUDE_DIR/dataman/database/AMDatabase.cpp \
		$$AM_INCLUDE_DIR/dataman/AMAnalysisBlock.cpp \
		$$AM_INCLUDE_DIR/dataman/AMnDIndex.cpp \
		$$AM_INCLUDE_DIR/dataman/AMAxisInfo.cpp \
		$$AM_INCLUDE_DIR/dataman/AMNumber.cpp \
		$$AM_INCLUDE_DIR/dataman/datasource/AMDataSource.cpp \
		$$AM_INCLUDE_DIR/dataman/datasource/AMDataSourceImageData.cpp \
		$$AM_INCLUDE_DIR/analysis/AMStandardAnalysisBlock.cpp \
		$$AM_INCLUDE_DIR/util/AMSettings.cpp \
		$$AM_INCLUDE_DIR/util/AMErrorMonitor.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlot.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotItem.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotWidget.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotAxis.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotAxisScale.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotSeries.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotPoint.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotAbstractTool.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotTools.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotMarker.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotRectangle.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotImage.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotImageData.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotColorMap.cpp \
		$$MPLOT_INCLUDE_DIR/MPlot/MPlotLegend.cpp


