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


#include "AMLoopActionInfo3.h"

AMLoopActionInfo3::~AMLoopActionInfo3(){}

AMLoopActionInfo3::AMLoopActionInfo3(int iterations, const QString &shortDescription, const QString &longDescription, const QString &iconFileName, QObject *parent)
	: AMSequentialListActionInfo3(QString("%1 (repeat %2 time%3)").arg(shortDescription).arg(iterations).arg(iterations > 1 ? "s" : ""), longDescription, iconFileName, parent)
{
	loopCount_ = iterations;
	connect(this, SIGNAL(loopCountChanged(int)), this, SLOT(onLoopCountChanged(int)));
}

AMLoopActionInfo3::AMLoopActionInfo3(const AMLoopActionInfo3 &other)
	: AMSequentialListActionInfo3(other)
{
	loopCount_ = other.loopCount_;
	connect(this, SIGNAL(loopCountChanged(int)), this, SLOT(onLoopCountChanged(int)));
}

void AMLoopActionInfo3::setLoopCount(int loopCount)
{
	if(loopCount == loopCount_)
		return;

	loopCount_ = loopCount;
	setModified(true);
	emit loopCountChanged(loopCount_);
}

AMActionInfo3 *AMLoopActionInfo3::createCopy() const
{
	AMActionInfo3 *info = new AMLoopActionInfo3(*this);
	info->dissociateFromDb(true);
	return info;
}

void AMLoopActionInfo3::onLoopCountChanged(int newLoopCount)
{
	QString newDescription = shortDescription();
	newDescription = newDescription.replace(QRegExp("repeat [0-9]{1,} time(s|)"), QString("repeat %1 time%2").arg(newLoopCount).arg(newLoopCount > 1 ? "s" : ""));
	setShortDescription(newDescription);
}
