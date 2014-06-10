#include "AM3DScanConfiguration.h"

 AM3DScanConfiguration::~AM3DScanConfiguration(){}
AM3DScanConfiguration::AM3DScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
	setUserScanName("");
	xRange_ = AMRange();
	yRange_ = AMRange();
	zRange_ = AMRange();
	steps_ = QList<double>() << 0.0 << 0.0 << 0.0;
	time_ = 0.0;
	xPriority_ = 0;
	yPriority_ = 1;
	zPriority_ = 2;
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
	if (xRange_.minimum() == xRange_.maximum())
		return false;

	// If the sign of the step size is different from the difference between start and end points then the scan cannot make it from the start to the end.
	else if ((xRange_.maximum() - xRange_.minimum()) > 0 && steps_.at(0) < 0)
		return false;

	else if ((xRange_.maximum() - xRange_.minimum()) < 0 && steps_.at(0) > 0)
		return false;

	return true;
}

bool AM3DScanConfiguration::validYAxis() const
{
	// If there is no finite difference between the start and end points.
	if (yRange_.minimum() == yRange_.maximum())
		return false;

	// If the sign of the step size is different from the difference between start and end points then the scan cannot make it from the start to the end.
	else if ((yRange_.maximum() - yRange_.minimum()) > 0 && steps_.at(1) < 0)
		return false;

	else if ((yRange_.maximum() - yRange_.minimum()) < 0 && steps_.at(1) > 0)
		return false;

	return true;
}

bool AM3DScanConfiguration::validZAxis() const
{
	// If there is no finite difference between the start and end points.
	if (zRange_.minimum() == zRange_.maximum())
		return false;

	// If the sign of the step size is different from the difference between start and end points then the scan cannot make it from the start to the end.
	else if ((zRange_.maximum() - zRange_.minimum()) > 0 && steps_.at(2) < 0)
		return false;

	else if ((zRange_.maximum() - zRange_.minimum()) < 0 && steps_.at(2) > 0)
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
	xRange_.setMinimum(start);
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
	xRange_.setMaximum(end);
	emit xEndChanged(end);
	setModified(true);
}

void AM3DScanConfiguration::setYStart(double start)
{
	yRange_.setMinimum(start);
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
	yRange_.setMaximum(end);
	emit yEndChanged(end);
	setModified(true);
}

void AM3DScanConfiguration::setZStart(double start)
{
	zRange_.setMinimum(start);
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
	zRange_.setMaximum(end);
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
	if (priority >= 0 && priority <= 2){

		xPriority_ = priority;
		emit axisConfigurationChanged();
		setModified(true);
	}
}

void AM3DScanConfiguration::setYPriority(int priority)
{
	if (priority >= 0 && priority <= 2){

		yPriority_ = priority;
		emit axisConfigurationChanged();
		setModified(true);
	}
}

void AM3DScanConfiguration::setZPriority(int priority)
{
	if (priority >= 0 && priority <= 2){

		zPriority_ = priority;
		emit axisConfigurationChanged();
		setModified(true);
	}
}

void AM3DScanConfiguration::setXRange(AMRange x)
{
	setXStart(x.minimum());
	setXEnd(x.maximum());
}

void AM3DScanConfiguration::setXRange(double start, double end)
{
	setXStart(start);
	setXEnd(end);
}

void AM3DScanConfiguration::setYRange(AMRange y)
{
	setYStart(y.minimum());
	setYEnd(y.maximum());
}

void AM3DScanConfiguration::setYRange(double start, double end)
{
	setYStart(start);
	setYEnd(end);
}

void AM3DScanConfiguration::setZRange(AMRange z)
{
	setZStart(z.minimum());
	setZEnd(z.maximum());
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
