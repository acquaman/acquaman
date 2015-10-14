
include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlSet.pri )

TARGET = BioXASMainTableSoftIOCCoordinator

HEADERS += \  
	source/beamline/BioXAS/BioXASMainTableCoordinator.h

SOURCES += \
	source/application/BioXAS/BioXASMainTableCoordinatorMain.cpp \
	source/beamline/BioXAS/BioXASMainTableCoordinator.cpp
