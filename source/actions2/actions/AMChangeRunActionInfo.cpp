#include "AMChangeRunActionInfo.h"

#include "dataman/AMRun.h"
#include "util/AMDateTimeUtils.h"

AMChangeRunActionInfo::AMChangeRunActionInfo(QObject *parent) :
	AMActionInfo("Change Run", "Change Run", QString(), parent)
{
	runId_ = -2;
	newRunName_ = "New Run";
	setRunId(-1);
}

void AMChangeRunActionInfo::setRunId(int runId)
{
	if(runId == runId_)
		return;

	runId_ = runId;

	if(runId_ < 1) {
		QString description = QString("Creating a new run called: %1").arg(newRunName_);
		setShortDescription(description);
		setLongDescription(description);
	}
	else {
		AMRun r(runId_, AMDatabase::database("user"));
		QString description = QString("Changing the current run to: %1 (%2).").arg(r.name(),AMDateTimeUtils::prettyDate(r.dateTime()));
		setShortDescription(description);
		setLongDescription(description);
	}
}

void AMChangeRunActionInfo::setNewRunName(const QString &newRunName)
{
	if(newRunName_ == newRunName)
		return;

	newRunName_ = newRunName;
	if(runId_ < 1) {
		QString description = QString("Creating a new run called: %1").arg(newRunName_);
		setShortDescription(description);
		setLongDescription(description);
	}
}
