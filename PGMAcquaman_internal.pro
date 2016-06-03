include ( clsCommon.pri )

TARGET = PGMAcquaman

FORMS +=

HEADERS +=	\
	source/application/PGM/PGM.h \
	source/application/PGM/PGMAppController.h \
	source/acquaman/PGM/PGMXASScanConfiguration.h \
	source/acquaman/PGM/PGMXASScanController.h \
	source/beamline/PGM/PGMBeamline.h \
	source/beamline/PGM/PGMBPMControl.h \
	source/beamline/PGM/PGMOceanOpticsXRFDetector.h \
	source/beamline/PGM/PGMPicoAmmeter.h \
	source/dataman/PGM/PGMUserConfiguration.h \
	source/ui/PGM/PGMPersistentView.h \
	source/ui/PGM/PGMSlitControlView.h \
	source/ui/PGM/PGMBeamStatusView.h \
	source/ui/PGM/PGMBPMStatusView.h \
	source/ui/PGM/PGMBladeCurrentView.h \
    source/ui/PGM/PGMPicoAmmeterView.h \
    source/ui/PGM/PGMXASScanConfigurationView.h \
    source/ui/PGM/PGMGratingView.h \
	source/ui/PGM/PGMUndulatorView.h

SOURCES +=	\
	source/application/PGM/PGMMain.cpp \
	source/application/PGM/PGMAppController.cpp \
	source/acquaman/PGM/PGMXASScanConfiguration.cpp \
	source/acquaman/PGM/PGMXASScanController.cpp \
	source/beamline/PGM/PGMBeamline.cpp \
	source/beamline/PGM/PGMBPMControl.cpp \
	source/beamline/PGM/PGMOceanOpticsXRFDetector.cpp \
	source/beamline/PGM/PGMPicoAmmeter.cpp \
	source/dataman/PGM/PGMUserConfiguration.cpp \
	source/ui/PGM/PGMPersistentView.cpp \
	source/ui/PGM/PGMSlitControlView.cpp \
	source/ui/PGM/PGMBeamStatusView.cpp \
	source/ui/PGM/PGMBPMStatusView.cpp \
	source/ui/PGM/PGMBladeCurrentView.cpp \
    source/ui/PGM/PGMPicoAmmeterView.cpp \
    source/ui/PGM/PGMXASScanConfigurationView.cpp \
    source/ui/PGM/PGMGratingView.cpp \
	source/ui/PGM/PGMUndulatorView.cpp


RESOURCES += \
	source/icons
