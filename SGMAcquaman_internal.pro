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
    source/ui/SGM/SGMEnergyTrajectoryView.h \
    source/acquaman/SGM/SGMXASScanConfiguration.h \
    source/acquaman/SGM/SGMXASScanController.h \
    source/ui/SGM/SGMXASScanConfigurationView.h

SOURCES += \
    source/application/SGM/SGMMain.cpp \
    source/application/SGM/SGMAppController.cpp \
	source/ui/SGM/SGMPersistentView.cpp \
    source/ui/SGM/SGMHexapodTrajectoryView.cpp \
    source/ui/SGM/SGMHexapodSystemRotationView.cpp \
    source/ui/SGM/SGMHexapodView.cpp \
    source/ui/SGM/SGMEnergyView.cpp \
    source/ui/SGM/SGMEnergyTrajectoryView.cpp \
    source/acquaman/SGM/SGMXASScanConfiguration.cpp \
    source/acquaman/SGM/SGMXASScanController.cpp \
    source/ui/SGM/SGMXASScanConfigurationView.cpp

include( $$PATH_TO_AM/compositeCommon/AMAcquamanDataServer.pri )
