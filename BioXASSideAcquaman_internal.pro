
include ( clsCommon.pri )

include ( bioxasCommon.pri )
include ( bioxasSideCommon.pri )

TARGET = BioXASSideAcquaman

HEADERS += \
    source/beamline/BioXAS/BioXASMasterValves.h \
    source/beamline/BioXAS/BioXASFrontEndBeamStatus.h


SOURCES += \
	source/application/BioXAS/BioXASSideMain.cpp \
    source/beamline/BioXAS/BioXASMasterValves.cpp \
    source/beamline/BioXAS/BioXASFrontEndBeamStatus.cpp
