#include "AMTemperatureMonitor.h"

AMTemperatureMonitor::AMTemperatureMonitor(const QString &name, const QString &units, QObject *parent) :
	AMConnectedControl(name, units, parent)
{
	goodStatusValue_ = -1;
	badStatusValue_ = -1;

	statusControl_ = 0;
	valueControl_ = 0;
}

AMTemperatureMonitor::~AMTemperatureMonitor()
{

}

bool AMTemperatureMonitor::canMeasure() const
{
	return (valueControl_ && valueControl_->canMeasure());
}

bool AMTemperatureMonitor::isGood() const
{
	bool result = false;

	if (statusControl_ && statusControl_->canMeasure())
		result = (statusControl_->value() == goodStatusValue_);

	return result;
}

bool AMTemperatureMonitor::isBad() const
{
	bool result = false;

	if (statusControl_ && statusControl_->canMeasure())
		result = (statusControl_->value() == badStatusValue_);

	return result;
}

double AMTemperatureMonitor::status() const
{
	double result = -1;

	if (statusControl_ && statusControl_->canMeasure())
		result = statusControl_->value();

	return result;
}

double AMTemperatureMonitor::value() const
{
	double result = -1;

	if (valueControl_ && valueControl_->canMeasure())
		result = valueControl_->value();

	return result;
}

void AMTemperatureMonitor::setStatusControl(AMControl *control, double goodValue, double badValue)
{
	if (statusControl_ != control) {

		if (statusControl_)
			removeChildControl(statusControl_);

		statusControl_ = control;

		if (statusControl_) {
			goodStatusValue_ = goodValue;
			badStatusValue_ = badValue;

			addChildControl(statusControl_);

			connect( statusControl_, SIGNAL(valueChanged(double)), this, SIGNAL(statusChanged(double)) );
		}

		emit statusControlChanged(statusControl_);
	}
}

void AMTemperatureMonitor::setValueControl(AMControl *control)
{
	if (valueControl_ != control) {

		if (valueControl_)
			removeChildControl(valueControl_);

		valueControl_ = control;

		if (valueControl_) {
			addChildControl(valueControl_);

			connect( valueControl_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)) );
		}

		emit valueControlChanged(valueControl_);
	}
}
