include ( acquamanCommon.pri )

TARGET = VESPERSAcquaman

FORMS +=

HEADERS +=	source/application/VESPERS/VESPERSAppController.h \
	source/beamline/VESPERS/VESPERSBeamline.h \
	source/ui/VESPERS/VESPERSMotorView.h \
	source/dataman/VESPERS/XRFDetectorInfo.h \
	source/beamline/VESPERS/XRFDetector.h \
	source/ui/VESPERS/XRFDetectorView.h \
	source/ui/VESPERS/VESPERSDeadTimeButton.h \
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
	source/util/VESPERS/ROIHelper.h \
	source/ui/VESPERS/VESPERSCustomizeRegionsOfInterest.h \
	source/beamline/VESPERS/VESPERSSampleStageControl.h \
	source/beamline/VESPERS/VESPERSPIDLoopControl.h \
	source/ui/VESPERS/VESPERSPIDLoopControlView.h \
	source/ui/VESPERS/VESPERSMapSetupView.h \
	source/ui/VESPERS/VESPERSDeviceStatusView.h \
	source/util/VESPERS/GeneralUtilities.h \
	source/util/VESPERS/XRFElement.h \
	source/beamline/VESPERS/VESPERSEndstation.h \
	source/ui/VESPERS/VESPERSEndstationView.h \
	source/acquaman/VESPERS/XRFFreeRun.h \
	source/ui/VESPERS/VESPERSXRFFreeRunView.h \
	source/util/VESPERS/VortexDetectorStatusHelper.h \
	source/beamline/VESPERS/VESPERSMonochromator.h \
	source/beamline/VESPERS/VESPERSIntermediateSlits.h \
	source/ui/VESPERS/VESPERSBeamSelectorView.h \
	source/ui/VESPERS/VESPERSDiagnosticsView.h \
	source/ui/VESPERS/VESPERSExperimentConfigurationView.h \
	source/ui/util/AMPeriodicTableDialog.h \
	source/beamline/VESPERS/VESPERSExperimentConfiguration.h \
	source/util/VESPERS/VESPERSMapSetup.h \
	source/acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h \
	source/acquaman/VESPERS/VESPERSEXAFSDacqScanController.h \
	source/ui/VESPERS/VESPERSEXAFSScanConfigurationView.h \
    source/beamline/VESPERS/VESPERSRoperCCDDetector.h \
    source/dataman/VESPERS/VESPERSRoperCCDDetectorInfo.h \
    source/ui/VESPERS/VESPERSRoperCCDDetectorView.h \
    source/util/VESPERS/VESPERSWorkflowAssistant.h \
    source/ui/VESPERS/VESPERSWorkflowAssistantView.h \
    source/acquaman/VESPERS/VESPERS2DDacqScanController.h \
    source/acquaman/VESPERS/VESPERS2DScanConfiguration.h \
    source/ui/VESPERS/VESPERS2DScanConfigurationView.h \
    source/dataman/export/VESPERS/VESPERSExporter2DAscii.h \
    source/beamline/VESPERS/VESPERSEndstationConfiguration.h \
    source/ui/VESPERS/VESPERSEndstationConfigurationView.h \
    source/dataman/export/VESPERS/VESPERSExporterSMAK.h \
    source/ui/VESPERS/VESPERSSpatialLineScanConfigurationView.h \
    source/acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.h \
    source/acquaman/VESPERS/VESPERSSpatialLineDacqScanController.h \
    source/dataman/export/VESPERS/VESPERSExporterLineScanAscii.h \
    source/acquaman/VESPERS/VESPERSEnergyDacqScanController.h \
    source/ui/VESPERS/VESPERSEnergyScanConfigurationView.h \
    source/acquaman/VESPERS/VESPERSEnergyScanConfiguration.h

SOURCES +=	source/application/VESPERS/VESPERSAppController.cpp \
	source/application/VESPERS/VESPERSMain.cpp \
	source/beamline/VESPERS/VESPERSBeamline.cpp \
	source/ui/VESPERS/VESPERSMotorView.cpp \
	source/dataman/VESPERS/XRFDetectorInfo.cpp \
	source/beamline/VESPERS/XRFDetector.cpp \
	source/ui/VESPERS/XRFDetectorView.cpp \
	source/ui/VESPERS/VESPERSDeadTimeButton.cpp \
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
	source/util/VESPERS/ROIHelper.cpp \
	source/ui/VESPERS/VESPERSCustomizeRegionsOfInterest.cpp \
	source/beamline/VESPERS/VESPERSSampleStageControl.cpp \
	source/beamline/VESPERS/VESPERSPIDLoopControl.cpp \
	source/ui/VESPERS/VESPERSPIDLoopControlView.cpp \
	source/ui/VESPERS/VESPERSMapSetupView.cpp \
	source/ui/VESPERS/VESPERSDeviceStatusView.cpp \
	source/util/VESPERS/GeneralUtilities.cpp \
	source/util/VESPERS/XRFElement.cpp \
	source/beamline/VESPERS/VESPERSEndstation.cpp \
	source/ui/VESPERS/VESPERSEndstationView.cpp \
	source/acquaman/VESPERS/XRFFreeRun.cpp \
	source/ui/VESPERS/VESPERSXRFFreeRunView.cpp \
	source/util/VESPERS/VortexDetectorStatusHelper.cpp \
	source/beamline/VESPERS/VESPERSMonochromator.cpp \
	source/beamline/VESPERS/VESPERSIntermediateSlits.cpp \
	source/ui/VESPERS/VESPERSBeamSelectorView.cpp \
	source/ui/VESPERS/VESPERSDiagnosticsView.cpp \
	source/ui/VESPERS/VESPERSExperimentConfigurationView.cpp \
	source/ui/util/AMPeriodicTableDialog.cpp \
	source/beamline/VESPERS/VESPERSExperimentConfiguration.cpp \
	source/util/VESPERS/VESPERSMapSetup.cpp \
	source/acquaman/VESPERS/VESPERSEXAFSScanConfiguration.cpp \
	source/acquaman/VESPERS/VESPERSEXAFSDacqScanController.cpp \
	source/ui/VESPERS/VESPERSEXAFSScanConfigurationView.cpp \
    source/beamline/VESPERS/VESPERSRoperCCDDetector.cpp \
    source/dataman/VESPERS/VESPERSRoperCCDDetectorInfo.cpp \
    source/ui/VESPERS/VESPERSRoperCCDDetectorView.cpp \
    source/util/VESPERS/VESPERSWorkflowAssistant.cpp \
    source/ui/VESPERS/VESPERSWorkflowAssistantView.cpp \
    source/acquaman/VESPERS/VESPERS2DDacqScanController.cpp \
    source/acquaman/VESPERS/VESPERS2DScanConfiguration.cpp \
    source/ui/VESPERS/VESPERS2DScanConfigurationView.cpp \
    source/dataman/export/VESPERS/VESPERSExporter2DAscii.cpp \
    source/beamline/VESPERS/VESPERSEndstationConfiguration.cpp \
    source/ui/VESPERS/VESPERSEndstationConfigurationView.cpp \
    source/dataman/export/VESPERS/VESPERSExporterSMAK.cpp \
    source/ui/VESPERS/VESPERSSpatialLineScanConfigurationView.cpp \
    source/acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.cpp \
    source/acquaman/VESPERS/VESPERSSpatialLineDacqScanController.cpp \
    source/dataman/export/VESPERS/VESPERSExporterLineScanAscii.cpp \
    source/acquaman/VESPERS/VESPERSEnergyDacqScanController.cpp \
    source/ui/VESPERS/VESPERSEnergyScanConfigurationView.cpp \
    source/acquaman/VESPERS/VESPERSEnergyScanConfiguration.cpp

RESOURCES += \
	source/icons/VESPERS/vespersIcons.qrc










































