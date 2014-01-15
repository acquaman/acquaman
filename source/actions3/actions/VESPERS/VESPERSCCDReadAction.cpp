#include "VESPERSCCDReadAction.h"

#include "acquaman/AMAgnosticDataAPI.h"

VESPERSCCDReadAction::VESPERSCCDReadAction(AMDetectorReadActionInfo *info, AMDetector *detector, QObject *parent)
	: AMDetectorReadAction(info, detector, parent)
{
}

VESPERSCCDReadAction::VESPERSCCDReadAction(const VESPERSCCDReadAction &other)
	: AMDetectorReadAction(other)
{

}

VESPERSCCDReadAction::~VESPERSCCDReadAction()
{

}

void VESPERSCCDReadAction::internalSetSucceeded()
{
	disconnect(detector_, 0, this, 0);

	if(generateScanActionMessages_){

		QList<int> dimensionSizes;
		QStringList dimensionNames;
		QStringList dimensionUnits;
		QList<double> detectorData;

		VESPERSCCDDetector *detector = qobject_cast<VESPERSCCDDetector*>(detector_);
		detectorData << detector->ccdFileNumber();

		AMAgnosticDataAPIDataAvailableMessage dataAvailableMessage(detector_->name(), detectorData, dimensionSizes, dimensionNames, dimensionUnits);
		AMAgnosticDataAPISupport::handlerFromLookupKey("ScanActions")->postMessage(dataAvailableMessage);
	}

	setSucceeded();
}
