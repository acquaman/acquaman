# #####################################################################
# QMake project file for acquaman.  		January 2010. mark.boots@usask.ca
# Note: Set EPICS_INCLUDE_DIRS, EPICS_LIB_DIR, VLC_*, and GSL_* correctly for platform
# ####################################################################

# Video Support: Remove this line if you do not have the multimedia module from QtMobility
# CONFIG += mobility

# Automatically determines a user's home folder
HOME_FOLDER = $$system(echo $HOME)

macx {

	# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
	DEV_PATH = dev

	# EPICS Dependencies:
	EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/base-3.14.12/include \
		$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/base-3.14.12/include/os/Darwin
	EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/base-3.14.12/lib/darwin-x86

	# MPlot Source
	MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/src
	GSL_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/include

	# GSL Dependencies
	GSL_LIB = -L$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/lib -lgsl
	GSL_CBLAS_LIB = -L$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/lib -lgslcblas

	# QwtPlot3d dependencies (Disabled for now...)
	# QWTPLOT3D_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/qwtplot3d/lib
	# QWTPLOT3D_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/qwtplot3d/include

	# LibXML Dependencies (required by dacq library)
	XML_LIB = -lxml2
	XML_INCLUDE_DIR = /usr/include/libxml2
}
linux-g++ {

	# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
	DEV_PATH = beamline/programming

	# EPICS Dependencies:
	EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/epics/base/include \
		$$HOME_FOLDER/$$DEV_PATH/epics/base/include/os/Linux
	EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/epics/base/lib/linux-x86

	# MPlot Source
	MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/src

	# GSL Dependencies
	GSL_LIB = -lgsl
	GSL_CBLAS_LIB = -lgslcblas

	# LibXML Dependencies (required by dacq library)
	XML_LIB = -lxml2
	XML_INCLUDE_DIR = /usr/include/libxml2
}
# The following works well for CLS beamline OPI machines, built using VMSL54.cs.clsi.ca

linux-g++-64 {

	# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
	DEV_PATH = dev

	# EPICS Dependencies:
	EPICS_INCLUDE_DIRS = /home/epics/src/R3.14.12/base/include \
		/home/epics/src/R3.14.12/base/include/os/Linux
	EPICS_LIB_DIR = /home/epics/src/R3.14.12/base/lib/linux-x86_64

	# MPlot Source
	MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/src

	# GSL Dependencies
	GSL_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/include
	GSL_LIB = -L$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/lib -lgsl
	GSL_CBLAS_LIB = -lgslcblas

	# LibXML Dependencies (required by dacq library)
	XML_LIB = -lxml2
	XML_INCLUDE_DIR = /usr/include/libxml2
}

QT += core gui sql opengl

# video using Multimedia module from QtMobility, if we have it
#CONFIG(mobility) {
#	MOBILITY += multimedia
#}

DESTDIR = build
DEPENDPATH += . source
INCLUDEPATH += . source

INCLUDEPATH += $$EPICS_INCLUDE_DIRS \
	$$MPLOT_INCLUDE_DIR \
	$$GSL_INCLUDE_DIR \
	$$XML_INCLUDE_DIR \
	$$QWTPLOT3D_INCLUDE_DIR

LIBS += $$GSL_LIB \
	$$GSL_CBLAS_LIB \
	$$XML_LIB \
#	-L$$QWTPLOT3D_LIB_DIR -lqwtplot3d \
	-L$$EPICS_LIB_DIR -lca -lCom


# Specify runtime search locations for libraries (Must change for release bundle, if epics in a different location)
macx {

	contains(QT_MINOR_VERSION, 7):contains(QT_PATCH_VERSION, 2) {
	# 4.7.2: Use same as linux-g++
#		QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$EPICS_LIB_DIR,-rpath,$$QWTPLOT3D_LIB_DIR"
#		QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$EPICS_LIB_DIR,-rpath,$$QWTPLOT3D_LIB_DIR"
		QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$EPICS_LIB_DIR"
		QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$EPICS_LIB_DIR"
	} else {
		QMAKE_LFLAGS_RPATH += "$$EPICS_LIB_DIR"
		#QMAKE_LFLAGS_RPATH += "$$QWTPLOT3D_LIB_DIR"
	}
}


linux-g++ {
		#QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$EPICS_LIB_DIR,-rpath,$$QwtPlot3d_LIB_DIR"
		#QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$EPICS_LIB_DIR,-rpath,$$QwtPlot3d_LIB_DIR"
		QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$EPICS_LIB_DIR"
		QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$EPICS_LIB_DIR"
}
linux-g++-64 {
		#QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$EPICS_LIB_DIR,-rpath,$$QwtPlot3d_LIB_DIR"
		#QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$EPICS_LIB_DIR,-rpath,$$QwtPlot3d_LIB_DIR"
		QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$EPICS_LIB_DIR"
		QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$EPICS_LIB_DIR"
}


# Source Files (Acquaman Common)
#######################

HEADERS += ../MPlot/src/MPlot/MPlot.h \
	../MPlot/src/MPlot/MPlotAbstractTool.h \
	../MPlot/src/MPlot/MPlotAxis.h \
	../MPlot/src/MPlot/MPlotAxisScale.h \
	../MPlot/src/MPlot/MPlotColorMap.h \
	../MPlot/src/MPlot/MPlotImage.h \
	../MPlot/src/MPlot/MPlotImageData.h \
	../MPlot/src/MPlot/MPlotItem.h \
	../MPlot/src/MPlot/MPlotLegend.h \
	../MPlot/src/MPlot/MPlotMarker.h \
	../MPlot/src/MPlot/MPlotPoint.h \
	../MPlot/src/MPlot/MPlotRectangle.h \
	../MPlot/src/MPlot/MPlotSeries.h \
	../MPlot/src/MPlot/MPlotSeriesData.h \
	../MPlot/src/MPlot/MPlotTools.h \
	../MPlot/src/MPlot/MPlotWidget.h \
	source/acquaman/AMAcqScanOutput.h \
	source/acquaman/AMAcqScanSpectrumOutput.h \
	source/acquaman/AMDacqScanController.h \
	source/acquaman/AMRegion.h \
	source/acquaman/AMRegionsList.h \
	source/acquaman/AMScanConfiguration.h \
	source/acquaman/AMScanController.h \
	source/acquaman/AMXASScanConfiguration.h \
	source/acquaman/dacq3_2/acqDataHandler.h \
	source/acquaman/dacq3_2/acqLibHelper.h \
	source/acquaman/dacq3_2/acquisitionLib.h \
	source/acquaman/dacq3_2/acquisitionLib.internal.h \
	source/acquaman/dacq3_2/acquisitionLib.main.h \
	source/acquaman/dacq3_2/displayAlias.h \
	source/acquaman/dacq3_2/epicsConnect.h \
	source/acquaman/dacq3_2/epicsConnect.main.h \
	source/acquaman/dacq3_2/factoryQtTemplate.h \
	source/acquaman/dacq3_2/OutputHandler/acqBaseOutput.h \
	source/acquaman/dacq3_2/OutputHandler/acqBaseStream.h \
	source/acquaman/dacq3_2/OutputHandler/acqFactory.h \
	source/acquaman/dacq3_2/OutputHandler/acqFileStream.h \
	source/acquaman/dacq3_2/OutputHandler/acqProperties.h \
	source/acquaman/dacq3_2/OutputHandler/acqTextOutput.h \
	source/acquaman/dacq3_2/OutputHandler/acqTextSpectrumOutput.h \
	source/acquaman/dacq3_2/qepicsacqclass.h \
	source/acquaman/dacq3_2/qepicsacqlocal.h \
	source/acquaman/dacq3_2/qepicsadvacq.h \
	source/application/AMAppController.h \
	source/util/AMBiHash.h \
	source/util/AMErrorMonitor.h \
	source/util/AMSettings.h \
	source/beamline/AMBeamline.h \
	source/beamline/AMBeamlineActionItem.h \
	source/beamline/AMBeamlineControlAction.h \
	source/beamline/AMControl.h \
	source/beamline/AMControlSet.h \
	source/beamline/AMControlState.h \
	source/beamline/AMDetector.h \
	source/beamline/AMProcessVariable.h \
	source/beamline/AMPVNames.h \
	source/dataman/AMDatabase.h \
	source/dataman/AMDbObject.h \
	source/dataman/AMDetectorInfo.h \
	source/dataman/AMExperiment.h \
	source/dataman/AMFirstTimeController.h \
	source/dataman/AMImportController.h \
	source/dataman/AMRun.h \
	source/dataman/AMSample.h \
	source/dataman/AMScan.h \
	source/dataman/AMScanSetModel.h \
	source/dataman/AMXASScan.h \
	source/dataman/SGM2004FileLoader.h \
	source/dataman/SGM2010FastFileLoader.h \
	source/muParser/muParser.h \
	source/muParser/muParserBase.h \
	source/muParser/muParserBytecode.h \
	source/muParser/muParserCallback.h \
	source/muParser/muParserDef.h \
	source/muParser/muParserError.h \
	source/muParser/muParserFixes.h \
	source/muParser/muParserStack.h \
	source/muParser/muParserToken.h \
	source/muParser/muParserTokenReader.h \
	source/ui/AMRunSelector.h \
	source/ui/AMControlSetView.h \
	source/ui/AMCramBarHorizontal.h \
	source/ui/AMDataView.h \
	source/ui/AMDataViewWithActionButtons.h \
	source/ui/AMFirstTimeWidget.h \
	source/ui/AMFlowGraphicsLayout.h \
	source/ui/AMImportControllerWidget.h \
	source/ui/AMMainWindow.h \
	source/ui/AMRegionsLineView.h \
	source/ui/AMRunExperimentInsert.h \
	source/ui/AMScanConfigurationView.h \
	source/ui/AMScanView.h \
	source/ui/AMSidebar.h \
	source/ui/AMStatusView.h \
	source/ui/AMThumbnailScrollViewer.h \
	source/ui/AMXASRegionsView.h \
	source/ui/BottomBar.h \
	#deprecated: source/ui/AMBeamlineCameraWidget.h \
	source/ui/AMControlEditor.h \
	source/acquaman.h \
	source/ui/AMNewRunDialog.h \
	source/ui/AMDetectorView.h \
	source/ui/AMWorkflowManagerView.h \
	source/beamline/AMBeamlineScanAction.h \
	source/dataman/AMAbstractFileLoader.h \
	source/beamline/AMBeamlineControlMoveAction.h \
	source/ui/AMPrefixSuffixLineEdit.h \
	source/ui/AMDragDropItemModel.h \
	source/dataman/AMRunExperimentItems.h \
	source/ui/AMSampleManagementWidget.h \
	source/ui/AMSampleManipulatorView.h \
	source/ui/AMSamplePlateView.h \
	source/dataman/AMControlInfoList.h \
	source/dataman/AMSamplePlate.h \
	source/ui/AMGenericScanEditor.h \
	source/ui/AMDetailedItemDelegate.h \
	source/ui/AMVerticalStackWidget.h \
	source/ui/AMHeaderButton.h \
	source/ui/AMSampleEditor.h \
	source/ui/AMDateTimeUtils.h \
	source/ui/AMElementListEdit.h \
	source/ui/AMCloseItemDelegate.h \
	source/ui/AMDataSourcesEditor.h \
	source/beamline/AMBeamlineActionsList.h \
	source/ui/AMWrappingLineEdit.h \
	source/beamline/AMBeamlineControlSetMoveAction.h \
	source/ui/AMStartScreen.h \
	source/ui/AMSignallingGraphicsScene.h \
	source/dataman/AMUser.h \
	#deprecated: source/ui/AMVideoPlayerWidget.h \
	source/dataman/AMXESScan.h \
	source/dataman/ALSBL8XESDetectorInfo.h \
	source/dataman/ALSBL8XASFileLoader.h \
	source/ui/AMWindowPaneModel.h \
	source/dataman/AMScanEditorModelItem.h \
	source/dataman/AMDataSource.h \
	source/dataman/AMAxisInfo.h \
	source/dataman/AMNumber.h \
	source/dataman/AMnDIndex.h \
	source/dataman/AMDataStore.h \
	source/dataman/AMAnalysisBlock.h \
	source/dataman/AMDataSourceSeriesData.h \
	source/dataman/AMDataSourceImageData.h \
	source/analysis/AM1DExpressionAB.h \
	source/dataman/AMDbObjectSupport.h \
	source/util/AMOrderedSetSignalSource.h \
	source/util/AMOrderedSet.h \
	source/dataman/AMRawDataSource.h \
	source/analysis/AM1DExpressionABEditor.h \
	source/dataman/AMMeasurementInfo.h \
	source/dataman/AMInMemoryDataStore.h \
	source/acquaman/AMFastScanConfiguration.h \
	source/ui/AMScanConfigurationViewer.h \
	source/dataman/AMFastScan.h \
	source/analysis/AM2DSummingAB.h \
	source/analysis/AMStandardAnalysisBlock.h \
	source/analysis/AM2DSummingABEditor.h \
	source/util/AMOrderedList.h \
	source/beamline/AMBeamlineParallelActionsList.h \
	source/acquaman/AMControlOptimization.h \
	source/acquaman/AMDetectorInfoList.h \
	source/ui/AMControlOptimizationView.h \
	source/dataman/SGM2010FastSensibleFileLoader.h \
	source/beamline/AMBeamlineControlStopAction.h \
	source/dataman/REIXS/REIXSXESRawFileLoader.h \
	source/util/AMDeferredFunctionCall.h \
	#deprecated: source/ui/AMVideoWidget.h \
	source/ui/AMScanConfigurationViewHolder.h \
	source/ui/AMPeriodicTableView.h \
	source/util/AMPeriodicTable.h \
	source/util/AMElement.h \
	source/dataman/AMSpectralOutputDetectorInfo.h \
	source/dataman/MCPDetectorInfo.h \
	source/dataman/PGTDetectorInfo.h \
	source/beamline/AMSingleControlDetector.h \
	source/beamline/AMSpectralOutputDetector.h \
	source/beamline/PGTDetector.h \
	source/beamline/MCPDetector.h \
	source/ui/AMDetectorViewSupport.h \
	source/ui/AMSingleControlDetectorView.h \
	source/ui/MCPDetectorView.h \
	source/ui/PGTDetectorView.h \
	source/ui/AMDetectorSetView.h \
	source/beamline/AMDetectorSet.h \
	source/dataman/AMROIInfo.h \
	source/beamline/AMROI.h \
	source/ui/AMSamplePositionViewActionsWidget.h \
	source/beamline/AMBeamlineListAction.h \
	source/beamline/AMBeamlineControlWaitAction.h \
	source/beamline/AMBeamlineUserConfirmAction.h \
	source/ui/AMScanQueryModel.h \
	source/ui/AMFreeRunScanConfigurationViewHolder.h \
	source/dataman/AMExportController.h \
	source/dataman/AMExporter.h \
	source/dataman/AMExporterOption.h \
	source/dataman/AMExporterOptionGeneral.h \
	source/dataman/AMExporterOptionGeneralAscii.h \
	source/dataman/AMExporterGeneralAscii.h \
	source/ui/AMExportWizard.h \
	source/ui/AMFolderPathLineEdit.h \
	source/util/AMTagReplacementParser.h \
	source/ui/AMExporterOptionGeneralAsciiView.h \
#	source/ui/AM3dDataSourceView.h \
	source/ui/AMTopFrame.h \
	source/beamline/CLS/CLSVMEMotor.h \
	source/analysis/AM1DDerivativeAB.h \
	source/analysis/AMExternalScanDataSourceAB.h \
	source/analysis/AM1DSummingAB.h \
	source/analysis/AMDeadTimeAB.h \
	source/dataman/AMProcessVariableDataSource.h \
	source/ui/AMChooseScanDialog.h \
	source/application/AMDatamanAppController.h \
	source/ui/AMColorPickerButton.h

CONFIG(mobility) {
HEADERS +=	source/ui/AMCrosshairOverlayVideoWidget.h \
	source/ui/AMOverlayVideoWidget.h \
	source/ui/AMBeamlineCameraBrowser.h
}

FORMS +=	source/ui/AMDataView.ui \
	source/ui/AMDataViewEmptyHeader.ui \
	source/ui/AMDataViewSectionHeader.ui \
	source/ui/AMImportControllerWidget.ui \
	source/ui/AMScanConfigurationView.ui \
	source/ui/BottomBar.ui \
	source/ui/AMGenericScanEditor.ui \
	source/ui/AMDataSourcesEditor.ui \
	source/ui/AMSamplePlateSelector.ui \
	source/ui/AMSamplePositionViewActionsWidget.ui \
	source/ui/AMExporterOptionGeneralAsciiView.ui \
	source/ui/AMDataViewActionsBar.ui \
	source/ui/AMChooseScanDialog.ui
SOURCES += ../MPlot/src/MPlot/MPlot.cpp \
	../MPlot/src/MPlot/MPlotAbstractTool.cpp \
	../MPlot/src/MPlot/MPlotAxis.cpp \
	../MPlot/src/MPlot/MPlotAxisScale.cpp \
	../MPlot/src/MPlot/MPlotColorMap.cpp \
	../MPlot/src/MPlot/MPlotImage.cpp \
	../MPlot/src/MPlot/MPlotImageData.cpp \
	../MPlot/src/MPlot/MPlotItem.cpp \
	../MPlot/src/MPlot/MPlotLegend.cpp \
	../MPlot/src/MPlot/MPlotMarker.cpp \
	../MPlot/src/MPlot/MPlotPoint.cpp \
	../MPlot/src/MPlot/MPlotRectangle.cpp \
	../MPlot/src/MPlot/MPlotSeries.cpp \
	../MPlot/src/MPlot/MPlotSeriesData.cpp \
	../MPlot/src/MPlot/MPlotTools.cpp \
	../MPlot/src/MPlot/MPlotWidget.cpp \
	source/acquaman/AMAcqScanOutput.cpp \
	source/acquaman/AMAcqScanSpectrumOutput.cpp \
	source/acquaman/AMDacqScanController.cpp \
	source/acquaman/AMRegion.cpp \
	source/acquaman/AMRegionsList.cpp \
	source/acquaman/AMScanConfiguration.cpp \
	source/acquaman/AMScanController.cpp \
	source/acquaman/AMXASScanConfiguration.cpp \
	source/acquaman/dacq3_2/acqAction.c \
	source/acquaman/dacq3_2/acqActSetup.c \
	source/acquaman/dacq3_2/acqExtern.c \
	source/acquaman/dacq3_2/acqLibHelper.c \
	source/acquaman/dacq3_2/acqLoad.c \
	source/acquaman/dacq3_2/acqMessage.c \
	source/acquaman/dacq3_2/acqMonitor.c \
	source/acquaman/dacq3_2/acqMotor.c \
	source/acquaman/dacq3_2/channel_hash.c \
	source/acquaman/dacq3_2/channel.c \
	source/acquaman/dacq3_2/connector.c \
	source/acquaman/dacq3_2/displayAlias.cpp \
	source/acquaman/dacq3_2/macro.c \
	source/acquaman/dacq3_2/OutputHandler/acqBaseOutput.cpp \
	source/acquaman/dacq3_2/OutputHandler/acqBaseStream.cpp \
	source/acquaman/dacq3_2/OutputHandler/acqFactory.cpp \
	source/acquaman/dacq3_2/OutputHandler/acqFileStream.cpp \
	source/acquaman/dacq3_2/OutputHandler/acqTextOutput.cpp \
	source/acquaman/dacq3_2/OutputHandler/acqTextSpectrumOutput.cpp \
	source/acquaman/dacq3_2/qepicsacqclass.cpp \
	source/acquaman/dacq3_2/qepicsacqlocal.cpp \
	source/acquaman/dacq3_2/qepicsadvacq.cpp \
	source/acquaman/dacq3_2/update.c \
	source/acquaman/dacq3_2/xmlRead.cpp \
	source/acquaman/dacq3_2/xmlWrite.cpp \
	source/application/AMAppController.cpp \
	source/util/AMErrorMonitor.cpp \
	source/util/AMSettings.cpp \
	source/beamline/AMBeamline.cpp \
	source/beamline/AMDetector.cpp \
	source/beamline/AMBeamlineActionItem.cpp \
	source/beamline/AMBeamlineControlAction.cpp \
	source/beamline/AMControl.cpp \
	source/beamline/AMControlSet.cpp \
	source/beamline/AMControlState.cpp \
	source/beamline/AMProcessVariable.cpp \
	source/beamline/AMPVNames.cpp \
	source/dataman/AMDatabase.cpp \
	source/dataman/AMDbObject.cpp \
	source/dataman/AMDetectorInfo.cpp \
	source/dataman/AMExperiment.cpp \
	source/dataman/AMFirstTimeController.cpp \
	source/dataman/AMImportController.cpp \
	source/dataman/AMRun.cpp \
	source/dataman/AMSample.cpp \
	source/dataman/AMScan.cpp \
	source/dataman/AMScanSetModel.cpp \
	source/dataman/AMXASScan.cpp \
	source/dataman/SGM2004FileLoader.cpp \
	source/dataman/SGM2010FastFileLoader.cpp \
	source/muParser/muParser.cpp \
	source/muParser/muParserBase.cpp \
	source/muParser/muParserBytecode.cpp \
	source/muParser/muParserCallback.cpp \
	source/muParser/muParserError.cpp \
	source/muParser/muParserTokenReader.cpp \
	source/ui/AMRunSelector.cpp \
	source/ui/AMControlSetView.cpp \
	source/ui/AMCramBarHorizontal.cpp \
	source/ui/AMDataView.cpp \
	source/ui/AMDataViewWithActionButtons.cpp \
	source/ui/AMFlowGraphicsLayout.cpp \
	source/ui/AMMainWindow.cpp \
	source/ui/AMRegionsLineView.cpp \
	source/ui/AMRunExperimentInsert.cpp \
	source/ui/AMScanView.cpp \
	source/ui/AMSidebar.cpp \
	source/ui/AMStatusView.cpp \
	source/ui/AMThumbnailScrollViewer.cpp \
	source/ui/AMXASRegionsView.cpp \
	source/ui/BottomBar.cpp \
	#deprecated: source/ui/AMBeamlineCameraWidget.cpp \
	source/ui/AMControlEditor.cpp \
	source/ui/AMDetectorView.cpp \
	source/ui/AMNewRunDialog.cpp \
	source/ui/AMWorkflowManagerView.cpp \
	source/beamline/AMBeamlineScanAction.cpp \
	source/beamline/AMBeamlineControlMoveAction.cpp \
	source/ui/AMPrefixSuffixLineEdit.cpp \
	source/ui/AMDragDropItemModel.cpp \
	source/dataman/AMRunExperimentItems.cpp \
	source/ui/AMSampleManagementWidget.cpp \
	source/ui/AMSampleManipulatorView.cpp \
	source/ui/AMSamplePlateView.cpp \
	source/dataman/AMControlInfoList.cpp \
	source/dataman/AMSamplePlate.cpp \
	source/ui/AMGenericScanEditor.cpp \
	source/ui/AMDetailedItemDelegate.cpp \
	source/ui/AMVerticalStackWidget.cpp \
	source/ui/AMHeaderButton.cpp \
	source/ui/AMSampleEditor.cpp \
	source/ui/AMDateTimeUtils.cpp \
	source/ui/AMElementListEdit.cpp \
	source/ui/AMCloseItemDelegate.cpp \
	source/ui/AMDataSourcesEditor.cpp \
	source/beamline/AMBeamlineActionsList.cpp \
	source/ui/AMWrappingLineEdit.cpp \
	source/beamline/AMBeamlineControlSetMoveAction.cpp \
	source/ui/AMStartScreen.cpp \
	source/ui/AMSignallingGraphicsScene.cpp \
	source/dataman/AMUser.cpp \
	#deprecated: source/ui/AMVideoPlayerWidget.cpp \
	source/dataman/AMXESScan.cpp \
	source/dataman/ALSBL8XESDetectorInfo.cpp \
	source/dataman/ALSBL8XASFileLoader.cpp \
	source/ui/AMWindowPaneModel.cpp \
	source/dataman/AMScanEditorModelItem.cpp \
	source/dataman/AMDataSource.cpp \
	source/dataman/AMAxisInfo.cpp \
	source/dataman/AMNumber.cpp \
	source/dataman/AMnDIndex.cpp \
	source/dataman/AMDataStore.cpp \
	source/dataman/AMAnalysisBlock.cpp \
	source/dataman/AMDataSourceSeriesData.cpp \
	source/dataman/AMDataSourceImageData.cpp \
	source/analysis/AM1DExpressionAB.cpp \
	source/dataman/AMDbObjectSupport.cpp \
	source/acquaman.cpp \
	source/util/AMOrderedSetSignalSource.cpp \
	source/dataman/AMRawDataSource.cpp \
	source/analysis/AM1DExpressionABEditor.cpp \
	source/dataman/AMInMemoryDataStore.cpp \
	source/acquaman/AMFastScanConfiguration.cpp \
	source/ui/AMScanConfigurationViewer.cpp \
	source/dataman/AMFastScan.cpp \
	source/analysis/AM2DSummingAB.cpp \
	source/analysis/AMStandardAnalysisBlock.cpp \
	source/analysis/AM2DSummingABEditor.cpp \
	source/beamline/AMBeamlineParallelActionsList.cpp \
	source/acquaman/AMControlOptimization.cpp \
	source/acquaman/AMDetectorInfoList.cpp \
	source/ui/AMControlOptimizationView.cpp \
	source/dataman/SGM2010FastSensibleFileLoader.cpp \
	source/beamline/AMBeamlineControlStopAction.cpp \
	source/dataman/REIXS/REIXSXESRawFileLoader.cpp \
	source/util/AMDeferredFunctionCall.cpp \
	source/ui/AMScanConfigurationViewHolder.cpp \
	source/ui/AMPeriodicTableView.cpp \
	source/util/AMPeriodicTable.cpp \
	source/util/AMElement.cpp \
	source/dataman/AMSpectralOutputDetectorInfo.cpp \
	source/dataman/MCPDetectorInfo.cpp \
	source/dataman/PGTDetectorInfo.cpp \
	source/beamline/AMSingleControlDetector.cpp \
	source/beamline/AMSpectralOutputDetector.cpp \
	source/beamline/PGTDetector.cpp \
	source/beamline/MCPDetector.cpp \
	source/ui/AMDetectorViewSupport.cpp \
	source/ui/AMSingleControlDetectorView.cpp \
	source/ui/MCPDetectorView.cpp \
	source/ui/PGTDetectorView.cpp \
	source/ui/AMDetectorSetView.cpp \
	source/beamline/AMDetectorSet.cpp \
	source/dataman/AMROIInfo.cpp \
	source/beamline/AMROI.cpp \
	#deprecated: source/ui/AMVideoWidget.cpp \
	#source/beamline/AMBeamlineListAction.cpp
	source/ui/AMSamplePositionViewActionsWidget.cpp \
	source/beamline/AMBeamlineListAction.cpp \
	source/beamline/AMBeamlineControlWaitAction.cpp \
	source/beamline/AMBeamlineUserConfirmAction.cpp \
	source/ui/AMScanQueryModel.cpp \
	source/ui/AMFreeRunScanConfigurationViewHolder.cpp \
	source/dataman/AMExportController.cpp \
	source/dataman/AMExporterOption.cpp \
	source/dataman/AMExporterOptionGeneral.cpp \
	source/dataman/AMExporterOptionGeneralAscii.cpp \
	source/dataman/AMExporterGeneralAscii.cpp \
	source/ui/AMExportWizard.cpp \
	source/ui/AMFolderPathLineEdit.cpp \
	source/util/AMTagReplacementParser.cpp \
	source/ui/AMExporterOptionGeneralAsciiView.cpp \
	source/ui/AMTopFrame.cpp \
	source/dataman/AMExporter.cpp \
#	source/ui/AM3dDataSourceView.cpp \
	source/beamline/CLS/CLSVMEMotor.cpp \
	source/analysis/AM1DDerivativeAB.cpp \
	source/analysis/AMExternalScanDataSourceAB.cpp \
	source/analysis/AM1DSummingAB.cpp \
	source/analysis/AMDeadTimeAB.cpp \
	source/dataman/AMProcessVariableDataSource.cpp \
	source/ui/AMChooseScanDialog.cpp \
	source/application/AMDatamanAppController.cpp \
	source/ui/AMColorPickerButton.cpp

CONFIG(mobility) {
SOURCES +=	source/ui/AMOverlayVideoWidget.cpp \
	source/ui/AMCrosshairOverlayVideoWidget.cpp \
	source/ui/AMBeamlineCameraBrowser.cpp
}

RESOURCES = source/icons/icons.qrc \
	source/configurationFiles/configurationFiles.qrc \
	source/util/ElementData.qrc \
	source/stylesheets/stylesheets.qrc


OTHER_FILES += \
	source/stylesheets/sliderWaitLessThan.qss \
	source/stylesheets/sliderWaitGreaterThan.qss
