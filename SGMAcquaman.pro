include ( acquamanCommon.pri )

TARGET = SGMAcquaman

FORMS += 	source/ui/SGMXASScanConfigurationViewer.ui

HEADERS += 	source/acquaman/SGM/SGMScanConfiguration.h \
	source/acquaman/SGM/SGMXASDacqScanController.h \
	source/acquaman/SGM/SGMXASScanConfiguration.h \
	source/acquaman/SGM/SGMXASScanController.h \
	source/acquaman/SGM/SGMFastScanConfiguration.h \
	source/acquaman/SGM/SGMFastDacqScanController.h \
	source/acquaman/SGM/SGMFastScanController.h \
	source/beamline/SGMBeamline.h \
	source/ui/SGMSampleTransferView.h \
	source/ui/SGMXASScanConfigurationViewer.h \
	source/ui/SGMXASScanConfigurationWizard.h \
	source/ui/SGMSidebar.h \
	source/ui/SGMBeamlineAtAGlanceBar.h \
	source/ui/SGMFastScanConfigurationViewer.h

SOURCES +=	source/main.cpp \
	source/acquaman/SGM/SGMScanConfiguration.cpp \
	source/acquaman/SGM/SGMXASDacqScanController.cpp \
	source/acquaman/SGM/SGMXASScanConfiguration.cpp \
	source/acquaman/SGM/SGMXASScanController.cpp \
	source/acquaman/SGM/SGMFastScanConfiguration.cpp \
	source/acquaman/SGM/SGMFastScanController.cpp \
	source/acquaman/SGM/SGMFastDacqScanController.cpp \
	source/beamline/SGMBeamline.cpp \
	source/ui/SGMSampleTransferView.cpp \
	source/ui/SGMXASScanConfigurationViewer.cpp \
	source/ui/SGMXASScanConfigurationWizard.cpp \
	source/ui/SGMSidebar.cpp \
	source/ui/SGMBeamlineAtAGlanceBar.cpp \
	source/ui/SGMFastScanConfigurationViewer.cpp
