include ( acquamanCommon.pri )

TARGET = IDEASSAcquaman

FORMS +=

HEADERS +=	source/application/IDEAS/IDEASAppController.h \
	source/beamline/IDEAS/IDEASBeamline.h

SOURCES +=	source/application/IDEAS/IDEASAppController.cpp \
	source/application/IDEAS/IDEASMain.cpp \
	source/beamline/IDEAS/IDEASBeamline.cpp
