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
	QVector<double> betterScalerEnergyFeedbacks = QVector<double>(expectedDurationScaledToBaseTimeScale);
	for(int x = 0, size = betterScalerEnergyFeedbacks.count(); x < size; x++){
		currentEncoderValue += encoderUpVector.at(x+scalerInitiateMovementIndex) - encoderDownVector.at(x+scalerInitiateMovementIndex);
		betterScalerEnergyFeedbacks[x] = SGMGratingSupport::energyFromGrating(SGMGratingSupport::LowGrating, currentEncoderValue);
	}

	// END OF STEP 5

	// STEP 6: Place Data
	bool upScan = false;
	if(betterScalerEnergyFeedbacks.first() < betterScalerEnergyFeedbacks.last())
		upScan = true;

	AMDSLightWeightGenericFlatArrayDataHolder *dataHolderAsGenericFlatArrayDataHolder = 0;
	if(upScan){
		for(int x = 0, size = betterScalerEnergyFeedbacks.count(); x < size; x++){
			scan_->rawData()->beginInsertRows(1, -1);
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), betterScalerEnergyFeedbacks.at(x));

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
		for(int x = betterScalerEnergyFeedbacks.count()-1; x >= 0; x--){
			scan_->rawData()->beginInsertRows(1, -1);
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), betterScalerEnergyFeedbacks.at(x));

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

