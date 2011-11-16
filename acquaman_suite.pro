# #####################################################################
# QMake project file for acquaman, including test suites			January 2010. mark.boots@usask.ca
# #####################################################################

TEMPLATE = subdirs
SUBDIRS +=	pluginProjects/FileLoaders/AMFileLoaderPlugins.pro \
	pluginProjects/AnalysisBlocks/AMAnalysisBlockPlugins.pro \
        REIXSTest.pro \
	REIXSAcquaman.pro \
	SGMAcquaman.pro \
	BareBonesAcquaman.pro \
	acquamanTest.pro \
	VESPERSAcquaman.pro \
	# VESPERSDataman.pro \
	#AcquaCam.pro \
	XRFSpectraViewer.pro \
	MidIRBPM.pro \
	CLSPGTDwellTimeCoordinator.pro \
        SGMSSAAcquaman.pro
