/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
