# video using Multimedia module from QtMobility
CONFIG += mobility
MOBILITY += multimedia

# Automatically determines a user's home folder
HOME_FOLDER = $$system(echo $HOME)

linux-g++ {

                # Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
                DEV_PATH = beamline/programming

                # The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
                PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman

                # EPICS Dependencies:
                EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/epics/base/include \
                                $$HOME_FOLDER/$$DEV_PATH/epics/base/include/os/Linux
                EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/epics/base/lib/linux-x86

                # MPlot Source
                MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
                MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib

                # GSL Dependencies
                #GSL_LIB = -lgsl
                #GSL_CBLAS_LIB = -lgslcblas
                GSL_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/include
                GSL_LIB = -L$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/lib -lgsl
                GSL_CBLAS_LIB = -lgslcblas

                # LibXML Dependencies (required by dacq library)
                XML_LIB = -lxml2
                XML_INCLUDE_DIR = /usr/include/libxml2

                #CDFLib dependencies
                #CDF_LIB = -lcdf
                #CDF_INCLUDE_DIR = /usr/local/include
                CDF_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/cdf34_1-dist/lib
                CDF_LIB = -L$$CDF_LIB_DIR -lcdf
                CDF_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/cdf34_1-dist/include

                #Qt Mobility Dependencies
                MOBILITY_QT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-1.1.3-Ubuntu12.04/lib
                MOBILITY_QT_LIB = -L$$MOBILITY_QT_LIB_DIR -lQtMultimediaKit
                MOBILITY_QT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-1.1.3-Ubuntu12.04/include
}

QT += core gui sql opengl network

# add video using Multimedia module from QtMobility, if we have it
CONFIG(mobility) {
        MOBILITY += multimedia
}

DESTDIR = build

DEPENDPATH += $$PATH_TO_AM $$PATH_TO_AM/source
INCLUDEPATH += $$PATH_TO_AM $$PATH_TO_AM/source

CONFIG(mobility) {
        INCLUDEPATH += $$MOBILITY_QT_INCLUDE_DIR
        LIBS += $$MOBILITY_QT_LIB
}

TARGET = AcquaCam

FORMS +=

HEADERS += source/ui/AMColorPickerButton2.h \
        source/ui/AMCrosshairOverlayVideoWidget2.h \
        source/ui/AMOverlayVideoWidget2.h \
        source/ui/AMBeamlineCameraBrowser2.h

SOURCES += source/application/AcquaCam/AcquaCamMain.cpp \
        source/ui/AMColorPickerButton2.cpp \
        source/ui/AMCrosshairOverlayVideoWidget2.cpp \
        source/ui/AMOverlayVideoWidget2.cpp \
        source/ui/AMBeamlineCameraBrowser2.cpp

