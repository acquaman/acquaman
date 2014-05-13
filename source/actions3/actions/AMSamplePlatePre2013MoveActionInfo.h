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


#ifndef AMSAMPLEPLATEPRE2013MOVEACTIONINFO_H
#define AMSAMPLEPLATEPRE2013MOVEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfoList.h"

#define AMSAMPLEPLATEPRE2013MOVEACTIONINFO_INVALID_SAMPLEPLATE_ID 327001
#define AMSAMPLEPLATEPRE2013MOVEACTIONINFO_INVALID_SAMPLEPOSITION_INDEX 327002
#define AMSAMPLEPLATEPRE2013MOVEACTIONINFO_INVALID_SAMPLEPOSITION_ID 327003

class AMSamplePlatePre2013MoveActionInfo : public AMActionInfo3
{
Q_OBJECT
Q_PROPERTY(int samplePlateId READ samplePlateId WRITE setSamplePlateId)
Q_PROPERTY(int samplePositionIndex READ samplePositionIndex WRITE setSamplePositionIndex)

public:
	/// Constructor. You should always specify the paramaters, but we provide the default for database reload
	Q_INVOKABLE AMSamplePlatePre2013MoveActionInfo(int samplePlateId = -1, int samplePositionIndex = -1, QObject *parent = 0);

	virtual ~AMSamplePlatePre2013MoveActionInfo();

	/// Copy constructor
	AMSamplePlatePre2013MoveActionInfo(const AMSamplePlatePre2013MoveActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3* createCopy() const;

	/// This describes the type of the action
	virtual QString typeDescription() const;

	/// Returns the database id of the samplePlate we're working with
	int samplePlateId() const;

	/// Returns the database id of the samplePosition on the plate we're interested in
	int samplePositionIndex() const;

	/// Returns the actual control info list (position we want to move to) ... loads this from the database if necessary
	const AMControlInfoList* samplePosition();

	/// Returns the name of the sample plate
	QString samplePlateName() const;

	/// Returns the name of the sample position
	QString samplePositionName() const;

public slots:
	/// Changes the plate we're intested in by choosing a different database id
	void setSamplePlateId(int samplePlateId);

	/// Changes the positino we're interested in by choosing a different database id
	void setSamplePositionIndex(int samplePositionIndex);

protected:
	/// A short helper function to update the action's description
	void updateDescriptionText();

protected:
	/// Holds the samplePlate database id
	int samplePlateId_;
	/// Holds the samplePosition index in the samplePlate
	int samplePositionIndex_;

	/// Holds a copy of the control info list (position we want to move to)
	AMControlInfoList *samplePosition_;
};

#endif // AMSAMPLEPLATEPRE2013MOVEACTIONINFO_H
