include ( acquamanCommon.pri )

TARGET = VESPERSAcquaman

FORMS +=

HEADERS +=	source/application/VESPERS/VESPERSAppController.h \
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
    source/ui/VESPERS/VESPERSSampleStageView.h

SOURCES +=	source/application/VESPERS/VESPERSAppController.cpp \
	source/application/VESPERS/VESPERSMain.cpp \
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
    source/ui/VESPERS/VESPERSSampleStageView.cpp

RESOURCES += \
	source/ui/VESPERS/EndstationPictures.qrc \
    source/ui/VESPERS/vespersIcons.qrc \
    source/ui/StopButton.qrc
