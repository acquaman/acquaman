QT += core gui sql opengl

# video using Multimedia module from QtMobility
CONFIG += mobility
MOBILITY += multimedia

DESTDIR = build
DEPENDPATH += . source
INCLUDEPATH += . source

TARGET = AcquaCam

FORMS +=

HEADERS += source/ui/dataman/AMColorPickerButton.h \
	source/ui/AMCrosshairOverlayVideoWidget.h \
	source/ui/AMOverlayVideoWidget.h \
	source/ui/AMBeamlineCameraWidget.h \
	source/ui/AMBeamlineCameraBrowser.h

SOURCES +=	source/application/AcquaCam/AcquaCamMain.cpp \
	source/ui/dataman/AMColorPickerButton.cpp \
	source/ui/AMCrosshairOverlayVideoWidget.cpp \
	source/ui/AMOverlayVideoWidget.cpp \
	source/ui/AMBeamlineCameraWidget.cpp \
	source/ui/AMBeamlineCameraBrowser.cpp
