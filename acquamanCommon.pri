# #####################################################################
# QMake project file for acquaman.  		October 2014. david.k.chevrier@gmail.com
# ####################################################################

# Video Support: Remove this line if you do not have the multimedia module from QtMobility
CONFIG *= mobility

include ( $$PATH_TO_AM/compositeCommon/AMCommon.pri )

QT *= network

# Debug: Uncomment this to build the program in debug mode (no optimizations; include debugging symbols.)
# Note that as of November 18, 2011, building in debug mode triggers a failure in the dacq library: the main (eV) PV ends up disabled in the dacq scan config.  This is likely a serious memory error.
# CONFIG += debug

include ( $$PATH_TO_AM/clsFileLoader.pri )

include ( $$PATH_TO_AM/compositeCommon/AMMPlot.pri )

include ( $$PATH_TO_AM/compositeCommon/AMMobility.pri )
include ( $$PATH_TO_AM/compositeCommon/AMCDF.pri )

include ( $$PATH_TO_AM/compositeCommon/Eigen.pri )
include ( $$PATH_TO_AM/compositeCommon/MuParser.pri )
include ( $$PATH_TO_AM/compositeCommon/QJSON.pri )

include ( $$PATH_TO_AM/compositeCommon/AMBeamline.pri )
include ( $$PATH_TO_AM/compositeCommon/AMPVControl.pri )

# Source Files (Acquaman Framework Common)
#######################

HEADERS += \
	source/acquaman/AMScanConfiguration.h \
	source/acquaman/AMScanController.h \
	source/application/AMAppController.h \
	source/util/AMBiHash.h \
	source/util/AMSettings.h \
	source/beamline/AMPVNames.h \
	source/dataman/AMExperiment.h \
	source/dataman/AMImportController.h \
	source/dataman/AMRun.h \
	source/dataman/AMScan.h \
	source/dataman/AMScanSetModel.h \
	source/dataman/AMXASScan.h \
	source/ui/dataman/AMRunSelector.h \
	source/ui/beamline/AMControlSetView.h \
	source/ui/dataman/AMCramBarHorizontal.h \
	source/ui/dataman/AMFirstTimeWidget.h \
	source/ui/dataman/AMImportControllerWidget.h \
	source/ui/AMMainWindow.h \
	source/ui/dataman/AMRunExperimentInsert.h \
	source/ui/acquaman/AMScanConfigurationView.h \
	source/ui/dataman/AMScanView.h \
	source/ui/AMSidebar.h \
	source/ui/AMStatusView.h \
	source/ui/AMThumbnailScrollViewer.h \
	source/ui/beamline/AMControlEditor.h \
	source/ui/dataman/AMNewRunDialog.h \
	source/dataman/AMAbstractFileLoader.h \
	source/ui/AMPrefixSuffixLineEdit.h \
	source/ui/AMDragDropItemModel.h \
	source/dataman/AMRunExperimentItems.h \
	source/ui/dataman/AMGenericScanEditor.h \
	source/ui/AMDetailedItemDelegate.h \
	source/ui/AMVerticalStackWidget.h \
	source/ui/AMHeaderButton.h \
	source/util/AMDateTimeUtils.h \
	source/ui/AMElementListEdit.h \
	source/ui/AMCloseItemDelegate.h \
	source/ui/dataman/AMDataSourcesEditor.h \
	source/ui/AMWrappingLineEdit.h \
	source/ui/AMStartScreen.h \
	source/dataman/AMUser.h \
	source/dataman/AMXESScan.h \
	source/dataman/info/ALSBL8XESDetectorInfo.h \
	source/dataman/ALSBL8XASFileLoader.h \
	source/ui/AMWindowPaneModel.h \
	source/dataman/AMScanEditorModelItem.h \
	source/dataman/datastore/AMDataStore.h \
	source/dataman/AMAnalysisBlock.h \
	source/dataman/datasource/AMDataSourceSeriesData.h \
	source/dataman/datasource/AMDataSourceImageData.h \
	source/analysis/AM1DExpressionAB.h \
	source/dataman/datasource/AMRawDataSource.h \
	source/analysis/AM1DExpressionABEditor.h \
	source/dataman/AMMeasurementInfo.h \
	source/dataman/datastore/AMInMemoryDataStore.h \
	source/acquaman/AMFastScanConfiguration.h \
	source/dataman/AMFastScan.h \
	source/analysis/AM2DSummingAB.h \
	source/analysis/AMStandardAnalysisBlock.h \
	source/analysis/AM2DSummingABEditor.h \
	source/ui/util/AMPeriodicTableView.h \
	source/util/AMPeriodicTable.h \
	source/util/AMElement.h \
	source/dataman/info/AMSpectralOutputDetectorInfo.h \
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
	source/ui/AMTopFrame.h \
	source/analysis/AM1DDerivativeAB.h \
	source/analysis/AM1DInterpolationAB.h \
	source/analysis/AM1DRunningAverageFilterAB.h \
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
	source/dataman/datasource/AMXYScatterPVDataSource.h \
	source/beamline/AMCompositeControl.h \
	source/dataman/AMFileLoaderInterface.h \
	source/ui/util/AMSettingsView.h \
	source/dataman/AMScanDictionary.h \
	source/dataman/AMScanParametersDictionary.h \
	source/dataman/AMScanExemplarDictionary.h \
	source/dataman/AMScanExemplar.h \
	source/ui/dataman/AMDictionaryLineEdit.h \
	source/application/AMAppControllerSupport.h \
	source/application/AMPluginsManager.h \
	source/dataman/import/AMScanDatabaseImportController.h \
	source/ui/dataman/AMScanDatabaseImportWizard.h \
	source/ui/beamline/AMControlMoveButton.h \
	source/beamline/AMSampleManipulator.h \
	source/beamline/AMControlSetSampleManipulator.h \
	source/util/AMGithubManager.h \
	source/ui/util/AMGithubIssueSubmissionView.h \
	source/dataman/export/AMExporterAthena.h \
	source/analysis/AM1DBasicDerivativeABEditor.h \
	source/analysis/AM1DIntegralAB.h \
	source/analysis/AM1DBasicIntegralABEditor.h \
	source/util/AMJoystick.h \
	source/ui/dataman/AMControlInfoListTableView.h \
	source/dataman/AM2DScan.h \
	source/analysis/AM2DNormalizationAB.h \
	source/analysis/AM2DNormalizationABEditor.h \
	source/dataman/info/AMBasicXRFDetectorInfo.h \
	source/util/AMFontSizes.h \
	source/ui/dataman/AMScanViewUtilities.h \
	source/ui/dataman/AM2DScanView.h \
	source/ui/AMDatamanStartupSplashScreen.h \
	source/dataman/database/AMDbUpgrade.h \
	source/dataman/datasource/AMDataSourceImageDatawDefault.h \
	source/util/AMDataSourcePlotSettings.h \
	source/util/AMSelectablePeriodicTable.h \
	source/ui/util/AMSelectablePeriodicTableView.h \
	source/ui/dataman/AMDbObjectGeneralView.h \
	source/ui/dataman/AMDbObjectGeneralViewSupport.h \
	source/ui/dataman/AM2DScanConfigurationGeneralView.h \
	source/actions3/AMLoopActionInfo3.h \
	source/actions3/AMLoopAction3.h \
	source/actions3/AMListAction3.h \
	source/actions3/AMActionRunner3.h \
	source/actions3/AMActionRegistry3.h \
	source/actions3/AMActionLog3.h \
	source/actions3/editors/AMLoopActionEditor3.h \
	source/ui/actions3/AMWorkflowView3.h \
	source/ui/actions3/AMAddActionDialog3.h \
	source/ui/actions3/AMActionRunnerQueueView3.h \
	source/ui/actions3/AMActionRunnerCurrentView3.h \
	source/ui/actions3/AMActionRunnerAddActionBar3.h \
	source/ui/actions3/AMActionHistoryView3.h \
	source/actions3/actions/AMNumberChangeAction.h \
	source/actions3/actions/AMNumberChangeActionInfo.h \
	source/actions3/editors/AMNumberChangeActionEditor.h \
	source/actions3/AMListActionInfo3.h \
	source/actions3/editors/AMListActionEditor3.h \
	source/actions3/actions/AMControlMoveActionInfo3.h \
	source/actions3/actions/AMControlMoveAction3.h \
	source/actions3/editors/AMControlMoveActionEditor3.h \
	source/actions3/AMActionCondition.h \
	source/ui/actions3/AMActionHistoryTreeView.h \
	source/ui/actions3/AMActionHistoryModel.h \
	source/actions3/actions/AMScanAction.h \
	source/actions3/actions/AMScanActionInfo.h \
	source/actions3/editors/AMScanActionEditor.h \
	source/analysis/AM1DNormalizationAB.h \
	source/analysis/AM1DNormalizationABEditor.h \
	source/analysis/AM1DCalibrationAB.h \
	source/analysis/AM1DCalibrationABEditor.h \
	source/ui/AMAddAnalysisBlockDialog.h \
	source/ui/acquaman/AMScanConfigurationViewHolder3.h \
	source/dataman/datastore/AMCDFDataStore.h \
	source/util/amlikely.h \
	source/ui/dataman/AMSimpleDataSourceEditor.h \
	source/ui/beamline/AMExtendedControlEditor.h \
	source/ui/beamline/AMControlButton.h \
	source/dataman/AMLineScan.h \
	source/ui/AMTopFrame2.h \
	source/application/AMDatamanAppControllerForActions3.h \
	source/analysis/AM2DAdditionAB.h \
	source/analysis/AM3DAdditionAB.h \
	source/analysis/AM3DBinningAB.h \
	source/analysis/AM3DBinningABEditor.h \
	source/analysis/AM2DDeadTimeAB.h \
	source/analysis/AM3DDeadTimeAB.h \
	source/actions3/editors/AMLiveLoopActionEditor3.h \
	source/ui/AMBottomPanel.h \
	source/ui/AMDatamanAppBottomPanel.h \
	source/ui/AMAppBottomPanel.h \
	source/ui/actions3/AMActionRunnerBottomBarCurrentView3.h \
	source/dataman/info/AMOldDetectorInfo.h \
	source/beamline/AMOldDetector.h \
	source/dataman/info/AMOldDetectorInfoSet.h \
	source/dataman/AMDbUpgrade1Pt1.h \
	source/dataman/AMDbUpgrade1Pt2.h \
	source/ui/beamline/AMOldDetectorView.h \
	source/ui/beamline/AMOldDetectorViewSupport.h \
	source/ui/beamline/AMDetectorView.h \
	source/beamline/AMDetectorSet.h \
	source/dataman/info/AMDetectorInfoSet.h \
	source/ui/beamline/AMDetectorSelectorView.h \
	source/beamline/AMDetectorGroup.h \
	source/beamline/AMDetectorSelector.h \
	source/acquaman/AMScanActionController.h \
	source/acquaman/AMScanActionControllerScanAssembler.h \
	source/actions3/actions/AMAxisStartedActionInfo.h \
	source/actions3/actions/AMAxisFinishedActionInfo.h \
	source/actions3/actions/AMAxisStartedAction.h \
	source/actions3/actions/AMAxisFinishedAction.h \
	source/beamline/AMBasicControlDetectorEmulator.h \
	source/dataman/AMScanAxisRegion.h \
	source/dataman/AMScanAxis.h \
	source/acquaman/AMScanActionControllerScanOptimizer.h \
	source/acquaman/AMDetectorTriggerSourceScanOptimizer.h \
	source/acquaman/AMListActionScanOptimizer.h \
	source/acquaman/AMScanActionControllerScanValidator.h \
	source/acquaman/AMScanActionTreeSupport.h \
	source/acquaman/AMNestedAxisTypeValidator.h \
	source/dataman/AM3DScan.h \
	source/dataman/AMTextStream.h \
	source/acquaman/AMDetectorDwellTimeSourceScanOptimizer.h \
	source/analysis/AM3DNormalizationAB.h \
	source/analysis/AM3DNormalizationABEditor.h \
	source/analysis/AM4DBinningAB.h \
	source/analysis/AM4DBinningABEditor.h \
	source/analysis/AMOrderReductionAB.h \
	source/analysis/AMOrderReductionABEditor.h \
	source/beamline/AMMotorGroup.h \
	source/beamline/AM4DMotorGroup.h \
	source/ui/AM4DMotorGroupView.h \
	source/ui/AMMotorGroupView.h \
	source/util/AMPointerTree.h \
	source/dataman/AMDbUpgrade1Pt4.h \
	source/dataman/AMSamplePre2013.h \
	source/dataman/AMSamplePlatePre2013.h \
	source/ui/dataman/AMSamplePlatePre2013View.h \
	source/ui/dataman/AMSampleManagementPre2013Widget.h \
	source/ui/dataman/AMSamplePre2013Editor.h \
	source/ui/dataman/AMSamplePre2013Selector.h \
	source/actions3/actions/AMSamplePlatePre2013MoveAction.h \
	source/actions3/actions/AMSamplePlatePre2013MoveActionInfo.h \
	source/ui/dataman/AMSamplePositionPre2013ViewActionsWidget.h \
	source/actions3/editors/AMSamplePlatePre2013MoveActionEditor.h \
	source/dataman/AMSample.h \
	source/beamline/camera/AMShapeData.h \
	source/beamline/camera/AMCamera.h \
	source/beamline/camera/AMBeamConfiguration.h \
	source/beamline/camera/AMCameraConfiguration.h \
	source/ui/dataman/AMSampleView.h \
	source/ui/beamline/camera/AMCameraConfigurationView.h \
	source/ui/beamline/camera/AMBeamConfigurationView.h \
	source/ui/beamline/camera/AMShapeDataView.h \
	source/ui/beamline/camera/AMCameraConfigurationWizard.h \
	source/ui/beamline/camera/AMBeamConfigurationWizard.h \
	source/ui/beamline/camera/AMSamplePlateWizard.h \
	source/ui/beamline/camera/AMGraphicsViewWizard.h \
	source/ui/beamline/camera/AMRotationWizard.h \
	source/ui/AMColorPickerButton2.h \
	source/ui/util/AMSamplePeriodicTableDialog.h \
	source/beamline/camera/AMGraphicsVideoSceneCopier.h \
	source/ui/AMOverlayVideoWidget2.h \
	source/ui/util/AMPeriodicTableDialog.h \
	source/dataman/AMSamplePlate.h \
	source/ui/dataman/AMSamplePlateView.h \
	source/ui/beamline/AMBeamlineSampleManagementView.h \
	source/ui/dataman/AMSamplePlateBrowserView.h \
	source/beamline/camera/AMSampleCamera.h \
	source/beamline/camera/AMSampleCameraBrowser.h \
	source/ui/beamline/camera/AMSampleCameraView.h \
	source/ui/beamline/camera/AMSampleCameraBrowserView.h \
	source/ui/beamline/camera/AMSampleCameraGraphicsView.h \
	source/ui/AMGraphicsTextItem.h \
	source/ui/beamline/camera/AMSampleCameraWizardSelector.h \
	source/ui/dataman/AMSampleEditor.h \
	source/beamline/camera/AMRotationalOffset.h \
	source/analysis/AM1DDeadTimeAB.h \
	source/analysis/AM2DDeadTimeCorrectionAB.h \
	source/analysis/AM3DDeadTimeCorrectionAB.h \
	source/beamline/AMXRFDetector.h \
	source/ui/beamline/AMXRFBaseDetectorView.h \
	source/util/AMAbsorptionEdge.h \
	source/util/AMEmissionLine.h \
	source/util/AMSelectableElement.h \
	source/util/AMCustomizablePeriodicTable.h \
	source/ui/util/AMCustomizablePeriodicTableView.h \
	source/util/AMRange.h \
	source/ui/util/AMSelectableElementView.h \
	source/ui/util/AMSelectableItemView.h \
	source/ui/beamline/AMXRFDetailedDetectorView.h \
	source/util/AMNameAndRangeValidator.h \
	source/dataman/AMRegionOfInterest.h \
	source/analysis/AMnDDeadTimeAB.h \
	source/ui/AMSelectionDialog.h \
	source/ui/beamline/AMDeadTimeButton.h \
	source/analysis/AMRegionOfInterestAB.h \
	source/ui/beamline/AMRegionOfInterestView.h \
	source/ui/actions3/AMCancelActionPrompt.h \
	source/actions3/AMActionSupport.h \
	source/actions3/actions/AMControlStopAction.h \
	source/actions3/actions/AMControlStopActionInfo.h \
	source/beamline/camera/AMAngle.h \
	source/ui/dataman/AMSampleBriefView.h \
	source/ui/beamline/camera/AMSimpleBeamConfigurationWizard.h \
	source/ui/beamline/camera/AMWizardManager.h \
	source/ui/beamline/camera/AMSimpleSamplePlateWizard.h \
	source/dataman/database/AMDbObjectDefinitions.h \
	source/ui/dataman/AMControlInfoListView.h \
	source/ui/dataman/AMOldDetectorInfoSetView.h \
	source/ui/beamline/AMDetectorSelectorRequiredView.h \
	source/actions3/actions/AMControlWaitActionInfo.h \
	source/actions3/actions/AMControlWaitAction.h \
	source/analysis/AM1DDarkCurrentCorrectionAB.h \
	source/analysis/AM1DDarkCurrentCorrectionABEditor.h \
	source/beamline/AMCurrentAmplifier.h \
	source/beamline/AMAdvancedControlDetectorEmulator.h \
	source/dataman/AMDbUpgrade1Pt3.h \
	source/acquaman/AMScanActionControllerBasicFileWriter.h \
	source/acquaman/AMTimedScanActionController.h \
	source/acquaman/AMTimedRegionScanConfiguration.h \
	source/actions3/actions/AMSampleMoveActionInfo.h \
	source/actions3/actions/AMSampleMoveAction.h \
	source/actions3/editors/AMSampleMoveActionEditor.h \
	source/util/AMVariableIntegrationTime.h \
	source/acquaman/AMStepScanConfiguration.h \
	source/acquaman/AMGenericScanActionControllerAssembler.h \
	source/acquaman/AMStepScanActionController.h \
	source/dataman/AMScanAxisEXAFSRegion.h \
	source/acquaman/AMEXAFSScanActionControllerAssembler.h \
	source/util/AMEnergyToKSpaceCalculator.h \
	source/actions3/actions/AMAxisValueFinishedAction.h \
	source/actions3/actions/AMAxisValueFinishedActionInfo.h \
	source/ui/dataman/AMStepScanAxisView.h \
	source/ui/dataman/AMEXAFSScanAxisView.h \
	source/ui/AMRegExpLineEdit.h \
	source/application/AMCrashMonitorSupport.h \
	source/analysis/AM0DAccumulatorAB.h \
	source/util/AMBuildInfo.h \
	source/util/AMBuildReporter.h \
	source/analysis/AM1DTimedDataAB.h \
	source/analysis/AM1DKSpaceCalculatorAB.h \
	source/dataman/AMLightweightScanInfo.h \
	source/dataman/AMLightweightScanInfoCollection.h \
	source/dataman/AMLightweightScanInfoModel.h \
	source/ui/util/AMSortFilterWidget.h \
	source/ui/dataman/AMScanDataView.h \
	source/dataman/AMLightweightScanInfoFilterProxyModel.h \
	source/ui/dataman/AMScanTableView.h \
	source/ui/dataman/AMScanTreeView.h \
	source/util/AMRecursiveDirectoryCompare.h \
	source/util/AMDirectorySynchronizer.h \
	source/ui/util/AMDialog.h \
	source/ui/util/AMDirectorySynchronizerDialog.h \
	source/ui/beamline/AMCurrentAmplifierCompositeView.h \
	source/ui/beamline/AMCurrentAmplifierSingleView.h \
	source/ui/beamline/AMCurrentAmplifierView.h \
	source/actions3/actions/AMWaitActionInfo.h \
	source/actions3/actions/AMWaitAction.h \
	source/actions3/editors/AMWaitActionEditor.h \
	source/ui/util/AMBuildInfoWidget.h \
	source/ui/util/AMAboutDialog.h \
	source/beamline/AM1DControlDetectorEmulator.h \
	source/ui/actions3/AMActionRunnerCurrentViewBase.h \
	source/dataman/AMDbUpgrade1Pt5.h \
	source/beamline/AMBeamlineControlAPI.h \
	source/beamline/AMBeamlineControlSetAPI.h \
	source/beamline/AMBeamlineDetectorAPI.h \
	source/beamline/AMBeamlineDetectorSetAPI.h \
	source/beamline/AMBeamlineSynchronizedDwellTimeAPI.h \
	source/util/AMThread.h \
	source/util/AMAppArgumentParser.h \
	source/beamline/AMStorageRing.h \
	source/ui/dataman/AMBrowseScansView.h \
	source/beamline/AMScalerTimeControlDetector.h \
	source/dataman/export/AMSMAKExporter.h \
	source/dataman/export/AMExporter2DAscii.h \
	source/util/AMEnergyList.h \
	source/ui/util/AMEnergyListView.h \
	source/dataman/export/AMExporterOptionSMAK.h \
	source/ui/AMScanEditorsCloseView.h \
	source/ui/util/AMSortFilterScansWidget.h \
	source/ui/AMPenStyleComboBox.h \
	source/ui/AMPlotMarkerComboBox.h \
	source/actions3/actions/AMChangeToleranceActionInfo.h \
	source/actions3/actions/AMChangeToleranceAction.h \
	source/dataman/AMXRFScan.h \
	source/ui/util/AMChooseDataFolderDialog.h \
	source/acquaman/AMTimedScanActionControllerAssembler.h \
	source/dataman/AMLightweightScanInfoFactory.h \
	source/ui/util/AMGridFlowGeometryManager.h \
	source/ui/dataman/AMScanThumbnailGridViewItemDelegate.h \
	source/ui/dataman/AMScanThumbnailGridView.h \
	source/ui/dataman/AMScanThumbnailGridInputManager.h \
	source/ui/dataman/AMScanThumbnailGridGeometryManager.h \
	source/acquaman/AMGenericStepScanController.h \
	source/acquaman/AMGenericStepScanConfiguration.h \
	source/ui/acquaman/AMGenericStepScanConfigurationView.h \
	source/util/AMCSVParser.h \
	source/actions3/AMTimeoutLoopActionInfo.h \
	source/actions3/AMTimeoutLoopAction.h \
	source/beamline/AMPseudoMotorControl.h \
	source/beamline/AMXspress3XRFDetector.h \
	source/ui/beamline/AMXspress3XRFDetectorView.h \
    source/dataman/export/AMExporterXDIFormat.h \
    source/dataman/export/AMExporterOptionXDIFormat.h \
    source/ui/dataman/AMScanViewPlotToolsView.h \
    source/ui/dataman/AMScanViewPlotToolsButtonView.h \
    source/ui/dataman/AMScanViewPlotToolView.h \
    source/ui/dataman/AMScanViewPlotSelectedToolsView.h \
    source/dataman/AMScanViewPlotTools.h \
    source/ui/dataman/AMScanPlotView.h

FORMS += \

SOURCES += \
	source/acquaman/AMScanConfiguration.cpp \
	source/acquaman/AMScanController.cpp \
	source/application/AMAppController.cpp \
	source/util/AMSettings.cpp \
	source/beamline/AMPVNames.cpp \
	source/dataman/AMExperiment.cpp \
	source/dataman/AMImportController.cpp \
	source/dataman/AMRun.cpp \
	source/dataman/AMScan.cpp \
	source/dataman/AMScanSetModel.cpp \
	source/dataman/AMXASScan.cpp \
	source/ui/dataman/AMRunSelector.cpp \
	source/ui/beamline/AMControlSetView.cpp \
	source/ui/dataman/AMCramBarHorizontal.cpp \
	source/ui/AMMainWindow.cpp \
	source/ui/dataman/AMRunExperimentInsert.cpp \
	source/ui/dataman/AMScanView.cpp \
	source/ui/AMSidebar.cpp \
	source/ui/AMStatusView.cpp \
	source/ui/AMThumbnailScrollViewer.cpp \
	source/ui/beamline/AMControlEditor.cpp \
	source/ui/dataman/AMNewRunDialog.cpp \
	source/ui/AMPrefixSuffixLineEdit.cpp \
	source/ui/AMDragDropItemModel.cpp \
	source/dataman/AMRunExperimentItems.cpp \
	source/ui/dataman/AMGenericScanEditor.cpp \
	source/ui/AMDetailedItemDelegate.cpp \
	source/ui/AMVerticalStackWidget.cpp \
	source/ui/AMHeaderButton.cpp \
	source/util/AMDateTimeUtils.cpp \
	source/ui/AMElementListEdit.cpp \
	source/ui/AMCloseItemDelegate.cpp \
	source/ui/dataman/AMDataSourcesEditor.cpp \
	source/ui/AMWrappingLineEdit.cpp \
	source/ui/AMStartScreen.cpp \
	source/dataman/AMUser.cpp \
	source/dataman/AMXESScan.cpp \
	source/dataman/info/ALSBL8XESDetectorInfo.cpp \
	source/dataman/ALSBL8XASFileLoader.cpp \
	source/ui/AMWindowPaneModel.cpp \
	source/dataman/AMScanEditorModelItem.cpp \
	source/dataman/datastore/AMDataStore.cpp \
	source/dataman/AMAnalysisBlock.cpp \
	source/dataman/datasource/AMDataSourceSeriesData.cpp \
	source/dataman/datasource/AMDataSourceImageData.cpp \
	source/analysis/AM1DExpressionAB.cpp \
	source/dataman/datasource/AMRawDataSource.cpp \
	source/analysis/AM1DExpressionABEditor.cpp \
	source/dataman/datastore/AMInMemoryDataStore.cpp \
	source/acquaman/AMFastScanConfiguration.cpp \
	source/dataman/AMFastScan.cpp \
	source/analysis/AM2DSummingAB.cpp \
	source/analysis/AMStandardAnalysisBlock.cpp \
	source/analysis/AM2DSummingABEditor.cpp \
	source/ui/util/AMPeriodicTableView.cpp \
	source/util/AMPeriodicTable.cpp \
	source/util/AMElement.cpp \
	source/dataman/info/AMSpectralOutputDetectorInfo.cpp \
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
	source/analysis/AM1DDerivativeAB.cpp \
	source/analysis/AM1DInterpolationAB.cpp \
	source/analysis/AM1DRunningAverageFilterAB.cpp \
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
	source/dataman/datasource/AMXYScatterPVDataSource.cpp \
	source/beamline/AMCompositeControl.cpp \
	source/ui/util/AMSettingsView.cpp \
	source/dataman/AMScanDictionary.cpp \
	source/dataman/AMScanParametersDictionary.cpp \
	source/dataman/AMScanExemplarDictionary.cpp \
	source/dataman/AMScanExemplar.cpp \
	source/ui/dataman/AMDictionaryLineEdit.cpp \
	source/application/AMPluginsManager.cpp \
	source/application/AMAppControllerSupport.cpp \
	source/dataman/import/AMScanDatabaseImportController.cpp \
	source/ui/dataman/AMScanDatabaseImportWizard.cpp \
	source/ui/beamline/AMControlMoveButton.cpp \
	source/beamline/AMControlSetSampleManipulator.cpp \
	source/util/AMGithubManager.cpp \
	source/ui/util/AMGithubIssueSubmissionView.cpp \
	source/dataman/export/AMExporterAthena.cpp \
	source/analysis/AM1DBasicDerivativeABEditor.cpp \
	source/analysis/AM1DIntegralAB.cpp \
	source/analysis/AM1DBasicIntegralABEditor.cpp \
	source/util/AMJoystick.cpp \
	source/ui/dataman/AMControlInfoListTableView.cpp \
	source/dataman/AM2DScan.cpp \
	source/analysis/AM2DNormalizationAB.cpp \
	source/analysis/AM2DNormalizationABEditor.cpp \
	source/dataman/info/AMBasicXRFDetectorInfo.cpp \
	source/ui/dataman/AMScanViewUtilities.cpp \
	source/ui/dataman/AM2DScanView.cpp \
	source/ui/AMDatamanStartupSplashScreen.cpp \
	source/dataman/database/AMDbUpgrade.cpp \
	source/dataman/datasource/AMDataSourceImageDatawDefault.cpp \
	source/util/AMSelectablePeriodicTable.cpp \
	source/ui/util/AMSelectablePeriodicTableView.cpp \
	source/ui/dataman/AMDbObjectGeneralView.cpp \
	source/ui/dataman/AMDbObjectGeneralViewSupport.cpp \
	source/ui/dataman/AM2DScanConfigurationGeneralView.cpp \
	source/actions3/AMLoopActionInfo3.cpp \
	source/actions3/AMLoopAction3.cpp \
	source/actions3/AMListAction3.cpp \
	source/actions3/AMActionRunner3.cpp \
	source/actions3/AMActionRegistry3.cpp \
	source/actions3/AMActionLog3.cpp \
	source/actions3/editors/AMLoopActionEditor3.cpp \
	source/ui/actions3/AMWorkflowView3.cpp \
	source/ui/actions3/AMAddActionDialog3.cpp \
	source/ui/actions3/AMActionRunnerQueueView3.cpp \
	source/ui/actions3/AMActionRunnerCurrentView3.cpp \
	source/ui/actions3/AMActionRunnerAddActionBar3.cpp \
	source/ui/actions3/AMActionHistoryView3.cpp \
	source/actions3/actions/AMNumberChangeAction.cpp \
	source/actions3/actions/AMNumberChangeActionInfo.cpp \
	source/actions3/editors/AMNumberChangeActionEditor.cpp \
	source/actions3/AMListActionInfo3.cpp \
	source/actions3/editors/AMListActionEditor3.cpp \
	source/actions3/actions/AMControlMoveActionInfo3.cpp \
	source/actions3/actions/AMControlMoveAction3.cpp \
	source/actions3/editors/AMControlMoveActionEditor3.cpp \
	source/actions3/AMActionCondition.cpp \
	source/ui/actions3/AMActionHistoryTreeView.cpp \
	source/ui/actions3/AMActionHistoryModel.cpp \
	source/actions3/actions/AMScanAction.cpp \
	source/actions3/actions/AMScanActionInfo.cpp \
	source/actions3/editors/AMScanActionEditor.cpp \
	source/analysis/AM1DNormalizationAB.cpp \
	source/analysis/AM1DNormalizationABEditor.cpp \
	source/analysis/AM1DCalibrationAB.cpp \
	source/analysis/AM1DCalibrationABEditor.cpp \
	source/ui/AMAddAnalysisBlockDialog.cpp \
	source/ui/acquaman/AMScanConfigurationViewHolder3.cpp \
	source/dataman/datastore/AMCDFDataStore.cpp \
	source/ui/dataman/AMSimpleDataSourceEditor.cpp \
	source/ui/beamline/AMExtendedControlEditor.cpp \
	source/ui/beamline/AMControlButton.cpp \
	source/dataman/AMLineScan.cpp \
	source/ui/AMTopFrame2.cpp \
	source/application/AMDatamanAppControllerForActions3.cpp \
	source/analysis/AM2DAdditionAB.cpp \
	source/analysis/AM3DAdditionAB.cpp \
	source/analysis/AM3DBinningAB.cpp \
	source/analysis/AM3DBinningABEditor.cpp \
	source/analysis/AM2DDeadTimeAB.cpp \
	source/analysis/AM3DDeadTimeAB.cpp \
	source/actions3/editors/AMLiveLoopActionEditor3.cpp \
	source/ui/AMBottomPanel.cpp \
	source/ui/AMDatamanAppBottomPanel.cpp \
	source/ui/AMAppBottomPanel.cpp \
	source/ui/actions3/AMActionRunnerBottomBarCurrentView3.cpp \
	source/dataman/info/AMOldDetectorInfo.cpp \
	source/beamline/AMOldDetector.cpp \
	source/dataman/info/AMOldDetectorInfoSet.cpp \
	source/dataman/AMDbUpgrade1Pt1.cpp \
	source/dataman/AMDbUpgrade1Pt2.cpp \
	source/ui/beamline/AMOldDetectorViewSupport.cpp \
	source/ui/beamline/AMOldDetectorView.cpp \
	source/ui/beamline/AMDetectorView.cpp \
	source/beamline/AMDetectorSet.cpp \
	source/dataman/info/AMDetectorInfoSet.cpp \
	source/ui/beamline/AMDetectorSelectorView.cpp \
	source/beamline/AMDetectorGroup.cpp \
	source/beamline/AMDetectorSelector.cpp \
	source/acquaman/AMScanActionController.cpp \
	source/acquaman/AMScanActionControllerScanAssembler.cpp \
	source/actions3/actions/AMAxisStartedActionInfo.cpp \
	source/actions3/actions/AMAxisFinishedActionInfo.cpp \
	source/actions3/actions/AMAxisStartedAction.cpp \
	source/actions3/actions/AMAxisFinishedAction.cpp \
	source/beamline/AMBasicControlDetectorEmulator.cpp \
	source/dataman/AMScanAxisRegion.cpp \
	source/dataman/AMScanAxis.cpp \
	source/acquaman/AMScanActionControllerScanOptimizer.cpp \
	source/acquaman/AMDetectorTriggerSourceScanOptimizer.cpp \
	source/acquaman/AMListActionScanOptimizer.cpp \
	source/acquaman/AMScanActionControllerScanValidator.cpp \
	source/acquaman/AMScanActionTreeSupport.cpp \
	source/acquaman/AMNestedAxisTypeValidator.cpp \
	source/dataman/AM3DScan.cpp \
	source/dataman/AMTextStream.cpp \
	source/acquaman/AMDetectorDwellTimeSourceScanOptimizer.cpp \
	source/analysis/AM3DNormalizationAB.cpp \
	source/analysis/AM3DNormalizationABEditor.cpp \
	source/analysis/AM4DBinningAB.cpp \
	source/analysis/AM4DBinningABEditor.cpp \
	source/analysis/AMOrderReductionAB.cpp \
	source/analysis/AMOrderReductionABEditor.cpp \
	source/beamline/AMMotorGroup.cpp \
	source/beamline/AM4DMotorGroup.cpp \
	source/ui/AM4DMotorGroupView.cpp \
	source/ui/AMMotorGroupView.cpp \
	source/util/AMPointerTree.cpp \
	source/dataman/AMDbUpgrade1Pt4.cpp \
	source/dataman/AMSamplePre2013.cpp \
	source/dataman/AMSamplePlatePre2013.cpp \
	source/ui/dataman/AMSamplePlatePre2013View.cpp \
	source/ui/dataman/AMSampleManagementPre2013Widget.cpp \
	source/ui/dataman/AMSamplePre2013Editor.cpp \
	source/ui/dataman/AMSamplePre2013Selector.cpp \
	source/actions3/actions/AMSamplePlatePre2013MoveAction.cpp \
	source/actions3/actions/AMSamplePlatePre2013MoveActionInfo.cpp \
	source/ui/dataman/AMSamplePositionPre2013ViewActionsWidget.cpp \
	source/actions3/editors/AMSamplePlatePre2013MoveActionEditor.cpp \
	source/dataman/AMSample.cpp \
	source/beamline/camera/AMShapeData.cpp \
	source/beamline/camera/AMCamera.cpp \
	source/beamline/camera/AMBeamConfiguration.cpp \
	source/beamline/camera/AMCameraConfiguration.cpp \
	source/ui/dataman/AMSampleView.cpp \
	source/ui/beamline/camera/AMCameraConfigurationView.cpp \
	source/ui/beamline/camera/AMBeamConfigurationView.cpp \
	source/ui/beamline/camera/AMShapeDataView.cpp \
	source/ui/beamline/camera/AMCameraConfigurationWizard.cpp \
	source/ui/beamline/camera/AMBeamConfigurationWizard.cpp \
	source/ui/beamline/camera/AMSamplePlateWizard.cpp \
	source/ui/beamline/camera/AMGraphicsViewWizard.cpp \
	source/ui/beamline/camera/AMRotationWizard.cpp \
	source/ui/AMColorPickerButton2.cpp \
	source/ui/util/AMSamplePeriodicTableDialog.cpp \
	source/beamline/camera/AMGraphicsVideoSceneCopier.cpp \
	source/ui/AMOverlayVideoWidget2.cpp \
	source/ui/util/AMPeriodicTableDialog.cpp \
	source/dataman/AMSamplePlate.cpp \
	source/ui/dataman/AMSamplePlateView.cpp \
	source/ui/beamline/AMBeamlineSampleManagementView.cpp \
	source/ui/dataman/AMSamplePlateBrowserView.cpp \
	source/beamline/camera/AMSampleCamera.cpp \
	source/beamline/camera/AMSampleCameraBrowser.cpp \
	source/ui/beamline/camera/AMSampleCameraView.cpp \
	source/ui/beamline/camera/AMSampleCameraBrowserView.cpp \
	source/ui/beamline/camera/AMSampleCameraGraphicsView.cpp \
	source/ui/AMGraphicsTextItem.cpp \
	source/ui/beamline/camera/AMSampleCameraWizardSelector.cpp \
	source/ui/dataman/AMSampleEditor.cpp \
	source/beamline/camera/AMRotationalOffset.cpp \
	source/analysis/AM1DDeadTimeAB.cpp \
	source/analysis/AM2DDeadTimeCorrectionAB.cpp \
	source/analysis/AM3DDeadTimeCorrectionAB.cpp \
	source/beamline/AMXRFDetector.cpp \
	source/ui/beamline/AMXRFBaseDetectorView.cpp \
	source/util/AMAbsorptionEdge.cpp \
	source/util/AMEmissionLine.cpp \
	source/util/AMSelectableElement.cpp \
	source/util/AMCustomizablePeriodicTable.cpp \
	source/ui/util/AMCustomizablePeriodicTableView.cpp \
	source/util/AMRange.cpp \
	source/ui/util/AMSelectableElementView.cpp \
	source/ui/util/AMSelectableItemView.cpp \
	source/ui/beamline/AMXRFDetailedDetectorView.cpp \
	source/util/AMNameAndRangeValidator.cpp \
	source/dataman/AMRegionOfInterest.cpp \
	source/analysis/AMnDDeadTimeAB.cpp \
	source/ui/AMSelectionDialog.cpp \
	source/ui/beamline/AMDeadTimeButton.cpp \
	source/analysis/AMRegionOfInterestAB.cpp \
	source/ui/beamline/AMRegionOfInterestView.cpp \
	source/ui/actions3/AMCancelActionPrompt.cpp \
	source/actions3/actions/AMControlStopAction.cpp \
	source/actions3/actions/AMControlStopActionInfo.cpp \
	source/beamline/camera/AMAngle.cpp \
	source/ui/dataman/AMSampleBriefView.cpp \
	source/ui/beamline/camera/AMSimpleBeamConfigurationWizard.cpp \
	source/ui/beamline/camera/AMWizardManager.cpp \
	source/ui/beamline/camera/AMSimpleSamplePlateWizard.cpp \
	source/ui/dataman/AMControlInfoListView.cpp \
	source/ui/dataman/AMOldDetectorInfoSetView.cpp \
	source/ui/beamline/AMDetectorSelectorRequiredView.cpp \
	source/actions3/actions/AMControlWaitActionInfo.cpp \
	source/actions3/actions/AMControlWaitAction.cpp \
	source/analysis/AM1DDarkCurrentCorrectionAB.cpp \
	source/analysis/AM1DDarkCurrentCorrectionABEditor.cpp \
	source/beamline/AMCurrentAmplifier.cpp \
	source/beamline/AMAdvancedControlDetectorEmulator.cpp \
	source/dataman/AMDbUpgrade1Pt3.cpp \
	source/acquaman/AMScanActionControllerBasicFileWriter.cpp \
	source/acquaman/AMTimedScanActionController.cpp \
	source/acquaman/AMTimedRegionScanConfiguration.cpp \
	source/actions3/actions/AMSampleMoveActionInfo.cpp \
	source/actions3/actions/AMSampleMoveAction.cpp \
	source/actions3/editors/AMSampleMoveActionEditor.cpp \
	source/util/AMVariableIntegrationTime.cpp \
	source/acquaman/AMStepScanConfiguration.cpp \
	source/acquaman/AMGenericScanActionControllerAssembler.cpp \
	source/acquaman/AMStepScanActionController.cpp \
	source/dataman/AMScanAxisEXAFSRegion.cpp \
	source/acquaman/AMEXAFSScanActionControllerAssembler.cpp \
	source/util/AMEnergyToKSpaceCalculator.cpp \
	source/actions3/actions/AMAxisValueFinishedAction.cpp \
	source/actions3/actions/AMAxisValueFinishedActionInfo.cpp \
	source/ui/dataman/AMStepScanAxisView.cpp \
	source/ui/dataman/AMEXAFSScanAxisView.cpp \
	source/ui/AMRegExpLineEdit.cpp \
	source/application/AMCrashMonitorSupport.cpp \
	source/analysis/AM0DAccumulatorAB.cpp \
	source/util/AMBuildInfo.cpp \
	source/util/AMBuildReporter.cpp \
	source/analysis/AM1DTimedDataAB.cpp \
	source/analysis/AM1DKSpaceCalculatorAB.cpp \
	source/dataman/AMLightweightScanInfo.cpp \
	source/dataman/AMLightweightScanInfoCollection.cpp \
	source/dataman/AMLightweightScanInfoModel.cpp \
	source/ui/util/AMSortFilterWidget.cpp \
	source/ui/dataman/AMScanDataView.cpp \
	source/dataman/AMLightweightScanInfoFilterProxyModel.cpp \
	source/ui/dataman/AMScanTableView.cpp \
	source/ui/dataman/AMScanTreeView.cpp \
	source/util/AMRecursiveDirectoryCompare.cpp \
	source/util/AMDirectorySynchronizer.cpp \
	source/ui/util/AMDialog.cpp \
	source/ui/util/AMDirectorySynchronizerDialog.cpp \
	source/ui/beamline/AMCurrentAmplifierCompositeView.cpp \
	source/ui/beamline/AMCurrentAmplifierSingleView.cpp \
	source/ui/beamline/AMCurrentAmplifierView.cpp \
	source/actions3/actions/AMWaitActionInfo.cpp \
	source/actions3/actions/AMWaitAction.cpp \
	source/actions3/editors/AMWaitActionEditor.cpp \
	source/ui/util/AMBuildInfoWidget.cpp \
	source/ui/util/AMAboutDialog.cpp \
	source/beamline/AM1DControlDetectorEmulator.cpp \
	source/ui/actions3/AMActionRunnerCurrentViewBase.cpp \
	source/dataman/AMDbUpgrade1Pt5.cpp \
	source/beamline/AMBeamlineControlAPI.cpp \
	source/beamline/AMBeamlineControlSetAPI.cpp \
	source/beamline/AMBeamlineDetectorAPI.cpp \
	source/beamline/AMBeamlineDetectorSetAPI.cpp \
	source/beamline/AMBeamlineSynchronizedDwellTimeAPI.cpp \
	source/util/AMThread.cpp \
	source/util/AMAppArgumentParser.cpp \
	source/beamline/AMStorageRing.cpp \
	source/ui/dataman/AMBrowseScansView.cpp \
	source/beamline/AMScalerTimeControlDetector.cpp \
	source/dataman/export/AMSMAKExporter.cpp \
	source/dataman/export/AMExporter2DAscii.cpp \
	source/util/AMEnergyList.cpp \
	source/ui/util/AMEnergyListView.cpp \
	source/dataman/export/AMExporterOptionSMAK.cpp \
	source/ui/AMScanEditorsCloseView.cpp \
	source/ui/util/AMSortFilterScansWidget.cpp \
	source/ui/dataman/AMImportControllerWidget.cpp \
	source/ui/AMPenStyleComboBox.cpp \
	source/ui/AMPlotMarkerComboBox.cpp \
	source/actions3/actions/AMChangeToleranceActionInfo.cpp \
	source/actions3/actions/AMChangeToleranceAction.cpp \
	source/dataman/AMXRFScan.cpp \
	source/ui/util/AMChooseDataFolderDialog.cpp \
	source/acquaman/AMTimedScanActionControllerAssembler.cpp \
	source/dataman/AMLightweightScanInfoFactory.cpp \
	source/ui/util/AMGridFlowGeometryManager.cpp \
	source/ui/dataman/AMScanThumbnailGridViewItemDelegate.cpp \
	source/ui/dataman/AMScanThumbnailGridView.cpp \
	source/ui/dataman/AMScanThumbnailGridInputManager.cpp \
	source/ui/dataman/AMScanThumbnailGridGeometryManager.cpp \
	source/acquaman/AMGenericStepScanController.cpp \
	source/acquaman/AMGenericStepScanConfiguration.cpp \
	source/ui/acquaman/AMGenericStepScanConfigurationView.cpp \
	source/util/AMCSVParser.cpp \
	source/actions3/AMTimeoutLoopActionInfo.cpp \
	source/actions3/AMTimeoutLoopAction.cpp \
	source/beamline/AMPseudoMotorControl.cpp \
	source/beamline/AMXspress3XRFDetector.cpp \
	source/ui/beamline/AMXspress3XRFDetectorView.cpp \
    source/dataman/export/AMExporterXDIFormat.cpp \
    source/dataman/export/AMExporterOptionXDIFormat.cpp \
    source/ui/dataman/AMScanViewPlotToolsView.cpp \
    source/ui/dataman/AMScanViewPlotToolsButtonView.cpp \
    source/ui/dataman/AMScanViewPlotToolView.cpp \
    source/ui/dataman/AMScanViewPlotSelectedToolsView.cpp \
    source/dataman/AMScanViewPlotTools.cpp \
    source/ui/dataman/AMScanPlotView.cpp

RESOURCES *= source/icons/icons.qrc \
		source/configurationFiles/configurationFiles.qrc \
		source/util/ElementData.qrc \
		source/stylesheets/stylesheets.qrc


OTHER_FILES *= \
	source/stylesheets/sliderWaitLessThan.qss \
	source/stylesheets/sliderWaitGreaterThan.qss

contains(DEFINES, AM_BUILD_REPORTER_ENABLED){
	HEADERS *= source/util/AMRunTimeBuildInfo.h

	SOURCES *= source/util/AMRunTimeBuildInfo.cpp
}
































