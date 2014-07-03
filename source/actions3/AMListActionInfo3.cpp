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


#include "AMListActionInfo3.h"

// AMListActionInfo
////////////////////////////////

 AMListActionInfo3::~AMListActionInfo3(){}
AMListActionInfo3::AMListActionInfo3(const QString &shortDescription, const QString &longDescription, const QString &iconFileName, QObject *parent)
	: AMActionInfo3(shortDescription, longDescription, iconFileName, parent)
{
}

AMListActionInfo3::AMListActionInfo3(const AMListActionInfo3 &other) :
	AMActionInfo3(other)
{
}

AMActionInfo3* AMListActionInfo3::createCopy() const{
	return new AMListActionInfo3(*this);
}

QString AMListActionInfo3::typeDescription() const{
	return "List";
}

// AMSequentialListActionInfo
////////////////////////////////

 AMSequentialListActionInfo3::~AMSequentialListActionInfo3(){}
AMSequentialListActionInfo3::AMSequentialListActionInfo3(const QString &shortDescription, const QString &longDescription, const QString &iconFileName, QObject *parent)
	: AMListActionInfo3(shortDescription, longDescription, iconFileName, parent)
{
}

AMSequentialListActionInfo3::AMSequentialListActionInfo3(const AMSequentialListActionInfo3 &other) :
	AMListActionInfo3(other)
{
}

AMActionInfo3* AMSequentialListActionInfo3::createCopy() const{
	return new AMSequentialListActionInfo3(*this);
}

QString AMSequentialListActionInfo3::typeDescription() const{
	return "Sequential List";
}

// AMParallelListActionInfo
////////////////////////////////

 AMParallelListActionInfo3::~AMParallelListActionInfo3(){}
AMParallelListActionInfo3::AMParallelListActionInfo3(const QString &shortDescription, const QString &longDescription, const QString &iconFileName, QObject *parent)
	: AMListActionInfo3(shortDescription, longDescription, iconFileName, parent)
{
}

AMParallelListActionInfo3::AMParallelListActionInfo3(const AMParallelListActionInfo3 &other) :
	AMListActionInfo3(other)
{
}

AMActionInfo3* AMParallelListActionInfo3::createCopy() const{
	return new AMParallelListActionInfo3(*this);
}

QString AMParallelListActionInfo3::typeDescription() const{
	return "Parallel List";
}
