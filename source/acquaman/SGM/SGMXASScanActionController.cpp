#include "SGMXASScanActionController.h"

#include "acquaman/AMScanActionControllerScanAssembler.h"
#include "dataman/AMXASScan.h"
#include "beamline/SGM/SGMBeamline.h"
#include <QDebug>

SGMXASScanActionController::SGMXASScanActionController(SGMXASScanConfiguration2013 *cfg, QObject *parent) :
	AMScanActionController(cfg, parent)
{
	scan_ = new AMXASScan();
	scan_->setFileFormat("sgm2011XAS");
	scan_->setRunId(AMUser::user()->currentRunId());
	scan_->setScanConfiguration(cfg);
	scan_->setSampleId(SGMBeamline::sgm()->currentSampleId());
	scan_->setIndexType("fileSystem");
	scan_->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	insertionIndex_ = AMnDIndex(0);

	AMScanActionControllerScanAssembler *newScanAssembler = new AMScanActionControllerScanAssembler(this);

	AMScanAxisRegion firstRegion(cfg->regionStart(0), cfg->regionDelta(0), cfg->regionEnd(0), cfg->regionTime(0), this);
	AMScanAxis *energyAxis = new AMScanAxis(AMScanAxis::StepAxis, firstRegion, this);
	for(int x = 1; x < cfg->regionCount(); x++){
		AMScanAxisRegion anotherRegion(cfg->regionStart(x), cfg->regionDelta(x), cfg->regionEnd(x), cfg->regionTime(x), this);
		energyAxis->appendRegion(anotherRegion);
	}

	newScanAssembler->appendAxis(SGMBeamline::sgm()->energy(), energyAxis);

	for(int x = 0; x < cfg->detectorConfigurations().count(); x++)
		qDebug() << "This configuration has a detector named " << cfg->detectorConfigurations().at(x).name();

	newScanAssembler->addDetector(SGMBeamline::sgm()->newAmptekSDD1());
	newScanAssembler->addDetector(SGMBeamline::sgm()->newAmptekSDD2());

	if(scan_->rawData()->addMeasurement(AMMeasurementInfo(*(SGMBeamline::sgm()->newAmptekSDD1()))))
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1));
	if(scan_->rawData()->addMeasurement(AMMeasurementInfo(*(SGMBeamline::sgm()->newAmptekSDD2()))))
		scan_->addRawDataSource(new AMRawDataSource(scan_->rawData(), scan_->rawData()->measurementCount()-1));

	actionTree_ = newScanAssembler->generateActionTree();
}

AMAction3* SGMXASScanActionController::actionsTree(){
	return actionTree_;
}

void SGMXASScanActionController::setPointer(QObject *pointer){
	pointer_ = pointer;
}

bool SGMXASScanActionController::initializeImplementation(){
	return true;
}

bool SGMXASScanActionController::startImplementation(){
	if(SGMBeamline::sgm()->isBeamlineScanning()){
		AMErrorMon::report(AMErrorReport(this,
				AMErrorReport::Alert,
				SGMXASSCANACTIONCONTROLLER_CANT_INTIALIZE,
				"Error, SGM XAS Scan Action Controller failed to start (SGM is already scanning). Either another scan is currently running or the scanning flag is stuck at Scanning."));
		return false;
	}
	return AMScanActionController::startImplementation();
}

void SGMXASScanActionController::cancelImplementation(){
	AMScanActionController::cancelImplementation();
}

#include "application/AMAppController.h"
#include "acquaman/AMAgnosticDataAPI.h"
bool SGMXASScanActionController::event(QEvent *e){
	if(e->type() == (QEvent::Type)AMAgnosticDataAPIDefinitions::MessageEvent){
		AMAgnosticDataAPIMessage message = ((AMAgnositicDataEvent*)e)->message_;

		qDebug() << "Just heard a message with type " << message.value("message").toString() << " and message type " << message.messageType();

		switch(message.messageType()){
		case AMAgnosticDataAPIDefinitions::AxisStarted:
			break;
		case AMAgnosticDataAPIDefinitions::AxisFinished:{

			qDebug() << "Try to launch to editor";
			AMAppController *toAppController = qobject_cast<AMAppController*>(pointer_);
			toAppController->openScanInEditor(scan_);

			break;}
		case AMAgnosticDataAPIDefinitions::LoopIncremented:
			scan_->rawData()->endInsertRows();
			insertionIndex_[0] = insertionIndex_.i()+1;
			qDebug() << "Now the insertionIndex is " << insertionIndex_.toString();
			break;
		case AMAgnosticDataAPIDefinitions::DataAvailable:{
			qDebug() << "Have some data from detector named " << message.uniqueID() << " which is " << scan_->rawData()->idOfMeasurement(message.uniqueID());
			if(insertionIndex_.i() >= scan_->rawData()->scanSize(0)){
				qDebug() << "First detector to report back, need to beginInsertRows and setAxisValue";
				scan_->rawData()->beginInsertRows(insertionIndex_.i()-scan_->rawData()->scanSize(0)+1, -1);
				scan_->rawData()->setAxisValue(0, insertionIndex_.i(), currentAxisValue_);
			}

			QVector<double> localDetectorData;
			QVariantList detectorDataValues = message.value("DetectorData").toList();
			for(int x = 0; x < detectorDataValues.count(); x++)
				localDetectorData.append(detectorDataValues.at(x).toDouble());

			scan_->rawData()->setValue(insertionIndex_, scan_->rawData()->idOfMeasurement(message.uniqueID()), localDetectorData.constData());
			break;}
		case AMAgnosticDataAPIDefinitions::ControlMoved:
			if(message.value("ControlMovementType") == "Absolute")
				currentAxisValue_ = message.value("ControlMovementValue").toDouble();
			else if(message.value("ControlMovementType") == "Relative")
				currentAxisValue_ += message.value("ControlMovementValue").toDouble();
			qDebug() << "Now the axisValue is " << currentAxisValue_;

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
