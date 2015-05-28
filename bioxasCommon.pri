# #####################################################################
# QMake project file for BioXAS common files  	October 2014. david.k.chevrier@gmail.com
# ####################################################################

# Source Files (BioXAS Common)
#######################

HEADERS += \
	source/beamline/BioXAS/BioXASBeamlineDef.h \
	source/beamline/BioXAS/BioXASPseudoMotorControl.h \
	source/ui/BioXAS/BioXASCLSMAXvMotorControlEditor.h \
	source/beamline/BioXAS/BioXAS32ElementGeDetector.h \
	source/ui/BioXAS/BioXAS32ElementGeDetectorView.h \
	source/acquaman/BioXAS/BioXASXRFScanConfiguration.h \
	source/acquaman/BioXAS/BioXASXRFScanController.h \
	source/beamline/BioXAS/BioXASSSRLMonochromator.h \
	source/beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h \
	source/ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.h \
	source/ui/BioXAS/BioXASSSRLMonochromatorRegionControlView.h \
	source/beamline/BioXAS/BioXASSSRLMonochromatorEnergyControl.h \
	source/ui/BioXAS/BioXASSSRLMonochromatorConfigurationView.h \
	source/ui/BioXAS/BioXASSIS3820ScalerChannelView.h \
	source/ui/BioXAS/BioXASSIS3820ScalerView.h \
	source/ui/BioXAS/BioXASSIS3820ScalerChannelsView.h \
	source/dataman/BioXAS/BioXASUserConfiguration.h \
	source/acquaman/BioXAS/BioXASScanConfigurationDbObject.h \
	source/acquaman/BioXAS/BioXASScanConfiguration.h \
	source/beamline/BioXAS/BioXASCarbonFilterFarm.h \
	source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h \
	source/beamline/BioXAS/BioXASXIAFilters.h \
    source/ui/BioXAS/BioXASXIAFiltersView.h \
	source/ui/BioXAS/BioXASCarbonFilterFarmView.h \
    source/beamline/BioXAS/BioXASBeamline.h \
    source/beamline/BioXAS/BioXASMonochromator.h \
    source/beamline/BioXAS/BioXASM2Mirror.h \
	source/ui/BioXAS/BioXASM2MirrorView.h \
    source/beamline/BioXAS/BioXASDBHRMirror.h \
    source/ui/BioXAS/BioXASDBHRMirrorView.h \
    source/beamline/BioXAS/BioXASM1Mirror.h \
	source/beamline/BioXAS/BioXASBeamlineComponent.h \
	source/beamline/BioXAS/BioXASCarbonFilterFarmControl.h \
    source/ui/BioXAS/BioXASBeamlineBeamView.h \
    source/beamline/BioXAS/BioXASMirrorMotor.h \
    source/beamline/BioXAS/BioXASMirror.h \
    source/ui/BioXAS/BioXASMirrorView.h \
	source/ui/BioXAS/BioXASM1MirrorView.h \
	source/beamline/BioXAS/BioXASMirrorPitchControl.h \
    source/beamline/BioXAS/BioXASMirrorRollControl.h \
    source/beamline/BioXAS/BioXASMirrorPseudoMotorControl.h \
    source/beamline/BioXAS/BioXASMirrorHeightControl.h \
    source/beamline/BioXAS/BioXASMirrorLateralControl.h \
    source/beamline/BioXAS/BioXASMirrorYawControl.h \
    source/beamline/BioXAS/BioXASMirrorControl.h \
    source/beamline/BioXAS/BioXASMirrorBendControl.h

SOURCES += \
	source/beamline/BioXAS/BioXASPseudoMotorControl.cpp \
	source/ui/BioXAS/BioXASCLSMAXvMotorControlEditor.cpp \
	source/beamline/BioXAS/BioXAS32ElementGeDetector.cpp \
	source/ui/BioXAS/BioXAS32ElementGeDetectorView.cpp \
	source/acquaman/BioXAS/BioXASXRFScanConfiguration.cpp \
	source/acquaman/BioXAS/BioXASXRFScanController.cpp \
	source/beamline/BioXAS/BioXASSSRLMonochromator.cpp \
	source/beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.cpp \
	source/ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.cpp \
	source/ui/BioXAS/BioXASSSRLMonochromatorRegionControlView.cpp \
	source/beamline/BioXAS/BioXASSSRLMonochromatorEnergyControl.cpp \
	source/ui/BioXAS/BioXASSSRLMonochromatorConfigurationView.cpp \
	source/ui/BioXAS/BioXASSIS3820ScalerChannelView.cpp \
	source/ui/BioXAS/BioXASSIS3820ScalerView.cpp \
	source/ui/BioXAS/BioXASSIS3820ScalerChannelsView.cpp \
	source/dataman/BioXAS/BioXASUserConfiguration.cpp \
	source/acquaman/BioXAS/BioXASScanConfigurationDbObject.cpp \
	source/acquaman/BioXAS/BioXASScanConfiguration.cpp \
	source/beamline/BioXAS/BioXASCarbonFilterFarm.cpp \
	source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.cpp \
	source/beamline/BioXAS/BioXASXIAFilters.cpp \
    source/ui/BioXAS/BioXASXIAFiltersView.cpp \
	source/ui/BioXAS/BioXASCarbonFilterFarmView.cpp \
    source/beamline/BioXAS/BioXASBeamline.cpp \
	source/beamline/BioXAS/BioXASMonochromator.cpp \
    source/beamline/BioXAS/BioXASM2Mirror.cpp \
	source/ui/BioXAS/BioXASM2MirrorView.cpp \
    source/beamline/BioXAS/BioXASDBHRMirror.cpp \
    source/ui/BioXAS/BioXASDBHRMirrorView.cpp \
    source/beamline/BioXAS/BioXASM1Mirror.cpp \
	source/beamline/BioXAS/BioXASBeamlineComponent.cpp \
	source/beamline/BioXAS/BioXASCarbonFilterFarmControl.cpp \
	source/ui/BioXAS/BioXASBeamlineBeamView.cpp \
    source/beamline/BioXAS/BioXASMirrorMotor.cpp \
    source/beamline/BioXAS/BioXASMirror.cpp \
    source/ui/BioXAS/BioXASMirrorView.cpp \
	source/ui/BioXAS/BioXASM1MirrorView.cpp \
	source/beamline/BioXAS/BioXASMirrorPitchControl.cpp \
    source/beamline/BioXAS/BioXASMirrorRollControl.cpp \
    source/beamline/BioXAS/BioXASMirrorPseudoMotorControl.cpp \
    source/beamline/BioXAS/BioXASMirrorHeightControl.cpp \
    source/beamline/BioXAS/BioXASMirrorLateralControl.cpp \
    source/beamline/BioXAS/BioXASMirrorYawControl.cpp \
    source/beamline/BioXAS/BioXASMirrorControl.cpp \
    source/beamline/BioXAS/BioXASMirrorBendControl.cpp






















