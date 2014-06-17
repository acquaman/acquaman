#include "AMBuildReporter.h"

#ifdef AM_BUILD_REPORTER_ENABLED
#include "util/AMRunTimeBuildInfo.h"
#endif

AMBuildReporter* AMBuildReporter::buildReporterInstance_ = 0;

AMBuildReporter* AMBuildReporter::buildReporter(){
	if(!buildReporterInstance_)
		buildReporterInstance_ = new AMBuildReporter();
	return buildReporterInstance_;
}

AMBuildReporter::AMBuildReporter()
{
	#ifdef AM_BUILD_REPORTER_ENABLED
	buildInfo_ = new AMRunTimeBuildInfo();
	#else
	buildInfo_ = new AMBuildInfo();
	#endif
}

const AMBuildInfo* AMBuildReporter::buildInfo() const{
	return buildInfo_;
}
