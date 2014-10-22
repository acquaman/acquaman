include ( acquamanCommon.pri )

TARGET = BioXASSideAcquaman

HEADERS += \
    source/application/BioXAS/BioXASSideAppController.h \
    source/beamline/BioXAS/BioXASSideBeamline.h \
    source/ui/BioXAS/BioXASSidePersistentView.h \
    source/acquaman/BioXASSide/BioXASSideXASScanActionController.h \
    source/acquaman/BioXASSide/BioXASSideXASScanConfiguration.h \
	source/ui/BioXAS/BioXASSideXASScanConfigurationView.h \
    source/beamline/BioXAS/BioXASSideMonochromator.h \
    source/ui/BioXAS/BioXASSideMonoBasicEnergyView.h \
    source/ui/BioXAS/BioXASSideMonoCrystalChangeView.h \
    source/beamline/BioXAS/BioXASSideMonochromatorControl.h \
    source/acquaman/BioXASSide/BioXASSideMonoCrystalChangeSteps.h

SOURCES += \
    source/application/BioXAS/BioXASSideMain.cpp \
    source/application/BioXAS/BioXASSideAppController.cpp \
    source/beamline/BioXAS/BioXASSideBeamline.cpp \
    source/ui/BioXAS/BioXASSidePersistentView.cpp \
    source/acquaman/BioXASSide/BioXASSideXASScanActionController.cpp \
    source/acquaman/BioXASSide/BioXASSideXASScanConfiguration.cpp \
	source/ui/BioXAS/BioXASSideXASScanConfigurationView.cpp \
    source/beamline/BioXAS/BioXASSideMonochromator.cpp \
    source/ui/BioXAS/BioXASSideMonoBasicEnergyView.cpp \
    source/ui/BioXAS/BioXASSideMonoCrystalChangeView.cpp \
    source/beamline/BioXAS/BioXASSideMonochromatorControl.cpp \
    source/acquaman/BioXASSide/BioXASSideMonoCrystalChangeSteps.cpp
