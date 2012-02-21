#include "AM2DScanConfiguration.h"

AM2DScanConfiguration::AM2DScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
	setUserScanName("");
	xRange_ = qMakePair(0.0, 0.0);
	yRange_ = qMakePair(0.0, 0.0);
	steps_ = qMakePair(0.0, 0.0);
	time_ = 0;
	fast_ = X;
	slow_ = Y;
}

AM2DScanConfiguration::AM2DScanConfiguration(const AM2DScanConfiguration &original)
	: AMScanConfiguration(original.parent())
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
	if (xRange_.first == xRange_.second)
		return false;

	// If the sign of the step size is different from the difference between start and end points then the scan cannot make it from the start to the end.
	else if ((xRange_.second - xRange_.first) > 0 && steps_.first < 0)
		return false;

	else if ((xRange_.second - xRange_.first) < 0 && steps_.first > 0)
		return false;

	return true;
}

bool AM2DScanConfiguration::validYAxis() const
{
	// If there is no finite difference between the start and end points.
	if (yRange_.first == yRange_.second)
		return false;

	// If the sign of the step size is different from the difference between start and end points then the scan cannot make it from the start to the end.
	else if ((yRange_.second - yRange_.first) > 0 && steps_.second < 0)
		return false;

	else if ((yRange_.second - yRange_.first) < 0 && steps_.second > 0)
		return false;

	return true;
}

bool AM2DScanConfiguration::validTimeStep() const
{
	if (time_ > 0)
		return true;

	return false;
}
