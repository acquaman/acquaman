# #####################################################################
# QMake project file for acquaman, including test suites			January 2010. mark.boots@usask.ca
# #####################################################################

TEMPLATE = subdirs
SUBDIRS +=	Initialize.pro \
		pluginProjects/FileLoaders/AMFileLoaderPlugins.pro \
		VESPERSAcquaman.pro \
		VESPERSAddOnsCoordinator.pro \
		AMCrashReporter.pro \
		CLSNetworkDirectorySynchronizer.pro
