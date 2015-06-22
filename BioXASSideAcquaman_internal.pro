
include ( clsCommon.pri )

include ( bioxasCommon.pri )
include ( bioxasSideCommon.pri )

TARGET = BioXASSideAcquaman

HEADERS += \
	source/ui/BioXAS/BioXAS32ElementGeDetectorView.h \
	source/ui/BioXAS/BioXASEndstationTableView.h

SOURCES += \
	source/application/BioXAS/BioXASSideMain.cpp \
	source/ui/BioXAS/BioXAS32ElementGeDetectorView.cpp \
	source/ui/BioXAS/BioXASEndstationTableView.cpp

