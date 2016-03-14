# #####################################################################
# QMake project file for acquaman, including test suites			January 2010. mark.boots@usask.ca
# #####################################################################

TEMPLATE = subdirs
SUBDIRS += \
	Initialize.pro \
	pluginProjects/FileLoaders/AMFileLoaderPlugins.pro \
	CLSPGTDwellTimeCoordinator.pro \
	SGMAddOnsCoordinator.pro \
	SGMAmptekCoordinator.pro \
	SGMLookupTableCoordinator.pro \
	AMCrashReporter.pro \
	CLSNetworkDirectorySynchronizer.pro \
	SGMSSAAcquaman.pro \
        SGMAcquaman.pro
