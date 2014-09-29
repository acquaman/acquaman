include ( acquamanCommon.pri )

QT	   += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BioXASToolSuite
TEMPLATE = app

HEADERS  += \
	source/beamline/BioXAS/BioXASToolSuite/ShutterModel.h \
	source/beamline/BioXAS/BioXASToolSuite/BioXASMAXvMotor.h \
	source/beamline/BioXAS/BioXASToolSuite/BioXASBeamline.h \
	source/beamline/BioXAS/BioXASToolSuite/BioXASBeamlineDataModel.h \
	source/ui/BioXAS/BioXASToolSuite/MainWindow.h \
	source/ui/BioXAS/BioXASToolSuite/ShutterToolMainWindow.h \
	source/ui/BioXAS/BioXASToolSuite/MotorToolMainScreen.h \
	source/ui/BioXAS/BioXASToolSuite/MAXvMotorControlEditor.h \
	source/ui/BioXAS/BioXASToolSuite/BioXASBeamlineTabWidget.h

SOURCES += \
	source/application/BioXAS/BioXASToolSuiteMain.cpp \
	source/beamline/BioXAS/BioXASToolSuite/ShutterModel.cpp \
	source/beamline/BioXAS/BioXASToolSuite/BioXASBeamline.cpp \
	source/beamline/BioXAS/BioXASToolSuite/BioXASBeamlineDataModel.cpp \
	source/ui/BioXAS/BioXASToolSuite/MainWindow.cpp \
	source/ui/BioXAS/BioXASToolSuite/ShutterToolMainWindow.cpp \
	source/ui/BioXAS/BioXASToolSuite/MotorToolMainScreen.cpp \
	source/ui/BioXAS/BioXASToolSuite/MAXvMotorControlEditor.cpp \
	source/ui/BioXAS/BioXASToolSuite/BioXASBeamlineTabWidget.cpp

RESOURCES += \
	source/ui/BioXAS/BioXASToolSuite/resources/BioXASImages.qrc
