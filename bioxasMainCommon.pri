# #####################################################################
# QMake project file for BioXAS Main common files  	October 2014. david.k.chevrier@gmail.com
# ####################################################################

# Source Files (BioXAS Common)
#######################

FORMS +=

HEADERS += \
	source/beamline/BioXAS/BioXASMainMonochromator.h \
    source/beamline/BioXAS/BioXASMainMonochromatorControl.h \
	source/ui/BioXAS/BioXASMainMonoCrystalChangeView.h

SOURCES += \
	source/beamline/BioXAS/BioXASMainMonochromator.cpp \
    source/beamline/BioXAS/BioXASMainMonochromatorControl.cpp \
	source/ui/BioXAS/BioXASMainMonoCrystalChangeView.cpp
