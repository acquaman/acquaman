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


#include "AMTimedWaitAction3.h"

 AMTimedWaitAction3::~AMTimedWaitAction3(){}
AMTimedWaitAction3::AMTimedWaitAction3(AMTimedWaitActionInfo3 *info, QObject *parent) :
	AMAction3(info, parent)
{
	waitTimer_ = 0; //NULL
}

AMTimedWaitAction3::AMTimedWaitAction3(const AMTimedWaitAction3 &other) :
	AMAction3(other)
{
	waitTimer_ = 0; //NULL
}

void AMTimedWaitAction3::startImplementation(){
	waitTimer_ = new QTimer();
	waitTimer_->setSingleShot(true);
	waitTimer_->setInterval(timedWaitInfo()->waitTime()*1000);
	connect(waitTimer_, SIGNAL(timeout()), this, SLOT(onWaitTimerTimeout()));

	setStarted();
	waitTimer_->start();
}

void AMTimedWaitAction3::cancelImplementation(){
	if(waitTimer_)
		waitTimer_->stop();

	setCancelled();
}

void AMTimedWaitAction3::onWaitTimerTimeout(){
	setProgress(100, 100);
	setSucceeded();
}
