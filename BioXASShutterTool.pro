include ( acquamanCommon.pri )

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BioXASShutterTool
TEMPLATE = app


SOURCES += \
	source/BioXASShutterTool/MainWindow.cpp \
	source/BioXASShutterTool/Model.cpp \
    source/BioXASShutterTool/BioXASShutterToolMain.cpp

HEADERS  += source/BioXASShutterTool/MainWindow.h \
	source/BioXASShutterTool/Model.h
