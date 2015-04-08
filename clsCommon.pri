
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
	source/beamline/CLS/CLSJJSlit.h \
	source/dataman/info/CLSPGTDetectorInfo.h \
	source/dataman/info/CLSOceanOptics65000DetectorInfo.h \
	source/dataman/info/CLSAmptekSDD123DetectorInfo.h \
	source/dataman/info/CLSSynchronizedDwellTimeConfigurationInfo.h \
	source/dataman/SGM/SGM2004FileLoader.h \
	source/dataman/SGM/SGM2010FastFileLoader.h \
	source/dataman/SGM/SGMMCPDetectorInfo.h \
	source/dataman/SGM/SGM2011XASFileLoader.h \
	source/dataman/export/SGM/SGMAxis2000Exporter.h \
	source/dataman/REIXS/REIXSXESCalibration2.h \
	source/dataman/VESPERS/VESPERSXASDataLoader.h \
	source/ui/CLS/CLSSynchronizedDwellTimeView.h \
	source/ui/CLS/CLSPseudoMotorGroupView.h \
	source/ui/CLS/CLSPGTDetectorV2View.h \
	source/ui/CLS/CLSAmptekSDD123DetectorNewView.h \
	source/ui/CLS/CLSAmptekSDD123DetailedDetectorView.h \
	source/ui/CLS/CLSSIS3820ScalerView.h \
	source/ui/CLS/CLSSIS3820CompositeScalerChannelView.h \
	source/ui/CLS/CLSSIS3820ScalerChannelViewWithDarkCurrent.h \
	source/ui/CLS/CLSSIS3820ScalerDarkCurrentWidget.h \
    source/ui/CLS/CLSJJSlitView.h

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
	source/beamline/CLS/CLSJJSlit.cpp \
	source/dataman/info/CLSPGTDetectorInfo.cpp \
	source/dataman/info/CLSOceanOptics65000DetectorInfo.cpp \
	source/dataman/info/CLSAmptekSDD123DetectorInfo.cpp \
	source/dataman/info/CLSSynchronizedDwellTimeConfigurationInfo.cpp \
	source/dataman/SGM/SGM2004FileLoader.cpp \
	source/dataman/SGM/SGM2010FastFileLoader.cpp \
	source/dataman/SGM/SGM2011XASFileLoader.cpp \
	source/dataman/SGM/SGMMCPDetectorInfo.cpp \
	source/dataman/export/SGM/SGMAxis2000Exporter.cpp \
	source/dataman/REIXS/REIXSXESRawFileLoader.cpp \
	source/dataman/REIXS/REIXSXESCalibration2.cpp \
	source/dataman/VESPERS/VESPERSXASDataLoader.cpp \
	source/ui/CLS/CLSSynchronizedDwellTimeView.cpp \
	source/ui/CLS/CLSPseudoMotorGroupView.cpp \
	source/ui/CLS/CLSPGTDetectorV2View.cpp \
	source/ui/CLS/CLSAmptekSDD123DetectorNewView.cpp \
	source/ui/CLS/CLSAmptekSDD123DetailedDetectorView.cpp \
	source/ui/CLS/CLSSIS3820ScalerView.cpp \
	source/ui/CLS/CLSSIS3820CompositeScalerChannelView.cpp \
	source/ui/CLS/CLSSIS3820ScalerChannelViewWithDarkCurrent.cpp \
	source/ui/CLS/CLSSIS3820ScalerDarkCurrentWidget.cpp \
    source/ui/CLS/CLSJJSlitView.cpp


