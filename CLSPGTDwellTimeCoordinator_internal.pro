include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlSet.pri )

TARGET = CLSPGTDwellTimeCoordinator

FORMS +=

HEADERS += \ 
	source/beamline/CLS/CLSPGTDwellTimeCoordinator.h

SOURCES += \
	source/application/CLSPGTDwellTimeCoordinator/CLSPGTDwellTimeCoordinatorMain.cpp \
	source/beamline/CLS/CLSPGTDwellTimeCoordinator.cpp


