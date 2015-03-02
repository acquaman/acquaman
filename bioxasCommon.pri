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
	source/beamline/BioXAS/BioXAS32ElementGeDetector.h \
	source/ui/BioXAS/BioXAS32ElementGeDetectorView.h \
	source/acquaman/BioXAS/BioXASXRFScanConfiguration.h \
	source/acquaman/BioXAS/BioXASXRFScanController.h \
	source/beamline/BioXAS/BioXASSSRLMonochromator.h \
    source/beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h

SOURCES += \
	source/beamline/BioXAS/BioXASPseudoMotorControl.cpp \
	source/ui/BioXAS/BioXASCLSMAXvMotorControlEditor.cpp \
	source/beamline/BioXAS/BioXAS32ElementGeDetector.cpp \
	source/ui/BioXAS/BioXAS32ElementGeDetectorView.cpp \
	source/acquaman/BioXAS/BioXASXRFScanConfiguration.cpp \
	source/acquaman/BioXAS/BioXASXRFScanController.cpp \
	source/beamline/BioXAS/BioXASSSRLMonochromator.cpp \
    source/beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.cpp

