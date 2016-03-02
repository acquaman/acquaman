#-------------------------------------------------
#
# Project created by QtCreator 2015-06-03T11:07:05
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = AMPIC887DriverServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    AMPIC887ConsoleApplication.cpp \
    AMPIC887Controller.cpp \
    AMGCS2Support.cpp \
    AMGCS2Command.cpp \
    AMGCS2MoveCommand.cpp \
    AMGCS2CurrentPositionCommand.cpp \
    AMPIC887ControllerCollection.cpp \
    AMConsoleInputHandler.cpp \
    AMAbstractConsoleCommandParser.cpp \
    AMPIC887ConsoleCommandParser.cpp \
    AMGCS2CommandFactory.cpp

HEADERS += \
    AMPIC887ConsoleApplication.h \
    AMPIC887Controller.h \
    AMCArrayHandler.h \
    AMGCS2.h \
    AMGCS2Support.h \
    AMGCS2Command.h \
    AMGCS2MoveCommand.h \
    AMGCS2CurrentPositionCommand.h \
    AMPIC887ControllerCollection.h \
    AMConsoleInputHandler.h \
    AMAbstractConsoleCommandParser.h \
    AMPIC887ConsoleCommandParser.h \
    AMGCS2CommandFactory.h

#Libraries for PI_GCS2_DLL
GCS2 = /home/workmai/documents/SGMUpgrade/HexapodCD/Linux/PI_C867/libpi_pi_gcs2_x86_64
GCS2_LIB = -L$$GCS2/lib -lpi_pi_gcs2
GCS2_INCLUDE = $$GCS2/include

INCLUDEPATH *= $$GCS2_INCLUDE

LIBS *= $$GCS2_LIB






































































