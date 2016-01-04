# Automatically determines a user's home folder
HOME_FOLDER = $$system(echo $HOME)

USERNAME = $$system(whoami)

equals(QMAKE_CXX, "clang++"){
	DEFINES *= AM_PRAGMA_WARNING_CONTROLS
}

macx {

	contains(USERNAME, helfrij){
		CONFIG -= mobility
		CONFIG -= app_bundle
	}

	contains(USERNAME, hunterd){
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

DEPENDPATH *= $$PATH_TO_AM $$PATH_TO_AM/source
INCLUDEPATH *= $$PATH_TO_AM $$PATH_TO_AM/source
VPATH *= $$PATH_TO_AM

# Set standard level of compiler warnings for everyone. (Otherwise the warnings shown will be system-dependent.)
QMAKE_CXXFLAGS *= -Wextra -g

equals($$AM_ENABLE_BOUNDS_CHECKING, 1) {
	DEFINES *= AM_ENABLE_BOUNDS_CHECKING
}

QT *= core gui

DESTDIR = build

OBJECTS_DIR=build_files
MOC_DIR=build_files
UI_DIR=build_files
RCC_DIR=build_files

HEADERS *= \
	source/AMQEvents.h \
	source/AMQGraphics.h \
	source/AMQPlainTextEdit.h \
	source/AMQtGui.h \
	source/AMQtTest.h
