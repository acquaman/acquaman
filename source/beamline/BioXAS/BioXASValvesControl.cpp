#include "BioXASValvesControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASValvesControl::BioXASValvesControl(const QString &name, QObject *parent) :
	AMPseudoMotorControl(name, "", parent)
{
	// Initialize inherited variables.

	value_ = None;
	setpoint_ = None;
	minimumValue_ = Open;
	maximumValue_ = None;

	setEnumStates(QStringList() << valueToString(Open) << valueToString(Closed) << valueToString(None));
	setMoveEnumStates(QStringList() << valueToString(Open));

	setContextKnownDescription("ValvesControl");

	// Initialize class variables.

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
	bool result = false;

	if (isConnected())
		result = (valvesCanMeasure(frontEndValveSet_) && valvesCanMeasure(sideValveSet_) && valvesCanMeasure(mainValveSet_) && valvesCanMeasure(imagingValveSet_));

	return result;
}

bool BioXASValvesControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = (valvesCanMove(frontEndValveSet_) && valvesCanMove(sideValveSet_) && valvesCanMove(mainValveSet_) && valvesCanMove(imagingValveSet_));

	return result;
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
	case Open:
		result = "Open";
		break;
	case Closed:
		result = "Closed";
		break;
	case None:
		result = "None";
		break;
	default:
		break;
	}

	return result;
}

void BioXASValvesControl::setFrontEndValves(AMControlSet *valveSet)
{
	if (frontEndValveSet_ != valveSet) {

		if (frontEndValveSet_)
			disconnect( frontEndValveSet_, 0, this, 0 );

		frontEndValveSet_ = valveSet;

		if (frontEndValveSet_)
			connect( frontEndValveSet_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		updateStates();

		emit frontEndValveSetChanged(frontEndValveSet_);
	}
}

void BioXASValvesControl::setSideValves(AMControlSet *valveSet)
{
	if (sideValveSet_ != valveSet) {

		if (sideValveSet_)
			disconnect( sideValveSet_, 0, this, 0 );

		sideValveSet_ = valveSet;

		if (sideValveSet_)
			connect( sideValveSet_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		updateStates();

		emit sideValvesSetChanged(sideValveSet_);
	}
}

void BioXASValvesControl::setMainValves(AMControlSet *valveSet)
{
	if (mainValveSet_ != valveSet) {

		if (mainValveSet_)
			disconnect( mainValveSet_, 0, this, 0 );

		mainValveSet_ = valveSet;

		if (mainValveSet_)
			connect( mainValveSet_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		updateStates();

		emit mainValvesSetChanged(mainValveSet_);
	}
}

void BioXASValvesControl::setImagingValves(AMControlSet *valveSet)
{
	if (imagingValveSet_ != valveSet) {

		if (imagingValveSet_)
			disconnect( imagingValveSet_, 0, this, 0 );

		imagingValveSet_ = valveSet;

		if (imagingValveSet_)
			connect( imagingValveSet_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		updateStates();

		emit imagingValvesSetChanged(imagingValveSet_);
	}
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
			CLSBiStateControl *biStateValve = qobject_cast<CLSBiStateControl*>(control);
			if (biStateValve)
				actionList->addSubAction(AMActionSupport::buildControlMoveAction(biStateValve, CLSBiStateControl::Open));

			AMReadOnlyPVControl *readOnlyValve = qobject_cast<AMReadOnlyPVControl*>(control);
			if (!biStateValve && readOnlyValve)
				actionList->addSubAction(AMActionSupport::buildControlWaitAction(readOnlyValve, 1));
		}

		result = actionList;
	}

	return result;
}

bool BioXASValvesControl::valvesCanMeasure(AMControlSet *valveSet) const
{
	bool result = false;

	if (valveSet) {
		bool canMeasure = true;

		for (int valveIndex = 0, valveCount = valveSet->count(); valveIndex < valveCount && canMeasure; valveIndex++) {
			AMControl *valve = valveSet->at(valveIndex);
			canMeasure &= (valve && valve->canMeasure());
		}

		result = canMeasure;
	}

	return result;
}

bool BioXASValvesControl::valvesCanMove(AMControlSet *valveSet) const
{
	bool result = false;

	if (valveSet) {
		bool canMove = true;

		for (int valveIndex = 0, valveCount = valveSet->count(); valveIndex < valveCount && canMove; valveIndex++) {
			AMControl *valve = valveSet->at(valveIndex);
			canMove &= (valve && valve->canMove());
		}

		result = canMove;
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
