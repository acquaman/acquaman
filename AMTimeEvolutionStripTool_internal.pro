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

TARGET = AMTimeEvolutionStripTool

HEADERS += \
    source/application/TimeEvolutionStripTool/AMTESTAppController.h

SOURCES += \
    source/application/TimeEvolutionStripTool/AMTESTAppController.cpp \
    source/application/TimeEvolutionStripTool/AMTESTMain.cpp
