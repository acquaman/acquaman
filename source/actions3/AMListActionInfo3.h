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


#ifndef AMLISTACTIONINFO3_H
#define AMLISTACTIONINFO3_H

#include "actions3/AMActionInfo3.h"

class AMListActionInfo3 : public AMActionInfo3
{
Q_OBJECT

public:
	/// Constructor.
 	virtual ~AMListActionInfo3();
	Q_INVOKABLE AMListActionInfo3(const QString& shortDescription = "List", const QString& longDescription = "List of Actions to Run", const QString& iconFileName = ":/32x32/format-list-ordered.png", QObject *parent = 0);
	/// Copy constructor.
	AMListActionInfo3(const AMListActionInfo3 &other);
	/// Method that creates a copy of the current action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns a description for this action.
	virtual QString typeDescription() const;
};

class AMSequentialListActionInfo3 : public AMListActionInfo3
{
Q_OBJECT

public:
	/// Constructor.
 	virtual ~AMSequentialListActionInfo3();
	Q_INVOKABLE AMSequentialListActionInfo3(const QString& shortDescription = "Sequential List", const QString& longDescription = "Sequential List of Actions to Run", const QString& iconFileName = ":/32x32/format-list-ordered.png", QObject *parent = 0);
	/// Copy constructor.
	AMSequentialListActionInfo3(const AMSequentialListActionInfo3 &other);
	/// Method that creates a copy of the current action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns a description for this action.
	virtual QString typeDescription() const;
};

class AMParallelListActionInfo3 : public AMListActionInfo3
{
Q_OBJECT

public:
	/// Constructor.
 	virtual ~AMParallelListActionInfo3();
	Q_INVOKABLE AMParallelListActionInfo3(const QString& shortDescription = "Parallel List", const QString& longDescription = "Parallel List of Actions to Run", const QString& iconFileName = ":/32x32/format-line-spacing-triple.png", QObject *parent = 0);
	/// Copy constructor.
	AMParallelListActionInfo3(const AMParallelListActionInfo3 &other);
	/// Method that creates a copy of the current action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns a description for this action.
	virtual QString typeDescription() const;
};

#endif // AMLISTACTIONINFO3_H
