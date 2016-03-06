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

bool BioXASZebraModifyDetectorsAction::supportedOption(int option) const
{
	switch (option) {
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

void BioXASZebraModifyDetectorsAction::startImplementation()
{
	// Must have a valid, connected Zebra.

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();

	if ( !(zebra && zebra->isConnected()) ) {
		QString message = QString("Failed to modify the Zebra detectors. The Zebra provided is invalid or not connected.");
		AMErrorMon::alert(this, BIOXASZEBRAMODIFYDETECTORSACTION_INVALID_ZEBRA, message);
		setFailed(message);
		return;
	}

	// Must have a valid option.

	int option = modifyDetectorsInfo()->option();

	if (!supportedOption(option)) {
		QString message = QString("Failed to modify the Zebra detectors. The modification option provided (%1) is invalid/not supported.").arg(option);
		AMErrorMon::alert(this, BIOXASZEBRAMODIFYDETECTORSACTION_INVALID_ACTION, message);
		setFailed(message);
		return;
	}

	// May need a valid detector.

	AMDetector *detector = 0;

	if (option == BioXASZebraModifyDetectorsActionInfo::AddDetector || option == BioXASZebraModifyDetectorsActionInfo::RemoveDetector) {
		detector = BioXASBeamline::bioXAS()->exposedDetectorByInfo(modifyDetectorsInfo()->detectorInfo());

		if ( !(detector && detector->isConnected()) ) {
			QString message = QString("Failed to modify the Zebra detectors. The detector provided is invalid or not connected.");
			AMErrorMon::alert(this, BIOXASZEBRAMODIFYDETECTORSACTION_INVALID_DETECTOR, message);
			setFailed(message);
			return;
		}
	}

	// Set the action as started.

	setStarted();

	// Perform the detectors modification.

	bool result = false;

	if (option == BioXASZebraModifyDetectorsActionInfo::AddDetector) {
		result = zebra->addDetector(detector);
	} else if (option == BioXASZebraModifyDetectorsActionInfo::RemoveDetector) {
		result = zebra->removeDetector(detector);
	} else if (option == BioXASZebraModifyDetectorsActionInfo::ClearDetectors) {
		result = zebra->clearDetectors();
	}

	// Check whether the modification was successful.

	if (result) {
		setSucceeded();

	} else {
		QString message = QString("Failed to modify the Zebra detectors.");
		AMErrorMon::alert(this, BIOXASZEBRAMODIFYDETECTORSACTION_ACTION_FAILED, message);
		setFailed(message);
	}
}
