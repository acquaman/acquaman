include ( clsCommon.pri )
include ( $$PATH_TO_AM/compositeCommon/AMGSL.pri )

TARGET = REIXSAcquaman

HEADERS +=	\
	source/application/REIXS/REIXS.h \
	source/application/REIXS/REIXSAppController.h \
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
	source/analysis/REIXS/REIXSXESImageInterpolationAB.h \
	source/analysis/REIXS/REIXSXESImageInterpolationABEditor.h \
	source/acquaman/REIXS/REIXSXASScanConfiguration.h \
	source/ui/REIXS/REIXSXASScanConfigurationView.h \
	source/acquaman/REIXS/REIXSXASScanActionController.h \
	source/beamline/REIXS/REIXSXESMCPDetector.h \
	source/acquaman/REIXS/REIXSXESMCPDetectorPre2013.h \
	source/dataman/datasource/REIXS/REIXSXESMCPDataSource.h \
	source/acquaman/REIXS/REIXSXESScanActionController.h \
	source/ui/REIXS/REIXSRIXSScanConfigurationView.h \
	source/actions3/REIXS/REIXSBeamOnOffAction.h \
	source/ui/REIXS/REIXSScalerView.h \
	source/ui/REIXS/REIXSXESSpectrometerControlPanel.h \
	source/acquaman/REIXS/REIXSScanActionControllerMCPFileWriter.h \
    source/ui/REIXS/REIXSSampleManagementPre2013Widget.h \
    source/ui/REIXS/REIXSAppBottomPanel.h \
    source/ui/REIXS/REIXSActionRunnerBottomBarCurrentView3.h \
    source/beamline/REIXS/REIXSSampleMotor.h

SOURCES += \
	source/application/REIXS/REIXSMain.cpp \
	source/application/REIXS/REIXSAppController.cpp \
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
	source/analysis/REIXS/REIXSXESImageInterpolationAB.cpp \
	source/analysis/REIXS/REIXSXESImageInterpolationABEditor.cpp \
	source/analysis/REIXS/REIXSXESImageAB.cpp \
	source/analysis/REIXS/REIXSXESImageABEditor.cpp \
	source/acquaman/REIXS/REIXSXASScanConfiguration.cpp \
	source/ui/REIXS/REIXSXASScanConfigurationView.cpp \
    source/acquaman/REIXS/REIXSXASScanActionController.cpp \
    source/beamline/REIXS/REIXSXESMCPDetector.cpp \
    source/acquaman/REIXS/REIXSXESMCPDetectorPre2013.cpp \
    source/dataman/datasource/REIXS/REIXSXESMCPDataSource.cpp \
    source/acquaman/REIXS/REIXSXESScanActionController.cpp \
	source/ui/REIXS/REIXSRIXSScanConfigurationView.cpp \
    source/actions3/REIXS/REIXSBeamOnOffAction.cpp \
    source/ui/REIXS/REIXSScalerView.cpp \
    source/ui/REIXS/REIXSXESSpectrometerControlPanel.cpp \
    source/acquaman/REIXS/REIXSScanActionControllerMCPFileWriter.cpp \
    source/ui/REIXS/REIXSSampleManagementPre2013Widget.cpp \
    source/ui/REIXS/REIXSAppBottomPanel.cpp \
    source/ui/REIXS/REIXSActionRunnerBottomBarCurrentView3.cpp \
    source/beamline/REIXS/REIXSSampleMotor.cpp























