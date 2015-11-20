#include "AMContinuousScanActionController.h"

#include <QFile>
#include <QFileInfo>
#include <QStringBuilder>

#include "application/AMAppController.h"
#include "application/AMAppControllerSupport.h"
#include "acquaman/AMGenericScanActionControllerAssembler.h"
#include "acquaman/AMAgnosticDataAPI.h"
#include "actions3/AMActionRunner3.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "beamline/AMBeamline.h"
#include "dataman/datastore/AMCDFDataStore.h"

#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/DataHolder/AMDSGenericFlatArrayDataHolder.h"
#include "source/DataHolder/AMDSSpectralDataHolder.h"
#include <stdint.h>

#include <QDebug>

AMContinuousScanActionController::AMContinuousScanActionController(AMContinuousScanConfiguration *configuration, QObject *parent)
	: AMScanActionController(configuration, parent)
{
	continuousConfiguration_ = configuration;

	insertionIndex_ = AMnDIndex(0);
}

AMContinuousScanActionController::~AMContinuousScanActionController()
{

}

void AMContinuousScanActionController::buildScanController()
{
	// Build the scan assembler.
	createScanAssembler();

	// Setup all the axes.
	for (int i = 0, axisCount = scan_->rawData()->scanAxesCount(); i < axisCount; i++){

		continuousConfiguration_->scanAxisAt(i)->setName(scan_->rawData()->scanAxisAt(i).name);

		for (int j = 0, regionCount = continuousConfiguration_->scanAxisAt(i)->regionCount(); j < regionCount; j++)
			continuousConfiguration_->scanAxisAt(i)->regionAt(j)->setName(QString("%1 %2 %3").arg(scan_->rawData()->scanAxisAt(i).name).arg("region").arg(j+1));
	}

	// Configure the scan assemblers axes.
	qDebug() << "Scan axes count = " << scan_->rawData()->scanAxesCount();
	for (int i = 0, axisCount = scan_->rawData()->scanAxesCount(); i < axisCount; i++){

		scanAssembler_->insertAxis(i, AMBeamline::bl()->exposedControlByInfo(continuousConfiguration_->axisControlInfos().at(i)), continuousConfiguration_->scanAxisAt(i));
		qDebug() << "Adding axis " << continuousConfiguration_->axisControlInfos().at(i).name();
		qDebug() << "Start: " << double(continuousConfiguration_->scanAxisAt(0)->axisStart());
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

#include "source/DataHolder/AMDSScalarDataHolder.h"
#include "source/beamline/CLS/CLSAdvancedScalerChannelDetector.h"
bool AMContinuousScanActionController::event(QEvent *e)
{
	if (e->type() == (QEvent::Type)AMAgnosticDataAPIDefinitions::MessageEvent){

		AMAgnosticDataAPIMessage message = ((AMAgnositicDataEvent*)e)->message_;

		switch(message.messageType()){

		case AMAgnosticDataAPIDefinitions::AxisStarted:{

			// Double check this!
//			scan_->rawData()->beginInsertRows(continuousConfiguration_->scanAxisAt(0)->numberOfPoints(), -1);

			break;}

		case AMAgnosticDataAPIDefinitions::AxisFinished:{
			// An argument could be made to put the control axis value stuff here.
			if(generalConfig_->axisControlInfos().count() == 1)
				axisFinished1DHelper();
			else if(generalConfig_->axisControlInfos().count() == 2)
				axisFinished2DHelper();


			break;
		}

		case AMAgnosticDataAPIDefinitions::AxisValueFinished:

			// Not relevant for continuous scans.
			break;

		case AMAgnosticDataAPIDefinitions::DataAvailable:{
			AMAgnosticDataAPIDataAvailableMessage *dataAvailableMessage = static_cast<AMAgnosticDataAPIDataAvailableMessage*>(&message);

			if(message.uniqueID() == "GratingEncoderFeedback")
				metaDataMap_.insert(message.uniqueID(), message.value("DetectorData").toList().at(0).toDouble());
			else{
				intptr_t dataRequestPointer = dataAvailableMessage->detectorDataAsAMDS();
				void *dataRequestVoidPointer = (void*)dataRequestPointer;
				AMDSClientDataRequest *dataRequest = static_cast<AMDSClientDataRequest*>(dataRequestVoidPointer);
				clientDataRequestMap_.insert(dataRequest->bufferName(), dataRequest);
			}

			// Step 1:
			// This will need a transform where:
			// QList<QVector<double>>(rawAxes)  -> QVector<double>(continuousAxis)
			// Where this implies that some set of raw axis values are
			// mapped to a single set of axis values that can be set via setAxisValue()

			// Step 2:
			// for each control value element of the list of data passed in the event
			// fill the AMDataStore::setAxisValue();
			// Can unfortunately only be done by iterating and calling setAxisValue().

			// Step 3:
			// This will need a data align transform where the data is assigned to an
			// axis value (insertion index) based on the time stamp.

			// Step 4:
			// for each data element of the list of data passed in the event
			// fill the AMDataStore::setValue();
			// Can unfortunately only be done by iterating and calling setAxisValue().

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
	scanAssembler_ = new AMGenericScanActionControllerAssembler(this);
}

void AMContinuousScanActionController::axisFinished1DHelper()
{
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
	CLSScalerChannelDetector *asScalerChannelDetector = 0;
	for(int x = 0, size = generalConfig_->detectorConfigurations().count(); x < size; x++){
		AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(x));
		asScalerChannelDetector = qobject_cast<CLSScalerChannelDetector*>(oneDetector);
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


	int scalerInitiateMovementIndex = 0;
	bool foundScalerMovementStart = false;
	bool foundScalerMovementEnd = false;

	QVector<qint32> encoderUpVector = scalerChannelVectors.value("EncoderUp");
	QVector<qint32> encoderDownVector = scalerChannelVectors.value("EncoderDown");

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
				asScalerChannelDetector = qobject_cast<CLSScalerChannelDetector*>(oneDetector);
				if(!asScalerChannelDetector){
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value(oneDetector->amdsBufferName())->data().at(x));
					scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(oneDetector->name()), dataHolderAsGenericFlatArrayDataHolder->dataArray().constVectorDouble().constData());
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
	else{
		for(int x = encoderUpVector.count()-1; x >= scalerInitiateMovementIndex; x--){
			scan_->rawData()->beginInsertRows(1, -1);
			scan_->rawData()->setAxisValue(0, insertionIndex_.i(), scalerEnergyFeedbacks.at(x-scalerInitiateMovementIndex));

			for(int y = 0, ySize = generalConfig_->detectorConfigurations().count(); y < ySize; y++){
				AMDetector *oneDetector = AMBeamline::bl()->exposedDetectorByInfo(generalConfig_->detectorConfigurations().at(y));
				asScalerChannelDetector = qobject_cast<CLSScalerChannelDetector*>(oneDetector);
				if(!asScalerChannelDetector){
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value(oneDetector->amdsBufferName())->data().at(x));
					scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(oneDetector->name()), dataHolderAsGenericFlatArrayDataHolder->dataArray().constVectorDouble().constData());
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

}

void AMContinuousScanActionController::axisFinished2DHelper()
{

}
