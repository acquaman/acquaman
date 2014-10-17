# #####################################################################
# QMake project file for acquaman, including test suites			January 2010. mark.boots@usask.ca
# #####################################################################

TEMPLATE = subdirs
SUBDIRS +=	pluginProjects/FileLoaders/AMFileLoaderPlugins.pro \
	# pluginProjects/AnalysisBlocks/AMAnalysisBlockPlugins.pro \
	REIXSTest.pro \
	REIXSAcquaman.pro \
	SGMAcquaman.pro \
	BareBonesAcquaman.pro \
	acquamanTest.pro \
	VESPERSAcquaman.pro \
	MidIRBPM.pro \
	CLSPGTDwellTimeCoordinator.pro \
	SGMAddOnsCoordinator.pro \
	SGMSSAAcquaman.pro \
	EnsureHeaderNotice.pro \
	VESPERSBendingMirrors.pro \
	SGMAmptekCoordinator.pro \
	SGMLookupTableCoordinator.pro \
	AMCrashReporter.pro \
	StripToolProject.pro \
	IDEASAcquaman.pro \
	BioXASSideAcquaman.pro \
	BioXASMainAcquaman.pro \
	BioXASImagingAcquaman.pro \
	BioXASToolSuite.pro \
	VESPERSDatabaseDuplicateEntryPatch.pro \
	StripTool2.pro \
	CLSNetworkDirectorySynchronizer.pro \
	XRDAnalysis.pro \
	CLSSR570Coordinator.pro
