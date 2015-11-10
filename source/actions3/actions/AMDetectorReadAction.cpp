/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMDetectorReadAction.h"

//#include "beamline/AMBeamline.h"
#include "beamline/AMBeamlineSupport.h"
#include "util/AMErrorMonitor.h"
#include "acquaman/AMAgnosticDataAPI.h"

AMDetectorReadAction::~AMDetectorReadAction(){}

AMDetectorReadAction::AMDetectorReadAction(AMDetectorReadActionInfo *info, AMDetector *detector, QObject *parent) :
	AMAction3(info, parent)
{
	if(detector)
		detector_ = detector;
	else if(AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(info->detectorInfo()));
	else
		detector_ = 0; //NULL
}

AMDetectorReadAction::AMDetectorReadAction(const AMDetectorReadAction &other) :
	AMAction3(other)
{
	const AMDetectorReadActionInfo *info = qobject_cast<const AMDetectorReadActionInfo*>(other.info());

	if(info && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(detectorReadInfo()->detectorInfo()));
	else
		detector_ = 0;
}

void AMDetectorReadAction::startImplementation(){
	// If you still don't have a detector, check the exposed detectors one last time.
	if(!detector_ && AMBeamlineSupport::beamlineDetectorAPI())
		detector_ = AMBeamlineSupport::beamlineDetectorAPI()->exposedDetectorByInfo(*(detectorReadInfo()->detectorInfo()));

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

#include <QDebug>
void AMDetectorReadAction::internalSetSucceeded(){
	disconnect(detector_, 0, this, 0);

	if(generateScanActionMessages_){

		if(AMAgnosticDataAPISupport::handlerFromLookupKey("AmptekTest")){
			QList<double> detectorData;
			detectorData.append(27);
			QList<int> dimensionSizes;
			dimensionSizes.append(1);
			QStringList dimensionNames;
			dimensionNames.append("Anything");
			QStringList dimensionUnits;
			dimensionUnits.append("Whatever");
			AMAgnosticDataAPIDataAvailableMessage dataAvailableMessage(detector_->name(), detectorData, dimensionSizes, dimensionNames, dimensionUnits, true);
			intptr_t continuousDataPointer = (intptr_t)(detector_->lastContinuousData(1));
			quint64 continuousDataPointer64 = continuousDataPointer;
			qDebug() << "Pointer value was " << continuousDataPointer << " 64 as " << continuousDataPointer64;
//			dataAvailableMessage.setDetectorDataAsAMDS(detector_->lastContinuousData(1));
			dataAvailableMessage.setDetectorDataAsAMDS(continuousDataPointer64);
//			qDebug() << "Placing pointer with value " << (intptr_t)(dataAvailableMessage.detectorDataAsAMDS());
			AMAgnosticDataAPISupport::handlerFromLookupKey("AmptekTest")->postMessage(dataAvailableMessage);

			qDebug() << "About to postMessage to AmptekTest";
			return;
		}

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
//		else
//			detectorDataPointerSize = detector_->lastContinuousSize();
		QVector<double> detectorDataPointer = QVector<double>(detectorDataPointerSize);
		detector_->data(detectorDataPointer.data());

		if(detector_->rank() == 0 && detector_->readMode() == AMDetectorDefinitions::SingleRead)
			detectorData.append(detectorDataPointer[0]);
//		else if(detector_->rank() == 0 && detector_->readMode() == AMDetectorDefinitions::ContinuousRead){
//			int totalPoints = detector_->lastContinuousSize();
//			if(totalPoints < 0)
//				totalPoints = 0;
//			for(int x = 0; x < totalPoints; x++)
//				detectorData.append(detectorDataPointer[x]);
//		}
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
