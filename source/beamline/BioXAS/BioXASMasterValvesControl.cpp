#include "BioXASMasterValvesControl.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

BioXASMasterValvesControl::BioXASMasterValvesControl(const QString &name, QObject *parent) :
	BioXASValvesControl(name, parent)
{
	// Initialize class variables.

	frontEndValves_ = 0;
	sideValves_ = 0;
	mainValves_ = 0;
	imagingValves_ = 0;

	// Current settings.

	updateStates();
}

BioXASMasterValvesControl::~BioXASMasterValvesControl()
{

}

bool BioXASMasterValvesControl::canMeasure() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndValves_->canMeasure() && sideValves_->canMeasure() && mainValves_->canMeasure() && imagingValves_->canMeasure());

	return result;
}

bool BioXASMasterValvesControl::canMove() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndValves_->canMove() && sideValves_->canMove() && mainValves_->canMove() && imagingValves_->canMove());

	return result;
}

bool BioXASMasterValvesControl::isOpen() const
{
	bool result = false;

	if (isConnected())
		result = (frontEndValves_->isOpen() && sideValves_->isOpen() && mainValves_->isOpen() && imagingValves_->isOpen());

	return result;
}

void BioXASMasterValvesControl::setFrontEndValves(BioXASValvesSetControl *newValves)
{
	if (frontEndValves_ != newValves) {

		if (frontEndValves_)
			removeChildControl(frontEndValves_);

		frontEndValves_ = newValves;

		if (frontEndValves_)
			addChildControl(frontEndValves_);

		updateStates();

		emit frontEndValvesChanged(frontEndValves_);
	}
}

void BioXASMasterValvesControl::setSideValves(BioXASValvesSetControl *newValves)
{
	if (sideValves_ != newValves) {

		if (sideValves_)
			removeChildControl(sideValves_);

		sideValves_ = newValves;

		if (sideValves_)
			addChildControl(sideValves_);

		updateStates();

		emit sideValvesChanged(sideValves_);
	}
}

void BioXASMasterValvesControl::setMainValves(BioXASValvesSetControl *newValves)
{
	if (mainValves_ != newValves) {

		if (mainValves_)
			removeChildControl(mainValves_);

		mainValves_ = newValves;

		if (mainValves_)
			addChildControl(mainValves_);

		updateStates();

		emit mainValvesChanged(mainValves_);
	}
}

void BioXASMasterValvesControl::setImagingValves(BioXASValvesSetControl *newValves)
{
	if (imagingValves_ != newValves) {

		if (imagingValves_)
			removeChildControl(imagingValves_);

		imagingValves_ = newValves;

		if (imagingValves_)
			addChildControl(imagingValves_);

		updateStates();

		emit imagingValvesChanged(imagingValves_);
	}
}

void BioXASMasterValvesControl::updateConnected()
{
	bool connected = (
				frontEndValves_ && frontEndValves_->isConnected() &&
				sideValves_ && sideValves_->isConnected() &&
				mainValves_ && mainValves_->isConnected() &&
				imagingValves_ && imagingValves_->isConnected()
				);

	setConnected(connected);
}

AMAction3* BioXASMasterValvesControl::createOpenValvesAction()
{
	AMListAction3 *actionsList = new AMListAction3(new AMListActionInfo3("Opening all valves", "Opening all valves"), AMListAction3::Parallel);
	actionsList->addSubAction(AMActionSupport::buildControlMoveAction(frontEndValves_, BioXASValvesControl::Open));
	actionsList->addSubAction(AMActionSupport::buildControlMoveAction(sideValves_, BioXASValvesControl::Open));
	actionsList->addSubAction(AMActionSupport::buildControlMoveAction(mainValves_, BioXASValvesControl::Open));
	actionsList->addSubAction(AMActionSupport::buildControlMoveAction(imagingValves_, BioXASValvesControl::Open));

	return actionsList;
}
