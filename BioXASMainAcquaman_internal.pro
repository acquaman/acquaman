include ( clsCommon.pri )
include ( bioxasCommon.pri )
include ( bioxasMainCommon.pri )

TARGET = BioXASMainAcquaman

SOURCES += \
    source/application/BioXAS/BioXASMainMain.cpp \
    source/beamline/BioXAS/BioXASShutters.cpp \
    source/beamline/BioXAS/BioXASBiStateControl.cpp

HEADERS += \
    source/beamline/BioXAS/BioXASShutters.h \
    source/beamline/BioXAS/BioXASBiStateControl.h

