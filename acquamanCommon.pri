# #####################################################################
# QMake project file for acquaman.  		January 2010. mark.boots@usask.ca
# Note: Set EPICS_INCLUDE_DIRS, EPICS_LIB_DIR, VLC_*, and GSL_* correctly for platform
# ####################################################################

# Video Support: Remove this line if you do not have the multimedia module from QtMobility

#CONFIG += mobility

# Automatically determines a user's home folder
HOME_FOLDER = $$system(echo $HOME)

macx {

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = dev

		# EPICS Dependencies:
				EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/include \
								$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/include/os/Darwin
				EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/lib/darwin-x86

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
linux-g++-32 {

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
		DEV_PATH = Sandbox/Acquaman2011/dev

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = /home/epics/src/R3.14.12/base/include \
				/home/epics/src/R3.14.12/base/include/os/Linux
		EPICS_LIB_DIR = /home/epics/src/R3.14.12/base/lib/linux-x86_64

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/src

		# GSL Dependencies
		GSL_INCLUDE_DIR = /home/beamline/tools/gsl/gsl-1.14-install/include
		GSL_LIB = -L/home/beamline/tools/gsl/gsl-1.14-install/lib -lgsl
		GSL_CBLAS_LIB = -lgslcblas

		# LibXML Dependencies (required by dacq library)
		XML_LIB = -lxml2
		XML_INCLUDE_DIR = /usr/include/libxml2
}

QT += core gui sql opengl

# video using Multimedia module from QtMobility, if we have it. (This will only be activated if you set the CONFIG += mobility line at the top of this file)
CONFIG(mobility) {
		MOBILITY += multimedia
}

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

				contains(QT_MINOR_VERSION, 7) {
						contains(QT_PATCH_VERSION, 2) | contains(QT_PATCH_VERSION, 3) | contains(QT_PATCH_VERSION, 4) {
								# 4.7.2 or 4.7.3 or 4.7.4: Use same as linux-g++
#		QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$EPICS_LIB_DIR,-rpath,$$QWTPLOT3D_LIB_DIR"
#		QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$EPICS_LIB_DIR,-rpath,$$QWTPLOT3D_LIB_DIR"
								QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$EPICS_LIB_DIR"
								QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$EPICS_LIB_DIR"
						}
						else {
								QMAKE_LFLAGS_RPATH += "$$EPICS_LIB_DIR"
								#QMAKE_LFLAGS_RPATH += "$$QWTPLOT3D_LIB_DIR"
						}
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
linux-g++-32 {
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


# Source Files (Acquaman Framework Common)
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
	source/actions/AMBeamlineActionItem.h \
	source/actions/AMBeamlineControlAction.h \
	source/beamline/AMControl.h \
	source/beamline/AMControlSet.h \
	source/beamline/AMControlState.h \
	source/beamline/AMDetector.h \
	source/beamline/AMProcessVariable.h \
	source/beamline/AMPVNames.h \
	source/dataman/database/AMDatabase.h \
	source/dataman/database/AMDbObject.h \
	source/dataman/info/AMDetectorInfo.h \
	source/dataman/AMExperiment.h \
	source/dataman/AMFirstTimeController.h \
	source/dataman/AMImportController.h \
	source/dataman/AMRun.h \
	source/dataman/AMSample.h \
	source/dataman/AMScan.h \
	source/dataman/AMScanSetModel.h \
	source/dataman/AMXASScan.h \
	source/dataman/SGM/SGM2004FileLoader.h \
	source/dataman/SGM/SGM2010FastFileLoader.h \
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
	source/ui/dataman/AMRunSelector.h \
	source/ui/beamline/AMControlSetView.h \
	source/ui/dataman/AMCramBarHorizontal.h \
	source/ui/dataman/AMDataView.h \
	source/ui/dataman/AMDataViewWithActionButtons.h \
	source/ui/dataman/AMFirstTimeWidget.h \
	source/ui/dataman/AMFlowGraphicsLayout.h \
	source/ui/dataman/AMImportControllerWidget.h \
	source/ui/AMMainWindow.h \
	source/ui/acquaman/AMRegionsLineView.h \
	source/ui/dataman/AMRunExperimentInsert.h \
	source/ui/acquaman/AMScanConfigurationView.h \
	source/ui/dataman/AMScanView.h \
	source/ui/AMSidebar.h \
	source/ui/AMStatusView.h \
	source/ui/AMThumbnailScrollViewer.h \
	source/ui/acquaman/AMRegionsView.h \
	source/ui/BottomBar.h \
	#deprecated: source/ui/AMBeamlineCameraWidget.h \
	source/ui/beamline/AMControlEditor.h \
	source/acquaman.h \
	source/ui/dataman/AMNewRunDialog.h \
	source/ui/beamline/AMDetectorView.h \
	source/ui/AMWorkflowManagerView.h \
	source/actions/AMBeamlineScanAction.h \
	source/dataman/AMAbstractFileLoader.h \
	source/actions/AMBeamlineControlMoveAction.h \
	source/ui/AMPrefixSuffixLineEdit.h \
	source/ui/AMDragDropItemModel.h \
	source/dataman/AMRunExperimentItems.h \
	source/ui/dataman/AMSampleManagementWidget.h \
	source/ui/dataman/AMSampleManipulatorView.h \
	source/ui/dataman/AMSamplePlateView.h \
	source/dataman/info/AMControlInfoList.h \
	source/dataman/AMSamplePlate.h \
	source/ui/dataman/AMGenericScanEditor.h \
	source/ui/AMDetailedItemDelegate.h \
	source/ui/AMVerticalStackWidget.h \
	source/ui/AMHeaderButton.h \
	source/ui/dataman/AMSampleEditor.h \
	source/util/AMDateTimeUtils.h \
	source/ui/AMElementListEdit.h \
	source/ui/AMCloseItemDelegate.h \
	source/ui/dataman/AMDataSourcesEditor.h \
	source/actions/AMBeamlineActionsList.h \
	source/ui/AMWrappingLineEdit.h \
	source/actions/AMBeamlineControlSetMoveAction.h \
	source/ui/AMStartScreen.h \
	source/ui/AMSignallingGraphicsScene.h \
	source/dataman/AMUser.h \
	#deprecated: source/ui/AMVideoPlayerWidget.h \
	source/dataman/AMXESScan.h \
	source/dataman/info/ALSBL8XESDetectorInfo.h \
	source/dataman/ALSBL8XASFileLoader.h \
	source/ui/AMWindowPaneModel.h \
	source/dataman/AMScanEditorModelItem.h \
	source/dataman/datasource/AMDataSource.h \
	source/dataman/AMAxisInfo.h \
	source/dataman/AMNumber.h \
	source/dataman/AMnDIndex.h \
	source/dataman/datastore/AMDataStore.h \
	source/dataman/AMAnalysisBlock.h \
	source/dataman/datasource/AMDataSourceSeriesData.h \
	source/dataman/datasource/AMDataSourceImageData.h \
	source/analysis/AM1DExpressionAB.h \
	source/dataman/database/AMDbObjectSupport.h \
	source/util/AMOrderedSetSignalSource.h \
	source/util/AMOrderedSet.h \
	source/dataman/datasource/AMRawDataSource.h \
	source/analysis/AM1DExpressionABEditor.h \
	source/dataman/AMMeasurementInfo.h \
	source/dataman/datastore/AMInMemoryDataStore.h \
	source/acquaman/AMFastScanConfiguration.h \
	source/ui/AMScanConfigurationViewer.h \
	source/dataman/AMFastScan.h \
	source/analysis/AM2DSummingAB.h \
	source/analysis/AMStandardAnalysisBlock.h \
	source/analysis/AM2DSummingABEditor.h \
	source/util/AMOrderedList.h \
	source/actions/AMBeamlineParallelActionsList.h \
	source/beamline/AMControlOptimization.h \
	source/dataman/info/AMDetectorInfoList.h \
	source/ui/beamline/AMControlOptimizationView.h \
	source/actions/AMBeamlineControlStopAction.h \
	source/dataman/REIXS/REIXSXESRawFileLoader.h \
	source/util/AMDeferredFunctionCall.h \
	#deprecated: source/ui/AMVideoWidget.h \
	source/ui/acquaman/AMScanConfigurationViewHolder.h \
	source/ui/util/AMPeriodicTableView.h \
	source/util/AMPeriodicTable.h \
	source/util/AMElement.h \
	source/dataman/info/AMSpectralOutputDetectorInfo.h \
	source/dataman/info/MCPDetectorInfo.h \
	source/dataman/info/PGTDetectorInfo.h \
	source/beamline/AMSingleControlDetector.h \
	source/beamline/AMSpectralOutputDetector.h \
	source/beamline/PGTDetector.h \
	source/beamline/MCPDetector.h \
	source/ui/beamline/AMDetectorViewSupport.h \
	source/ui/beamline/AMSingleControlDetectorView.h \
	source/ui/beamline/MCPDetectorView.h \
	source/ui/beamline/PGTDetectorView.h \
	source/ui/beamline/AMDetectorSetView.h \
	source/beamline/AMDetectorSet.h \
	source/dataman/info/AMROIInfo.h \
	source/beamline/AMROI.h \
	source/ui/dataman/AMSamplePositionViewActionsWidget.h \
	source/actions/AMBeamlineListAction.h \
	source/actions/AMBeamlineControlWaitAction.h \
	source/actions/AMBeamlineUserConfirmAction.h \
	source/ui/dataman/AMScanQueryModel.h \
	source/dataman/export/AMExportController.h \
	source/dataman/export/AMExporter.h \
	source/dataman/export/AMExporterOption.h \
	source/dataman/export/AMExporterOptionGeneral.h \
	source/dataman/export/AMExporterOptionGeneralAscii.h \
	source/dataman/export/AMExporterGeneralAscii.h \
	source/ui/dataman/AMExportWizard.h \
	source/ui/AMFolderPathLineEdit.h \
	source/util/AMTagReplacementParser.h \
	source/ui/dataman/AMExporterOptionGeneralAsciiView.h \
#	source/ui/dataman/AM3dDataSourceView.h \
	source/ui/AMTopFrame.h \
	source/actions/AMBeamlineSamplePlateMoveAction.h \
	source/actions/AMBeamlineFiducializationMoveAction.h \
	source/dataman/info/OceanOptics65000DetectorInfo.h \
	source/beamline/OceanOptics65000Detector.h \
	source/ui/beamline/OceanOptics65000DetectorView.h \
	source/dataman/SGM/SGM2011XASFileLoader.h \
	source/beamline/CLS/CLSVMEMotor.h \
	source/analysis/AM1DDerivativeAB.h \
	source/beamline/AMHighVoltageChannel.h \
	source/beamline/CLS/CLSCAEN2527HVChannel.h \
	source/ui/CLS/CLSCAEN2527ChannelBasicView.h \
	source/actions/AMBeamlineHighVoltageChannelToggleAction.h \
	source/beamline/CLS/CLSPGT8000HVChannel.h \
	source/analysis/AM1DInterpolationAB.h \
	source/analysis/AM1DRunningAverageFilterAB.h \
	source/analysis/SGM/SGM1DFastScanFilterAB.h \
	source/analysis/AMExternalScanDataSourceAB.h \
	source/analysis/AM1DSummingAB.h \
	source/analysis/AMDeadTimeAB.h \
	source/dataman/datasource/AMProcessVariableDataSource.h \
	source/ui/dataman/AMChooseScanDialog.h \
	source/application/AMDatamanAppController.h \
	source/ui/dataman/AMColorPickerButton.h \
	source/ui/dataman/AMScanSetItemPropertyDialog.h \
	source/ui/dataman/AMColoredTextToolButton.h \
	source/ui/AMLinePropertyEditor.h \
	source/ui/dataman/AMImagePropertyEditor.h \
	source/beamline/CLS/CLSSynchronizedDwellTime.h \
	source/dataman/datasource/AMXYScatterPVDataSource.h \
	source/beamline/AMCompositeControl.h \
	source/ui/CLS/CLSSynchronizedDwellTimeView.h \
	source/analysis/AM2DDeadTimeAB.h \
	source/dataman/VESPERS/VESPERSXASDataLoader.h \
	source/dataman/AMFileLoaderInterface.h \
	source/ui/util/AMSettingsView.h \
    source/beamline/AMIonChamber.h \
    source/dataman/info/AMIonChamberInfo.h \
    source/beamline/CLS/CLSIonChamber.h \
    source/beamline/CLS/CLSSR570.h \
    source/ui/beamline/AMIonChamberView.h \
    source/ui/CLS/CLSIonChamberView.h \
    source/beamline/CLS/CLSBiStateControl.h \
    source/beamline/AMSplitIonChamber.h \
    source/beamline/CLS/CLSSplitIonChamber.h \
    source/ui/beamline/AMSplitIonChamberView.h \
    source/ui/CLS/CLSSplitIonChamberView.h

CONFIG(mobility) {
HEADERS += source/ui/AMCrosshairOverlayVideoWidget.h \
	source/ui/AMOverlayVideoWidget.h \
	source/ui/AMBeamlineCameraBrowser.h
}

FORMS += source/ui/dataman/AMDataView.ui \
	source/ui/dataman/AMDataViewEmptyHeader.ui \
	source/ui/dataman/AMDataViewSectionHeader.ui \
	source/ui/dataman/AMImportControllerWidget.ui \
	source/ui/acquaman/AMScanConfigurationView.ui \
	source/ui/BottomBar.ui \
	source/ui/dataman/AMGenericScanEditor.ui \
	source/ui/dataman/AMDataSourcesEditor.ui \
	source/ui/dataman/AMSamplePlateSelector.ui \
	source/ui/dataman/AMSamplePositionViewActionsWidget.ui \
	source/ui/dataman/AMExporterOptionGeneralAsciiView.ui \
	source/ui/dataman/AMDataViewActionsBar.ui \
	source/ui/dataman/AMChooseScanDialog.ui \
	source/ui/AMLinePropertyEditor.ui \
	source/ui/dataman/AMImagePropertyEditor.ui
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
	source/actions/AMBeamlineActionItem.cpp \
	source/actions/AMBeamlineControlAction.cpp \
	source/beamline/AMControl.cpp \
	source/beamline/AMControlSet.cpp \
	source/beamline/AMControlState.cpp \
	source/beamline/AMProcessVariable.cpp \
	source/beamline/AMPVNames.cpp \
	source/dataman/database/AMDatabase.cpp \
	source/dataman/database/AMDbObject.cpp \
	source/dataman/info/AMDetectorInfo.cpp \
	source/dataman/AMExperiment.cpp \
	source/dataman/AMFirstTimeController.cpp \
	source/dataman/AMImportController.cpp \
	source/dataman/AMRun.cpp \
	source/dataman/AMSample.cpp \
	source/dataman/AMScan.cpp \
	source/dataman/AMScanSetModel.cpp \
	source/dataman/AMXASScan.cpp \
	source/dataman/SGM/SGM2004FileLoader.cpp \
	source/dataman/SGM/SGM2010FastFileLoader.cpp \
	source/muParser/muParser.cpp \
	source/muParser/muParserBase.cpp \
	source/muParser/muParserBytecode.cpp \
	source/muParser/muParserCallback.cpp \
	source/muParser/muParserError.cpp \
	source/muParser/muParserTokenReader.cpp \
	source/ui/dataman/AMRunSelector.cpp \
	source/ui/beamline/AMControlSetView.cpp \
	source/ui/dataman/AMCramBarHorizontal.cpp \
	source/ui/dataman/AMDataView.cpp \
	source/ui/dataman/AMDataViewWithActionButtons.cpp \
	source/ui/dataman/AMFlowGraphicsLayout.cpp \
	source/ui/AMMainWindow.cpp \
	source/ui/acquaman/AMRegionsLineView.cpp \
	source/ui/dataman/AMRunExperimentInsert.cpp \
	source/ui/dataman/AMScanView.cpp \
	source/ui/AMSidebar.cpp \
	source/ui/AMStatusView.cpp \
	source/ui/AMThumbnailScrollViewer.cpp \
	source/ui/acquaman/AMRegionsView.cpp \
	source/ui/BottomBar.cpp \
	#deprecated: source/ui/AMBeamlineCameraWidget.cpp \
	source/ui/beamline/AMControlEditor.cpp \
	source/ui/beamline/AMDetectorView.cpp \
	source/ui/dataman/AMNewRunDialog.cpp \
	source/ui/AMWorkflowManagerView.cpp \
	source/actions/AMBeamlineScanAction.cpp \
	source/actions/AMBeamlineControlMoveAction.cpp \
	source/ui/AMPrefixSuffixLineEdit.cpp \
	source/ui/AMDragDropItemModel.cpp \
	source/dataman/AMRunExperimentItems.cpp \
	source/ui/dataman/AMSampleManagementWidget.cpp \
	source/ui/dataman/AMSampleManipulatorView.cpp \
	source/ui/dataman/AMSamplePlateView.cpp \
	source/dataman/info/AMControlInfoList.cpp \
	source/dataman/AMSamplePlate.cpp \
	source/ui/dataman/AMGenericScanEditor.cpp \
	source/ui/AMDetailedItemDelegate.cpp \
	source/ui/AMVerticalStackWidget.cpp \
	source/ui/AMHeaderButton.cpp \
	source/ui/dataman/AMSampleEditor.cpp \
	source/util/AMDateTimeUtils.cpp \
	source/ui/AMElementListEdit.cpp \
	source/ui/AMCloseItemDelegate.cpp \
	source/ui/dataman/AMDataSourcesEditor.cpp \
	source/actions/AMBeamlineActionsList.cpp \
	source/ui/AMWrappingLineEdit.cpp \
	source/actions/AMBeamlineControlSetMoveAction.cpp \
	source/ui/AMStartScreen.cpp \
	source/ui/AMSignallingGraphicsScene.cpp \
	source/dataman/AMUser.cpp \
	#deprecated: source/ui/AMVideoPlayerWidget.cpp \
	source/dataman/AMXESScan.cpp \
	source/dataman/info/ALSBL8XESDetectorInfo.cpp \
	source/dataman/ALSBL8XASFileLoader.cpp \
	source/ui/AMWindowPaneModel.cpp \
	source/dataman/AMScanEditorModelItem.cpp \
	source/dataman/datasource/AMDataSource.cpp \
	source/dataman/AMAxisInfo.cpp \
	source/dataman/AMNumber.cpp \
	source/dataman/AMnDIndex.cpp \
	source/dataman/datastore/AMDataStore.cpp \
	source/dataman/AMAnalysisBlock.cpp \
	source/dataman/datasource/AMDataSourceSeriesData.cpp \
	source/dataman/datasource/AMDataSourceImageData.cpp \
	source/analysis/AM1DExpressionAB.cpp \
	source/dataman/database/AMDbObjectSupport.cpp \
	source/acquaman.cpp \
	source/util/AMOrderedSetSignalSource.cpp \
	source/dataman/datasource/AMRawDataSource.cpp \
	source/analysis/AM1DExpressionABEditor.cpp \
	source/dataman/datastore/AMInMemoryDataStore.cpp \
	source/acquaman/AMFastScanConfiguration.cpp \
	source/ui/AMScanConfigurationViewer.cpp \
	source/dataman/AMFastScan.cpp \
	source/analysis/AM2DSummingAB.cpp \
	source/analysis/AMStandardAnalysisBlock.cpp \
	source/analysis/AM2DSummingABEditor.cpp \
	source/actions/AMBeamlineParallelActionsList.cpp \
	source/beamline/AMControlOptimization.cpp \
	source/dataman/info/AMDetectorInfoList.cpp \
	source/ui/beamline/AMControlOptimizationView.cpp \
	source/actions/AMBeamlineControlStopAction.cpp \
	source/dataman/REIXS/REIXSXESRawFileLoader.cpp \
	source/util/AMDeferredFunctionCall.cpp \
	source/ui/acquaman/AMScanConfigurationViewHolder.cpp \
	source/ui/util/AMPeriodicTableView.cpp \
	source/util/AMPeriodicTable.cpp \
	source/util/AMElement.cpp \
	source/dataman/info/AMSpectralOutputDetectorInfo.cpp \
	source/dataman/info/MCPDetectorInfo.cpp \
	source/dataman/info/PGTDetectorInfo.cpp \
	source/beamline/AMSingleControlDetector.cpp \
	source/beamline/AMSpectralOutputDetector.cpp \
	source/beamline/PGTDetector.cpp \
	source/beamline/MCPDetector.cpp \
	source/ui/beamline/AMDetectorViewSupport.cpp \
	source/ui/beamline/AMSingleControlDetectorView.cpp \
	source/ui/beamline/MCPDetectorView.cpp \
	source/ui/beamline/PGTDetectorView.cpp \
	source/ui/beamline/AMDetectorSetView.cpp \
	source/beamline/AMDetectorSet.cpp \
	source/dataman/info/AMROIInfo.cpp \
	source/beamline/AMROI.cpp \
	#deprecated: source/ui/AMVideoWidget.cpp \
	#source/beamline/AMBeamlineListAction.cpp
	source/ui/dataman/AMSamplePositionViewActionsWidget.cpp \
	source/actions/AMBeamlineListAction.cpp \
	source/actions/AMBeamlineControlWaitAction.cpp \
	source/actions/AMBeamlineUserConfirmAction.cpp \
	source/ui/dataman/AMScanQueryModel.cpp \
	source/dataman/export/AMExportController.cpp \
	source/dataman/export/AMExporterOption.cpp \
	source/dataman/export/AMExporterOptionGeneral.cpp \
	source/dataman/export/AMExporterOptionGeneralAscii.cpp \
	source/dataman/export/AMExporterGeneralAscii.cpp \
	source/ui/dataman/AMExportWizard.cpp \
	source/ui/AMFolderPathLineEdit.cpp \
	source/util/AMTagReplacementParser.cpp \
	source/ui/dataman/AMExporterOptionGeneralAsciiView.cpp \
	source/ui/AMTopFrame.cpp \
	source/dataman/export/AMExporter.cpp \
#	source/ui/dataman/AM3dDataSourceView.cpp \
	source/actions/AMBeamlineSamplePlateMoveAction.cpp \
	source/actions/AMBeamlineFiducializationMoveAction.cpp \
	source/dataman/info/OceanOptics65000DetectorInfo.cpp \
	source/beamline/OceanOptics65000Detector.cpp \
	source/ui/beamline/OceanOptics65000DetectorView.cpp \
	source/dataman/SGM/SGM2011XASFileLoader.cpp \
	source/beamline/CLS/CLSVMEMotor.cpp \
	source/analysis/AM1DDerivativeAB.cpp \
	source/beamline/AMHighVoltageChannel.cpp \
	source/beamline/CLS/CLSCAEN2527HVChannel.cpp \
	source/ui/CLS/CLSCAEN2527ChannelBasicView.cpp \
	source/actions/AMBeamlineHighVoltageChannelToggleAction.cpp \
	source/beamline/CLS/CLSPGT8000HVChannel.cpp \
	source/analysis/AM1DInterpolationAB.cpp \
	source/analysis/AM1DRunningAverageFilterAB.cpp \
	source/analysis/SGM/SGM1DFastScanFilterAB.cpp \
	source/analysis/AMExternalScanDataSourceAB.cpp \
	source/analysis/AM1DSummingAB.cpp \
	source/analysis/AMDeadTimeAB.cpp \
	source/dataman/datasource/AMProcessVariableDataSource.cpp \
	source/ui/dataman/AMChooseScanDialog.cpp \
	source/application/AMDatamanAppController.cpp \
	source/ui/dataman/AMColorPickerButton.cpp \
	source/ui/dataman/AMScanSetItemPropertyDialog.cpp \
	source/ui/dataman/AMColoredTextToolButton.cpp \
	source/ui/AMLinePropertyEditor.cpp \
	source/ui/dataman/AMImagePropertyEditor.cpp \
	source/beamline/CLS/CLSSynchronizedDwellTime.cpp \
	source/dataman/datasource/AMXYScatterPVDataSource.cpp \
	source/beamline/AMCompositeControl.cpp \
	source/ui/CLS/CLSSynchronizedDwellTimeView.cpp \
	source/analysis/AM2DDeadTimeAB.cpp \
	source/dataman/VESPERS/VESPERSXASDataLoader.cpp \
	source/ui/util/AMSettingsView.cpp \
    source/beamline/AMIonChamber.cpp \
    source/dataman/info/AMIonChamberInfo.cpp \
    source/beamline/CLS/CLSIonChamber.cpp \
    source/beamline/CLS/CLSSR570.cpp \
    source/ui/beamline/AMIonChamberView.cpp \
    source/ui/CLS/CLSIonChamberView.cpp \
    source/beamline/CLS/CLSBiStateControl.cpp \
    source/beamline/AMSplitIonChamber.cpp \
    source/beamline/CLS/CLSSplitIonChamber.cpp \
    source/ui/beamline/AMSplitIonChamberView.cpp \
    source/ui/CLS/CLSSplitIonChamberView.cpp

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






















