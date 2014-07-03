
include (sgmCommon.pri)
include (acquamanCommon.pri)
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
		EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen
}

macx {
		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/include \
				$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/include/os/Darwin
		EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/lib/darwin-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib

		# GSL Dependencies
		#GSL_LIB = -lgsl
		#GSL_CBLAS_LIB = -lgslcblas
		GSL_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/include

		# GSL Dependencies
		GSL_LIB = -L$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/lib -lgsl
		GSL_CBLAS_LIB = -L$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/lib -lgslcblas

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
                #MOBILITY_QT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-1.2-osx/lib/QtMultimediaKit.framework/Versions/Current
                #MOBILITY_QT_LIB = -L$$MOBILITY_QT_LIB_DIR -lQtMultimediaKit
                #MOBILITY_QT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-1.2-osx/include/QtMultimediaKit
                ##MULTIMEDIA_QT_INCLUDE_SRC_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-opensource-src-1.1.3/src/multimedia
                #MOBILITY_QT_INCLUDE_SRC_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-1.2-osx/include/QtMobility

#                MOBILITY_QT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-1.1.3-osx/Library/Frameworks/QtMultimediaKit.framework
#                MOBILITY_QT_LIB = -L$$MOBILITY_QT_LIB_DIR -lQtMultimediaKit
#                MOBILITY_QT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-1.1.3-osx/include/QtMultimediaKit
#                #MULTIMEDIA_QT_INCLUDE_SRC_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-opensource-src-1.1.3/src/multimedia
#                MOBILITY_QT_INCLUDE_SRC_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-1.1.3-osx/include/QtMobility

		MOBILITY_QT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility/qt-mobility-opensource-src-1.1.3/install/Library/Frameworks/QtMultimediaKit.framework/Versions/Current
		MOBILITY_QT_LIB = -L$$MOBILITY_QT_LIB_DIR -lQtMultimediaKit
		MOBILITY_QT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility/qt-mobility-opensource-src-1.1.3/install/include/QtMultimediaKit \
						$$HOME_FOLDER/$$DEV_PATH/qt-mobility/qt-mobility-opensource-src-1.1.3/install/include/QtMobility

                #Eigen
		EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen
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
	#INCLUDEPATH += $$MULTIMEDIA_QT_INCLUDE_SRC_DIR
	INCLUDEPATH += $$MOBILITY_QT_INCLUDE_SRC_DIR
        LIBS += $$MOBILITY_QT_LIB
}

QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$MPLOT_LIB_DIR"
QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$MPLOT_LIB_DIR"

TARGET = AcquaCam

SOURCES += source/application/AcquaCam/AcquaCamMain.cpp

#FORMS += source/ui/dataman/AMSamplePlateSelector.ui \
#        source/ui/dataman/AMGenericScanEditor.ui

#HEADERS += source/ui/AMColorPickerButton2.h \
#        source/ui/AMOverlayVideoWidget2.h \
#    source/dataman/database/AMDatabase.h \
#    source/util/AMErrorMonitor.h \
#    source/dataman/database/AMDbObject.h \
#    source/dataman/database/AMDbObjectSupport.h \
#    source/dataman/AMnDIndex.h \
#    source/util/AMPeriodicTable.h \
#    source/util/AMElement.h \
#    source/dataman/AMSamplePre2013.h \
#    source/ui/AMCameraBrowserView.h \
#    source/ui/AMShapeData.h \
#    source/ui/AMCameraConfigurationView.h \
#    source/ui/AMCameraConfiguration.h \
#    source/ui/AMBeamConfigurationView.h \
#    source/ui/AMBeamConfiguration.h \
#    source/ui/AMShapeDataSet.h \
#    source/ui/AMShapeDataSetView.h \
#    source/ui/AMShapeDataView.h \
#    source/ui/AMCameraBrowser.h \
#    source/ui/AMShapeDataSetGraphicsView.h \
#    source/beamline/SGM/SGMMAXvMotor.h \
#    source/beamline/CLS/CLSMAXvMotor.h \
#    source/beamline/AMPVControl.h \
#    source/actions/AMBeamlineControlMoveAction.h \
#    source/actions/AMBeamlineControlStopAction.h \
#    source/actions/AMBeamlineActionItem.h \
#    source/beamline/AMProcessVariable.h \
#    source/beamline/AMProcessVariablePrivate.h \
#    source/util/AMOrderedSetSignalSource.h \
#    source/util/AMDeferredFunctionCall.h \
#    source/beamline/AMControl.h \
#    source/ui/GraphicsTextItem.h \
#    source/ui/AMCameraConfigurationWizard.h \
#    source/ui/AMGraphicsVideoSceneCopier.h \
#    source/ui/AMGraphicsViewWizard.h \
#    source/ui/AMBeamConfigurationWizard.h \
#    source/ui/AMSamplePlateWizard.h \
#    source/ui/AMShapeDataListView.h \
#    source/ui/AMCamera.h \
#    source/ui/AMSample.h \
#    source/ui/AMSampleView.h \
#    source/ui/AMElementListEdit.h \
#    source/ui/util/AMPeriodicTableDialog.h \
#    source/ui/util/AMPeriodicTableView.h \
#    source/acquaman.h \
#    source/ui/util/AMSamplePeriodicTableDialog.h \
#    source/dataman/AMSamplePlatePre2013.h \
#    source/dataman/info/AMControlInfoList.h \
#    source/dataman/info/AMControlInfo.h \
#    source/ui/AMSampleContainer.h \
#    source/ui/AMSampleContainerView.h \
#    source/dataman/AMScan.h \
#    source/dataman/datastore/AMInMemoryDataStore.h \
#    source/dataman/AMScanDictionary.h \
#    source/dataman/AMScanParametersDictionary.h \
#    source/dataman/AMAnalysisBlock.h \
#    source/util/AMDateTimeUtils.h \
#    source/dataman/datasource/AMDataSourceSeriesData.h \
#    source/dataman/datasource/AMDataSourceImageDatawDefault.h \
#    source/dataman/datasource/AMDataSourceImageData.h \
#    source/application/AMPluginsManager.h \
#    source/util/AMSettings.h \
#    source/dataman/datasource/AMRawDataSource.h \
#    source/acquaman/AMScanConfiguration.h \
#    source/dataman/datastore/AMDataStore.h \
#    source/dataman/AMNumber.h \
#    source/dataman/AMRun.h \
#    source/util/AMTagReplacementParser.h \
#    source/dataman/datasource/AMDataSource.h \
#    source/ui/dataman/AMSimpleDataSourceEditor.h \
#    source/dataman/info/AMDetectorInfoSet.h \
#    source/dataman/info/AMDetectorInfo.h \
#    source/dataman/AMAxisInfo.h \
#    source/dataman/database/AMConstDbObject.h \
#	source/beamline/AMBeamline.h \
#	source/beamline/AMControlSet.h \
#	source/beamline/AMDetectorSet.h \
#	source/beamline/AMDetector.h \
#	source/actions3/actions/AMDetectorReadAction.h \
#	source/actions3/actions/AMDetectorCleanupAction.h \
#	source/actions3/actions/AMDetectorTriggerAction.h \
#	source/actions3/AMActionInfo3.h \
#	source/beamline/AMDetectorTriggerSource.h

#SOURCES += source/application/AcquaCam/AcquaCamMain.cpp \
#        source/ui/AMColorPickerButton2.cpp \
#        source/ui/AMOverlayVideoWidget2.cpp \
#    source/dataman/database/AMDatabase.cpp \
#    source/util/AMErrorMonitor.cpp \
#    source/dataman/database/AMDbObject.cpp \
#    source/dataman/database/AMDbObjectSupport.cpp \
#    source/dataman/AMnDIndex.cpp \
#    source/util/AMPeriodicTable.cpp \
#    source/util/AMElement.cpp \
#    source/dataman/AMSamplePre2013.cpp \
#    source/ui/AMCameraBrowserView.cpp \
#    source/ui/AMShapeData.cpp \
#    source/ui/AMCameraConfigurationView.cpp \
#    source/ui/AMCameraConfiguration.cpp \
#    source/ui/AMBeamConfigurationView.cpp \
#    source/ui/AMBeamConfiguration.cpp \
#    source/ui/AMShapeDataSet.cpp \
#    source/ui/AMShapeDataSetView.cpp \
#    source/ui/AMShapeDataView.cpp \
#    source/ui/AMCameraBrowser.cpp \
#    source/ui/AMShapeDataSetGraphicsView.cpp \
#    source/beamline/SGM/SGMMAXvMotor.cpp \
#    source/beamline/CLS/CLSMAXvMotor.cpp \
#    source/beamline/AMPVControl.cpp \
#    source/actions/AMBeamlineControlMoveAction.cpp \
#    source/actions/AMBeamlineControlStopAction.cpp \
#    source/actions/AMBeamlineActionItem.cpp \
#    source/beamline/AMProcessVariable.cpp \
#    source/beamline/AMProcessVariablePrivate.cpp \
#    source/util/AMOrderedSetSignalSource.cpp \
#    source/util/AMDeferredFunctionCall.cpp \
#    source/beamline/AMControl.cpp \
#    source/ui/GraphicsTextItem.cpp \
#    source/ui/AMCameraConfigurationWizard.cpp \
#    source/ui/AMGraphicsVideoSceneCopier.cpp \
#    source/ui/AMGraphicsViewWizard.cpp \
#    source/ui/AMBeamConfigurationWizard.cpp \
#    source/ui/AMSamplePlateWizard.cpp \
#    source/ui/AMShapeDataListView.cpp \
#    source/ui/AMCamera.cpp \
#    source/ui/AMSample.cpp \
#    source/ui/AMSampleView.cpp \
#    source/ui/AMElementListEdit.cpp \
#    source/ui/util/AMPeriodicTableDialog.cpp \
#    source/ui/util/AMPeriodicTableView.cpp \
#    source/acquaman.cpp \
#    source/ui/util/AMSamplePeriodicTableDialog.cpp \
#    source/dataman/AMSamplePlatePre2013.cpp \
#    source/dataman/info/AMControlInfoList.cpp \
#    source/dataman/info/AMControlInfo.cpp \
#    source/ui/AMSampleContainer.cpp \
#    source/ui/AMSampleContainerView.cpp \
#    source/dataman/AMScan.cpp \
#    source/dataman/datastore/AMInMemoryDataStore.cpp \
#    source/dataman/AMScanDictionary.cpp \
#    source/dataman/AMScanParametersDictionary.cpp \
#    source/dataman/AMAnalysisBlock.cpp \
#    source/util/AMDateTimeUtils.cpp \
#    source/dataman/datasource/AMDataSourceSeriesData.cpp \
#    source/dataman/datasource/AMDataSourceImageDatawDefault.cpp \
#    source/dataman/datasource/AMDataSourceImageData.cpp \
#    source/application/AMPluginsManager.cpp \
#    source/util/AMSettings.cpp \
#    source/dataman/datasource/AMRawDataSource.cpp \
#    source/acquaman/AMScanConfiguration.cpp \
#    source/dataman/datastore/AMDataStore.cpp \
#    source/dataman/AMNumber.cpp \
#    source/dataman/AMRun.cpp \
#    source/util/AMTagReplacementParser.cpp \
#    source/dataman/datasource/AMDataSource.cpp \
#    source/ui/dataman/AMSimpleDataSourceEditor.cpp \
#    source/dataman/info/AMDetectorInfoSet.cpp \
#    source/dataman/info/AMDetectorInfo.cpp \
#    source/dataman/AMAxisInfo.cpp \
#    source/dataman/database/AMConstDbObject.cpp \
#	source/beamline/AMBeamline.cpp \
#	source/beamline/AMControlSet.cpp \
#	source/beamline/AMDetectorSet.cpp \
#	source/beamline/AMDetector.cpp \
#	source/actions3/actions/AMDetectorReadAction.cpp \
#	source/actions3/actions/AMDetectorCleanupAction.cpp \
#	source/actions3/actions/AMDetectorTriggerAction.cpp \
#	source/actions3/AMActionInfo3.cpp \
#	source/beamline/AMDetectorTriggerSource.cpp

#RESOURCES = source/util/ElementData.qrc
