# Automatically determines a user's home folder
HOME_FOLDER = $$system(echo $HOME)

USERNAME = $$system(whoami)

equals(QMAKE_CXX, "clang++"){
	DEFINES *= AM_PRAGMA_WARNING_CONTROLS
}

macx {

	contains(USERNAME, helfrij){
		CONFIG -= mobility
	}

	QMAKE_CXXFLAGS_X86_64 *= "-mmacosx-version-min=10.7"

	QMAKE_LFLAGS_DEBUG *= "-mmacosx-version-min=10.7"
	QMAKE_LFLAGS_RELEASE *= "-mmacosx-version-min=10.7"
}
linux-g++ {

}
linux-g++-32 {
	# Disable Qt Mobility Video until Darren's laptop is ready for that.
	CONFIG -= mobility
}
linux-g++-64 {

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

QT *= core gui

HEADERS *= \
	source/AMQEvents.h \
	source/AMQGraphics.h \
	source/AMQPlainTextEdit.h \
	source/AMQtGui.h \
	source/AMQtTest.h
