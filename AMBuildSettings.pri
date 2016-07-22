#################################################
# Acquaman Build Settings
#
# Defines the required include & lib paths for building Acquaman projects.
#
# NOTE: Do not change this file to alter settings. Instead create a new file in
# the root acquaman directory named AMBuildConfig.pri and set the values you wish
# to alter in there.
#################################################

# Location of the CDF header and library files
CDF_INCLUDE_DIR = /home/beamline/tools/cdf/include
CDF_LIB_DIR = /home/beamline/tools/cdf/lib

# Location of the CDF header and library files
HDF5_INCLUDE_DIR = /home/beamline/tools/hdf5/include
HDF5_LIB_DIR = /home/beamline/tools/hdf5/lib
	
# Location of the EPICS base header and library files
EPICS_INCLUDE_DIR = /home/epics/src/R3.14.12/base/include \
			/home/epics/src/R3.14.12/base/include/os/Linux
EPICS_LIB_DIR = /home/epics/src/R3.14.12/base/lib/linux-x86_64

# Location of the GSL header and library files
GSL_INCLUDE_DIR = /home/beamline/tools/gsl/gsl-1.14-install/include
GSL_LIB_DIR = /home/beamline/tools/gsl/gsl-1.14-install/lib

# Whether to attempt to use Qt mobility, and the location of its header and library files
USE_MOBILITY_QT = 0
MOBILITY_QT_INCLUDE_DIR = 
MOBILITY_QT_LIB_DIR = 

# Location of the MPlot header and library files
MPLOT_INCLUDE_DIR = $$PATH_TO_AM/../MPlot/include
MPLOT_LIB_DIR = $$PATH_TO_AM/../MPlot/lib

# location of the Eigen Vector header and library files
EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen

# location of the AcquamanDataServer header and source files
PATH_TO_AMDS = $$PATH_TO_AM/../AcquamanDataServer

exists($$PATH_TO_AM/AMBuildConfig.pri) {

	# If a config file exists, use that
	include($$PATH_TO_AM/AMBuildConfig.pri)
}


