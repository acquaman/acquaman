include ( acquamanCommon.pri )

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StripToolProject
TEMPLATE = app

SOURCES += source/StripChart/main.cpp \
	source/StripChart/AddPVDialog.cpp \
    source/StripChart/StripTool.cpp \
    source/StripChart/StripToolItem.cpp \
    source/StripChart/StripToolContainer.cpp \
    source/StripChart/StripToolPlot.cpp \
    source/StripChart/StripToolPVListModel.cpp \
    source/StripChart/StripToolSeries.cpp

HEADERS  += \
	source/StripChart/AddPVDialog.h \
    source/StripChart/StripTool.h \
    source/StripChart/StripToolItem.h \
    source/StripChart/StripToolContainer.h \
    source/StripChart/StripToolPlot.h \
    source/StripChart/StripToolPVListModel.h \
    source/StripChart/StripToolSeries.h

OTHER_FILES += \
    source/StripChart/pvGroupInfo.txt
