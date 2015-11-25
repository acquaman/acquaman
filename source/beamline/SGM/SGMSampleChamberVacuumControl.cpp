#include "SGMSampleChamberVacuumControl.h"
#include "source/actions3/AMListAction3.h"
#include "source/actions3/AMActionSupport.h"

SGMSampleChamberVacuumControl::SGMSampleChamberVacuumControl(const QString &name, QObject *parent) :
	AMSingleEnumeratedControl(name, "", parent)
{
	// Initialize class variables.

	turbos_ = 0;
	vatValve_ = 0;
}

SGMSampleChamberVacuumControl::~SGMSampleChamberVacuumControl()
{

}

void SGMSampleChamberVacuumControl::setPressure(AMControl *newControl)
{
	if (setBaseControl(newControl))
		emit pressureChanged(control_);
}

void SGMSampleChamberVacuumControl::setTurbos(AMControlSet *newControls)
{
	if (turbos_ != newControls) {

		if (turbos_)
			disconnect( turbos_, 0, this, 0 );

		turbos_ = newControls;

		if (turbos_)
			connect( turbos_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		updateConnected();

		emit turbosChanged(turbos_);
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
				turbos_ && turbos_->isConnected() &&
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
