
include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )
include ( $$PATH_TO_AM/compositeCommon/AMControlSet.pri )

TARGET = BioXASSideSoftIOCCoordinator

HEADERS += \  
    source/beamline/BioXAS/BioXASSideTableCoordinator.h

SOURCES += \
    source/beamline/BioXAS/BioXASSideTableCoordinator.cpp \
    source/application/BioXAS/BioXASSideTableCoordinatorMain.cpp
