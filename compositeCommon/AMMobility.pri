macx {
	MOBILITY_QT_LIB_DIR = $$PATH_TO_AM/../qt-mobility/qt-mobility-opensource-src-1.1.3/install/Library/Frameworks/QtMultimediaKit.framework/Versions/Current
	MOBILITY_QT_LIB = -L$$MOBILITY_QT_LIB_DIR -lQtMultimediaKit
	MOBILITY_QT_INCLUDE_DIR = $$PATH_TO_AM/../qt-mobility/qt-mobility-opensource-src-1.1.3/install/include/QtMultimediaKit \
					$$PATH_TO_AM/../qt-mobility/qt-mobility-opensource-src-1.1.3/install/include/QtMobility
}
linux-g++ {
	#Qt Mobility Dependencies
	MOBILITY_QT_LIB_DIR = $$PATH_TO_AM/../qt-mobility-1.1.3-Ubuntu12.04/lib
	MOBILITY_QT_LIB = -L$$MOBILITY_QT_LIB_DIR -lQtMultimediaKit
	MOBILITY_QT_INCLUDE_DIR = $$PATH_TO_AM/../qt-mobility-1.1.3-Ubuntu12.04/include
}
linux-g++-32 {

}
linux-g++-64 {
	contains(USERNAME, david){
		CONFIG -= mobility
	}
}

# Special build paths and options for running on the Jenkins auto-build server (currently at http://beamteam.usask.ca:8080)
CONFIG(jenkins_build) {
		# Disable Qt Mobility Video until the Jenkins-machine supports that
		CONFIG -= mobility
}

QT *= opengl

# add video using Multimedia module from QtMobility, if we have it
CONFIG(mobility) {
	MOBILITY *= multimedia

	INCLUDEPATH *= $$MOBILITY_QT_INCLUDE_DIR
	LIBS *= $$MOBILITY_QT_LIB

	DEFINES *= AM_MOBILITY_VIDEO_ENABLED

	HEADERS *= source/ui/AMCrosshairOverlayVideoWidget.h \
		source/ui/AMOverlayVideoWidget.h \
		source/ui/AMBeamlineCameraWidget.h \
		source/ui/AMBeamlineCameraWidgetWithSourceTabs.h \
		source/ui/AMBeamlineCameraBrowser.h

	SOURCES *= source/ui/AMOverlayVideoWidget.cpp \
		source/ui/AMCrosshairOverlayVideoWidget.cpp \
		source/ui/AMBeamlineCameraWidget.cpp \
		source/ui/AMBeamlineCameraWidgetWithSourceTabs.cpp \
		source/ui/AMBeamlineCameraBrowser.cpp
}