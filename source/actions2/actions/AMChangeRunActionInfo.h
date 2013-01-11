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


#ifndef AMCHANGERUNACTIONINFO_H
#define AMCHANGERUNACTIONINFO_H

#include "actions2/AMActionInfo.h"

/// Specifies the run to switch to, or the name of the new run, for AMChangeRunAction.  If a new run is created, it will be made at the same facility as the current run.
class AMChangeRunActionInfo : public AMActionInfo
{
    Q_OBJECT
	Q_PROPERTY(QString newRunName READ newRunName WRITE setNewRunName)
	Q_PROPERTY(int runIdOrNewRun READ runId WRITE setRunId)

public:
	/// Default constructor: Changes to a new run called "New Run".
	Q_INVOKABLE AMChangeRunActionInfo(QObject *parent = 0);
	/// Copy constructor
	AMChangeRunActionInfo(const AMChangeRunActionInfo& other) : AMActionInfo(other), runId_(other.runId_), newRunName_(other.newRunName_) {}
	/// Virtual copy constructor
	virtual AMActionInfo* createCopy() const { return new AMChangeRunActionInfo(*this); }


	// Re-implemented public functions
	///////////////////////////////////

	/// This should describe the type of the action:
	virtual QString typeDescription() const { return "Change Run"; }

	// New public functions
	////////////////////////////

	int runId() const { return runId_; }
	QString newRunName() const { return newRunName_; }


signals:

public slots:

	/// Set the run ID to switch to, or -1 to create a new run.
	void setRunId(int runId);
	/// Set the name of the new run (if creating a new run)
	void setNewRunName(const QString& newRunName);


protected:
	/// If valid (>0), the existing run to switch to. Otherwise, a new run will be created.
	int runId_;
	/// Name for the new run, if runId_ < 1.
	QString newRunName_;


};

#endif // AMCHANGERUNACTIONINFO_H
