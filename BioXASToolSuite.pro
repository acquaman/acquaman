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
	source/BioXASToolSuite/motorTool/ui/MotorToolMainScreen.h \
    source/BioXASToolSuite/motorTool/BioXASMAXvMotor.h \
    source/BioXASToolSuite/motorTool/ui/MAXvMotorControlEditor.h \
    source/BioXASToolSuite/motorTool/BioXASBeamline.h \
    source/BioXASToolSuite/motorTool/ui/BioXASBeamlineTabWidget.h

SOURCES += \
	source/BioXASToolSuite/BioXASToolMain.cpp \
	source/BioXASToolSuite/MainWindow.cpp \
	source/BioXASToolSuite/shutterTool/ShutterToolMainWindow.cpp \
	source/BioXASToolSuite/shutterTool/ShutterModel.cpp \
	source/BioXASToolSuite/motorTool/MotorModel.cpp \
	source/BioXASToolSuite/motorTool/ui/MotorToolMainScreen.cpp \
    source/BioXASToolSuite/motorTool/ui/MAXvMotorControlEditor.cpp \
    source/BioXASToolSuite/motorTool/BioXASBeamline.cpp \
    source/BioXASToolSuite/motorTool/ui/BioXASBeamlineTabWidget.cpp

FORMS +=

RESOURCES += \
	source/BioXASToolSuite/resources/BioXASImages.qrc














































































































