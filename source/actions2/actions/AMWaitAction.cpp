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


#include "AMWaitAction.h"

AMWaitAction::AMWaitAction(AMWaitActionInfo* info, QObject *parent) :
	AMAction(info, parent)
{
}

AMWaitAction::AMWaitAction(double seconds, QObject* parent) :
	AMAction(new AMWaitActionInfo(seconds), parent)
{
}

const AMWaitActionInfo * AMWaitAction::waitInfo() const
{
	return qobject_cast<const AMWaitActionInfo*>(info());
}

void AMWaitAction::startImplementation()
{
	connect(&timer_, SIGNAL(timeout()), this, SLOT(onFinished()));
	timer_.start(int(waitInfo()->secondsToWait()*1000));

	connect(&progressTick_, SIGNAL(timeout()), this, SLOT(onProgressTick()));
	progressTick_.start(200);

	notifyStarted();
}

void AMWaitAction::pauseImplementation()
{
	timer_.stop();
	progressTick_.stop();

	notifyPaused();
}

void AMWaitAction::resumeImplementation()
{
	double secondsCompleted = runningTime();
	timer_.start(int((waitInfo()->secondsToWait() - secondsCompleted)*1000));
	progressTick_.start();

	notifyResumed();
}

void AMWaitAction::cancelImplementation()
{
	timer_.stop();
	progressTick_.stop();
	notifyCancelled();
}

void AMWaitAction::onProgressTick()
{
	double secondsCompleted = runningTime();
	setProgress(secondsCompleted, waitInfo()->secondsToWait());
}

void AMWaitAction::onFinished()
{
	timer_.stop();
	notifySucceeded();
}
