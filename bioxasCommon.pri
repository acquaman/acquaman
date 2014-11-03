# #####################################################################
# QMake project file for BioXAS common files  	October 2014. david.k.chevrier@gmail.com
# ####################################################################

# Source Files (BioXAS Common)
#######################

FORMS += 

HEADERS += \
	source/beamline/BioXAS/BioXASBeamlineDef.h \
	source/beamline/BioXAS/BioXASCLSMAXvMotor.h \
	source/ui/BioXAS/BioXASCLSMAXvMotorControlEditor.h

SOURCES += \
	source/beamline/BioXAS/BioXASCLSMAXvMotor.cpp \
	source/ui/BioXAS/BioXASCLSMAXvMotorControlEditor.cpp

