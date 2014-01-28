include ( acquamanCommon.pri )

TARGET = REIXSAcquaman

FORMS += \
	source/ui/REIXS/REIXSXESSpectrometerControlEditor.ui \
	source/ui/REIXS/REIXSSampleChamberButtonPanel.ui \
	source/ui/REIXS/REIXSSidebar.ui \
	source/ui/REIXS/REIXSXASScanConfigurationView.ui

HEADERS +=	source/application/REIXS/REIXSAppController.h \
	source/acquaman/REIXS/REIXSXESScanConfiguration.h \
	source/acquaman/REIXS/REIXSXESScanController.h \
	source/beamline/REIXS/REIXSBeamline.h \
	source/ui/REIXS/REIXSXESScanConfigurationView.h \
	source/dataman/REIXS/REIXSXESMCPDetectorInfo.h \
	source/dataman/REIXS/REIXSXESCalibration.h \
	source/ui/REIXS/REIXSXESMCPDetectorView.h \
	source/ui/REIXS/REIXSXESScanConfigurationDetailedView.h \
	source/ui/REIXS/REIXSXESHexapodControlEditor.h \
	source/ui/REIXS/REIXSXESSpectrometerControlEditor.h \
	source/ui/REIXS/REIXSSampleChamberButtonPanel.h \
	source/beamline/REIXS/REIXSSampleManipulator.h \
	source/ui/REIXS/REIXSSidebar.h \
	source/ui/REIXS/REIXSActionBasedControlEditor.h \
	source/analysis/REIXS/REIXSXESImageAB.h \
	source/analysis/REIXS/REIXSXESImageABEditor.h \
	source/acquaman/REIXS/REIXSXASScanConfiguration.h \
	source/acquaman/REIXS/REIXSXASScanController.h \
	source/ui/REIXS/REIXSXASScanConfigurationView.h \
    source/acquaman/REIXS/REIXSXASScanActionController.h \
	source/acquaman/SGM/SGMXASScanActionControllerFileWriter.h \
    source/beamline/REIXS/REIXSXESMCPDetector.h \
    source/acquaman/REIXS/REIXSXESMCPDetectorPre2013.h \
    source/dataman/datasource/REIXS/REIXSXESMCPDataSource.h \
    source/acquaman/REIXS/REIXSXESScanActionController.h \
    source/ui/REIXS/REIXSRIXSScanConfigurationView.h


SOURCES +=	source/application/REIXS/REIXSAppController.cpp \
	source/application/REIXS/REIXSMain.cpp \
	source/acquaman/REIXS/REIXSXESScanConfiguration.cpp \
	source/acquaman/REIXS/REIXSXESScanController.cpp \
	source/beamline/REIXS/REIXSBeamline.cpp \
	source/ui/REIXS/REIXSXESScanConfigurationView.cpp \
	source/dataman/REIXS/REIXSXESMCPDetectorInfo.cpp \
	source/dataman/REIXS/REIXSXESCalibration.cpp \
	source/ui/REIXS/REIXSXESMCPDetectorView.cpp \
	source/ui/REIXS/REIXSXESScanConfigurationDetailedView.cpp \
	source/ui/REIXS/REIXSXESHexapodControlEditor.cpp \
	source/ui/REIXS/REIXSXESSpectrometerControlEditor.cpp \
	source/ui/REIXS/REIXSSampleChamberButtonPanel.cpp \
	source/beamline/REIXS/REIXSSampleManipulator.cpp \
	source/ui/REIXS/REIXSSidebar.cpp \
	source/ui/REIXS/REIXSActionBasedControlEditor.cpp \
	source/analysis/REIXS/REIXSXESImageAB.cpp \
	source/analysis/REIXS/REIXSXESImageABEditor.cpp \
	source/acquaman/REIXS/REIXSXASScanConfiguration.cpp \
	source/acquaman/REIXS/REIXSXASScanController.cpp \
	source/ui/REIXS/REIXSXASScanConfigurationView.cpp \
    source/acquaman/REIXS/REIXSXASScanActionController.cpp \
	source/acquaman/SGM/SGMXASScanActionControllerFileWriter.cpp \
    source/beamline/REIXS/REIXSXESMCPDetector.cpp \
    source/acquaman/REIXS/REIXSXESMCPDetectorPre2013.cpp \
    source/dataman/datasource/REIXS/REIXSXESMCPDataSource.cpp \
    source/acquaman/REIXS/REIXSXESScanActionController.cpp \
    source/ui/REIXS/REIXSRIXSScanConfigurationView.cpp


















