include ( clsCommon.pri )
include ( bioxasCommon.pri )
include ( bioxasSideCommon.pri )
include ( bioxasMainCommon.pri )
include ( bioxasImagingCommon.pri )

QT	   += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BioXASToolSuite
TEMPLATE = app

HEADERS  += \
	source/beamline/BioXAS/BioXASShutterToolController.h \
	source/ui/BioXAS/BioXASToolSuite/ShutterToolMainWindow.h \
	source/ui/BioXAS/BioXASToolSuite/MotorToolMainScreen.h \
	source/ui/BioXAS/BioXASCLSMAXvMotorControlEditor.h \
	source/ui/BioXAS/BioXASPseudoMotorControlEditor.h \
	source/ui/BioXAS/BioXASToolSuite/BioXASToolSuiteMainWindow.h

SOURCES += \
	source/application/BioXAS/BioXASToolSuiteMain.cpp \
	source/beamline/BioXAS/BioXASShutterToolController.cpp \
	source/ui/BioXAS/BioXASToolSuite/ShutterToolMainWindow.cpp \
	source/ui/BioXAS/BioXASToolSuite/MotorToolMainScreen.cpp \
	source/ui/BioXAS/BioXASCLSMAXvMotorControlEditor.cpp \
	source/ui/BioXAS/BioXASPseudoMotorControlEditor.cpp \
	source/ui/BioXAS/BioXASToolSuite/BioXASToolSuiteMainWindow.cpp

RESOURCES += \
	source/ui/BioXAS/BioXASToolSuite/resources/BioXASImages.qrc
