include ( clsCommon.pri )
include ( bioxasCommon.pri )
include ( bioxasSideCommon.pri )

TARGET = BioXASSideAcquaman

SOURCES += \
	source/application/BioXAS/BioXASSideMain.cpp \
    source/beamline/BioXAS/BioXASSideZebra.cpp

HEADERS += \
    source/beamline/BioXAS/BioXASSideZebra.h
