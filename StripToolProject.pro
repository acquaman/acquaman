include ( acquamanCommon.pri )

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StripToolProject
TEMPLATE = app

SOURCES += source/StripChart/main.cpp \
	source/StripChart/AddPVDialog.cpp \
    source/StripChart/StripTool.cpp \
    source/StripChart/StripToolItem.cpp

HEADERS  += \
	source/StripChart/AddPVDialog.h \
    source/StripChart/StripTool.h \
    source/StripChart/StripToolItem.h