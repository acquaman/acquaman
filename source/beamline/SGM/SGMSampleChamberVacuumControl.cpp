#include "SGMSampleChamberVacuumControl.h"

SGMSampleChamberVacuumControl::SGMSampleChamberVacuumControl(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize class variables.

	turboPump5Running_ = 0;
	turboPump6Running_ = 0;
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

void SGMSampleChamberVacuumControl::setTurboPump5Running(AMSinglePVControl *newControl)
{
	if (turboPump5Running_ != newControl) {

		if (turboPump5Running_)
			removeChildControl(turboPump5Running_);

		turboPump5Running_ = newControl;

		if (turboPump5Running_)
			addChildControl(turboPump5Running_);

		updateConnected();

		emit turboPump5RunningChanged(turboPump5Running_);
	}
}

void SGMSampleChamberVacuumControl::setTurboPump6Running(AMSinglePVControl *newControl)
{
	if (turboPump6Running_ != newControl) {

		if (turboPump6Running_)
			removeChildControl(turboPump6Running_);

		turboPump6Running_ = newControl;

		if (turboPump6Running_)
			addChildControl(turboPump6Running_);

		updateConnected();

		emit turboPump6RunningChanged(turboPump6Running_);
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
				turboPump5Running_ && turboPump5Running_->isConnected() &&
				turboPump6Running_ && turboPump6Running_->isConnected() &&
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
