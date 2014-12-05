include ( acquamanCommon.pri )
include ( bioxasCommon.pri )

QT	   += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BioXASToolSuite
TEMPLATE = app

HEADERS  += \
	source/beamline/BioXAS/BioXASMainBeamline.h \
	source/beamline/BioXAS/BioXASSideBeamline.h \
	source/beamline/BioXAS/BioXASImagingBeamline.h \
	source/beamline/BioXAS/BioXASShutterToolController.h \
	source/ui/BioXAS/BioXASToolSuite/MainWindow.h \
	source/ui/BioXAS/BioXASToolSuite/ShutterToolMainWindow.h \
	source/ui/BioXAS/BioXASToolSuite/MotorToolMainScreen.h \
	source/beamline/BioXAS/BioXASSideMonochromatorControl.h \
	source/beamline/BioXAS/BioXASSideMonochromator.h

SOURCES += \
	source/application/BioXAS/BioXASToolSuiteMain.cpp \
	source/beamline/BioXAS/BioXASMainBeamline.cpp \
	source/beamline/BioXAS/BioXASSideBeamline.cpp \
	source/beamline/BioXAS/BioXASImagingBeamline.cpp \
	source/beamline/BioXAS/BioXASShutterToolController.cpp \
	source/ui/BioXAS/BioXASToolSuite/MainWindow.cpp \
	source/ui/BioXAS/BioXASToolSuite/ShutterToolMainWindow.cpp \
	source/ui/BioXAS/BioXASToolSuite/MotorToolMainScreen.cpp \
	source/beamline/BioXAS/BioXASSideMonochromatorControl.cpp \
	source/beamline/BioXAS/BioXASSideMonochromator.cpp

RESOURCES += \
	source/ui/BioXAS/BioXASToolSuite/resources/BioXASImages.qrc
