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
	buildTests/AMBuildTest.pro \
	StripTool2.pro \
	CLSPGTDwellTimeCoordinator.pro \
#	AMPIC887DriverServer.pro \
	CLSSR570Coordinator.pro \
	PGMAcquaman.pro
