#include "BioXASUtilities.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMTemperatureMonitors.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"

BioXASUtilities::BioXASUtilities(const QString &name, QObject *parent) :
	BioXASUtilitiesGroup(name, parent)
{
	// Initialize shutters.

	shutters_ = new BioXASShutters(QString("%1%2").arg(name).arg("Shutters"), this);
	addControl(shutters_, BioXASShutters::Closed, BioXASShutters::Open);

	connect( shutters_, SIGNAL(valueChanged(double)), this, SIGNAL(shuttersValueChanged(double)) );

	// Initialize valves.

	beampathValves_ = new BioXASValves(QString("%1%2").arg(name).arg("BeampathValves"), this);
	addControl(beampathValves_, BioXASValves::Closed, BioXASValves::Open);

	connect( beampathValves_, SIGNAL(valueChanged(double)), this, SIGNAL(beampathValvesValueChanged(double)) );

	valves_ = new BioXASValves(QString("%1%2").arg(name).arg("Valves"), this);
	addControl(valves_, BioXASValves::Closed, BioXASValves::Open);

	connect( valves_, SIGNAL(valueChanged(double)), this, SIGNAL(valvesValueChanged(double)) );

	// Initialize ion pumps.

	ionPumps_ = new BioXASUtilitiesGroup(QString("%1%2").arg(name).arg("IonPumps"), this);
	addControl(ionPumps_);

	connect( ionPumps_, SIGNAL(valueChanged(double)), this, SIGNAL(ionPumpsValueChanged(double)) );

	// Initialize flow switches.

	flowSwitches_ = new BioXASUtilitiesGroup(QString("%1%2").arg(name).arg("FlowSwitches"), this);
	addControl(flowSwitches_);

	connect( flowSwitches_, SIGNAL(valueChanged(double)), this, SIGNAL(flowSwitchesValueChanged(double)) );

	// Initialize pressure monitors.

	pressureMonitors_ = new BioXASUtilitiesGroup(QString("%1%2").arg(name).arg("PressureMonitors"), this);
	addControl(pressureMonitors_);

	connect( pressureMonitors_, SIGNAL(valueChanged(double)), this, SIGNAL(pressureMonitorsValueChanged(double)) );

	// Initialize temperature monitors.

	temperatureMonitors_ = new AMTemperatureMonitors(QString("%1%2").arg(name).arg("TemperatureMonitors"), this);
	addControl(temperatureMonitors_->statusControl(), AMTemperatureMonitorsStatus::Bad, AMTemperatureMonitorsStatus::Good);

	// Initialize flow transducers.

	flowTransducers_ = new BioXASUtilitiesGroup(QString("%1%2").arg(name).arg("FlowTransducers"), this);
	addControl(flowTransducers_);

	connect( flowTransducers_, SIGNAL(valueChanged(double)), this, SIGNAL(flowTransducersValueChanged(double)) );
}

BioXASUtilities::~BioXASUtilities()
{

}

bool BioXASUtilities::isConnected() const
{
	bool connected = (
				shutters_ && shutters_->isConnected() &&
				beampathValves_ && beampathValves_->isConnected() &&
				valves_ && valves_->isConnected() &&
				ionPumps_ && ionPumps_->isConnected() &&
				flowSwitches_ && flowSwitches_->isConnected() &&
				pressureMonitors_ && pressureMonitors_->isConnected() &&
				temperatureMonitors_ && temperatureMonitors_->isConnected() &&
				flowTransducers_ && flowTransducers_->isConnected()
				);

	return connected;
}

double BioXASUtilities::shuttersValue() const
{
	double result = -1;

	if (shutters_ && shutters_->canMeasure())
		result = shutters_->value();

	return result;
}

double BioXASUtilities::beampathValvesValue() const
{
	double result = -1;

	if (beampathValves_ && beampathValves_->canMeasure())
		result = beampathValves_->value();

	return result;
}

double BioXASUtilities::valvesValue() const
{
	double result = -1;

	if (valves_ && valves_->canMeasure())
		result = valves_->value();

	return result;
}

double BioXASUtilities::ionPumpsValue() const
{
	double result = -1;

	if (ionPumps_ && ionPumps_->canMeasure())
		result = ionPumps_->value();

	return result;
}

double BioXASUtilities::flowSwitchesValue() const
{
	double result = -1;

	if (flowSwitches_ && flowSwitches_->canMeasure())
		result = flowSwitches_->value();

	return result;
}

double BioXASUtilities::pressureMonitorsValue() const
{
	double result = -1;

	if (pressureMonitors_ && pressureMonitors_->canMeasure())
		result = pressureMonitors_->value();

	return result;
}

double BioXASUtilities::temperatureMonitorsValue() const
{
	double result = -1;

	if (temperatureMonitors_ && temperatureMonitors_->canMeasure())
		result = temperatureMonitors_->value();

	return result;
}

double BioXASUtilities::flowTransducersValue() const
{
	double result = -1;

	if (flowTransducers_ && flowTransducers_->canMeasure())
		result = flowTransducers_->value();

	return result;
}

bool BioXASUtilities::hasShutter(AMControl *control) const
{
	bool result = false;

	if (shutters_)
		result = shutters_->hasShutter(control);

	return result;
}

bool BioXASUtilities::hasBeampathValve(AMControl *control) const
{
	bool result = false;

	if (beampathValves_)
		result = beampathValves_->hasValve(control);

	return result;
}

bool BioXASUtilities::hasValve(AMControl *control) const
{
	bool result = false;

	if (valves_)
		result = valves_->hasValve(control);

	return result;
}

bool BioXASUtilities::hasIonPump(AMControl *control) const
{
	bool result = false;

	if (ionPumps_)
		result = ionPumps_->hasControl(control);

	return result;
}

bool BioXASUtilities::hasFlowSwitch(AMControl *control) const
{
	bool result = false;

	if (flowSwitches_)
		result = flowSwitches_->hasControl(control);

	return result;
}

bool BioXASUtilities::hasPressureMonitor(AMControl *control) const
{
	bool result = false;

	if (pressureMonitors_)
		result = pressureMonitors_->hasControl(control);

	return result;
}

bool BioXASUtilities::hasTemperatureMonitor(AMControl *control) const
{
	bool result = false;

	if (temperatureMonitors_)
		result = temperatureMonitors_->hasChildControl(control);

	return result;
}

bool BioXASUtilities::hasFlowTransducer(AMControl *control) const
{
	bool result = false;

	if (flowTransducers_)
		result = flowTransducers_->hasControl(control);

	return result;
}

bool BioXASUtilities::addShutter(AMControl *newControl, double openValue, double closedValue)
{
	bool result = false;

	if (shutters_)
		result = shutters_->addShutter(newControl, openValue, closedValue);

	return result;
}

bool BioXASUtilities::removeShutter(AMControl *control)
{
	bool result = false;

	if (shutters_)
		result = shutters_->removeShutter(control);

	return result;
}

bool BioXASUtilities::clearShutters()
{
	bool result = false;

	if (shutters_)
		result = shutters_->clearShutters();

	return result;
}

bool BioXASUtilities::addBeampathValve(AMControl *newControl, double openValue, double closedValue)
{
	bool added = false;

	if (beampathValves_)
		added = beampathValves_->addValve(newControl, openValue, closedValue);

	// If the valve was added and is not a member of valves_, add it.

	if (added && !hasValve(newControl))
		addValve(newControl, openValue, closedValue);

	return added;
}

bool BioXASUtilities::removeBeampathValve(AMControl *control)
{
	bool result = false;

	if (beampathValves_)
		result = beampathValves_->removeValve(control);

	return result;
}

bool BioXASUtilities::clearBeampathValves()
{
	bool result = false;

	if (beampathValves_)
		result = beampathValves_->clearValves();

	return result;
}

bool BioXASUtilities::addValve(AMControl *newControl, double openValue, double closedValue)
{
	bool result = false;

	if (valves_)
		result = valves_->addValve(newControl, openValue, closedValue);

	return result;
}

bool BioXASUtilities::removeValve(AMControl *control)
{
	bool removed = false;

	if (valves_)
		removed = valves_->removeValve(control);

	// If the valve was removed and is a member of beampathValves_, remove it.

	if (removed && hasBeampathValve(control))
		removeBeampathValve(control);

	return removed;
}

bool BioXASUtilities::clearValves()
{
	bool result = false;

	if (valves_) {
		bool cleared = valves_->clearValves();

		if (cleared)
			clearBeampathValves();

		result = cleared;
	}

	return result;
}

bool BioXASUtilities::addIonPump(AMControl *newControl)
{
	bool result = false;

	if (ionPumps_)
		result = ionPumps_->addControl(newControl);

	return result;
}

bool BioXASUtilities::removeIonPump(AMControl *control)
{
	bool result = false;

	if (ionPumps_)
		result = ionPumps_->removeControl(control);

	return result;
}

bool BioXASUtilities::clearIonPumps()
{
	bool result = false;

	if (ionPumps_)
		result = ionPumps_->clearControls();

	return result;
}

bool BioXASUtilities::addFlowSwitch(AMControl *newControl)
{
	bool result = false;

	if (flowSwitches_)
		result = flowSwitches_->addControl(newControl);

	return result;
}

bool BioXASUtilities::removeFlowSwitch(AMControl *control)
{
	bool result = false;

	if (flowSwitches_)
		result = flowSwitches_->removeControl(control);

	return result;
}

bool BioXASUtilities::clearFlowSwitches()
{
	bool result = false;

	if (flowSwitches_)
		result = flowSwitches_->clearControls();

	return result;
}

bool BioXASUtilities::addPressureMonitor(AMControl *newControl)
{
	bool result = false;

	if (pressureMonitors_)
		result = pressureMonitors_->addControl(newControl);

	return result;
}

bool BioXASUtilities::removePressureMonitor(AMControl *control)
{
	bool result = false;

	if (pressureMonitors_)
		result = pressureMonitors_->removeControl(control);

	return result;
}

bool BioXASUtilities::clearPressureMonitors()
{
	bool result = false;

	if (pressureMonitors_)
		result = pressureMonitors_->clearControls();

	return result;
}

bool BioXASUtilities::addTemperatureMonitor(AMTemperatureMonitor *newControl)
{
	bool result = false;

	if (temperatureMonitors_)
		result = temperatureMonitors_->addMonitor(newControl);

	return result;
}

bool BioXASUtilities::removeTemperatureMonitor(AMTemperatureMonitor *control)
{
	bool result = false;

	if (temperatureMonitors_)
		result = temperatureMonitors_->removeMonitor(control);

	return result;
}

bool BioXASUtilities::clearTemperatureMonitors()
{
	bool result = false;

	if (temperatureMonitors_)
		result = temperatureMonitors_->clearMonitors();

	return result;
}

bool BioXASUtilities::addFlowTransducer(AMControl *newControl)
{
	bool result = false;

	if (flowTransducers_)
		result = flowTransducers_->addControl(newControl);

	return result;
}

bool BioXASUtilities::removeFlowTransducer(AMControl *control)
{
	bool result = false;

	if (flowTransducers_)
		result = flowTransducers_->removeControl(control);

	return result;
}

bool BioXASUtilities::clearFlowTransducers()
{
	bool result = false;

	if (flowTransducers_)
		result = flowTransducers_->clearControls();

	return result;
}

void BioXASUtilities::updateMoving()
{
	setIsMoving(false);
}

int BioXASUtilities::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isBad())
		result = Bad;
	else if (isGood())
		result = Good;

	return result;
}
