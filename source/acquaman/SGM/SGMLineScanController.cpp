#include "SGMLineScanController.h"

#include <stdint.h>

#include "beamline/SGM/SGMBeamline.h"
#include "beamline/CLS/CLSAMDSScalerChannelDetector.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"

#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/DataHolder/AMDSGenericFlatArrayDataHolder.h"
#include "source/DataHolder/AMDSSpectralDataHolder.h"
#include "source/DataHolder/AMDSScalarDataHolder.h"

#include "beamline/SGM/SGMBeamline.h"
#include "beamline/CLS/CLSAMDSScaler.h"
#include "beamline/SGM/SGMHexapod.h"
#include "beamline/SGM/SGMHexapodTransformedAxis.h"

SGMLineScanController::SGMLineScanController(SGMLineScanConfiguration *configuration, QObject *parent)
	: SGMContinuousScanController(configuration, parent)
{

}

SGMLineScanController::~SGMLineScanController()
{

}

void SGMLineScanController::onAxisFinished()
{
	// STEP 1: Data Checks & Meta Info Collection
	if(!generateAnalysisMetaInfo()){

		AMErrorMon::error(this,
				  SGMLINESCANCONTROLLER_COULD_NOT_GENERATE_META_INFO,
				  QString("%1 could not generate the necessary meta information to get the data.").arg(continuousConfiguration_->name()));
		setFailed();
		return;
	}
	// END OF STEP 1

	// STEP 2: Retrieve and remap the scaler data into vectors for each channel
	if(!generateScalerMaps()){

		AMErrorMon::error(this,
				  SGMLINESCANCONTROLLER_COULD_NOT_GENERATE_SCALER_MAPS,
				  QString("%1 could not generate the scaler maps to retrieve scaler data.").arg(continuousConfiguration_->name()));
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
	int axisIndex = 0;
	if(AMBeamline::bl()->exposedControlByInfo(configuration_->axisControlInfoAt(0)) == SGMBeamline::sgm()->hexapod()->yAxisPrimeControl())
		axisIndex = 1;
	else if(AMBeamline::bl()->exposedControlByInfo(configuration_->axisControlInfoAt(0)) == SGMBeamline::sgm()->hexapod()->zAxisPrimeControl())
		axisIndex = 2;
	// END OF STEP 5


	// STEP 6: Place Data
	CLSAMDSScalerChannelDetector *asScalerChannelDetector = 0;
	AMDSLightWeightGenericFlatArrayDataHolder *dataHolderAsGenericFlatArrayDataHolder = 0;
	for(int x = scalerInitiateMovementIndex, size = hexapodRedVector.count(); (x < size) && (x < 200+scalerInitiateMovementIndex); x++){

		scan_->rawData()->beginInsertRows(1, -1);
		if(axisIndex == 0)
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), primeCoordinateRecorderPositions.at(x-scalerInitiateMovementIndex).x());
		else if(axisIndex == 1)
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), primeCoordinateRecorderPositions.at(x-scalerInitiateMovementIndex).y());
		else
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), primeCoordinateRecorderPositions.at(x-scalerInitiateMovementIndex).z());

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

		scan_->rawData()->endInsertRows();
		insertionIndex_[0] = insertionIndex_.i()+1;
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

void SGMLineScanController::fillDataMaps(AMAgnosticDataAPIDataAvailableMessage *message)
{
	qDebug() << "Got a fillDataMaps call with " << message->messageType();

	if(message->uniqueID() == "HexapodXRecorder"
			|| message->uniqueID() == "HexapodYRecorder"
			|| message->uniqueID() == "HexapodZRecorder"
			|| message->uniqueID() == "HexapodTimeRecorder"){

		QVariantList localDetectorDataAsVariant = message->value("DetectorData").toList();
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

