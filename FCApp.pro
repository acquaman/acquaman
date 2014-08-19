include ( acquamanCommon.pri )

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FCApp
TEMPLATE = app


SOURCES +=\
    source/FCApp/FCAppMain.cpp \
    source/FCApp/FCApp.cpp \
    source/FCApp/BeamlineShutters.cpp \
    source/FCApp/BeamPositionMonitor.cpp \
    source/FCApp/BeamlineShutterView.cpp \
    source/FCApp/BeamPositionMonitorView.cpp


HEADERS  += \
    source/FCApp/FCApp.h \
    source/FCApp/BeamlineShutters.h \
    source/FCApp/BeamlineShutterView.h \
    source/FCApp/BeamPositionMonitor.h \
    source/FCApp/BeamPositionMonitorView.h


