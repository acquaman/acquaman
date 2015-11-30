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
	int largestBaseTimeScale = 0;
	for(int x = 0, size = timeScales_.count(); x < size; x++)
		if(timeScales_.at(x) > largestBaseTimeScale)
			largestBaseTimeScale = timeScales_.at(x);

	// Rebase the scaler or copy if no change in scale
	QMap<QString, QVector<qint32> > scalerChannelRebaseVectors;
	if(scalerChannelDetectors_.first()->amdsPollingBaseTimeMilliseconds() < largestBaseTimeScale)
		scalerChannelRebaseVectors = SGMBeamline::sgm()->amdsScaler()->rebaseScalerData(scalerChannelVectors, largestBaseTimeScale);
	else{
		QMap<QString, QVector<qint32> >::const_iterator l = scalerChannelVectors.constBegin();
		while(l != scalerChannelVectors.constEnd()){
			scalerChannelRebaseVectors.insert(l.key(), l.value());
			l++;
		}
	}

	// This should never happen, but what if it did?
	if(!amptekDetectors_.isEmpty() && amptekDetectors_.first()->amdsPollingBaseTimeMilliseconds() < largestBaseTimeScale){
		// what to do here?
	}
	// END OF STEP 3

	// STEP 4: Find Motion Start Indices
	int expectedDurationScaledToBaseTimeScale = double(continuousConfiguration_->scanAxes().at(0)->regionAt(0)->regionTime())*1000/largestBaseTimeScale;

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
		amptekRangeData = highestAverageAmptekDetector->retrieveContinuousMotionRangeData(amptekBaseData, expectedDurationScaledToBaseTimeScale, 20);
	}


	int scalerInitiateMovementIndex = 0;
	QList<QVector<qint32> > scalerBaseData;
	scalerBaseData << scalerChannelRebaseVectors.value("EncoderUp") << scalerChannelRebaseVectors.value("EncoderDown");
	AMDetectorContinuousMotionRangeData scalerRangeData = scalerChannelDetectors_.first()->retrieveContinuousMotionRangeData(scalerBaseData, expectedDurationScaledToBaseTimeScale, 20);
	if(scalerRangeData.isValid()){
		scalerInitiateMovementIndex = scalerRangeData.motionStartIndex();
	}

	QVector<qint32> encoderUpVector = scalerChannelRebaseVectors.value("EncoderUp");
	QVector<qint32> encoderDownVector = scalerChannelRebaseVectors.value("EncoderDown");
	if( (encoderUpVector.count()-scalerInitiateMovementIndex) < expectedDurationScaledToBaseTimeScale){
		qDebug() << "Scaler start movment index was out of bounds at " << scalerInitiateMovementIndex;
		scalerInitiateMovementIndex += ((encoderUpVector.count()-scalerInitiateMovementIndex)-expectedDurationScaledToBaseTimeScale);
		scalerRangeData.setMotionStartIndex(scalerInitiateMovementIndex);
		qDebug() << "Changed to " << scalerInitiateMovementIndex;
	}
	if(!amptekDetectors_.isEmpty()){
		AMDSClientDataRequest *oneAmptekRequest = clientDataRequestMap_.value(amptekDetectors_.first()->amdsBufferName());
		if( (oneAmptekRequest->data().count() - amptekRangeData.motionStartIndex()) < expectedDurationScaledToBaseTimeScale){
			qDebug() << "Amptek start movment index was out of bounds at " << amptekRangeData.motionStartIndex();
			amptekRangeData.setMotionStartIndex( ((oneAmptekRequest->data().count() - amptekRangeData.motionStartIndex()) - expectedDurationScaledToBaseTimeScale) + amptekRangeData.motionStartIndex() );
			qDebug() << "Changed to " << amptekRangeData.motionStartIndex();
		}
	}

	qDebug() << "Amptek: " << amptekRangeData.motionStartIndex() << amptekRangeData.motionEndIndex() << amptekRangeData.listIndex();
	qDebug() << "Scaler: " << scalerRangeData.motionStartIndex() << scalerRangeData.motionEndIndex() << scalerRangeData.listIndex();

	CLSAMDSScalerChannelDetector *asScalerChannelDetector = 0;
	CLSAmptekSDD123DetectorNew *asAmptekDetector = 0;
	QMap<QString, int> detectorStartMotionIndexMap;
	for(int x = 0, size = generalConfig_->detectorConfigurations().count(); x < size; x++){
		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(x));

		asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
		if(asScalerChannelDetector)
			detectorStartMotionIndexMap.insert(oneDetector->name(), scalerRangeData.motionStartIndex());

		asAmptekDetector = qobject_cast<CLSAmptekSDD123DetectorNew*>(oneDetector);
		if(asAmptekDetector)
			detectorStartMotionIndexMap.insert(oneDetector->name(), amptekRangeData.motionStartIndex());

		if(!asScalerChannelDetector && !asAmptekDetector){
			qDebug() << "Can't identify type of detector " << oneDetector->name() << " so no start motion index available";
			detectorStartMotionIndexMap.insert(oneDetector->name(), 0);
		}
	}

	// END OF STEP 4


	// STEP 5: Generate the Axis Values "In Order"
	if(!metaDataMap_.contains("GratingEncoderFeedback")){
		AMErrorMon::debug(this, AMCONTINUOUSSCANACTIONCONTROLLER_INITIAL_ENCODER_POSITION_MISSING, "Missing initial encoder position");
		return;
	}

	int startEncoderValue = (int)(metaDataMap_.value("GratingEncoderFeedback"));
	int currentEncoderValue = startEncoderValue;
	QVector<double> scalerEnergyFeedbacks = QVector<double>(expectedDurationScaledToBaseTimeScale);
	for(int x = 0, size = scalerEnergyFeedbacks.count(); x < size; x++){
		currentEncoderValue += encoderUpVector.at(x+scalerInitiateMovementIndex) - encoderDownVector.at(x+scalerInitiateMovementIndex);
		scalerEnergyFeedbacks[x] = SGMGratingSupport::energyFromGrating(SGMGratingSupport::LowGrating, currentEncoderValue);
	}
	// END OF STEP 5


	// STEP 6: Interpolation

	// Set up interpolation parameters
	double energyStep = 0.1;
	double startEnergy = scalerEnergyFeedbacks.first();
	double endEnergy = scalerEnergyFeedbacks.last();
	if(startEnergy > endEnergy)
		qSwap(startEnergy, endEnergy);

	qDebug() << "Start energy was " << startEnergy << " end energy was " << endEnergy;
	startEnergy = ceilf(startEnergy * (1/energyStep)) / (1/energyStep);
	endEnergy = floorf(endEnergy * (1/energyStep)) / (1/energyStep);
	qDebug() << "Start energy is " << startEnergy << " end energy is " << endEnergy;

	double interpolatedSize = (endEnergy-startEnergy)/energyStep;
	qDebug() << "interpolatedSize is " << interpolatedSize;

	// Create the interpolated axis and midpoints
	double currentEnergy = startEnergy;
	QVector<double> interpolatedEnergyAxis = QVector<double>(interpolatedSize);
	QVector<double> interpolatedEnergyMidpoints = QVector<double>(interpolatedSize);
	for(int x = 0, size = interpolatedEnergyAxis.count(); x < size; x++){
		interpolatedEnergyAxis[x] = currentEnergy;
		interpolatedEnergyMidpoints[x] = currentEnergy+(energyStep/2);
		currentEnergy += energyStep;
	}

	bool isUpScan = true;
	if(scalerEnergyFeedbacks.first() > scalerEnergyFeedbacks.last())
		isUpScan = false;

	// Find the index mapping between interpolated points and feedback points
	QVector<double> interpolatedEnergyMidpointsMappingIndices = QVector<double>(interpolatedSize);
	int currentInOrderEnergyLookupIndex = 0;
	int inOrderEnergyRelativeStep = 1;

	if(!isUpScan){
		currentInOrderEnergyLookupIndex = scalerEnergyFeedbacks.count()-1;
		inOrderEnergyRelativeStep = -1;
	}

	double currentInOrderEnergyValue = scalerEnergyFeedbacks.at(currentInOrderEnergyLookupIndex);
//	double lastInOrderEnergyValue = currentInOrderEnergyLookupIndex;
	double lastInOrderEnergyValue = currentInOrderEnergyValue;
	double fractionalIndex = 0;
	double lastFractionalIndex = 0;



	if(isUpScan){
		for(int x = 0, size = interpolatedSize; x < size; x++){
			double oneEnergyMidpoint = interpolatedEnergyMidpoints.at(x);

			while(oneEnergyMidpoint > currentInOrderEnergyValue){
				currentInOrderEnergyLookupIndex++;
				lastInOrderEnergyValue = currentInOrderEnergyValue;
				currentInOrderEnergyValue = scalerEnergyFeedbacks.at(currentInOrderEnergyLookupIndex);
			}

			lastFractionalIndex = fractionalIndex;
			fractionalIndex = (currentInOrderEnergyLookupIndex-1) + ((oneEnergyMidpoint-lastInOrderEnergyValue)/(currentInOrderEnergyValue-lastInOrderEnergyValue));

	//		qDebug() << QString("Found %1 between %2 [%3] and %4 [%5] as fractional %6").arg(oneEnergyMidpoint).arg(lastInOrderEnergyValue).arg(currentInOrderEnergyLookupIndex-1).arg(currentInOrderEnergyValue).arg(currentInOrderEnergyLookupIndex).arg(fractionalIndex);
	//		if(lastFractionalIndex >= fractionalIndex)
	//			qDebug() << "\n\nFound an out of order step with " << lastFractionalIndex << " greater than " << fractionalIndex << "\n";
			interpolatedEnergyMidpointsMappingIndices[x] = fractionalIndex;
		}
	}
	else{
		for(int x = 0, size = interpolatedSize; x < size; x++){
			double oneEnergyMidpoint = interpolatedEnergyMidpoints.at(x);
	//		double currentInOrderEnergyValue = scalerEnergyFeedbacks.at(currentInOrderEnergyLookupIndex);

			while(oneEnergyMidpoint > currentInOrderEnergyValue){
				currentInOrderEnergyLookupIndex--;
//				currentInOrderEnergyLookupIndex += inOrderEnergyRelativeStep;
				lastInOrderEnergyValue = currentInOrderEnergyValue;
				currentInOrderEnergyValue = scalerEnergyFeedbacks.at(currentInOrderEnergyLookupIndex);
			}

			lastFractionalIndex = fractionalIndex;
			fractionalIndex = (currentInOrderEnergyLookupIndex+1) - ((oneEnergyMidpoint-lastInOrderEnergyValue)/(currentInOrderEnergyValue-lastInOrderEnergyValue));
//			fractionalIndex = (currentInOrderEnergyLookupIndex-1) + (inOrderEnergyRelativeStep)*((oneEnergyMidpoint-lastInOrderEnergyValue)/(currentInOrderEnergyValue-lastInOrderEnergyValue));

//			qDebug() << QString("Found %1 between %2 [%3] and %4 [%5] as fractional %6").arg(oneEnergyMidpoint).arg(lastInOrderEnergyValue).arg(currentInOrderEnergyLookupIndex+1).arg(currentInOrderEnergyValue).arg(currentInOrderEnergyLookupIndex).arg(fractionalIndex);
	//		if(lastFractionalIndex >= fractionalIndex)
	//			qDebug() << "\n\nFound an out of order step with " << lastFractionalIndex << " greater than " << fractionalIndex << "\n";
			interpolatedEnergyMidpointsMappingIndices[x] = fractionalIndex;
		}
	}
	/*
	for(int x = 0, size = interpolatedSize; x < size; x++){
		double oneEnergyMidpoint = interpolatedEnergyMidpoints.at(x);
//		double currentInOrderEnergyValue = scalerEnergyFeedbacks.at(currentInOrderEnergyLookupIndex);

		while(oneEnergyMidpoint > currentInOrderEnergyValue){
//			currentInOrderEnergyLookupIndex++;
			currentInOrderEnergyLookupIndex += inOrderEnergyRelativeStep;
			lastInOrderEnergyValue = currentInOrderEnergyValue;
			currentInOrderEnergyValue = scalerEnergyFeedbacks.at(currentInOrderEnergyLookupIndex);
		}

		lastFractionalIndex = fractionalIndex;
//		fractionalIndex = (currentInOrderEnergyLookupIndex-1) + ((oneEnergyMidpoint-lastInOrderEnergyValue)/(currentInOrderEnergyValue-lastInOrderEnergyValue));
		fractionalIndex = (currentInOrderEnergyLookupIndex-1) + (inOrderEnergyRelativeStep)*((oneEnergyMidpoint-lastInOrderEnergyValue)/(currentInOrderEnergyValue-lastInOrderEnergyValue));

//		qDebug() << QString("Found %1 between %2 [%3] and %4 [%5] as fractional %6").arg(oneEnergyMidpoint).arg(lastInOrderEnergyValue).arg(currentInOrderEnergyLookupIndex-1).arg(currentInOrderEnergyValue).arg(currentInOrderEnergyLookupIndex).arg(fractionalIndex);
//		if(lastFractionalIndex >= fractionalIndex)
//			qDebug() << "\n\nFound an out of order step with " << lastFractionalIndex << " greater than " << fractionalIndex << "\n";
		interpolatedEnergyMidpointsMappingIndices[x] = fractionalIndex;
	}
	*/

	// Just some debugs
//	qDebug() << "Fractional Indices are: ";
	qDebug() << interpolatedEnergyMidpointsMappingIndices;

//	qDebug() << QString("Energy Value %1 starts at index %2 with fractional index width %3").arg(interpolatedEnergyAxis.at(0), 0, 'f', 2).arg(0, 2, 'f', 3, ' ').arg(interpolatedEnergyMidpointsMappingIndices.at(0), 0, 'f', 4);
//	for(int x = 1, size = interpolatedEnergyMidpoints.count()-1; x < size; x++){
//		qDebug() << QString("Energy Value %1 starts at index %2 with fractional index width %3").arg(interpolatedEnergyAxis.at(x), 0, 'f', 2).arg(interpolatedEnergyMidpointsMappingIndices.at(x-1), 2, 'f', 3, ' ').arg(interpolatedEnergyMidpointsMappingIndices.at(x)-interpolatedEnergyMidpointsMappingIndices.at(x-1), 0, 'f', 4);
//	}

	// Actually create the interpolated scaler vectors
	QMap<QString, QVector<double> > interpolatedScalerChannelVectors;
	QMap<QString, QVector<qint32> >::const_iterator a = scalerChannelVectors.constBegin();
	while(a != scalerChannelVectors.constEnd()){
		interpolatedScalerChannelVectors.insert(a.key(), QVector<double>(interpolatedSize));
		a++;
	}

	// Loop over scaler interpolated vectors and fill them
	QMap<QString, QVector<double> >::iterator b = interpolatedScalerChannelVectors.begin();
	while(b != interpolatedScalerChannelVectors.end()){
//		QVector<qint32> oneOriginalVector = scalerChannelVectors.value(b.key());
		QVector<qint32> oneOriginalVector = scalerChannelRebaseVectors.value(b.key());

		double startFractionalIndex;
		double endFractionIndex;

		int innerCount = 0;
		int adjacentCount = 0;
		int spreadCount = 0;

		if(isUpScan){
			qDebug() << "DOING AN UP SCAN " << b.key();
			int startFloorIndex;
			int endFloorIndex;
			for(int x = 0, size = b.value().count(); x < size; x++){
				if(x == 0){
					startFractionalIndex = 0;
					qDebug() << "Effective start index " << startFractionalIndex;
				}
				else
					startFractionalIndex = interpolatedEnergyMidpointsMappingIndices.at(x-1);

				if(x == b.value().count()-1){
					endFractionIndex = ((oneOriginalVector.count()-1)-scalerInitiateMovementIndex)+0.999999;
					qDebug() << "Effective end index " << endFractionIndex << oneOriginalVector.count() << scalerInitiateMovementIndex;
				}
				else
					endFractionIndex = interpolatedEnergyMidpointsMappingIndices.at(x);


				startFloorIndex = floor(startFractionalIndex);
				endFloorIndex = floor(endFractionIndex);
	//			qDebug() << "Start fractional index is " << startFractionalIndex;
	//			qDebug() << "End fractional index is " << endFractionIndex;
	//			qDebug() << "Start floor index is " << startFloorIndex;
	//			qDebug() << "End floor index is " << endFloorIndex;
	//			qDebug() << "\n";


				// Both fractions within one index, use a subfractional amount
				if(startFloorIndex == endFloorIndex){
					innerCount++;
					b.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex))*(endFractionIndex-startFractionalIndex);

//					qDebug() << "Adding[i] " << b.value().at(x);
				} // The fractions are in adjacent indices, so use a fraction of each
	//			else if( (endFloorIndex-startFloorIndex) == 1){
				else if( (endFloorIndex-startFloorIndex) == inOrderEnergyRelativeStep){
					adjacentCount++;
	//				qDebug() << "StartFractional " << startFractionalIndex << "EndFractional " << endFractionIndex;
					b.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex))*(double(startFloorIndex+1)-startFractionalIndex);
					b.value()[x] += double(oneOriginalVector.at(endFloorIndex+scalerInitiateMovementIndex))*(endFractionIndex-double(endFloorIndex));
//					qDebug() << "Adding[a] " << b.value().at(x);
				} // The fractions are separate by several indices, so use a fraction of the first and last and all of the ones in between
				else{
					spreadCount++;
	//				qDebug() << QString("Do %1 (%2) From %3 to %4 and %5 (%6)").arg(startFloorIndex).arg(startFloorIndex+scalerInitiateMovementIndex).arg(startFloorIndex+inOrderEnergyRelativeStep).arg(endFloorIndex).arg(endFloorIndex).arg(endFloorIndex+scalerInitiateMovementIndex);
					b.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex))*(double(startFloorIndex+1)-startFractionalIndex);
	//				for(int y = startFloorIndex+1; y < endFloorIndex; y++)
					for(int y = startFloorIndex+inOrderEnergyRelativeStep; y < endFloorIndex; y += inOrderEnergyRelativeStep)
						b.value()[x] += oneOriginalVector.at(y+scalerInitiateMovementIndex);
					b.value()[x] += double(oneOriginalVector.at(endFloorIndex+scalerInitiateMovementIndex))*(endFractionIndex-double(endFloorIndex));

//					qDebug() << "Adding[s] " << b.value().at(x);
				}

//				qDebug() << QString("At %1 [%2] {%3 %4} {%5 %6} is %7").arg(x).arg(interpolatedEnergyMidpoints.at(x)).arg(startFractionalIndex).arg(endFractionIndex).arg(startFloorIndex).arg(endFloorIndex).arg(b.value().at(x));
			}
		}
		else{
			if(b.key() == "EncoderDown")
				qDebug() << "DOING A DOWN SCAN " << b.key();
			int startCeilIndex;
			int endCeilIndex;
			for(int x = 0, size = b.value().count(); x < size; x++){
				if(x == 0){
					startFractionalIndex = ((oneOriginalVector.count()-1)-scalerInitiateMovementIndex)-0.0000001;
					qDebug() << "Effective end index " << startFractionalIndex << oneOriginalVector.count() << scalerInitiateMovementIndex;
				}
				else
					startFractionalIndex = interpolatedEnergyMidpointsMappingIndices.at(x-1);

				if(x == b.value().count()-1){
					endFractionIndex = 0;
					qDebug() << "Effective start index " << endFractionIndex;
				}
				else
					endFractionIndex = interpolatedEnergyMidpointsMappingIndices.at(x);


				startCeilIndex = ceil(startFractionalIndex);
				endCeilIndex = ceil(endFractionIndex);

				// Both fractions within one index, use a subfractional amount
				if(startCeilIndex == endCeilIndex){
					innerCount++;
					b.value()[x] = double(oneOriginalVector.at(startCeilIndex-1+scalerInitiateMovementIndex))*(startFractionalIndex-endFractionIndex);

//					qDebug() << "Adding[i] " << b.value().at(x);
				} // The fractions are in adjacent indices, so use a fraction of each
				else if( (startCeilIndex-endCeilIndex) == 1){
					adjacentCount++;
					b.value()[x] = double(oneOriginalVector.at(startCeilIndex-1+scalerInitiateMovementIndex))*(startFractionalIndex-double(startCeilIndex-1));
					b.value()[x] += double(oneOriginalVector.at(endCeilIndex-1+scalerInitiateMovementIndex))*(double(endCeilIndex)-endFractionIndex);
//					qDebug() << "Adding[a] " << b.value().at(x);
				} // The fractions are separate by several indices, so use a fraction of the first and last and all of the ones in between
				else{
					spreadCount++;
					b.value()[x] = double(oneOriginalVector.at(startCeilIndex-1+scalerInitiateMovementIndex))*(startFractionalIndex-double(startCeilIndex+inOrderEnergyRelativeStep));
					for(int y = startCeilIndex-1; y < endCeilIndex; y--)
						b.value()[x] += oneOriginalVector.at(y-1+scalerInitiateMovementIndex);
					b.value()[x] += double(oneOriginalVector.at(endCeilIndex-1+scalerInitiateMovementIndex))*(double(endCeilIndex)-endFractionIndex);

//					qDebug() << "Adding[s] " << b.value().at(x);
				}

				if(b.key() == "EncoderDown")
					qDebug() << QString("At %1 [%2] {%3 %4} {%5 %6} is %7").arg(x).arg(interpolatedEnergyMidpoints.at(x)).arg(startFractionalIndex).arg(endFractionIndex).arg(startCeilIndex).arg(endCeilIndex).arg(b.value().at(x));
			}
		}

		/*
		for(int x = 0, size = b.value().count(); x < size; x++){
			if((x == 0) && isUpScan)
				startFractionalIndex = 0;
			else if((x == 0) && !isUpScan)
				startFractionalIndex = ((oneOriginalVector.count()-1)-scalerInitiateMovementIndex)-0.999999;
			else
				startFractionalIndex = interpolatedEnergyMidpointsMappingIndices.at(x-1);

			if( (x == b.value().count()-1) && isUpScan)
				endFractionIndex = ((oneOriginalVector.count()-1)-scalerInitiateMovementIndex)+0.999999;
			else if((x == b.value().count()-1) && !isUpScan)
				endFractionIndex = 0;
			else
				endFractionIndex = interpolatedEnergyMidpointsMappingIndices.at(x);


			if(isUpScan){
				startFloorIndex = floor(startFractionalIndex);
				endFloorIndex = floor(endFractionIndex);
			}
			else{
				startFloorIndex = ceil(startFractionalIndex);
				endFloorIndex = ceil(endFractionIndex);
			}
//			qDebug() << "Start fractional index is " << startFractionalIndex;
//			qDebug() << "End fractional index is " << endFractionIndex;
//			qDebug() << "Start floor index is " << startFloorIndex;
//			qDebug() << "End floor index is " << endFloorIndex;
//			qDebug() << "\n";


			// Both fractions within one index, use a subfractional amount
			if(startFloorIndex == endFloorIndex){
				innerCount++;
				if(isUpScan)
					b.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex))*(endFractionIndex-startFractionalIndex);
				else
					b.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex))*(startFractionalIndex-endFractionIndex);

				qDebug() << "Adding[i] " << b.value().at(x);
			} // The fractions are in adjacent indices, so use a fraction of each
//			else if( (endFloorIndex-startFloorIndex) == 1){
			else if( (endFloorIndex-startFloorIndex) == inOrderEnergyRelativeStep){
				adjacentCount++;
//				qDebug() << "StartFractional " << startFractionalIndex << "EndFractional " << endFractionIndex;
				if(isUpScan){
					b.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex))*(double(startFloorIndex+1)-startFractionalIndex);
					b.value()[x] += double(oneOriginalVector.at(endFloorIndex+scalerInitiateMovementIndex))*(endFractionIndex-double(endFloorIndex));
				}
				else{
					b.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex))*(startFractionalIndex-double(startFloorIndex+inOrderEnergyRelativeStep));
					b.value()[x] += double(oneOriginalVector.at(endFloorIndex+scalerInitiateMovementIndex))*(double(endFloorIndex)-endFractionIndex);
				}
				qDebug() << "Adding[a] " << b.value().at(x);
			} // The fractions are separate by several indices, so use a fraction of the first and last and all of the ones in between
			else{
				spreadCount++;
//				qDebug() << QString("Do %1 (%2) From %3 to %4 and %5 (%6)").arg(startFloorIndex).arg(startFloorIndex+scalerInitiateMovementIndex).arg(startFloorIndex+inOrderEnergyRelativeStep).arg(endFloorIndex).arg(endFloorIndex).arg(endFloorIndex+scalerInitiateMovementIndex);
				if(isUpScan){
					b.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex))*(double(startFloorIndex+1)-startFractionalIndex);
					b.value()[x] += double(oneOriginalVector.at(endFloorIndex+scalerInitiateMovementIndex))*(endFractionIndex-double(endFloorIndex));
				}
				else{
					b.value()[x] = double(oneOriginalVector.at(startFloorIndex+scalerInitiateMovementIndex))*(startFractionalIndex-double(startFloorIndex+inOrderEnergyRelativeStep));
					b.value()[x] += double(oneOriginalVector.at(endFloorIndex+scalerInitiateMovementIndex))*(double(endFloorIndex)-endFractionIndex);
				}
//				for(int y = startFloorIndex+1; y < endFloorIndex; y++)
				for(int y = startFloorIndex+inOrderEnergyRelativeStep; y < endFloorIndex; y += inOrderEnergyRelativeStep)
					b.value()[x] += oneOriginalVector.at(y+scalerInitiateMovementIndex);

				qDebug() << "Adding[s] " << b.value().at(x);
			}
		}
		*/
		b++;

		qDebug() << "Inner: " << innerCount << " adjacent: " << adjacentCount << " spread: " << spreadCount;
	}


	// Check the percent difference (conservation of total count) for scalers
	QMap<QString, QVector<double> >::const_iterator c = interpolatedScalerChannelVectors.constBegin();
	while(c != interpolatedScalerChannelVectors.constEnd()){
		QVector<qint32> rebaseOfInterest = scalerChannelRebaseVectors.value(c.key()).mid(scalerInitiateMovementIndex);
		QVector<double> interpolatedOfInterest = c.value();

		qDebug() << "Rebase size: " << rebaseOfInterest.count();
		qDebug() << "Interpolated size: " << interpolatedOfInterest.count();

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

		qDebug() << QString("For %1, rebase sum is %2 and interpolated sum is %3 with percent difference %4").arg(c.key()).arg(rebaseSum).arg(interpolatedSum).arg(percentDifference);
//		qDebug() << QString("Rebased [%1]: ").arg(rebaseOfInterest.count()) << rebaseOfInterest;
//		qDebug() << QString("Interpolated [%1]: ").arg(interpolatedOfInterest.count()) << interpolatedOfInterest;
//		qDebug() << "\n\n";
		c++;
	}

	QMap<QString, QVector<double> > interpolatedAmptekVectors;
	if(!amptekDetectors_.isEmpty()){
		AMDSClientDataRequest *oneAmptekRequest = clientDataRequestMap_.value(amptekDetectors_.first()->amdsBufferName());
		// Actually create the interpolated amptek vectors
		int amptekSize = amptekDetectors_.first()->size().product();
		QMap<QString, QVector<double> > originalAmptekVectors;
		foreach(CLSAmptekSDD123DetectorNew *amptekDetector, amptekDetectors_){
			originalAmptekVectors.insert(amptekDetector->name(), QVector<double>(oneAmptekRequest->data().count()*amptekSize));
			interpolatedAmptekVectors.insert(amptekDetector->name(), QVector<double>(interpolatedSize*amptekSize));
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

//		int amptekInitiateMovementIndex = detectorStartMotionIndexMap.value(amptekDetectors_.first()->name());
		int amptekInitiateMovementIndex = scalerInitiateMovementIndex;
		// Loop over amptek interpolated vectors and fill them
		QMap<QString, QVector<double> >::iterator d = interpolatedAmptekVectors.begin();
		while(d != interpolatedAmptekVectors.end()){
			QVector<double> oneOriginalVector = originalAmptekVectors.value(d.key());

			double startFractionalIndex;
			int startFloorIndex;
			double endFractionIndex;
			int endFloorIndex;

			for(int x = 0, size = d.value().count()/amptekSize; x < size; x++){
				if(x == 0)
					startFractionalIndex = 0;
				else
					startFractionalIndex = interpolatedEnergyMidpointsMappingIndices.at(x-1);

				if(x == (d.value().count()/amptekSize)-1)
					endFractionIndex = ((oneOriginalVector.count()/amptekSize-1)-amptekInitiateMovementIndex)+0.999999;
				else
					endFractionIndex = interpolatedEnergyMidpointsMappingIndices.at(x);

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

			qDebug() << QString("For %1, rebase sum is %2 and interpolated sum is %3 with percent difference %4").arg(e.key()).arg(rebaseSum).arg(interpolatedSum).arg(percentDifference);
			e++;
		}
	}

	// END OF STEP 6


	// STEP 7: Place Data
	bool upScan = false;
	if(scalerEnergyFeedbacks.first() < scalerEnergyFeedbacks.last())
		upScan = true;

	if(upScan){
		for(int x = 0, size = interpolatedEnergyAxis.count(); x < size; x++){
			scan_->rawData()->beginInsertRows(1, -1);
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), interpolatedEnergyAxis.at(x));

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
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(i.value()), AMnDIndex(), interpolatedScalerChannelVectors.value(i.value()).at(x));
				i++;
			}

			scan_->rawData()->endInsertRows();
			insertionIndex_[0] = insertionIndex_.i()+1;
		}
	}
	else{
		for(int x = 0, size = interpolatedEnergyAxis.count(); x < size; x++){
			scan_->rawData()->beginInsertRows(1, -1);
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), interpolatedEnergyAxis.at(x));

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
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(i.value()), AMnDIndex(), interpolatedScalerChannelVectors.value(i.value()).at(x));
				i++;
			}

			scan_->rawData()->endInsertRows();
			insertionIndex_[0] = insertionIndex_.i()+1;
		}
	}

	/*
	AMDSLightWeightGenericFlatArrayDataHolder *dataHolderAsGenericFlatArrayDataHolder = 0;
	if(upScan){
		for(int x = 0, size = scalerEnergyFeedbacks.count(); x < size; x++){
			scan_->rawData()->beginInsertRows(1, -1);
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), scalerEnergyFeedbacks.at(x));

			for(int y = 0, ySize = generalConfig_->detectorConfigurations().count(); y < ySize; y++){
				AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(y));

				asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
				if(!asScalerChannelDetector){
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value(oneDetector->amdsBufferName())->data().at(x+detectorStartMotionIndexMap.value(oneDetector->name())));
					scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(oneDetector->name()), dataHolderAsGenericFlatArrayDataHolder->dataArray().asConstVectorDouble().constData());
				}
			}

			QMap<int, QString>::const_iterator i = scalerChannelIndexMap_.constBegin();
			while(i != scalerChannelIndexMap_.constEnd()){
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(i.value()), AMnDIndex(), scalerChannelRebaseVectors.value(i.value()).at(x+detectorStartMotionIndexMap.value(i.value())));
				i++;
			}

			scan_->rawData()->endInsertRows();
			insertionIndex_[0] = insertionIndex_.i()+1;
		}
	}
	else{
		for(int x = scalerEnergyFeedbacks.count()-1; x >= 0; x--){
			scan_->rawData()->beginInsertRows(1, -1);
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), scalerEnergyFeedbacks.at(x));

			for(int y = 0, ySize = generalConfig_->detectorConfigurations().count(); y < ySize; y++){
				AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(y));

				asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
				if(!asScalerChannelDetector){
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value(oneDetector->amdsBufferName())->data().at(x+detectorStartMotionIndexMap.value(oneDetector->name())));
					scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(oneDetector->name()), dataHolderAsGenericFlatArrayDataHolder->dataArray().asConstVectorDouble().constData());
				}
			}

			QMap<int, QString>::const_iterator i = scalerChannelIndexMap_.constBegin();
			while(i != scalerChannelIndexMap_.constEnd()){
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(i.value()), AMnDIndex(), scalerChannelRebaseVectors.value(i.value()).at(x+detectorStartMotionIndexMap.value(i.value())));
				i++;
			}

			scan_->rawData()->endInsertRows();
			insertionIndex_[0] = insertionIndex_.i()+1;
		}
	}
	*/
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

AMAction3 *SGMXASScanController::createInitializationActions()
{
	return 0;
}

AMAction3 *SGMXASScanController::createCleanupActions()
{
	return 0;
}

