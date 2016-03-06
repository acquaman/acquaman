#include "BioXASZebraModifyDetectorsAction.h"
#include "beamline/BioXAS/BioXASBeamline.h"

BioXASZebraModifyDetectorsAction::BioXASZebraModifyDetectorsAction(BioXASZebraModifyDetectorsActionInfo *info, QObject *parent) :
	AMAction3(info, parent)
{

}

BioXASZebraModifyDetectorsAction::BioXASZebraModifyDetectorsAction(const BioXASZebraModifyDetectorsAction &original) :
	AMAction3(original)
{

}

BioXASZebraModifyDetectorsAction::~BioXASZebraModifyDetectorsAction()
{

}

void BioXASZebraModifyDetectorsAction::startImplementation()
{
	// Must have a valid, connected Zebra.

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if ( !(zebra && zebra->isConnected()) ) {
		QString message = QString("Failed to modify the Zebra detectors. The Zebra provided is invalid.");
		AMErrorMon::alert(this, BIOXASZEBRAMODIFYDETECTORSACTION_INVALID_ZEBRA, message);
		setFailed(message);
		return;
	}

	// Must have a valid action.

	int action = modifyDetectorsInfo()->action();

	if (!validAction(action)) {
		QString message = QString("Failed to modify the Zebra detectors. The modification action provided is invalid/not supported.");
		AMErrorMon::alert(this, BIOXASZEBRAMODIFYDETECTORSACTION_INVALID_ACTION, message);
		setFailed(message);
		return;
	}

	// May need a valid detector.

	AMDetector *detector = 0;

	if (action == BioXASZebraModifyDetectorsActionInfo::AddDetector || action == BioXASZebraModifyDetectorsActionInfo::RemoveDetector) {
		detector = BioXASBeamline::bioXAS()->exposedDetectorByInfo(modifyDetectorsInfo()->detectorInfo());

		if (!detector) {
			QString message = QString("Failed to modify the Zebra detectors. The modification action provided requires a valid detector, and a valid detector was not found.");
			AMErrorMon::alert(this, BIOXASZEBRAMODIFYDETECTORSACTION_INVALID_DETECTOR, message);
			setFailed(message);
			return;
		}
	}

	// Set the action as started.

	setStarted();

	// Perform the detectors modification.

	bool result = false;

	if (action == BioXASZebraModifyDetectorsActionInfo::AddDetector) {
		result = zebra->addDetector(detector);
	} else if (action == BioXASZebraModifyDetectorsActionInfo::RemoveDetector) {
		result = zebra->removeDetector(detector);
	} else if (action == BioXASZebraModifyDetectorsActionInfo::ClearDetectors) {
		result = zebra->clearDetectors();
	}

	// Check whether the modification was successful.

	if (result) {
		setSucceeded();

	} else {
		QString message = QString("Failed to modify the Zebra detectors. Perhaps the Zebra doesn't have a trigger source?");
		AMErrorMon::alert(this, BIOXASZEBRAMODIFYDETECTORSACTION_ACTION_FAILED, message);
		setFailed(message);
	}
}

bool BioXASZebraModifyDetectorsAction::validAction(int action) const
{
	switch (action) {
	case BioXASZebraModifyDetectorsActionInfo::AddDetector:
		return true;
	case BioXASZebraModifyDetectorsActionInfo::RemoveDetector:
		return true;
	case BioXASZebraModifyDetectorsActionInfo::ClearDetectors:
		return true;
	default:
		return false;
	}
}
