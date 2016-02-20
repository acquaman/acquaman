include ( clsCommon.pri )
include ( bioxasCommon.pri )
include ( bioxasSideCommon.pri )

TARGET = BioXASSideAcquaman

SOURCES += \
	source/application/BioXAS/BioXASSideMain.cpp \
    source/ui/BioXAS/BioXASBiStateControlButton.cpp

HEADERS += \
    source/ui/BioXAS/BioXASBiStateControlButton.h
