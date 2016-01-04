#################################################
# Acquaman Build Settings
#
# Defines the required include & lib paths for building Acquaman projects.
#
# NOTE: Do not change this file to alter settings. Instead create a new file in
# the root acquaman directory named AMBuildConfig.pri, copy the default values below
# into it, and then alter them in there.
#################################################

exists($$PATH_TO_AM/AMBuildConfig.pri) {

	# If a config file exists, use that
	include(AMBuildConfig.pri)
} else {

	#Load Defaults
	AM_ENABLE_BOUNDS_CHECKING = 1

	CDF_INCLUDE_DIR = /home/beamline/tools/cdf/include
	CDF_LIB_DIR = /home/beamline/tools/cdf/lib

	EPICS_INCLUDE_DIR = /home/epics/src/R3.14.12/base/include \
				/home/epics/src/R3.14.12/base/include/os/Linux
	EPICS_LIB_DIR = /home/epics/src/R3.14.12/base/lib/linux-x86_64

	GSL_INCLUDE_DIR = /home/beamline/tools/gsl/gsl-1.14-install/include
	GSL_LIB_DIR = /home/beamline/tools/gsl/gsl-1.14-install/lib

	USE_MOBILITY_QT = 1
	MOBILITY_QT_INCLUDE_DIR = $$PATH_TO_AM/../qt-mobility-1.1.3-Ubuntu12.04/include
	MOBILITY_QT_LIB_DIR = $$PATH_TO_AM/../qt-mobility-1.1.4-Ubuntu12.04/lib

	MPLOT_INCLUDE_DIR = $$PATH_TO_AM/../MPlot/include
	MPLOT_LIB_DIR = $$PATH_TO_AM/../MPlot/lib

	EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen
}


