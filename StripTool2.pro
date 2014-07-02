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
    source/StripTool2/STVariableCollection.cpp

HEADERS  += \
	source/StripTool2/STWidget.h \
    source/StripTool2/STPlotEditor.h \
	source/StripTool2/STPlotWidget.h \
	source/StripTool2/STVariable.h \
    source/StripTool2/STPlot.h \
    source/StripTool2/STVariableEditor.h \
    source/StripTool2/STVariableCollection.h
