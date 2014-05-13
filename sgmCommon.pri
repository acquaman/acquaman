# #####################################################################
# QMake project file for SGM common files  	January 2012. david.k.chevrier@gmail.com
# Note: Set EPICS_INCLUDE_DIRS, EPICS_LIB_DIR, VLC_*, and GSL_* correctly for platform
# ####################################################################

# Automatically determines a user's home folder
HOME_FOLDER = $$system(echo $HOME)

macx {
		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming
}
linux-g++ {
		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming
}
linux-g++-32 {
		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming
}
# The following works well for CLS beamline OPI machines, built using VMSL54.cs.clsi.ca

linux-g++-64 {
		# Where you want to do your acquaman development (as a path from $HOME). You don't need to include leading or trailing slashes.
		DEV_PATH = beamline/programming
}

# Source Files (SGM Common)
#######################

FORMS += 	source/ui/SGMXASScanConfigurationViewer.ui

HEADERS += \
	source/application/SGM/SGMAppController.h \
	source/application/SGM/SGMSSAAppController.h \
	source/acquaman/SGM/SGMScanConfiguration.h \
	source/acquaman/SGM/SGMXASScanConfiguration.h \
	source/acquaman/SGM/SGMFastScanConfiguration.h \
	source/beamline/SGM/SGMBeamline.h \
	source/ui/SGM/SGMXASScanConfigurationView.h \
	source/ui/SGM/SGMSidebar.h \
	source/ui/SGM/SGMFastScanConfigurationView.h \
	source/ui/SGM/SGMSampleManipulatorView.h \
	source/dataman/SGM/SGMBeamlineInfo.h \
	source/util/SGM/SGMElementInfo.h \
	source/util/SGM/SGMPeriodicTable.h \
	source/ui/util/SGM/SGMEnergyPositionView.h \
	source/analysis/SGM/SGM1DFastScanFilterAB.h \
	source/util/SGM/SGMSettings.h \
	source/util/SGM/SGMPluginsLocation.h \
	source/ui/util/SGM/SGMSettingsMasterView.h \
	source/beamline/SGM/SGMMAXvMotor.h \
	source/ui/util/SGM/SGMFastScanSettingsView.h \
	source/dataman/SGM/SGMDbUpgrade1Pt1.h \
	source/beamline/SGM/SGMOptimizationSupport.h \
	source/ui/SGM/SGMAdvancedControlsView.h \
	source/ui/SGM/SGMPeriodicTableView.h \
#<<<<<<< HEAD
#    source/ui/util/SGM/SGMFastScanParametersView.h \
#    source/acquaman/SGM/SGMXASScanActionController.h \
#    source/acquaman/SGM/SGMScanConfiguration2013.h \
#    source/acquaman/SGM/SGMSScanConfigurationDbObject.h \
#    source/acquaman/SGM/SGMXASScanConfiguration2013.h \
#    source/ui/SGM/SGMXASScanConfiguration2013View.h \
#    source/ui/SGM/SGMFluxResolutionPickerView.h \
#    source/acquaman/SGM/SGMFastScanActionController.h \
#    source/acquaman/SGM/SGMFastScanConfiguration2013.h \
#    source/ui/SGM/SGMFastScanConfiguration2013View.h \
#    source/acquaman/SGM/SGMXASScanActionControllerFileWriter.h \
    source/ui/SGM/SGMSampleManipulatorMotorGroupView.h \
    source/beamline/SGM/SGMSampleManipulatorMotorGroup.h \
    source/ui/SGM/SGMSampleManagementView.h \
#=======
	source/ui/util/SGM/SGMFastScanParametersView.h \
	source/acquaman/SGM/SGMXASScanActionController.h \
	source/acquaman/SGM/SGMScanConfiguration2013.h \
	source/acquaman/SGM/SGMSScanConfigurationDbObject.h \
	source/acquaman/SGM/SGMXASScanConfiguration2013.h \
	source/ui/SGM/SGMXASScanConfiguration2013View.h \
	source/ui/SGM/SGMFluxResolutionPickerView.h \
	source/acquaman/SGM/SGMFastScanActionController.h \
	source/acquaman/SGM/SGMFastScanConfiguration2013.h \
	source/ui/SGM/SGMFastScanConfiguration2013View.h \
	source/ui/SGM/SGMAdvancedMirrorView.h \
	source/ui/SGM/SGMSIS3820ScalerView.h
#>>>>>>> SGM_Release

SOURCES += \
	source/application/SGM/SGMAppController.cpp \
	source/application/SGM/SGMSSAAppController.cpp \
	source/acquaman/SGM/SGMScanConfiguration.cpp \
	source/acquaman/SGM/SGMXASScanConfiguration.cpp \
	source/acquaman/SGM/SGMFastScanConfiguration.cpp \
	source/beamline/SGM/SGMBeamline.cpp \
	source/ui/SGM/SGMXASScanConfigurationView.cpp \
	source/ui/SGM/SGMSidebar.cpp \
	source/ui/SGM/SGMFastScanConfigurationView.cpp \
	source/ui/SGM/SGMSampleManipulatorView.cpp \
	source/dataman/SGM/SGMBeamlineInfo.cpp \
	source/util/SGM/SGMElementInfo.cpp \
	source/util/SGM/SGMPeriodicTable.cpp \
	source/ui/util/SGM/SGMEnergyPositionView.cpp \
	source/analysis/SGM/SGM1DFastScanFilterAB.cpp \
	source/util/SGM/SGMSettings.cpp \
	source/util/SGM/SGMPluginsLocation.cpp \
	source/ui/util/SGM/SGMSettingsMasterView.cpp \
	source/beamline/SGM/SGMMAXvMotor.cpp \
	source/ui/util/SGM/SGMFastScanSettingsView.cpp \
	source/dataman/SGM/SGMDbUpgrade1Pt1.cpp \
	source/beamline/SGM/SGMOptimizationSupport.cpp \
	source/ui/SGM/SGMAdvancedControlsView.cpp \
	source/ui/SGM/SGMPeriodicTableView.cpp \
#<<<<<<< HEAD
#    source/ui/util/SGM/SGMFastScanParametersView.cpp \
#    source/acquaman/SGM/SGMXASScanActionController.cpp \
#    source/acquaman/SGM/SGMScanConfiguration2013.cpp \
#    source/acquaman/SGM/SGMSScanConfigurationDbObject.cpp \
#    source/acquaman/SGM/SGMXASScanConfiguration2013.cpp \
#    source/ui/SGM/SGMXASScanConfiguration2013View.cpp \
#    source/ui/SGM/SGMFluxResolutionPickerView.cpp \
#    source/acquaman/SGM/SGMFastScanActionController.cpp \
#    source/acquaman/SGM/SGMFastScanConfiguration2013.cpp \
#    source/ui/SGM/SGMFastScanConfiguration2013View.cpp \
#    source/acquaman/SGM/SGMXASScanActionControllerFileWriter.cpp \
    source/ui/SGM/SGMSampleManipulatorMotorGroupView.cpp \
    source/beamline/SGM/SGMSampleManipulatorMotorGroup.cpp \
    source/ui/SGM/SGMSampleManagementView.cpp \
#=======
	source/ui/util/SGM/SGMFastScanParametersView.cpp \
	source/acquaman/SGM/SGMXASScanActionController.cpp \
	source/acquaman/SGM/SGMScanConfiguration2013.cpp \
	source/acquaman/SGM/SGMSScanConfigurationDbObject.cpp \
	source/acquaman/SGM/SGMXASScanConfiguration2013.cpp \
	source/ui/SGM/SGMXASScanConfiguration2013View.cpp \
	source/ui/SGM/SGMFluxResolutionPickerView.cpp \
	source/acquaman/SGM/SGMFastScanActionController.cpp \
	source/acquaman/SGM/SGMFastScanConfiguration2013.cpp \
	source/ui/SGM/SGMFastScanConfiguration2013View.cpp \
	source/ui/SGM/SGMAdvancedMirrorView.cpp \
	source/ui/SGM/SGMSIS3820ScalerView.cpp
#>>>>>>> SGM_Release
