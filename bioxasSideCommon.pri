# #####################################################################
# QMake project file for BioXAS Side beamline common files  	Dec 2014. Sheldon Liu
# ####################################################################

# Source Files (BioXAS Side Common)
#######################

include ( bioxasCommon.pri )

HEADERS += \
	source/beamline/BioXAS/BioXASSideBeamline.h \
	source/beamline/BioXAS/BioXASSideMonochromatorControl.h \
	source/beamline/BioXAS/BioXASSideMonochromator.h \
    source/beamline/BioXAS/BioXASSideMonochromatorRegionControl.h

SOURCES += \
	source/beamline/BioXAS/BioXASSideBeamline.cpp \
	source/beamline/BioXAS/BioXASSideMonochromatorControl.cpp \
	source/beamline/BioXAS/BioXASSideMonochromator.cpp \
    source/beamline/BioXAS/BioXASSideMonochromatorRegionControl.cpp





