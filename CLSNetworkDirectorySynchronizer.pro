# Video Support: Remove this line if you do not have the multimedia module from QtMobility
CONFIG += mobility

# Debug: Uncomment this to build the program in debug mode (no optimizations; include debugging symbols.)
# Note that as of November 18, 2011, building in debug mode triggers a failure in the dacq library: the main (eV) PV ends up disabled in the dacq scan config.  This is likely a serious memory error.
# CONFIG += debug

USERNAME = $$system(whoami)

# Automatically determines a user's home folder
HOME_FOLDER = $$system(echo $HOME)

USERNAME = $$system(whoami)

macx {

contains(USERNAME, helfrij){
	CONFIG -= mobility
}

		# Disable Qt Mobility Video until everyone's Mac laptops support that
		#CONFIG -= mobility

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman_4_8

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/include \
								$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/include/os/Darwin
		EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/lib/darwin-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot_4_8/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot_4_8/lib

		GSL_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/include

		# GSL Dependencies
		GSL_LIB = -L$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/lib -lgsl
		GSL_CBLAS_LIB = -L$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/lib -lgslcblas

		# QwtPlot3d dependencies (Disabled for now...)
		 #QWTPLOT3D_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman_4_8/contrib/qwtplot3d/lib
		 #QWTPLOT3D_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman_4_8/contrib/qwtplot3d/include

		# CDFlib dependencies
		CDF_LIB = /Applications/cdf34_0-dist/lib/libcdf.a
		CDF_INCLUDE_DIR = /Applications/cdf34_0-dist/include

				#Eigen
		EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen

		MOBILITY_QT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility/qt-mobility-opensource-src-1.1.3/install/Library/Frameworks/QtMultimediaKit.framework/Versions/Current
		MOBILITY_QT_LIB = -L$$MOBILITY_QT_LIB_DIR -lQtMultimediaKit
		MOBILITY_QT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility/qt-mobility-opensource-src-1.1.3/install/include/QtMultimediaKit \
						$$HOME_FOLDER/$$DEV_PATH/qt-mobility/qt-mobility-opensource-src-1.1.3/install/include/QtMobility

		QMAKE_CXXFLAGS_X86_64 += "-mmacosx-version-min=10.7"

		QMAKE_LFLAGS_DEBUG += "-mmacosx-version-min=10.7"
		QMAKE_LFLAGS_RELEASE += "-mmacosx-version-min=10.7"
}
linux-g++ {

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/epics/base/include \
				$$HOME_FOLDER/$$DEV_PATH/epics/base/include/os/Linux
		EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/epics/base/lib/linux-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib


#                GSL_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/include
#                GSL_LIB = -L$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/lib -lgsl
#                GSL_CBLAS_LIB = -lgslcblas

		# GSL Dependencies
		GSL_INCLUDE_DIR = $$PATH_TO_AM/contrib/gsl-install/include
		GSL_LIB = -L$$PATH_TO_AM/contrib/gsl-install/lib -lgsl
		GSL_CBLAS_LIB = -L$$PATH_TO_AM/contrib/gsl-install/lib -lgslcblas


				#Qt Mobility Dependencies
				MOBILITY_QT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-1.1.3-Ubuntu12.04/lib
				MOBILITY_QT_LIB = -L$$MOBILITY_QT_LIB_DIR -lQtMultimediaKit
				MOBILITY_QT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-1.1.3-Ubuntu12.04/include

				#Eigen
		EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen

		# CDFlib dependencies
#		CDF_LIB = -lcdf
#		CDF_INCLUDE_DIR = /usr/local/include
		CDF_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/cdf34_1-dist/lib
		CDF_LIB = -L$$CDF_LIB_DIR -lcdf
		CDF_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/cdf34_1-dist/include


		QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$CDF_LIB_DIR"
}
linux-g++-32 {

		# Disable Qt Mobility Video until Darren's laptop is ready for that.
		CONFIG -= mobility

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/epics/base/include \
				$$HOME_FOLDER/$$DEV_PATH/epics/base/include/os/Linux
		EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/epics/base/lib/linux-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib

		# GSL Dependencies
		GSL_LIB = -lgsl
		GSL_CBLAS_LIB = -lgslcblas

		# QwtPlot3d dependencies (Disabled for now...)
#		QWTPLOT3D_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/qwtplot3d/lib
#		QWTPLOT3D_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/qwtplot3d/include

		# CDFlib dependencies
		CDF_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/cdf34_1-dist/lib
		CDF_LIB = -L$$CDF_LIB_DIR -lcdf
		CDF_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/cdf34_1-dist/include

		QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$CDF_LIB_DIR"

				#Eigen
		EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen
}
# The following works well for CLS beamline OPI machines, built using VMSL54.cs.clsi.ca

linux-g++-64 {

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = /home/epics/src/R3.14.12/base/include \
				/home/epics/src/R3.14.12/base/include/os/Linux
		EPICS_LIB_DIR = /home/epics/src/R3.14.12/base/lib/linux-x86_64

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib

		# GSL Dependencies
		GSL_INCLUDE_DIR = /home/beamline/tools/gsl/gsl-1.14-install/include
		GSL_LIB = -L/home/beamline/tools/gsl/gsl-1.14-install/lib -lgsl
		GSL_CBLAS_LIB = -lgslcblas

		# QwtPlot3d dependencies (Disabled for now...)
		# QWTPLOT3D_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/qwtplot3d/lib
		# QWTPLOT3D_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/qwtplot3d/include

		# CDFlib dependencies
		CDF_LIB_DIR = /home/beamline/tools/cdf/lib
		CDF_LIB = -L$$CDF_LIB_DIR -lcdf
		CDF_INCLUDE_DIR = /home/beamline/tools/cdf/include

		QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$CDF_LIB_DIR"

				#Eigen
		EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen

}

# Special build paths and options for running on the Jenkins auto-build server (currently at http://beamteam.usask.ca:8080)
CONFIG(jenkins_build) {

		message("Detected Jenkins auto-build... Specifying dependency paths for the build server.")

		# Disable Qt Mobility Video until the Jenkins-machine supports that
		CONFIG -= mobility

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = /jobs/AcquamanOnLinux_MasterBranch/workspace

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = /home/mark/dev/epics/base/include \
				/home/mark/dev/epics/base/include/os/Linux
		EPICS_LIB_DIR = /home/mark/dev/epics/base/lib/linux-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = "/var/lib/jenkins/jobs/MPlotOnLinux_MasterBranch/workspace/include"
		MPLOT_LIB_DIR = "/var/lib/jenkins/jobs/MPlotOnLinux_MasterBranch/workspace/lib"

		#Eigen
		EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen
}

DEPENDPATH += $$PATH_TO_AM $$PATH_TO_AM/source
INCLUDEPATH += $$PATH_TO_AM $$PATH_TO_AM/source

TARGET = build/CLSNetworkDirectorySynchronizer

HEADERS +=\
	source/application/CLSNetworkDirectorySynchronizer/CLSNetworkDirectorySynchronizer.h \
	source/util/AMDirectorySynchronizer.h \
	source/util/AMRecursiveDirectoryCompare.h \
	source/ui/util/AMDirectorySynchronizerDialog.h \
	source/ui/AMVerticalStackWidget.h \
	source/ui/AMHeaderButton.h \
	source/ui/util/AMMessageBoxWTimeout.h \
	source/util/AMThread.h

SOURCES += \
	source/application/CLSNetworkDirectorySynchronizer/CLSNetworkDirectorySynchronizerMain.cpp \
	source/application/CLSNetworkDirectorySynchronizer/CLSNetworkDirectorySynchronizer.cpp \
	source/util/AMDirectorySynchronizer.cpp \
	source/util/AMRecursiveDirectoryCompare.cpp \
	source/ui/util/AMDirectorySynchronizerDialog.cpp \
	source/ui/AMVerticalStackWidget.cpp \
	source/ui/AMHeaderButton.cpp \
	source/ui/util/AMMessageBoxWTimeout.cpp \
	source/util/AMThread.cpp

RESOURCES = source/icons/icons.qrc
