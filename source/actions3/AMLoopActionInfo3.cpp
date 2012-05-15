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


#include "AMLoopActionInfo3.h"

AMLoopActionInfo3::AMLoopActionInfo3(int iterations, const QString &shortDescription, const QString &longDescription, const QString &iconFileName, QObject *parent)
	: AMListActionInfo3(shortDescription, longDescription, iconFileName, parent)
{
	setShortDescription(QString("Looping %1 times").arg(iterations));
	setLongDescription(QString("This action repeats all of its sub-actions in order, %1 times").arg(iterations));
	setIconFileName(":/32x32/media-playlist-repeat.png");
	loopCount_ = iterations;
}

void AMLoopActionInfo3::setLoopCount(int loopCount)
{
	if(loopCount == loopCount_)
		return;

	loopCount_ = loopCount;
	setShortDescription(QString("Looping %1 times").arg(loopCount));
	setLongDescription(QString("This action repeats all of its sub-actions in order, %1 times").arg(loopCount));

	setModified(true);
	emit loopCountChanged(loopCount_);
}
