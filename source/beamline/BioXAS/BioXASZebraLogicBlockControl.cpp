#include "BioXASZebraLogicBlockControl.h"

BioXASZebraLogicBlockControl::BioXASZebraLogicBlockControl(const QString &name, QObject *parent) :
	AMControl(name, "", parent)
{
	connected_ = false;

	inputControls_ = 0;
	outputStatusControl_ = 0;
}

BioXASZebraLogicBlockControl::~BioXASZebraLogicBlockControl()
{

}

bool BioXASZebraLogicBlockControl::outputStatus() const
{
	bool result = false;

	if (outputStatusControl_ && outputStatusControl_->canMeasure())
		result = (outputStatusControl_->value() == On);

	return result;
}

void BioXASZebraLogicBlockControl::setInputControlsSet(AMControlSet *newControls)
{
	if (inputControls_ != newControls) {

		if (inputControls_)
			disconnect( inputControls_, 0, this, 0 );

		inputControls_ = newControls;

		if (inputControls_)
			connect( inputControls_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		emit inputControlsSetChanged(inputControls_);
	}
}

void BioXASZebraLogicBlockControl::setOutputStatusControl(AMReadOnlyPVControl *newControl)
{
	if (outputStatusControl_ != newControl) {

		if (outputStatusControl_)
			disconnect( outputStatusControl_, 0, this, 0 );

		outputStatusControl_ = newControl;

		if (outputStatusControl_)
			connect( outputStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onOutputStatusValueChanged()) );

		emit outputStatusControlChanged(outputStatusControl_);
	}
}

void BioXASZebraLogicBlockControl::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connected(connected_);
	}
}

void BioXASZebraLogicBlockControl::updateConnected()
{
	bool newState = (
				inputControls_ && inputControls_->isConnected() &&
				outputStatusControl_ && outputStatusControl_->isConnected()
				);

	setConnected(newState);
}

void BioXASZebraLogicBlockControl::onOutputStatusValueChanged()
{
	emit outputStatusChanged(outputStatus());
}
