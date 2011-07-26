include ( acquamanCommon.pri )

TARGET = REIXSAcquaman

FORMS += \
    source/ui/REIXS/REIXSXESSpectrometerControlEditor.ui

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
    source/ui/REIXS/REIXSXESSpectrometerControlEditor.h


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
    source/ui/REIXS/REIXSXESSpectrometerControlEditor.cpp
