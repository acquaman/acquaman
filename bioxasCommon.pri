# #####################################################################
# QMake project file for BioXAS common files  	October 2014. david.k.chevrier@gmail.com
# ####################################################################

# Source Files (BioXAS Common)
#######################

FORMS +=

HEADERS += \
	source/beamline/BioXAS/BioXASBeamlineDef.h \
	source/beamline/BioXAS/BioXASPseudoMotorControl.h \
	source/ui/BioXAS/BioXASCLSMAXvMotorControlEditor.h \
    source/acquaman/BioXASMain/BioXASMainXASScanActionController.h \
    source/acquaman/BioXASMain/BioXASMainXASScanConfiguration.h \
    source/ui/BioXAS/BioXASMainXASScanConfigurationView.h

SOURCES += \
	source/beamline/BioXAS/BioXASPseudoMotorControl.cpp \
	source/ui/BioXAS/BioXASCLSMAXvMotorControlEditor.cpp \
    source/acquaman/BioXASMain/BioXASMainXASScanActionController.cpp \
    source/acquaman/BioXASMain/BioXASMainXASScanConfiguration.cpp \
    source/ui/BioXAS/BioXASMainXASScanConfigurationView.cpp
