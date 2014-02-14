include ( acquamanCommon.pri )

TARGET = IDEASSAcquaman

FORMS +=

HEADERS +=	source/application/IDEAS/IDEASAppController.h \
	source/beamline/IDEAS/IDEASBeamline.h \
    source/ui/IDEAS/IDEASPersistentView.h \
    source/beamline/IDEAS/IDEASMonochromatorControl.h \
    source/acquaman/IDEAS/IDEASXASScanActionController.h \
    source/acquaman/IDEAS/IDEASXASScanConfiguration.h \
    source/ui/IDEAS/IDEASXASScanConfigurationView.h \
    source/beamline/IDEAS/IDEASKETEKDetector.h

SOURCES +=	source/application/IDEAS/IDEASAppController.cpp \
	source/application/IDEAS/IDEASMain.cpp \
	source/beamline/IDEAS/IDEASBeamline.cpp \
    source/ui/IDEAS/IDEASPersistentView.cpp \
    source/beamline/IDEAS/IDEASMonochromatorControl.cpp \
    source/acquaman/IDEAS/IDEASXASScanActionController.cpp \
    source/acquaman/IDEAS/IDEASXASScanConfiguration.cpp \
    source/ui/IDEAS/IDEASXASScanConfigurationView.cpp \
    source/beamline/IDEAS/IDEASKETEKDetector.cpp


