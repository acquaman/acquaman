# #####################################################################
# QMake project file for BioXAS Main common files  	October 2014. david.k.chevrier@gmail.com
# ####################################################################

# Source Files (BioXAS Common)
#######################

FORMS +=

HEADERS += \
	source/beamline/BioXAS/BioXASMainMonochromator.h \
    source/beamline/BioXAS/BioXASMainMonochromatorControl.h \
    source/beamline/BioXAS/BioXASMainMonochromatorCrystalChangeControl.h \
    source/ui/BioXAS/BioXASMainMonochromatorCrystalChangeView.h

SOURCES += \
	source/beamline/BioXAS/BioXASMainMonochromator.cpp \
    source/beamline/BioXAS/BioXASMainMonochromatorControl.cpp \
    source/beamline/BioXAS/BioXASMainMonochromatorCrystalChangeControl.cpp \
    source/ui/BioXAS/BioXASMainMonochromatorCrystalChangeView.cpp
