# #####################################################################
# QMake project file for acquaman, including test suites			January 2010. mark.boots@usask.ca
# #####################################################################

TEMPLATE = subdirs
SUBDIRS += \
	Initialize.pro \
	pluginProjects/FileLoaders/AMFileLoaderPlugins.pro \
	EnsureHeaderNotice.pro \
	AMCrashReporter.pro \
	CLSNetworkDirectorySynchronizer.pro \
	BioXASAcquaman.pro \
	BioXASMainAcquaman.pro \
	BioXASMainTableCoordinator.pro \
	BioXASSideAcquaman.pro \
	BioXASSideTableCoordinator.pro \
	BioXASImagingAcquaman.pro \
	BioXASToolSuite.pro \
	REIXSAcquaman.pro \
	IDEASAcquaman.pro \
	SGMAcquaman.pro \
	SGMAddOnsCoordinator.pro \
	SGMAmptekCoordinator.pro \
	SGMLookupTableCoordinator.pro \
	SGMSSAAcquaman.pro \
	SXRMBAcquaman.pro \
	SXRMBAddOnsCoordinator.pro \
	VESPERSAcquaman.pro \
	VESPERSAddOnsCoordinator.pro \
	buildTests/AMBuildTest.pro \
	StripTool2.pro \
	CLSPGTDwellTimeCoordinator.pro \
	CLSSR570Coordinator.pro
