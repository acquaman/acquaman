include ( clsCommon.pri )

TARGET = PGMAcquaman

FORMS +=

HEADERS +=	source/application/PGM/PGMAppController.h \
	source/beamline/PGM/PGMBeamline.h \
    source/beamline/PGM/PGMMonochromatorControl.h \
    source/ui/PGM/PGMPersistentView.h

SOURCES +=	source/application/PGM/PGMAppController.cpp \
	source/application/PGM/PGMMain.cpp \
	source/beamline/PGM/PGMBeamline.cpp \
    source/beamline/PGM/PGMMonochromatorControl.cpp \
    source/ui/PGM/PGMPersistentView.cpp
