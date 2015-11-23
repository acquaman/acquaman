include ( clsCommon.pri )
include ( sgmCommon.pri )

TARGET = SGMAcquaman	

RESOURCES += \

HEADERS += \
	source/application/SGM/SGMAppController.h \
	source/ui/SGM/SGMPersistentView.h \
    source/ui/SGM/SGMHexapodTrajectoryView.h \
    source/ui/SGM/SGMHexapodSystemRotationView.h \
    source/ui/SGM/SGMHexapodView.h \
    source/ui/SGM/SGMEnergyView.h \
    source/ui/SGM/SGMEnergyTrajectoryView.h

SOURCES += \
    source/application/SGM/SGMMain.cpp \
    source/application/SGM/SGMAppController.cpp \
	source/ui/SGM/SGMPersistentView.cpp \
    source/ui/SGM/SGMHexapodTrajectoryView.cpp \
    source/ui/SGM/SGMHexapodSystemRotationView.cpp \
    source/ui/SGM/SGMHexapodView.cpp \
    source/ui/SGM/SGMEnergyView.cpp \
    source/ui/SGM/SGMEnergyTrajectoryView.cpp

include( $$PATH_TO_AM/compositeCommon/AMAcquamanDataServer.pri )
