#include "SGMXASScanController.h"

#include "beamline/SGM/SGMBeamline.h"
#include "beamline/CLS/CLSAMDSScalerChannelDetector.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "beamline/CLS/CLSAMDSScaler.h"

#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/DataHolder/AMDSGenericFlatArrayDataHolder.h"
#include "source/DataHolder/AMDSSpectralDataHolder.h"
#include "source/DataHolder/AMDSScalarDataHolder.h"

SGMXASScanController::SGMXASScanController(SGMXASScanConfiguration *configuration, QObject *parent)
	: AMGenericContinuousScanController(configuration, parent)
{

}

SGMXASScanController::~SGMXASScanController()
{

}

void SGMXASScanController::onAxisFinished()
{
	// STEP 1: Data Checks & Meta Info Collection
	if(!generateAnalysisMetaInfo())
		return;
	// END OF STEP 1

	// STEP 2: Retrieve and remap the scaler data into vectors for each channel
	if(!generateScalerMaps())
		return;

	QMap<QString, QVector<qint32> > scalerChannelVectors = SGMBeamline::sgm()->amdsScaler()->retrieveScalerData(scalerChannelIndexMap_, clientDataRequestMap_.value(SGMBeamline::sgm()->amdsScaler()->amdsBufferName()));
	// END OF STEP 2


	// STEP 3: Rebase
	largestBaseTimeScale_ = 0;
	for(int x = 0, size = timeScales_.count(); x < size; x++)
		if(timeScales_.at(x) > largestBaseTimeScale_)
			largestBaseTimeScale_ = timeScales_.at(x);

	// Rebase the scaler or copy if no change in scale
//	QMap<QString, QVector<qint32> > scalerChannelRebaseVectors_;
	if(scalerChannelDetectors_.first()->amdsPollingBaseTimeMilliseconds() < largestBaseTimeScale_)
		scalerChannelRebaseVectors_ = SGMBeamline::sgm()->amdsScaler()->rebaseScalerData(scalerChannelVectors, largestBaseTimeScale_);
	else{
		QMap<QString, QVector<qint32> >::const_iterator l = scalerChannelVectors.constBegin();
		while(l != scalerChannelVectors.constEnd()){
			scalerChannelRebaseVectors_.insert(l.key(), l.value());
			l++;
		}
	}

	// This should never happen, but what if it did?
	if(!amptekDetectors_.isEmpty() && amptekDetectors_.first()->amdsPollingBaseTimeMilliseconds() < largestBaseTimeScale_){
		// what to do here?
	}
	// END OF STEP 3

	// STEP 4: Find Motion Start Indices
	if(!findStartMotionIndices())
		return;
	// END OF STEP 4


	// STEP 5: Generate the Axis Values "In Order"
	if(!generateAxisFeedbackValues())
		return;
	// END OF STEP 5


	// STEP 6: Interpolation
	// Set up interpolation parameters
	resolutionStep_ = 0.1;
	if(!generateInterpolatedParameters())
		return;

	// Just some debugs
//	qDebug() << "Fractional Indices are: ";
//	qDebug() << interpolatedEnergyMidpointsMappingIndices;

//	qDebug() << QString("Energy Value %1 starts at index %2 with fractional index width %3").arg(interpolatedEnergyAxis.at(0), 0, 'f', 2).arg(0, 2, 'f', 3, ' ').arg(interpolatedEnergyMidpointsMappingIndices.at(0), 0, 'f', 4);
//	for(int x = 1, size = interpolatedEnergyMidpoints.count()-1; x < size; x++){
//		qDebug() << QString("Energy Value %1 starts at index %2 with fractional index width %3").arg(interpolatedEnergyAxis.at(x), 0, 'f', 2).arg(interpolatedEnergyMidpointsMappingIndices.at(x-1), 2, 'f', 3, ' ').arg(interpolatedEnergyMidpointsMappingIndices.at(x)-interpolatedEnergyMidpointsMappingIndices.at(x-1), 0, 'f', 4);
//	}

	// Actually create the interpolated scaler vectors
	if(!generateInterpolatedScalerVectors())
		return;
//	QMap<QString, QVector<double> > interpolatedScalerChannelVectors_;
	/*
	QMap<QString, QVector<qint32> >::const_iterator a = scalerChannelVectors.constBegin();
	while(a != scalerChannelVectors.constEnd()){
		interpolatedScalerChannelVectors_.insert(a.key(), QVector<double>(interpolatedSize_));
		a++;
	}

	// Loop over scaler interpolated vectors and fill them
	QMap<QString, QVector<double> >::iterator b = interpolatedScalerChannelVectors_.begin();
	while(b != interpolatedScalerChannelVectors_.end()){
//		QVector<qint32> oneOriginalVector = scalerChannelVectors.value(b.key());
		QVector<qint32> oneOriginalVector = scalerChannelRebaseVectors_.value(b.key());

		double startFractionalIndex;
		double endFractionIndex;

		if(isUpScan_){
			int startFloorIndex;
			int endFloorIndex;
			for(int x = 0, size = b.value().count(); x < size; x++){
				if(x == 0)
					startFractionalIndex = 0;
				else
					startFractionalIndex = interpolatedMidpointsMappingIndices_.at(x-1);

				if(x == b.value().count()-1)
					endFractionIndex = ((oneOriginalVector.count()-1)-scalerInitiateMovementIndex_)+0.999999;
				else
					endFractionIndex = interpolatedMidpointsMappingIndices_.at(x);


				startFloorIndex = floor(startFractionalIndex);
				endFloorIndex = floor(endFractionIndex);


				// Both fractions within one index, use a subfractional amount
				if(startFloorIndex == endFloorIndex){
					b.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex_))*(endFractionIndex-startFractionalIndex);
				} // The fractions are in adjacent indices, so use a fraction of each
				else if( (endFloorIndex-startFloorIndex) == 1){
					b.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex_))*(double(startFloorIndex+1)-startFractionalIndex);
					b.value()[x] += double(oneOriginalVector.at(endFloorIndex+scalerInitiateMovementIndex_))*(endFractionIndex-double(endFloorIndex));
				} // The fractions are separate by several indices, so use a fraction of the first and last and all of the ones in between
				else{
					b.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex_))*(double(startFloorIndex+1)-startFractionalIndex);
					for(int y = startFloorIndex+1; y < endFloorIndex; y++)
						b.value()[x] += oneOriginalVector.at(y+scalerInitiateMovementIndex_);
					b.value()[x] += double(oneOriginalVector.at(endFloorIndex+scalerInitiateMovementIndex_))*(endFractionIndex-double(endFloorIndex));

				}
			}
		}
		else{
			int startCeilIndex;
			int endCeilIndex;
			for(int x = 0, size = b.value().count(); x < size; x++){
				if(x == 0)
					startFractionalIndex = ((oneOriginalVector.count()-1)-scalerInitiateMovementIndex_)-0.000001;
				else
					startFractionalIndex = interpolatedMidpointsMappingIndices_.at(x-1);

				if(x == b.value().count()-1)
					endFractionIndex = 0;
				else
					endFractionIndex = interpolatedMidpointsMappingIndices_.at(x);


				startCeilIndex = ceil(startFractionalIndex);
				endCeilIndex = ceil(endFractionIndex);

				// Both fractions within one index, use a subfractional amount
				if(startCeilIndex == endCeilIndex){
					b.value()[x] = double(oneOriginalVector.at(startCeilIndex-1+scalerInitiateMovementIndex_))*(startFractionalIndex-endFractionIndex);
				} // The fractions are in adjacent indices, so use a fraction of each
				else if( (startCeilIndex-endCeilIndex) == 1){
					b.value()[x] = double(oneOriginalVector.at(startCeilIndex-1+scalerInitiateMovementIndex_))*(startFractionalIndex-double(startCeilIndex-1));
					b.value()[x] += double(oneOriginalVector.at(endCeilIndex-1+scalerInitiateMovementIndex_))*(double(endCeilIndex)-endFractionIndex);
				} // The fractions are separate by several indices, so use a fraction of the first and last and all of the ones in between
				else{
					b.value()[x] = double(oneOriginalVector.at(startCeilIndex-1+scalerInitiateMovementIndex_))*(startFractionalIndex-double(startCeilIndex-1));
					for(int y = startCeilIndex-1; y > endCeilIndex; y--)
						b.value()[x] += oneOriginalVector.at(y-1+scalerInitiateMovementIndex_);
					b.value()[x] += double(oneOriginalVector.at(endCeilIndex-1+scalerInitiateMovementIndex_))*(double(endCeilIndex)-endFractionIndex);
				}
			}
		}
		b++;
	}


	// Check the percent difference (conservation of total count) for scalers
	QMap<QString, QVector<double> >::const_iterator c = interpolatedScalerChannelVectors_.constBegin();
	while(c != interpolatedScalerChannelVectors_.constEnd()){
		QVector<qint32> rebaseOfInterest = scalerChannelRebaseVectors_.value(c.key()).mid(scalerInitiateMovementIndex_);
		QVector<double> interpolatedOfInterest = c.value();

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
			qDebug() << QString("For %1, rebase sum is %2 and interpolated sum is %3 with percent difference %4").arg(c.key()).arg(rebaseSum).arg(interpolatedSum).arg(percentDifference);
		c++;
	}
	*/

	QMap<QString, QVector<double> > interpolatedAmptekVectors;
	if(!amptekDetectors_.isEmpty()){
		AMDSClientDataRequest *oneAmptekRequest = clientDataRequestMap_.value(amptekDetectors_.first()->amdsBufferName());
		// Actually create the interpolated amptek vectors
		int amptekSize = amptekDetectors_.first()->size().product();
		QMap<QString, QVector<double> > originalAmptekVectors;
		foreach(CLSAmptekSDD123DetectorNew *amptekDetector, amptekDetectors_){
			originalAmptekVectors.insert(amptekDetector->name(), QVector<double>(oneAmptekRequest->data().count()*amptekSize));
			interpolatedAmptekVectors.insert(amptekDetector->name(), QVector<double>(interpolatedSize_*amptekSize));
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
		QMap<QString, QVector<double> >::iterator d = interpolatedAmptekVectors.begin();
		while(d != interpolatedAmptekVectors.end()){
			QVector<double> oneOriginalVector = originalAmptekVectors.value(d.key());

			double startFractionalIndex;
			double endFractionIndex;

			if(isUpScan_){
				int startFloorIndex;
				int endFloorIndex;

				for(int x = 0, size = d.value().count()/amptekSize; x < size; x++){
					if(x == 0)
						startFractionalIndex = 0;
					else
						startFractionalIndex = interpolatedMidpointsMappingIndices_.at(x-1);

					if(x == (d.value().count()/amptekSize)-1)
						endFractionIndex = ((oneOriginalVector.count()/amptekSize-1)-amptekInitiateMovementIndex)+0.999999;
					else
						endFractionIndex = interpolatedMidpointsMappingIndices_.at(x);

					startFloorIndex = floor(startFractionalIndex);
					endFloorIndex = floor(endFractionIndex);

					// Both fractions within one index, use a subfractional amount
					if(startFloorIndex == endFloorIndex){
						for(int z = 0; z < amptekSize; z++)
							d.value()[x*amptekSize+z] = oneOriginalVector.at( (startFloorIndex+amptekInitiateMovementIndex)*amptekSize + z )*(endFractionIndex-startFractionalIndex);
					} // The fractions are in adjacent indices, so use a fraction of each
					else if( (endFloorIndex-startFloorIndex) == 1){
						for(int z = 0; z < amptekSize; z++){
							d.value()[x*amptekSize+z] = oneOriginalVector.at( (startFloorIndex+amptekInitiateMovementIndex)*amptekSize + z )*(double(startFloorIndex+1)-startFractionalIndex);
							d.value()[x*amptekSize+z] += oneOriginalVector.at( (endFloorIndex+amptekInitiateMovementIndex)*amptekSize + z )*(endFractionIndex-double(endFloorIndex));
						}
					} // The fractions are separate by several indices, so use a fraction of the first and last and all of the ones in between
					else{
						for(int z = 0; z < amptekSize; z++){
							d.value()[x*amptekSize+z] = oneOriginalVector.at( (startFloorIndex+amptekInitiateMovementIndex)*amptekSize + z )*(double(startFloorIndex+1)-startFractionalIndex);
							for(int y = startFloorIndex+1; y < endFloorIndex; y++)
								d.value()[x*amptekSize+z] += oneOriginalVector.at( (y+amptekInitiateMovementIndex)*amptekSize + z );
							d.value()[x*amptekSize+z] += oneOriginalVector.at( (endFloorIndex+amptekInitiateMovementIndex)*amptekSize + z )*(endFractionIndex-double(endFloorIndex));
						}
					}
				}
			}
			else{
				int startCeilIndex;
				int endCeilIndex;

				for(int x = 0, size = d.value().count()/amptekSize; x < size; x++){
					if(x == 0)
						startFractionalIndex = ((oneOriginalVector.count()/amptekSize-1)-amptekInitiateMovementIndex)-0.000001;
					else
						startFractionalIndex = interpolatedMidpointsMappingIndices_.at(x-1);

					if(x == (d.value().count()/amptekSize)-1)
						endFractionIndex = 0;
					else
						endFractionIndex = interpolatedMidpointsMappingIndices_.at(x);

					startCeilIndex = ceil(startFractionalIndex);
					endCeilIndex = ceil(endFractionIndex);

					// Both fractions within one index, use a subfractional amount
					if(startCeilIndex == endCeilIndex){
						for(int z = 0; z < amptekSize; z++)
							d.value()[x*amptekSize+z] = oneOriginalVector.at( (startCeilIndex-1+amptekInitiateMovementIndex)*amptekSize + z )*(startFractionalIndex-endFractionIndex);
					} // The fractions are in adjacent indices, so use a fraction of each
					else if( (startCeilIndex-endCeilIndex) == 1){
						for(int z = 0; z < amptekSize; z++){
							d.value()[x*amptekSize+z] = oneOriginalVector.at( (startCeilIndex-1+amptekInitiateMovementIndex)*amptekSize + z )*(startFractionalIndex-double(startCeilIndex-1));
							d.value()[x*amptekSize+z] += oneOriginalVector.at( (endCeilIndex-1+amptekInitiateMovementIndex)*amptekSize + z )*(double(endCeilIndex)-endFractionIndex);
						}
					} // The fractions are separate by several indices, so use a fraction of the first and last and all of the ones in between
					else{
						for(int z = 0; z < amptekSize; z++){
							d.value()[x*amptekSize+z] = oneOriginalVector.at( (startCeilIndex-1+amptekInitiateMovementIndex)*amptekSize + z )*(startFractionalIndex-double(startCeilIndex-1));
							for(int y = startCeilIndex-1; y > endCeilIndex; y--)
								d.value()[x*amptekSize+z] += oneOriginalVector.at( (y-1+amptekInitiateMovementIndex)*amptekSize + z );
							d.value()[x*amptekSize+z] += oneOriginalVector.at( (endCeilIndex-1+amptekInitiateMovementIndex)*amptekSize + z )*(double(endCeilIndex)-endFractionIndex);
						}
					}
				}
			}
			d++;
		}

		// Check the percent difference (conservation of total count) for ampteks
		QMap<QString, QVector<double> >::const_iterator e = interpolatedAmptekVectors.constBegin();
		while(e != interpolatedAmptekVectors.constEnd()){
			QVector<double> rebaseOfInterest = originalAmptekVectors.value(e.key()).mid(amptekInitiateMovementIndex*amptekSize);
			QVector<double> interpolatedOfInterest = e.value();

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
				qDebug() << QString("For %1, rebase sum is %2 and interpolated sum is %3 with percent difference %4").arg(e.key()).arg(rebaseSum).arg(interpolatedSum).arg(percentDifference);
			e++;
		}
	}

	// END OF STEP 6


	// STEP 7: Place Data
	bool upScan = false;
	if(axisFeedbackValues_.first() < axisFeedbackValues_.last())
		upScan = true;

	CLSAMDSScalerChannelDetector *asScalerChannelDetector = 0;
	if(upScan){
		for(int x = 0, size = interpolatedAxis_.count(); x < size; x++){
			scan_->rawData()->beginInsertRows(1, -1);
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), interpolatedAxis_.at(x));

			for(int y = 0, ySize = generalConfig_->detectorConfigurations().count(); y < ySize; y++){
				AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(y));

				asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
				if(!asScalerChannelDetector){
					const double *detectorData = interpolatedAmptekVectors.value(oneDetector->name()).mid(insertionIndex_.i()*oneDetector->size().product(), oneDetector->size().product()).constData();
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
	}
	else{
		for(int x = 0, size = interpolatedAxis_.count(); x < size; x++){
			scan_->rawData()->beginInsertRows(1, -1);
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), interpolatedAxis_.at(x));

			for(int y = 0, ySize = generalConfig_->detectorConfigurations().count(); y < ySize; y++){
				AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(y));

				asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
				if(!asScalerChannelDetector){
					const double *detectorData = interpolatedAmptekVectors.value(oneDetector->name()).mid(insertionIndex_.i()*oneDetector->size().product(), oneDetector->size().product()).constData();
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
	}
	// END OF STEP 7

	// STEP 8: Clean Up
	QMap<QString, AMDSClientDataRequest*>::const_iterator j = clientDataRequestMap_.constBegin();
	while(j != clientDataRequestMap_.constEnd()){
		j.value()->deleteLater();
		j++;
	}

	onScanningActionsSucceeded();
	// END OF STEP 8
}

void SGMXASScanController::fillDataMaps(AMAgnosticDataAPIDataAvailableMessage *message)
{
	if(message->uniqueID() == "GratingEncoderFeedback")
		metaDataMap_.insert(message->uniqueID(), message->value("DetectorData").toList().at(0).toDouble());
	else{
		intptr_t dataRequestPointer = message->detectorDataAsAMDS();
		void *dataRequestVoidPointer = (void*)dataRequestPointer;
		AMDSClientDataRequest *dataRequest = static_cast<AMDSClientDataRequest*>(dataRequestVoidPointer);
		clientDataRequestMap_.insert(dataRequest->bufferName(), dataRequest);
	}
}

bool SGMXASScanController::generateAxisFeedbackValues()
{
	if(!metaDataMap_.contains("GratingEncoderFeedback")){
		AMErrorMon::debug(this, AMCONTINUOUSSCANACTIONCONTROLLER_INITIAL_ENCODER_POSITION_MISSING, "Missing initial encoder position");
		return false;
	}

	int startEncoderValue = (int)(metaDataMap_.value("GratingEncoderFeedback"));
	int currentEncoderValue = startEncoderValue;
	axisFeedbackValues_ = QVector<double>(expectedDurationScaledToBaseTimeScale_);
	for(int x = 0, size = axisFeedbackValues_.count(); x < size; x++){
		currentEncoderValue += encoderUpVector_.at(x+scalerInitiateMovementIndex_) - encoderDownVector_.at(x+scalerInitiateMovementIndex_);
		axisFeedbackValues_[x] = SGMGratingSupport::energyFromGrating(SGMGratingSupport::LowGrating, currentEncoderValue);
	}

	return true;
}

AMAction3 *SGMXASScanController::createInitializationActions()
{
	return 0;
}

AMAction3 *SGMXASScanController::createCleanupActions()
{
	return 0;
}

