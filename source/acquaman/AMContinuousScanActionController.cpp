#include "AMContinuousScanActionController.h"

#include <QFile>
#include <QFileInfo>
#include <QStringBuilder>

#include "application/AMAppController.h"
#include "application/AMAppControllerSupport.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSAMDSScalerChannelDetector.h"
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "dataman/datastore/AMCDFDataStore.h"

#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/DataHolder/AMDSScalarDataHolder.h"
#include "source/DataHolder/AMDSGenericFlatArrayDataHolder.h"
#include "source/DataHolder/AMDSSpectralDataHolder.h"
#include <stdint.h>

#include <QDebug>

AMContinuousScanActionController::AMContinuousScanActionController(AMContinuousScanConfiguration *configuration, QObject *parent)
	: AMScanActionController(configuration, parent)
{
	continuousConfiguration_ = configuration;
	insertionIndex_ = AMnDIndex(continuousConfiguration_->scanAxes().size(), AMnDIndex::DoInit, 0);

	connect(this, SIGNAL(started()), this, SLOT(onScanningActionsStarted()));
}

AMContinuousScanActionController::~AMContinuousScanActionController()
{

}

void AMContinuousScanActionController::buildScanController()
{
	// Build the scan assembler.
	createScanAssembler();
	// Create the axis order map for higher dimensional scans.
	createAxisOrderMap();

	// Setup all the axes.
	for (int i = 0, axisCount = scan_->rawData()->scanAxesCount(); i < axisCount; i++){

		continuousConfiguration_->scanAxisAt(i)->setName(scan_->rawData()->scanAxisAt(i).name);

		for (int j = 0, regionCount = continuousConfiguration_->scanAxisAt(i)->regionCount(); j < regionCount; j++)
			continuousConfiguration_->scanAxisAt(i)->regionAt(j)->setName(QString("%1 %2 %3").arg(scan_->rawData()->scanAxisAt(i).name).arg("region").arg(j+1));
	}

	// Configure the scan assemblers axes.
	qDebug() << "Scan axes count = " << scan_->rawData()->scanAxesCount();
	for (int i = 0, axisCount = scan_->rawData()->scanAxesCount(); i < axisCount; i++){

		int actualAxis = axisOrderMap_.value(scan_->rawData()->scanAxisAt(i).name);
		scanAssembler_->insertAxis(i, AMBeamline::bl()->exposedControlByInfo(continuousConfiguration_->axisControlInfos().at(actualAxis)), continuousConfiguration_->scanAxisAt(actualAxis));
		qDebug() << "Adding axis " << continuousConfiguration_->axisControlInfos().at(i).name();
		qDebug() << "Start: " << double(continuousConfiguration_->scanAxisAt(i)->axisStart());
	}

	// Add all the detectors.
	for (int i = 0, size = continuousConfiguration_->detectorConfigurations().count(); i < size; i++){

		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(continuousConfiguration_->detectorConfigurations().at(i));

		if (oneDetector && !scanAssembler_->addDetector(oneDetector)){

			AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_COULD_NOT_ADD_DETECTOR, QString("Could not add the following detector to the assembler: %1").arg(oneDetector != 0 ? oneDetector->name() : "Not found"));
			return;
		}
	}

	// Handle some general scan stuff, including setting the default file path.
	scan_->setRunId(AMUser::user()->currentRunId());

	bool has1DDetectors = false;

	for (int i = 0, size = continuousConfiguration_->detectorConfigurations().count(); i < size && !has1DDetectors; i++){

		AMDetector *detector = AMBeamline::bl()->exposedDetectorByInfo(continuousConfiguration_->detectorConfigurations().at(i));

		if (detector && detector->rank() == 1)
			has1DDetectors = true;
	}

	QFileInfo fullPath(AMUserSettings::defaultRelativePathForScan(QDateTime::currentDateTime()));	// ex: 2010/09/Mon_03_12_24_48_0000   (Relative, and with no extension)

	// If you want to use the CDF data file format.
	if (scan_->fileFormat() == "amCDFv1"){

		// Get all the scan axes so they can be added to the new data store.
		QList<AMAxisInfo> scanAxes;

		for (int i = 0, size = scan_->rawData()->scanAxesCount(); i < size; i++)
			scanAxes << scan_->rawData()->scanAxisAt(i);

		scan_->setFilePath(fullPath.filePath() % ".cdf");
		scan_->replaceRawDataStore(new AMCDFDataStore(AMUserSettings::userDataFolder % scan_->filePath(), false));

		// Add all the old scan axes.
		foreach (AMAxisInfo axis, scanAxes)
			scan_->rawData()->addScanAxis(axis);

		connect(this, SIGNAL(started()), this, SLOT(flushCDFDataStoreToDisk()));
		connect(this, SIGNAL(cancelled()), this, SLOT(flushCDFDataStoreToDisk()));
		connect(this, SIGNAL(failed()), this, SLOT(flushCDFDataStoreToDisk()));
		connect(this, SIGNAL(finished()), this, SLOT(flushCDFDataStoreToDisk()));
	}

	// Get all the detectors added to the scan.
	for (int i = 0, size = continuousConfiguration_->detectorConfigurations().count(); i < size; i++){

		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(continuousConfiguration_->detectorConfigurations().at(i));

		if(oneDetector && scan_->rawData()->addMeasurement(AMMeasurementInfo(*oneDetector)))
			scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1), oneDetector->isVisible(), oneDetector->hiddenFromUsers());
	}

	connect(scanAssembler_, SIGNAL(actionTreeGenerated(AMAction3*)), this, SLOT(onScanningActionsGenerated(AMAction3*)));

	if (scanAssembler_->generateActionTree())
		buildScanControllerImplementation();

	else
		setFailed();
}

void AMContinuousScanActionController::flushCDFDataStoreToDisk()
{
	AMCDFDataStore *dataStore = qobject_cast<AMCDFDataStore *>(scan_->rawData());
	if(dataStore && !dataStore->flushToDisk())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, 38, "Error saving the currently-running scan's raw data file to disk. Watch out... your data may not be saved! Please report this bug to your beamline's software developers."));
}

void AMContinuousScanActionController::onScanningActionsStarted()
{
	disconnect(scanningActions_, SIGNAL(succeeded()), this, SLOT(onScanningActionsSucceeded()));
}

bool AMContinuousScanActionController::event(QEvent *e)
{
	if (e->type() == (QEvent::Type)AMAgnosticDataAPIDefinitions::MessageEvent){

		AMAgnosticDataAPIMessage message = ((AMAgnositicDataEvent*)e)->message_;

		switch(message.messageType()){

		case AMAgnosticDataAPIDefinitions::AxisStarted:{

			break;}

		case AMAgnosticDataAPIDefinitions::AxisFinished:{

			onAxisFinished();
			flushCDFDataStoreToDisk();

			break;
		}

		case AMAgnosticDataAPIDefinitions::AxisValueFinished:

			// Not relevant for continuous scans.
			break;

		case AMAgnosticDataAPIDefinitions::DataAvailable:{

			AMAgnosticDataAPIDataAvailableMessage *dataAvailableMessage = static_cast<AMAgnosticDataAPIDataAvailableMessage*>(&message);
			fillDataMaps(dataAvailableMessage);
			flushCDFDataStoreToDisk();

			break;}

		case AMAgnosticDataAPIDefinitions::ControlMoved:

			// Not relevant for continuous scans.
			break;

		case AMAgnosticDataAPIDefinitions::InvalidMessage:
			break;

		default:
			break;
		}

		e->accept();
		return true;
	}

	else
		return AMScanActionController::event(e);
}

void AMContinuousScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMGenericScanActionControllerAssembler(continuousConfiguration_->automaticDirectionAssessment(), continuousConfiguration_->direction(), this);
}

void AMContinuousScanActionController::createAxisOrderMap()
{
	if (scan_->scanRank() == 1){

		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 0);
	}

	else if (scan_->scanRank() == 2){

		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(0).name, 1);
		axisOrderMap_.insert(scan_->rawData()->scanAxisAt(1).name, 0);
	}
}

void AMContinuousScanActionController::onAxisFinished()
{

}

void AMContinuousScanActionController::fillDataMaps(AMAgnosticDataAPIDataAvailableMessage *message)
{
	Q_UNUSED(message)
}

bool AMContinuousScanActionController::generateAnalysisMetaInfo()
{
	CLSAMDSScalerChannelDetector *tempScalerChannelDetector = 0;
	CLSAmptekSDD123DetectorNew *tempAmptekDetector = 0;

	for(int x = 0, size = generalConfig_->detectorConfigurations().count(); x < size; x++){
		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(x));

		if(!oneDetector->amdsBufferName().isEmpty() && !requiredBufferNames_.contains(oneDetector->amdsBufferName()))
			requiredBufferNames_.append(oneDetector->amdsBufferName());

		tempScalerChannelDetector = qobject_cast<CLSAMDSScalerChannelDetector*>(oneDetector);
		if(tempScalerChannelDetector){
			timeScales_.append(tempScalerChannelDetector->amdsPollingBaseTimeMilliseconds());
			scalerChannelDetectors_.append(tempScalerChannelDetector);
		}

		tempAmptekDetector = qobject_cast<CLSAmptekSDD123DetectorNew*>(oneDetector);
		if(tempAmptekDetector){
			timeScales_.append(tempAmptekDetector->amdsPollingBaseTimeMilliseconds());
			amptekDetectors_.append(tempAmptekDetector);
		}
	}

	// Always add a sensible value of 25ms ... any more time resolution and even quick scans (~10s) are too many points
	timeScales_.append(25);

	for(int x = 0, size = requiredBufferNames_.count(); x < size; x++){
		if(!clientDataRequestMap_.contains(requiredBufferNames_.at(x))){
			AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_REQUIRED_DATA_MISSING, QString("Missing data %1").arg(requiredBufferNames_.at(x)));
			return false;
		}
	}

	return true;
}

bool AMContinuousScanActionController::generateScalerMaps()
{
	AMDSClientDataRequest *scalerClientDataRequest = clientDataRequestMap_.value("Scaler (BL1611-ID-1)");
	if(!scalerClientDataRequest){
		AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_REQUIRED_DATA_MISSING, QString("Missing scaler data for continuous scan processing."));
		return false;
	}

	scalerTotalCount_ = scalerClientDataRequest->data().count();

	// Set up maps
	foreach(CLSAMDSScalerChannelDetector *scalerChannelDetector, scalerChannelDetectors_)
		scalerChannelIndexMap_.insert(scalerChannelDetector->enabledChannelIndex(), scalerChannelDetector->name());

	// Check data holder casts to correct type
	qDebug() << "Total scaler data holders" << scalerTotalCount_ << ((AMDSClientRelativeCountPlusCountDataRequest*)scalerClientDataRequest)->relativeCount() << ((AMDSClientRelativeCountPlusCountDataRequest*)scalerClientDataRequest)->count();
	AMDSLightWeightScalarDataHolder *asScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(scalerClientDataRequest->data().at(0));
	if(!asScalarDataHolder){
		AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_BAD_SCALER_DATAHOLDER_TYPE, QString("Scaler data holder is the wrong type."));
		return false;
	}
	// Check that we won't index into more positions than the AMDS-Scaler returned
	if(scalerChannelIndexMap_.count() > asScalarDataHolder->dataArray().constVectorQint32().count()){
		AMErrorMon::alert(this, AMCONTINUOUSSCANACTIONCONTROLLER_SCALER_CHANNEL_MISMATCH, QString("There is a mismatch between the number of enabled scaler channels and the number requested."));
		return false;
	}

	return true;
}

bool AMContinuousScanActionController::findStartMotionIndices()
{
	expectedDurationScaledToBaseTimeScale_ = double(continuousConfiguration_->scanAxes().at(0)->regionAt(0)->regionTime())*1000/largestBaseTimeScale_;

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
		amptekRangeData = highestAverageAmptekDetector->retrieveContinuousMotionRangeData(amptekBaseData, expectedDurationScaledToBaseTimeScale_, 20);
	}


	int scalerInitiateMovementIndex = 0;
	QList<QVector<qint32> > scalerBaseData;
	scalerBaseData << scalerChannelRebaseVectors_.value("EncoderUp") << scalerChannelRebaseVectors_.value("EncoderDown");
	AMDetectorContinuousMotionRangeData scalerRangeData = scalerChannelDetectors_.first()->retrieveContinuousMotionRangeData(scalerBaseData, expectedDurationScaledToBaseTimeScale_, 20);
	if(scalerRangeData.isValid()){
		scalerInitiateMovementIndex = scalerRangeData.motionStartIndex();
	}


	// Not general yet ... need to pass in which channel we should look in for scaler movement data
	encoderUpVector_ = scalerChannelRebaseVectors_.value("EncoderUp");
	encoderDownVector_ = scalerChannelRebaseVectors_.value("EncoderDown");

	if( (encoderUpVector_.count()-scalerInitiateMovementIndex) < expectedDurationScaledToBaseTimeScale_){
		qDebug() << "Scaler start movment index was out of bounds at " << scalerInitiateMovementIndex;
		scalerInitiateMovementIndex += ((encoderUpVector_.count()-scalerInitiateMovementIndex)-expectedDurationScaledToBaseTimeScale_);
		scalerRangeData.setMotionStartIndex(scalerInitiateMovementIndex);
		qDebug() << "Changed to " << scalerInitiateMovementIndex;
	}
	if(!amptekDetectors_.isEmpty()){
		AMDSClientDataRequest *oneAmptekRequest = clientDataRequestMap_.value(amptekDetectors_.first()->amdsBufferName());
		if( (oneAmptekRequest->data().count() - amptekRangeData.motionStartIndex()) < expectedDurationScaledToBaseTimeScale_){
			qDebug() << "Amptek start movment index was out of bounds at " << amptekRangeData.motionStartIndex();
			amptekRangeData.setMotionStartIndex( ((oneAmptekRequest->data().count() - amptekRangeData.motionStartIndex()) - expectedDurationScaledToBaseTimeScale_) + amptekRangeData.motionStartIndex() );
			qDebug() << "Changed to " << amptekRangeData.motionStartIndex();
		}
	}

	foreach(CLSAMDSScalerChannelDetector *scalerChannelDetector, scalerChannelDetectors_)
		detectorStartMotionIndexMap_.insert(scalerChannelDetector->name(), scalerRangeData.motionStartIndex());
	if(!amptekDetectors_.isEmpty()){
		foreach(CLSAmptekSDD123DetectorNew* amptekDetector, amptekDetectors_){
			detectorStartMotionIndexMap_.insert(amptekDetector->name(), amptekRangeData.motionStartIndex());
		}
	}

	qDebug() << "Amptek: " << amptekRangeData.motionStartIndex() << amptekRangeData.motionEndIndex() << amptekRangeData.listIndex();
	qDebug() << "Scaler: " << scalerRangeData.motionStartIndex() << scalerRangeData.motionEndIndex() << scalerRangeData.listIndex();

	scalerInitiateMovementIndex_ = detectorStartMotionIndexMap_.value(scalerChannelDetectors_.first()->name());

	return true;
}

bool AMContinuousScanActionController::generateAxisFeedbackValues()
{
	return false;
}
