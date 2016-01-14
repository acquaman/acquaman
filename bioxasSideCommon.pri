# #####################################################################
# QMake project file for BioXAS Side beamline common files  	Dec 2014. Sheldon Liu
# ####################################################################

# Source Files (BioXAS Side Common)
#######################

HEADERS += \
    source/application/BioXAS/BioXASSideAppController.h \
    source/beamline/BioXAS/BioXASSideBeamline.h \
    source/beamline/BioXAS/BioXASSideMonochromator.h \
    source/ui/BioXAS/BioXASSidePersistentView.h \
	source/beamline/BioXAS/BioXASSideM2Mirror.h \
	source/beamline/BioXAS/BioXASSideXIAFilters.h \
    source/beamline/BioXAS/BioXASSideCarbonFilterFarm.h \
	source/beamline/BioXAS/BioXASSideM1Mirror.h \
    source/beamline/BioXAS/BioXASSideM1MirrorBendControl.h \
	source/beamline/BioXAS/BioXASSideM2MirrorBendControl.h \
    source/beamline/BioXAS/BioXASSideDBHRMirrors.h \
    source/beamline/BioXAS/BioXASSideBeamlineUtilities.h \
    source/beamline/BioXAS/BioXASSideCryostatStage.h \
    source/beamline/BioXAS/BioXASSideMonochromatorMask.h \
	source/beamline/BioXAS/BioXASSideBeamStatus.h \
	source/beamline/BioXAS/BioXASSidePOEBeamStatus.h \
	source/beamline/BioXAS/BioXASSideSOEBeamStatus.h \
	source/ui/BioXAS/BioXASSidePOEBeamStatusView.h \
	source/ui/BioXAS/BioXASSideSOEBeamStatusView.h \
	source/ui/BioXAS/BioXASSideBeamStatusView.h \
	source/beamline/BioXAS/BioXASSideFilterFlipper.h \
    source/beamline/BioXAS/BioXASSideShutters.h

SOURCES += \
    source/application/BioXAS/BioXASSideAppController.cpp \
    source/beamline/BioXAS/BioXASSideBeamline.cpp \
    source/beamline/BioXAS/BioXASSideMonochromator.cpp \
    source/ui/BioXAS/BioXASSidePersistentView.cpp \
	source/beamline/BioXAS/BioXASSideM2Mirror.cpp \
	source/beamline/BioXAS/BioXASSideXIAFilters.cpp \
    source/beamline/BioXAS/BioXASSideCarbonFilterFarm.cpp \
	source/beamline/BioXAS/BioXASSideM1Mirror.cpp \
    source/beamline/BioXAS/BioXASSideM1MirrorBendControl.cpp \
	source/beamline/BioXAS/BioXASSideM2MirrorBendControl.cpp \
    source/beamline/BioXAS/BioXASSideDBHRMirrors.cpp \
    source/beamline/BioXAS/BioXASSideBeamlineUtilities.cpp \
    source/beamline/BioXAS/BioXASSideCryostatStage.cpp \
    source/beamline/BioXAS/BioXASSideMonochromatorMask.cpp \
	source/beamline/BioXAS/BioXASSideBeamStatus.cpp \
	source/beamline/BioXAS/BioXASSidePOEBeamStatus.cpp \
	source/beamline/BioXAS/BioXASSideSOEBeamStatus.cpp \
	source/ui/BioXAS/BioXASSidePOEBeamStatusView.cpp \
	source/ui/BioXAS/BioXASSideSOEBeamStatusView.cpp \
	source/ui/BioXAS/BioXASSideBeamStatusView.cpp \
	source/beamline/BioXAS/BioXASSideFilterFlipper.cpp \
    source/beamline/BioXAS/BioXASSideShutters.cpp


