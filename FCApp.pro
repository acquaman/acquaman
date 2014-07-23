include ( acquamanCommon.pri )

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FCApp
TEMPLATE = app


SOURCES +=\ source/FCApp/FCAppMain.cpp \
    source/FCApp/FCApp.cpp


HEADERS  += \
    source/FCApp/FCApp.h


