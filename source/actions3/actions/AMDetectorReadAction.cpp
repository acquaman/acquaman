#include "AMDetectorReadAction.h"

#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"
#include "acquaman/AMAgnosticDataAPI.h"

AMDetectorReadAction::~AMDetectorReadAction(){}

AMDetectorReadAction::AMDetectorReadAction(AMDetectorReadActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	if(detector)
		detector_ = detector;
	else
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
}

AMDetectorReadAction::AMDetectorReadAction(const AMDetectorReadAction &other) :
	AMAction3(other)
{
	const AMDetectorReadActionInfo *info = qobject_cast<const AMDetectorReadActionInfo*>(other.info());

	if(info)
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(info->detectorInfo()));
	else
		detector_ = 0;
}

void AMDetectorReadAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	if(!detector_)
		detector_ = AMBeamline::bl()->exposedDetectorByInfo(*(detectorReadInfo()->detectorInfo()));

	if(!detector_) {
		AMErrorMon::alert(this,
						  AMDETECTORREADACTION_NO_VALID_DETECTOR,
						  QString("There was an error reading the detector '%1', because the detector was not found. Please report this problem to the Acquaman developers.").arg(detectorReadInfo()->name()));
		setFailed();
		return;
	}

	if(detector_->readMethod() == AMDetectorDefinitions::WaitRead){
		// connect to detector initialization signals
		connect(detector_, SIGNAL(newValuesAvailable()), this, SLOT(onDetectorNewValuesAvailable()));
	}
	else
		internalSetSucceeded();
}

void AMDetectorReadAction::cancelImplementation(){
	if(!detector_){
		setCancelled();
		return;
	}

	// FIGURE OUT WHAT TO DO HERE
	setCancelled();
}

void AMDetectorReadAction::onDetectorNewValuesAvailable(){
	internalSetSucceeded();
}

void AMDetectorReadAction::internalSetSucceeded(){
	disconnect(detector_, 0, this, 0);

	if(generateScanActionMessages_){

		QList<int> dimensionSizes;
		QStringList dimensionNames;
		QStringList dimensionUnits;
		QList<AMAxisInfo> axes = detector_->axes();

		for(int x = 0; x < axes.count(); x++){
			dimensionSizes.append(axes.at(x).size);
			dimensionNames.append(axes.at(x).name);
			dimensionUnits.append(axes.at(x).units);
		}

		QList<double> detectorData;
		int detectorDataPointerSize;
		if(detector_->readMode() == AMDetectorDefinitions::SingleRead)
			detectorDataPointerSize = detector_->size().product();
		else
			detectorDataPointerSize = detector_->lastContinuousSize();
		QVector<double> detectorDataPointer = QVector<double>(detectorDataPointerSize);
		detector_->data(detectorDataPointer.data());

		if(detector_->rank() == 0 && detector_->readMode() == AMDetectorDefinitions::SingleRead)
			detectorData.append(detectorDataPointer[0]);
		else if(detector_->rank() == 0 && detector_->readMode() == AMDetectorDefinitions::ContinuousRead){
			int totalPoints = detector_->lastContinuousSize();
			if(totalPoints < 0)
				totalPoints = 0;
			for(int x = 0; x < totalPoints; x++)
				detectorData.append(detectorDataPointer[x]);
		}
		else{
			int totalPoints = AMnDIndex(detector_->rank(), AMnDIndex::DoInit, 0).totalPointsTo(detector_->size())-1;
			for(int x = 0; x < totalPoints; x++)
				detectorData.append(detectorDataPointer[x]);
		}

		AMAgnosticDataAPIDataAvailableMessage dataAvailableMessage(detector_->name(), detectorData, dimensionSizes, dimensionNames, dimensionUnits);
		AMAgnosticDataAPISupport::handlerFromLookupKey("ScanActions")->postMessage(dataAvailableMessage);
	}

	setSucceeded();
}
