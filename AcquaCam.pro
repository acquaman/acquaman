QT += core gui sql opengl

# video using Multimedia module from QtMobility
CONFIG += mobility
MOBILITY += multimedia

DESTDIR = build
DEPENDPATH += . source
INCLUDEPATH += . source

TARGET = AcquaCam

FORMS +=

HEADERS += source/ui/AMColorPickerButton.h \
	source/ui/AMCrosshairOverlayVideoWidget.h \
	source/ui/AMOverlayVideoWidget.h \
	source/ui/AMBeamlineCameraBrowser.h

SOURCES +=	source/application/AcquaCam/AcquaCamMain.cpp \
	source/ui/AMColorPickerButton.cpp \
	source/ui/AMCrosshairOverlayVideoWidget.cpp \
	source/ui/AMOverlayVideoWidget.cpp \
	source/ui/AMBeamlineCameraBrowser.cpp
