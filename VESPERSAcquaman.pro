include ( acquamanCommon.pri )

TARGET = VESPERSAcquaman

FORMS +=

HEADERS +=	source/application/VESPERS/VESPERSAppController.h \
	source/beamline/VESPERS/VESPERSBeamline.h \
	source/ui/VESPERS/VESPERSBeamlineView.h \
	source/ui/AMShutterButton.h \
	source/ui/AMStopButton.h \
	source/ui/VESPERS/VESPERSMotorView.h \
	source/dataman/VESPERS/XRFDetectorInfo.h \
	source/beamline/VESPERS/XRFDetector.h \
	source/ui/VESPERS/XRFDetectorView.h \
	source/ui/VESPERS/DeadTimeButton.h \
	source/dataman/VESPERS/AMXRFScan.h \
	source/dataman/VESPERS/VESPERSXRFDataLoader.h \
	source/acquaman/VESPERS/VESPERSXRFScanConfiguration.h \
	source/acquaman/VESPERS/VESPERSXRFScanController.h \
	source/ui/VESPERS/VESPERSXRFScanConfigurationView.h \
	source/ui/VESPERS/VESPERSPersistentView.h \
	source/ui/VESPERS/VESPERSSampleStageView.h \
	source/ui/VESPERS/VESPERSXRFElementView.h \
	source/util/VESPERS/XRFPeriodicTable.h \
	source/ui/VESPERS/XRFPeriodicTableView.h \
	source/beamline/VESPERS/AMValveControl.h \
	source/ui/VESPERS/ROIPlotMarker.h \
	source/util/VESPERS/ROIHelper.h \
	source/ui/VESPERS/CustomizeRegionsOfInterest.h \
	source/beamline/VESPERS/SampleStageControl.h \
	source/beamline/VESPERS/VESPERSValveGroupControl.h \
	source/beamline/VESPERS/PIDLoopControl.h \
	source/ui/VESPERS/PIDLoopControlView.h \
	source/ui/VESPERS/XRFMapSetup.h \
	source/ui/VESPERS/VESPERSDeviceStatusView.h \
	source/util/VESPERS/GeneralUtilities.h \
	source/util/VESPERS/XRFElement.h \
	source/acquaman/VESPERS/VESPERSXASScanConfiguration.h \
	source/acquaman/VESPERS/VESPERSXASDacqScanController.h \
	source/ui/VESPERS/VESPERSXASScanConfigurationView.h \
	source/beamline/VESPERS/VESPERSEndstation.h \
	source/ui/VESPERS/VESPERSEndstationView.h \
	source/acquaman/VESPERS/XRFFreeRun.h \
	source/ui/VESPERS/XRFFreeRunView.h \
	source/util/VESPERS/VortexDetectorStatusHelper.h \
	source/beamline/VESPERS/VESPERSIonChamberCalibration.h \
	source/beamline/VESPERS/VESPERSIonChamber.h \
	source/ui/VESPERS/VESPERSIonChamberView.h \
	source/ui/VESPERS/VESPERSIonChamberCalibrationView.h \
	source/beamline/VESPERS/VESPERSMonochromator.h \
	source/beamline/VESPERS/VESPERSIntermediateSlits.h \
	source/ui/VESPERS/VESPERSIntermediateSlitsView.h \
	source/ui/VESPERS/VESPERSBeamSelectorView.h \
	source/beamline/VESPERS/VESPERSSplitIonChamber.h \
	source/ui/VESPERS/VESPERSDiagnosticsView.h \
    source/ui/VESPERS/VESPERSExperimentConfigurationView.h \
    source/ui/AMPeriodicTableDialog.h

SOURCES +=	source/application/VESPERS/VESPERSAppController.cpp \
	source/application/VESPERS/VESPERSMain.cpp \
	source/beamline/VESPERS/VESPERSBeamline.cpp \
	source/ui/VESPERS/VESPERSBeamlineView.cpp \
	source/ui/AMShutterButton.cpp \
	source/ui/VESPERS/VESPERSMotorView.cpp \
	source/dataman/VESPERS/XRFDetectorInfo.cpp \
	source/beamline/VESPERS/XRFDetector.cpp \
	source/ui/VESPERS/XRFDetectorView.cpp \
	source/ui/VESPERS/DeadTimeButton.cpp \
	source/dataman/VESPERS/AMXRFScan.cpp \
	source/dataman/VESPERS/VESPERSXRFDataLoader.cpp \
	source/acquaman/VESPERS/VESPERSXRFScanConfiguration.cpp \
	source/acquaman/VESPERS/VESPERSXRFScanController.cpp \
	source/ui/VESPERS/VESPERSXRFScanConfigurationView.cpp \
	source/ui/VESPERS/VESPERSPersistentView.cpp \
	source/ui/VESPERS/VESPERSSampleStageView.cpp \
	source/ui/VESPERS/VESPERSXRFElementView.cpp \
	source/util/VESPERS/XRFPeriodicTable.cpp \
	source/ui/VESPERS/XRFPeriodicTableView.cpp \
	source/beamline/VESPERS/AMValveControl.cpp \
	source/ui/VESPERS/ROIPlotMarker.cpp \
	source/util/VESPERS/ROIHelper.cpp \
	source/ui/VESPERS/CustomizeRegionsOfInterest.cpp \
	source/beamline/VESPERS/SampleStageControl.cpp \
	source/beamline/VESPERS/VESPERSValveGroupControl.cpp \
	source/beamline/VESPERS/PIDLoopControl.cpp \
	source/ui/VESPERS/PIDLoopControlView.cpp \
	source/ui/VESPERS/XRFMapSetup.cpp \
	source/ui/VESPERS/VESPERSDeviceStatusView.cpp \
	source/util/VESPERS/GeneralUtilities.cpp \
	source/util/VESPERS/XRFElement.cpp \
	source/acquaman/VESPERS/VESPERSXASScanConfiguration.cpp \
	source/acquaman/VESPERS/VESPERSXASDacqScanController.cpp \
	source/ui/VESPERS/VESPERSXASScanConfigurationView.cpp \
	source/beamline/VESPERS/VESPERSEndstation.cpp \
	source/ui/VESPERS/VESPERSEndstationView.cpp \
	source/acquaman/VESPERS/XRFFreeRun.cpp \
	source/ui/VESPERS/XRFFreeRunView.cpp \
	source/util/VESPERS/VortexDetectorStatusHelper.cpp \
	source/beamline/VESPERS/VESPERSIonChamberCalibration.cpp \
	source/beamline/VESPERS/VESPERSIonChamber.cpp \
	source/ui/VESPERS/VESPERSIonChamberView.cpp \
	source/ui/VESPERS/VESPERSIonChamberCalibrationView.cpp \
	source/beamline/VESPERS/VESPERSMonochromator.cpp \
	source/beamline/VESPERS/VESPERSIntermediateSlits.cpp \
	source/ui/VESPERS/VESPERSIntermediateSlitsView.cpp \
	source/ui/VESPERS/VESPERSBeamSelectorView.cpp \
	source/beamline/VESPERS/VESPERSSplitIonChamber.cpp \
	source/ui/VESPERS/VESPERSDiagnosticsView.cpp \
    source/ui/VESPERS/VESPERSExperimentConfigurationView.cpp \
    source/ui/AMPeriodicTableDialog.cpp

RESOURCES += \
	source/ui/VESPERS/EndstationPictures.qrc \
	source/ui/VESPERS/vespersIcons.qrc \
	source/ui/StopButton.qrc






