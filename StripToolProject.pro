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
    source/StripChart/EntryWidget.cpp \
    source/StripChart/DoubleButtonWidget.cpp \
    source/StripChart/TimeEntryWidget.cpp \
    source/StripChart/StripToolControlsPanel.cpp \
    source/StripChart/StripToolSidePanel.cpp \
    source/StripChart/WaterfallEntryWidget.cpp \
    source/StripChart/StripToolSeries.cpp \
    source/StripChart/StripToolVariableInfoEditor.cpp \
    source/StripChart/StripToolVariableInfoImporter.cpp \
    source/StripChart/StripToolVariableInfoExporter.cpp \
    source/StripChart/StripToolVariableInfo.cpp \
    source/StripChart/StripToolVariable.cpp \
    source/StripChart/StripToolSeries2.cpp \
	source/StripChart/StripToolAccumulatorAB.cpp

HEADERS  += \
    source/StripChart/StripToolPlot.h \
    source/StripChart/StripToolView.h \
    source/StripChart/StripTool.h \
    source/StripChart/StripToolModel.h \
    source/StripChart/StripToolListView.h \
    source/StripChart/EditPVDialog.h \
    source/StripChart/StripToolSelector.h \
    source/StripChart/ReloadPVDialog.h \
    source/StripChart/EntryWidget.h \
    source/StripChart/DoubleButtonWidget.h \
    source/StripChart/TimeEntryWidget.h \
    source/StripChart/StripToolControlsPanel.h \
    source/StripChart/StripToolSidePanel.h \
    source/StripChart/WaterfallEntryWidget.h \
    source/StripChart/StripToolSeries.h \
    source/StripChart/StripToolVariableInfoEditor.h \
    source/StripChart/StripToolVariableInfoImporter.h \
    source/StripChart/StripToolVariableInfoExporter.h \
    source/StripChart/StripToolVariableInfo.h \
    source/StripChart/StripToolVariable.h \
    source/StripChart/StripToolSeries2.h \
	source/StripChart/StripToolAccumulatorAB.h

OTHER_FILES +=
