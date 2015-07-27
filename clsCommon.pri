
DESTDIR = build

include ( $$PATH_TO_AM/acquamanCommon.pri )

HEADERS *=\
	source/actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementAction.h \
	source/actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementActionInfo.h \
	source/beamline/CLS/CLSFacilityID.h \
	source/beamline/CLS/CLSBeamline.h \
	source/beamline/CLS/CLSMAXvMotor.h \
	source/beamline/CLS/CLSSynchronizedDwellTime.h \
	source/beamline/CLS/CLSBiStateControl.h \
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
	source/dataman/info/CLSPGTDetectorInfo.h \
	source/dataman/info/CLSOceanOptics65000DetectorInfo.h \
	source/dataman/info/CLSAmptekSDD123DetectorInfo.h \
	source/dataman/info/CLSSynchronizedDwellTimeConfigurationInfo.h \
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
	source/beamline/CLS/CLSJJSlitGapControl.h \
	source/beamline/CLS/CLSJJSlitCenterControl.h \
	source/beamline/CLS/CLSJJSlits.h \
	source/ui/CLS/CLSJJSlitsView.h \
	source/beamline/CLS/CLSJJSlitBladesControl.h \
	source/beamline/CLS/CLSStandardsWheel.h \
	source/ui/CLS/CLSStandardsWheelConfigurationView.h \
    source/beamline/CLS/CLSSIS3820ScalerModeControl.h \
    source/acquaman/CLS/CLSJJSlitsCenterOptimizationScanConfiguration.h \
    source/acquaman/CLS/CLSJJSlitsCenterOptimizationScanController.h

SOURCES *= \
	source/actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementAction.cpp \
	source/actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementActionInfo.cpp \
	source/beamline/CLS/CLSBeamline.cpp \
	source/beamline/CLS/CLSMAXvMotor.cpp \
	source/beamline/CLS/CLSSynchronizedDwellTime.cpp \
	source/beamline/CLS/CLSBiStateControl.cpp \
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
	source/dataman/info/CLSPGTDetectorInfo.cpp \
	source/dataman/info/CLSOceanOptics65000DetectorInfo.cpp \
	source/dataman/info/CLSAmptekSDD123DetectorInfo.cpp \
	source/dataman/info/CLSSynchronizedDwellTimeConfigurationInfo.cpp \
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
	source/beamline/CLS/CLSJJSlitGapControl.cpp \
	source/beamline/CLS/CLSJJSlitCenterControl.cpp \
	source/beamline/CLS/CLSJJSlits.cpp \
	source/ui/CLS/CLSJJSlitsView.cpp \
	source/beamline/CLS/CLSJJSlitBladesControl.cpp \
	source/beamline/CLS/CLSStandardsWheel.cpp \
	source/ui/CLS/CLSStandardsWheelConfigurationView.cpp \
    source/beamline/CLS/CLSSIS3820ScalerModeControl.cpp \
    source/acquaman/CLS/CLSJJSlitsCenterOptimizationScanConfiguration.cpp \
    source/acquaman/CLS/CLSJJSlitsCenterOptimizationScanController.cpp






