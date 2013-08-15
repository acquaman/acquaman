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
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib

		# CDFlib dependencies
		CDF_LIB = /Applications/cdf34_0-dist/lib/libcdf.a
		CDF_INCLUDE_DIR = /Applications/cdf34_0-dist/include
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
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib
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
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib

		# CDFlib dependencies
		CDF_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/cdf34_1-dist/lib
		CDF_LIB = -L$$CDF_LIB_DIR -lcdf
		CDF_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/cdf34_1-dist/include

		QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$CDF_LIB_DIR"
}
# The following works well for CLS beamline OPI machines, built using VMSL54.cs.clsi.ca

linux-g++-64 {

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# Where the acquaman source is
		AM_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/source

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = /home/epics/src/R3.14.12/base/include \
				/home/epics/src/R3.14.12/base/include/os/Linux
		EPICS_LIB_DIR = /home/epics/src/R3.14.12/base/lib/linux-x86_64

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib

		# CDFlib dependencies
		CDF_LIB = -L/home/beamline/tools/cdf/lib -lcdf
		CDF_INCLUDE_DIR = /home/beamline/tools/cdf/include
}

# Special build paths and options for running on the Jenkins auto-build server (currently at http://beamteam.usask.ca:8080)
CONFIG(jenkins_build) {

		message("Detected Jenkins auto-build... Specifying dependency paths for the build server.")

		# Where the acquaman source is
		AM_INCLUDE_DIR = "/var/lib/jenkins/jobs/AcquamanOnLinux_MasterBranch/workspace/source"

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = /home/mark/dev/epics/base/include \
				/home/mark/dev/epics/base/include/os/Linux
		EPICS_LIB_DIR = /home/mark/dev/epics/base/lib/linux-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = "/var/lib/jenkins/jobs/MPlotOnLinux_MasterBranch/workspace/include"
		MPLOT_LIB_DIR = "/var/lib/jenkins/jobs/MPlotOnLinux_MasterBranch/workspace/lib"
}


QT +=		sql
INCLUDEPATH    += $$AM_INCLUDE_DIR \
		$$MPLOT_INCLUDE_DIR \
		$$CDF_INCLUDE_DIR

LIBS +=	$$CDF_LIB \
		-L$$MPLOT_LIB_DIR -lMPlot

QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$MPLOT_LIB_DIR"
QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$MPLOT_LIB_DIR"


HEADERS         = $$AM_INCLUDE_DIR/dataman/AMFileLoaderInterface.h \
		$$AM_INCLUDE_DIR/dataman/AMScan.h \
		$$AM_INCLUDE_DIR/dataman/AMScanDictionary.h \
		$$AM_INCLUDE_DIR/dataman/AMScanParametersDictionary.h \
		$$AM_INCLUDE_DIR/util/AMTagReplacementParser.h \
		$$AM_INCLUDE_DIR/util/AMDateTimeUtils.h \
		$$AM_INCLUDE_DIR/dataman/AMRun.h \
		$$AM_INCLUDE_DIR/dataman/database/AMDbObject.h \
		$$AM_INCLUDE_DIR/dataman/database/AMDatabase.h \
		$$AM_INCLUDE_DIR/dataman/AMnDIndex.h \
		$$AM_INCLUDE_DIR/dataman/database/AMDbObjectSupport.h \
		$$AM_INCLUDE_DIR/dataman/info/AMControlInfo.h \
		$$AM_INCLUDE_DIR/dataman/info/AMControlInfoList.h \
		$$AM_INCLUDE_DIR/dataman/datasource/AMDataSource.h \
		$$AM_INCLUDE_DIR/dataman/datasource/AMRawDataSource.h \
		$$AM_INCLUDE_DIR/dataman/datastore/AMDataStore.h \
		$$AM_INCLUDE_DIR/dataman/datastore/AMInMemoryDataStore.h \
		$$AM_INCLUDE_DIR/dataman/datastore/AMCDFDataStore.h \
		$$AM_INCLUDE_DIR/dataman/datasource/AMDataSourceImageData.h \
		$$AM_INCLUDE_DIR/dataman/datasource/AMDataSourceSeriesData.h \
		$$AM_INCLUDE_DIR/dataman/AMAnalysisBlock.h \
		$$AM_INCLUDE_DIR/dataman/AMAxisInfo.h \
		$$AM_INCLUDE_DIR/dataman/AMNumber.h \
		$$AM_INCLUDE_DIR/dataman/AMSamplePre2013.h \
		$$AM_INCLUDE_DIR/acquaman/AMScanConfiguration.h \
		$$AM_INCLUDE_DIR/util/AMSettings.h \
		$$AM_INCLUDE_DIR/util/AMOrderedSetSignalSource.h \
		$$AM_INCLUDE_DIR/util/AMErrorMonitor.h \
		$$AM_INCLUDE_DIR/util/AMPeriodicTable.h \
		$$AM_INCLUDE_DIR/util/AMElement.h \
		$$AM_INCLUDE_DIR/application/AMPluginsManager.h \
		$$AM_INCLUDE_DIR/dataman/datasource/AMDataSourceImageDatawDefault.h \
		$$AM_INCLUDE_DIR/ui/dataman/AMSimpleDataSourceEditor.h \
		$$AM_INCLUDE_DIR/dataman/info/AMDetectorInfo.h \
		$$AM_INCLUDE_DIR/dataman/info/AMDetectorInfoSet.h \
		$$AM_INCLUDE_DIR/dataman/AMTextStream.h \
		$$AM_INCLUDE_DIR/dataman/database/AMConstDbObject.h

SOURCES         = $$AM_INCLUDE_DIR/dataman/AMScan.cpp \
		$$AM_INCLUDE_DIR/dataman/AMScanDictionary.cpp \
		$$AM_INCLUDE_DIR/dataman/AMScanParametersDictionary.cpp \
		$$AM_INCLUDE_DIR/util/AMTagReplacementParser.cpp \
		$$AM_INCLUDE_DIR/util/AMDateTimeUtils.cpp \
		$$AM_INCLUDE_DIR/dataman/AMRun.cpp \
		$$AM_INCLUDE_DIR/dataman/database/AMDbObject.cpp \
		$$AM_INCLUDE_DIR/dataman/database/AMDatabase.cpp \
		$$AM_INCLUDE_DIR/dataman/AMnDIndex.cpp \
		$$AM_INCLUDE_DIR/dataman/database/AMDbObjectSupport.cpp \
		$$AM_INCLUDE_DIR/dataman/info/AMControlInfo.cpp \
		$$AM_INCLUDE_DIR/dataman/info/AMControlInfoList.cpp \
		$$AM_INCLUDE_DIR/dataman/datasource/AMDataSource.cpp \
		$$AM_INCLUDE_DIR/dataman/datasource/AMRawDataSource.cpp \
		$$AM_INCLUDE_DIR/dataman/datastore/AMDataStore.cpp \
		$$AM_INCLUDE_DIR/dataman/datastore/AMInMemoryDataStore.cpp \
		$$AM_INCLUDE_DIR/dataman/datastore/AMCDFDataStore.cpp \
		$$AM_INCLUDE_DIR/dataman/datasource/AMDataSourceImageData.cpp \
		$$AM_INCLUDE_DIR/dataman/datasource/AMDataSourceSeriesData.cpp \
		$$AM_INCLUDE_DIR/dataman/AMAnalysisBlock.cpp \
		$$AM_INCLUDE_DIR/dataman/AMAxisInfo.cpp \
		$$AM_INCLUDE_DIR/dataman/AMNumber.cpp \
		$$AM_INCLUDE_DIR/dataman/AMSamplePre2013.cpp \
		$$AM_INCLUDE_DIR/acquaman/AMScanConfiguration.cpp \
		$$AM_INCLUDE_DIR/util/AMSettings.cpp \
		$$AM_INCLUDE_DIR/util/AMOrderedSetSignalSource.cpp \
		$$AM_INCLUDE_DIR/util/AMErrorMonitor.cpp \
		$$AM_INCLUDE_DIR/util/AMPeriodicTable.cpp \
		$$AM_INCLUDE_DIR/util/AMElement.cpp \
		$$AM_INCLUDE_DIR/application/AMPluginsManager.cpp \
		$$AM_INCLUDE_DIR/dataman/datasource/AMDataSourceImageDatawDefault.cpp \
		$$AM_INCLUDE_DIR/ui/dataman/AMSimpleDataSourceEditor.cpp \
		$$AM_INCLUDE_DIR/dataman/info/AMDetectorInfo.cpp \
		$$AM_INCLUDE_DIR/dataman/info/AMDetectorInfoSet.cpp \
		$$AM_INCLUDE_DIR/dataman/AMTextStream.cpp \
		$$AM_INCLUDE_DIR/dataman/database/AMConstDbObject.cpp
