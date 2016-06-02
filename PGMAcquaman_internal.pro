include ( clsCommon.pri )

TARGET = PGMAcquaman

FORMS +=

HEADERS +=	\
	source/application/PGM/PGMAppController.h \
	source/beamline/PGM/PGMBeamline.h \
    source/ui/PGM/PGMSlitControlView.h \
    source/ui/PGM/PGMPersistentView.h \
	source/ui/PGM/PGMBladeCurrentView.h \
    source/ui/PGM/PGMPicoAmmeterView.h \
    source/dataman/PGM/PGMUserConfiguration.h

SOURCES +=	\
	source/application/PGM/PGMAppController.cpp \
	source/application/PGM/PGMMain.cpp \
	source/beamline/PGM/PGMBeamline.cpp \
    source/ui/PGM/PGMSlitControlView.cpp \
	source/ui/PGM/PGMPersistentView.cpp \
	source/ui/PGM/PGMBladeCurrentView.cpp \
    source/ui/PGM/PGMPicoAmmeterView.cpp \
    source/dataman/PGM/PGMUserConfiguration.cpp
