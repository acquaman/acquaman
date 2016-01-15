# #####################################################################
# QMake project file for BioXAS Main common files  	October 2014. david.k.chevrier@gmail.com
# ####################################################################

# Source Files (BioXAS Main Common)
#######################


HEADERS += \
    source/application/BioXAS/BioXASMainAppController.h \
    source/beamline/BioXAS/BioXASMainBeamline.h \
    source/beamline/BioXAS/BioXASMainMonochromator.h \
    source/ui/BioXAS/BioXASMainPersistentView.h \
    source/beamline/BioXAS/BioXASMainM2Mirror.h \
    source/beamline/BioXAS/BioXASMainM1Mirror.h \
    source/beamline/BioXAS/BioXASMainM1MirrorBendControl.h \
    source/beamline/BioXAS/BioXASMainCarbonFilterFarm.h \
    source/beamline/BioXAS/BioXASMainXIAFilters.h \
    source/beamline/BioXAS/BioXASMainDBHRMirrors.h \
	source/beamline/BioXAS/BioXASMainM2MirrorBendControl.h \
	source/beamline/BioXAS/BioXASMainStandardsWheel.h \
	source/beamline/BioXAS/BioXASMainMonochromatorMask.h

SOURCES += \
    source/application/BioXAS/BioXASMainAppController.cpp \
    source/beamline/BioXAS/BioXASMainBeamline.cpp \
    source/beamline/BioXAS/BioXASMainMonochromator.cpp \
    source/ui/BioXAS/BioXASMainPersistentView.cpp \
    source/beamline/BioXAS/BioXASMainM2Mirror.cpp \
    source/beamline/BioXAS/BioXASMainM1Mirror.cpp \
    source/beamline/BioXAS/BioXASMainM1MirrorBendControl.cpp \
    source/beamline/BioXAS/BioXASMainCarbonFilterFarm.cpp \
    source/beamline/BioXAS/BioXASMainXIAFilters.cpp \
    source/beamline/BioXAS/BioXASMainDBHRMirrors.cpp \
	source/beamline/BioXAS/BioXASMainM2MirrorBendControl.cpp \
	source/beamline/BioXAS/BioXASMainStandardsWheel.cpp \
	source/beamline/BioXAS/BioXASMainMonochromatorMask.cpp




