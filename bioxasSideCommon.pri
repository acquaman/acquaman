# #####################################################################
# QMake project file for BioXAS Side beamline common files  	Dec 2014. Sheldon Liu
# ####################################################################

# Source Files (BioXAS Side Common)
#######################

HEADERS += \
    source/application/BioXAS/BioXASSideAppController.h \
    source/beamline/BioXAS/BioXASSideBeamline.h \
    source/beamline/BioXAS/BioXASSideMonochromator.h \
    source/beamline/BioXAS/BioXASSideMonochromatorRegionControl.h \
    source/beamline/BioXAS/BioXASSideMonochromatorEnergyControl.h \
    source/acquaman/BioXAS/BioXASSideXASScanActionController.h \
    source/acquaman/BioXAS/BioXASSideXASScanConfiguration.h \
    source/ui/BioXAS/BioXASSideXASScanConfigurationView.h \
    source/ui/BioXAS/BioXASSidePersistentView.h

SOURCES += \
    source/application/BioXAS/BioXASSideAppController.cpp \
    source/beamline/BioXAS/BioXASSideBeamline.cpp \
    source/beamline/BioXAS/BioXASSideMonochromator.cpp \
    source/beamline/BioXAS/BioXASSideMonochromatorRegionControl.cpp \
    source/beamline/BioXAS/BioXASSideMonochromatorEnergyControl.cpp \
    source/acquaman/BioXAS/BioXASSideXASScanActionController.cpp \
    source/acquaman/BioXAS/BioXASSideXASScanConfiguration.cpp \
    source/ui/BioXAS/BioXASSideXASScanConfigurationView.cpp \
    source/ui/BioXAS/BioXASSidePersistentView.cpp






















