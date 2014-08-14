include(AMDbObject.pri)

macx {
		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman_4_8

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/include \
								$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/include/os/Darwin
		EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/lib/darwin-x86

		QMAKE_CXXFLAGS_X86_64 += "-mmacosx-version-min=10.7"

		QMAKE_LFLAGS_DEBUG *= "-mmacosx-version-min=10.7"
		QMAKE_LFLAGS_RELEASE *= "-mmacosx-version-min=10.7"
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

		QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
}
linux-g++-32 {
		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/epics/base/include \
				$$HOME_FOLDER/$$DEV_PATH/epics/base/include/os/Linux
		EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/epics/base/lib/linux-x86

		QMAKE_LFLAGS_DEBUG *= "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE *= "-Wl,-rpath,$$CDF_LIB_DIR"
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

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = /home/mark/dev/epics/base/include \
				/home/mark/dev/epics/base/include/os/Linux
		EPICS_LIB_DIR = /home/mark/dev/epics/base/lib/linux-x86
}

QT *= core gui sql opengl network

# add video using Multimedia module from QtMobility, if we have it
CONFIG(mobility) {
	MOBILITY *= multimedia
}

DESTDIR = build

INCLUDEPATH *= $$EPICS_INCLUDE_DIRS

LIBS *= -L$$EPICS_LIB_DIR -lca -lCom

DEFINES *= AM_ENABLE_BOUNDS_CHECKING

contains(USERNAME, helfrij){
	QMAKE_CXXFLAGS_X86_64 *= "-mmacosx-version-min=10.7"

	QMAKE_LFLAGS_DEBUG *= "-mmacosx-version-min=10.7"
	QMAKE_LFLAGS_RELEASE *= "-mmacosx-version-min=10.7"
}

contains(USERNAME, chevrid){
	QMAKE_CXXFLAGS_X86_64 *= "-mmacosx-version-min=10.7"

	QMAKE_LFLAGS_DEBUG *= "-mmacosx-version-min=10.7"
	QMAKE_LFLAGS_RELEASE *= "-mmacosx-version-min=10.7"
}

DEPENDPATH *= $$PATH_TO_AM $$PATH_TO_AM/source
INCLUDEPATH *= $$PATH_TO_AM $$PATH_TO_AM/source

HEADERS *= \
	source/dataman/info/AMControlInfo.h \
	source/beamline/AMControl.h \
	source/beamline/AMPVControl.h \
	source/beamline/AMProcessVariable.h \
	source/beamline/AMProcessVariablePrivate.h \
	source/util/AMDeferredFunctionCall.h

SOURCES *= \
	source/dataman/info/AMControlInfo.cpp \
	source/beamline/AMControl.cpp \
	source/beamline/AMPVControl.cpp \
	source/beamline/AMProcessVariable.cpp \
	source/beamline/AMProcessVariablePrivate.cpp \
	source/util/AMDeferredFunctionCall.cpp
