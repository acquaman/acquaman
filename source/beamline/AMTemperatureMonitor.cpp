#include "AMTemperatureMonitor.h"

AMTemperatureMonitor::AMTemperatureMonitor(const QString &name, const QString &units, QObject *parent) :
	AMConnectedControl(name, units, parent)
{
	goodValue_ = -1;
	badValue_ = -1;

	status_ = 0;
	value_ = 0;
}

AMTemperatureMonitor::~AMTemperatureMonitor()
{

}

bool AMTemperatureMonitor::canMeasure() const
{
	return (value_ && value_->canMeasure());
}

bool AMTemperatureMonitor::isGood() const
{
	bool result = false;

	if (status_ && status_->canMeasure())
		result = (status_->value() == goodValue_);

	return result;
}

bool AMTemperatureMonitor::isBad() const
{
	bool result = false;

	if (status_ && status_->canMeasure())
		result = (status_->value() == badValue_);

	return result;
}

double AMTemperatureMonitor::status() const
{
	double result = -1;

	if (status_ && status_->canMeasure())
		result = status_->value();

	return result;
}

double AMTemperatureMonitor::value() const
{
	double result = -1;

	if (value_ && value_->canMeasure())
		result = value_->value();

	return result;
}

void AMTemperatureMonitor::setStatusControl(AMControl *control, double goodValue, double badValue)
{
	if (status_ != control) {

		if (status_)
			removeChildControl(status_);

		status_ = control;

		if (status_) {
			goodValue_ = goodValue;
			badValue_ = badValue;

			addChildControl(status_);

			connect( status_, SIGNAL(valueChanged(double)), this, SIGNAL(statusChanged(double)) );
		}

		emit statusControlChanged(status_);
	}
}

void AMTemperatureMonitor::setValueControl(AMControl *control)
{
	if (value_ != control) {

		if (value_)
			removeChildControl(value_);

		value_ = control;

		if (value_) {
			addChildControl(value_);

			connect( value_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)) );
		}

		emit valueControlChanged(value_);
	}
}
