# #####################################################################
# QMake project file for acquaman.  		January 2010. mark.boots@usask.ca
# Note: Set EPICS_INCLUDE_DIRS, EPICS_LIB_DIR, VLC_*, and GSL_* correctly for platform
# ####################################################################

# Video Support: Remove this line if you do not have the multimedia module from QtMobility
CONFIG += mobility

# Debug: Uncomment this to build the program in debug mode (no optimizations; include debugging symbols.)
# Note that as of November 18, 2011, building in debug mode triggers a failure in the dacq library: the main (eV) PV ends up disabled in the dacq scan config.  This is likely a serious memory error.
# CONFIG += debug

# Automatically determines a user's home folder
HOME_FOLDER = $$system(echo $HOME)

macx {

		# Disable Qt Mobility Video until everyone's Mac laptops support that
		CONFIG -= mobility

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

		# Disable Qt Mobility Video until Darren's laptop is ready for that.
		CONFIG -= mobility

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
		DEV_PATH = beamline/programming

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

# Special build paths and options for running on the Jenkins auto-build server (currently at http://beamteam.usask.ca:8080)
CONFIG(jenkins_build) {

		message("Detected Jenkins auto-build... Specifying dependency paths for the build server.")

		# Disable Qt Mobility Video until the Jenkins-machine supports that
		CONFIG -= mobility

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = /home/mark/dev/epics/base/include \
				/home/mark/dev/epics/base/include/os/Linux
		EPICS_LIB_DIR = /home/mark/dev/epics/base/lib/linux-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = "/var/lib/jenkins/jobs/MPlotOnLinux_MasterBranch/workspace/src"
}


QT += core gui sql opengl network

# add video using Multimedia module from QtMobility, if we have it
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

# Set standard level of compiler warnings for everyone. (Otherwise the warnings shown will be system-dependent.)
QMAKE_CXXFLAGS += -Wextra

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

HEADERS += $$MPLOT_INCLUDE_DIR/MPlot/MPlot.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotAbstractTool.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotAxis.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotAxisScale.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotColorMap.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotImage.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotImageData.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotItem.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotLegend.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotMarker.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotPoint.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotRectangle.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotSeries.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotSeriesData.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotTools.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotWidget.h \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotMarkerTransparentVerticalRectangle.h \
	source/acquaman/AMAcqScanOutput.h \
	source/acquaman/AMAcqScanSpectrumOutput.h \
	source/acquaman/AMDacqScanController.h \
	source/acquaman/AMRegion.h \
	source/acquaman/AMRegionsList.h \
	source/acquaman/AMScanConfiguration.h \
	source/acquaman/AMScanController.h \
	source/acquaman/AMXASScanConfiguration.h \
	source/acquaman/dacq3_3/acqDataHandler.h \
	source/acquaman/dacq3_3/acqLibHelper.h \
	source/acquaman/dacq3_3/acquisitionLib.h \
	source/acquaman/dacq3_3/acquisitionLib.internal.h \
	source/acquaman/dacq3_3/acquisitionLib.main.h \
	source/acquaman/dacq3_3/displayAlias.h \
	source/acquaman/dacq3_3/epicsConnect.h \
	source/acquaman/dacq3_3/epicsConnect.main.h \
	source/acquaman/dacq3_3/factoryQtTemplate.h \
	source/acquaman/dacq3_3/OutputHandler/acqBaseOutput.h \
	source/acquaman/dacq3_3/OutputHandler/acqBaseStream.h \
	source/acquaman/dacq3_3/OutputHandler/acqFactory.h \
	source/acquaman/dacq3_3/OutputHandler/acqFileStream.h \
	source/acquaman/dacq3_3/OutputHandler/acqProperties.h \
	source/acquaman/dacq3_3/OutputHandler/acqTextOutput.h \
	source/acquaman/dacq3_3/OutputHandler/acqTextSpectrumOutput.h \
	source/acquaman/dacq3_3/qepicsacqclass.h \
	source/acquaman/dacq3_3/qepicsacqlocal.h \
	source/acquaman/dacq3_3/qepicsadvacq.h \
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
	source/ui/AMBottomBar.h \
	source/ui/acquaman/AMRegionsView.h \
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
	source/ui/AMSignallingGraphicsView.h \
	source/dataman/AMUser.h \
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
	source/dataman/AMFastScan.h \
	source/analysis/AM2DSummingAB.h \
	source/analysis/AMStandardAnalysisBlock.h \
	source/analysis/AM2DSummingABEditor.h \
	source/util/AMOrderedList.h \
	source/actions/AMBeamlineParallelActionsList.h \
	source/beamline/AMControlOptimization.h \
	source/dataman/info/AMDetectorInfoSet.h \
	source/ui/beamline/AMControlOptimizationView.h \
	source/actions/AMBeamlineControlStopAction.h \
	source/dataman/REIXS/REIXSXESRawFileLoader.h \
	source/util/AMDeferredFunctionCall.h \
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
	source/dataman/database/AMQueryTableModel.h \
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
	source/beamline/CLS/CLSMAXvMotor.h \
	source/analysis/AM1DDerivativeAB.h \
	source/beamline/AMHighVoltageChannel.h \
	source/beamline/CLS/CLSCAEN2527HVChannel.h \
	source/ui/CLS/CLSCAEN2527ChannelBasicView.h \
	source/actions/AMBeamlineHighVoltageChannelToggleAction.h \
	source/beamline/CLS/CLSPGT8000HVChannel.h \
	source/analysis/AM1DInterpolationAB.h \
	source/analysis/AM1DRunningAverageFilterAB.h \
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
	source/dataman/AMScanDictionary.h \
	source/dataman/AMScanParametersDictionary.h \
	source/dataman/AMScanExemplarDictionary.h \
	source/dataman/AMScanExemplar.h \
	source/ui/dataman/AMDictionaryLineEdit.h \
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
	source/ui/CLS/CLSSplitIonChamberView.h \
	source/application/AMAppControllerSupport.h \
	source/application/AMPluginsManager.h \
	source/dataman/import/AMScanDatabaseImportController.h \
	source/ui/dataman/AMScanDatabaseImportWizard.h \
	source/beamline/CLS/CLSMDriveMotorControl.h \
	source/ui/beamline/AMControlMoveButton.h \
	source/beamline/AMSampleManipulator.h \
	source/beamline/AMControlSetSampleManipulator.h \
	source/ui/CLS/CLSStopLightButton.h \
	source/acquaman/AMRegionScanConfiguration.h \
	source/acquaman/AMEXAFSScanConfiguration.h \
	source/beamline/CLS/CLSVariableIntegrationTime.h \
	source/beamline/CLS/CLSOMS58Motor.h \
	source/beamline/CLS/CLSSIS3820Scaler.h \
	source/ui/CLS/CLSSIS3820ScalerView.h \
	source/qjson/json_parser.hh \
	source/qjson/json_scanner.h \
	source/qjson/location.hh \
	source/qjson/parser_p.h \
	source/qjson/parser.h \
	source/qjson/parserrunnable.h \
	source/qjson/position.hh \
	source/qjson/qjson_debug.h \
	source/qjson/qjson_export.h \
	source/qjson/qobjecthelper.h \
	source/qjson/serializer.h \
	source/qjson/serializerrunnable.h \
	source/qjson/stack.hh \
	source/util/AMGithubManager.h \
    source/ui/util/AMGithubIssueSubmissionView.h \
    source/actions2/AMAction.h \
    source/actions2/AMListAction.h \
	source/actions2/AMActionInfo.h \
    source/actions2/AMActionRunner.h \
    source/ui/actions2/AMActionRunnerQueueView.h \
    source/ui/actions2/AMActionRunnerCurrentView.h \
    source/ui/actions2/AMWorkflowView.h \
	source/actions2/actions/AMWaitAction.h \
	source/actions2/actions/AMWaitActionInfo.h

CONFIG(mobility) {
DEFINES += AM_MOBILITY_VIDEO_ENABLED

HEADERS += source/ui/AMCrosshairOverlayVideoWidget.h \
	source/ui/AMOverlayVideoWidget.h \
	source/ui/AMBeamlineCameraWidget.h \
	source/ui/AMBeamlineCameraWidgetWithSourceTabs.h \
	source/ui/AMBeamlineCameraBrowser.h
}

FORMS += source/ui/dataman/AMDataView.ui \
	source/ui/dataman/AMDataViewEmptyHeader.ui \
	source/ui/dataman/AMDataViewSectionHeader.ui \
	source/ui/dataman/AMImportControllerWidget.ui \
	source/ui/acquaman/AMScanConfigurationView.ui \
	source/ui/AMBottomBar.ui \
	source/ui/dataman/AMGenericScanEditor.ui \
	source/ui/dataman/AMDataSourcesEditor.ui \
	source/ui/dataman/AMSamplePlateSelector.ui \
	source/ui/dataman/AMSamplePositionViewActionsWidget.ui \
	source/ui/dataman/AMExporterOptionGeneralAsciiView.ui \
	source/ui/dataman/AMDataViewActionsBar.ui \
	source/ui/dataman/AMChooseScanDialog.ui \
	source/ui/AMLinePropertyEditor.ui \
	source/ui/dataman/AMImagePropertyEditor.ui
SOURCES += $$MPLOT_INCLUDE_DIR/MPlot/MPlot.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotAbstractTool.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotAxis.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotAxisScale.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotColorMap.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotImage.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotImageData.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotItem.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotLegend.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotMarker.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotPoint.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotRectangle.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotSeries.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotSeriesData.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotTools.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotWidget.cpp \
	$$MPLOT_INCLUDE_DIR/MPlot/MPlotMarkerTransparentVerticalRectangle.cpp \
	source/acquaman/AMAcqScanOutput.cpp \
	source/acquaman/AMAcqScanSpectrumOutput.cpp \
	source/acquaman/AMDacqScanController.cpp \
	source/acquaman/AMRegion.cpp \
	source/acquaman/AMRegionsList.cpp \
	source/acquaman/AMScanConfiguration.cpp \
	source/acquaman/AMScanController.cpp \
	source/acquaman/AMXASScanConfiguration.cpp \
	source/acquaman/dacq3_3/acqAction.c \
	source/acquaman/dacq3_3/acqActSetup.c \
	source/acquaman/dacq3_3/acqExtern.c \
	source/acquaman/dacq3_3/acqLibHelper.c \
	source/acquaman/dacq3_3/acqLoad.c \
	source/acquaman/dacq3_3/acqMessage.c \
	source/acquaman/dacq3_3/acqMonitor.c \
	source/acquaman/dacq3_3/acqMotor.c \
	source/acquaman/dacq3_3/channel_hash.c \
	source/acquaman/dacq3_3/channel.c \
	source/acquaman/dacq3_3/connector.c \
	source/acquaman/dacq3_3/displayAlias.cpp \
	source/acquaman/dacq3_3/macro.c \
	source/acquaman/dacq3_3/OutputHandler/acqBaseOutput.cpp \
	source/acquaman/dacq3_3/OutputHandler/acqBaseStream.cpp \
	source/acquaman/dacq3_3/OutputHandler/acqFactory.cpp \
	source/acquaman/dacq3_3/OutputHandler/acqFileStream.cpp \
	source/acquaman/dacq3_3/OutputHandler/acqTextOutput.cpp \
	source/acquaman/dacq3_3/OutputHandler/acqTextSpectrumOutput.cpp \
	source/acquaman/dacq3_3/qepicsacqclass.cpp \
	source/acquaman/dacq3_3/qepicsacqlocal.cpp \
	source/acquaman/dacq3_3/qepicsadvacq.cpp \
	source/acquaman/dacq3_3/update.c \
	source/acquaman/dacq3_3/xmlRead.cpp \
	source/acquaman/dacq3_3/xmlWrite.cpp \
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
	source/ui/AMBottomBar.cpp \
	source/ui/acquaman/AMRegionsView.cpp \
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
	source/ui/AMSignallingGraphicsView.cpp \
	source/dataman/AMUser.cpp \
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
	source/dataman/AMFastScan.cpp \
	source/analysis/AM2DSummingAB.cpp \
	source/analysis/AMStandardAnalysisBlock.cpp \
	source/analysis/AM2DSummingABEditor.cpp \
	source/actions/AMBeamlineParallelActionsList.cpp \
	source/beamline/AMControlOptimization.cpp \
	source/dataman/info/AMDetectorInfoSet.cpp \
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
	source/ui/dataman/AMSamplePositionViewActionsWidget.cpp \
	source/actions/AMBeamlineListAction.cpp \
	source/actions/AMBeamlineControlWaitAction.cpp \
	source/actions/AMBeamlineUserConfirmAction.cpp \
	source/dataman/database/AMQueryTableModel.cpp \
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
	source/beamline/CLS/CLSMAXvMotor.cpp \
	source/analysis/AM1DDerivativeAB.cpp \
	source/beamline/AMHighVoltageChannel.cpp \
	source/beamline/CLS/CLSCAEN2527HVChannel.cpp \
	source/ui/CLS/CLSCAEN2527ChannelBasicView.cpp \
	source/actions/AMBeamlineHighVoltageChannelToggleAction.cpp \
	source/beamline/CLS/CLSPGT8000HVChannel.cpp \
	source/analysis/AM1DInterpolationAB.cpp \
	source/analysis/AM1DRunningAverageFilterAB.cpp \
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
	source/dataman/AMScanDictionary.cpp \
	source/dataman/AMScanParametersDictionary.cpp \
	source/dataman/AMScanExemplarDictionary.cpp \
	source/dataman/AMScanExemplar.cpp \
	source/ui/dataman/AMDictionaryLineEdit.cpp \
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
	source/ui/CLS/CLSSplitIonChamberView.cpp \
	source/application/AMPluginsManager.cpp \
	source/application/AMAppControllerSupport.cpp \
	source/dataman/import/AMScanDatabaseImportController.cpp \
	source/ui/dataman/AMScanDatabaseImportWizard.cpp \
	source/beamline/CLS/CLSMDriveMotorControl.cpp \
	source/ui/beamline/AMControlMoveButton.cpp \
	source/beamline/AMControlSetSampleManipulator.cpp \
	source/ui/CLS/CLSStopLightButton.cpp \
	source/acquaman/AMRegionScanConfiguration.cpp \
	source/acquaman/AMEXAFSScanConfiguration.cpp \
	source/beamline/CLS/CLSVariableIntegrationTime.cpp \
	source/beamline/CLS/CLSSIS3820Scaler.cpp \
	source/ui/CLS/CLSSIS3820ScalerView.cpp \
	source/qjson/json_parser.cc \
	source/qjson/json_scanner.cpp \
	source/qjson/parser.cpp \
	source/qjson/parserrunnable.cpp \
	source/qjson/qobjecthelper.cpp \
	source/qjson/serializer.cpp \
	source/qjson/serializerrunnable.cpp \
	source/util/AMGithubManager.cpp \
    source/ui/util/AMGithubIssueSubmissionView.cpp \
    source/actions2/AMAction.cpp \
    source/actions2/AMListAction.cpp \
    source/actions2/AMActionInfo.cpp \
    source/actions2/AMActionRunner.cpp \
    source/ui/actions2/AMActionRunnerQueueView.cpp \
    source/ui/actions2/AMActionRunnerCurrentView.cpp \
    source/ui/actions2/AMWorkflowView.cpp \
	source/actions2/actions/AMWaitAction.cpp \
	source/actions2/actions/AMWaitActionInfo.cpp

CONFIG(mobility) {
SOURCES +=	source/ui/AMOverlayVideoWidget.cpp \
		source/ui/AMCrosshairOverlayVideoWidget.cpp \
		source/ui/AMBeamlineCameraWidget.cpp \
		source/ui/AMBeamlineCameraWidgetWithSourceTabs.cpp \
		source/ui/AMBeamlineCameraBrowser.cpp
}

RESOURCES = source/icons/icons.qrc \
		source/configurationFiles/configurationFiles.qrc \
		source/util/ElementData.qrc \
		source/stylesheets/stylesheets.qrc


OTHER_FILES += \
	source/stylesheets/sliderWaitLessThan.qss \
	source/stylesheets/sliderWaitGreaterThan.qss














