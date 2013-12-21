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


#ifndef AMNUMBERCHANGEACTIONINFO_H
#define AMNUMBERCHANGEACTIONINFO_H

#include "actions3/AMActionInfo3.h"

class AMNumberChangeActionInfo : public AMActionInfo3
{
Q_OBJECT
Q_PROPERTY(int indexToChange READ indexToChange WRITE setIndexToChange)
Q_PROPERTY(int newNumber READ newNumber WRITE setNewNumber)

public:
 	virtual ~AMNumberChangeActionInfo();
	Q_INVOKABLE AMNumberChangeActionInfo(int newNumber = 0, int indexToChange = -1, QObject *parent = 0);

	AMNumberChangeActionInfo(const AMNumberChangeActionInfo &other);

	virtual AMActionInfo3* createCopy() const;

	virtual QString typeDescription() const;

	int newNumber() const;
	int indexToChange() const;

public slots:
	void setNewNumber(int newNumber);
	void setIndexToChange(int indexToChange);

protected:
	void updateDescriptionText();

protected:
	int newNumber_;
	int indexToChange_;
};

#endif // AMNUMBERCHANGEACTIONINFO_H
