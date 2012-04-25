include ( acquamanCommon.pri )

TARGET = VESPERSXRFSpectraViewer

FORMS +=

HEADERS += \
	source/ui/VESPERS/VESPERSXRFViewer.h \
	source/ui/VESPERS/VESPERSDeadTimeButton.h

SOURCES += \
	source/application/VESPERS/VESPERSXRFSpectraViewerMain.cpp \
	source/ui/VESPERS/VESPERSXRFViewer.cpp \
	source/ui/VESPERS/VESPERSDeadTimeButton.cpp

