include ( acquamanCommon.pri )

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StripTool2
TEMPLATE = app


SOURCES +=\
	STWidget.cpp \
    STMain.cpp

HEADERS  += STWidget.h
