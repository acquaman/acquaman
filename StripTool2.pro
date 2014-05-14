include ( acquamanCommon.pri )


QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StripTool2
TEMPLATE = app


SOURCES += \
	source/StripTool2/StripTool2.cpp \
	source/StripTool2/STDataCollection.cpp \
	source/StripTool2/STVariable.cpp \
    source/StripTool2/STRealtimeDataCollection.cpp \
    source/StripTool2/STVectorDataCollection.cpp \
    source/StripTool2/STVariableCollection.cpp \
    source/StripTool2/STAppController.cpp \
    source/StripTool2/STPlotView.cpp \
    source/StripTool2/STMain.cpp \
    source/StripTool2/STModel.cpp

HEADERS  += \
	source/StripTool2/StripTool2.h \
	source/StripTool2/STDataCollection.h \
	source/StripTool2/STVariable.h \
    source/StripTool2/STRealtimeDataCollection.h \
    source/StripTool2/STVectorDataCollection.h \
    source/StripTool2/STVariableCollection.h \
    source/StripTool2/STAppController.h \
    source/StripTool2/STPlotView.h \
    source/StripTool2/STModel.h
