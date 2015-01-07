include ( acquamanCommon.pri )
include ( bioxasCommon.pri )
include ( bioxasMainCommon.pri )

TARGET = BioXASMainAcquaman

FORMS +=

HEADERS += \
    source/application/BioXAS/BioXASMainAppController.h \
	source/ui/BioXAS/BioXASMainPersistentView.h \
    source/beamline/BioXAS/BioXASMainBeamline.h

SOURCES += \
    source/application/BioXAS/BioXASMainMain.cpp \
    source/application/BioXAS/BioXASMainAppController.cpp \
	source/ui/BioXAS/BioXASMainPersistentView.cpp \
    source/beamline/BioXAS/BioXASMainBeamline.cpp
