include ( acquamanCommon.pri )

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StripTool2
TEMPLATE = app


SOURCES +=\
	source/StripTool2/STWidget.cpp \
	source/StripTool2/STMain.cpp \
    source/StripTool2/STPlotEditor.cpp \
	source/StripTool2/STPlotWidget.cpp \
	source/StripTool2/STVariable.cpp \
    source/StripTool2/STPlot.cpp \
    source/StripTool2/STVariableEditor.cpp \
    source/StripTool2/STVariableCollection.cpp \
    source/StripTool2/STVariableCollectionEditor.cpp \
    source/StripTool2/STEditorDialog.cpp \
	source/StripTool2/STEditor.cpp \
    source/StripTool2/STTime.cpp \
    source/StripTool2/STListView.cpp \
    source/StripTool2/STColorButton.cpp \
    source/StripTool2/STPlotSelectorTool.cpp

HEADERS  += \
	source/StripTool2/STWidget.h \
    source/StripTool2/STPlotEditor.h \
	source/StripTool2/STPlotWidget.h \
	source/StripTool2/STVariable.h \
    source/StripTool2/STPlot.h \
    source/StripTool2/STVariableEditor.h \
    source/StripTool2/STVariableCollection.h \
    source/StripTool2/STVariableCollectionEditor.h \
    source/StripTool2/STEditorDialog.h \
	source/StripTool2/STEditor.h \
    source/StripTool2/STTime.h \
    source/StripTool2/STListView.h \
    source/StripTool2/STColorButton.h \
    source/StripTool2/STPlotSelectorTool.h
