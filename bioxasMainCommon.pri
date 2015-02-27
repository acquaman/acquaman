# #####################################################################
# QMake project file for BioXAS Main common files  	October 2014. david.k.chevrier@gmail.com
# ####################################################################

# Source Files (BioXAS Common)
#######################

FORMS +=

HEADERS += \
	source/beamline/BioXAS/BioXASMainMonochromator.h \
    source/beamline/BioXAS/BioXASMainMonochromatorControl.h \
    source/beamline/BioXAS/BioXASMainMonochromatorRegionControl.h \
    source/ui/BioXAS/BioXASMainMonochromatorRegionControlView.h \
    source/ui/BioXAS/BioXASMainMonochromatorRegionView.h

SOURCES += \
	source/beamline/BioXAS/BioXASMainMonochromator.cpp \
    source/beamline/BioXAS/BioXASMainMonochromatorControl.cpp \
    source/beamline/BioXAS/BioXASMainMonochromatorRegionControl.cpp \
    source/ui/BioXAS/BioXASMainMonochromatorRegionControlView.cpp \
    source/ui/BioXAS/BioXASMainMonochromatorRegionView.cpp
