include ( acquamanCommon.pri )

TARGET = XRFSpectraViewer

FORMS +=

HEADERS += \
	source/ui/VESPERS/XRFViewer.h \
	source/ui/VESPERS/VESPERSDeadTimeButton.h

SOURCES += \
	source/application/VESPERS/XRFMain.cpp \
	source/ui/VESPERS/XRFViewer.cpp \
	source/ui/VESPERS/VESPERSDeadTimeButton.cpp

