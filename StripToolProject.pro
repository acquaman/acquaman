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
    source/StripChart/EditPVDialog.cpp \
    source/StripChart/StripToolSelector.cpp \
    source/StripChart/ReloadPVDialog.cpp \
    source/StripChart/StripToolDataController.cpp \
    source/StripChart/EntryWidget.cpp \
    source/StripChart/DoubleButtonWidget.cpp \
    source/StripChart/TimeEntryWidget.cpp \
    source/StripChart/StripToolControlsPanel.cpp

HEADERS  += \
    source/StripChart/StripToolPlot.h \
    source/StripChart/StripToolPV.h \
    source/StripChart/StripToolView.h \
    source/StripChart/StripTool.h \
    source/StripChart/StripToolModel.h \
    source/StripChart/StripToolListView.h \
    source/StripChart/StripToolQuickControls.h \
    source/StripChart/EditPVDialog.h \
    source/StripChart/StripToolSelector.h \
    source/StripChart/ReloadPVDialog.h \
    source/StripChart/StripToolDataController.h \
    source/StripChart/EntryWidget.h \
    source/StripChart/DoubleButtonWidget.h \
    source/StripChart/TimeEntryWidget.h \
    source/StripChart/StripToolControlsPanel.h

OTHER_FILES +=
