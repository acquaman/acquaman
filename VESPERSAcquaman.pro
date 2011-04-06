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
    source/dataman/VESPERS/AMXRFScan.h

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
    source/dataman/VESPERS/AMXRFScan.cpp

RESOURCES += \
	source/ui/VESPERS/EndstationPictures.qrc \
    source/ui/VESPERS/vespersIcons.qrc
