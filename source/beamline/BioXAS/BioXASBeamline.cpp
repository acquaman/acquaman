#include "BioXASBeamline.h"

#include "actions3/AMActionSupport.h"
#include "beamline/AMControlSet.h"
#include "beamline/CLS/CLSStorageRing.h"
#include "util/AMErrorMonitor.h"

BioXASBeamline::~BioXASBeamline()
{

}

bool BioXASBeamline::isConnected() const
{
	bool connected = (
				beamStatus_ && beamStatus_->isConnected() &&
				utilities_ && utilities_->isConnected() &&
				detectorStageLateralMotors_ && detectorStageLateralMotors_->isConnected() &&
				ge32Detectors_ && ge32Detectors_->isConnnected()
				);

	return connected;
}

BioXASShutters* BioXASBeamline::shutters() const
{
	BioXASShutters *result = 0;

	if (utilities_)
		result = utilities_->shutters();

	return result;
}

BioXASValves* BioXASBeamline::beampathValves() const
{
	BioXASValves *result = 0;

	if (utilities_)
		result = utilities_->beampathValves();

	return result;
}

BioXASValves* BioXASBeamline::valves() const
{
	BioXASValves *result = 0;

	if (utilities_)
		result = utilities_->valves();

	return result;
}

BioXASUtilitiesGroup* BioXASBeamline::ionPumps() const
{
	BioXASUtilitiesGroup *result = 0;

	if (utilities_)
		result = utilities_->ionPumps();

	return result;
}

BioXASUtilitiesGroup* BioXASBeamline::flowSwitches() const
{
	BioXASUtilitiesGroup *result = 0;

	if (utilities_)
		result = utilities_->flowSwitches();

	return result;
}

BioXASUtilitiesGroup* BioXASBeamline::pressureMonitors() const
{
	BioXASUtilitiesGroup *result = 0;

	if (utilities_)
		result = utilities_->pressureMonitors();

	return result;
}

BioXASUtilitiesGroup* BioXASBeamline::temperatureMonitors() const
{
	BioXASUtilitiesGroup *result = 0;

	if (utilities_)
		result = utilities_->temperatureMonitors();

	return result;
}

BioXASUtilitiesGroup* BioXASBeamline::flowTransducers() const
{
	BioXASUtilitiesGroup *result = 0;

	if (utilities_)
		result = utilities_->flowTransducers();

	return result;
}

AMBasicControlDetectorEmulator* BioXASBeamline::detectorForControl(AMControl *control) const
{
	return controlDetectorMap_.value(control, 0);
}

bool BioXASBeamline::addDetectorStageLateralMotor(CLSMAXvMotor *newMotor)
{
	bool result = false;

	if (detectorStageLateralMotors_->addControl(newMotor)) {
		result = true;
		emit detectorStageLateralMotorsChanged();
	}

	return result;
}

bool BioXASBeamline::removeDetectorStageLateralMotor(CLSMAXvMotor *motor)
{
	bool result = false;

	if (detectorStageLateralMotors_->removeControl(motor)) {
		result = true;
		emit detectorStageLateralMotorsChanged();
	}

	return result;
}

bool BioXASBeamline::clearDetectorStageLateralMotors()
{
	detectorStageLateralMotors_->clear();
	emit detectorStageLateralMotorsChanged();
	return true;
}

bool BioXASBeamline::addGe32Detector(BioXAS32ElementGeDetector *newDetector)
{
	bool result = true;

	if (ge32Detectors_->addDetector(newDetector)) {
		result = true;
		emit ge32DetectorsChanged();
	}

	return result;
}

bool BioXASBeamline::removeGe32Detector(BioXAS32ElementGeDetector *detector)
{
	bool result = false;

	if (ge32Detectors_->removeDetector(detector)) {
		result = true;
		emit ge32DetectorsChanged();
	}

	return result;
}

bool BioXASBeamline::clearGe32Detectors()
{
	ge32Detectors_->clear();
	emit ge32DetectorsChanged();
	return true;
}

void BioXASBeamline::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connectedChanged(connected_);
	}
}

void BioXASBeamline::updateConnected()
{
	setConnected( isConnected() );
}

void BioXASBeamline::addShutter(AMControl *newControl, double openValue, double closedValue)
{
	if (utilities_)
		utilities_->addShutter(newControl, openValue, closedValue);

	if (beamStatus_)
		beamStatus_->addShutter(newControl, openValue, closedValue);
}

void BioXASBeamline::removeShutter(AMControl *control)
{
	if (utilities_)
		utilities_->removeShutter(control);

	if (beamStatus_)
		beamStatus_->removeShutter(control);
}

void BioXASBeamline::clearShutters()
{
	if (utilities_)
		utilities_->clearShutters();

	if (beamStatus_)
		beamStatus_->clearShutters();
}

void BioXASBeamline::addBeampathValve(AMControl *newControl, double openValue, double closedValue)
{
	if (utilities_)
		utilities_->addBeampathValve(newControl, openValue, closedValue);

	if (beamStatus_)
		beamStatus_->addValve(newControl, openValue, closedValue);
}

void BioXASBeamline::removeBeampathValve(AMControl *control)
{
	if (utilities_)
		utilities_->removeBeampathValve(control);

	if (beamStatus_)
		beamStatus_->removeValve(control);
}

void BioXASBeamline::clearBeampathValves()
{
	if (utilities_)
		utilities_->clearBeampathValves();

	if (beamStatus_)
		beamStatus_->clearValves();
}

void BioXASBeamline::addValve(AMControl *newControl, double openValue, double closedValue)
{
	if (utilities_)
		utilities_->addValve(newControl, openValue, closedValue);
}

void BioXASBeamline::removeValve(AMControl *control)
{
	if (utilities_)
		utilities_->removeValve(control);
}

void BioXASBeamline::clearValves()
{
	if (utilities_)
		utilities_->clearValves();
}

void BioXASBeamline::addIonPump(AMControl *newControl)
{
	if (utilities_)
		utilities_->addIonPump(newControl);
}

void BioXASBeamline::removeIonPump(AMControl *control)
{
	if (utilities_)
		utilities_->removeIonPump(control);
}

void BioXASBeamline::clearIonPumps()
{
	if (utilities_)
		utilities_->clearIonPumps();
}

void BioXASBeamline::addFlowSwitch(AMControl *newControl)
{
	if (utilities_)
		utilities_->addFlowSwitch(newControl);
}

void BioXASBeamline::removeFlowSwitch(AMControl *control)
{
	if (utilities_)
		utilities_->removeFlowSwitch(control);
}

void BioXASBeamline::clearFlowSwitches()
{
	if (utilities_)
		utilities_->clearFlowSwitches();
}

void BioXASBeamline::addPressureMonitor(AMControl *newControl)
{
	if (utilities_)
		utilities_->addPressureMonitor(newControl);
}

void BioXASBeamline::removePressureMonitor(AMControl *control)
{
	if (utilities_)
		utilities_->removePressureMonitor(control);
}

void BioXASBeamline::clearPressureMonitors()
{
	if (utilities_)
		utilities_->clearPressureMonitors();
}

void BioXASBeamline::addTemperatureMonitor(AMControl *newControl)
{
	if (utilities_)
		utilities_->addTemperatureMonitor(newControl);
}

void BioXASBeamline::removeTemperatureMonitor(AMControl *control)
{
	if (utilities_)
		utilities_->removeTemperatureMonitor(control);
}

void BioXASBeamline::clearTemperatureMonitors()
{
	if (utilities_)
		utilities_->clearTemperatureMonitors();
}

void BioXASBeamline::addFlowTransducer(AMControl *newControl)
{
	if (utilities_)
		utilities_->addFlowTransducer(newControl);
}

void BioXASBeamline::removeFlowTransducer(AMControl *control)
{
	if (utilities_)
		utilities_->removeFlowTransducer(control);
}

void BioXASBeamline::clearFlowTransducers()
{
	if (utilities_)
		utilities_->clearFlowTransducers();
}

void BioXASBeamline::setupComponents()
{
	// Beam status.

	beamStatus_ = new BioXASBeamStatus("BioXASBeamStatus", this);
	connect( beamStatus_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Utilities.

	utilities_ = new BioXASUtilities("BioXASUtilities", this);
	connect( utilities_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Utilities - front-end shutters.

	addShutter(new BioXASFrontEndShutters("Front-end shutters", this), BioXASFrontEndShutters::Open, BioXASFrontEndShutters::Closed);

	// Utilities - front-end beampath valves.

	addBeampathValve(new AMReadOnlyPVControl("VVR1407-I00-01", "VVR1407-I00-01:state", this), BIOXASBEAMLINE_VALVE_OPEN, BIOXASBEAMLINE_VALVE_CLOSED);
	addBeampathValve(new AMReadOnlyPVControl("VVF1407-I00-01", "VVF1407-I00-01:state", this), BIOXASBEAMLINE_FASTVALVE_OPEN, BIOXASBEAMLINE_FASTVALVE_CLOSED);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I00-01", "VVR1607-5-I00-01:state", "VVR1607-5-I00-01:opr:open", "VVR1607-5-I00-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-01", "VVR1607-5-I22-01:state", "VVR1607-5-I22-01:opr:open", "VVR1607-5-I22-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-01", "VVR1607-5-I21-01:state", "VVR1607-5-I21-01:opr:open", "VVR1607-5-I21-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-01", "VVR1607-5-I10-01:state", "VVR1607-5-I10-01:opr:open", "VVR1607-5-I10-01:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - Side beampath valves.

	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-02", "VVR1607-5-I22-02:state", "VVR1607-5-I22-02:opr:open", "VVR1607-5-I22-02:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-03", "VVR1607-5-I22-03:state", "VVR1607-5-I22-03:opr:open", "VVR1607-5-I22-03:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-04", "VVR1607-5-I22-04:state", "VVR1607-5-I22-04:opr:open", "VVR1607-5-I22-04:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-06", "VVR1607-5-I22-06:state", "VVR1607-5-I22-06:opr:open", "VVR1607-5-I22-06:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I22-07", "VVR1607-5-I22-07:state", "VVR1607-5-I22-07:opr:open", "VVR1607-5-I22-07:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - Main beampath valves.

	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-02", "VVR1607-5-I21-02:state", "VVR1607-5-I21-02:opr:open", "VVR1607-5-I21-02:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-03", "VVR1607-5-I21-03:state", "VVR1607-5-I21-03:opr:open", "VVR1607-5-I21-03:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-04", "VVR1607-5-I21-04:state", "VVR1607-5-I21-04:opr:open", "VVR1607-5-I21-04:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-06", "VVR1607-5-I21-06:state", "VVR1607-5-I21-06:opr:open", "VVR1607-5-I21-06:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I21-07", "VVR1607-5-I21-07:state", "VVR1607-5-I21-07:opr:open", "VVR1607-5-I21-07:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - Imaging beampath valves.

	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-02", "VVR1607-5-I10-02:state", "VVR1607-5-I10-02:opr:open", "VVR1607-5-I10-02:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-03", "VVR1607-5-I10-03:state", "VVR1607-5-I10-03:opr:open", "VVR1607-5-I10-03:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-04", "VVR1607-5-I10-04:state", "VVR1607-5-I10-04:opr:open", "VVR1607-5-I10-04:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-05", "VVR1607-5-I10-05:state", "VVR1607-5-I10-05:opr:open", "VVR1607-5-I10-05:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	//addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-06", "VVR1607-5-I10-06:state", "VVR1607-5-I10-06:opr:open", "VVR1607-5-I10-06:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	//addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-07", "VVR1607-5-I10-07:state", "VVR1607-5-I10-07:opr:open", "VVR1607-5-I10-07:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-08", "VVR1607-5-I10-08:state", "VVR1607-5-I10-08:opr:open", "VVR1607-5-I10-08:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);
	addBeampathValve(new CLSExclusiveStatesControl("VVR1607-5-I10-10", "VVR1607-5-I10-10:state", "VVR1607-5-I10-10:opr:open", "VVR1607-5-I10-10:opr:close", this), CLSExclusiveStatesControl::Open, CLSExclusiveStatesControl::Closed);

	// Utilities - front-end ion pumps.

	addIonPump(new AMReadOnlyPVControl("IOP1407-I00-01", "IOP1407-I00-01", this));
	addIonPump(new AMReadOnlyPVControl("IOP1407-I00-02", "IOP1407-I00-02", this));
	addIonPump(new AMReadOnlyPVControl("IOP1407-I00-03", "IOP1407-I00-03", this));
	addIonPump(new AMReadOnlyPVControl("IOP1607-5-I00-01", "IOP1607-5-I00-01", this));
	addIonPump(new AMReadOnlyPVControl("IOP1607-5-I00-02", "IOP1607-5-I00-02", this));

	// Utilities - front-end flow switches.

	addFlowSwitch(new AMReadOnlyPVControl("SWF1407-I00-01", "SWF1407-I00-01", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1407-I00-02", "SWF1407-I00-02", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1407-I00-03", "SWF1407-I00-03", this));

	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I00-01", "SWF1607-5-I00-01", this));

	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I20-01", "SWF1607-5-I20-01", this));

	// Utilities - beamline flow switches.

	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-01", "SWF1607-5-I10-01", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-02", "SWF1607-5-I10-02", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-03", "SWF1607-5-I10-03", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-04", "SWF1607-5-I10-04", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-05", "SWF1607-5-I10-05", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-06", "SWF1607-5-I10-06", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I10-07", "SWF1607-5-I10-07", this));

	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I20-01", "SWF1607-5-I20-01", this));

	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I21-01", "SWF1607-5-I21-01", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I21-02", "SWF1607-5-I21-02", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I21-03", "SWF1607-5-I21-03", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I21-04", "SWF1607-5-I21-04", this));

	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I22-01", "SWF1607-5-I22-01", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I22-02", "SWF1607-5-I22-02", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I22-03", "SWF1607-5-I22-03", this));
	addFlowSwitch(new AMReadOnlyPVControl("SWF1607-5-I22-04", "SWF1607-5-I22-04", this));

	// Utilities - front-end pressure monitors.

	addPressureMonitor(new AMReadOnlyPVControl("CCG1407-I00-01", "CCG1407-I00-01:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1407-I00-02", "CCG1407-I00-02:vac", this));

	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I00-02", "CCG1607-5-I00-02:vac", this));

	// Utilities - beamline pressure monitors.

	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I00-03", "CCG1607-5-I00-03:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I00-04", "CCG1607-5-I00-04:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I00-05", "CCG1607-5-I00-05:vac", this));

	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I10-01", "CCG1607-5-I10-01:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I10-02", "CCG1607-5-I10-02:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I10-03", "CCG1607-5-I10-03:vac", this));

	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-8-I10-01", "CCG1607-8-I10-01:vac", this));

	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I21-01", "CCG1607-5-I21-01:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I21-02", "CCG1607-5-I21-02:vac", this));

	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I22-01", "CCG1607-5-I22-01:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I22-02", "CCG1607-5-I22-02:vac", this));
	addPressureMonitor(new AMReadOnlyPVControl("CCG1607-5-I22-03", "CCG1607-5-I22-03:vac", this));

	// Utilities - front-end temperature monitors.

	addTemperatureMonitor(new AMReadOnlyPVControl("TM1407-I00-01", "TM1407-I00-01:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1407-I00-02", "TM1407-I00-02:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1407-I00-03", "TM1407-I00-03:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1407-I00-04", "TM1407-I00-04:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1407-I00-05", "TM1407-I00-05:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I00-01", "TM1607-5-I00-01:trip", this));

	// Utilities - beamline temperature monitors.

	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-01", "TM1607-5-I10-01:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-02", "TM1607-5-I10-02:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-03", "TM1607-5-I10-03:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-04", "TM1607-5-I10-04:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-05", "TM1607-5-I10-05:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-06", "TM1607-5-I10-06:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-07", "TM1607-5-I10-07:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-08", "TM1607-5-I10-08:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-09", "TM1607-5-I10-09:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-10", "TM1607-5-I10-10:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-11", "TM1607-5-I10-11:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-12", "TM1607-5-I10-12:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-13", "TM1607-5-I10-13:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-14", "TM1607-5-I10-14:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I10-15", "TM1607-5-I10-15:trip", this));

	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I20-01", "TM1607-5-I20-01:trip", this));

	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-01", "TM1607-5-I21-01:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-02", "TM1607-5-I21-02:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-03", "TM1607-5-I21-03:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-04", "TM1607-5-I21-04:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-05", "TM1607-5-I21-05:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-06", "TM1607-5-I21-06:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-07", "TM1607-5-I21-07:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-08", "TM1607-5-I21-08:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-09", "TM1607-5-I21-09:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-10", "TM1607-5-I21-10:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I21-11", "TM1607-5-I21-11:trip", this));

	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-01", "TM1607-5-I21-01:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-02", "TM1607-5-I21-02:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-03", "TM1607-5-I21-03:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-04", "TM1607-5-I21-04:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-05", "TM1607-5-I21-05:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-06", "TM1607-5-I21-06:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-07", "TM1607-5-I21-07:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-08", "TM1607-5-I21-08:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-09", "TM1607-5-I21-09:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-10", "TM1607-5-I21-10:trip", this));
	addTemperatureMonitor(new AMReadOnlyPVControl("TM1607-5-I22-11", "TM1607-5-I21-11:trip", this));

	// Utilities - front-end flow transducers.

	addFlowTransducer(new AMReadOnlyPVControl("FLT1407-I00-01", "FLT1407-I00-01:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1407-I00-02", "FLT1407-I00-02:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1407-I00-03", "FLT1407-I00-03:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I00-01", "FLT1607-5-I00-01:lowflow", this));

	// Utilities - beamline flow transducers.

	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-01", "FLT1607-5-I10-01:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-02", "FLT1607-5-I10-02:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-03", "FLT1607-5-I10-03:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-04", "FLT1607-5-I10-04:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-05", "FLT1607-5-I10-05:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-06", "FLT1607-5-I10-06:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I10-07", "FLT1607-5-I10-07:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I20-01", "FLT1607-5-I20-01:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I21-01", "FLT1607-5-I21-01:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I21-02", "FLT1607-5-I21-02:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I21-03", "FLT1607-5-I21-03:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I21-04", "FLT1607-5-I21-04:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I22-01", "FLT1607-5-I22-01:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I22-02", "FLT1607-5-I22-02:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I22-03", "FLT1607-5-I22-03:lowflow", this));
	addFlowTransducer(new AMReadOnlyPVControl("FLT1607-5-I22-04", "FLT1607-5-I22-04:lowflow", this));

	// Detector stage motors.

	detectorStageLateralMotors_ = new AMControlSet(this);
	connect( detectorStageLateralMotors_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// 32Ge detectors.

	ge32Detectors_ = new AMDetectorSet(this);
	connect( ge32Detectors_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

AMBasicControlDetectorEmulator* BioXASBeamline::createDetectorEmulator(const QString &name, const QString &description, AMControl *control, bool hiddenFromUsers, bool isVisible)
{
	AMBasicControlDetectorEmulator *detector = 0;

	if (control) {
		detector = new AMBasicControlDetectorEmulator(name, description, control, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
		detector->setHiddenFromUsers(hiddenFromUsers);
		detector->setIsVisible(isVisible);
	}

	return detector;
}

void BioXASBeamline::addControlAsDetector(const QString &name, const QString &description, AMControl *control, bool hiddenFromUsers, bool isVisible)
{
	if (control && !controlDetectorMap_.contains(control)) {
		AMBasicControlDetectorEmulator *detector = createDetectorEmulator(name, description, control, hiddenFromUsers, isVisible);
		controlDetectorMap_.insert(control, detector);
	}
}

BioXASBeamline::BioXASBeamline(const QString &controlName) :
	CLSBeamline(controlName)
{
	// Initialize member variables.

	connected_ = false;

	beamStatus_ = 0;
	utilities_ = 0;

	// Setup procedures.

	setupComponents();
}
