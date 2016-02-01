# #####################################################################
# QMake project file for acquaman, including test suites			January 2010. mark.boots@usask.ca
# #####################################################################

TEMPLATE = subdirs
SUBDIRS +=	Initialize.pro \
		pluginProjects/FileLoaders/AMFileLoaderPlugins.pro \
		pluginProjects/FileLoaders/VESPERS2011XRFFileLoader/VESPERS2011XRFFileLoader.pro \
		SXRMBAcquaman.pro \
		SXRMBAddOnsCoordinator.pro \
		AMCrashReporter.pro
