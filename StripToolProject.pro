include ( acquamanCommon.pri )

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StripToolProject
TEMPLATE = app

SOURCES += source/StripChart/main.cpp \
    source/StripChart/StripToolPlot.cpp \
    source/StripChart/StripToolPV.cpp \
    source/StripChart/StripToolView.cpp \
    source/StripChart/StripTool.cpp \
    source/StripChart/StripToolModel.cpp \
    source/StripChart/StripToolListView.cpp \
    source/StripChart/StripToolQuickControls.cpp \
    source/StripChart/EditPVDialog.cpp

HEADERS  += \
    source/StripChart/StripToolPlot.h \
    source/StripChart/StripToolPV.h \
    source/StripChart/StripToolView.h \
    source/StripChart/StripTool.h \
    source/StripChart/StripToolModel.h \
    source/StripChart/StripToolListView.h \
    source/StripChart/StripToolQuickControls.h \
    source/StripChart/EditPVDialog.h

OTHER_FILES += \
    source/StripChart/pvGroupInfo.txt
