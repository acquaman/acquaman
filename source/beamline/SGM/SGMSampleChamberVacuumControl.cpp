#include "SGMSampleChamberVacuumControl.h"

SGMSampleChamberVacuumControl::SGMSampleChamberVacuumControl(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize class variables.

	turbo5_ = 0;
	turbo6_ = 0;
	vatValve_ = 0;
}

SGMSampleChamberVacuumControl::~SGMSampleChamberVacuumControl()
{

}

void SGMSampleChamberVacuumControl::setPressure(AMReadOnlyPVControl *newControl)
{
	if (setBaseControl(newControl))
		emit pressureChanged(control_);
}

void SGMSampleChamberVacuumControl::setTurbo5(SGMTurboPump *newControl)
{
	if (turbo5_ != newControl) {

		if (turbo5_)
			removeChildControl(turbo5_);

		turbo5_ = newControl;

		if (turbo5_)
			addChildControl(turbo5_);

		updateConnected();

		emit turbo5Changed(turbo5_);
	}
}

void SGMSampleChamberVacuumControl::setTurbo6(SGMTurboPump *newControl)
{
	if (turbo6_ != newControl) {

		if (turbo6_)
			removeChildControl(turbo6_);

		turbo6_ = newControl;

		if (turbo6_)
			addChildControl(turbo6_);

		updateConnected();

		emit turbo6Changed(turbo6_);
	}
}

void SGMSampleChamberVacuumControl::setVATValve(SGMVATValve *newControl)
{
	if (vatValve_ != newControl) {

		if (vatValve_)
			removeChildControl(vatValve_);

		vatValve_ = newControl;

		if (vatValve_)
			addChildControl(vatValve_);

		updateConnected();

		emit vatValveChanged(vatValve_);
	}
}

void SGMSampleChamberVacuumControl::updateConnected()
{
	bool isConnected = (
				AMSingleEnumeratedControl::isConnected() &&
				turbo5_ && turbo5_->isConnected() &&
				turbo6_ && turbo6_->isConnected() &&
				vatValve_ && vatValve_->isConnected()
				);

	setConnected(isConnected);
}

bool SGMSampleChamberVacuumControl::addVacuumOption(int index, const QString &optionString, double valueMin, double valueMax)
{
	bool result = addValueOption(index, optionString, index, valueMin, valueMax);

	if (result)
		emit vacuumOptionsChanged();

	return result;
}

bool SGMSampleChamberVacuumControl::removeVacuumOption(int index)
{
	bool result = removeOption(index);

	if (result)
		emit vacuumOptionsChanged();

	return result;
}
