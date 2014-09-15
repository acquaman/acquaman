include ( acquamanCommon.pri )

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BioXASToolSuite
TEMPLATE = app

HEADERS  += \
	source/BioXASToolSuite/MainWindow.h \
	source/BioXASToolSuite/shutterTool/ShutterToolMainWindow.h \
	source/BioXASToolSuite/shutterTool/ShutterModel.h \
	source/BioXASToolSuite/motorTool/MotorModel.h \
    source/BioXASToolSuite/motorTool/MotorToolMainScreen.h

SOURCES += \
	source/BioXASToolSuite/BioXASToolMain.cpp \
	source/BioXASToolSuite/MainWindow.cpp \
	source/BioXASToolSuite/shutterTool/ShutterToolMainWindow.cpp \
	source/BioXASToolSuite/shutterTool/ShutterModel.cpp \
	source/BioXASToolSuite/motorTool/MotorModel.cpp \
	source/BioXASToolSuite/motorTool/MotorToolMainScreen.cpp

FORMS += \
	source/BioXASToolSuite/motorTool/MotorToolMainScreen.ui

RESOURCES += \
	source/BioXASToolSuite/resources/BioXASImages.qrc
























