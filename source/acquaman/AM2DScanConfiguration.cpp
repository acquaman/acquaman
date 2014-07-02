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


#include "AM2DScanConfiguration.h"

 AM2DScanConfiguration::~AM2DScanConfiguration(){}
AM2DScanConfiguration::AM2DScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
	setUserScanName("");
	xRange_ = AMRange();
	yRange_ = AMRange();
	steps_ = qMakePair(0.0, 0.0);
	time_ = 0;
	fast_ = X;
	slow_ = Y;
}

AM2DScanConfiguration::AM2DScanConfiguration(const AM2DScanConfiguration &original)
	: AMScanConfiguration()
{
	setUserScanName(original.userScanName());
	xRange_ = original.xRange();
	yRange_ = original.yRange();
	steps_ = original.steps();
	time_ = original.timeStep();
	fast_ = original.fastAxis();
	slow_ = original.slowAxis();
}

bool AM2DScanConfiguration::validXAxis() const
{
	// If there is no finite difference between the start and end points.
	if (xRange_.minimum() == xRange_.maximum())
		return false;

	// If the sign of the step size is different from the difference between start and end points then the scan cannot make it from the start to the end.
	else if ((xRange_.maximum() - xRange_.minimum()) > 0 && steps_.first < 0)
		return false;

	else if ((xRange_.maximum() - xRange_.minimum()) < 0 && steps_.first > 0)
		return false;

	return true;
}

bool AM2DScanConfiguration::validYAxis() const
{
	// If there is no finite difference between the start and end points.
	if (yRange_.minimum() == yRange_.maximum())
		return false;

	// If the sign of the step size is different from the difference between start and end points then the scan cannot make it from the start to the end.
	else if ((yRange_.maximum() - yRange_.minimum()) > 0 && steps_.second < 0)
		return false;

	else if ((yRange_.maximum() - yRange_.minimum()) < 0 && steps_.second > 0)
		return false;

	return true;
}

bool AM2DScanConfiguration::validTimeStep() const
{
	return time_ > 0;
}
