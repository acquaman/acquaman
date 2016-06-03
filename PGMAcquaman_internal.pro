include ( clsCommon.pri )

TARGET = PGMAcquaman

FORMS +=

HEADERS +=	\
	source/application/PGM/PGM.h \
	source/application/PGM/PGMAppController.h \
	source/beamline/PGM/PGMBeamline.h \
	source/beamline/PGM/PGMBPMControl.h \
	source/beamline/PGM/PGMOceanOpticsXRFDetector.h \
	source/ui/PGM/PGMPersistentView.h \
	source/ui/PGM/PGMSlitControlView.h \
	source/ui/PGM/PGMBeamStatusView.h \
	source/ui/PGM/PGMBPMStatusView.h \
	source/ui/PGM/PGMBladeCurrentView.h \
    source/ui/PGM/PGMPicoAmmeterView.h

SOURCES +=	\
	source/application/PGM/PGMMain.cpp \
	source/application/PGM/PGMAppController.cpp \
	source/beamline/PGM/PGMBeamline.cpp \
	source/beamline/PGM/PGMBPMControl.cpp \
	source/beamline/PGM/PGMOceanOpticsXRFDetector.cpp \
	source/ui/PGM/PGMPersistentView.cpp \
	source/ui/PGM/PGMSlitControlView.cpp \
	source/ui/PGM/PGMBeamStatusView.cpp \
	source/ui/PGM/PGMBPMStatusView.cpp \
	source/ui/PGM/PGMBladeCurrentView.cpp \
    source/ui/PGM/PGMPicoAmmeterView.cpp
