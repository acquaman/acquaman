#include "AMBeamlineControl.h"

AMBeamlineControl::AMBeamlineControl(const QString &name, const QString &units, QObject *parent) :
	AMConnectedControl(name, units, parent)
{
	goodStatusValue_ = -1;
	badStatusValue_ = -1;

	statusControl_ = 0;
	valueControl_ = 0;
}

AMBeamlineControl::~AMBeamlineControl()
{

}

bool AMBeamlineControl::canMeasure() const
{
	return (valueControl_ && valueControl_->canMeasure());
}

bool AMBeamlineControl::isGood() const
{
	bool result = false;

	if (statusControl_ && statusControl_->canMeasure())
		result = (statusControl_->value() == goodStatusValue_);

	return result;
}

bool AMBeamlineControl::isBad() const
{
	bool result = false;

	if (statusControl_ && statusControl_->canMeasure())
		result = (statusControl_->value() == badStatusValue_);

	return result;
}

double AMBeamlineControl::status() const
{
	double result = -1;

	if (statusControl_ && statusControl_->canMeasure())
		result = statusControl_->value();

	return result;
}

double AMBeamlineControl::value() const
{
	double result = -1;

	if (valueControl_ && valueControl_->canMeasure())
		result = valueControl_->value();

	return result;
}

void AMBeamlineControl::setStatusControl(AMControl *control, double goodValue, double badValue)
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

void AMBeamlineControl::setValueControl(AMControl *control)
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
