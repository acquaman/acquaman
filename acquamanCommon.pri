# #####################################################################
# QMake project file for acquaman.  		January 2010. mark.boots@usask.ca
# Note: Set EPICS_INCLUDE_DIRS, EPICS_LIB_DIR, VLC_*, and GSL_* correctly for platform
# ####################################################################

# Video Support: Remove this line if you do not have the multimedia module from QtMobility
CONFIG += mobility

# Debug: Uncomment this to build the program in debug mode (no optimizations; include debugging symbols.)
# Note that as of November 18, 2011, building in debug mode triggers a failure in the dacq library: the main (eV) PV ends up disabled in the dacq scan config.  This is likely a serious memory error.
# CONFIG += debug

USERNAME = $$system(whoami)

# Automatically determines a user's home folder
HOME_FOLDER = $$system(echo $HOME)

USERNAME = $$system(whoami)

macx {

contains(USERNAME, helfrij){
	CONFIG -= mobility
}

		# Disable Qt Mobility Video until everyone's Mac laptops support that
		#CONFIG -= mobility

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman_4_8

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/include \
								$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/include/os/Darwin
		EPICS_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/epics/base/lib/darwin-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot_4_8/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot_4_8/lib

		GSL_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/include

		# GSL Dependencies
		GSL_LIB = -L$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/lib -lgsl
		GSL_CBLAS_LIB = -L$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/lib -lgslcblas

		# QwtPlot3d dependencies (Disabled for now...)
		 #QWTPLOT3D_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman_4_8/contrib/qwtplot3d/lib
		 #QWTPLOT3D_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman_4_8/contrib/qwtplot3d/include

		# CDFlib dependencies
		CDF_LIB = /Applications/cdf34_0-dist/lib/libcdf.a
		CDF_INCLUDE_DIR = /Applications/cdf34_0-dist/include

				#Eigen
		EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen

		MOBILITY_QT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility/qt-mobility-opensource-src-1.1.3/install/Library/Frameworks/QtMultimediaKit.framework/Versions/Current
		MOBILITY_QT_LIB = -L$$MOBILITY_QT_LIB_DIR -lQtMultimediaKit
		MOBILITY_QT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility/qt-mobility-opensource-src-1.1.3/install/include/QtMultimediaKit \
						$$HOME_FOLDER/$$DEV_PATH/qt-mobility/qt-mobility-opensource-src-1.1.3/install/include/QtMobility

		QMAKE_CXXFLAGS_X86_64 += "-mmacosx-version-min=10.7"

		QMAKE_LFLAGS_DEBUG += "-mmacosx-version-min=10.7"
		QMAKE_LFLAGS_RELEASE += "-mmacosx-version-min=10.7"
}
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


#                GSL_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/include
#                GSL_LIB = -L$$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/gsl-install/lib -lgsl
#                GSL_CBLAS_LIB = -lgslcblas

		# GSL Dependencies
		GSL_INCLUDE_DIR = $$PATH_TO_AM/contrib/gsl-install/include
		GSL_LIB = -L$$PATH_TO_AM/contrib/gsl-install/lib -lgsl
		GSL_CBLAS_LIB = -L$$PATH_TO_AM/contrib/gsl-install/lib -lgslcblas


				#Qt Mobility Dependencies
				MOBILITY_QT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-1.1.3-Ubuntu12.04/lib
				MOBILITY_QT_LIB = -L$$MOBILITY_QT_LIB_DIR -lQtMultimediaKit
				MOBILITY_QT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/qt-mobility-1.1.3-Ubuntu12.04/include

				#Eigen
		EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen

		# CDFlib dependencies
#		CDF_LIB = -lcdf
#		CDF_INCLUDE_DIR = /usr/local/include
		CDF_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/cdf34_1-dist/lib
		CDF_LIB = -L$$CDF_LIB_DIR -lcdf
		CDF_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/cdf34_1-dist/include


		QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$CDF_LIB_DIR"
}
linux-g++-32 {

		# Disable Qt Mobility Video until Darren's laptop is ready for that.
		CONFIG -= mobility

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
		GSL_LIB = -lgsl
		GSL_CBLAS_LIB = -lgslcblas

		# QwtPlot3d dependencies (Disabled for now...)
#		QWTPLOT3D_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/qwtplot3d/lib
#		QWTPLOT3D_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/qwtplot3d/include

		# CDFlib dependencies
		CDF_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/cdf34_1-dist/lib
		CDF_LIB = -L$$CDF_LIB_DIR -lcdf
		CDF_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/cdf34_1-dist/include

		QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$CDF_LIB_DIR"

				#Eigen
		EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen
}
# The following works well for CLS beamline OPI machines, built using VMSL54.cs.clsi.ca

linux-g++-64 {

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH/acquaman

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = /home/epics/src/R3.14.12/base/include \
				/home/epics/src/R3.14.12/base/include/os/Linux
		EPICS_LIB_DIR = /home/epics/src/R3.14.12/base/lib/linux-x86_64

		# MPlot Source
		MPLOT_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/include
		MPLOT_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/MPlot/lib

		# GSL Dependencies
		GSL_INCLUDE_DIR = /home/beamline/tools/gsl/gsl-1.14-install/include
		GSL_LIB = -L/home/beamline/tools/gsl/gsl-1.14-install/lib -lgsl
		GSL_CBLAS_LIB = -lgslcblas

		# QwtPlot3d dependencies (Disabled for now...)
		# QWTPLOT3D_LIB_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/qwtplot3d/lib
		# QWTPLOT3D_INCLUDE_DIR = $$HOME_FOLDER/$$DEV_PATH/acquaman/contrib/qwtplot3d/include

		# CDFlib dependencies
		CDF_LIB_DIR = /home/beamline/tools/cdf/lib
		CDF_LIB = -L$$CDF_LIB_DIR -lcdf
		CDF_INCLUDE_DIR = /home/beamline/tools/cdf/include

		QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$CDF_LIB_DIR"
		QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$CDF_LIB_DIR"

				#Eigen
		EIGEN_INCLUDE_DIR = $$PATH_TO_AM/source/Eigen

}

# Special build paths and options for running on the Jenkins auto-build server (currently at http://beamteam.usask.ca:8080)
CONFIG(jenkins_build) {

		message("Detected Jenkins auto-build... Specifying dependency paths for the build server.")

		# Disable Qt Mobility Video until the Jenkins-machine supports that
		CONFIG -= mobility

		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = /jobs/AcquamanOnLinux_MasterBranch/workspace

		# The full path to the acquaman folder.  This MUST point to the location where acquamanCommon.pri lives.
		PATH_TO_AM = $$HOME_FOLDER/$$DEV_PATH

		# EPICS Dependencies:
		EPICS_INCLUDE_DIRS = /home/mark/dev/epics/base/include \
				/home/mark/dev/epics/base/include/os/Linux
		EPICS_LIB_DIR = /home/mark/dev/epics/base/lib/linux-x86

		# MPlot Source
		MPLOT_INCLUDE_DIR = "/var/lib/jenkins/jobs/MPlotOnLinux_MasterBranch/workspace/include"
		MPLOT_LIB_DIR = "/var/lib/jenkins/jobs/MPlotOnLinux_MasterBranch/workspace/lib"

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
		$$MPLOT_INCLUDE_DIR \
		$$GSL_INCLUDE_DIR \
		$$QWTPLOT3D_INCLUDE_DIR \
				$$CDF_INCLUDE_DIR \
				$$EIGEN_INCLUDE_DIR

LIBS += $$GSL_LIB \
		$$GSL_CBLAS_LIB \
		-L$$MPLOT_LIB_DIR -lMPlot \
#		-L$$QWTPLOT3D_LIB_DIR -lqwtplot3d \
		-L$$EPICS_LIB_DIR -lca -lCom \
		$$CDF_LIB

CONFIG(mobility) {
		INCLUDEPATH += $$MOBILITY_QT_INCLUDE_DIR
		LIBS += $$MOBILITY_QT_LIB
}

DEFINES += AM_ENABLE_BOUNDS_CHECKING


# Set standard level of compiler warnings for everyone. (Otherwise the warnings shown will be system-dependent.)
QMAKE_CXXFLAGS += -Wextra -g

# Specify RPATH (runtime library search paths) so that libraries can be found without requiring LD_LIBRARY_PATH
# For Qt 4.7.1 and earlier, need to use this instead:
								#QMAKE_LFLAGS_RPATH += "$$EPICS_LIB_DIR"
								#QMAKE_LFLAGS_RPATH += "$$QWTPLOT3D_LIB_DIR"

contains(USERNAME, helfrij){
	QMAKE_CXXFLAGS_X86_64 += "-mmacosx-version-min=10.7"

	QMAKE_LFLAGS_DEBUG += "-mmacosx-version-min=10.7"
	QMAKE_LFLAGS_RELEASE += "-mmacosx-version-min=10.7"

	#QMAKE_LFLAGS_DEBUG += "-mmacosx-version-min=10.7"
	#QMAKE_LFLAGS_RELEASE += "-mmacosx-version-min=10.7"
}

contains(USERNAME, chevrid){
	QMAKE_CXXFLAGS_X86_64 += "-mmacosx-version-min=10.7"

	QMAKE_LFLAGS_DEBUG += "-mmacosx-version-min=10.7"
	QMAKE_LFLAGS_RELEASE += "-mmacosx-version-min=10.7"
}

QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$EPICS_LIB_DIR"
QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$EPICS_LIB_DIR"

QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,$$MPLOT_LIB_DIR"
QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,$$MPLOT_LIB_DIR"

# Source Files (Acquaman Framework Common)
#######################

HEADERS += \
	source/acquaman/AMScanConfiguration.h \
	source/acquaman/AMScanController.h \
	source/application/AMAppController.h \
	source/util/AMBiHash.h \
	source/util/AMErrorMonitor.h \
	source/util/AMSettings.h \
	source/beamline/AMBeamline.h \
	source/beamline/AMControl.h \
	source/beamline/AMPVControl.h \
	source/beamline/AMControlSet.h \
#	source/beamline/AMControlState.h \
	source/beamline/AMProcessVariable.h \
	source/beamline/AMProcessVariablePrivate.h \
	source/beamline/AMPVNames.h \
	source/dataman/database/AMDatabase.h \
	source/dataman/database/AMDbObject.h \
	source/dataman/AMExperiment.h \
	source/dataman/AMImportController.h \
	source/dataman/AMRun.h \
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
	source/ui/dataman/AMFirstTimeWidget.h \
	source/ui/dataman/AMImportControllerWidget.h \
	source/ui/AMMainWindow.h \
	source/ui/dataman/AMRunExperimentInsert.h \
	source/ui/acquaman/AMScanConfigurationView.h \
	source/ui/dataman/AMScanView.h \
	source/ui/AMSidebar.h \
	source/ui/AMStatusView.h \
	source/ui/AMThumbnailScrollViewer.h \
	source/ui/AMBottomBar.h \
	source/ui/beamline/AMControlEditor.h \
	source/acquaman.h \
	source/ui/dataman/AMNewRunDialog.h \
	source/dataman/AMAbstractFileLoader.h \
	source/ui/AMPrefixSuffixLineEdit.h \
	source/ui/AMDragDropItemModel.h \
	source/dataman/AMRunExperimentItems.h \
	source/dataman/info/AMControlInfoList.h \
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
	source/dataman/REIXS/REIXSXESRawFileLoader.h \
	source/util/AMDeferredFunctionCall.h \
	source/ui/util/AMPeriodicTableView.h \
	source/util/AMPeriodicTable.h \
	source/util/AMElement.h \
	source/dataman/info/AMSpectralOutputDetectorInfo.h \
	source/dataman/SGM/SGMMCPDetectorInfo.h \
	source/dataman/info/CLSPGTDetectorInfo.h \
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
	#source/ui/dataman/AM3dDataSourceView.h \
	source/ui/AMTopFrame.h \
	source/dataman/info/CLSOceanOptics65000DetectorInfo.h \
	source/dataman/SGM/SGM2011XASFileLoader.h \
	source/beamline/CLS/CLSMAXvMotor.h \
	source/analysis/AM1DDerivativeAB.h \
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
	source/dataman/VESPERS/VESPERSXASDataLoader.h \
	source/dataman/AMFileLoaderInterface.h \
	source/ui/util/AMSettingsView.h \
	source/dataman/AMScanDictionary.h \
	source/dataman/AMScanParametersDictionary.h \
	source/dataman/AMScanExemplarDictionary.h \
	source/dataman/AMScanExemplar.h \
	source/ui/dataman/AMDictionaryLineEdit.h \
	source/beamline/CLS/CLSBiStateControl.h \
	source/application/AMAppControllerSupport.h \
	source/application/AMPluginsManager.h \
	source/dataman/import/AMScanDatabaseImportController.h \
	source/ui/dataman/AMScanDatabaseImportWizard.h \
	source/beamline/CLS/CLSMDriveMotorControl.h \
	source/ui/beamline/AMControlMoveButton.h \
	source/beamline/AMSampleManipulator.h \
	source/beamline/AMControlSetSampleManipulator.h \
	source/ui/CLS/CLSStopLightButton.h \
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
	source/dataman/export/AMExporterAthena.h \
	source/analysis/AM1DBasicDerivativeABEditor.h \
	source/analysis/AM1DIntegralAB.h \
	source/analysis/AM1DBasicIntegralABEditor.h \
	source/util/AMJoystick.h \
	source/ui/dataman/AMControlInfoListTableView.h \
	source/dataman/AM2DScan.h \
	source/dataman/info/CLSAmptekSDD123DetectorInfo.h \
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
	source/actions3/AMActionInfo3.h \
	source/actions3/AMAction3.h \
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
	source/dataman/REIXS/REIXSXESCalibration2.h \
	source/ui/beamline/AMExtendedControlEditor.h \
	source/ui/beamline/AMControlButton.h \
	source/dataman/info/AMControlInfo.h \
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
	source/beamline/AMDetector.h \
	source/dataman/AMDbUpgrade1Pt1.h \
	source/dataman/AMDbUpgrade1Pt2.h \
	source/dataman/info/AMDetectorInfo.h \
	source/beamline/CLS/CLSAmptekSDD123DetectorNew.h \
	source/ui/beamline/AMOldDetectorView.h \
	source/ui/beamline/AMOldDetectorViewSupport.h \
	source/ui/beamline/AMDetectorView.h \
	source/beamline/AMSynchronizedDwellTime.h \
	source/beamline/AMDetectorSet.h \
	source/dataman/info/AMDetectorInfoSet.h \
	source/ui/beamline/AMDetectorSelectorView.h \
	source/beamline/CLS/CLSBasicScalerChannelDetector.h \
	source/beamline/AMDetectorTriggerSource.h \
	source/beamline/AMDetectorGroup.h \
	source/beamline/AMDetectorSelector.h \
	source/acquaman/AMAgnosticDataAPI.h \
	source/acquaman/AMScanActionController.h \
	source/acquaman/AMScanActionControllerScanAssembler.h \
	source/actions3/actions/AMDetectorInitializeActionInfo.h \
	source/actions3/actions/AMDetectorInitializeAction.h \
	source/actions3/actions/AMDetectorAcquisitionActionInfo.h \
	source/actions3/actions/AMDetectorAcquisitionAction.h \
	source/actions3/actions/AMDetectorCleanupActionInfo.h \
	source/actions3/actions/AMDetectorCleanupAction.h \
	source/actions3/actions/AMAxisStartedActionInfo.h \
	source/actions3/actions/AMAxisFinishedActionInfo.h \
	source/actions3/actions/AMAxisStartedAction.h \
	source/actions3/actions/AMAxisFinishedAction.h \
	source/beamline/AMBasicControlDetectorEmulator.h \
	source/actions3/actions/AMDetectorTriggerActionInfo.h \
	source/actions3/actions/AMDetectorTriggerAction.h \
	source/actions3/actions/AMDetectorReadActionInfo.h \
	source/actions3/actions/AMDetectorReadAction.h \
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
	source/ui/util/AMMessageBoxWTimeout.h \
	source/beamline/CLS/CLSPGTDetectorV2.h \
	source/beamline/CLS/CLSQE65000Detector.h \
	source/actions3/actions/AMDetectorDwellTimeActionInfo.h \
	source/actions3/actions/AMDetectorDwellTimeAction.h \
	source/acquaman/AMDetectorDwellTimeSourceScanOptimizer.h \
	source/beamline/CLS/CLSAdvancedScalerChannelDetector.h \
	source/dataman/info/CLSSynchronizedDwellTimeConfigurationInfo.h \
	source/beamline/CLS/CLSSynchronizedDwellTimeConfiguration.h \
	source/analysis/AM3DNormalizationAB.h \
	source/analysis/AM3DNormalizationABEditor.h \
	source/analysis/AM4DBinningAB.h \
	source/analysis/AM4DBinningABEditor.h \
	source/analysis/AMOrderReductionAB.h \
	source/analysis/AMOrderReductionABEditor.h \
	source/beamline/AMMotorGroup.h \
	source/ui/AMMotorGroupView.h \
	source/ui/CLS/CLSPseudoMotorGroupView.h \
	source/beamline/CLS/CLSPseudoMotorGroup.h \
	source/util/AMPointerTree.h \
	source/dataman/database/AMConstDbObject.h \
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
	source/ui/CLS/CLSPGTDetectorV2View.h \
	source/ui/CLS/CLSAmptekSDD123DetectorNewView.h \
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
	source/dataman/export/SGM/SGMAxis2000Exporter.h \
	source/ui/CLS/CLSAmptekSDD123DetailedDetectorView.h \
	source/actions3/AMActionSupport.h \
	source/ui/CLS/CLSSIS3820CompositeScalerChannelView.h \
	source/beamline/CLS/CLSBasicCompositeScalerChannelDetector.h \
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
	source/actions3/actions/AMDetectorSetAsDarkCurrentCorrectionActionInfo.h \
	source/actions3/actions/AMDetectorSetAsDarkCurrentCorrectionAction.h \
	source/actions3/actions/AMControlWaitActionInfo.h \
	source/actions3/actions/AMControlWaitAction.h \
	source/analysis/AM1DDarkCurrentCorrectionAB.h \
	source/actions3/actions/AMDoingDarkCurrentCorrectionActionInfo.h \
	source/actions3/actions/AMDoingDarkCurrentCorrectionAction.h \
	source/actions3/actions/AMDoDarkCurrentCorrectionAction.h \
	source/actions3/actions/AMDoDarkCurrentCorrectionActionInfo.h \
	source/ui/CLS/CLSSIS3820ScalerChannelViewWithDarkCurrent.h \
	source/analysis/AM1DDarkCurrentCorrectionABEditor.h \
	source/ui/CLS/CLSDarkCurrentWidget.h \
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
	source/beamline/CLS/CLSPseudoMotorControl.h \
	source/beamline/CLS/CLSKeithley428.h \
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
	source/ui/dataman/AMScanThumbnailView.h \
	source/ui/dataman/AMScanTreeView.h \
	source/util/AMRecursiveDirectoryCompare.h \
	source/util/AMDirectorySynchronizer.h \
	source/ui/util/AMDirectorySynchronizerDialog.h \
	source/actions3/actions/AMDirectorySynchronizationAction.h \
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
	source/util/AMThread.h \
	source/dataman/AMDbUpgrade1Pt5.h \
	source/util/AMAppArgumentParser.h \
	source/beamline/CLS/CLSSR570.h \
    source/ui/dataman/AMBrowseScansView.h

# OS-specific files:
linux-g++|linux-g++-32|linux-g++-64 {
	 HEADERS +=
}

# Only when the Mobility Video framework is available:
CONFIG(mobility) {
DEFINES += AM_MOBILITY_VIDEO_ENABLED

HEADERS += source/ui/AMCrosshairOverlayVideoWidget.h \
	source/ui/AMOverlayVideoWidget.h \
	source/ui/AMBeamlineCameraWidget.h \
	source/ui/AMBeamlineCameraWidgetWithSourceTabs.h \
	source/ui/AMBeamlineCameraBrowser.h
}

FORMS += \
	source/ui/dataman/AMImportControllerWidget.ui \
	source/ui/AMBottomBar.ui \
	source/ui/dataman/AMGenericScanEditor.ui \
	source/ui/dataman/AMDataSourcesEditor.ui \
	source/ui/dataman/AMSamplePlateSelector.ui \
	source/ui/dataman/AMExporterOptionGeneralAsciiView.ui \
	source/ui/AMLinePropertyEditor.ui \
	source/ui/dataman/AMImagePropertyEditor.ui \
	source/ui/actions3/AMAddActionDialog3.ui \
	source/ui/AMTopFrame2.ui

SOURCES += \
	source/acquaman/AMScanConfiguration.cpp \
	source/acquaman/AMScanController.cpp \
	source/application/AMAppController.cpp \
	source/util/AMErrorMonitor.cpp \
	source/util/AMSettings.cpp \
	source/beamline/AMBeamline.cpp \
	source/beamline/AMControl.cpp \
	source/beamline/AMPVControl.cpp \
	source/beamline/AMControlSet.cpp \
#	source/beamline/AMControlState.cpp \
	source/beamline/AMProcessVariable.cpp \
	source/beamline/AMProcessVariablePrivate.cpp \
	source/beamline/AMPVNames.cpp \
	source/dataman/database/AMDatabase.cpp \
	source/dataman/database/AMDbObject.cpp \
	source/dataman/AMExperiment.cpp \
	source/dataman/AMImportController.cpp \
	source/dataman/AMRun.cpp \
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
	source/ui/AMMainWindow.cpp \
	source/ui/dataman/AMRunExperimentInsert.cpp \
	source/ui/dataman/AMScanView.cpp \
	source/ui/AMSidebar.cpp \
	source/ui/AMStatusView.cpp \
	source/ui/AMThumbnailScrollViewer.cpp \
	source/ui/AMBottomBar.cpp \
	source/ui/beamline/AMControlEditor.cpp \
	source/ui/dataman/AMNewRunDialog.cpp \
	source/ui/AMPrefixSuffixLineEdit.cpp \
	source/ui/AMDragDropItemModel.cpp \
	source/dataman/AMRunExperimentItems.cpp \
	source/dataman/info/AMControlInfoList.cpp \
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
	source/dataman/REIXS/REIXSXESRawFileLoader.cpp \
	source/util/AMDeferredFunctionCall.cpp \
	source/ui/util/AMPeriodicTableView.cpp \
	source/util/AMPeriodicTable.cpp \
	source/util/AMElement.cpp \
	source/dataman/info/AMSpectralOutputDetectorInfo.cpp \
	source/dataman/SGM/SGMMCPDetectorInfo.cpp \
	source/dataman/info/CLSPGTDetectorInfo.cpp \
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
	#source/ui/dataman/AM3dDataSourceView.cpp \
	source/dataman/info/CLSOceanOptics65000DetectorInfo.cpp \
	source/dataman/SGM/SGM2011XASFileLoader.cpp \
	source/beamline/CLS/CLSMAXvMotor.cpp \
	source/analysis/AM1DDerivativeAB.cpp \
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
	source/dataman/VESPERS/VESPERSXASDataLoader.cpp \
	source/ui/util/AMSettingsView.cpp \
	source/dataman/AMScanDictionary.cpp \
	source/dataman/AMScanParametersDictionary.cpp \
	source/dataman/AMScanExemplarDictionary.cpp \
	source/dataman/AMScanExemplar.cpp \
	source/ui/dataman/AMDictionaryLineEdit.cpp \
	source/beamline/CLS/CLSBiStateControl.cpp \
	source/application/AMPluginsManager.cpp \
	source/application/AMAppControllerSupport.cpp \
	source/dataman/import/AMScanDatabaseImportController.cpp \
	source/ui/dataman/AMScanDatabaseImportWizard.cpp \
	source/beamline/CLS/CLSMDriveMotorControl.cpp \
	source/ui/beamline/AMControlMoveButton.cpp \
	source/beamline/AMControlSetSampleManipulator.cpp \
	source/ui/CLS/CLSStopLightButton.cpp \
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
	source/dataman/export/AMExporterAthena.cpp \
	source/analysis/AM1DBasicDerivativeABEditor.cpp \
	source/analysis/AM1DIntegralAB.cpp \
	source/analysis/AM1DBasicIntegralABEditor.cpp \
	source/util/AMJoystick.cpp \
	source/ui/dataman/AMControlInfoListTableView.cpp \
	source/dataman/AM2DScan.cpp \
	source/dataman/info/CLSAmptekSDD123DetectorInfo.cpp \
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
	source/actions3/AMActionInfo3.cpp \
	source/actions3/AMAction3.cpp \
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
	source/dataman/REIXS/REIXSXESCalibration2.cpp \
	source/ui/beamline/AMExtendedControlEditor.cpp \
	source/ui/beamline/AMControlButton.cpp \
	source/dataman/info/AMControlInfo.cpp \
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
	source/beamline/AMDetector.cpp \
	source/dataman/AMDbUpgrade1Pt1.cpp \
	source/dataman/AMDbUpgrade1Pt2.cpp \
	source/dataman/info/AMDetectorInfo.cpp \
	source/beamline/CLS/CLSAmptekSDD123DetectorNew.cpp \
	source/ui/beamline/AMOldDetectorViewSupport.cpp \
	source/ui/beamline/AMOldDetectorView.cpp \
	source/ui/beamline/AMDetectorView.cpp \
	source/beamline/AMSynchronizedDwellTime.cpp \
	source/beamline/AMDetectorSet.cpp \
	source/dataman/info/AMDetectorInfoSet.cpp \
	source/ui/beamline/AMDetectorSelectorView.cpp \
	source/beamline/CLS/CLSBasicScalerChannelDetector.cpp \
	source/beamline/AMDetectorTriggerSource.cpp \
	source/beamline/AMDetectorGroup.cpp \
	source/beamline/AMDetectorSelector.cpp \
	source/acquaman/AMAgnosticDataAPI.cpp \
	source/acquaman/AMScanActionController.cpp \
	source/acquaman/AMScanActionControllerScanAssembler.cpp \
	source/actions3/actions/AMDetectorInitializeActionInfo.cpp \
	source/actions3/actions/AMDetectorInitializeAction.cpp \
	source/actions3/actions/AMDetectorAcquisitionActionInfo.cpp \
	source/actions3/actions/AMDetectorAcquisitionAction.cpp \
	source/actions3/actions/AMDetectorCleanupActionInfo.cpp \
	source/actions3/actions/AMDetectorCleanupAction.cpp \
	source/actions3/actions/AMAxisStartedActionInfo.cpp \
	source/actions3/actions/AMAxisFinishedActionInfo.cpp \
	source/actions3/actions/AMAxisStartedAction.cpp \
	source/actions3/actions/AMAxisFinishedAction.cpp \
	source/beamline/AMBasicControlDetectorEmulator.cpp \
	source/actions3/actions/AMDetectorTriggerActionInfo.cpp \
	source/actions3/actions/AMDetectorTriggerAction.cpp \
	source/actions3/actions/AMDetectorReadActionInfo.cpp \
	source/actions3/actions/AMDetectorReadAction.cpp \
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
	source/ui/util/AMMessageBoxWTimeout.cpp \
	source/beamline/CLS/CLSPGTDetectorV2.cpp \
	source/beamline/CLS/CLSQE65000Detector.cpp \
	source/actions3/actions/AMDetectorDwellTimeActionInfo.cpp \
	source/actions3/actions/AMDetectorDwellTimeAction.cpp \
	source/acquaman/AMDetectorDwellTimeSourceScanOptimizer.cpp \
	source/beamline/CLS/CLSAdvancedScalerChannelDetector.cpp \
	source/dataman/info/CLSSynchronizedDwellTimeConfigurationInfo.cpp \
	source/beamline/CLS/CLSSynchronizedDwellTimeConfiguration.cpp \
	source/analysis/AM3DNormalizationAB.cpp \
	source/analysis/AM3DNormalizationABEditor.cpp \
	source/analysis/AM4DBinningAB.cpp \
	source/analysis/AM4DBinningABEditor.cpp \
	source/analysis/AMOrderReductionAB.cpp \
	source/analysis/AMOrderReductionABEditor.cpp \
	source/beamline/AMMotorGroup.cpp \
	source/ui/AMMotorGroupView.cpp \
	source/ui/CLS/CLSPseudoMotorGroupView.cpp \
	source/beamline/CLS/CLSPseudoMotorGroup.cpp \
	source/util/AMPointerTree.cpp \
	source/dataman/database/AMConstDbObject.cpp \
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
	source/ui/CLS/CLSPGTDetectorV2View.cpp \
	source/ui/CLS/CLSAmptekSDD123DetectorNewView.cpp \
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
	source/dataman/export/SGM/SGMAxis2000Exporter.cpp \
	source/ui/CLS/CLSAmptekSDD123DetailedDetectorView.cpp \
	source/ui/CLS/CLSSIS3820CompositeScalerChannelView.cpp \
	source/beamline/CLS/CLSBasicCompositeScalerChannelDetector.cpp \
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
	source/actions3/actions/AMDetectorSetAsDarkCurrentCorrectionActionInfo.cpp \
	source/actions3/actions/AMDetectorSetAsDarkCurrentCorrectionAction.cpp \
	source/actions3/actions/AMControlWaitActionInfo.cpp \
	source/actions3/actions/AMControlWaitAction.cpp \
	source/analysis/AM1DDarkCurrentCorrectionAB.cpp \
	source/actions3/actions/AMDoingDarkCurrentCorrectionActionInfo.cpp \
	source/actions3/actions/AMDoingDarkCurrentCorrectionAction.cpp \
	source/actions3/actions/AMDoDarkCurrentCorrectionAction.cpp \
	source/actions3/actions/AMDoDarkCurrentCorrectionActionInfo.cpp \
	source/ui/CLS/CLSSIS3820ScalerChannelViewWithDarkCurrent.cpp \
	source/analysis/AM1DDarkCurrentCorrectionABEditor.cpp \
	source/ui/CLS/CLSDarkCurrentWidget.cpp \
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
	source/beamline/CLS/CLSPseudoMotorControl.cpp \
	source/beamline/CLS/CLSKeithley428.cpp \
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
	source/ui/dataman/AMScanThumbnailView.cpp \
	source/ui/dataman/AMScanTreeView.cpp \
	source/util/AMRecursiveDirectoryCompare.cpp \
	source/util/AMDirectorySynchronizer.cpp \
	source/ui/util/AMDirectorySynchronizerDialog.cpp \
	source/actions3/actions/AMDirectorySynchronizationAction.cpp \
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
	source/util/AMThread.cpp \
	source/dataman/AMDbUpgrade1Pt5.cpp \
	source/util/AMAppArgumentParser.cpp \
	source/beamline/CLS/CLSSR570.cpp \
    source/ui/dataman/AMBrowseScansView.cpp

# OS-specific files
linux-g++|linux-g++-32|linux-g++-64 {
	SOURCES +=
}

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

contains(DEFINES, AM_BUILD_REPORTER_ENABLED){
	HEADERS += source/util/AMRunTimeBuildInfo.h

	SOURCES += source/util/AMRunTimeBuildInfo.cpp
}
