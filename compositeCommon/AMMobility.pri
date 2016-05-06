QT *= opengl

equals(USE_MOBILITY_QT, 1) {

	CONFIG += mobility
} else {
	CONFIG -= mobility
}

# add video using Multimedia module from QtMobility, if we have it
CONFIG(mobility) {
	exists($$MOBILITY_QT_LIB_DIR){

		LIBS *= -L$$MOBILITY_QT_LIB_DIR -lQtMultimediaKit
	} else {

		error(Can not find library dependency: QtMobility at $$MOBILITY_QT_LIB_DIR)
	}

	for(CURRENT_DIR, MOBILITY_QT_INCLUDE_DIR) {
		exists($$CURRENT_DIR) {

			INCLUDEPATH *= $$CURRENT_DIR
		} else {

			error(Can not find include dependency: QtMobility at $$CURRENT_DIR)
		}
	}

	MOBILITY *= multimedia

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
