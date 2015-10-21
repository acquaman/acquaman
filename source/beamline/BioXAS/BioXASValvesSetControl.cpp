#include "BioXASValvesSetControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASValvesSetControl::BioXASValvesSetControl(const QString &name, QObject *parent) :
	BioXASValvesControl(name, parent)
{
	// Initialize class variables.

	valveSet_ = 0;

	// Current settings.

	updateStates();
}

BioXASValvesSetControl::~BioXASValvesSetControl()
{

}

bool BioXASValvesSetControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = valvesCanMeasure(valveSet_);

	return result;
}

bool BioXASValvesSetControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = valvesCanMove(valveSet_);

	return result;
}

bool BioXASValvesSetControl::isOpen() const
{
	bool result = valvesOpen(valveSet_);
	return result;
}

bool BioXASValvesSetControl::isClosed() const
{
	bool result = valvesClosed(valveSet_);
	return result;
}

void BioXASValvesSetControl::setValveSet(AMControlSet *newSet)
{
	if (valveSet_ != newSet) {

		if (valveSet_)
			disconnect( valveSet_, 0, this, 0 );

		valveSet_ = newSet;

		if (valveSet_)
			connect( valveSet_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

		updateStates();

		emit valveSetChanged(valveSet_);
	}
}

void BioXASValvesSetControl::updateConnected()
{
	bool connected = (
				valveSet_ && valveSet_->isConnected()
				);

	setConnected(connected);
}

AMAction3* BioXASValvesSetControl::createOpenValvesAction()
{
	AMAction3 *result = 0;

	if (valveSet_ && !valveSet_->isEmpty()) {
		AMListAction3 *actionList = new AMListAction3(new AMListActionInfo3("Open valves", "Open valves"), AMListAction3::Parallel);

		foreach (AMControl *control, valveSet_->toList()) {
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

bool BioXASValvesSetControl::valvesCanMeasure(AMControlSet *valveSet)
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

bool BioXASValvesSetControl::valvesCanMove(AMControlSet *valveSet)
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

bool BioXASValvesSetControl::valvesOpen(AMControlSet *valveSet)
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

bool BioXASValvesSetControl::valvesClosed(AMControlSet *valveSet)
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

