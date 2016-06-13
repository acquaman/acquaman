# #####################################################################
# QMake project file for BioXAS common files      October 2014. david.k.chevrier@gmail.com
# ####################################################################

# Source Files (BioXAS Common)
#######################

HEADERS += \
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
    source/beamline/BioXAS/BioXASCryostatStage.h \
    source/ui/BioXAS/BioXASXASScanConfigurationRegionsEditor.h \
    source/ui/BioXAS/BioXASXASScanConfigurationEdgeEditor.h \
    source/ui/BioXAS/BioXASXASScanConfigurationEditor.h \
    source/ui/BioXAS/BioXASXASScanConfigurationView.h \
    source/dataman/BioXAS/BioXASDbUpgrade1Pt1.h \
	source/beamline/BioXAS/BioXASFrontEndShutters.h \
    source/ui/BioXAS/BioXASValvesView.h \
    source/ui/BioXAS/BioXASFrontEndShuttersView.h \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorPositionControl.h \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorFilterControl.h \
    source/beamline/BioXAS/BioXASCarbonFilterFarmFilterControl.h \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuator.h \
    source/beamline/BioXAS/BioXASXIAFiltersActuatorControl.h \
	source/beamline/BioXAS/BioXASXIAFiltersActuator.h \
    source/beamline/BioXAS/BioXASZebra.h \
	source/beamline/BioXAS/BioXASSIS3820Scaler.h \
    source/beamline/BioXAS/BioXASZebraPulseControl.h \
    source/beamline/BioXAS/BioXASZebraCommands.h \
    source/ui/BioXAS/BioXASZebraView.h \
    source/ui/BioXAS/BioXASZebraPulseControlView.h \
    source/beamline/BioXAS/BioXASM1MirrorMask.h \
	source/beamline/BioXAS/BioXASFilterFlipper.h \
	source/beamline/BioXAS/BioXASFilterFlipperFilter.h \
	source/ui/BioXAS/BioXASFilterFlipperView.h \
	source/ui/BioXAS/BioXASFilterFlipperFilterView.h \
	source/beamline/BioXAS/BioXASFilterFlipperFilters.h \
	source/ui/BioXAS/BioXASFilterFlipperConfigurationView.h \
	source/ui/BioXAS/BioXASM1MirrorMaskView.h \
    source/ui/BioXAS/BioXASSSRLMonochromatorView.h \
    source/ui/BioXAS/BioXASSSRLMonochromatorBasicView.h \
    source/ui/BioXAS/BioXASSSRLMonochromatorCrystalsView.h \
    source/ui/BioXAS/BioXASSSRLMonochromatorEnergyView.h \
    source/ui/BioXAS/BioXASSSRLMonochromatorCalibrationView.h \
	source/ui/BioXAS/BioXASSSRLMonochromatorMaskView.h \
    source/beamline/BioXAS/BioXASFastShutter.h \
	source/beamline/BioXAS/BioXASZebraTimeSeconds.h \
	source/beamline/BioXAS/BioXASZebraSoftInputControl.h \
    source/beamline/BioXAS/BioXASZebraLogicBlock.h \
    source/beamline/BioXAS/BioXASZebraLogicBlockInput.h \
    source/beamline/BioXAS/BioXASZebraInput.h \
    source/ui/BioXAS/BioXASZebraInputView.h \
	source/ui/BioXAS/BioXASZebraLogicBlockView.h \
	source/ui/BioXAS/BioXASZebraLogicBlockInputCheckbox.h \
	source/beamline/BioXAS/BioXASUtilities.h \
	source/ui/BioXAS/BioXASShuttersView.h \
	source/ui/BioXAS/BioXASUtilitiesView.h \
	source/beamline/BioXAS/BioXASUtilitiesGroup.h \
    $$PWD/source/ui/BioXAS/BioXASCarbonFilterFarmView.h \
    $$PWD/source/ui/BioXAS/BioXASCarbonFilterFarmActuatorView.h \
	source/actions3/BioXAS/BioXASSIS3820ScalerDarkCurrentMeasurementAction.h \
	source/ui/BioXAS/BioXASBeamlineConfigurationView.h \
    source/beamline/BioXAS/BioXASSollerSlit.h \
	source/ui/BioXAS/BioXASSollerSlitView.h \
	source/beamline/BioXAS/BioXASBeamlineSupport.h \
	source/beamline/BioXAS/BioXASCryostat.h \
	source/ui/BioXAS/BioXASCryostatView.h \
    source/acquaman/BioXAS/BioXASGenericStepScanConfiguration.h \
	source/acquaman/BioXAS/BioXASGenericStepScanController.h \
	source/beamline/BioXAS/BioXASZebraOutputControl.h \
	source/beamline/BioXAS/BioXASSideZebra.h \
	source/beamline/BioXAS/BioXASMainZebra.h \
	source/ui/BioXAS/BioXASZebraOutputControlView.h \
	source/beamline/BioXAS/BioXASMAXvMotor.h \
	source/actions3/BioXAS/BioXASMAXvMotorMoveToLimitActionInfo.h \
	source/actions3/BioXAS/BioXASMAXvMotorMoveToLimitAction.h \
	source/ui/BioXAS/BioXASGenericStepScanConfigurationView.h

SOURCES += \
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
    source/beamline/BioXAS/BioXASCryostatStage.cpp \
    source/ui/BioXAS/BioXASXASScanConfigurationRegionsEditor.cpp \
    source/ui/BioXAS/BioXASXASScanConfigurationEdgeEditor.cpp \
    source/ui/BioXAS/BioXASXASScanConfigurationEditor.cpp \
    source/ui/BioXAS/BioXASXASScanConfigurationView.cpp \
    source/dataman/BioXAS/BioXASDbUpgrade1Pt1.cpp \
	source/beamline/BioXAS/BioXASFrontEndShutters.cpp \
    source/ui/BioXAS/BioXASValvesView.cpp \
    source/ui/BioXAS/BioXASFrontEndShuttersView.cpp \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorPositionControl.cpp \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuatorFilterControl.cpp \
    source/beamline/BioXAS/BioXASCarbonFilterFarmFilterControl.cpp \
    source/beamline/BioXAS/BioXASCarbonFilterFarmActuator.cpp \
    source/beamline/BioXAS/BioXASXIAFiltersActuatorControl.cpp \
	source/beamline/BioXAS/BioXASXIAFiltersActuator.cpp \
    source/beamline/BioXAS/BioXASZebra.cpp \
	source/beamline/BioXAS/BioXASSIS3820Scaler.cpp \
    source/beamline/BioXAS/BioXASZebraPulseControl.cpp \
    source/ui/BioXAS/BioXASZebraView.cpp \
    source/ui/BioXAS/BioXASZebraPulseControlView.cpp \
	source/beamline/BioXAS/BioXASM1MirrorMask.cpp \
	source/beamline/BioXAS/BioXASFilterFlipper.cpp \
	source/beamline/BioXAS/BioXASFilterFlipperFilter.cpp \
	source/ui/BioXAS/BioXASFilterFlipperView.cpp \
	source/ui/BioXAS/BioXASFilterFlipperFilterView.cpp \
	source/beamline/BioXAS/BioXASFilterFlipperFilters.cpp \
	source/ui/BioXAS/BioXASFilterFlipperConfigurationView.cpp \
	source/ui/BioXAS/BioXASM1MirrorMaskView.cpp \
    source/ui/BioXAS/BioXASSSRLMonochromatorView.cpp \
    source/ui/BioXAS/BioXASSSRLMonochromatorBasicView.cpp \
    source/ui/BioXAS/BioXASSSRLMonochromatorCrystalsView.cpp \
    source/ui/BioXAS/BioXASSSRLMonochromatorEnergyView.cpp \
    source/ui/BioXAS/BioXASSSRLMonochromatorCalibrationView.cpp \
	source/ui/BioXAS/BioXASSSRLMonochromatorMaskView.cpp \
	source/beamline/BioXAS/BioXASFastShutter.cpp \
	source/beamline/BioXAS/BioXASZebraTimeSeconds.cpp \
	source/beamline/BioXAS/BioXASZebraSoftInputControl.cpp \
    source/beamline/BioXAS/BioXASZebraLogicBlock.cpp \
    source/beamline/BioXAS/BioXASZebraLogicBlockInput.cpp \
    source/beamline/BioXAS/BioXASZebraInput.cpp \
    source/ui/BioXAS/BioXASZebraInputView.cpp \
	source/ui/BioXAS/BioXASZebraLogicBlockView.cpp \
	source/ui/BioXAS/BioXASZebraLogicBlockInputCheckbox.cpp \
	source/beamline/BioXAS/BioXASUtilities.cpp \
	source/ui/BioXAS/BioXASShuttersView.cpp \
	source/ui/BioXAS/BioXASUtilitiesView.cpp \
	source/beamline/BioXAS/BioXASUtilitiesGroup.cpp \
    $$PWD/source/ui/BioXAS/BioXASCarbonFilterFarmView.cpp \
    $$PWD/source/ui/BioXAS/BioXASCarbonFilterFarmActuatorView.cpp \
	source/actions3/BioXAS/BioXASSIS3820ScalerDarkCurrentMeasurementAction.cpp \
	source/ui/BioXAS/BioXASBeamlineConfigurationView.cpp \
    source/beamline/BioXAS/BioXASSollerSlit.cpp \
	source/ui/BioXAS/BioXASSollerSlitView.cpp \
	source/beamline/BioXAS/BioXASBeamlineSupport.cpp \
	source/beamline/BioXAS/BioXASCryostat.cpp \
	source/ui/BioXAS/BioXASCryostatView.cpp \
	source/acquaman/BioXAS/BioXASGenericStepScanConfiguration.cpp \
	source/acquaman/BioXAS/BioXASGenericStepScanController.cpp \
	source/beamline/BioXAS/BioXASZebraOutputControl.cpp \
	source/beamline/BioXAS/BioXASSideZebra.cpp \
	source/beamline/BioXAS/BioXASMainZebra.cpp \
	source/ui/BioXAS/BioXASZebraOutputControlView.cpp \
	source/beamline/BioXAS/BioXASMAXvMotor.cpp \
	source/actions3/BioXAS/BioXASMAXvMotorMoveToLimitActionInfo.cpp \
	source/actions3/BioXAS/BioXASMAXvMotorMoveToLimitAction.cpp \
	source/ui/BioXAS/BioXASGenericStepScanConfigurationView.cpp

RESOURCES += \
	source/stylesheets/BioXAS/BioXASStylesheets.qrc
