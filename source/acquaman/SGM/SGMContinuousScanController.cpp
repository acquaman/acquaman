#include "SGMContinuousScanController.h"

#include "beamline/CLS/CLSAMDSScalerChannelDetector.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "source/DataHolder/AMDSGenericFlatArrayDataHolder.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/DataHolder/AMDSScalarDataHolder.h"
#include "beamline/AMBeamline.h"
SGMContinuousScanController::SGMContinuousScanController(AMGenericContinuousScanConfiguration *configuration,
                                                         QObject *parent) :
    AMGenericContinuousScanController(configuration, parent)
{

}

bool SGMContinuousScanController::generateAnalysisMetaInfo()
{
	CLSAMDSScalerChannelDetector *tempScalerChannelDetector = 0;
	CLSAmptekSDD123DetectorNew *tempAmptekDetector = 0;

	for(int x = 0, size = generalConfig_->detectorConfigurations().count(); x < size; x++){
		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(x));

		if(!oneDetector->amdsBufferName().isEmpty() && !requiredBufferNames_.contains(oneDetector->amdsBufferName()))
			requiredBufferNames_.append(oneDetector->amdsBufferName());

		tempScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
		if(tempScalerChannelDetector){
			timeScales_.append(tempScalerChannelDetector->amdsPollingBaseTimeMilliseconds());
			scalerChannelDetectors_.append(tempScalerChannelDetector);
		}

		tempAmptekDetector = qobject_cast<CLSAmptekSDD123DetectorNew*>(oneDetector);
		if(tempAmptekDetector){
			timeScales_.append(tempAmptekDetector->amdsPollingBaseTimeMilliseconds());
			amptekDetectors_.append(tempAmptekDetector);
		}
	}

	// Always add a sensible value of 25ms ... any more time resolution and even quick scans (~10s) are too many points
	timeScales_.append(25);

	for(int x = 0, size = requiredBufferNames_.count(); x < size; x++){
		if(!clientDataRequestMap_.contains(requiredBufferNames_.at(x))){
			AMErrorMon::alert(this, SGMCONTINUOUSSCANCONTROLLER_REQUIRED_DATA_MISSING, QString("Missing data %1").arg(requiredBufferNames_.at(x)));
			return false;
		}
	}

	return true;
}

bool SGMContinuousScanController::findStartMotionIndices()
{
	expectedDurationScaledToBaseTimeScale_ = double(continuousConfiguration_->scanAxes().at(0)->regionAt(0)->regionTime())*1000/largestBaseTimeScale_;

	// The hardcoded number is the empirical energy encoder count threshold.
	// This will be generalized at a later date.
	AMDetectorContinuousMotionRangeData amptekRangeData;
	if(!amptekDetectors_.isEmpty()){
		CLSAmptekSDD123DetectorNew *highestAverageAmptekDetector = 0;
		QMap<CLSAmptekSDD123DetectorNew*, CLSAmptekSDD123DetectorGeneralPurposeCounterData> generalPurposeCounters;
		foreach(CLSAmptekSDD123DetectorNew* amptekDetector, amptekDetectors_){
			generalPurposeCounters.insert(amptekDetector, amptekDetector->retrieveGeneralPurposeCounterData(clientDataRequestMap_.value(amptekDetector->amdsBufferName())));
			if(!highestAverageAmptekDetector || (generalPurposeCounters.value(amptekDetector).averageValue() > generalPurposeCounters.value(highestAverageAmptekDetector).averageValue()) )
				highestAverageAmptekDetector = amptekDetector;
		}

		QList<QVector<qint32> > amptekBaseData;
		amptekBaseData << generalPurposeCounters.value(highestAverageAmptekDetector).generalPurposeCounterVector();
		amptekRangeData = highestAverageAmptekDetector->retrieveContinuousMotionRangeData(amptekBaseData, expectedDurationScaledToBaseTimeScale_, 20);
	}


	int scalerInitiateMovementIndex = 0;
	QList<QVector<qint32> > scalerBaseData;
	scalerBaseData << scalerChannelRebaseVectors_.value("EncoderUp") << scalerChannelRebaseVectors_.value("EncoderDown");
	AMDetectorContinuousMotionRangeData scalerRangeData = scalerChannelDetectors_.first()->retrieveContinuousMotionRangeData(scalerBaseData, expectedDurationScaledToBaseTimeScale_, 20);
	if(scalerRangeData.isValid()){
		scalerInitiateMovementIndex = scalerRangeData.motionStartIndex();
	}


	// Not general yet ... need to pass in which channel we should look in for scaler movement data
	encoderUpVector_ = scalerChannelRebaseVectors_.value("EncoderUp");
	encoderDownVector_ = scalerChannelRebaseVectors_.value("EncoderDown");

	if( (encoderUpVector_.count()-scalerInitiateMovementIndex) < expectedDurationScaledToBaseTimeScale_){
		qDebug() << "Scaler start movment index was out of bounds at " << scalerInitiateMovementIndex;
		scalerInitiateMovementIndex += ((encoderUpVector_.count()-scalerInitiateMovementIndex)-expectedDurationScaledToBaseTimeScale_);
		scalerRangeData.setMotionStartIndex(scalerInitiateMovementIndex);
		qDebug() << "Changed to " << scalerInitiateMovementIndex;
	}
	if(!amptekDetectors_.isEmpty()){
		AMDSClientDataRequest *oneAmptekRequest = clientDataRequestMap_.value(amptekDetectors_.first()->amdsBufferName());
		if( (oneAmptekRequest->data().count() - amptekRangeData.motionStartIndex()) < expectedDurationScaledToBaseTimeScale_){
			qDebug() << "Amptek start movment index was out of bounds at " << amptekRangeData.motionStartIndex();
			amptekRangeData.setMotionStartIndex(oneAmptekRequest->data().count() - expectedDurationScaledToBaseTimeScale_);
			qDebug() << "Changed to " << amptekRangeData.motionStartIndex();
		}
	}

	foreach(CLSAMDSScalerChannelDetector *scalerChannelDetector, scalerChannelDetectors_)
		detectorStartMotionIndexMap_.insert(scalerChannelDetector->name(), scalerRangeData.motionStartIndex());
	if(!amptekDetectors_.isEmpty()){
		foreach(CLSAmptekSDD123DetectorNew* amptekDetector, amptekDetectors_){
			detectorStartMotionIndexMap_.insert(amptekDetector->name(), amptekRangeData.motionStartIndex());
		}
	}

	qDebug() << "Amptek: " << amptekRangeData.motionStartIndex() << amptekRangeData.motionEndIndex() << amptekRangeData.listIndex();
	qDebug() << "Scaler: " << scalerRangeData.motionStartIndex() << scalerRangeData.motionEndIndex() << scalerRangeData.listIndex();

	scalerInitiateMovementIndex_ = detectorStartMotionIndexMap_.value(scalerChannelDetectors_.first()->name());

	return true;
}

bool SGMContinuousScanController::generateAxisFeedbackValues()
{
	return false;
}

bool SGMContinuousScanController::generateScalerMaps()
{
	AMDSClientDataRequest *scalerClientDataRequest = clientDataRequestMap_.value("Scaler (BL1611-ID-1)");
	if(!scalerClientDataRequest){
		AMErrorMon::alert(this, SGMCONTINUOUSSCANCONTROLLER_REQUIRED_DATA_MISSING, QString("Missing scaler data for continuous scan processing."));
		return false;
	}

	scalerTotalCount_ = scalerClientDataRequest->data().count();

	// Set up maps
	foreach(CLSAMDSScalerChannelDetector *scalerChannelDetector, scalerChannelDetectors_)
		scalerChannelIndexMap_.insert(scalerChannelDetector->enabledChannelIndex(), scalerChannelDetector->name());

	// Check data holder casts to correct type
	qDebug() << "Total scaler data holders" << scalerTotalCount_ << ((AMDSClientRelativeCountPlusCountDataRequest*)scalerClientDataRequest)->relativeCount() << ((AMDSClientRelativeCountPlusCountDataRequest*)scalerClientDataRequest)->count();
	AMDSLightWeightScalarDataHolder *asScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(scalerClientDataRequest->data().at(0));
	if(!asScalarDataHolder){
		AMErrorMon::alert(this, SGMCONTINUOUSSCANCONTROLLER_BAD_SCALER_DATAHOLDER_TYPE, QString("Scaler data holder is the wrong type."));
		return false;
	}
	// Check that we won't index into more positions than the AMDS-Scaler returned
	if(scalerChannelIndexMap_.count() > asScalarDataHolder->dataArray().constVectorQint32().count()){
		AMErrorMon::alert(this, SGMCONTINUOUSSCANCONTROLLER_SCALER_CHANNEL_MISMATCH, QString("There is a mismatch between the number of enabled scaler channels and the number requested."));
		return false;
	}

	return true;
}

bool SGMContinuousScanController::generateInterpolatedParameters()
{
	double startAxisValue = axisFeedbackValues_.first();
	double endAxisVaue = axisFeedbackValues_.last();
	if(startAxisValue > endAxisVaue)
		qSwap(startAxisValue, endAxisVaue);

	qDebug() << "Start energy was " << startAxisValue << " end energy was " << endAxisVaue;
	startAxisValue = ceilf(startAxisValue / resolutionStep_)  * resolutionStep_;
	endAxisVaue = floorf(endAxisVaue / resolutionStep_) * resolutionStep_;
	qDebug() << "Start energy is " << startAxisValue << " end energy is " << endAxisVaue;

	interpolatedSize_ = (endAxisVaue-startAxisValue)/resolutionStep_;
	qDebug() << "interpolatedSize is " << interpolatedSize_;

	// Create the interpolated axis and midpoints
	double currentAxisValue = startAxisValue;
	interpolatedAxis_ = QVector<double>(interpolatedSize_);
	interpolatedMidpoints_ = QVector<double>(interpolatedSize_);
	double halfResolution = resolutionStep_/2;

	for(int x = 0, size = interpolatedAxis_.count(); x < size; x++){
		interpolatedAxis_[x] = currentAxisValue;
		interpolatedMidpoints_[x] = currentAxisValue+halfResolution;
		currentAxisValue += resolutionStep_;
	}

	isUpScan_ = (axisFeedbackValues_.first() < axisFeedbackValues_.last());

	// Find the index mapping between interpolated points and feedback points
	interpolatedMidpointsMappingIndices_ = QVector<double>(interpolatedSize_);
	int currentInOrderEnergyLookupIndex = 0;
	if(!isUpScan_)
		currentInOrderEnergyLookupIndex = axisFeedbackValues_.count()-1;

	double currentInOrderEnergyValue = axisFeedbackValues_.at(currentInOrderEnergyLookupIndex);
	double lastInOrderEnergyValue = currentInOrderEnergyValue;
	double fractionalIndex = 0;
	double lastFractionalIndex = 0;

	if(isUpScan_){
		for(int x = 0, size = interpolatedSize_; x < size; x++){
			double oneEnergyMidpoint = interpolatedMidpoints_.at(x);

			while(oneEnergyMidpoint > currentInOrderEnergyValue){
				currentInOrderEnergyLookupIndex++;
				lastInOrderEnergyValue = currentInOrderEnergyValue;
				currentInOrderEnergyValue = axisFeedbackValues_.at(currentInOrderEnergyLookupIndex);
			}

			lastFractionalIndex = fractionalIndex;
			fractionalIndex = (currentInOrderEnergyLookupIndex-1) + ((oneEnergyMidpoint-lastInOrderEnergyValue)/(currentInOrderEnergyValue-lastInOrderEnergyValue));
			interpolatedMidpointsMappingIndices_[x] = fractionalIndex;
		}
	}
	else{
		for(int x = 0, size = interpolatedSize_; x < size; x++){
			double oneEnergyMidpoint = interpolatedMidpoints_.at(x);

			while(oneEnergyMidpoint > currentInOrderEnergyValue){
				currentInOrderEnergyLookupIndex--;
				lastInOrderEnergyValue = currentInOrderEnergyValue;
				currentInOrderEnergyValue = axisFeedbackValues_.at(currentInOrderEnergyLookupIndex);
			}

			lastFractionalIndex = fractionalIndex;
			fractionalIndex = (currentInOrderEnergyLookupIndex+1) - ((oneEnergyMidpoint-lastInOrderEnergyValue)/(currentInOrderEnergyValue-lastInOrderEnergyValue));
			interpolatedMidpointsMappingIndices_[x] = fractionalIndex;
		}
	}

	return true;
}

bool SGMContinuousScanController::generateInterpolatedScalerVectors()
{
	foreach(CLSAMDSScalerChannelDetector *scalerChannelDetector, scalerChannelDetectors_)
		interpolatedScalerChannelVectors_.insert(scalerChannelDetector->name(), QVector<double>(interpolatedSize_));

	// Loop over scaler interpolated vectors and fill them
	QMap<QString, QVector<double> >::iterator interpolatedScalerIterator = interpolatedScalerChannelVectors_.begin();
	while(interpolatedScalerIterator != interpolatedScalerChannelVectors_.end()){
		QVector<qint32> oneOriginalVector = scalerChannelRebaseVectors_.value(interpolatedScalerIterator.key());

		double startFractionalIndex;
		double endFractionIndex;

		if(isUpScan_){
			int startFloorIndex;
			int endFloorIndex;
			for(int x = 0, size = interpolatedScalerIterator.value().count(); x < size; x++){
				if(x == 0)
					startFractionalIndex = 0;
				else
					startFractionalIndex = interpolatedMidpointsMappingIndices_.at(x-1);

				if(x == interpolatedScalerIterator.value().count()-1)
					endFractionIndex = ((oneOriginalVector.count()-1)-scalerInitiateMovementIndex_)+0.999999;
				else
					endFractionIndex = interpolatedMidpointsMappingIndices_.at(x);


				startFloorIndex = floor(startFractionalIndex);
				endFloorIndex = floor(endFractionIndex);


				// Both fractions within one index, use a subfractional amount
				if(startFloorIndex == endFloorIndex){
					interpolatedScalerIterator.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex_))*(endFractionIndex-startFractionalIndex);
				} // The fractions are in adjacent indices, so use a fraction of each
				else if( (endFloorIndex-startFloorIndex) == 1){
					interpolatedScalerIterator.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex_))*(double(startFloorIndex+1)-startFractionalIndex);
					interpolatedScalerIterator.value()[x] += double(oneOriginalVector.at(endFloorIndex+scalerInitiateMovementIndex_))*(endFractionIndex-double(endFloorIndex));
				} // The fractions are separate by several indices, so use a fraction of the first and last and all of the ones in between
				else{
					interpolatedScalerIterator.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex_))*(double(startFloorIndex+1)-startFractionalIndex);
					for(int y = startFloorIndex+1; y < endFloorIndex; y++)
						interpolatedScalerIterator.value()[x] += oneOriginalVector.at(y+scalerInitiateMovementIndex_);
					interpolatedScalerIterator.value()[x] += double(oneOriginalVector.at(endFloorIndex+scalerInitiateMovementIndex_))*(endFractionIndex-double(endFloorIndex));

				}
			}
		}
		else{
			int startCeilIndex;
			int endCeilIndex;
			for(int x = 0, size = interpolatedScalerIterator.value().count(); x < size; x++){
				if(x == 0)
					startFractionalIndex = ((oneOriginalVector.count()-1)-scalerInitiateMovementIndex_)-0.000001;
				else
					startFractionalIndex = interpolatedMidpointsMappingIndices_.at(x-1);

				if(x == interpolatedScalerIterator.value().count()-1)
					endFractionIndex = 0;
				else
					endFractionIndex = interpolatedMidpointsMappingIndices_.at(x);


				startCeilIndex = ceil(startFractionalIndex);
				endCeilIndex = ceil(endFractionIndex);

				// Both fractions within one index, use a subfractional amount
				if(startCeilIndex == endCeilIndex){
					interpolatedScalerIterator.value()[x] = double(oneOriginalVector.at(startCeilIndex-1+scalerInitiateMovementIndex_))*(startFractionalIndex-endFractionIndex);
				} // The fractions are in adjacent indices, so use a fraction of each
				else if( (startCeilIndex-endCeilIndex) == 1){
					interpolatedScalerIterator.value()[x] = double(oneOriginalVector.at(startCeilIndex-1+scalerInitiateMovementIndex_))*(startFractionalIndex-double(startCeilIndex-1));
					interpolatedScalerIterator.value()[x] += double(oneOriginalVector.at(endCeilIndex-1+scalerInitiateMovementIndex_))*(double(endCeilIndex)-endFractionIndex);
				} // The fractions are separate by several indices, so use a fraction of the first and last and all of the ones in between
				else{
					interpolatedScalerIterator.value()[x] = double(oneOriginalVector.at(startCeilIndex-1+scalerInitiateMovementIndex_))*(startFractionalIndex-double(startCeilIndex-1));
					for(int y = startCeilIndex-1; y > endCeilIndex; y--)
						interpolatedScalerIterator.value()[x] += oneOriginalVector.at(y-1+scalerInitiateMovementIndex_);
					interpolatedScalerIterator.value()[x] += double(oneOriginalVector.at(endCeilIndex-1+scalerInitiateMovementIndex_))*(double(endCeilIndex)-endFractionIndex);
				}
			}
		}
		interpolatedScalerIterator++;
	}


	// Check the percent difference (conservation of total count) for scalers
	QMap<QString, QVector<double> >::const_iterator percentDifferenceIterator = interpolatedScalerChannelVectors_.constBegin();
	while(percentDifferenceIterator != interpolatedScalerChannelVectors_.constEnd()){
		QVector<qint32> rebaseOfInterest = scalerChannelRebaseVectors_.value(percentDifferenceIterator.key()).mid(scalerInitiateMovementIndex_);
		QVector<double> interpolatedOfInterest = percentDifferenceIterator.value();

		double rebaseSum = 0;
		for(int x = 0, size = rebaseOfInterest.count(); x < size; x++)
			rebaseSum += rebaseOfInterest.at(x);
		double interpolatedSum = 0;
		for(int x = 0, size = interpolatedOfInterest.count(); x < size; x++)
			interpolatedSum += interpolatedOfInterest.at(x);

		double percentDifference;
		if( (rebaseSum < 0.00001) && (interpolatedSum < 0.00001) )
			percentDifference = 0;
		else
			percentDifference = 100*(fabs(rebaseSum-interpolatedSum))/qMax(rebaseSum, interpolatedSum);

		if(percentDifference >= 1.00)
			qDebug() << QString("For %1, rebase sum is %2 and interpolated sum is %3 with percent difference %4").arg(percentDifferenceIterator.key()).arg(rebaseSum).arg(interpolatedSum).arg(percentDifference);
		percentDifferenceIterator++;
	}

	return true;
}

bool SGMContinuousScanController::generateInterpolatedAmptekVectors()
{
	AMDSClientDataRequest *oneAmptekRequest = clientDataRequestMap_.value(amptekDetectors_.first()->amdsBufferName());
	// Actually create the interpolated amptek vectors
	int amptekSize = amptekDetectors_.first()->size().product();
	QMap<QString, QVector<double> > originalAmptekVectors;
	foreach(CLSAmptekSDD123DetectorNew *amptekDetector, amptekDetectors_){
		originalAmptekVectors.insert(amptekDetector->name(), QVector<double>(oneAmptekRequest->data().count()*amptekSize));
		interpolatedAmptekVectors_.insert(amptekDetector->name(), QVector<double>(interpolatedSize_*amptekSize));
	}

	AMDSLightWeightGenericFlatArrayDataHolder *dataHolderAsGenericFlatArrayDataHolder = 0;
	foreach(CLSAmptekSDD123DetectorNew *amptekDetector, amptekDetectors_){
		oneAmptekRequest = clientDataRequestMap_.value(amptekDetector->amdsBufferName());
		dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(oneAmptekRequest->data().at(0));
		if(dataHolderAsGenericFlatArrayDataHolder){
			double *amptekDataArray = originalAmptekVectors[amptekDetector->name()].data();
			for(int x = 0, size = oneAmptekRequest->data().count(); x < size; x++){
				dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(oneAmptekRequest->data().at(x));
				memcpy(amptekDataArray+x*amptekSize, dataHolderAsGenericFlatArrayDataHolder->dataArray().asConstVectorDouble().constData(), amptekSize*sizeof(double));
			}
		}
	}

	int amptekInitiateMovementIndex = detectorStartMotionIndexMap_.value(amptekDetectors_.first()->name());
	if(amptekInitiateMovementIndex == -1)
		amptekInitiateMovementIndex = scalerInitiateMovementIndex_;
	// Loop over amptek interpolated vectors and fill them
	QMap<QString, QVector<double> >::iterator amptekInterpolationIterator = interpolatedAmptekVectors_.begin();
	while(amptekInterpolationIterator != interpolatedAmptekVectors_.end()){
		QVector<double> oneOriginalVector = originalAmptekVectors.value(amptekInterpolationIterator.key());

		double startFractionalIndex;
		double endFractionIndex;

		if(isUpScan_){
			int startFloorIndex;
			int endFloorIndex;

			for(int x = 0, size = amptekInterpolationIterator.value().count()/amptekSize; x < size; x++){
				if(x == 0)
					startFractionalIndex = 0;
				else
					startFractionalIndex = interpolatedMidpointsMappingIndices_.at(x-1);

				if(x == (amptekInterpolationIterator.value().count()/amptekSize)-1)
					endFractionIndex = ((oneOriginalVector.count()/amptekSize-1)-amptekInitiateMovementIndex)+0.999999;
				else
					endFractionIndex = interpolatedMidpointsMappingIndices_.at(x);

				startFloorIndex = floor(startFractionalIndex);
				endFloorIndex = floor(endFractionIndex);

				// Both fractions within one index, use a subfractional amount
				if(startFloorIndex == endFloorIndex){
					for(int z = 0; z < amptekSize; z++)
						amptekInterpolationIterator.value()[x*amptekSize+z] = oneOriginalVector.at( (startFloorIndex+amptekInitiateMovementIndex)*amptekSize + z )*(endFractionIndex-startFractionalIndex);
				} // The fractions are in adjacent indices, so use a fraction of each
				else if( (endFloorIndex-startFloorIndex) == 1){
					for(int z = 0; z < amptekSize; z++){
						amptekInterpolationIterator.value()[x*amptekSize+z] = oneOriginalVector.at( (startFloorIndex+amptekInitiateMovementIndex)*amptekSize + z )*(double(startFloorIndex+1)-startFractionalIndex);
						amptekInterpolationIterator.value()[x*amptekSize+z] += oneOriginalVector.at( (endFloorIndex+amptekInitiateMovementIndex)*amptekSize + z )*(endFractionIndex-double(endFloorIndex));
					}
				} // The fractions are separate by several indices, so use a fraction of the first and last and all of the ones in between
				else{
					for(int z = 0; z < amptekSize; z++){
						amptekInterpolationIterator.value()[x*amptekSize+z] = oneOriginalVector.at( (startFloorIndex+amptekInitiateMovementIndex)*amptekSize + z )*(double(startFloorIndex+1)-startFractionalIndex);
						for(int y = startFloorIndex+1; y < endFloorIndex; y++)
							amptekInterpolationIterator.value()[x*amptekSize+z] += oneOriginalVector.at( (y+amptekInitiateMovementIndex)*amptekSize + z );
						amptekInterpolationIterator.value()[x*amptekSize+z] += oneOriginalVector.at( (endFloorIndex+amptekInitiateMovementIndex)*amptekSize + z )*(endFractionIndex-double(endFloorIndex));
					}
				}
			}
		}
		else{
			int startCeilIndex;
			int endCeilIndex;

			for(int x = 0, size = amptekInterpolationIterator.value().count()/amptekSize; x < size; x++){
				if(x == 0)
					startFractionalIndex = ((oneOriginalVector.count()/amptekSize-1)-amptekInitiateMovementIndex)-0.000001;
				else
					startFractionalIndex = interpolatedMidpointsMappingIndices_.at(x-1);

				if(x == (amptekInterpolationIterator.value().count()/amptekSize)-1)
					endFractionIndex = 0;
				else
					endFractionIndex = interpolatedMidpointsMappingIndices_.at(x);

				startCeilIndex = ceil(startFractionalIndex);
				endCeilIndex = ceil(endFractionIndex);

				// Both fractions within one index, use a subfractional amount
				if(startCeilIndex == endCeilIndex){
					for(int z = 0; z < amptekSize; z++)
						amptekInterpolationIterator.value()[x*amptekSize+z] = oneOriginalVector.at( (startCeilIndex-1+amptekInitiateMovementIndex)*amptekSize + z )*(startFractionalIndex-endFractionIndex);
				} // The fractions are in adjacent indices, so use a fraction of each
				else if( (startCeilIndex-endCeilIndex) == 1){
					for(int z = 0; z < amptekSize; z++){
						amptekInterpolationIterator.value()[x*amptekSize+z] = oneOriginalVector.at( (startCeilIndex-1+amptekInitiateMovementIndex)*amptekSize + z )*(startFractionalIndex-double(startCeilIndex-1));
						amptekInterpolationIterator.value()[x*amptekSize+z] += oneOriginalVector.at( (endCeilIndex-1+amptekInitiateMovementIndex)*amptekSize + z )*(double(endCeilIndex)-endFractionIndex);
					}
				} // The fractions are separate by several indices, so use a fraction of the first and last and all of the ones in between
				else{
					for(int z = 0; z < amptekSize; z++){
						amptekInterpolationIterator.value()[x*amptekSize+z] = oneOriginalVector.at( (startCeilIndex-1+amptekInitiateMovementIndex)*amptekSize + z )*(startFractionalIndex-double(startCeilIndex-1));
						for(int y = startCeilIndex-1; y > endCeilIndex; y--)
							amptekInterpolationIterator.value()[x*amptekSize+z] += oneOriginalVector.at( (y-1+amptekInitiateMovementIndex)*amptekSize + z );
						amptekInterpolationIterator.value()[x*amptekSize+z] += oneOriginalVector.at( (endCeilIndex-1+amptekInitiateMovementIndex)*amptekSize + z )*(double(endCeilIndex)-endFractionIndex);
					}
				}
			}
		}
		amptekInterpolationIterator++;
	}

	// Check the percent difference (conservation of total count) for ampteks
	QMap<QString, QVector<double> >::const_iterator amptekPercentDifferenceIterator = interpolatedAmptekVectors_.constBegin();
	while(amptekPercentDifferenceIterator != interpolatedAmptekVectors_.constEnd()){
		QVector<double> rebaseOfInterest = originalAmptekVectors.value(amptekPercentDifferenceIterator.key()).mid(amptekInitiateMovementIndex*amptekSize);
		QVector<double> interpolatedOfInterest = amptekPercentDifferenceIterator.value();

		double rebaseSum = 0;
		for(int x = 0, size = rebaseOfInterest.count(); x < size; x++)
			rebaseSum += rebaseOfInterest.at(x);
		double interpolatedSum = 0;
		for(int x = 0, size = interpolatedOfInterest.count(); x < size; x++)
			interpolatedSum += interpolatedOfInterest.at(x);

		double percentDifference;
		if( (rebaseSum < 0.00001) && (interpolatedSum < 0.00001) )
			percentDifference = 0;
		else
			percentDifference = 100*(fabs(rebaseSum-interpolatedSum))/qMax(rebaseSum, interpolatedSum);

		if(percentDifference >= 1.00)
			qDebug() << QString("For %1, rebase sum is %2 and interpolated sum is %3 with percent difference %4").arg(amptekPercentDifferenceIterator.key()).arg(rebaseSum).arg(interpolatedSum).arg(percentDifference);
		amptekPercentDifferenceIterator++;
	}

	return true;
}

bool SGMContinuousScanController::placeInterpolatedDataInDataStore()
{
	CLSAMDSScalerChannelDetector *asScalerChannelDetector = 0;
	for(int x = 0, size = interpolatedAxis_.count(); x < size; x++){
		scan_->rawData()->beginInsertRows(1, -1);
		scan_->rawData()->setAxisValue(0, insertionIndex_.i(), interpolatedAxis_.at(x));

		for(int y = 0, ySize = generalConfig_->detectorConfigurations().count(); y < ySize; y++){
			AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(y));

			asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
			if(!asScalerChannelDetector){
				const double *detectorData = interpolatedAmptekVectors_.value(oneDetector->name()).mid(insertionIndex_.i()*oneDetector->size().product(), oneDetector->size().product()).constData();
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(oneDetector->name()), detectorData);
			}
		}

		QMap<int, QString>::const_iterator i = scalerChannelIndexMap_.constBegin();
		while(i != scalerChannelIndexMap_.constEnd()){
			scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(i.value()), AMnDIndex(), interpolatedScalerChannelVectors_.value(i.value()).at(x));
			i++;
		}

		scan_->rawData()->endInsertRows();
		insertionIndex_[0] = insertionIndex_.i()+1;
	}

	return true;
}
