#include "SGMLineScanController.h"

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSAMDSScalerChannelDetector.h"

#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/DataHolder/AMDSGenericFlatArrayDataHolder.h"
#include "source/DataHolder/AMDSSpectralDataHolder.h"
#include "source/DataHolder/AMDSScalarDataHolder.h"

#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMHexapod.h"
#include "beamline/SGM/SGMHexapodTransformedAxis.h"

SGMLineScanController::SGMLineScanController(SGMLineScanConfiguration *configuration, QObject *parent)
	: AMGenericContinuousScanController(configuration, parent)
{

}

SGMLineScanController::~SGMLineScanController()
{

}

void SGMLineScanController::onAxisFinished()
{
	// PUT CODE IN HERE DAVE
	qDebug() << "Got the AxisFinished message";

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


	int baseScalerTimeScale = -1; //timescale in ms
	int baseAmptekTimeScale = -1; //timescale in ms

	if(requiredBufferNames.contains("Amptek SDD 240"))
		baseAmptekTimeScale = AMBeamline::bl()->exposedDetectorByName("AmptekSDD1")->amdsPollingBaseTimeMilliseconds();
	if(requiredBufferNames.contains("Scaler (BL1611-ID-1)"))
		baseScalerTimeScale = AMBeamline::bl()->exposedDetectorByName("TEY")->amdsPollingBaseTimeMilliseconds();

	int largestBaseTimeScale;
	if(baseScalerTimeScale < baseAmptekTimeScale)
		largestBaseTimeScale = baseAmptekTimeScale;
	else
		largestBaseTimeScale = baseScalerTimeScale;


	AMDSClientDataRequest *scalerClientDataRequst = clientDataRequestMap_.value("Scaler (BL1611-ID-1)");
	int totalCount = scalerClientDataRequst->data().count();
	int rebasedTotalCount = (totalCount*baseScalerTimeScale)/largestBaseTimeScale;
	qDebug() << "Original totalCount " << totalCount << " rebasedTotalCount " << rebasedTotalCount;

	QMap<QString, QVector<qint32> > scalerChannelVectors;
	QMap<QString, qint32> scalerChannelRunningSums;
	QMap<int, QString> scalerChannelIndexMap;

	CLSAMDSScalerChannelDetector *asScalerChannelDetector = 0;
	for(int x = 0, size = generalConfig_->detectorConfigurations().count(); x < size; x++){
		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(x));
		asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
		if(asScalerChannelDetector){
			scalerChannelIndexMap.insert(asScalerChannelDetector->enabledChannelIndex(), oneDetector->name());
			scalerChannelVectors.insert(oneDetector->name(), QVector<qint32>(rebasedTotalCount, 0));
			scalerChannelRunningSums.insert(oneDetector->name(), 0);
		}
	}

	// I think we can do a check against the number of actually enabled channels on the scaler ... just can't figure out a good way to get that value right now
//	AMDSLightWeightScalarDataHolder *asScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(scalerClientDataRequst->data().at(0));
//	if(asScalarDataHolder && asScalarDataHolder->dataArray().constVectorQint32().count() != ){
//		qDebug() << "FATAL ERROR, MISMATCH BETWEEN RECEIVED DATA SIZE OF SCALER AND REQUESTED SIZE. RECEIVED: " << asScalarDataHolder->dataArray().constVectorQint32().count() << "REQUESTED: " << scalerChannelVectors.count();
//		return;
//	}

	for(int x = 0; x < totalCount; x++){
		AMDSLightWeightScalarDataHolder *asScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(scalerClientDataRequst->data().at(x));
		if(asScalarDataHolder){
			QVector<qint32> oneVector = asScalarDataHolder->dataArray().constVectorQint32();

			int tempRunningSum;
			QString channelString;
			for(int y = 0, ySize = oneVector.count(); y < ySize; y++){
				channelString = scalerChannelIndexMap.value(y);
				tempRunningSum = scalerChannelRunningSums.value(channelString);
				tempRunningSum += oneVector.at(y);
				scalerChannelRunningSums[channelString] = tempRunningSum;

				if( (((x+1)*baseScalerTimeScale) % largestBaseTimeScale) == 0){
					int rebaseIndex = (x*baseScalerTimeScale)/largestBaseTimeScale;
					(scalerChannelVectors[channelString])[rebaseIndex] = scalerChannelRunningSums.value(channelString);
					scalerChannelRunningSums[channelString] = 0;
				}
			}
		}
	}

	AMDSClientDataRequest *oneAmptekDataRequest = clientDataRequestMap_.value("Amptek SDD 240");

	int initiateMovementIndex = 0;
	bool foundMovementStart = false;
	bool foundMovementEnd = false;

	// Loop backwards from the end to find the start of the movement we're interested in
	for(int x = oneAmptekDataRequest->data().count()-1; (x > 0) && !foundMovementStart; x--){
		AMDSDwellSpectralDataHolder *dataHolderAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(oneAmptekDataRequest->data().at(x));
		int encoderPulsesInPeriod = dataHolderAsDwellSpectral->dwellStatusData().generalPurposeCounter();
		if(!foundMovementEnd && encoderPulsesInPeriod > 5){
			qDebug() << "Found movement end at index " << x;
			foundMovementEnd = true;
		}
		else if(foundMovementEnd && encoderPulsesInPeriod < 1){
			foundMovementStart = true;
			initiateMovementIndex = x;
			qDebug() << "Found movement start at index " << initiateMovementIndex;
		}
	}


	int hexapodRedInitiateMovementIndex = 0;
	bool foundHexapodRedMovementStart = false;
	bool foundHexapodRedMovementEnd = false;
	QVector<qint32> hexapodRedVector = scalerChannelVectors.value("HexapodRed");
	for(int x = hexapodRedVector.count()-1; (x > 0) && !foundHexapodRedMovementStart; x--){
		if(!foundHexapodRedMovementEnd && hexapodRedVector.at(x) > 5){
			qDebug() << "Found scaler movement end at index " << x;
			foundHexapodRedMovementEnd = true;
		}
		else if(foundHexapodRedMovementEnd && hexapodRedVector.at(x) < 1){
			foundHexapodRedMovementStart = true;
			hexapodRedInitiateMovementIndex = x;
			qDebug() << "Found scaler movement start index " << hexapodRedInitiateMovementIndex;
		}
	}

	int hexapodBlackInitiateMovementIndex = 0;
	bool foundHexapodBlackMovementStart = false;
	bool foundHexapodBlackMovementEnd = false;
	QVector<qint32> hexapodBlackVector = scalerChannelVectors.value("HexapodBlack");
	for(int x = hexapodBlackVector.count()-1; (x > 0) && !foundHexapodBlackMovementStart; x--){
		if(!foundHexapodBlackMovementEnd && hexapodBlackVector.at(x) > 5){
			qDebug() << "Found scaler movement end at index " << x;
			foundHexapodBlackMovementEnd = true;
		}
		else if(foundHexapodBlackMovementEnd && hexapodBlackVector.at(x) < 1){
			foundHexapodBlackMovementStart = true;
			hexapodBlackInitiateMovementIndex = x;
			qDebug() << "Found scaler movement start index " << hexapodBlackInitiateMovementIndex;
		}
	}

	qDebug() << "Amptek: " << initiateMovementIndex;
	qDebug() << "Hexapod Red: " << hexapodRedInitiateMovementIndex;
	qDebug() << "Hexapod Black: " << hexapodBlackInitiateMovementIndex;

	QVector<double> globalXRecorderData = dataRecorderMap_.value("HexapodXRecorder");
	QVector<double> globalYRecorderData = dataRecorderMap_.value("HexapodYRecorder");
	QVector<double> globalZRecorderData = dataRecorderMap_.value("HexapodZRecorder");
	QList<QVector3D> globalCoordinateRecorderPositions;
	for(int x = 0, size = globalXRecorderData.count(); x < size; x++)
		globalCoordinateRecorderPositions.append(QVector3D(globalXRecorderData.at(x), globalYRecorderData.at(x), globalZRecorderData.at(x)));

	QList<QVector3D> primeCoordinateRecorderPositions = SGMBeamline::sgm()->hexapod()->transformVectors(globalCoordinateRecorderPositions);
	int axisIndex = 0;
	if(AMBeamline::bl()->exposedControlByInfo(configuration_->axisControlInfos().at(0)) == SGMBeamline::sgm()->hexapod()->yAxisPrimeControl())
		axisIndex = 1;
	else if(AMBeamline::bl()->exposedControlByInfo(configuration_->axisControlInfos().at(0)) == SGMBeamline::sgm()->hexapod()->zAxisPrimeControl())
		axisIndex = 2;

	AMDSLightWeightGenericFlatArrayDataHolder *dataHolderAsGenericFlatArrayDataHolder = 0;
	for(int x = hexapodRedInitiateMovementIndex, size = hexapodRedVector.count(); (x < size) && (x < 200); x++){

		scan_->rawData()->beginInsertRows(1, -1);
		if(axisIndex == 0)
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), primeCoordinateRecorderPositions.at(x-hexapodRedInitiateMovementIndex).x());
		else if(axisIndex == 1)
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), primeCoordinateRecorderPositions.at(x-hexapodRedInitiateMovementIndex).y());
		else
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), primeCoordinateRecorderPositions.at(x-hexapodRedInitiateMovementIndex).z());

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
			scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(i.value()), AMnDIndex(), scalerChannelVectors.value(i.value()).at(x));
			i++;
		}

		scan_->rawData()->endInsertRows();
		insertionIndex_[0] = insertionIndex_.i()+1;
	}
}

void SGMLineScanController::fillDataMaps(AMAgnosticDataAPIDataAvailableMessage *message)
{
	qDebug() << "Got a fillDataMaps call with " << message->messageType();
	// PLACE HOLDER CODE FOR DAVE
	if(message->uniqueID() == "HexapodXRecorder" || message->uniqueID() == "HexapodYRecorder" || message->uniqueID() == "HexapodZRecorder" || message->uniqueID() == "HexapodTimeRecorder"){
//		metaDataMap_.insert(message->uniqueID(), message->value("DetectorData").toList().at(0).toDouble());
		QVariantList localDetectorDataAsVariant = message->value("DetectorData").toList();
//		QVector<double> detectorData = QVector<double>(localDetectorDataAsVariant.count());
		QVector<double> detectorData = QVector<double>(HEXAPOD_RECORDER_POINTS_PER_MOVE);
		for(int x = 0, size = HEXAPOD_RECORDER_POINTS_PER_MOVE; x < size; x++)
			detectorData[x] = localDetectorDataAsVariant.at(x+1).toDouble();

		dataRecorderMap_.insert(message->uniqueID(), detectorData);
	}
	else{
		intptr_t dataRequestPointer = message->detectorDataAsAMDS();
		void *dataRequestVoidPointer = (void*)dataRequestPointer;
		AMDSClientDataRequest *dataRequest = static_cast<AMDSClientDataRequest*>(dataRequestVoidPointer);
		clientDataRequestMap_.insert(dataRequest->bufferName(), dataRequest);
	}
}

AMAction3 *SGMLineScanController::createInitializationActions()
{
	return 0;
}

AMAction3 *SGMLineScanController::createCleanupActions()
{
	return 0;
}

