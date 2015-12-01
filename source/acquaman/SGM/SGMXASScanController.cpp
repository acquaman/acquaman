#include "SGMXASScanController.h"

#include <QFile>

#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/energy/SGMEnergyControlSet.h"
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
	QTime startTime = QTime::currentTime();
	QTime stepTime  = QTime::currentTime();
	int currentStep = 1;
	// STEP 1: Data Checks & Meta Info Collection
	if(!generateAnalysisMetaInfo())
		return;
	// END OF STEP 1
	qDebug() << QString("[Step %1] Total Time: %2 Delta Time %3").arg(currentStep++).arg(startTime.msecsTo(QTime::currentTime())).arg(stepTime.msecsTo(QTime::currentTime()));
	stepTime = QTime::currentTime();

	// STEP 2: Retrieve and remap the scaler data into vectors for each channel
	if(!generateScalerMaps())
		return;

	QMap<QString, QVector<qint32> > scalerChannelVectors = SGMBeamline::sgm()->amdsScaler()->retrieveScalerData(scalerChannelIndexMap_, clientDataRequestMap_.value(SGMBeamline::sgm()->amdsScaler()->amdsBufferName()));
	// END OF STEP 2
	qDebug() << QString("[Step %1] Total Time: %2 Delta Time %3").arg(currentStep++).arg(startTime.msecsTo(QTime::currentTime())).arg(stepTime.msecsTo(QTime::currentTime()));
	stepTime = QTime::currentTime();

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
	qDebug() << QString("[Step %1] Total Time: %2 Delta Time %3").arg(currentStep++).arg(startTime.msecsTo(QTime::currentTime())).arg(stepTime.msecsTo(QTime::currentTime()));
	stepTime = QTime::currentTime();

	// STEP 4: Find Motion Start Indices
	if(!findStartMotionIndices())
		return;
	// END OF STEP 4
	qDebug() << QString("[Step %1] Total Time: %2 Delta Time %3").arg(currentStep++).arg(startTime.msecsTo(QTime::currentTime())).arg(stepTime.msecsTo(QTime::currentTime()));
	stepTime = QTime::currentTime();


	// STEP 5: Generate the Axis Values "In Order"
	if(!generateAxisFeedbackValues())
		return;
	// END OF STEP 5
	qDebug() << QString("[Step %1] Total Time: %2 Delta Time %3").arg(currentStep++).arg(startTime.msecsTo(QTime::currentTime())).arg(stepTime.msecsTo(QTime::currentTime()));
	stepTime = QTime::currentTime();


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
	qDebug() << QString("[Step %1] Total Time: %2 Delta Time %3").arg(currentStep++).arg(startTime.msecsTo(QTime::currentTime())).arg(stepTime.msecsTo(QTime::currentTime()));
	stepTime = QTime::currentTime();


	// STEP 7: Place Data
	if(!placeInterpolatedDataInDataStore())
		return;
	// END OF STEP 7
	qDebug() << QString("[Step %1] Total Time: %2 Delta Time %3").arg(currentStep++).arg(startTime.msecsTo(QTime::currentTime())).arg(stepTime.msecsTo(QTime::currentTime()));
	stepTime = QTime::currentTime();

	// STEP 8: Write Client Request to File
	QFile *clientDataRequestFile = new QFile(scan_->additionalFilePaths().first());
	qDebug() << "Going to save to " << scan_->additionalFilePaths().first();
	if(clientDataRequestFile->open(QIODevice::WriteOnly)) {
		QDataStream dataStream(clientDataRequestFile);

		quint16 numberOfRequests = clientDataRequestMap_.count();
		dataStream << numberOfRequests;

		AMDSClientDataRequest *oneClientRequest;
		QMap<QString, AMDSClientDataRequest*>::const_iterator j = clientDataRequestMap_.constBegin();
		while(j != clientDataRequestMap_.constEnd()){
			oneClientRequest = j.value();
			AMDSClientRequest::encodeAndwriteClientRequest(&dataStream, oneClientRequest);
			j++;
		}

	} else {
//		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_CLIENT_ERR_FAILED_TO_OPEN_TARGET_EXPORT_FILE, QString("AMDS client data request Failed to open target file %1 for writing.").arg(currentExportedFilePath_));
		clientDataRequestFile->close();
		clientDataRequestFile->deleteLater();
	}
	qDebug() << QString("[Step %1] Total Time: %2 Delta Time %3").arg(currentStep++).arg(startTime.msecsTo(QTime::currentTime())).arg(stepTime.msecsTo(QTime::currentTime()));
	stepTime = QTime::currentTime();

	// END OF STEP 8

	// STEP 9: Clean Up
	if(!cleanupClientDataRequests())
		return;
	// END OF STEP 9
	qDebug() << QString("[Step %1] Total Time: %2 Delta Time %3").arg(currentStep++).arg(startTime.msecsTo(QTime::currentTime())).arg(stepTime.msecsTo(QTime::currentTime()));
	stepTime = QTime::currentTime();

	/*
	QMap<QString, AMDSClientDataRequest*> reopenDataRequestMap;
	QFile *openDataFile = new QFile("/Users/chevrid/Desktop/AMDSClientReqeustData/tryit.amds");
	if(openDataFile->open(QIODevice::ReadOnly)) {
		QDataStream inDataStream(openDataFile);

		quint16 numberOfRequests;
		inDataStream >> numberOfRequests;

		AMDSClientRequest *oneClientRequest;
		for(quint16 x = 0, size = numberOfRequests; x < size; x++){
			oneClientRequest = AMDSClientRequest::decodeAndInstantiateClientRequest(&inDataStream);

			AMDSClientDataRequest *oneClientDataRequest = qobject_cast<AMDSClientDataRequest*>(oneClientRequest);
			if(oneClientDataRequest){
				reopenDataRequestMap.insert(oneClientDataRequest->bufferName(), oneClientDataRequest);
			}
		}

	} else {
//		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_CLIENT_ERR_FAILED_TO_OPEN_TARGET_EXPORT_FILE, QString("AMDS client data request Failed to open target file %1 for writing.").arg(currentExportedFilePath_));
		openDataFile->close();
		openDataFile->deleteLater();
	}
	*/

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
		SGMGratingSupport::GratingTranslation currentGratingTranslationChoice;
		if(SGMBeamline::sgm()->energyControlSet()->gratingTranslation()->withinTolerance(0.0))
			currentGratingTranslationChoice = SGMGratingSupport::LowGrating;
		else if(SGMBeamline::sgm()->energyControlSet()->gratingTranslation()->withinTolerance(1.0))
			currentGratingTranslationChoice = SGMGratingSupport::MediumGrating;
		else if(SGMBeamline::sgm()->energyControlSet()->gratingTranslation()->withinTolerance(2.0))
			currentGratingTranslationChoice = SGMGratingSupport::HighGrating;
		else
			return false;
		axisFeedbackValues_[x] = SGMGratingSupport::energyFromGrating(currentGratingTranslationChoice, currentEncoderValue);
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

