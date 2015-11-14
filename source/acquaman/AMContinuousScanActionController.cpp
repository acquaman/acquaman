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

			AMDSClientDataRequest *oneClientDataRequest = clientDataRequestMap_.value("Amptek SDD 240");

			if(oneClientDataRequest){

				bool castToGenericFlatArrayHolder = false;
				AMDSLightWeightGenericFlatArrayDataHolder *dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(oneClientDataRequest->data().at(0));
				if(dataHolderAsGenericFlatArrayDataHolder){
					qDebug() << "Can cast to generic flat array holder";
					castToGenericFlatArrayHolder = true;
				}

				bool castToDwellSpectralHolder = false;
				AMDSDwellSpectralDataHolder *dataHolderAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(oneClientDataRequest->data().at(0));
				if(dataHolderAsDwellSpectral){
					qDebug() << "Can cast to dwell spectral data holder";
					castToDwellSpectralHolder = true;
				}

				int initiateMovementIndex = 0;
				bool foundMovementStart = false;
				bool foundMovementEnd = false;

				// Loop backwards from the end to find the start of the movement we're interested in
				for(int x = oneClientDataRequest->data().count()-1; (x > 0) && !foundMovementStart; x--){
					if(castToDwellSpectralHolder){
						dataHolderAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(oneClientDataRequest->data().at(x));
						dataHolderAsGenericFlatArrayDataHolder = dataHolderAsDwellSpectral;
					}
					else if(castToGenericFlatArrayHolder){
						dataHolderAsDwellSpectral = 0;
						dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(oneClientDataRequest->data().at(x));
					}

					int encoderPulsesInPeriod = dataHolderAsDwellSpectral->dwellStatusData().generalPurposeCounter();
					if(!foundMovementEnd && encoderPulsesInPeriod > 20){
						qDebug() << "Found movement end at index " << x;
						foundMovementEnd = true;
					}
					else if(foundMovementEnd && encoderPulsesInPeriod < 20){
						foundMovementStart = true;
						initiateMovementIndex = x;
						qDebug() << "Found movement start at index " << initiateMovementIndex;
					}
				}



				int startEncoderValue = -412449;
				int currentEncoderValue = startEncoderValue;
				QVector<double> energyFeedbacks = QVector<double>(oneClientDataRequest->data().count()-initiateMovementIndex+1);
				energyFeedbacks[0] = SGMGratingSupport::energyFromGrating(SGMGratingSupport::LowGrating, startEncoderValue);

				// Loop from the start of the movement to the end and recreate the axis values (energy in this case) from the encoder pulse changes
				for(int x = initiateMovementIndex, size = oneClientDataRequest->data().count(); x < size; x++){
					if(castToDwellSpectralHolder){
						dataHolderAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(oneClientDataRequest->data().at(x));
						dataHolderAsGenericFlatArrayDataHolder = dataHolderAsDwellSpectral;
					}
					else if(castToGenericFlatArrayHolder){
						dataHolderAsDwellSpectral = 0;
						dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(oneClientDataRequest->data().at(x));
					}

					currentEncoderValue += dataHolderAsDwellSpectral->dwellStatusData().generalPurposeCounter();
					energyFeedbacks[x-initiateMovementIndex+1] = SGMGratingSupport::energyFromGrating(SGMGratingSupport::LowGrating, currentEncoderValue);
				}

				// Loop from the start of the movement to the end and place any data that we have into the actual rawData()
				for(int x = initiateMovementIndex, size = oneClientDataRequest->data().count(); x < size; x++){
					if(castToDwellSpectralHolder){
						dataHolderAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(oneClientDataRequest->data().at(x));
						dataHolderAsGenericFlatArrayDataHolder = dataHolderAsDwellSpectral;
					}
					else if(castToGenericFlatArrayHolder){
						dataHolderAsDwellSpectral = 0;
						dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(oneClientDataRequest->data().at(x));
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

					scan_->rawData()->endInsertRows();
					insertionIndex_[0] = insertionIndex_.i()+1;
				}
			}

			break;}

		case AMAgnosticDataAPIDefinitions::AxisValueFinished:

			// Not relevant for continuous scans.
			break;

		case AMAgnosticDataAPIDefinitions::DataAvailable:{
			AMAgnosticDataAPIDataAvailableMessage *dataAvailableMessage = static_cast<AMAgnosticDataAPIDataAvailableMessage*>(&message);

			intptr_t dataRequestPointer = dataAvailableMessage->detectorDataAsAMDS();
			void *dataRequestVoidPointer = (void*)dataRequestPointer;
			AMDSClientDataRequest *dataRequest = static_cast<AMDSClientDataRequest*>(dataRequestVoidPointer);
			qDebug() << "Inserting " << dataRequest->bufferName() << " into map";
			clientDataRequestMap_.insert(dataRequest->bufferName(), dataRequest);

			/*
			bool castToGenericFlatArrayHolder = false;
			AMDSLightWeightGenericFlatArrayDataHolder *dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(dataRequest->data().at(0));
			if(dataHolderAsGenericFlatArrayDataHolder){
				qDebug() << "Can cast to generic flat array holder";
				castToGenericFlatArrayHolder = true;
			}

			bool castToDwellSpectralHolder = false;
			AMDSDwellSpectralDataHolder *dataHolderAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(dataRequest->data().at(0));
			if(dataHolderAsDwellSpectral){
				qDebug() << "Can cast to dwell spectral data holder";
				castToDwellSpectralHolder = true;
			}

			for(int x = 0, size = dataRequest->data().count(); x < size; x++){
				QString outputString = QString("Data Request [%1 @ %2]").arg(x).arg(dataRequest->data().at(x)->eventTime().toString("hh:mm:ss.zzz"));

				if(castToDwellSpectralHolder){
					dataHolderAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(dataRequest->data().at(x));
					dataHolderAsGenericFlatArrayDataHolder = dataHolderAsDwellSpectral;
				}
				else if(castToGenericFlatArrayHolder){
					dataHolderAsDwellSpectral = 0;
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(dataRequest->data().at(x));
				}

				if(dataHolderAsGenericFlatArrayDataHolder){
					QVector<double> oneFlatArray = dataHolderAsGenericFlatArrayDataHolder->dataArray().constVectorDouble();

					for(int y = 18, max = 28; y < max; y++)
						outputString.append(QString("%1 ").arg(oneFlatArray.at(y)));
				}
				if(dataHolderAsDwellSpectral){
					outputString.append(QString(" {%1}").arg(dataHolderAsDwellSpectral->dwellStatusData().generalPurposeCounter()));
				}

				qDebug() << outputString;
			}
			*/

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
