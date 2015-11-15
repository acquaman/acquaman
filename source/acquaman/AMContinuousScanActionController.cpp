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

#include "source/DataHolder/AMDSScalarDataHolder.h"
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

			QVector<qint32> teyVector;
			QVector<qint32> tfyVector;
			QVector<qint32> i0Vector;
			QVector<qint32> pdVector;
			QVector<qint32> fpd1Vector;
			QVector<qint32> fpd2Vector;
			QVector<qint32> fpd3Vector;
			QVector<qint32> fpd4Vector;
			QVector<qint32> hexapodRedVector;
			QVector<qint32> hexapodBlackVector;
			QVector<qint32> encoderUpVector;
			QVector<qint32> encoderDownVector;

			if(clientDataRequestMap_.contains("Scaler (BL1611-ID-1)")){
				AMDSClientDataRequest *scalerClientDataRequst = clientDataRequestMap_.value("Scaler (BL1611-ID-1)");
				int totalCount = scalerClientDataRequst->data().count();

				teyVector = QVector<qint32>(totalCount);
				tfyVector = QVector<qint32>(totalCount);
				i0Vector = QVector<qint32>(totalCount);
				pdVector = QVector<qint32>(totalCount);
				fpd1Vector = QVector<qint32>(totalCount);
				fpd2Vector = QVector<qint32>(totalCount);
				fpd3Vector = QVector<qint32>(totalCount);
				fpd4Vector = QVector<qint32>(totalCount);
				hexapodRedVector = QVector<qint32>(totalCount);
				hexapodBlackVector = QVector<qint32>(totalCount);
				encoderUpVector = QVector<qint32>(totalCount);
				encoderDownVector = QVector<qint32>(totalCount);

				for(int x = 0; x < totalCount; x++){
					AMDSLightWeightScalarDataHolder *asScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(scalerClientDataRequst->data().at(x));
					QVector<qint32> oneVector = asScalarDataHolder->dataArray().constVectorQint32();

					tfyVector[x] = oneVector.at(0);
					pdVector[x] = oneVector.at(1);
					teyVector[x] = oneVector.at(2);
					i0Vector[x] = oneVector.at(3);
					fpd2Vector[x] = oneVector.at(4);
					fpd3Vector[x] = oneVector.at(5);
					fpd4Vector[x] = oneVector.at(6);
					fpd1Vector[x] = oneVector.at(7);
					hexapodRedVector[x] = oneVector.at(8);
					hexapodBlackVector[x] = oneVector.at(9);
					encoderUpVector[x] = oneVector.at(10);
					encoderDownVector[x] = oneVector.at(11);
				}
			}

			AMDSClientDataRequest *oneAmptekDataRequest = clientDataRequestMap_.value("Amptek SDD 240");

			bool castToGenericFlatArrayHolder = false;
			AMDSLightWeightGenericFlatArrayDataHolder *dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(oneAmptekDataRequest->data().at(0));
			if(dataHolderAsGenericFlatArrayDataHolder)
				castToGenericFlatArrayHolder = true;

			bool castToDwellSpectralHolder = false;
			AMDSDwellSpectralDataHolder *dataHolderAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(oneAmptekDataRequest->data().at(0));
			if(dataHolderAsDwellSpectral)
				castToDwellSpectralHolder = true;

			int initiateMovementIndex = 0;
			bool foundMovementStart = false;
			bool foundMovementEnd = false;

			// Loop backwards from the end to find the start of the movement we're interested in
			for(int x = oneAmptekDataRequest->data().count()-1; (x > 0) && !foundMovementStart; x--){
				if(castToDwellSpectralHolder){
					dataHolderAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(oneAmptekDataRequest->data().at(x));
					dataHolderAsGenericFlatArrayDataHolder = dataHolderAsDwellSpectral;
				}
				else if(castToGenericFlatArrayHolder){
					dataHolderAsDwellSpectral = 0;
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(oneAmptekDataRequest->data().at(x));
				}

				int encoderPulsesInPeriod = dataHolderAsDwellSpectral->dwellStatusData().generalPurposeCounter();
				if(!foundMovementEnd && encoderPulsesInPeriod > 20){
					qDebug() << "Found movement end at index " << x;
					foundMovementEnd = true;
				}
				else if(foundMovementEnd && encoderPulsesInPeriod < 5){
					foundMovementStart = true;
					initiateMovementIndex = x;
					qDebug() << "Found movement start at index " << initiateMovementIndex;
				}
			}


			int scalerInitiateMovementIndex = 0;
			bool foundScalerMovementStart = false;
			bool foundScalerMovementEnd = false;

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

			qDebug() << "Amptek: " << initiateMovementIndex;
			qDebug() << "Scaler: " << scalerInitiateMovementIndex;
			qDebug() << "Apples to apples: " << initiateMovementIndex*50 << scalerInitiateMovementIndex;


			/*
			int startEncoderValue = -412449;
			int currentEncoderValue = startEncoderValue;
			QVector<double> energyFeedbacks = QVector<double>(oneAmptekDataRequest->data().count()-initiateMovementIndex+1);
			energyFeedbacks[0] = SGMGratingSupport::energyFromGrating(SGMGratingSupport::LowGrating, startEncoderValue);

			// Loop from the start of the movement to the end and recreate the axis values (energy in this case) from the encoder pulse changes
			for(int x = initiateMovementIndex, size = oneAmptekDataRequest->data().count(); x < size; x++){
				if(castToDwellSpectralHolder){
					dataHolderAsDwellSpectral = qobject_cast<AMDSDwellSpectralDataHolder*>(oneAmptekDataRequest->data().at(x));
					dataHolderAsGenericFlatArrayDataHolder = dataHolderAsDwellSpectral;
				}
				else if(castToGenericFlatArrayHolder){
					dataHolderAsDwellSpectral = 0;
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(oneAmptekDataRequest->data().at(x));
				}

				currentEncoderValue += dataHolderAsDwellSpectral->dwellStatusData().generalPurposeCounter();
				energyFeedbacks[x-initiateMovementIndex+1] = SGMGratingSupport::energyFromGrating(SGMGratingSupport::LowGrating, currentEncoderValue);
			}
			*/


			int startEncoderValue = -412449;
			int currentEncoderValue = startEncoderValue;
			QVector<double> scalerEnergyFeedbacks = QVector<double>(encoderUpVector.count()-scalerInitiateMovementIndex+1);
			scalerEnergyFeedbacks[0] = SGMGratingSupport::energyFromGrating(SGMGratingSupport::LowGrating, startEncoderValue);

			// Loop from the start of the movement to the end and recreate the axis values (energy in this case) from the encoder pulse changes
			for(int x = scalerInitiateMovementIndex, size = encoderUpVector.count(); x < size; x++){
				currentEncoderValue += encoderUpVector.at(x) - encoderDownVector.at(x);
//				qDebug() << "Inserting " << x-scalerInitiateMovementIndex+1 << " of " << scalerEnergyFeedbacks.count() << " as " << currentEncoderValue << SGMGratingSupport::energyFromGrating(SGMGratingSupport::LowGrating, currentEncoderValue);
				scalerEnergyFeedbacks[x-scalerInitiateMovementIndex+1] = SGMGratingSupport::energyFromGrating(SGMGratingSupport::LowGrating, currentEncoderValue);
			}

			int amptekMapIndex;
			int lastAmptekMapIndex = 0;
			for(int x = scalerInitiateMovementIndex, size = encoderUpVector.count(); x < size; x++){
				amptekMapIndex = floor(((double)x)/((double)50.0));
//				qDebug() << "Map index from " << x << " to " << amptekMapIndex;

				scan_->rawData()->beginInsertRows(1, -1);
				scan_->rawData()->setAxisValue(0, insertionIndex_.i(), scalerEnergyFeedbacks.at(x-scalerInitiateMovementIndex+1));

				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("I0"), AMnDIndex(), encoderUpVector.at(x) - encoderDownVector.at(x));

				if(amptekMapIndex != lastAmptekMapIndex){
					lastAmptekMapIndex = amptekMapIndex;
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value("Amptek SDD 240")->data().at(amptekMapIndex));
					scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("AmptekSDD1"), dataHolderAsGenericFlatArrayDataHolder->dataArray().constVectorDouble().constData());
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value("Amptek SDD 241")->data().at(amptekMapIndex));
					scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("AmptekSDD2"), dataHolderAsGenericFlatArrayDataHolder->dataArray().constVectorDouble().constData());
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value("Amptek SDD 242")->data().at(amptekMapIndex));
					scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("AmptekSDD3"), dataHolderAsGenericFlatArrayDataHolder->dataArray().constVectorDouble().constData());
					dataHolderAsGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder*>(clientDataRequestMap_.value("Amptek SDD 243")->data().at(amptekMapIndex));
					scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("AmptekSDD4"), dataHolderAsGenericFlatArrayDataHolder->dataArray().constVectorDouble().constData());
				}

				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("TEY"), AMnDIndex(), teyVector.at(x));
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("TFY"), AMnDIndex(), tfyVector.at(x));
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("PD"), AMnDIndex(), pdVector.at(x));

				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("FilteredPD1"), AMnDIndex(), fpd1Vector.at(x));
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("FilteredPD2"), AMnDIndex(), fpd2Vector.at(x));
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("FilteredPD3"), AMnDIndex(), fpd3Vector.at(x));
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("FilteredPD4"), AMnDIndex(), fpd4Vector.at(x));

				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("HexapodRed"), AMnDIndex(), hexapodRedVector.at(x));
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("HexapodBlack"), AMnDIndex(), hexapodBlackVector.at(x));
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("EncoderUp"), AMnDIndex(), encoderUpVector.at(x));
				scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement("EncoderDown"), AMnDIndex(), encoderDownVector.at(x));


				scan_->rawData()->endInsertRows();
				insertionIndex_[0] = insertionIndex_.i()+1;
			}

			/*
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

			break;}

		case AMAgnosticDataAPIDefinitions::AxisValueFinished:

			// Not relevant for continuous scans.
			break;

		case AMAgnosticDataAPIDefinitions::DataAvailable:{
			AMAgnosticDataAPIDataAvailableMessage *dataAvailableMessage = static_cast<AMAgnosticDataAPIDataAvailableMessage*>(&message);

			intptr_t dataRequestPointer = dataAvailableMessage->detectorDataAsAMDS();
			void *dataRequestVoidPointer = (void*)dataRequestPointer;
			AMDSClientDataRequest *dataRequest = static_cast<AMDSClientDataRequest*>(dataRequestVoidPointer);
			clientDataRequestMap_.insert(dataRequest->bufferName(), dataRequest);

//			if(dataRequest->bufferName() == "Scaler (BL1611-ID-1)"){
//				qDebug() << "DataAvailable for the scaler, what's in here?";

//				qDebug() << dataRequest->metaObject()->className() << dataRequest->data().count();

//				AMDSClientRelativeCountPlusCountDataRequest *asRelativeRequesst = qobject_cast<AMDSClientRelativeCountPlusCountDataRequest*>(dataRequest);
//				if(asRelativeRequesst){
//					qDebug() << asRelativeRequesst->data().at(0)->metaObject()->className();

//					qDebug() << "Data count is " << asRelativeRequesst->data().count();

//					QString outputString;
//					for(int x = 0, size = asRelativeRequesst->data().count(); x < size; x++){
//						AMDSLightWeightScalarDataHolder *asScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(asRelativeRequesst->data().at(x));
////						qDebug() << "At " << x << asScalarDataHolder->dataArray().constVectorQint32();
//						QVector<qint32> oneVector = asScalarDataHolder->dataArray().constVectorQint32();
//						outputString.append(QString("[%1] ").arg(x));
//						for(int y = 0, ySize = oneVector.count(); y < ySize; y++)
//							outputString.append(QString("%1 ").arg(oneVector.at(y)));
//					}
//					qDebug() << outputString;
//				}
//			}
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
