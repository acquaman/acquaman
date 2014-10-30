include ( acquamanCommon.pri )

TARGET = VESPERSDataman

FORMS +=

HEADERS += \
	source/beamline/VESPERS/VESPERSBeamline.h \
	source/ui/VESPERS/VESPERSBeamlineView.h \
	source/ui/AMShutterButton.h \
	source/ui/AMStopButton.h \
	source/ui/VESPERS/VESPERSEndstationView.h \
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
	source/ui/VESPERS/VESPERSTemperatureView.h \
	source/util/VESPERS/GeneralUtilities.h \
	source/util/VESPERS/XRFElement.h \
	source/application/VESPERS/VESPERSDatamanAppController.h

SOURCES += \
	source/beamline/VESPERS/VESPERSBeamline.cpp \
	source/ui/VESPERS/VESPERSBeamlineView.cpp \
	source/ui/AMShutterButton.cpp \
	source/ui/VESPERS/VESPERSEndstationView.cpp \
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
	source/ui/VESPERS/VESPERSTemperatureView.cpp \
	source/util/VESPERS/GeneralUtilities.cpp \
	source/util/VESPERS/XRFElement.cpp \
	source/application/VESPERS/VESPERSDatamanMain.cpp \
    source/application/VESPERS/VESPERSDatamanAppController.cpp

RESOURCES += \
	source/ui/VESPERS/EndstationPictures.qrc \
	source/ui/VESPERS/vespersIcons.qrc \
	source/ui/StopButton.qrc
