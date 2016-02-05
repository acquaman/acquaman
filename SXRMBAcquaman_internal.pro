include ( clsCommon.pri )

TARGET = SXRMBAcquaman

FORMS +=

HEADERS += \
	source/application/SXRMB/SXRMB.h \
	source/application/SXRMB/SXRMBAppController.h \
	source/beamline/SXRMB/SXRMBBeamline.h \
	source/beamline/SXRMB/SXRMBBrukerDetector.h \
	source/beamline/SXRMB/SXRMBFourElementVortexDetector.h \
	source/beamline/SXRMB/SXRMBHVControl.h \
	source/beamline/SXRMB/SXRMBCrystalChangeModel.h \
	source/acquaman/SXRMB/SXRMBXRFScanConfiguration.h \
	source/acquaman/SXRMB/SXRMBXRFScanController.h \
	source/acquaman/SXRMB/SXRMBEXAFSScanConfiguration.h \
	source/acquaman/SXRMB/SXRMBEXAFSScanActionController.h \
	source/acquaman/SXRMB/SXRMB2DMapScanConfiguration.h \
	source/acquaman/SXRMB/SXRMB2DScanActionController.h \
	source/acquaman/SXRMB/SXRMBScanConfiguration.h \
	source/acquaman/SXRMB/SXRMBScanConfigurationDbObject.h \
	source/dataman/SXRMB/SXRMBUserConfiguration.h \
	source/dataman/SXRMB/SXRMBDbUpgrade1pt1.h \
	source/ui/acquaman/SXRMB/SXRMBOxidationMapScanConfigurationViewHolder.h \
	source/ui/SXRMB/SXRMBPersistentView.h \
	source/ui/SXRMB/SXRMBScanConfigurationView.h \
	source/ui/SXRMB/SXRMBEXAFSScanConfigurationView.h \
	source/ui/SXRMB/SXRMB2DMapScanConfigurationView.h \
	source/ui/SXRMB/SXRMB2DOxidationMapScanConfigurationView.h \
	source/ui/SXRMB/SXRMBXRFDetailedDetectorView.h \
	source/ui/SXRMB/SXRMBBrukerDetectorView.h \
	source/ui/SXRMB/SXRMBFourElementVortexDetectorView.h \
	source/ui/SXRMB/SXRMBHVControlView.h \
	source/ui/SXRMB/SXRMBCrystalChangeView.h


SOURCES += \
	source/application/SXRMB/SXRMBMain.cpp \
	source/application/SXRMB/SXRMBAppController.cpp \
	source/beamline/SXRMB/SXRMBBeamline.cpp \
	source/beamline/SXRMB/SXRMBBrukerDetector.cpp \
	source/beamline/SXRMB/SXRMBFourElementVortexDetector.cpp \
	source/beamline/SXRMB/SXRMBHVControl.cpp \
	source/beamline/SXRMB/SXRMBCrystalChangeModel.cpp \
	source/acquaman/SXRMB/SXRMBXRFScanConfiguration.cpp \
	source/acquaman/SXRMB/SXRMBXRFScanController.cpp \
	source/acquaman/SXRMB/SXRMBEXAFSScanConfiguration.cpp \
	source/acquaman/SXRMB/SXRMBEXAFSScanActionController.cpp \
	source/acquaman/SXRMB/SXRMB2DMapScanConfiguration.cpp \
	source/acquaman/SXRMB/SXRMB2DScanActionController.cpp \
	source/acquaman/SXRMB/SXRMBScanConfiguration.cpp \
	source/acquaman/SXRMB/SXRMBScanConfigurationDbObject.cpp \
	source/dataman/SXRMB/SXRMBUserConfiguration.cpp \
	source/dataman/SXRMB/SXRMBDbUpgrade1pt1.cpp \
	source/ui/acquaman/SXRMB/SXRMBOxidationMapScanConfigurationViewHolder.cpp \
	source/ui/SXRMB/SXRMBPersistentView.cpp \
	source/ui/SXRMB/SXRMBScanConfigurationView.cpp \
	source/ui/SXRMB/SXRMBEXAFSScanConfigurationView.cpp \
	source/ui/SXRMB/SXRMB2DMapScanConfigurationView.cpp \
	source/ui/SXRMB/SXRMB2DOxidationMapScanConfigurationView.cpp \
	source/ui/SXRMB/SXRMBXRFDetailedDetectorView.cpp \
	source/ui/SXRMB/SXRMBBrukerDetectorView.cpp \
	source/ui/SXRMB/SXRMBFourElementVortexDetectorView.cpp \
	source/ui/SXRMB/SXRMBHVControlView.cpp \
	source/ui/SXRMB/SXRMBCrystalChangeView.cpp






