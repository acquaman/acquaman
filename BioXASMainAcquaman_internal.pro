include ( acquamanCommon.pri )
include ( bioxasCommon.pri )
include ( bioxasMainCommon.pri )

TARGET = BioXASMainAcquaman

FORMS +=

HEADERS += \
	source/application/BioXAS/BioXASMainAppController.h \
	source/beamline/BioXAS/BioXASMainBeamline.h \
	source/acquaman/BioXASMain/BioXASMainXASScanActionController.h \
	source/acquaman/BioXASMain/BioXASMainXASScanConfiguration.h \
	source/ui/BioXAS/BioXASMainXASScanConfigurationView.h

SOURCES += \
	source/application/BioXAS/BioXASMainMain.cpp \
	source/application/BioXAS/BioXASMainAppController.cpp \
	source/beamline/BioXAS/BioXASMainBeamline.cpp \
	source/acquaman/BioXASMain/BioXASMainXASScanActionController.cpp \
	source/acquaman/BioXASMain/BioXASMainXASScanConfiguration.cpp \
	source/ui/BioXAS/BioXASMainXASScanConfigurationView.cpp \
