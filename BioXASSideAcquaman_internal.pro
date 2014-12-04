include ( acquamanCommon.pri )
include ( bioxasCommon.pri )

TARGET = BioXASSideAcquaman

#DEPENDPATH += source/StripTool2/
#INCLUDEPATH +=  ../testLib
#LIBS+=  -L../testLib/debug -ltestLib

FORMS +=

HEADERS += \
    source/application/BioXAS/BioXASSideAppController.h \
    source/beamline/BioXAS/BioXASSideBeamline.h \
    source/ui/BioXAS/BioXASSidePersistentView.h \
    source/acquaman/BioXASSide/BioXASSideXASScanActionController.h \
    source/acquaman/BioXASSide/BioXASSideXASScanConfiguration.h \
	source/ui/BioXAS/BioXASSideXASScanConfigurationView.h \
	source/beamline/BioXAS/BioXASSideMonochromatorControl.h \
	source/beamline/BioXAS/BioXASSideMonochromator.h \
	source/ui/BioXAS/BioXASSideScalerView.h

SOURCES += \
    source/application/BioXAS/BioXASSideMain.cpp \
    source/application/BioXAS/BioXASSideAppController.cpp \
    source/beamline/BioXAS/BioXASSideBeamline.cpp \
    source/ui/BioXAS/BioXASSidePersistentView.cpp \
    source/acquaman/BioXASSide/BioXASSideXASScanActionController.cpp \
    source/acquaman/BioXASSide/BioXASSideXASScanConfiguration.cpp \
	source/ui/BioXAS/BioXASSideXASScanConfigurationView.cpp \
	source/beamline/BioXAS/BioXASSideMonochromatorControl.cpp \
	source/beamline/BioXAS/BioXASSideMonochromator.cpp \
	source/ui/BioXAS/BioXASSideScalerView.cpp
