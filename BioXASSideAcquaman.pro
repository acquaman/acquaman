include ( acquamanCommon.pri )

TARGET = BioXASSideAcquaman

FORMS +=

HEADERS += \
    source/application/BioXAS/BioXASSideAppController.h \
    source/beamline/BioXAS/BioXASSideBeamline.h \
    source/ui/BioXAS/BioXASSidePersistentView.h

SOURCES += \
    source/application/BioXAS/BioXASSideMain.cpp \
    source/application/BioXAS/BioXASSideAppController.cpp \
    source/beamline/BioXAS/BioXASSideBeamline.cpp \
    source/ui/BioXAS/BioXASSidePersistentView.cpp





