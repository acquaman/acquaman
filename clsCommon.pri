
DESTDIR = build

include ( $$PATH_TO_AM/acquamanCommon.pri )

HEADERS *=\
	source/actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementAction.h \
	source/actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementActionInfo.h \
	source/application/CLS/CLSAppController.h \
	source/beamline/CLS/CLSBeamline.h \
	source/beamline/CLS/CLSBeamlineStatus.h \
	source/beamline/CLS/CLSBiStateGroup.h \
	source/beamline/CLS/CLSTriStateGroup.h \
	source/beamline/CLS/CLSValves.h \
	source/beamline/CLS/CLSShutters.h \
	source/beamline/CLS/CLSMirrorMaskState.h \
	source/beamline/CLS/CLSMAXvMotor.h \
	source/beamline/CLS/CLSSynchronizedDwellTime.h \
	source/beamline/CLS/CLSMDriveMotorControl.h \
	source/beamline/CLS/CLSVariableIntegrationTime.h \
	source/beamline/CLS/CLSOMS58Motor.h \
	source/beamline/CLS/CLSSIS3820Scaler.h \
	source/beamline/CLS/CLSAmptekSDD123DetectorNew.h \
	source/beamline/CLS/CLSBasicScalerChannelDetector.h \
	source/beamline/CLS/CLSPGTDetectorV2.h \
	source/beamline/CLS/CLSQE65000Detector.h \
	source/beamline/CLS/CLSAdvancedScalerChannelDetector.h \
	source/beamline/CLS/CLSSynchronizedDwellTimeConfiguration.h \
	source/beamline/CLS/CLSPseudoMotorGroup.h \
	source/beamline/CLS/CLSPseudoMotorControl.h \
	source/beamline/CLS/CLSBasicCompositeScalerChannelDetector.h \
	source/beamline/CLS/CLSKeithley428.h \
	source/beamline/CLS/CLSStorageRing.h \
	source/beamline/CLS/CLSSR570.h \
	source/beamline/CLS/CLSCrossHairGeneratorControl.h \
	source/beamline/CLS/CLSHVControl.h \
	source/dataman/info/CLSPGTDetectorInfo.h \
	source/dataman/info/CLSOceanOptics65000DetectorInfo.h \
	source/dataman/info/CLSAmptekSDD123DetectorInfo.h \
	source/dataman/info/CLSSynchronizedDwellTimeConfigurationInfo.h \
	source/dataman/CLS/CLSUserConfiguration.h \
	source/dataman/CLS/CLSDbUpgrade1Pt1.h \
	source/dataman/CLS/CLSDbUpgrade1Pt3.h \
	source/ui/CLS/CLSValueProgressLabel.h \
	source/ui/CLS/CLSValueEditor.h \
	source/ui/CLS/CLSControlEditor.h \
	source/ui/CLS/CLSValueSetpointEditor.h \
	source/ui/CLS/CLSValueSetpointEditorDialog.h \
	source/ui/CLS/CLSControlButton.h \
	source/ui/CLS/CLSButtonBar.h \
	source/ui/CLS/CLSControlButtonBar.h \
	source/ui/CLS/CLSBeamlineStatusButtonBar.h \
	source/ui/CLS/CLSBeamlineStatusView.h \
	source/ui/CLS/CLSSynchronizedDwellTimeView.h \
	source/ui/CLS/CLSPseudoMotorGroupView.h \
	source/ui/CLS/CLSPGTDetectorV2View.h \
	source/ui/CLS/CLSAmptekSDD123DetectorNewView.h \
	source/ui/CLS/CLSAmptekSDD123DetailedDetectorView.h \
	source/ui/CLS/CLSSIS3820ScalerView.h \
	source/ui/CLS/CLSSIS3820CompositeScalerChannelView.h \
	source/ui/CLS/CLSSIS3820ScalerDarkCurrentWidget.h \
	source/ui/CLS/CLSSIS3820ScalerControlsView.h \
	source/ui/CLS/CLSCrossHairGeneratorControlView.h \
	source/ui/CLS/CLSHVControlGroupView.h \
	source/beamline/CLS/CLSStandardsWheel.h \
	source/ui/CLS/CLSStandardsWheelConfigurationView.h \
    source/beamline/CLS/CLSAMDSScaler.h \
    source/ui/CLS/CLSAMDSScalerView.h \
    source/beamline/CLS/CLSAMDSScalerChannelDetector.h  \
    source/ui/CLS/CLSMAXvMotorConfigurationView.h \
	source/beamline/CLS/CLSSIS3820ScalerAcquisitionMode.h \
	source/beamline/CLS/CLSExclusiveStatesControl.h \
    source/actions3/CLS/CLSSIS3820ScalerTriggerActionInfo.h \
	source/actions3/CLS/CLSSIS3820ScalerTriggerAction.h \
	source/beamline/CLS/CLSTemperatureMonitor.h \
	source/beamline/CLS/CLSPressureMonitor.h \
	source/beamline/CLS/CLSFlowTransducer.h \
	source/beamline/CLS/CLSIonPump.h \
	source/beamline/CLS/CLSFlowSwitch.h \
	source/beamline/CLS/CLSBeamlineControl.h

SOURCES *= \
	source/actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementAction.cpp \
	source/actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementActionInfo.cpp \
	source/application/CLS/CLSAppController.cpp \
	source/beamline/CLS/CLSBeamline.cpp \
	source/beamline/CLS/CLSBeamlineStatus.cpp \
	source/beamline/CLS/CLSBiStateGroup.cpp \
	source/beamline/CLS/CLSTriStateGroup.cpp \
	source/beamline/CLS/CLSValves.cpp \
	source/beamline/CLS/CLSShutters.cpp \
	source/beamline/CLS/CLSMirrorMaskState.cpp \
	source/beamline/CLS/CLSMAXvMotor.cpp \
	source/beamline/CLS/CLSSynchronizedDwellTime.cpp \
	source/beamline/CLS/CLSMDriveMotorControl.cpp \
	source/beamline/CLS/CLSVariableIntegrationTime.cpp \
	source/beamline/CLS/CLSSIS3820Scaler.cpp \
	source/beamline/CLS/CLSAmptekSDD123DetectorNew.cpp \
	source/beamline/CLS/CLSBasicScalerChannelDetector.cpp \
	source/beamline/CLS/CLSPGTDetectorV2.cpp \
	source/beamline/CLS/CLSQE65000Detector.cpp \
	source/beamline/CLS/CLSAdvancedScalerChannelDetector.cpp \
	source/beamline/CLS/CLSSynchronizedDwellTimeConfiguration.cpp \
	source/beamline/CLS/CLSPseudoMotorGroup.cpp \
	source/beamline/CLS/CLSPseudoMotorControl.cpp \
	source/beamline/CLS/CLSBasicCompositeScalerChannelDetector.cpp \
	source/beamline/CLS/CLSKeithley428.cpp \
	source/beamline/CLS/CLSStorageRing.cpp \
	source/beamline/CLS/CLSSR570.cpp \
	source/beamline/CLS/CLSCrossHairGeneratorControl.cpp \
	source/beamline/CLS/CLSHVControl.cpp \
	source/dataman/info/CLSPGTDetectorInfo.cpp \
	source/dataman/info/CLSOceanOptics65000DetectorInfo.cpp \
	source/dataman/info/CLSAmptekSDD123DetectorInfo.cpp \
	source/dataman/info/CLSSynchronizedDwellTimeConfigurationInfo.cpp \
	source/dataman/CLS/CLSUserConfiguration.cpp \
	source/dataman/CLS/CLSDbUpgrade1Pt1.cpp \
	source/dataman/CLS/CLSDbUpgrade1Pt3.cpp \
	source/ui/CLS/CLSValueProgressLabel.cpp \
	source/ui/CLS/CLSValueEditor.cpp \
	source/ui/CLS/CLSControlEditor.cpp \
	source/ui/CLS/CLSValueSetpointEditor.cpp \
	source/ui/CLS/CLSValueSetpointEditorDialog.cpp \
	source/ui/CLS/CLSControlButton.cpp \
	source/ui/CLS/CLSButtonBar.cpp \
	source/ui/CLS/CLSControlButtonBar.cpp \
	source/ui/CLS/CLSBeamlineStatusButtonBar.cpp \
	source/ui/CLS/CLSBeamlineStatusView.cpp \
	source/ui/CLS/CLSSynchronizedDwellTimeView.cpp \
	source/ui/CLS/CLSPseudoMotorGroupView.cpp \
	source/ui/CLS/CLSPGTDetectorV2View.cpp \
	source/ui/CLS/CLSAmptekSDD123DetectorNewView.cpp \
	source/ui/CLS/CLSAmptekSDD123DetailedDetectorView.cpp \
	source/ui/CLS/CLSSIS3820ScalerView.cpp \
	source/ui/CLS/CLSSIS3820CompositeScalerChannelView.cpp \
	source/ui/CLS/CLSSIS3820ScalerDarkCurrentWidget.cpp \
	source/ui/CLS/CLSSIS3820ScalerControlsView.cpp \
	source/ui/CLS/CLSCrossHairGeneratorControlView.cpp \
	source/ui/CLS/CLSHVControlGroupView.cpp \
	source/beamline/CLS/CLSStandardsWheel.cpp \
	source/ui/CLS/CLSStandardsWheelConfigurationView.cpp \
    source/beamline/CLS/CLSAMDSScaler.cpp \
    source/ui/CLS/CLSAMDSScalerView.cpp \
    source/beamline/CLS/CLSAMDSScalerChannelDetector.cpp \
    source/ui/CLS/CLSMAXvMotorConfigurationView.cpp \
	source/beamline/CLS/CLSSIS3820ScalerAcquisitionMode.cpp \
	source/beamline/CLS/CLSExclusiveStatesControl.cpp \
    source/actions3/CLS/CLSSIS3820ScalerTriggerActionInfo.cpp \
	source/actions3/CLS/CLSSIS3820ScalerTriggerAction.cpp \
	source/beamline/CLS/CLSTemperatureMonitor.cpp \
	source/beamline/CLS/CLSPressureMonitor.cpp \
	source/beamline/CLS/CLSFlowTransducer.cpp \
	source/beamline/CLS/CLSIonPump.cpp \
	source/beamline/CLS/CLSFlowSwitch.cpp \
	source/beamline/CLS/CLSBeamlineControl.cpp

RESOURCES *= \
	source/stylesheets/CLS/CLSStylesheets.qrc

OTHER_FILES *= \
	source/stylesheets/CLS/CLSValueSetpointEditor.qss






