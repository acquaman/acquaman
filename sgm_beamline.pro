# #####################################################################
# QMake project file for acquaman, including test suites			January 2010. mark.boots@usask.ca
# #####################################################################

TEMPLATE = subdirs
SUBDIRS += \
	pluginProjects/FileLoaders/AMFileLoaderPlugins.pro \
	CLSPGTDwellTimeCoordinator.pro \
	SGMAddOnsCoordinator.pro \
	SGMAmptekCoordinator.pro \
	SGMLookupTableCoordinator.pro \
	SGMSSAAcquaman.pro \
	AMCrashReporter.pro \
        SGMAcquaman.pro
