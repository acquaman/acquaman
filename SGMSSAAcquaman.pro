include ( acquamanCommon.pri )

TARGET = SGMSSAAcquaman

FORMS += 	source/ui/SGMXASScanConfigurationViewer.ui

HEADERS += 	source/application/SGM/SGMSSAAppController.h \
	source/acquaman/SGM/SGMScanConfiguration.h \
	source/acquaman/SGM/SGMXASDacqScanController.h \
	source/acquaman/SGM/SGMXASScanConfiguration.h \
	source/acquaman/SGM/SGMXASScanController.h \
	source/acquaman/SGM/SGMFastScanConfiguration.h \
	source/acquaman/SGM/SGMFastDacqScanController.h \
	source/acquaman/SGM/SGMFastScanController.h \
	source/beamline/SGM/SGMBeamline.h \
	source/ui/SGM/SGMSampleTransferView.h \
	source/ui/SGM/SGMXASScanConfigurationView.h \
	source/ui/SGM/SGMXASScanConfigurationWizard.h \
	source/ui/SGM/SGMSidebar.h \
	source/ui/SGM/SGMFastScanConfigurationView.h \
	source/ui/SGM/SGMSampleManipulatorView.h \
	source/beamline/SGM/SGMBeamlineInfo.h \
	source/util/SGM/SGMElementInfo.h \
	source/util/SGM/SGMPeriodicTable.h

SOURCES +=	source/application/SGM/SGMSSAMain.cpp \
	source/application/SGM/SGMSSAAppController.cpp \
	source/acquaman/SGM/SGMScanConfiguration.cpp \
	source/acquaman/SGM/SGMXASDacqScanController.cpp \
	source/acquaman/SGM/SGMXASScanConfiguration.cpp \
	source/acquaman/SGM/SGMXASScanController.cpp \
	source/acquaman/SGM/SGMFastScanConfiguration.cpp \
	source/acquaman/SGM/SGMFastScanController.cpp \
	source/acquaman/SGM/SGMFastDacqScanController.cpp \
	source/beamline/SGM/SGMBeamline.cpp \
	source/ui/SGM/SGMSampleTransferView.cpp \
	source/ui/SGM/SGMXASScanConfigurationView.cpp \
	source/ui/SGM/SGMXASScanConfigurationWizard.cpp \
	source/ui/SGM/SGMSidebar.cpp \
	source/ui/SGM/SGMFastScanConfigurationView.cpp \
	source/ui/SGM/SGMSampleManipulatorView.cpp \
	source/beamline/SGM/SGMBeamlineInfo.cpp \
	source/util/SGM/SGMElementInfo.cpp \
	source/util/SGM/SGMPeriodicTable.cpp

RESOURCES += \
    source/beamline/SGM/TransferHelpImages/LoadLockOutImages.qrc \
    source/beamline/SGM/TransferHelpImages/LoadLockInImages.qrc \
    source/beamline/SGM/TransferHelpImages/ChamberOutImages.qrc \
    source/beamline/SGM/TransferHelpImages/ChamberInImages.qrc
