#ifndef AMBUILDREPORTER_H
#define AMBUILDREPORTER_H

#include "util/AMBuildInfo.h"

class AMBuildReporter
{
public:
	static AMBuildReporter* buildReporter();

	const AMBuildInfo* buildInfo() const;

protected:
	AMBuildReporter();

private:
	static AMBuildReporter *buildReporterInstance_;
	AMBuildInfo *buildInfo_;
};

#endif // AMBUILDREPORTER_H
