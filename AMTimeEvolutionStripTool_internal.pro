HOME_FOLDER = $$system(echo $HOME)
PROJECT_ROOT = $$PWD/..

PATH_TO_SOURCE = $$PWD/source
PATH_TO_ACQUAMAN = $$PROJECT_ROOT/acquaman/source
PATH_TO_MPLOT = $$PROJECT_ROOT/MPlot/src

INCLUDEPATH *= $$PATH_TO_SOURCE
INCLUDEPATH *= $$PATH_TO_ACQUAMAN
INCLUDEPATH *= $$PATH_TO_MPLOT

QT *= core network gui sql

include ( $$PATH_TO_AM/compositeCommon/AMMPlot.pri )
include( $$PATH_TO_AM/compositeCommon/AMAcquamanDataServer.pri )
include( $$PATH_TO_AMDS/compositCommon/AcquamanMPlot.pri )

DESTDIR = build

OBJECTS_DIR=build_files
MOC_DIR=build_files
UI_DIR=build_files
RCC_DIR=build_files

TARGET = AMTimeEvolutionStripTool

HEADERS += \
	source/application/TimeEvolutionStripTool/AMTESTAppController.h \
	source/ui/TimeEvolutionStripTool/AMTESTMainWindow.h \
	source/ui/TimeEvolutionStripTool/AMTESTServerConnectionView.h \
	source/ui/TimeEvolutionStripTool/AMTESTCentralWidgetView.h \
	source/util/TimeEvolutionStripTool/AMTESTServerConfiguration.h \
	source/util/TimeEvolutionStripTool/AMTESTServerConnection.h \
    source/ui/TimeEvolutionStripTool/AMTESTServerConnectionButton.h \
    source/util/TimeEvolutionStripTool/AMTESTStripTool.h \
    source/util/TimeEvolutionStripTool/AMTESTSeriesDataModel.h \
    source/util/TimeEvolutionStripTool/AMTESTDataModel.h \
    source/ui/TimeEvolutionStripTool/AMTESTDataModelListView.h \
    source/util/TimeEvolutionStripTool/AMTESTSeriesData.h \
    source/util/TimeEvolutionStripTool/AMTESTRingBuffer.h \
    source/util/TimeEvolutionStripTool/AMTESTImageDataModel.h \
    source/util/TimeEvolutionStripTool/AMTESTImageData.h

SOURCES += \
	source/application/TimeEvolutionStripTool/AMTESTAppController.cpp \
	source/application/TimeEvolutionStripTool/AMTESTMain.cpp \
	source/ui/TimeEvolutionStripTool/AMTESTMainWindow.cpp \
	source/ui/TimeEvolutionStripTool/AMTESTServerConnectionView.cpp \
	source/ui/TimeEvolutionStripTool/AMTESTCentralWidgetView.cpp \
	source/util/TimeEvolutionStripTool/AMTESTServerConfiguration.cpp \
	source/util/TimeEvolutionStripTool/AMTESTServerConnection.cpp \
    source/ui/TimeEvolutionStripTool/AMTESTServerConnectionButton.cpp \
    source/util/TimeEvolutionStripTool/AMTESTStripTool.cpp \
    source/util/TimeEvolutionStripTool/AMTESTSeriesDataModel.cpp \
    source/util/TimeEvolutionStripTool/AMTESTDataModel.cpp \
    source/ui/TimeEvolutionStripTool/AMTESTDataModelListView.cpp \
    source/util/TimeEvolutionStripTool/AMTESTSeriesData.cpp \
    source/util/TimeEvolutionStripTool/AMTESTRingBuffer.cpp \
    source/util/TimeEvolutionStripTool/AMTESTImageDataModel.cpp \
    source/util/TimeEvolutionStripTool/AMTESTImageData.cpp

RESOURCES *= \
	source/icons/icons.qrc
