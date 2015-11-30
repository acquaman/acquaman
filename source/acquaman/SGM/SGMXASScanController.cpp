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

	if(!amptekDetectors_.isEmpty() && !generateInterpolatedAmptekVectors())
			return;
	// END OF STEP 6


	// STEP 7: Place Data
	if(!placeInterpolatedDataInDataStore())
		return;
	// END OF STEP 7

	// STEP 8: Clean Up
	if(!cleanupClientDataRequests())
		return;

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

