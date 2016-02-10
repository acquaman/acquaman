#include "SGMMapScanController.h"

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSAMDSScalerChannelDetector.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"

#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/DataHolder/AMDSGenericFlatArrayDataHolder.h"
#include "source/DataHolder/AMDSSpectralDataHolder.h"
#include "source/DataHolder/AMDSScalarDataHolder.h"

#include "beamline/CLS/CLSAMDSScaler.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMHexapod.h"
#include "beamline/SGM/SGMHexapodTransformedAxis.h"

SGMMapScanController::SGMMapScanController(SGMMapScanConfiguration *configuration, QObject *parent)
	: SGMContinuousScanController(configuration, parent)
{

}

SGMMapScanController::~SGMMapScanController()
{

}

void SGMMapScanController::onAxisFinished()
{
	// PUT CODE IN HERE DARREN
	// Nuuuuuuu why me?!?!?!?!?
	// PUT CODE IN HERE DAVE
	// STEP 1: Data Checks & Meta Info Collection
	if(!generateAnalysisMetaInfo()){

		AMErrorMon::error(this, 676800, "Could not generate analysis meta info.");
		setFailed();
		return;
	}
	// END OF STEP 1

	// STEP 2: Retrieve and remap the scaler data into vectors for each channel
	if(!generateScalerMaps()){

		AMErrorMon::error(this, 676801, "Could not generate scaler maps.");
		setFailed();
		return;
	}

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
	int expectedDuration = double(continuousConfiguration_->scanAxes().at(0)->regionAt(0)->regionTime())*1000/largestBaseTimeScale;

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
		amptekRangeData = highestAverageAmptekDetector->retrieveContinuousMotionRangeData(amptekBaseData, expectedDuration, 5);
	}


	int scalerInitiateMovementIndex = 0;
	QList<QVector<qint32> > scalerBaseData;
	scalerBaseData << scalerChannelRebaseVectors.value("HexapodRed") << scalerChannelRebaseVectors.value("HexapodBlack");
	AMDetectorContinuousMotionRangeData scalerRangeData = scalerChannelDetectors_.first()->retrieveContinuousMotionRangeData(scalerBaseData, expectedDuration, 5);
	if(scalerRangeData.isValid())
		scalerInitiateMovementIndex = scalerRangeData.motionStartIndex();

	qDebug() << "Amptek: " << amptekRangeData.motionStartIndex() << amptekRangeData.motionEndIndex() << amptekRangeData.listIndex();
	qDebug() << "Hexapod Scaler: " << scalerRangeData.motionStartIndex() << scalerRangeData.motionEndIndex() << scalerRangeData.listIndex();

	QVector<qint32> hexapodRedVector = scalerChannelRebaseVectors.value("HexapodRed");
	// END OF STEP 4

	// STEP 5: Generate the Axis Values "In Order"
	QVector<double> globalXRecorderData = dataRecorderMap_.value("HexapodXRecorder");
	QVector<double> globalYRecorderData = dataRecorderMap_.value("HexapodYRecorder");
	QVector<double> globalZRecorderData = dataRecorderMap_.value("HexapodZRecorder");
	QList<QVector3D> globalCoordinateRecorderPositions;
	for(int x = 0, size = globalXRecorderData.count(); x < size; x++)
		globalCoordinateRecorderPositions.append(QVector3D(globalXRecorderData.at(x), globalYRecorderData.at(x), globalZRecorderData.at(x)));

	QList<QVector3D> primeCoordinateRecorderPositions = SGMBeamline::sgm()->hexapod()->transformVectors(globalCoordinateRecorderPositions);
	int horizontalAxisIndex = 0;
	if(AMBeamline::bl()->exposedControlByInfo(configuration_->axisControlInfos().at(0)) == SGMBeamline::sgm()->hexapod()->yAxisPrimeControl())
		horizontalAxisIndex = 1;
	else if(AMBeamline::bl()->exposedControlByInfo(configuration_->axisControlInfos().at(0)) == SGMBeamline::sgm()->hexapod()->zAxisPrimeControl())
		horizontalAxisIndex = 2;

	int verticalAxisIndex = 0;
	if(AMBeamline::bl()->exposedControlByInfo(configuration_->axisControlInfos().at(1)) == SGMBeamline::sgm()->hexapod()->yAxisPrimeControl())
		verticalAxisIndex = 1;
	else if(AMBeamline::bl()->exposedControlByInfo(configuration_->axisControlInfos().at(1)) == SGMBeamline::sgm()->hexapod()->zAxisPrimeControl())
		verticalAxisIndex = 2;

	// END OF STEP 5

	// STEP 6: Place Data
	CLSAMDSScalerChannelDetector *asScalerChannelDetector = 0;
	AMDSLightWeightGenericFlatArrayDataHolder *dataHolderAsGenericFlatArrayDataHolder = 0;

	if (insertionIndex_.i() == 0 && insertionIndex_.j() == 0){

		scan_->rawData()->beginInsertRows(200, -1);
		AMScanAxis *axis1 = continuousConfiguration_->scanAxisAt(0);
		AMScanAxis *axis2 = continuousConfiguration_->scanAxisAt(1);
		double axis1Step = (double(axis1->axisEnd())-double(axis1->axisStart()))/200;
		double axis2Step = double(axis2->regionAt(0)->regionStep());
		for (int j = 0; j < axis2->numberOfPoints(); j++){

			for (int i = 0; i < 200; i++){

				AMnDIndex insertIndex = AMnDIndex(i, j);
				scan_->rawData()->setAxisValue(0, insertIndex.i(), double(axis1->axisStart()) + i*axis1Step);
				scan_->rawData()->setAxisValue(1, insertIndex.j(), double(axis2->axisStart()) + j*axis2Step);
			}
		}

		scan_->rawData()->endInsertRows();
	}


	for(int x = scalerInitiateMovementIndex, size = hexapodRedVector.count(); (x < size) && (x < 200); x++){

		if (insertionIndex_.i() == 0){
//			if(horizontalAxisIndex == 0)
//				scan_->rawData()->setAxisValue(0, insertionIndex_.i(), primeCoordinateRecorderPositions.at(x-scalerInitiateMovementIndex).x());
//			else if(horizontalAxisIndex == 1)
//				scan_->rawData()->setAxisValue(0, insertionIndex_.i(), primeCoordinateRecorderPositions.at(x-scalerInitiateMovementIndex).y());
//			else
//				scan_->rawData()->setAxisValue(0, insertionIndex_.i(), primeCoordinateRecorderPositions.at(x-scalerInitiateMovementIndex).z());

//			if(verticalAxisIndex == 0)
//				scan_->rawData()->setAxisValue(1, insertionIndex_.j(), primeCoordinateRecorderPositions.at(scalerInitiateMovementIndex).x());
//			else if(verticalAxisIndex == 1)
//				scan_->rawData()->setAxisValue(1, insertionIndex_.j(), primeCoordinateRecorderPositions.at(scalerInitiateMovementIndex).y());
//			else
//				scan_->rawData()->setAxisValue(1, insertionIndex_.j(), primeCoordinateRecorderPositions.at(scalerInitiateMovementIndex).z());
		}

		for(int y = 0, ySize = generalConfig_->detectorConfigurations().count(); y < ySize; y++){
			AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(y));

			asScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
			if(!asScalerChannelDetector){
				dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value(oneDetector->amdsBufferName())->data().at(x));
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(oneDetector->name()), dataHolderAsGenericFlatArrayDataHolder->dataArray().asConstVectorDouble().constData());
			}
		}

		QMap<int, QString>::const_iterator i = scalerChannelIndexMap_.constBegin();
		while(i != scalerChannelIndexMap_.constEnd()){
			scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(i.value()), AMnDIndex(), scalerChannelRebaseVectors.value(i.value()).at(x));
			i++;
		}

		insertionIndex_[0] = insertionIndex_.i()+1;
	}

	insertionIndex_[0] = 0;
	insertionIndex_[1]++;

	// END OF STEP 6

	// STEP 7: Clean Up
	QMap<QString, AMDSClientDataRequest*>::const_iterator j = clientDataRequestMap_.constBegin();
	while(j != clientDataRequestMap_.constEnd()){
		j.value()->deleteLater();
		j++;
	}

	if (configuration_->scanAxisAt(1)->numberOfPoints() == insertionIndex_.j())
		onScanningActionsSucceeded();
	// END OF STEP 7
}

void SGMMapScanController::fillDataMaps(AMAgnosticDataAPIDataAvailableMessage *message)
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

AMAction3 *SGMMapScanController::createInitializationActions()
{
	return 0;
}

AMAction3 *SGMMapScanController::createCleanupActions()
{
	return 0;
}

