include ( acquamanCommon.pri )

TARGET = XRFSpectraViewer

FORMS +=

HEADERS += \
	source/ui/VESPERS/VESPERSXRFViewer.h \
	source/ui/VESPERS/VESPERSDeadTimeButton.h

SOURCES += \
	source/application/VESPERS/XRFMain.cpp \
	source/ui/VESPERS/VESPERSXRFViewer.cpp \
	source/ui/VESPERS/VESPERSDeadTimeButton.cpp

