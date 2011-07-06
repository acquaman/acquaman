include ( acquamanCommon.pri )

TARGET = SGMAcquaman

FORMS += 	source/ui/SGMXASScanConfigurationViewer.ui

HEADERS += 	source/application/SGM/SGMAppController.h \
	source/acquaman/SGM/SGMScanConfiguration.h \
	source/acquaman/SGM/SGMXASDacqScanController.h \
	source/acquaman/SGM/SGMXASScanConfiguration.h \
	source/acquaman/SGM/SGMXASScanController.h \
	source/acquaman/SGM/SGMFastScanConfiguration.h \
	source/acquaman/SGM/SGMFastDacqScanController.h \
	source/acquaman/SGM/SGMFastScanController.h \
	source/beamline/SGM/SGMBeamline.h \
	source/ui/SGMSampleTransferView.h \
	source/ui/SGMXASScanConfigurationView.h \
	source/ui/SGMXASScanConfigurationWizard.h \
	source/ui/SGMSidebar.h \
	source/ui/SGMFastScanConfigurationView.h \
	source/ui/SGM/SGMSampleManipulatorView.h \
    source/beamline/SGM/SGMBeamlineInfo.h \
    source/util/SGM/SGMElementInfo.h \
    source/util/SGM/SGMPeriodicTable.h #\
#    source/analysis/SGM/SGM1DFastScanFilterAB.h

SOURCES +=	source/application/SGM/SGMMain.cpp \
	source/application/SGM/SGMAppController.cpp \
	source/acquaman/SGM/SGMScanConfiguration.cpp \
	source/acquaman/SGM/SGMXASDacqScanController.cpp \
	source/acquaman/SGM/SGMXASScanConfiguration.cpp \
	source/acquaman/SGM/SGMXASScanController.cpp \
	source/acquaman/SGM/SGMFastScanConfiguration.cpp \
	source/acquaman/SGM/SGMFastScanController.cpp \
	source/acquaman/SGM/SGMFastDacqScanController.cpp \
	source/beamline/SGM/SGMBeamline.cpp \
	source/ui/SGMSampleTransferView.cpp \
	source/ui/SGMXASScanConfigurationView.cpp \
	source/ui/SGMXASScanConfigurationWizard.cpp \
	source/ui/SGMSidebar.cpp \
	source/ui/SGMFastScanConfigurationView.cpp \
	source/ui/SGM/SGMSampleManipulatorView.cpp \
    source/beamline/SGM/SGMBeamlineInfo.cpp \
    source/util/SGM/SGMElementInfo.cpp \
    source/util/SGM/SGMPeriodicTable.cpp #\
#    source/analysis/SGM/SGM1DFastScanFilterAB.cpp

RESOURCES += \
    source/beamline/SGM/TransferHelpImages/LoadLockOutImages.qrc \
    source/beamline/SGM/TransferHelpImages/LoadLockInImages.qrc \
    source/beamline/SGM/TransferHelpImages/ChamberOutImages.qrc \
    source/beamline/SGM/TransferHelpImages/ChamberInImages.qrc
