include ( acquamanCommon.pri )

TARGET = MidIRBPM

FORMS +=

HEADERS += \
	source/ui/MID-IR/BeamPositionMonitor.h

SOURCES += \
	source/ui/MID-IR/BeamPositionMonitor.cpp \
	source/application/MID-IR/BPMMain.cpp

RESOURCES += \
	source/ui/VESPERS/EndstationPictures.qrc \
	source/ui/VESPERS/vespersIcons.qrc \
	source/ui/StopButton.qrc
