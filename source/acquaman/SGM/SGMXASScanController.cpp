#include "SGMXASScanController.h"

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSAMDSScalerChannelDetector.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"

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
	/*
	QList<QString> requiredBufferNames;
	QList<int> timeScales;

	CLSAMDSScalerChannelDetector *tempScalerChannelDetector = 0;
	bool scalerChannelDetectorsPresent = false;
	CLSAmptekSDD123DetectorNew *tempAmptekDetector = 0;
	bool amptekDetectorsPresent = false;
	QList<CLSAmptekSDD123DetectorNew*> amptekDetectors;

	for(int x = 0, size = generalConfig_->detectorConfigurations().count(); x < size; x++){
		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(x));

		if(!oneDetector->amdsBufferName().isEmpty() && !requiredBufferNames.contains(oneDetector->amdsBufferName()))
			requiredBufferNames.append(oneDetector->amdsBufferName());

		if(!tempScalerChannelDetector){
			tempScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
			if(tempScalerChannelDetector){
				timeScales.append(tempScalerChannelDetector->amdsPollingBaseTimeMilliseconds());
				scalerChannelDetectorsPresent = true;
			}
		}

		tempAmptekDetector = qobject_cast<CLSAmptekSDD123DetectorNew*>(oneDetector);
		if(tempAmptekDetector){
			timeScales.append(tempAmptekDetector->amdsPollingBaseTimeMilliseconds());
			amptekDetectorsPresent = true;
			amptekDetectors.append(tempAmptekDetector);
		}
	}

	// Always add a sensible value of 25ms ... any more time resolution and even quick scans (~10s) are too many points
	timeScales.append(25);

	for(int x = 0, size = requiredBufferNames.count(); x < size; x++){
		if(!clientDataRequestMap_.contains(requiredBufferNames.at(x))){
			AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_REQUIRED_DATA_MISSING, QString("Missing data %1").arg(requiredBufferNames.at(x)));
			return;
		}
	}
	*/
	if(!generateAnalysisMetaInfo())
		return;
	// END OF STEP 1

	// STEP 2: Retrieve and remap the scaler data into vectors for each channel
	AMDSClientDataRequest *scalerClientDataRequest = clientDataRequestMap_.value("Scaler (BL1611-ID-1)");
	if(!scalerClientDataRequest){
		AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_REQUIRED_DATA_MISSING, QString("Missing scaler data for continuous scan processing."));
		return;
	}

	int totalCount = scalerClientDataRequest->data().count();
	QMap<QString, QVector<qint32> > scalerChannelVectors;
	QMap<int, QString> scalerChannelIndexMap;

	// Set up maps
	CLSAMDSScalerChannelDetector *asScalerChannelDetector = 0;
	for(int x = 0, size = generalConfig_->detectorConfigurations().count(); x < size; x++){
		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(x));
		asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
		if(asScalerChannelDetector){
			scalerChannelIndexMap.insert(asScalerChannelDetector->enabledChannelIndex(), oneDetector->name());
			scalerChannelVectors.insert(oneDetector->name(), QVector<qint32>(totalCount, 0));
		}
	}

	// Check data holder casts to correct type
	AMDSLightWeightScalarDataHolder *asScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(scalerClientDataRequest->data().at(0));
	if(!asScalarDataHolder){
		AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_BAD_SCALER_DATAHOLDER_TYPE, QString("Scaler data holder is the wrong type."));
		return;
	}
	// Check that we won't index into more positions than the AMDS-Scaler returned
	if(scalerChannelIndexMap.count() > asScalarDataHolder->dataArray().constVectorQint32().count()){
		AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_SCALER_CHANNEL_MISMATCH, QString("There is a mismatch between the number of enabled scaler channels and the number requested."));
		return;
	}

	// Retrieve scaler data from data holders and place into a vector for each channel
	for(int x = 0; x < totalCount; x++){
		AMDSLightWeightScalarDataHolder *asScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(scalerClientDataRequest->data().at(x));
		if(asScalarDataHolder){
			QVector<qint32> oneVector = asScalarDataHolder->dataArray().constVectorQint32();

			QString channelString;
			for(int y = 0, ySize = oneVector.count(); y < ySize; y++){
				if(scalerChannelIndexMap.contains(y)){
					channelString = scalerChannelIndexMap.value(y);
					(scalerChannelVectors[channelString])[x] = oneVector.at(y);
				}
			}
		}
	}
	// END OF STEP 2


	// STEP 3: Rebase
	int largestBaseTimeScale = 0;
	for(int x = 0, size = timeScales_.count(); x < size; x++)
		if(timeScales_.at(x) > largestBaseTimeScale)
			largestBaseTimeScale = timeScales_.at(x);

	// Rebase the scaler
	QMap<QString, QVector<qint32> > scalerChannelRebaseVectors;
	if(scalerChannelDetectors_.first()->amdsPollingBaseTimeMilliseconds() < largestBaseTimeScale){
		int baseScalerTimeScale = scalerChannelDetectors_.first()->amdsPollingBaseTimeMilliseconds();
		int rebasedTotalCount = (totalCount*baseScalerTimeScale)/largestBaseTimeScale;
		qDebug() << "Original totalCount " << totalCount << " rebasedTotalCount " << rebasedTotalCount;


		QMap<QString, qint32> scalerChannelRunningSums;

		QMap<int, QString>::const_iterator i = scalerChannelIndexMap.constBegin();
		while(i != scalerChannelIndexMap.constEnd()){
			scalerChannelRebaseVectors.insert(i.value(), QVector<qint32>(rebasedTotalCount, 0));
			scalerChannelRunningSums.insert(i.value(), 0);
			i++;
		}

		for(int x = 0; x < totalCount; x++){
			int tempRunningSum;
			QString channelString;
			QMap<int, QString>::const_iterator k = scalerChannelIndexMap.constBegin();
			while(k != scalerChannelIndexMap.constEnd()){
				channelString = k.value();
				tempRunningSum = scalerChannelRunningSums.value(channelString);
				tempRunningSum += (scalerChannelVectors[channelString]).at(x);
				scalerChannelRunningSums[channelString] = tempRunningSum;

				if( (((x+1)*baseScalerTimeScale) % largestBaseTimeScale) == 0){
					int rebaseIndex = (x*baseScalerTimeScale)/largestBaseTimeScale;
					(scalerChannelRebaseVectors[channelString])[rebaseIndex] = scalerChannelRunningSums.value(channelString);
					scalerChannelRunningSums[channelString] = 0;
				}

				k++;
			}
		}
	}
	// Or just copy if necessary
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
	int amptekInitiateMovementIndex = 0;
	bool foundAmptekMovementStart = false;
	bool foundAmptekMovementEnd = false;
	if(!amptekDetectors_.isEmpty()){
		CLSAmptekSDD123DetectorNew *oneAmptekDetector = 0;
		CLSAmptekSDD123DetectorNew *highestAverageAmptekDetector = 0;
		AMDSClientDataRequest *oneAmptekDataRequest = clientDataRequestMap_.value(amptekDetectors_.first()->amdsBufferName());
		int amptekDataCount = oneAmptekDataRequest->data().count();
		AMDSDwellSpectralDataHolder *oneDataHolderAsDwellSpectral = 0;

		QMap<CLSAmptekSDD123DetectorNew*, QVector<int> > amptekGeneralPurposeCounterMaps;
		foreach(CLSAmptekSDD123DetectorNew* amptekDetector, amptekDetectors_)
			amptekGeneralPurposeCounterMaps.insert(amptekDetector, QVector<int>(amptekDataCount+1, 0));

		// Loop backwards from the end to find the start of the movement we're interested in
		for(int x = 0; x < amptekDataCount; x++){
			QString countString = QString();
			for(int y = 0, ySize = amptekDetectors_.count(); y < ySize; y++){
				oneAmptekDetector = amptekDetectors_.at(y);
				oneAmptekDataRequest = clientDataRequestMap_.value(oneAmptekDetector->amdsBufferName());
				if(oneAmptekDataRequest){
					oneDataHolderAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(oneAmptekDataRequest->data().at(x));
					if(oneDataHolderAsDwellSpectral){
						countString.append(QString("%1 ").arg(oneDataHolderAsDwellSpectral->dwellStatusData().generalPurposeCounter()));
						(amptekGeneralPurposeCounterMaps[oneAmptekDetector])[x] = oneDataHolderAsDwellSpectral->dwellStatusData().generalPurposeCounter();
						(amptekGeneralPurposeCounterMaps[oneAmptekDetector])[amptekDataCount] = (amptekGeneralPurposeCounterMaps[oneAmptekDetector])[amptekDataCount] + (amptekGeneralPurposeCounterMaps[oneAmptekDetector])[x];
					}
				}
			}
		}

		double highestAverageCount = 0;
		for(int y = 0, ySize = amptekDetectors_.count(); y < ySize; y++){
			oneAmptekDetector = amptekDetectors_.at(y);
			double oneAverageCount = (amptekGeneralPurposeCounterMaps[oneAmptekDetector])[amptekDataCount]/amptekDataCount;
			if(oneAverageCount > highestAverageCount){
				highestAverageCount = oneAverageCount;
				highestAverageAmptekDetector = oneAmptekDetector;
			}
		}

		QVector<int> highestAverageAmptekVector = amptekGeneralPurposeCounterMaps.value(highestAverageAmptekDetector);
		for(int x = highestAverageAmptekVector.count()-2; (x > 0) && !foundAmptekMovementStart; x--){
			int encoderPulsesInPeriod = highestAverageAmptekVector.at(x);
			if(!foundAmptekMovementEnd && encoderPulsesInPeriod > 20){
				qDebug() << "Found movement end at index " << x;
				foundAmptekMovementEnd = true;
			}
			else if(foundAmptekMovementEnd && encoderPulsesInPeriod < 1){
				foundAmptekMovementStart = true;
				amptekInitiateMovementIndex = x;
				qDebug() << "Found movement start at index " << amptekInitiateMovementIndex;
			}
		}
	}

	int scalerInitiateMovementIndex = 0;
	bool foundScalerMovementStart = false;
	bool foundScalerMovementEnd = false;

	QVector<qint32> encoderUpVector = scalerChannelRebaseVectors.value("EncoderUp");
	QVector<qint32> encoderDownVector = scalerChannelRebaseVectors.value("EncoderDown");

	if(double(continuousConfiguration_->scanAxes().at(0)->regionAt(0)->regionStart()) < double(continuousConfiguration_->scanAxes().at(0)->regionAt(0)->regionEnd())){
		for(int x = encoderUpVector.count()-1; (x > 0) && !foundScalerMovementStart; x--){
			if(!foundScalerMovementEnd && encoderUpVector.at(x) > 20){
				qDebug() << "Found scaler movement end at index " << x;
				foundScalerMovementEnd = true;
			}
			else if(foundScalerMovementEnd && encoderUpVector.at(x) < 1){
				foundScalerMovementStart = true;
				scalerInitiateMovementIndex = x;
				qDebug() << "Found scaler movement start index " << scalerInitiateMovementIndex;
			}
		}
	}
	else {
		for(int x = encoderDownVector.count()-1; (x > 0) && !foundScalerMovementStart; x--){
			if(!foundScalerMovementEnd && encoderDownVector.at(x) > 20){
				qDebug() << "Found scaler movement end at index " << x;
				foundScalerMovementEnd = true;
			}
			else if(foundScalerMovementEnd && encoderDownVector.at(x) < 1){
				foundScalerMovementStart = true;
				scalerInitiateMovementIndex = x;
				qDebug() << "Found scaler movement start index " << scalerInitiateMovementIndex;
			}
		}
	}

	qDebug() << "Amptek: " << amptekInitiateMovementIndex;
	qDebug() << "Scaler: " << scalerInitiateMovementIndex;
	// END OF STEP 4


	// STEP 5: Generate the Axis Values "In Order"
	if(!metaDataMap_.contains("GratingEncoderFeedback")){
		AMErrorMon::debug(this, AMCONTINUOUSSCANACTIONCONTROLLER_INITIAL_ENCODER_POSITION_MISSING, "Missing initial encoder position");
		return;
	}

	int startEncoderValue = (int)(metaDataMap_.value("GratingEncoderFeedback"));
	int currentEncoderValue = startEncoderValue;
	QVector<double> scalerEnergyFeedbacks = QVector<double>(encoderUpVector.count()-scalerInitiateMovementIndex+1);
	scalerEnergyFeedbacks[0] = SGMGratingSupport::energyFromGrating(SGMGratingSupport::LowGrating, startEncoderValue);

	// Loop from the start of the movement to the end and recreate the axis values (energy in this case) from the encoder pulse changes
	for(int x = scalerInitiateMovementIndex, size = encoderUpVector.count(); x < size; x++){
		currentEncoderValue += encoderUpVector.at(x) - encoderDownVector.at(x);
		scalerEnergyFeedbacks[x-scalerInitiateMovementIndex+1] = SGMGratingSupport::energyFromGrating(SGMGratingSupport::LowGrating, currentEncoderValue);
	}
	// END OF STEP 5


	// STEP 6: Place Data
	bool upScan = false;
	if(scalerEnergyFeedbacks.first() < scalerEnergyFeedbacks.last())
		upScan = true;

	AMDSLightWeightGenericFlatArrayDataHolder *dataHolderAsGenericFlatArrayDataHolder = 0;
	if(upScan){
		for(int x = scalerInitiateMovementIndex, size = encoderUpVector.count(); x < size; x++){

			scan_->rawData()->beginInsertRows(1, -1);
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), scalerEnergyFeedbacks.at(x-scalerInitiateMovementIndex+1));

			for(int y = 0, ySize = generalConfig_->detectorConfigurations().count(); y < ySize; y++){
				AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(y));

				asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
				if(!asScalerChannelDetector){
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value(oneDetector->amdsBufferName())->data().at(x));
					scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(oneDetector->name()), dataHolderAsGenericFlatArrayDataHolder->dataArray().asConstVectorDouble().constData());
				}
			}

			QMap<int, QString>::const_iterator i = scalerChannelIndexMap.constBegin();
			while(i != scalerChannelIndexMap.constEnd()){
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(i.value()), AMnDIndex(), scalerChannelRebaseVectors.value(i.value()).at(x));
				i++;
			}

			scan_->rawData()->endInsertRows();
			insertionIndex_[0] = insertionIndex_.i()+1;
		}
	}
	else{
		for(int x = encoderUpVector.count()-1; x >= scalerInitiateMovementIndex; x--){
			scan_->rawData()->beginInsertRows(1, -1);
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), scalerEnergyFeedbacks.at(x-scalerInitiateMovementIndex));

			for(int y = 0, ySize = generalConfig_->detectorConfigurations().count(); y < ySize; y++){
				AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(y));

				asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
				if(!asScalerChannelDetector){
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value(oneDetector->amdsBufferName())->data().at(x));
					scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(oneDetector->name()), dataHolderAsGenericFlatArrayDataHolder->dataArray().asConstVectorDouble().constData());
				}
			}

			QMap<int, QString>::const_iterator i = scalerChannelIndexMap.constBegin();
			while(i != scalerChannelIndexMap.constEnd()){
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(i.value()), AMnDIndex(), scalerChannelRebaseVectors.value(i.value()).at(x));
				i++;
			}

			scan_->rawData()->endInsertRows();
			insertionIndex_[0] = insertionIndex_.i()+1;
		}
	}
	// END OF STEP 6

	// STEP 7: Clean Up
	QMap<QString, AMDSClientDataRequest*>::const_iterator j = clientDataRequestMap_.constBegin();
	while(j != clientDataRequestMap_.constEnd()){
		j.value()->deleteLater();
		j++;
	}

	onScanningActionsSucceeded();
	// END OF STEP 7
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

