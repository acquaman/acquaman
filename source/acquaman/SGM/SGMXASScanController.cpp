#include "SGMXASScanController.h"

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSAMDSScalerChannelDetector.h"

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
	// STEP 1: Data Checks
	QList<QString> requiredBufferNames;
	for(int x = 0, size = generalConfig_->detectorConfigurations().count(); x < size; x++){
		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(x));

		if(!oneDetector->amdsBufferName().isEmpty() && !requiredBufferNames.contains(oneDetector->amdsBufferName()))
			requiredBufferNames.append(oneDetector->amdsBufferName());
	}

	for(int x = 0, size = requiredBufferNames.count(); x < size; x++){
		if(!clientDataRequestMap_.contains(requiredBufferNames.at(x))){
			AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_REQUIRED_DATA_MISSING, QString("Missing data %1").arg(requiredBufferNames.at(x)));
			return;
		}
	}
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
	int baseScalerTimeScale = -1; //timescale in ms
	int baseAmptekTimeScale = -1; //timescale in ms

	if(requiredBufferNames.contains("Amptek SDD 240") || requiredBufferNames.contains("Amptek SDD 241") || requiredBufferNames.contains("Amptek SDD 242") || requiredBufferNames.contains("Amptek SDD 243"))
		baseAmptekTimeScale = AMBeamline::bl()->exposedDetectorByName("AmptekSDD1")->amdsPollingBaseTimeMilliseconds();
	if(requiredBufferNames.contains("Scaler (BL1611-ID-1)"))
		baseScalerTimeScale = AMBeamline::bl()->exposedDetectorByName("TEY")->amdsPollingBaseTimeMilliseconds();

	int largestBaseTimeScale;
	if(baseScalerTimeScale < baseAmptekTimeScale)
		largestBaseTimeScale = baseAmptekTimeScale;
	else
		largestBaseTimeScale = baseScalerTimeScale;


	int rebasedTotalCount = (totalCount*baseScalerTimeScale)/largestBaseTimeScale;
	qDebug() << "Original totalCount " << totalCount << " rebasedTotalCount " << rebasedTotalCount;

	QMap<QString, QVector<qint32> > scalerChannelRebaseVectors;
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
	// END OF STEP 3

	AMDSClientDataRequest *oneAmptekDataRequest = clientDataRequestMap_.value("Amptek SDD 240");
	AMDSClientDataRequest *twoAmptekDataRequest = clientDataRequestMap_.value("Amptek SDD 241");
	AMDSClientDataRequest *threeAmptekDataRequest = clientDataRequestMap_.value("Amptek SDD 242");
	AMDSClientDataRequest *fourAmptekDataRequest = clientDataRequestMap_.value("Amptek SDD 243");
	AMDSClientDataRequest *anAmptekDataRequest = 0;

	if(oneAmptekDataRequest)
		anAmptekDataRequest= oneAmptekDataRequest;
	else if(twoAmptekDataRequest)
		anAmptekDataRequest= twoAmptekDataRequest;
	else if(threeAmptekDataRequest)
		anAmptekDataRequest= threeAmptekDataRequest;
	else if(fourAmptekDataRequest)
		anAmptekDataRequest= fourAmptekDataRequest;


	AMDSDwellSpectralDataHolder *dataHolderOneAsDwellSpectral;
	AMDSDwellSpectralDataHolder *dataHolderTwoAsDwellSpectral;
	AMDSDwellSpectralDataHolder *dataHolderThreeAsDwellSpectral;
	AMDSDwellSpectralDataHolder *dataHolderFourAsDwellSpectral;

	int initiateMovementIndex = 0;
	bool foundMovementStart = false;
	bool foundMovementEnd = false;

	if(anAmptekDataRequest){
		// Loop backwards from the end to find the start of the movement we're interested in
		for(int x = anAmptekDataRequest->data().count()-1; (x > 0) && !foundMovementStart; x--){
			QString countString;
			int encoderPulsesInPeriod = 0;
			if(oneAmptekDataRequest){
				dataHolderOneAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(oneAmptekDataRequest->data().at(x));
				countString.append(QString("%1 ").arg(dataHolderOneAsDwellSpectral->dwellStatusData().generalPurposeCounter()));
				if(dataHolderOneAsDwellSpectral->dwellStatusData().generalPurposeCounter() > encoderPulsesInPeriod)
					encoderPulsesInPeriod = dataHolderOneAsDwellSpectral->dwellStatusData().generalPurposeCounter();
			}
			if(twoAmptekDataRequest){
				dataHolderTwoAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(twoAmptekDataRequest->data().at(x));
				countString.append(QString("%1 ").arg(dataHolderTwoAsDwellSpectral->dwellStatusData().generalPurposeCounter()));
				if(dataHolderTwoAsDwellSpectral->dwellStatusData().generalPurposeCounter() > encoderPulsesInPeriod)
					encoderPulsesInPeriod = dataHolderTwoAsDwellSpectral->dwellStatusData().generalPurposeCounter();
			}
			if(threeAmptekDataRequest){
				dataHolderThreeAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(threeAmptekDataRequest->data().at(x));
				countString.append(QString("%1 ").arg(dataHolderThreeAsDwellSpectral->dwellStatusData().generalPurposeCounter()));
				if(dataHolderThreeAsDwellSpectral->dwellStatusData().generalPurposeCounter() > encoderPulsesInPeriod)
					encoderPulsesInPeriod = dataHolderThreeAsDwellSpectral->dwellStatusData().generalPurposeCounter();
			}
			if(fourAmptekDataRequest){
				dataHolderFourAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(fourAmptekDataRequest->data().at(x));
				countString.append(QString("%1 ").arg(dataHolderFourAsDwellSpectral->dwellStatusData().generalPurposeCounter()));
				if(dataHolderFourAsDwellSpectral->dwellStatusData().generalPurposeCounter() > encoderPulsesInPeriod)
					encoderPulsesInPeriod = dataHolderFourAsDwellSpectral->dwellStatusData().generalPurposeCounter();
			}

			//		int encoderPulsesInPeriod = dataHolderAsDwellSpectral->dwellStatusData().generalPurposeCounter();
//			int encoderPulsesInPeriod = dataHolderThreeAsDwellSpectral->dwellStatusData().generalPurposeCounter();
			qDebug() << "Amptek pulse at " << x << ": " << encoderPulsesInPeriod << countString;
			if(!foundMovementEnd && encoderPulsesInPeriod > 20){
				qDebug() << "Found movement end at index " << x;
				foundMovementEnd = true;
			}
			else if(foundMovementEnd && encoderPulsesInPeriod < 1){
				foundMovementStart = true;
				initiateMovementIndex = x;
				qDebug() << "Found movement start at index " << initiateMovementIndex;
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

	/* LEAVE THIS IN PLACE IN CASE WE HAVE TO GO BACK TO USING SCALER AS BASE TIME SCALE FOR EVERYTHING
	for(int x = encoderUpVector.count()-6; (x > 5) && !foundScalerMovementStart; x--){

		double runningAverage = (encoderUpVector.at(x+5)+encoderUpVector.at(x+4)+encoderUpVector.at(x+3)+encoderUpVector.at(x+2)+encoderUpVector.at(x+1)
					 +encoderUpVector.at(x)+encoderUpVector.at(x-1)+encoderUpVector.at(x-2)+encoderUpVector.at(x-3)+encoderUpVector.at(x-4))/10;

		if(!foundScalerMovementEnd && runningAverage > 1){
			qDebug() << "Found scaler movement end at index " << x;
			foundScalerMovementEnd = true;
		}
		else if(foundScalerMovementEnd && runningAverage < 0.1){
			foundScalerMovementStart = true;
			scalerInitiateMovementIndex = x;
			qDebug() << "Found scaler movement start index " << scalerInitiateMovementIndex;
		}
	}
	*/

	qDebug() << "Amptek: " << initiateMovementIndex;
	qDebug() << "Scaler: " << scalerInitiateMovementIndex;

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

	/* LEAVE THIS IN PLACE IN CASE WE HAVE TO GO BACK TO USING SCALER AS BASE TIME SCALE FOR EVERYTHING
	// Loop from the start of the movement to the end and place any data that we have into the actual rawData()
	for(int x = initiateMovementIndex, size = oneAmptekDataRequest->data().count(); x < size; x++){
		if(castToDwellSpectralHolder){
			dataHolderAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(oneAmptekDataRequest->data().at(x));
			dataHolderAsGenericFlatArrayDataHolder = dataHolderAsDwellSpectral;
		}
		else if(castToGenericFlatArrayHolder){
			dataHolderAsDwellSpectral = 0;
			dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(oneAmptekDataRequest->data().at(x));
		}

		scan_->rawData()->beginInsertRows(1, -1);
		scan_->rawData()->setAxisValue(0, insertionIndex_.i(), energyFeedbacks.at(x-initiateMovementIndex+1));
		scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("I0"), AMnDIndex(), dataHolderAsDwellSpectral->dwellStatusData().generalPurposeCounter());
		scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("AmptekSDD1"), dataHolderAsGenericFlatArrayDataHolder->dataArray().constVectorDouble().constData());

		dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value("Amptek SDD 241")->data().at(x));
		if(dataHolderAsGenericFlatArrayDataHolder)
			scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("AmptekSDD2"), dataHolderAsGenericFlatArrayDataHolder->dataArray().constVectorDouble().constData());
		dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value("Amptek SDD 242")->data().at(x));
		if(dataHolderAsGenericFlatArrayDataHolder)
			scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("AmptekSDD3"), dataHolderAsGenericFlatArrayDataHolder->dataArray().constVectorDouble().constData());
		dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value("Amptek SDD 243")->data().at(x));
		if(dataHolderAsGenericFlatArrayDataHolder)
			scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("AmptekSDD4"), dataHolderAsGenericFlatArrayDataHolder->dataArray().constVectorDouble().constData());

		//					scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("TEY"), AMnDIndex(), teyVector.at(x*50));
		int energyLookupIndex = (x-initiateMovementIndex)*50;
		qDebug() << energyLookupIndex << scalerEnergyFeedbacks.count();
		if(energyLookupIndex < scalerEnergyFeedbacks.count())
			scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("TEY"), AMnDIndex(), scalerEnergyFeedbacks.at(energyLookupIndex));
		else
			scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("TEY"), AMnDIndex(), scalerEnergyFeedbacks.at(scalerEnergyFeedbacks.count()-1));
		scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("TFY"), AMnDIndex(), tfyVector.at(x*50));
		scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("PD"), AMnDIndex(), pdVector.at(x*50));

		scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("FilteredPD1"), AMnDIndex(), fpd1Vector.at(x*50));
		scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("FilteredPD2"), AMnDIndex(), fpd2Vector.at(x*50));
		scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("FilteredPD3"), AMnDIndex(), fpd3Vector.at(x*50));
		scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("FilteredPD4"), AMnDIndex(), fpd4Vector.at(x*50));

		scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("HexapodRed"), AMnDIndex(), hexapodRedVector.at(x*50));
		scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("HexapodBlack"), AMnDIndex(), hexapodBlackVector.at(x*50));
		scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("EncoderUp"), AMnDIndex(), encoderUpVector.at(x*50));
		scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("EncoderDown"), AMnDIndex(), encoderDownVector.at(x*50));


		scan_->rawData()->endInsertRows();
		insertionIndex_[0] = insertionIndex_.i()+1;
	}
	*/


	QMap<QString, AMDSClientDataRequest*>::const_iterator j = clientDataRequestMap_.constBegin();
	while(j != clientDataRequestMap_.constEnd()){
		j.value()->deleteLater();
		j++;
	}

	onScanningActionsSucceeded();
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

