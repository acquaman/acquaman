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
	source/ui/BioXAS/BioXASMainXASScanConfigurationView.h \
	source/beamline/BioXAS/BioXAS32ElementGeDetector.h \
	source/ui/BioXAS/BioXAS32ElementGeDetectorView.h \
	source/acquaman/BioXAS/BioXASXRFScanConfiguration.h \
	source/acquaman/BioXAS/BioXASXRFScanController.h

SOURCES += \
	source/beamline/BioXAS/BioXASPseudoMotorControl.cpp \
	source/ui/BioXAS/BioXASCLSMAXvMotorControlEditor.cpp \
    source/acquaman/BioXASMain/BioXASMainXASScanActionController.cpp \
    source/acquaman/BioXASMain/BioXASMainXASScanConfiguration.cpp \
	source/ui/BioXAS/BioXASMainXASScanConfigurationView.cpp \
	source/beamline/BioXAS/BioXAS32ElementGeDetector.cpp \
	source/ui/BioXAS/BioXAS32ElementGeDetectorView.cpp \
	source/acquaman/BioXAS/BioXASXRFScanConfiguration.cpp \
	source/acquaman/BioXAS/BioXASXRFScanController.cpp
