include ( clsCommon.pri )

TARGET = PGMAcquaman

FORMS +=

HEADERS +=	source/application/PGM/PGMAppController.h \
	source/beamline/PGM/PGMBeamline.h \
        source/ui/PGM/PGMPersistentView.h \
    source/ui/PGM/PGMBeamStatusView.h \
    source/beamline/PGM/PGMBPMControl.h \
    source/ui/PGM/PGMBPMStatusView.h

SOURCES +=	source/application/PGM/PGMAppController.cpp \
	source/application/PGM/PGMMain.cpp \
	source/beamline/PGM/PGMBeamline.cpp \
    source/ui/PGM/PGMPersistentView.cpp \
    source/ui/PGM/PGMBeamStatusView.cpp \
    source/beamline/PGM/PGMBPMControl.cpp \
    source/ui/PGM/PGMBPMStatusView.cpp
