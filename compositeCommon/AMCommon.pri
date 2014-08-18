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
#		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman_4_8

		QMAKE_CXXFLAGS_X86_64 *= "-mmacosx-version-min=10.7"

		QMAKE_LFLAGS_DEBUG *= "-mmacosx-version-min=10.7"
		QMAKE_LFLAGS_RELEASE *= "-mmacosx-version-min=10.7"
}
linux-g++ {

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
#		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman

		QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
}
linux-g++-32 {

		# Disable Qt Mobility Video until Darren's laptop is ready for that.
		CONFIG -= mobility

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
#		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman

		QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
}
# The following works well for CLS beamline OPI machines, built using VMSL54.cs.clsi.ca

linux-g++-64 {

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
#		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman

		QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
}

# Special build paths and options for running on the Jenkins auto-build server (currently at http://beamteam.usask.ca:8080)
CONFIG(jenkins_build) {

		message("Detected Jenkins auto-build... Specifying dependency paths for the build server.")

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = /jobs/AcquamanOnLinux_MasterBranch/workspace

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH
}

DEPENDPATH *= $$PATH_TO_AM $$PATH_TO_AM/source
INCLUDEPATH *= $$PATH_TO_AM $$PATH_TO_AM/source
VPATH *= $$PATH_TO_AM

# Set standard level of compiler warnings for everyone. (Otherwise the warnings shown will be system-dependent.)
QMAKE_CXXFLAGS *= -Wextra -g

DEFINES *= AM_ENABLE_BOUNDS_CHECKING

QT *= core gui opengl network
