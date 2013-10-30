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
	source/ui/VESPERS/VESPERSXRFElementView.h \
	source/util/VESPERS/XRFPeriodicTable.h \
	source/ui/VESPERS/XRFPeriodicTableView.h \
	source/util/VESPERS/ROIHelper.h \
	source/ui/VESPERS/VESPERSCustomizeRegionsOfInterest.h \
	source/beamline/VESPERS/VESPERSPIDLoopControl.h \
	source/ui/VESPERS/VESPERSPIDLoopControlView.h \
	source/ui/VESPERS/VESPERSDeviceStatusView.h \
	source/util/VESPERS/XRFElement.h \
	source/beamline/VESPERS/VESPERSEndstation.h \
	source/ui/VESPERS/VESPERSEndstationView.h \
	source/acquaman/VESPERS/XRFFreeRun.h \
	source/ui/VESPERS/VESPERSXRFFreeRunView.h \
	source/beamline/VESPERS/VESPERSMonochromator.h \
	source/beamline/VESPERS/VESPERSIntermediateSlits.h \
	source/ui/VESPERS/VESPERSBeamSelectorView.h \
	source/ui/VESPERS/VESPERSDiagnosticsView.h \
	source/acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h \
	source/acquaman/VESPERS/VESPERSEXAFSDacqScanController.h \
	source/ui/VESPERS/VESPERSEXAFSScanConfigurationView.h \
	source/beamline/VESPERS/VESPERSRoperCCDDetector.h \
	source/ui/VESPERS/VESPERSCCDDetectorView.h \
	source/acquaman/VESPERS/VESPERS2DDacqScanController.h \
	source/acquaman/VESPERS/VESPERS2DScanConfiguration.h \
	source/ui/VESPERS/VESPERS2DScanConfigurationView.h \
	source/dataman/export/VESPERS/VESPERSExporter2DAscii.h \
	source/dataman/export/VESPERS/VESPERSExporterSMAK.h \
	source/ui/VESPERS/VESPERSSpatialLineScanConfigurationView.h \
	source/acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.h \
	source/acquaman/VESPERS/VESPERSSpatialLineDacqScanController.h \
	source/dataman/export/VESPERS/VESPERSExporterLineScanAscii.h \
	source/acquaman/VESPERS/VESPERSEnergyDacqScanController.h \
	source/ui/VESPERS/VESPERSEnergyScanConfigurationView.h \
	source/acquaman/VESPERS/VESPERSEnergyScanConfiguration.h \
	source/util/VESPERS/VESPERSConfigurationFileBuilder.h \
	source/dataman/VESPERS/VESPERSDbUpgrade1Pt1.h \
	source/beamline/VESPERS/VESPERSMarCCDDetector.h \
	source/application/VESPERS/VESPERS.h \
	source/dataman/VESPERS/VESPERSDbUpgrade1Pt2.h \
	source/dataman/VESPERS/VESPERSDbUpgrade1Pt3.h \
	source/dataman/VESPERS/VESPERSDbUpgrade1Pt4.h \
	source/acquaman/VESPERS/VESPERSScanConfigurationDbObject.h \
	source/acquaman/VESPERS/VESPERSScanConfiguration.h \
	source/dataman/VESPERS/VESPERSDbUpgrade1Pt5.h \
	source/ui/VESPERS/VESPERSScanConfigurationView.h \
	source/acquaman/VESPERS/VESPERSScanController.h \
	source/ui/acquaman/VESPERS/VESPERSScanConfigurationViewHolder3.h \
	source/ui/VESPERS/VESPERSChooseDataFolderDialog.h \
	source/actions/VESPERS/VESPERSBeamlineSetStringAction.h \
	source/dataman/VESPERS/VESPERSCCDDetectorInfo.h \
	source/beamline/VESPERS/VESPERSCCDDetector.h \
	source/acquaman/VESPERS/VESPERS3DScanConfiguration.h \
	source/ui/VESPERS/VESPERS3DScanConfigurationView.h \
	source/acquaman/VESPERS/VESPERS3DDacqScanController.h \
	source/beamline/VESPERS/VESPERSPilatusCCDDetector.h \
	source/ui/VESPERS/VESPERSPilatusCCDDetectorView.h \
	source/util/VESPERS/VESPERSAttoCubeHack.h \
	source/dataman/export/VESPERS/VESPERSExporter3DAscii.h \
	source/beamline/VESPERS/VESPERSSingleElementVortexDetector.h \
	source/util/VESPERS/GeneralUtilities.h \
	source/beamline/VESPERS/VESPERSFourElementVortexDetector.h \
	source/acquaman/VESPERS/VESPERSXASScanActionController.h \
	source/acquaman/SGM/SGMXASScanActionControllerFileWriter.h

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
	source/ui/VESPERS/VESPERSXRFElementView.cpp \
	source/util/VESPERS/XRFPeriodicTable.cpp \
	source/ui/VESPERS/XRFPeriodicTableView.cpp \
	source/util/VESPERS/ROIHelper.cpp \
	source/ui/VESPERS/VESPERSCustomizeRegionsOfInterest.cpp \
	source/beamline/VESPERS/VESPERSPIDLoopControl.cpp \
	source/ui/VESPERS/VESPERSPIDLoopControlView.cpp \
	source/ui/VESPERS/VESPERSDeviceStatusView.cpp \
	source/util/VESPERS/XRFElement.cpp \
	source/beamline/VESPERS/VESPERSEndstation.cpp \
	source/ui/VESPERS/VESPERSEndstationView.cpp \
	source/acquaman/VESPERS/XRFFreeRun.cpp \
	source/ui/VESPERS/VESPERSXRFFreeRunView.cpp \
	source/beamline/VESPERS/VESPERSMonochromator.cpp \
	source/beamline/VESPERS/VESPERSIntermediateSlits.cpp \
	source/ui/VESPERS/VESPERSBeamSelectorView.cpp \
	source/ui/VESPERS/VESPERSDiagnosticsView.cpp \
	source/acquaman/VESPERS/VESPERSEXAFSScanConfiguration.cpp \
	source/acquaman/VESPERS/VESPERSEXAFSDacqScanController.cpp \
	source/ui/VESPERS/VESPERSEXAFSScanConfigurationView.cpp \
	source/beamline/VESPERS/VESPERSRoperCCDDetector.cpp \
	source/ui/VESPERS/VESPERSCCDDetectorView.cpp \
	source/acquaman/VESPERS/VESPERS2DDacqScanController.cpp \
	source/acquaman/VESPERS/VESPERS2DScanConfiguration.cpp \
	source/ui/VESPERS/VESPERS2DScanConfigurationView.cpp \
	source/dataman/export/VESPERS/VESPERSExporter2DAscii.cpp \
	source/dataman/export/VESPERS/VESPERSExporterSMAK.cpp \
	source/ui/VESPERS/VESPERSSpatialLineScanConfigurationView.cpp \
	source/acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.cpp \
	source/acquaman/VESPERS/VESPERSSpatialLineDacqScanController.cpp \
	source/dataman/export/VESPERS/VESPERSExporterLineScanAscii.cpp \
	source/acquaman/VESPERS/VESPERSEnergyDacqScanController.cpp \
	source/ui/VESPERS/VESPERSEnergyScanConfigurationView.cpp \
	source/acquaman/VESPERS/VESPERSEnergyScanConfiguration.cpp \
	source/util/VESPERS/VESPERSConfigurationFileBuilder.cpp \
	source/dataman/VESPERS/VESPERSDbUpgrade1Pt1.cpp \
	source/beamline/VESPERS/VESPERSMarCCDDetector.cpp \
	source/dataman/VESPERS/VESPERSDbUpgrade1Pt2.cpp \
	source/dataman/VESPERS/VESPERSDbUpgrade1Pt3.cpp \
	source/dataman/VESPERS/VESPERSDbUpgrade1Pt4.cpp \
	source/acquaman/VESPERS/VESPERSScanConfigurationDbObject.cpp \
	source/acquaman/VESPERS/VESPERSScanConfiguration.cpp \
	source/dataman/VESPERS/VESPERSDbUpgrade1Pt5.cpp \
	source/ui/VESPERS/VESPERSScanConfigurationView.cpp \
	source/acquaman/VESPERS/VESPERSScanController.cpp \
	source/ui/acquaman/VESPERS/VESPERSScanConfigurationViewHolder3.cpp \
	source/ui/VESPERS/VESPERSChooseDataFolderDialog.cpp \
	source/actions/VESPERS/VESPERSBeamlineSetStringAction.cpp \
	source/dataman/VESPERS/VESPERSCCDDetectorInfo.cpp \
	source/beamline/VESPERS/VESPERSCCDDetector.cpp \
	source/acquaman/VESPERS/VESPERS3DScanConfiguration.cpp \
	source/ui/VESPERS/VESPERS3DScanConfigurationView.cpp \
	source/acquaman/VESPERS/VESPERS3DDacqScanController.cpp \
	source/beamline/VESPERS/VESPERSPilatusCCDDetector.cpp \
	source/ui/VESPERS/VESPERSPilatusCCDDetectorView.cpp \
	source/util/VESPERS/VESPERSAttoCubeHack.cpp \
	source/dataman/export/VESPERS/VESPERSExporter3DAscii.cpp \
	source/beamline/VESPERS/VESPERSSingleElementVortexDetector.cpp \
	source/util/VESPERS/GeneralUtilities.cpp \
	source/beamline/VESPERS/VESPERSFourElementVortexDetector.cpp \
	source/acquaman/VESPERS/VESPERSXASScanActionController.cpp \
	source/acquaman/SGM/SGMXASScanActionControllerFileWriter.cpp

RESOURCES += \
	source/icons/VESPERS/vespersIcons.qrc

















































