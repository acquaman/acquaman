include ( acquamanCommon.pri )

TARGET = REIXSAcquaman

FORMS += \
    source/ui/REIXS/REIXSXESSpectrometerControlEditor.ui \
	source/ui/REIXS/REIXSSampleChamberButtonPanel.ui

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
    source/actions2/editors/REIXS/REIXSControlMoveActionEditor.h


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
    source/actions2/editors/REIXS/REIXSControlMoveActionEditor.cpp



















