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
    source/ui/SGM/SGMXASScanConfigurationView.h \
    source/acquaman/SGM/SGMLineScanConfiguration.h \
    source/acquaman/SGM/SGMLineScanController.h \
    source/ui/SGM/SGMLineScanConfigurationView.h \
    source/acquaman/SGM/SGMMapScanConfiguration.h \
    source/acquaman/SGM/SGMMapScanController.h \
    source/ui/SGM/SGMMapScanConfigurationView.h \
    source/dataman/SGM/SGMUserConfiguration.h \
	source/application/SGM/SGM.h \
    source/ui/SGM/SGMBeamOnControlWidget.h

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
    source/ui/SGM/SGMXASScanConfigurationView.cpp \
    source/acquaman/SGM/SGMLineScanConfiguration.cpp \
    source/acquaman/SGM/SGMLineScanController.cpp \
    source/ui/SGM/SGMLineScanConfigurationView.cpp \
    source/acquaman/SGM/SGMMapScanConfiguration.cpp \
    source/acquaman/SGM/SGMMapScanController.cpp \
    source/ui/SGM/SGMMapScanConfigurationView.cpp \
	source/dataman/SGM/SGMUserConfiguration.cpp \
    source/ui/SGM/SGMBeamOnControlWidget.cpp







