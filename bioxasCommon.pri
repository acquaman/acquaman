# #####################################################################
# QMake project file for BioXAS common files      October 2014. david.k.chevrier@gmail.com
# ####################################################################

# Source Files (BioXAS Common)
#######################

HEADERS += \
	source/acquaman/BioXAS/BioXASXRFScanConfiguration.h \
	source/acquaman/BioXAS/BioXASXRFScanController.h \
	source/beamline/BioXAS/BioXASBeamlineDef.h \
	source/beamline/BioXAS/BioXASPseudoMotorControl.h \
	source/beamline/BioXAS/BioXAS32ElementGeDetector.h \
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
	source/beamline/BioXAS/BioXASXIAFilters.h \
	source/ui/BioXAS/BioXASXIAFiltersView.h \
	source/beamline/BioXAS/BioXASBeamline.h \
	source/beamline/BioXAS/BioXASMonochromator.h \
	source/beamline/BioXAS/BioXASM2Mirror.h \
	source/ui/BioXAS/BioXASM2MirrorView.h \
	source/beamline/BioXAS/BioXASEndstationTable.h \
	source/beamline/BioXAS/BioXASM1Mirror.h \
	source/beamline/BioXAS/BioXASBeamlineComponent.h \
	source/beamline/BioXAS/BioXASMirrorMotor.h \
	source/beamline/BioXAS/BioXASMirror.h \
	source/ui/BioXAS/BioXASMirrorView.h \
	source/ui/BioXAS/BioXASM1MirrorView.h \
	source/beamline/BioXAS/BioXASMirrorPitchControl.h \
	source/beamline/BioXAS/BioXASMirrorRollControl.h \
	source/beamline/BioXAS/BioXASMirrorHeightControl.h \
	source/beamline/BioXAS/BioXASMirrorLateralControl.h \
	source/beamline/BioXAS/BioXASMirrorYawControl.h \
	source/beamline/BioXAS/BioXASMirrorControl.h \
	source/beamline/BioXAS/BioXASMirrorBendControl.h \
	source/beamline/BioXAS/BioXASFourElementVortexDetector.h \
	source/ui/BioXAS/BioXASFourElementVortexDetectorView.h \
	source/ui/BioXAS/BioXASXRFDetectorView.h \
	source/application/BioXAS/BioXASAppController.h \
	source/ui/BioXAS/BioXASPersistentView.h \
	source/ui/BioXAS/BioXASMirrorBendView.h \
	source/application/BioXAS/BioXAS.h \
    source/beamline/BioXAS/BioXASMonochromatorEnergyControl.h \
    source/acquaman/BioXAS/BioXASXASScanActionController.h \
    source/acquaman/BioXAS/BioXASXASScanConfiguration.h \
	source/ui/BioXAS/BioXASSSRLMonochromatorEnergyCalibrationView.h \
	source/ui/BioXAS/BioXASEndstationTableView.h \
	source/ui/BioXAS/BioXAS32ElementGeDetectorView.h \
    source/beamline/BioXAS/BioXASDBHRMirrors.h \
	source/ui/BioXAS/BioXASDBHRMirrorsView.h \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.h \
    source/beamline/BioXAS/BioXASBeamlineUtilities.h \
    source/beamline/BioXAS/BioXASCryostatStage.h \
    source/ui/BioXAS/BioXASXASScanConfigurationRegionsEditor.h \
    source/ui/BioXAS/BioXASXASScanConfigurationEdgeEditor.h \
    source/ui/BioXAS/BioXASXASScanConfigurationEditor.h \
    source/ui/BioXAS/BioXASXASScanConfigurationView.h \
    source/dataman/BioXAS/BioXASDbUpgrade1Pt1.h \
	source/beamline/BioXAS/BioXASFrontEndShutters.h \
	source/beamline/BioXAS/BioXASValves.h \
    source/ui/BioXAS/BioXASValvesView.h \
    source/beamline/BioXAS/BioXASFrontEndValves.h \
    source/beamline/BioXAS/BioXASSideValves.h \
    source/beamline/BioXAS/BioXASMainValves.h \
    source/beamline/BioXAS/BioXASImagingValves.h \
    source/ui/BioXAS/BioXASFrontEndShuttersView.h \
    source/beamline/BioXAS/BioXASBeamStatusControl.h \
    source/beamline/BioXAS/BioXASSSRLMonochromatorMaskControl.h \
    source/ui/BioXAS/BioXASSidePOEBeamStatusControlView.h \
    source/ui/BioXAS/BioXASSidePOEBeamStatusControlLEDView.h \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorPositionControl.h \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorFilterControl.h \
    source/beamline/BioXAS/BioXASCarbonFilterFarmControl.h \
    source/beamline/BioXAS/BioXASCarbonFilterFarmFilterControl.h \
    source/ui/BioXAS/BioXASCarbonFilterFarmActuatorControlView.h \
    source/ui/BioXAS/BioXASCarbonFilterFarmControlView.h \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuator.h \
    source/beamline/BioXAS/BioXASXIAFiltersActuatorControl.h \
    source/beamline/BioXAS/BioXASXIAFiltersActuator.h \
	source/beamline/AMExclusiveStatesEnumeratedControl.h \
	source/beamline/BioXAS/BioXASBeamStatus.h \
	source/beamline/BioXAS/BioXASBiStateGroup.h \
	source/beamline/BioXAS/BioXASShutters.h \
    source/beamline/BioXAS/BioXASMasterValves.h \
	source/beamline/BioXAS/BioXASBeamlineBeamStatus.h

SOURCES += \
	source/acquaman/BioXAS/BioXASXRFScanConfiguration.cpp \
	source/acquaman/BioXAS/BioXASXRFScanController.cpp \
	source/beamline/BioXAS/BioXASPseudoMotorControl.cpp \
	source/beamline/BioXAS/BioXAS32ElementGeDetector.cpp \
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
	source/beamline/BioXAS/BioXASXIAFilters.cpp \
	source/ui/BioXAS/BioXASXIAFiltersView.cpp \
	source/beamline/BioXAS/BioXASBeamline.cpp \
	source/beamline/BioXAS/BioXASMonochromator.cpp \
	source/beamline/BioXAS/BioXASM2Mirror.cpp \
	source/ui/BioXAS/BioXASM2MirrorView.cpp \
	source/beamline/BioXAS/BioXASEndstationTable.cpp \
	source/beamline/BioXAS/BioXASM1Mirror.cpp \
	source/beamline/BioXAS/BioXASBeamlineComponent.cpp \
	source/beamline/BioXAS/BioXASMirrorMotor.cpp \
	source/beamline/BioXAS/BioXASMirror.cpp \
	source/ui/BioXAS/BioXASMirrorView.cpp \
	source/ui/BioXAS/BioXASM1MirrorView.cpp \
	source/beamline/BioXAS/BioXASMirrorPitchControl.cpp \
	source/beamline/BioXAS/BioXASMirrorRollControl.cpp \
	source/beamline/BioXAS/BioXASMirrorHeightControl.cpp \
	source/beamline/BioXAS/BioXASMirrorLateralControl.cpp \
	source/beamline/BioXAS/BioXASMirrorYawControl.cpp \
	source/beamline/BioXAS/BioXASMirrorControl.cpp \
	source/beamline/BioXAS/BioXASMirrorBendControl.cpp \
	source/beamline/BioXAS/BioXASFourElementVortexDetector.cpp \
	source/ui/BioXAS/BioXASFourElementVortexDetectorView.cpp \
	source/ui/BioXAS/BioXASXRFDetectorView.cpp \
	source/application/BioXAS/BioXASAppController.cpp \
	source/ui/BioXAS/BioXASPersistentView.cpp \
	source/ui/BioXAS/BioXASMirrorBendView.cpp \
    source/beamline/BioXAS/BioXASMonochromatorEnergyControl.cpp \
    source/acquaman/BioXAS/BioXASXASScanActionController.cpp \
    source/acquaman/BioXAS/BioXASXASScanConfiguration.cpp \
	source/ui/BioXAS/BioXASSSRLMonochromatorEnergyCalibrationView.cpp \
	source/ui/BioXAS/BioXASEndstationTableView.cpp \
	source/ui/BioXAS/BioXAS32ElementGeDetectorView.cpp \
    source/beamline/BioXAS/BioXASDBHRMirrors.cpp \
	source/ui/BioXAS/BioXASDBHRMirrorsView.cpp \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorWindowControl.cpp \
    source/beamline/BioXAS/BioXASBeamlineUtilities.cpp \
    source/beamline/BioXAS/BioXASCryostatStage.cpp \
    source/ui/BioXAS/BioXASXASScanConfigurationRegionsEditor.cpp \
    source/ui/BioXAS/BioXASXASScanConfigurationEdgeEditor.cpp \
    source/ui/BioXAS/BioXASXASScanConfigurationEditor.cpp \
    source/ui/BioXAS/BioXASXASScanConfigurationView.cpp \
    source/dataman/BioXAS/BioXASDbUpgrade1Pt1.cpp \
	source/beamline/BioXAS/BioXASFrontEndShutters.cpp \
	source/beamline/BioXAS/BioXASValves.cpp \
    source/ui/BioXAS/BioXASValvesView.cpp \
    source/beamline/BioXAS/BioXASFrontEndValves.cpp \
    source/beamline/BioXAS/BioXASSideValves.cpp \
    source/beamline/BioXAS/BioXASMainValves.cpp \
    source/beamline/BioXAS/BioXASImagingValves.cpp \
    source/ui/BioXAS/BioXASFrontEndShuttersView.cpp \
    source/beamline/BioXAS/BioXASBeamStatusControl.cpp \
    source/beamline/BioXAS/BioXASSSRLMonochromatorMaskControl.cpp \
    source/ui/BioXAS/BioXASSidePOEBeamStatusControlView.cpp \
    source/ui/BioXAS/BioXASSidePOEBeamStatusControlLEDView.cpp \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorPositionControl.cpp \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.cpp \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorFilterControl.cpp \
    source/beamline/BioXAS/BioXASCarbonFilterFarmControl.cpp \
    source/beamline/BioXAS/BioXASCarbonFilterFarmFilterControl.cpp \
    source/ui/BioXAS/BioXASCarbonFilterFarmActuatorControlView.cpp \
    source/ui/BioXAS/BioXASCarbonFilterFarmControlView.cpp \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuator.cpp \
    source/beamline/BioXAS/BioXASXIAFiltersActuatorControl.cpp \
    source/beamline/BioXAS/BioXASXIAFiltersActuator.cpp \
	source/beamline/AMExclusiveStatesEnumeratedControl.cpp \
	source/beamline/BioXAS/BioXASBeamStatus.cpp \
	source/beamline/BioXAS/BioXASBiStateGroup.cpp \
	source/beamline/BioXAS/BioXASShutters.cpp \
    source/beamline/BioXAS/BioXASMasterValves.cpp \
	source/beamline/BioXAS/BioXASBeamlineBeamStatus.cpp
