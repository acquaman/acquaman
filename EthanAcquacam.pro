
#include (sgmCommon.pri)
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

                #Eigen
                EIGEN_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/Eigen
}

QT += core gui sql opengl network

# add video using Multimedia module from QtMobility, if we have it
CONFIG(mobility) {
        MOBILITY += multimedia
}

DESTDIR = build

DEPENDPATH += $$PATH_TO_AM $$PATH_TO_AM/source
INCLUDEPATH += $$PATH_TO_AM $$PATH_TO_AM/source

INCLUDEPATH += $$EPICS_INCLUDE_DIRS \
                $$GSL_INCLUDE_DIR \
                $$MPLOT_INCLUDE_DIR \
                $$EIGEN_INCLUDE_DIR

LIBS += $$GSL_LIB \
        $$GSL_CBLAS_LIB \
        -L$$MPLOT_LIB_DIR -lMPlot \
        -L$$EPICS_LIB_DIR -lca -lCom

CONFIG(mobility) {
        INCLUDEPATH += $$MOBILITY_QT_INCLUDE_DIR
        LIBS += $$MOBILITY_QT_LIB
}

QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$MPLOT_LIB_DIR"
QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$MPLOT_LIB_DIR"

TARGET = AcquaCam

FORMS += source/ui/dataman/AMSamplePlateSelector.ui \
        source/ui/dataman/AMGenericScanEditor.ui

HEADERS += source/ui/AMColorPickerButton2.h \
        source/ui/AMOverlayVideoWidget2.h \
    source/dataman/database/AMDatabase.h \
    source/util/AMErrorMonitor.h \
    source/dataman/database/AMDbObject.h \
    source/dataman/database/AMDbObjectSupport.h \
    source/dataman/AMnDIndex.h \
    source/util/AMPeriodicTable.h \
    source/util/AMElement.h \
    source/dataman/AMSample.h \
    source/ui/AMCameraBrowserView.h \
    source/ui/AMShapeData.h \
    source/ui/AMCameraConfigurationView.h \
    source/ui/AMCameraConfiguration.h \
    source/ui/AMBeamConfigurationView.h \
    source/ui/AMBeamConfiguration.h \
    source/ui/AMShapeDataSet.h \
    source/ui/AMShapeDataSetView.h \
    source/ui/AMShapeDataView.h \
    source/ui/AMCameraBrowser.h \
    source/ui/AMShapeDataSetGraphicsView.h \
    source/beamline/SGM/SGMMAXvMotor.h \
    source/beamline/CLS/CLSMAXvMotor.h \
    source/beamline/AMPVControl.h \
    source/actions/AMBeamlineControlMoveAction.h \
    source/actions/AMBeamlineControlStopAction.h \
    source/actions/AMBeamlineActionItem.h \
    source/beamline/AMProcessVariable.h \
    source/beamline/AMProcessVariablePrivate.h \
    source/util/AMOrderedSetSignalSource.h \
    source/util/AMDeferredFunctionCall.h \
    source/beamline/AMControl.h \
    source/ui/GraphicsTextItem.h \
    source/ui/AMCameraConfigurationWizard.h \
    source/ui/AMGraphicsVideoSceneCopier.h \
    source/ui/AMGraphicsViewWizard.h \
    source/ui/AMBeamConfigurationWizard.h \
    source/ui/AMSamplePlateWizard.h \
    source/ui/AMShapeDataListView.h \
    source/ui/AMCamera.h \
    source/ui/AMSampleEthan.h \
    source/ui/AMSampleEthanView.h \
    source/ui/AMElementListEdit.h \
    source/ui/util/AMPeriodicTableDialog.h \
    source/ui/util/AMPeriodicTableView.h

SOURCES += source/application/AcquaCam/AcquaCamMain.cpp \
        source/ui/AMColorPickerButton2.cpp \
        source/ui/AMOverlayVideoWidget2.cpp \
    source/dataman/database/AMDatabase.cpp \
    source/util/AMErrorMonitor.cpp \
    source/dataman/database/AMDbObject.cpp \
    source/dataman/database/AMDbObjectSupport.cpp \
    source/dataman/AMnDIndex.cpp \
    source/util/AMPeriodicTable.cpp \
    source/util/AMElement.cpp \
    source/dataman/AMSample.cpp \
    source/ui/AMCameraBrowserView.cpp \
    source/ui/AMShapeData.cpp \
    source/ui/AMCameraConfigurationView.cpp \
    source/ui/AMCameraConfiguration.cpp \
    source/ui/AMBeamConfigurationView.cpp \
    source/ui/AMBeamConfiguration.cpp \
    source/ui/AMShapeDataSet.cpp \
    source/ui/AMShapeDataSetView.cpp \
    source/ui/AMShapeDataView.cpp \
    source/ui/AMCameraBrowser.cpp \
    source/ui/AMShapeDataSetGraphicsView.cpp \
    source/beamline/SGM/SGMMAXvMotor.cpp \
    source/beamline/CLS/CLSMAXvMotor.cpp \
    source/beamline/AMPVControl.cpp \
    source/actions/AMBeamlineControlMoveAction.cpp \
    source/actions/AMBeamlineControlStopAction.cpp \
    source/actions/AMBeamlineActionItem.cpp \
    source/beamline/AMProcessVariable.cpp \
    source/beamline/AMProcessVariablePrivate.cpp \
    source/util/AMOrderedSetSignalSource.cpp \
    source/util/AMDeferredFunctionCall.cpp \
    source/beamline/AMControl.cpp \
    source/ui/GraphicsTextItem.cpp \
    source/ui/AMCameraConfigurationWizard.cpp \
    source/ui/AMGraphicsVideoSceneCopier.cpp \
    source/ui/AMGraphicsViewWizard.cpp \
    source/ui/AMBeamConfigurationWizard.cpp \
    source/ui/AMSamplePlateWizard.cpp \
    source/ui/AMShapeDataListView.cpp \
    source/ui/AMCamera.cpp \
    source/ui/AMSampleEthan.cpp \
    source/ui/AMSampleEthanView.cpp \
    source/ui/AMElementListEdit.cpp \
    source/ui/util/AMPeriodicTableDialog.cpp \
    source/ui/util/AMPeriodicTableView.cpp

RESOURCES = source/util/ElementData.qrc
