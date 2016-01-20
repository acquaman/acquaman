include ( clsCommon.pri )

TARGET = VESPERSAcquaman

FORMS +=

HEADERS +=	source/application/VESPERS/VESPERSAppController.h \
	source/beamline/VESPERS/VESPERSBeamline.h \
	source/ui/VESPERS/VESPERSMotorView.h \
	source/dataman/VESPERS/VESPERSXRFDataLoader.h \
	source/acquaman/VESPERS/VESPERSXRFScanConfiguration.h \
	source/acquaman/VESPERS/VESPERSXRFScanController.h \
	source/ui/VESPERS/VESPERSXRFScanConfigurationView.h \
	source/ui/VESPERS/VESPERSPersistentView.h \
	source/beamline/VESPERS/VESPERSPIDLoopControl.h \
	source/ui/VESPERS/VESPERSPIDLoopControlView.h \
	source/ui/VESPERS/VESPERSDeviceStatusView.h \
	source/beamline/VESPERS/VESPERSEndstation.h \
	source/ui/VESPERS/VESPERSEndstationView.h \
	source/beamline/VESPERS/VESPERSMonochromator.h \
	source/beamline/VESPERS/VESPERSIntermediateSlits.h \
	source/ui/VESPERS/VESPERSBeamSelectorView.h \
	source/ui/VESPERS/VESPERSDiagnosticsView.h \
	source/acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h \
	source/ui/VESPERS/VESPERSEXAFSScanConfigurationView.h \
	source/beamline/VESPERS/VESPERSRoperCCDDetector.h \
	source/ui/VESPERS/VESPERSCCDDetectorView.h \
	source/acquaman/VESPERS/VESPERS2DScanConfiguration.h \
	source/ui/VESPERS/VESPERS2DScanConfigurationView.h \
	source/dataman/export/VESPERS/VESPERSExporter2DAscii.h \
	source/dataman/export/VESPERS/VESPERSExporterSMAK.h \
	source/ui/VESPERS/VESPERSSpatialLineScanConfigurationView.h \
	source/acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.h \
	source/dataman/export/VESPERS/VESPERSExporterLineScanAscii.h \
	source/ui/VESPERS/VESPERSEnergyScanConfigurationView.h \
	source/acquaman/VESPERS/VESPERSEnergyScanConfiguration.h \
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
	source/beamline/VESPERS/VESPERSCCDDetector.h \
	source/acquaman/VESPERS/VESPERS3DScanConfiguration.h \
	source/ui/VESPERS/VESPERS3DScanConfigurationView.h \
	source/beamline/VESPERS/VESPERSPilatusCCDDetector.h \
	source/ui/VESPERS/VESPERSPilatusCCDDetectorView.h \
	source/dataman/export/VESPERS/VESPERSExporter3DAscii.h \
	source/beamline/VESPERS/VESPERSSingleElementVortexDetector.h \
	source/beamline/VESPERS/VESPERSFourElementVortexDetector.h \
	source/acquaman/VESPERS/VESPERSXASScanActionController.h \
	source/acquaman/VESPERS/VESPERSSpatialLineScanActionController.h \
	source/acquaman/VESPERS/VESPERSEnergyScanActionController.h \
	source/actions3/VESPERS/VESPERSSetStringAction.h \
	source/actions3/VESPERS/VESPERSSetStringActionInfo.h \
	source/ui/VESPERS/VESPERSScalerView.h \
	source/beamline/VESPERS/VESPERSMonochomatorControl.h \
	source/dataman/VESPERS/VESPERSUserConfiguration.h \
	source/beamline/VESPERS/VESPERSCCDBasicDetectorEmulator.h \
	source/acquaman/VESPERS/VESPERS2DScanActionController.h \
	source/acquaman/VESPERS/VESPERSTimeScanConfiguration.h \
	source/acquaman/VESPERS/VESPERSTimeScanActionController.h \
	source/ui/VESPERS/VESPERSTimeScanConfigurationView.h \
	source/dataman/VESPERS/VESPERSDbUpgrade1Pt6.h \
	source/acquaman/VESPERS/VESPERS3DScanActionController.h \
	source/ui/VESPERS/VESPERSXRFDetailedDetectorView.h \
	source/ui/VESPERS/VESPERSSingleElementVortexDetectorView.h \
	source/ui/VESPERS/VESPERSFourElementVortexDetectorView.h \
    source/acquaman/VESPERS/VESPERSTimedLineScanConfiguration.h \
    source/ui/VESPERS/VESPERSTimedLineScanConfigurationView.h \
    source/acquaman/VESPERS/VESPERSTimedLineScanActionController.h \
    source/acquaman/VESPERS/VESPERSTimedLineScanActionControllerAssembler.h \
    source/beamline/VESPERS/VESPERSToggleControl.h \
    source/beamline/VESPERS/VESPERS13ElementGeDetector.h

SOURCES +=	source/application/VESPERS/VESPERSAppController.cpp \
	source/application/VESPERS/VESPERSMain.cpp \
	source/beamline/VESPERS/VESPERSBeamline.cpp \
	source/ui/VESPERS/VESPERSMotorView.cpp \
	source/dataman/VESPERS/VESPERSXRFDataLoader.cpp \
	source/acquaman/VESPERS/VESPERSXRFScanConfiguration.cpp \
	source/acquaman/VESPERS/VESPERSXRFScanController.cpp \
	source/ui/VESPERS/VESPERSXRFScanConfigurationView.cpp \
	source/ui/VESPERS/VESPERSPersistentView.cpp \
	source/beamline/VESPERS/VESPERSPIDLoopControl.cpp \
	source/ui/VESPERS/VESPERSPIDLoopControlView.cpp \
	source/ui/VESPERS/VESPERSDeviceStatusView.cpp \
	source/beamline/VESPERS/VESPERSEndstation.cpp \
	source/ui/VESPERS/VESPERSEndstationView.cpp \
	source/beamline/VESPERS/VESPERSMonochromator.cpp \
	source/beamline/VESPERS/VESPERSIntermediateSlits.cpp \
	source/ui/VESPERS/VESPERSBeamSelectorView.cpp \
	source/ui/VESPERS/VESPERSDiagnosticsView.cpp \
	source/acquaman/VESPERS/VESPERSEXAFSScanConfiguration.cpp \
	source/ui/VESPERS/VESPERSEXAFSScanConfigurationView.cpp \
	source/beamline/VESPERS/VESPERSRoperCCDDetector.cpp \
	source/ui/VESPERS/VESPERSCCDDetectorView.cpp \
	source/acquaman/VESPERS/VESPERS2DScanConfiguration.cpp \
	source/ui/VESPERS/VESPERS2DScanConfigurationView.cpp \
	source/dataman/export/VESPERS/VESPERSExporter2DAscii.cpp \
	source/dataman/export/VESPERS/VESPERSExporterSMAK.cpp \
	source/ui/VESPERS/VESPERSSpatialLineScanConfigurationView.cpp \
	source/acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.cpp \
	source/dataman/export/VESPERS/VESPERSExporterLineScanAscii.cpp \
	source/ui/VESPERS/VESPERSEnergyScanConfigurationView.cpp \
	source/acquaman/VESPERS/VESPERSEnergyScanConfiguration.cpp \
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
	source/beamline/VESPERS/VESPERSCCDDetector.cpp \
	source/acquaman/VESPERS/VESPERS3DScanConfiguration.cpp \
	source/ui/VESPERS/VESPERS3DScanConfigurationView.cpp \
	source/beamline/VESPERS/VESPERSPilatusCCDDetector.cpp \
	source/ui/VESPERS/VESPERSPilatusCCDDetectorView.cpp \
	source/dataman/export/VESPERS/VESPERSExporter3DAscii.cpp \
	source/beamline/VESPERS/VESPERSSingleElementVortexDetector.cpp \
	source/beamline/VESPERS/VESPERSFourElementVortexDetector.cpp \
	source/acquaman/VESPERS/VESPERSXASScanActionController.cpp \
	source/acquaman/VESPERS/VESPERSSpatialLineScanActionController.cpp \
	source/acquaman/VESPERS/VESPERSEnergyScanActionController.cpp \
	source/actions3/VESPERS/VESPERSSetStringAction.cpp \
	source/actions3/VESPERS/VESPERSSetStringActionInfo.cpp \
	source/ui/VESPERS/VESPERSScalerView.cpp \
	source/beamline/VESPERS/VESPERSMonochomatorControl.cpp \
	source/dataman/VESPERS/VESPERSUserConfiguration.cpp \
	source/beamline/VESPERS/VESPERSCCDBasicDetectorEmulator.cpp \
	source/acquaman/VESPERS/VESPERS2DScanActionController.cpp \
	source/acquaman/VESPERS/VESPERSTimeScanConfiguration.cpp \
	source/acquaman/VESPERS/VESPERSTimeScanActionController.cpp \
	source/ui/VESPERS/VESPERSTimeScanConfigurationView.cpp \
	source/dataman/VESPERS/VESPERSDbUpgrade1Pt6.cpp \
	source/acquaman/VESPERS/VESPERS3DScanActionController.cpp \
	source/ui/VESPERS/VESPERSXRFDetailedDetectorView.cpp \
	source/ui/VESPERS/VESPERSSingleElementVortexDetectorView.cpp \
	source/ui/VESPERS/VESPERSFourElementVortexDetectorView.cpp \
    source/acquaman/VESPERS/VESPERSTimedLineScanConfiguration.cpp \
    source/ui/VESPERS/VESPERSTimedLineScanConfigurationView.cpp \
    source/acquaman/VESPERS/VESPERSTimedLineScanActionController.cpp \
    source/acquaman/VESPERS/VESPERSTimedLineScanActionControllerAssembler.cpp \
    source/beamline/VESPERS/VESPERSToggleControl.cpp \
    source/beamline/VESPERS/VESPERS13ElementGeDetector.cpp

RESOURCES += \
	source/icons/VESPERS/vespersIcons.qrc











































