include ( acquamanCommon.pri )

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StripTool2
TEMPLATE = app


SOURCES +=\
	source/StripTool2/STWidget.cpp \
	source/StripTool2/STMain.cpp

HEADERS  += \
	source/StripTool2/STWidget.h
