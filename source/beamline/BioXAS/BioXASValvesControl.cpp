#include "BioXASValvesControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASValvesControl::BioXASValvesControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "", parent)
{
	// Initialize inherited variables.

	value_ = None;
	setpoint_ = None;
	minimumValue_ = None;
	maximumValue_ = Closed;

	setEnumStates(QStringList() << valueToString(None) << valueToString(Open) << valueToString(Closed));
	setMoveEnumStates(QStringList() << valueToString(Open));

	setContextKnownDescription("ValvesControl");

	// Create member variables.

	frontEndValveSet_ = 0;
	sideValveSet_ = 0;
	mainValveSet_ = 0;
	imagingValveSet_ = 0;

	// Current settings.

	updateStates();
}

BioXASValvesControl::~BioXASValvesControl()
{

}

bool BioXASValvesControl::canMeasure() const
{
	return false;
}

bool BioXASValvesControl::canMove() const
{
	return false;
}

bool BioXASValvesControl::validValue(double value) const
{
	bool result = false;

	switch (int(value)) {
	case None:
		result = true;
		break;
	case Open:
		result = true;
		break;
	case Closed:
		result = true;
		break;
	default:
		break;
	}

	return result;
}

bool BioXASValvesControl::validSetpoint(double value) const
{
	bool result = false;

	// Only want to be able to use this control to view current
	// state, and to open valves (not close them).

	if (value == BioXASValvesControl::Open)
		result = true;

	return result;
}

AMControlSet* BioXASValvesControl::valveSet(BioXAS::Beamline beamline) const
{
	AMControlSet *result = 0;

	switch (beamline) {
	case BioXAS::FrontEnd:
		result = frontEndValveSet_;
		break;
	case BioXAS::Side:
		result = sideValveSet_;
		break;
	case BioXAS::Main:
		result = mainValveSet_;
		break;
	case BioXAS::Imaging:
		result = imagingValveSet_;
		break;
	default:
		break;
	}

	return result;
}

bool BioXASValvesControl::isOpen() const
{
	bool result = valvesOpen(frontEndValveSet_) && valvesOpen(sideValveSet_) && valvesOpen(mainValveSet_) && valvesOpen(imagingValveSet_);
	return result;
}

bool BioXASValvesControl::isClosed() const
{
	bool result = valvesClosed(frontEndValveSet_) || valvesClosed(sideValveSet_) || valvesClosed(mainValveSet_) || valvesClosed(imagingValveSet_);
	return result;
}

QString BioXASValvesControl::valueToString(BioXASValvesControl::Value value) const
{
	QString result;

	switch (value) {
	case None:
		result = "None";
		break;
	case Open:
		result = "Open";
		break;
	case Closed:
		result = "Closed";
		break;
	default:
		break;
	}

	return result;
}

void BioXASValvesControl::setValves(BioXAS::Beamline beamline, AMControlSet *valveSet)
{
	switch (beamline) {
	case BioXAS::FrontEnd:
		setFrontEndValves(valveSet);
		break;
	case BioXAS::Side:
		setSideValves(valveSet);
		break;
	case BioXAS::Main:
		setMainValves(valveSet);
		break;
	case BioXAS::Imaging:
		setImagingValves(valveSet);
		break;
	default:
		break;
	}
}

void BioXASValvesControl::setFrontEndValves(AMControlSet *valveSet)
{
	setValveSet(frontEndValveSet_, valveSet);
}

void BioXASValvesControl::setSideValves(AMControlSet *valveSet)
{
	setValveSet(sideValveSet_, valveSet);
}

void BioXASValvesControl::setMainValves(AMControlSet *valveSet)
{
	setValveSet(mainValveSet_, valveSet);
}

void BioXASValvesControl::setImagingValves(AMControlSet *valveSet)
{
	setValveSet(imagingValveSet_, valveSet);
}

void BioXASValvesControl::addValve(BioXAS::Beamline beamline, CLSBiStateControl *valveControl)
{
	switch (beamline) {
	case BioXAS::FrontEnd:
		addFrontEndValve(valveControl);
		break;
	case BioXAS::Side:
		addSideValve(valveControl);
		break;
	case BioXAS::Main:
		addMainValve(valveControl);
		break;
	case BioXAS::Imaging:
		addImagingValve(valveControl);
		break;
	default:
		break;
	}
}

void BioXASValvesControl::addFrontEndValve(CLSBiStateControl *valveControl)
{
	if (!frontEndValveSet_)
		frontEndValveSet_ = new AMControlSet(this);

	addValveToSet(frontEndValveSet_, valveControl);
}

void BioXASValvesControl::addSideValve(CLSBiStateControl *valveControl)
{
	if (!sideValveSet_)
		sideValveSet_ = new AMControlSet(this);

	addValveToSet(sideValveSet_, valveControl);
}

void BioXASValvesControl::addMainValve(CLSBiStateControl *valveControl)
{
	if (!mainValveSet_)
		mainValveSet_ = new AMControlSet(this);

	addValveToSet(mainValveSet_, valveControl);
}

void BioXASValvesControl::addImagingValve(CLSBiStateControl *valveControl)
{
	if (!imagingValveSet_)
		imagingValveSet_ = new AMControlSet(this);

	addValveToSet(imagingValveSet_, valveControl);
}

void BioXASValvesControl::updateConnected()
{
	bool connected = (
				frontEndValveSet_ && frontEndValveSet_->isConnected() &&
				sideValveSet_ && sideValveSet_->isConnected() &&
				mainValveSet_ && mainValveSet_->isConnected() &&
				imagingValveSet_ && imagingValveSet_->isConnected()
				);

	setConnected(connected);
}

void BioXASValvesControl::updateValue()
{
	Value newValue = None;

	if (isOpen())
		newValue = Open;
	else if (isClosed())
		newValue = Closed;

	setValue(newValue);
}

void BioXASValvesControl::setValveSet(AMControlSet *toSet, AMControlSet *desiredControls)
{
	if (toSet != desiredControls) {

		if (toSet) {
			removeChildren(toSet);
			toSet->deleteLater();
			toSet = 0;
		}

		toSet = desiredControls;

		if (toSet)
			addChildren(toSet);
	}
}

AMAction3* BioXASValvesControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (setpoint == Open) {
		AMListAction3 *actionList = new AMListAction3(new AMListActionInfo3("Opening all BioXAS valves", "Opening all BioXAS valves"), AMListAction3::Parallel);
		actionList->addSubAction(createOpenValvesAction(frontEndValveSet_));
		actionList->addSubAction(createOpenValvesAction(sideValveSet_));
		actionList->addSubAction(createOpenValvesAction(mainValveSet_));
		actionList->addSubAction(createOpenValvesAction(imagingValveSet_));

		result = actionList;
	}

	return result;
}

AMAction3* BioXASValvesControl::createOpenValvesAction(AMControlSet *valves)
{
	AMAction3 *result = 0;

	if (valves && !valves->isEmpty()) {
		AMListAction3 *actionList = new AMListAction3(new AMListActionInfo3("Open valves", "Open valves"), AMListAction3::Parallel);

		foreach (AMControl *control, valves->toList()) {
			CLSBiStateControl *valve = qobject_cast<CLSBiStateControl*>(control);

			if (valve)
				actionList->addSubAction(AMActionSupport::buildControlMoveAction(valve, CLSBiStateControl::Open));
		}

		result = actionList;
	}

	return result;
}

void BioXASValvesControl::addChildren(AMControlSet *valveSet)
{
	if (valveSet) {
		foreach (AMControl *valve, valveSet->toList())
			addChildControl(valve);
	}
}

void BioXASValvesControl::removeChildren(AMControlSet *valveSet)
{
	if (valveSet) {
		foreach (AMControl *valve, valveSet->toList())
			removeChildControl(valve);
	}
}

bool BioXASValvesControl::addValveToSet(AMControlSet *controlSet, AMControl *valveControl)
{
	bool result = false;

	if (controlSet && valveControl) {
		if (!controlSet->contains(valveControl->name())) {
			controlSet->addControl(valveControl);
			addChildControl(valveControl);
			result = true;
		}
	}

	return result;
}

bool BioXASValvesControl::valvesOpen(AMControlSet *valveSet) const
{
	bool result = false;

	if (valveSet) {
		bool valvesOpen = true;

		for (int valveIndex = 0, valveCount = valveSet->count(); valveIndex < valveCount && valvesOpen; valveIndex++) {
			CLSBiStateControl *valveControl = qobject_cast<CLSBiStateControl*>(valveSet->at(valveIndex));

			if (valveControl) {
				valvesOpen &= valveControl->isOpen();
			} else {
				valvesOpen = false;
			}
		}

		result = valvesOpen;
	}

	return result;
}

bool BioXASValvesControl::valvesClosed(AMControlSet *valveSet) const
{
	bool result = false;

	if (valveSet) {
		bool valvesClosed = true;

		for (int valveIndex = 0, valveCount = valveSet->count(); valveIndex < valveCount && valvesClosed; valveIndex++) {
			CLSBiStateControl *valveControl = qobject_cast<CLSBiStateControl*>(valveSet->at(valveIndex));

			if (valveControl) {
				valvesClosed &= valveControl->isClosed();
			} else {
				valvesClosed = false;
			}
		}

		result = valvesClosed;
	}

	return result;
}
