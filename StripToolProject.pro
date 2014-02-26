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
    source/StripChart/EditPVDialog.cpp \
    source/StripChart/StripToolSelector.cpp \
    source/StripChart/ReloadPVDialog.cpp \
    source/StripChart/StripToolDataController.cpp \
    source/StripChart/EntryWidget.cpp \
    source/StripChart/DoubleButtonWidget.cpp \
    source/StripChart/TimeEntryWidget.cpp \
    source/StripChart/StripToolControlsPanel.cpp \
    source/StripChart/StripToolSidePanel.cpp \
    source/StripChart/WaterfallEntryWidget.cpp \
    source/StripChart/StripToolSeries.cpp \
    source/StripChart/StripTool0DVariableInfo.cpp \
    source/StripChart/StripToolVariableInfoEditor.cpp \
    source/StripChart/StripToolAbstractVariableInfo.cpp \
    source/StripChart/StripToolBasicVariableInfo.cpp \
    source/StripChart/StripToolBasicVariable.cpp \
    source/StripChart/StripToolAbstractVariable.cpp \
    source/StripChart/StripTool0DVariable.cpp

HEADERS  += \
    source/StripChart/StripToolPlot.h \
    source/StripChart/StripToolPV.h \
    source/StripChart/StripToolView.h \
    source/StripChart/StripTool.h \
    source/StripChart/StripToolModel.h \
    source/StripChart/StripToolListView.h \
    source/StripChart/EditPVDialog.h \
    source/StripChart/StripToolSelector.h \
    source/StripChart/ReloadPVDialog.h \
    source/StripChart/StripToolDataController.h \
    source/StripChart/EntryWidget.h \
    source/StripChart/DoubleButtonWidget.h \
    source/StripChart/TimeEntryWidget.h \
    source/StripChart/StripToolControlsPanel.h \
    source/StripChart/StripToolSidePanel.h \
    source/StripChart/WaterfallEntryWidget.h \
    source/StripChart/StripToolSeries.h \
    source/StripChart/StripTool0DVariableInfo.h \
    source/StripChart/StripToolVariableInfoEditor.h \
    source/StripChart/StripToolAbstractVariableInfo.h \
    source/StripChart/StripToolBasicVariableInfo.h \
    source/StripChart/StripToolBasicVariable.h \
    source/StripChart/StripToolAbstractVariable.h \
    source/StripChart/StripTool0DVariable.h

OTHER_FILES +=
