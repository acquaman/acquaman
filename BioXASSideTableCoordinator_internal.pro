
include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlSet.pri )

TARGET = BioXASSideTableSoftIOCCoordinator

HEADERS += \
	source/beamline/BioXAS/BioXASSideTableCoordinator.h

SOURCES += \
	source/application/BioXAS/BioXASSideTableCoordinatorMain.cpp \
	source/beamline/BioXAS/BioXASSideTableCoordinator.cpp


