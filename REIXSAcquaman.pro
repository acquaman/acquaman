include ( acquamanCommon.pri )

TARGET = REIXSAcquaman

FORMS += \
    source/ui/REIXS/REIXSXESSpectrometerControlEditor.ui \
	source/ui/REIXS/REIXSSampleChamberButtonPanel.ui \
    source/ui/REIXS/REIXSSidebar.ui

HEADERS +=	source/application/REIXS/REIXSAppController.h \
	source/acquaman/REIXS/REIXSXESScanConfiguration.h \
	source/acquaman/REIXS/REIXSXESScanController.h \
	source/beamline/REIXS/REIXSBeamline.h \
	source/ui/REIXS/REIXSXESScanConfigurationView.h \
	source/dataman/REIXS/REIXSXESMCPDetectorInfo.h \
	source/acquaman/REIXS/REIXSXESMCPDetector.h \
	source/dataman/REIXS/REIXSXESCalibration.h \
	source/ui/REIXS/REIXSXESMCPDetectorView.h \
	source/ui/REIXS/REIXSXESScanConfigurationDetailedView.h \
	source/ui/REIXS/REIXSXESHexapodControlEditor.h \
    source/ui/REIXS/REIXSXESSpectrometerControlEditor.h \
	source/ui/REIXS/REIXSSampleChamberButtonPanel.h \
    source/actions2/actions/REIXS/REIXSControlMoveAction.h \
    source/actions2/actions/REIXS/REIXSControlMoveActionInfo.h \
    source/actions2/actions/REIXS/REIXSXESScanActionInfo.h \
    source/actions2/actions/REIXS/REIXSXESScanAction.h \
    source/ui/REIXS/REIXSScanConfigurationViewHolder.h \
    source/actions2/editors/REIXS/REIXSXESScanActionEditor.h \
    source/actions2/editors/REIXS/REIXSControlMoveActionEditor.h \
    source/actions2/actions/REIXS/REIXSSampleMoveAction.h \
    source/actions2/actions/REIXS/REIXSSampleMoveActionInfo.h \
    source/beamline/REIXS/REIXSSampleManipulator.h \
    source/ui/REIXS/REIXSSidebar.h \
    source/ui/REIXS/REIXSActionBasedControlEditor.h \
    source/ui/REIXS/REIXSActionBasedEnumControlEditor.h \
    source/actions2/actions/REIXS/REIXSMoveToSampleTransferPositionAction.h \
    source/actions2/actions/REIXS/REIXSMoveToSampleTransferPositionActionInfo.h \
    source/actions2/actions/REIXS/REIXSBeamOnOffAction.h \
    source/actions2/actions/REIXS/REIXSBeamOnOffActionInfo.h \
    source/actions2/editors/REIXS/REIXSBeamOnOffActionEditor.h \
    source/analysis/REIXS/REIXSXESImageAB.h \
    source/analysis/REIXS/REIXSXESImageABEditor.h


SOURCES +=	source/application/REIXS/REIXSAppController.cpp \
	source/application/REIXS/REIXSMain.cpp \
	source/acquaman/REIXS/REIXSXESScanConfiguration.cpp \
	source/acquaman/REIXS/REIXSXESScanController.cpp \
	source/beamline/REIXS/REIXSBeamline.cpp \
	source/ui/REIXS/REIXSXESScanConfigurationView.cpp \
	source/dataman/REIXS/REIXSXESMCPDetectorInfo.cpp \
	source/acquaman/REIXS/REIXSXESMCPDetector.cpp \
	source/dataman/REIXS/REIXSXESCalibration.cpp \
	source/ui/REIXS/REIXSXESMCPDetectorView.cpp \
	source/ui/REIXS/REIXSXESScanConfigurationDetailedView.cpp \
	source/ui/REIXS/REIXSXESHexapodControlEditor.cpp \
    source/ui/REIXS/REIXSXESSpectrometerControlEditor.cpp \
	source/ui/REIXS/REIXSSampleChamberButtonPanel.cpp \
    source/actions2/actions/REIXS/REIXSControlMoveAction.cpp \
    source/actions2/actions/REIXS/REIXSXESScanActionInfo.cpp \
    source/ui/REIXS/REIXSScanConfigurationViewHolder.cpp \
    source/actions2/editors/REIXS/REIXSXESScanActionEditor.cpp \
    source/actions2/editors/REIXS/REIXSControlMoveActionEditor.cpp \
    source/actions2/actions/REIXS/REIXSSampleMoveAction.cpp \
    source/actions2/actions/REIXS/REIXSSampleMoveActionInfo.cpp \
    source/beamline/REIXS/REIXSSampleManipulator.cpp \
    source/ui/REIXS/REIXSSidebar.cpp \
    source/ui/REIXS/REIXSActionBasedControlEditor.cpp \
    source/ui/REIXS/REIXSActionBasedEnumControlEditor.cpp \
    source/actions2/actions/REIXS/REIXSMoveToSampleTransferPositionAction.cpp \
    source/actions2/actions/REIXS/REIXSMoveToSampleTransferPositionActionInfo.cpp \
    source/actions2/actions/REIXS/REIXSBeamOnOffAction.cpp \
    source/actions2/actions/REIXS/REIXSBeamOnOffActionInfo.cpp \
    source/actions2/editors/REIXS/REIXSBeamOnOffActionEditor.cpp \
    source/analysis/REIXS/REIXSXESImageAB.cpp \
    source/analysis/REIXS/REIXSXESImageABEditor.cpp














































