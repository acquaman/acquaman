include ( acquamanCommon.pri )
include ( bioxasCommon.pri )

QT	   += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BioXASToolSuite
TEMPLATE = app

HEADERS  += \
#	source/beamline/BioXAS/BioXASBeamlineDef.h \
#	source/beamline/BioXAS/BioXASCLSMAXvMotor.h \
	source/beamline/BioXAS/BioXASMainBeamline.h \
	source/beamline/BioXAS/BioXASSideBeamline.h \
	source/beamline/BioXAS/BioXASImagingBeamline.h \
	source/beamline/BioXAS/BioXASShutterToolController.h \
#	source/ui/BioXAS/BioXASCLSMAXvMotorControlEditor.h \
	source/ui/BioXAS/BioXASToolSuite/MainWindow.h \
	source/ui/BioXAS/BioXASToolSuite/ShutterToolMainWindow.h \
	source/ui/BioXAS/BioXASToolSuite/MotorToolMainScreen.h
SOURCES += \
	source/application/BioXAS/BioXASToolSuiteMain.cpp \
#	source/beamline/BioXAS/BioXASCLSMAXvMotor.cpp \
	source/beamline/BioXAS/BioXASMainBeamline.cpp \
	source/beamline/BioXAS/BioXASSideBeamline.cpp \
	source/beamline/BioXAS/BioXASImagingBeamline.cpp \
	source/beamline/BioXAS/BioXASShutterToolController.cpp \
#	source/ui/BioXAS/BioXASCLSMAXvMotorControlEditor.cpp \
	source/ui/BioXAS/BioXASToolSuite/MainWindow.cpp \
	source/ui/BioXAS/BioXASToolSuite/ShutterToolMainWindow.cpp \
	source/ui/BioXAS/BioXASToolSuite/MotorToolMainScreen.cpp

RESOURCES += \
	source/ui/BioXAS/BioXASToolSuite/resources/BioXASImages.qrc
