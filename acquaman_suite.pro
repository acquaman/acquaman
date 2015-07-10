# #####################################################################
# QMake project file for acquaman, including test suites			January 2010. mark.boots@usask.ca
# #####################################################################

TEMPLATE = subdirs
SUBDIRS += \
	Initialize.pro \
	pluginProjects/FileLoaders/AMFileLoaderPlugins.pro \
	# pluginProjects/AnalysisBlocks/AMAnalysisBlockPlugins.pro \
	EnsureHeaderNotice.pro \
	AMCrashReporter.pro \
	CLSNetworkDirectorySynchronizer.pro \
	BioXASMainAcquaman.pro \
	BioXASSideAcquaman.pro \
	BioXASImagingAcquaman.pro \
	BioXASSideTableCoordinator.pro \
	BioXASToolSuite.pro \
	MidIRBPM.pro \
	REIXSTest.pro \
	REIXSAcquaman.pro \
	IDEASAcquaman.pro \
	SGMAcquaman.pro \
	SGMAddOnsCoordinator.pro \
	SGMAmptekCoordinator.pro \
	SGMLookupTableCoordinator.pro \
	SGMSSAAcquaman.pro \
	SXRMBAcquaman.pro \
	SXRMBAddOnsCoordinator.pro \
	VESPERSAcquaman.pro \
	VESPERSBendingMirrors.pro \
	VESPERSDatabaseDuplicateEntryPatch.pro \
	VESPERSAddOnsCoordinator.pro \
	BareBonesAcquaman.pro \
	acquamanTest.pro \
	buildTests/AMBuildTest.pro \
	XRDAnalysis.pro \
	StripToolProject.pro \
	StripTool2.pro \
	CLSPGTDwellTimeCoordinator.pro \
	CLSSR570Coordinator.pro \
	Timesheet.pro

