include ( clsCommon.pri )
include ( sgmCommon.pri )

TARGET = SGMAddOnsCoordinator

RESOURCES += \
    db/BL1611-ID-1-EnergyAddon.db

HEADERS += \
    source/beamline/SGM/SGMEnergyCoordinator.h \
    source/beamline/SGM/SGMNewEnergyPVSet.h \
    source/beamline/SGM/SGMOldEnergyPVSet.h

SOURCES += \
	source/application/SGMEnergyCoordinator/SGMEnergyCoordinatorMain.cpp \
    source/beamline/SGM/SGMEnergyCoordinator.cpp \
    source/beamline/SGM/SGMNewEnergyPVSet.cpp \
    source/beamline/SGM/SGMOldEnergyPVSet.cpp














