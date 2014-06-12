include ( acquamanCommon.pri )

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StripTool2
TEMPLATE = app


SOURCES +=\
	source/StripTool2/STWidget.cpp \
	source/StripTool2/STMain.cpp \
    source/StripTool2/STRealtimeModelAB.cpp

HEADERS  += \
	source/StripTool2/STWidget.h \
    source/StripTool2/STRealtimeModelAB.h
