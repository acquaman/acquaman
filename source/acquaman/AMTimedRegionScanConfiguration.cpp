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


#include "AMTimedRegionScanConfiguration.h"

#include "util/AMErrorMonitor.h"

AMTimedRegionScanConfiguration::AMTimedRegionScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
	userScanName_ = "$sample - $technique";
	time_ = 0.0;
	timePerAcquisition_ = 0.0;
	iterations_ = 0;
}

AMTimedRegionScanConfiguration::AMTimedRegionScanConfiguration(const AMTimedRegionScanConfiguration &original)
	: AMScanConfiguration()
{
	userScanName_ = original.name();
	time_ = original.time();
	timePerAcquisition_ = original.timePerAcquisition();
	iterations_ = original.iterations();
}

const QMetaObject *AMTimedRegionScanConfiguration::getMetaObject()
{
	return metaObject();
}

double AMTimedRegionScanConfiguration::time() const
{
	return time_;
}

double AMTimedRegionScanConfiguration::timePerAcquisition() const
{
	return timePerAcquisition_;
}

void AMTimedRegionScanConfiguration::setTime(double newTime)
{
	if (time_ != newTime){

		time_ = newTime;
		emit timeChanged(time_);
		setModified(true);
	}
}

void AMTimedRegionScanConfiguration::setTimePerAcquisition(double newTotalTime)
{
	if (timePerAcquisition_ != newTotalTime){

		timePerAcquisition_ = newTotalTime;
		emit timePerAcquisitionChanged(timePerAcquisition_);
		setModified(true);
	}
}

void AMTimedRegionScanConfiguration::setIterations(int newIteration)
{
	if (iterations_ != newIteration){

		iterations_ = newIteration;
		emit iterationsChanged(iterations_);
		setModified(true);
	}
}

int AMTimedRegionScanConfiguration::iterations() const
{
	return iterations_;
}
