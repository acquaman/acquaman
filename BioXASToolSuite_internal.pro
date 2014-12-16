include ( acquamanCommon.pri )
include ( bioxasSideCommon.pri )

QT	   += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BioXASToolSuite
TEMPLATE = app

HEADERS  += \
	source/beamline/BioXAS/BioXASMainBeamline.h \
	source/beamline/BioXAS/BioXASImagingBeamline.h \
	source/beamline/BioXAS/BioXASShutterToolController.h \
	source/ui/BioXAS/BioXASToolSuite/ShutterToolMainWindow.h \
	source/ui/BioXAS/BioXASToolSuite/MotorToolMainScreen.h \
	source/ui/BioXAS/BioXASPseudoMotorControlEditor.h \
	source/beamline/BioXAS/BioXASPseudoMotorControl.h \
	source/ui/BioXAS/BioXASToolSuite/BioXASToolSuiteMainWindow.h

SOURCES += \
	source/application/BioXAS/BioXASToolSuiteMain.cpp \
	source/beamline/BioXAS/BioXASMainBeamline.cpp \
	source/beamline/BioXAS/BioXASImagingBeamline.cpp \
	source/beamline/BioXAS/BioXASShutterToolController.cpp \
	source/ui/BioXAS/BioXASToolSuite/ShutterToolMainWindow.cpp \
	source/ui/BioXAS/BioXASToolSuite/MotorToolMainScreen.cpp \
	source/ui/BioXAS/BioXASPseudoMotorControlEditor.cpp \
	source/beamline/BioXAS/BioXASPseudoMotorControl.cpp \
	source/ui/BioXAS/BioXASToolSuite/BioXASToolSuiteMainWindow.cpp

RESOURCES += \
	source/ui/BioXAS/BioXASToolSuite/resources/BioXASImages.qrc
