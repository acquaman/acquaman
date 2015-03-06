#include "BioXASSSRLMonochromatorRegionControl.h"
#include "BioXASSSRLMonochromator.h"
#include <QDebug>

BioXASSSRLMonochromatorRegionControl::BioXASSSRLMonochromatorRegionControl(QObject *parent) :
	AMCompositeControl("RegionControl", "", parent, "BioXAS SSRL Monochromator Region Control")
{
	// Initialize local variables.

	moveInProgress_ = false;

	// Initialize inherited variables.

	setEnumStates(QStringList() << "None" << "A" << "B");
	setMoveEnumStates(QStringList() << "A" << "B");
	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Region Control");
}

BioXASSSRLMonochromatorRegionControl::~BioXASSSRLMonochromatorRegionControl()
{

}

double BioXASSSRLMonochromatorRegionControl::minimumValue() const
{
	return BioXASSSRLMonochromator::Region::None;
}

double BioXASSSRLMonochromatorRegionControl::maximumValue() const
{
	return BioXASSSRLMonochromator::Region::B;
}

bool BioXASSSRLMonochromatorRegionControl::canMeasure() const
{
	if (wasConnected_)
		return true;

	return false;
}

bool BioXASSSRLMonochromatorRegionControl::canMove() const
{
	if (wasConnected_)
		return true;

	return false;
}

void BioXASSSRLMonochromatorRegionControl::onCurrentMoveStepChanged(int stepIndex)
{
	qDebug() << "Executing crystal change step number" << stepIndex;
	emit moveStepChanged(stepDescription(stepIndex), stepInstruction(stepIndex));
}

void BioXASSSRLMonochromatorRegionControl::onRegionChangeStarted()
{
	moveInProgress_ = true;
	emit moveStarted();
}

void BioXASSSRLMonochromatorRegionControl::onRegionChangeCancelled()
{
	moveCleanup(sender());
	emit moveFailed(AMControl::WasStoppedFailure);
}

void BioXASSSRLMonochromatorRegionControl::onRegionChangeFailed()
{
	moveCleanup(sender());
	emit moveFailed(AMControl::OtherFailure);
}

void BioXASSSRLMonochromatorRegionControl::onRegionChangeSucceeded()
{
	moveCleanup(sender());
	emit moveSucceeded();
}

bool BioXASSSRLMonochromatorRegionControl::validRegion(int region)
{
	bool isValid = false;

	if (region == BioXASSSRLMonochromator::Region::A || region == BioXASSSRLMonochromator::Region::B)
		isValid = true;

	return isValid;
}

QString BioXASSSRLMonochromatorRegionControl::stepDescription(int stepIndex)
{
	QString result;

	switch (stepIndex) {
	case CloseSlits:
		result = "Closing mono slits...";
		break;
	case WaitForSlitsClosed:
		result = "Waiting for mono slits to close...";
		break;
	case RemovePaddle:
		result = "Removing mono phosphor paddle.";
		break;
	case WaitForPaddleRemoved:
		result = "Waiting for mono to finish removing paddle...";
		break;
	case WaitForKeyEnabled:
		result = "Waiting for crystal change key to be turned to 'Enabled'...";
		break;
	case MoveBraggIntoPosition:
		result = "Moving bragg motor to crystal change position.";
		break;
	case WaitForBraggInPosition:
		result = "Waiting for bragg motor to move to crystal change position...";
		break;
	case WaitForBrakeDisabled:
		result = "Waiting for brake switch to be flipped to 'Disabled'...";
		break;
	case MoveCrystalChangeIntoPosition:
		result = "Moving the crystal change motor to its limit.";
		break;
	case WaitForCrystalChangeInPosition:
		result = "Waiting for crystal change motor to reach its limit...";
		break;
	case WaitForBrakeEnabled:
		result = "Waiting for brake switch to be flipped to 'Enabled'...";
		break;
	case MoveBraggIntoRegion:
		result = "Moving bragg motor into new region.";
		break;
	case WaitForBraggInRegion:
		result = "Waiting for bragg motor to reach new region...";
		break;
	case WaitForKeyDisabled:
		result = "Waiting for crystal change key to be turned to 'Disabled'...";
		break;
	default:
		result = "";
	}

	return result;
}

QString BioXASSSRLMonochromatorRegionControl::stepInstruction(int stepIndex)
{
	QString result;

	switch (stepIndex) {
	case WaitForKeyEnabled:
		result = "Turn the crystal change key to 'Enabled,' counter-clockwise.";
		break;
	case WaitForBrakeDisabled:
		result = "Flip the brake switch to 'Disabled.' There is a mandatory 30s wait after the switch is flipped before proceeding.";
		break;
	case WaitForBrakeEnabled:
		result = "Flip the brake switch to 'Enabled.' There is a mandatory 30s wait after the switch is flipped before proceeding.";
		break;
	case WaitForKeyDisabled:
		result = "Turn the crystal change key to 'Disabled,' clockwise.";
		break;
	default:
		result = "";
	}

	return result;
}

void BioXASSSRLMonochromatorRegionControl::moveCleanup(QObject *action)
{
	moveInProgress_ = false;

	disconnect( action, 0, this, 0 );
	action->deleteLater();
}
