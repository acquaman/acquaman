include ( acquamanCommon.pri )

TARGET = VESPERSAcquaman

FORMS +=

HEADERS += \
    source/ui/VESPERS/XRFViewer.h

SOURCES += \
    source/application/VESPERS/XRFMain.cpp \
    source/ui/VESPERS/XRFViewer.cpp

RESOURCES += \
	source/ui/VESPERS/EndstationPictures.qrc \
	source/ui/VESPERS/vespersIcons.qrc \
	source/ui/StopButton.qrc
