include ( $$PATH_TO_AM/clsCompositeCommon/CLSAcquamanCommon.pri )

TARGET = MidIRBPM

FORMS +=

HEADERS += \
	source/ui/MID-IR/BeamPositionMonitor.h

SOURCES += \
	source/ui/MID-IR/BeamPositionMonitor.cpp \
	source/application/MID-IR/BPMMain.cpp
