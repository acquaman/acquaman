include ( acquamanCommon.pri )

TARGET = IDEASSAcquaman

FORMS +=

HEADERS +=	source/application/IDEAS/IDEASAppController.h \
	source/beamline/IDEAS/IDEASBeamline.h \
    source/ui/IDEAS/IDEASPersistentView.h \
    source/beamline/IDEAS/IDEASMonochromatorControl.h

SOURCES +=	source/application/IDEAS/IDEASAppController.cpp \
	source/application/IDEAS/IDEASMain.cpp \
	source/beamline/IDEAS/IDEASBeamline.cpp \
    source/ui/IDEAS/IDEASPersistentView.cpp \
    source/beamline/IDEAS/IDEASMonochromatorControl.cpp








