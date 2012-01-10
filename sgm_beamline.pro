# #####################################################################
# QMake project file for acquaman, including test suites			January 2010. mark.boots@usask.ca
# #####################################################################

TEMPLATE = subdirs
SUBDIRS +=	SGMAcquaman.pro \
	pluginProjects/FileLoaders/AMFileLoaderPlugins.pro \
	CLSPGTDwellTimeCoordinator.pro \
        SGMSSAAcquaman.pro
