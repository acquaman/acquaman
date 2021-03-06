# #####################################################################
# QMake project file for BioXAS Side beamline common files  	Dec 2014. Sheldon Liu
# ####################################################################

# Source Files (BioXAS Side Common)
#######################

HEADERS += \
    source/application/BioXAS/BioXASSideAppController.h \
    source/beamline/BioXAS/BioXASSideBeamline.h \
    source/beamline/BioXAS/BioXASSideMonochromator.h \
	source/beamline/BioXAS/BioXASSideM2Mirror.h \
	source/beamline/BioXAS/BioXASSideXIAFilters.h \
    source/beamline/BioXAS/BioXASSideCarbonFilterFarm.h \
	source/beamline/BioXAS/BioXASSideM1Mirror.h \
    source/beamline/BioXAS/BioXASSideM1MirrorBendControl.h \
	source/beamline/BioXAS/BioXASSideM2MirrorBendControl.h \
    source/beamline/BioXAS/BioXASSideDBHRMirrors.h \
    source/beamline/BioXAS/BioXASSideCryostatStage.h \
	source/beamline/BioXAS/BioXASSideFilterFlipper.h \
	source/beamline/BioXAS/BioXASSide32ElementGeDetector.h \
	source/beamline/BioXAS/BioXASSideCryostat.h

SOURCES += \
    source/application/BioXAS/BioXASSideAppController.cpp \
    source/beamline/BioXAS/BioXASSideBeamline.cpp \
    source/beamline/BioXAS/BioXASSideMonochromator.cpp \
	source/beamline/BioXAS/BioXASSideM2Mirror.cpp \
	source/beamline/BioXAS/BioXASSideXIAFilters.cpp \
    source/beamline/BioXAS/BioXASSideCarbonFilterFarm.cpp \
	source/beamline/BioXAS/BioXASSideM1Mirror.cpp \
    source/beamline/BioXAS/BioXASSideM1MirrorBendControl.cpp \
	source/beamline/BioXAS/BioXASSideM2MirrorBendControl.cpp \
    source/beamline/BioXAS/BioXASSideDBHRMirrors.cpp \
    source/beamline/BioXAS/BioXASSideCryostatStage.cpp \
	source/beamline/BioXAS/BioXASSideFilterFlipper.cpp \
	source/beamline/BioXAS/BioXASSide32ElementGeDetector.cpp \
	source/beamline/BioXAS/BioXASSideCryostat.cpp
