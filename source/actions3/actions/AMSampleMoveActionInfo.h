/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMSAMPLEMOVEACTIONINFO_H
#define AMSAMPLEMOVEACTIONINFO_H

#include "actions3/AMActionInfo3.h"

class AMSample;

class AMSampleMoveActionInfo : public AMActionInfo3
{
Q_OBJECT
Q_PROPERTY(AMConstDbObject* sample READ dbReadSample WRITE dbLoadSample)

public:
	/// Constructor takes a const pointer to an AMSample
	Q_INVOKABLE AMSampleMoveActionInfo(const AMSample *sample = 0, QObject *parent = 0);

	/// Copy constructor
	AMSampleMoveActionInfo(const AMSampleMoveActionInfo &other);

	/// Destructor
	virtual ~AMSampleMoveActionInfo();

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3* createCopy() const;

	/// This describes the type of the action
	virtual QString typeDescription() const;

	/// Returns a const pointer to the sample
	const AMSample* sample() const;

public slots:
	/// Sets the sample from a const pointer
	void setSample(const AMSample *sample);

protected:
	/// A short helper function to update the action's description
	void updateDescriptionText();

	/// Database reading so we can reference this sample in the database
	AMConstDbObject* dbReadSample() const;
	/// Database loading so we can pull this sample out of the database
	void dbLoadSample(AMConstDbObject *newSample);

protected:
	/// Our sample as an AMConstDbObject pointer
	AMConstDbObject *sample_;
};

#endif // AMSAMPLEMOVEACTIONINFO_H
