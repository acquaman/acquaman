include ( $$PATH_TO_AM/compositeCommon/AMCurrentAmplifier.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlSet.pri )

TARGET = CLSSR570CoordinatorApp

HEADERS += \
	source/beamline/CLS/CLSSR570Coordinator.h

SOURCES += \
	source/application/CLSSR570Coordinator/CLSSR570CoordinatorMain.cpp \
	source/beamline/CLS/CLSSR570Coordinator.cpp
