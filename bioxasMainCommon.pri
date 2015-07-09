# #####################################################################
# QMake project file for BioXAS Main common files  	October 2014. david.k.chevrier@gmail.com
# ####################################################################

# Source Files (BioXAS Main Common)
#######################


HEADERS += \
    source/application/BioXAS/BioXASMainAppController.h \
    source/beamline/BioXAS/BioXASMainBeamline.h \
    source/beamline/BioXAS/BioXASMainMonochromator.h \
    source/acquaman/BioXAS/BioXASMainXASScanConfiguration.h \
    source/acquaman/BioXAS/BioXASMainXASScanActionController.h \
    source/ui/BioXAS/BioXASMainPersistentView.h \
    source/beamline/BioXAS/BioXASMainM2Mirror.h \
    source/beamline/BioXAS/BioXASMainM1Mirror.h \
	source/beamline/BioXAS/BioXASMainM1MirrorBendControl.h
SOURCES += \
    source/application/BioXAS/BioXASMainAppController.cpp \
    source/beamline/BioXAS/BioXASMainBeamline.cpp \
    source/beamline/BioXAS/BioXASMainMonochromator.cpp \
    source/acquaman/BioXAS/BioXASMainXASScanConfiguration.cpp \
    source/acquaman/BioXAS/BioXASMainXASScanActionController.cpp \
    source/ui/BioXAS/BioXASMainPersistentView.cpp \
    source/beamline/BioXAS/BioXASMainM2Mirror.cpp \
    source/beamline/BioXAS/BioXASMainM1Mirror.cpp \
	source/beamline/BioXAS/BioXASMainM1MirrorBendControl.cpp



























