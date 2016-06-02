include ( clsCommon.pri )

TARGET = PGMAcquaman

FORMS +=

HEADERS +=	source/application/PGM/PGMAppController.h \
	source/beamline/PGM/PGMBeamline.h \
	source/ui/PGM/PGMPersistentView.h \
	source/ui/PGM/PGMBladeCurrentView.h \
    source/ui/PGM/PGMPicoAmmeterView.h \
    source/acquaman/PGM/PGMXASScanConfiguration.h \
    source/acquaman/PGM/PGMXASScanController.h \
    source/ui/PGM/PGMXASScanConfigurationView.h \
    source/beamline/PGM/PGMPicoAmmeter.h

SOURCES +=	source/application/PGM/PGMAppController.cpp \
	source/application/PGM/PGMMain.cpp \
	source/beamline/PGM/PGMBeamline.cpp \
	source/ui/PGM/PGMPersistentView.cpp \
	source/ui/PGM/PGMBladeCurrentView.cpp \
    source/ui/PGM/PGMPicoAmmeterView.cpp \
    source/acquaman/PGM/PGMXASScanConfiguration.cpp \
    source/acquaman/PGM/PGMXASScanController.cpp \
    source/ui/PGM/PGMXASScanConfigurationView.cpp \
    source/beamline/PGM/PGMPicoAmmeter.cpp
