# #####################################################################
# QMake project file for acquaman, including test suites			January 2010. mark.boots@usask.ca
# #####################################################################

TEMPLATE = subdirs
SUBDIRS += \
	Initialize.pro \
	pluginProjects/FileLoaders/AMFileLoaderPlugins.pro \
	EnsureHeaderNotice.pro \
#	AMPIC887DriverServer.pro \
	AMCrashReporter.pro \
	buildTests/AMBuildTest.pro \
	StripTool2.pro \
	CLSPGTDwellTimeCoordinator.pro \
	CLSSR570Coordinator.pro \
	CLSNetworkDirectorySynchronizer.pro \
	BioXASAcquaman.pro \
	REIXSAcquaman.pro \
	IDEASAcquaman.pro \
	SGMAcquaman.pro \
	SGMEnergyCoordinator.pro \
#	SGMAmptekCoordinator.pro \
#	SGMLookupTableCoordinator.pro \
	SXRMBAcquaman.pro \
	SXRMBAddOnsCoordinator.pro \
	VESPERSAcquaman.pro \
	VESPERSAddOnsCoordinator.pro \
	MockAcquaman.pro \
	PGMAcquaman.pro
