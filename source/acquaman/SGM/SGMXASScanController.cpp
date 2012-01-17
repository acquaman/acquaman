/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "SGMXASScanController.h"

#include "dataman/SGM/SGM2004FileLoader.h"
#include "dataman/SGM/SGM2011XASFileLoader.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"
#include "dataman/datasource/AMRawDataSource.h"
#include "dataman/AMUser.h"

#include "dataman/AMSamplePlate.h"

SGMXASScanController::SGMXASScanController(SGMXASScanConfiguration *cfg){
	config_ = cfg;
	initializationActions_ = 0; //NULL
	cleanUpActions_ = 0; //NULL
	beamlineInitialized_ = false;

	specificScan_ = new AMXASScan();
	specificScan_->setFileFormat("sgm2011XAS");
	specificScan_->setRunId(AMUser::user()->currentRunId());
	specificScan_->setScanConfiguration(config_);
	specificScan_->setSampleId(SGMBeamline::sgm()->currentSampleId());
	specificScan_->setIndexType("fileSystem");
	QString scanName;
	QString sampleName;
	if(specificScan_->sampleId() == -1)
		sampleName = "Unknown Sample";
	else
		sampleName = AMSample(specificScan_->sampleId(), AMUser::user()->database()).name();
	if(config_->userScanName() == ""){
		scanName = config_->autoScanName();
		specificScan_->setName(QString("%1 - %2").arg(sampleName).arg(scanName));
	}
	else{
		scanName = config_->userScanName();
		specificScan_->setName(QString("%1 - %2").arg(scanName).arg(sampleName));
	}

	// Create space in raw data store, and create raw data channels, for each detector.
	for(int i = 0; i < config_->allDetectorConfigurations().count(); i++){
		AMDetectorInfo* detectorInfo = config_->allDetectorConfigurations().detectorInfoAt(i);
		if(config_->allDetectorConfigurations().isActiveAt(i)){

			if(specificScan_->rawData()->addMeasurement(AMMeasurementInfo(*detectorInfo)))
				specificScan_->addRawDataSource(new AMRawDataSource(specificScan_->rawData(), specificScan_->rawData()->measurementCount()-1));
			else
				qDebug() << "BIG PROBLEM!!!!!! WHAT JUST HAPPENED?!?!?" << detectorInfo->name();
		}
	}

	QList<AMDataSource*> raw1DDataSources;
	for(int i=0; i<specificScan_->rawDataSources()->count(); i++)
		if(specificScan_->rawDataSources()->at(i)->rank() == 1)
			raw1DDataSources << specificScan_->rawDataSources()->at(i);

	int rawTeyIndex = specificScan_->rawDataSources()->indexOfKey(SGMBeamline::sgm()->teyDetector()->description());
	int rawTfyIndex = specificScan_->rawDataSources()->indexOfKey(SGMBeamline::sgm()->tfyDetector()->description());
	int rawI0Index = specificScan_->rawDataSources()->indexOfKey(SGMBeamline::sgm()->i0Detector()->description());

	if(rawTeyIndex != -1 && rawI0Index != -1) {
		AM1DExpressionAB* teyChannel = new AM1DExpressionAB(QString("%1Norm").arg(SGMBeamline::sgm()->teyDetector()->description()));
		teyChannel->setDescription("Normalized TEY");
		teyChannel->setInputDataSources(raw1DDataSources);
		teyChannel->setExpression(QString("%1/%2").arg(SGMBeamline::sgm()->teyDetector()->description()).arg(SGMBeamline::sgm()->i0Detector()->description()));

		specificScan_->addAnalyzedDataSource(teyChannel);
	}

	if(rawTfyIndex != -1 && rawI0Index != -1) {
		AM1DExpressionAB* tfyChannel = new AM1DExpressionAB(QString("%1Norm").arg(SGMBeamline::sgm()->tfyDetector()->description()));
		tfyChannel->setDescription("Normalized TFY");
		tfyChannel->setInputDataSources(raw1DDataSources);
		tfyChannel->setExpression(QString("-%1/%2").arg(SGMBeamline::sgm()->tfyDetector()->description()).arg(SGMBeamline::sgm()->i0Detector()->description()));

		specificScan_->addAnalyzedDataSource(tfyChannel);
	}

	if(SGMBeamline::sgm()->pgtDetector()){
		int rawSddIndex = specificScan_->rawDataSources()->indexOfKey(SGMBeamline::sgm()->pgtDetector()->description());
		if(rawSddIndex != -1) {
			AMRawDataSource* sddRaw = specificScan_->rawDataSources()->at(rawSddIndex);
			AM2DSummingAB* pfy = new AM2DSummingAB("PFY");
			QList<AMDataSource*> pfySource;
			pfySource << sddRaw;
			pfy->setInputDataSources(pfySource);
			pfy->setSumAxis(1);
			pfy->setSumRangeMax(sddRaw->size(1)-1);
			specificScan_->addAnalyzedDataSource(pfy);
			if(rawSddIndex != -1 && rawI0Index != -1) {
				AM1DExpressionAB* ipfyChannel = new AM1DExpressionAB(QString("IPFY"));
				ipfyChannel->setDescription("IPFY");
				QList<AMDataSource*> ipfySources;
				ipfySources.append(raw1DDataSources);
				ipfySources.append(pfy);
				ipfyChannel->setInputDataSources(ipfySources);
				ipfyChannel->setExpression(QString("%1/%2").arg(SGMBeamline::sgm()->i0Detector()->description()).arg("PFY"));

				specificScan_->addAnalyzedDataSource(ipfyChannel);
			}
		}
	}

	if(SGMBeamline::sgm()->oos65000Detector()){
		int rawOOSIndex = specificScan_->rawDataSources()->indexOfKey(SGMBeamline::sgm()->oos65000Detector()->description().remove('\n'));
		if(rawOOSIndex != -1) {
			AMRawDataSource* oosRaw = specificScan_->rawDataSources()->at(rawOOSIndex);
			AM2DSummingAB* ply = new AM2DSummingAB("PLY");
			QList<AMDataSource*> plySource;
			plySource << oosRaw;
			ply->setInputDataSources(plySource);
			ply->setSumAxis(1);
			ply->setSumRangeMax(oosRaw->size(1)-1);
			specificScan_->addAnalyzedDataSource(ply);
			if(rawOOSIndex != -1 && rawI0Index != -1) {
				AM1DExpressionAB* plyNormChannel = new AM1DExpressionAB(QString("PLYNorm"));
				plyNormChannel->setDescription("PLYNorm");
				QList<AMDataSource*> plyNormSources;
				plyNormSources.append(raw1DDataSources);
				plyNormSources.append(ply);
				plyNormChannel->setInputDataSources(plyNormSources);
				plyNormChannel->setExpression(QString("%1/%2").arg("PLY").arg(SGMBeamline::sgm()->i0Detector()->description()));

				specificScan_->addAnalyzedDataSource(plyNormChannel);
			}
		}
	}
}

bool SGMXASScanController::isBeamlineInitialized() {
	return beamlineInitialized_;
}

bool SGMXASScanController::beamlineInitialize(){
	AMBeamlineControlMoveAction *tmpAction = NULL;

	cleanUpActions_ = new AMBeamlineParallelActionsList();
	cleanUpActions_->appendStage(new QList<AMBeamlineActionItem*>());
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerMode());
	tmpAction->setSetpoint(SGMBeamline::sgm()->scalerMode()->value());
	cleanUpActions_->appendAction(0, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerStart());
	tmpAction->setSetpoint(SGMBeamline::sgm()->scalerStart()->value());
	cleanUpActions_->appendAction(0, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerTotalNumberOfScans());
	tmpAction->setSetpoint(SGMBeamline::sgm()->scalerTotalNumberOfScans()->value());
	cleanUpActions_->appendAction(0, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerScansPerBuffer());
	tmpAction->setSetpoint(SGMBeamline::sgm()->scalerScansPerBuffer()->value());
	cleanUpActions_->appendAction(0, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->fastShutterVoltage());
	tmpAction->setSetpoint(SGMBeamline::sgm()->fastShutterVoltage()->value());
	cleanUpActions_->appendAction(0, tmpAction);

	/* NTBA - August 25th, 2011 (David Chevrier)
			Who's going to delete the list and the actions?"
	*/
	initializationActions_ = new AMBeamlineParallelActionsList();

	initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
	AMBeamlineControlSetMoveAction* tmpSetAction = new AMBeamlineControlSetMoveAction(SGMBeamline::sgm()->fluxResolutionSet());
	tmpSetAction->setSetpoint((config_->fluxResolutionGroup()));
	initializationActions_->appendAction(0, tmpSetAction);

	tmpSetAction = new AMBeamlineControlSetMoveAction(SGMBeamline::sgm()->trackingSet());
	tmpSetAction->setSetpoint(config_->trackingGroup());
	initializationActions_->appendAction(0, tmpSetAction);

//	initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
/*
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->energy());
	tmpAction->setSetpoint(config_->startEnergy());
	initializationActions_->appendAction(0, tmpAction);
*/
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerMode());
	tmpAction->setSetpoint(0);
	initializationActions_->appendAction(0, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerStart());
	tmpAction->setSetpoint(0);
	initializationActions_->appendAction(0, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerTotalNumberOfScans());
	tmpAction->setSetpoint(1);
	initializationActions_->appendAction(0, tmpAction);
	tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->scalerScansPerBuffer());
	tmpAction->setSetpoint(1);
	initializationActions_->appendAction(0, tmpAction);


	for(int x = 0; x < config_->allDetectors()->count(); x++){
		if(config_->allDetectorConfigurations().isActiveAt(x)){
			//config_->allDetectors()->detectorAt(x)->setFromInfo(config_->allDetectorConfigurations().detectorInfoAt(x));
			config_->allDetectors()->detectorAt(x)->activate();
			if(config_->allDetectors()->detectorAt(x)->turnOnAction()){
				qDebug() << "Adding HV turn on to initialization actions";
				initializationActions_->appendAction(0, config_->allDetectors()->detectorAt(x)->turnOnAction());
			}
		}
	}

	initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());

        tmpAction = new AMBeamlineControlMoveAction(SGMBeamline::sgm()->energy());
		tmpAction->setSetpoint(config_->startEnergy());
        initializationActions_->appendAction(1, tmpAction);

	initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
	initializationActions_->appendAction(2, SGMBeamline::sgm()->createBeamOnActions());

	beamlineInitialized_ = true;
	return beamlineInitialized_;
}

