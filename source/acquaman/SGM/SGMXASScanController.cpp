/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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

#include "dataman/SGM2004FileLoader.h"
#include "analysis/AM1DExpressionAB.h"
#include "dataman/AMRawDataSource.h"
#include "dataman/AMUser.h"


SGMXASScanController::SGMXASScanController(SGMXASScanConfiguration *cfg){
	specificCfg_ = cfg;
	_pCfg_ = & specificCfg_;
	initializationActions_ = NULL;
	beamlineInitialized_ = false;

	specificScan_ = new AMXASScan();
	_pScan_ = &specificScan_;
	pScan_()->setName("SGM XAS Scan");
	pScan_()->setFilePath(pCfg_()->filePath()+pCfg_()->fileName());
	pScan_()->setFileFormat("sgm2004");
	pScan_()->setRunId(AMUser::user()->currentRunId());

	// Create space in raw data store, and create raw data channels, for each detector.

	for(int i = 0; i < pCfg_()->allDetectorConfigurations().count(); i++){
		AMDetectorInfo* detectorInfo = pCfg_()->allDetectorConfigurations().detectorInfoAt(i);
		if(pCfg_()->allDetectorConfigurations().isActiveAt(i)){
			pScan_()->rawData()->addMeasurement(AMMeasurementInfo(*detectorInfo));
			pScan_()->addRawDataSource(new AMRawDataSource(pScan_()->rawData(), i));
		}
	}

	QList<AMDataSource*> raw1DDataSources;
	for(int i=0; i<pScan_()->rawDataSources()->count(); i++)
		if(pScan_()->rawDataSources()->at(i)->rank() == 1)
			raw1DDataSources << pScan_()->rawDataSources()->at(i);

	int rawTeyIndex = pScan_()->rawDataSources()->indexOfKey("tey");
	int rawTfyIndex = pScan_()->rawDataSources()->indexOfKey("tfy");
	int rawI0Index = pScan_()->rawDataSources()->indexOfKey("I0");

	if(rawTeyIndex != -1 && rawI0Index != -1) {
		AM1DExpressionAB* teyChannel = new AM1DExpressionAB("tey_n");
		teyChannel->setDescription("Normalized TEY");
		teyChannel->setInputDataSources(raw1DDataSources);
		teyChannel->setExpression("tey/I0");

		pScan_()->addAnalyzedDataSource(teyChannel);
	}

	if(rawTfyIndex != -1 && rawI0Index != -1) {
		AM1DExpressionAB* tfyChannel = new AM1DExpressionAB("tfy_n");
		tfyChannel->setDescription("Normalized TFY");
		tfyChannel->setInputDataSources(raw1DDataSources);
		tfyChannel->setExpression("-tfy/I0");

		pScan_()->addAnalyzedDataSource(tfyChannel);
	}
}

bool SGMXASScanController::isBeamlineInitialized() {
	return beamlineInitialized_;
}

bool SGMXASScanController::beamlineInitialize(){
	AMBeamlineControlMoveAction *tmpAction = NULL;
	#warning "Hey David, who's going to delete the list and the actions?"
	initializationActions_ = new AMBeamlineParallelActionsList();

	initializationActions_->appendStage(new QList<AMBeamlineActionItem*>());
	AMBeamlineControlSetMoveAction* tmpSetAction = new AMBeamlineControlSetMoveAction(SGMBeamline::sgm()->fluxResolutionSet());
	tmpSetAction->setSetpoint((pCfg_()->fluxResolutionGroup()));

	initializationActions_->appendAction(0, tmpSetAction);
	tmpSetAction = new AMBeamlineControlSetMoveAction(SGMBeamline::sgm()->trackingSet());
	tmpSetAction->setSetpoint(pCfg_()->trackingGroup());
	initializationActions_->appendAction(0, tmpSetAction);

	AMBeamlineControlWaitAction *tmpWaitAction = new AMBeamlineControlWaitAction(SGMBeamline::sgm()->loadlockCCG(), AMBeamlineControlWaitAction::LessThanTarget);
	tmpWaitAction->setWaitpoint(2.2e-8);
	tmpWaitAction->setActionTolerance(1.0e-8);
	tmpWaitAction->setHoldTime(1000);
	initializationActions_->appendAction(0, tmpWaitAction);

	tmpWaitAction = new AMBeamlineControlWaitAction(SGMBeamline::sgm()->loadlockTCG(), AMBeamlineControlWaitAction::NotEqualToTarget);
	tmpWaitAction->setWaitpoint(720);
	tmpWaitAction->setActionTolerance(20);
	tmpWaitAction->setHoldTime(1000);
	initializationActions_->appendAction(0, tmpWaitAction);

	for(int x = 0; x < pCfg_()->allDetectors()->count(); x++)
		if(pCfg_()->allDetectorConfigurations().isActiveAt(x))
			pCfg_()->allDetectors()->detectorAt(x)->setFromInfo(pCfg_()->allDetectorConfigurations().detectorInfoAt(x));

	beamlineInitialized_ = true;
	return beamlineInitialized_;
}

SGMXASScanConfiguration* SGMXASScanController::pCfg_(){
	return *_pCfg_;
}

AMXASScan* SGMXASScanController::pScan_(){
	return *_pScan_;
}
