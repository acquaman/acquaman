#include "AM3DScanConfiguration.h"

AM3DScanConfiguration::AM3DScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
	setUserScanName("");
	xRange_ = qMakePair(0.0, 0.0);
	yRange_ = qMakePair(0.0, 0.0);
	zRange_ = qMakePair(0.0, 0.0);
	steps_ = QList<double>() << 0.0 << 0.0 << 0.0;
	time_ = 0.0;
	xPriority_ = 1;
	yPriority_ = 2;
	zPriority_ = 3;
}

AM3DScanConfiguration::AM3DScanConfiguration(const AM3DScanConfiguration &original)
	: AMScanConfiguration(original)
{
	setUserScanName(original.userScanName());
	xRange_ = original.xRange();
	yRange_ = original.yRange();
	zRange_ = original.zRange();
	steps_ = original.steps();
	time_ = original.timeStep();
	xPriority_ = original.xPriority();
	yPriority_ = original.yPriority();
	zPriority_ = original.zPriority();
}

bool AM3DScanConfiguration::validXAxis() const
{
	// If there is no finite difference between the start and end points.
	if (xRange_.first == xRange_.second)
		return false;

	// If the sign of the step size is different from the difference between start and end points then the scan cannot make it from the start to the end.
	else if ((xRange_.second - xRange_.first) > 0 && steps_.at(0) < 0)
		return false;

	else if ((xRange_.second - xRange_.first) < 0 && steps_.at(0) > 0)
		return false;

	return true;
}

bool AM3DScanConfiguration::validYAxis() const
{
	// If there is no finite difference between the start and end points.
	if (yRange_.first == yRange_.second)
		return false;

	// If the sign of the step size is different from the difference between start and end points then the scan cannot make it from the start to the end.
	else if ((yRange_.second - yRange_.first) > 0 && steps_.at(1) < 0)
		return false;

	else if ((yRange_.second - yRange_.first) < 0 && steps_.at(1) > 0)
		return false;

	return true;
}

bool AM3DScanConfiguration::validZAxis() const
{
	// If there is no finite difference between the start and end points.
	if (zRange_.first == zRange_.second)
		return false;

	// If the sign of the step size is different from the difference between start and end points then the scan cannot make it from the start to the end.
	else if ((zRange_.second - zRange_.first) > 0 && steps_.at(2) < 0)
		return false;

	else if ((zRange_.second - zRange_.first) < 0 && steps_.at(2) > 0)
		return false;

	return true;
}

bool AM3DScanConfiguration::validTimeStep() const
{
	if (time_ > 0)
		return true;

	return false;
}

void AM3DScanConfiguration::setXStart(double start)
{
	xRange_.first = start;
	emit xStartChanged(start);
	setModified(true);
}

void AM3DScanConfiguration::setXStep(double stepSize)
{
	steps_[0] = stepSize;
	emit xStepChanged(stepSize);
	setModified(true);
}

void AM3DScanConfiguration::setXEnd(double end)
{
	xRange_.second = end;
	emit xEndChanged(end);
	setModified(true);
}

void AM3DScanConfiguration::setYStart(double start)
{
	yRange_.first = start;
	emit yStartChanged(start);
	setModified(true);
}

void AM3DScanConfiguration::setYStep(double stepSize)
{
	steps_[1] = stepSize;
	emit yStepChanged(stepSize);
	setModified(true);
}

void AM3DScanConfiguration::setYEnd(double end)
{
	yRange_.second = end;
	emit yEndChanged(end);
	setModified(true);
}

void AM3DScanConfiguration::setZStart(double start)
{
	zRange_.first = start;
	emit zStartChanged(start);
	setModified(true);
}

void AM3DScanConfiguration::setZStep(double stepSize)
{
	steps_[2] = stepSize;
	emit zStepChanged(stepSize);
	setModified(true);
}

void AM3DScanConfiguration::setZEnd(double end)
{
	zRange_.second = end;
	emit zEndChanged(end);
	setModified(true);
}

void AM3DScanConfiguration::setTimeStep(double time)
{
	time_ = time;
	emit timeStepChanged(time);
	setModified(true);
}

void AM3DScanConfiguration::setXPriority(int priority)
{
	if (priority >= 1 && priority <= 3){

		xPriority_ = priority;
		emit axisConfigurationChanged();
		setModified(true);
	}
}

void AM3DScanConfiguration::setYPriority(int priority)
{
	if (priority >= 1 && priority <= 3){

		yPriority_ = priority;
		emit axisConfigurationChanged();
		setModified(true);
	}
}

void AM3DScanConfiguration::setZPriority(int priority)
{
	if (priority >= 1 && priority <= 3){

		zPriority_ = priority;
		emit axisConfigurationChanged();
		setModified(true);
	}
}

void AM3DScanConfiguration::setXRange(QPair<double, double> x)
{
	setXStart(x.first);
	setXEnd(x.second);
}

void AM3DScanConfiguration::setXRange(double start, double end)
{
	setXStart(start);
	setXEnd(end);
}

void AM3DScanConfiguration::setYRange(QPair<double, double> y)
{
	setYStart(y.first);
	setYEnd(y.second);
}

void AM3DScanConfiguration::setYRange(double start, double end)
{
	setYStart(start);
	setYEnd(end);
}

void AM3DScanConfiguration::setZRange(QPair<double, double> z)
{
	setZStart(z.first);
	setZEnd(z.second);
}

void AM3DScanConfiguration::setZRange(double start, double end)
{
	setZStart(start);
	setZEnd(end);
}

void AM3DScanConfiguration::setStepSize(QList<double> steps)
{
	setXStep(steps.at(0));
	setYStep(steps.at(1));
	setZStep(steps.at(2));
}

void AM3DScanConfiguration::setStepSize(double x, double y, double z)
{
	setXStep(x);
	setYStep(y);
	setZStep(z);
}
