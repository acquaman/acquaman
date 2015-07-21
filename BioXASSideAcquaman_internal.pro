
include ( clsCommon.pri )

include ( bioxasCommon.pri )
include ( bioxasSideCommon.pri )

TARGET = BioXASSideAcquaman

HEADERS += \
	source/ui/BioXAS/BioXAS32ElementGeDetectorView.h

SOURCES += \
	source/application/BioXAS/BioXASSideMain.cpp \
	source/ui/BioXAS/BioXAS32ElementGeDetectorView.cpp
