# #####################################################################
# QMake project file for acquaman, including test suites			January 2010. mark.boots@usask.ca
# #####################################################################

TEMPLATE = subdirs
SUBDIRS +=	REIXSAcquaman.pro \
		pluginProjects/FileLoaders/AMFileLoaderPlugins.pro \
		AMCrashReporter.pro
