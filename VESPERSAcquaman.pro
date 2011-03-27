include ( acquamanCommon.pri )

TARGET = VESPERSAcquaman

FORMS +=

HEADERS +=	source/application/VESPERS/VESPERSAppController.h \
	source/beamline/VESPERS/VESPERSBeamline.h \
    source/ui/VESPERS/VESPERSBeamlineView.h \
    source/ui/AMShutterButton.h \
    source/ui/AMStopButton.h \
    source/ui/VESPERS/VESPERSEndstationView.h \
    source/ui/VESPERS/VESPERSMotorView.h

SOURCES +=	source/application/VESPERS/VESPERSAppController.cpp \
	source/application/VESPERS/VESPERSMain.cpp \
	source/beamline/VESPERS/VESPERSBeamline.cpp \
    source/ui/VESPERS/VESPERSBeamlineView.cpp \
    source/ui/AMShutterButton.cpp \
    source/ui/VESPERS/VESPERSEndstationView.cpp \
    source/ui/VESPERS/VESPERSMotorView.cpp

RESOURCES += \
    source/ui/VESPERS/EndstationPictures.qrc
