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

/// If this had a one-line documented comment, I would know how to interpret these two strings. Are they the name and the channel expression? The expression and the x-expression?
typedef QPair<QString, QString> chPair;

SGMXASScanController::SGMXASScanController(SGMXASScanConfiguration *cfg){
	specificCfg_ = cfg;
	_pCfg_ = & specificCfg_;
	beamlineInitialized_ = false;

	QList<AMDetectorInfo*> scanDetectors = pCfg_()->usingDetectors();
	//scanDetectors.prepend(SGMBeamline::sgm()->i0Detector());
	//scanDetectors.prepend(SGMBeamline::sgm()->eVFbkDetector());




	specificScan_ = new AMXASScan();
	_pScan_ = &specificScan_;
	pScan_()->setName("SGM XAS Scan");
	pScan_()->setFilePath(pCfg_()->filePath()+pCfg_()->fileName());
	pScan_()->setFileFormat("sgm2004");
	pScan_()->setRunId(AMUser::user()->currentRunId());

	// Create space in raw data store, and create raw data channels, for each detector.

	for(int i=0; i<scanDetectors.count(); i++) {
		qDebug() << "Detector at " << i << " is " << scanDetectors.at(i)->name() ;
		AMDetectorInfo* detectorInfo = scanDetectors.at(i);
		pScan_()->rawData()->addMeasurement(AMMeasurementInfo(*detectorInfo));
		pScan_()->addRawDataSource(new AMRawDataSource(pScan_()->rawData(), i));
	}

	QList<AMDataSource*> raw1DDataSources;
	for(int i=0; i<pScan_()->rawDataSources()->count(); i++)
		if(pScan_()->rawDataSources()->at(i)->rank() == 1)
			raw1DDataSources << pScan_()->rawDataSources()->at(i);

	int rawTeyIndex = pScan_()->rawDataSources()->indexOf("tey");
	int rawTfyIndex = pScan_()->rawDataSources()->indexOf("tfy");
	int rawI0Index = pScan_()->rawDataSources()->indexOf("I0");

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

	/// \bug CRITICAL Removed creating default channels. They were never set anyway (nothing called the old AMXASScan::setDefaultChannels(); )

}

bool SGMXASScanController::isBeamlineInitialized() {
	return beamlineInitialized_;
}

bool SGMXASScanController::beamlineInitialize(){
	SGMBeamline::sgm()->exitSlitGap()->move( pCfg_()->exitSlitGap() );
	SGMBeamline::sgm()->grating()->move( pCfg_()->grating() );
	SGMBeamline::sgm()->harmonic()->move( pCfg_()->harmonic());
	SGMBeamline::sgm()->undulatorTracking()->move( pCfg_()->undulatorTracking() );
	SGMBeamline::sgm()->monoTracking()->move( pCfg_()->monoTracking() );
	SGMBeamline::sgm()->exitSlitTracking()->move( pCfg_()->exitSlitTracking() );

	AMDetectorInfo* tmpDI;
	#warning "David: Why are we using detectorSet() and not usingDetectors() besides the list versus class thing? Conversion function anyone?"
	for(int x = 0; x < pCfg_()->detectorSet()->count(); x++){
		tmpDI = pCfg_()->detectorSet()->detectorAt(x);
		#warning "David please review... Had to change because of removed AMDbObject::typeDescription"
		/* previously: typeDescription()s were never the safest way to tell what class something was anyway.
		if(tmpDI->typeDescription() == "PGT SDD Spectrum-Output Detector")
			((PGTDetector*)(pCfg_()->detectorSet()->detectorAt(x)))->setControls( (PGTDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
		else if(tmpDI->typeDescription() == "MCP Detector")
			((MCPDetector*)(pCfg_()->detectorSet()->detectorAt(x)))->setControls( (MCPDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
		else
			((AMSingleControlDetector*)(pCfg_()->detectorSet()->detectorAt(x)))->setControls( (AMDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
			*/
		// replaced with: use qobject_cast<toType*>(genericType*).  Returns 0 if genericType* is not of the toType type.
		PGTDetector* pgtDetector;
		MCPDetector* mcpDetector;
		AMSingleControlDetector* scDetector;

		if( (pgtDetector = qobject_cast<PGTDetector*>(tmpDI)) )
			pgtDetector->setControls( (PGTDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
		else if( (mcpDetector = qobject_cast<MCPDetector*>(tmpDI)) )
			mcpDetector->setControls( (MCPDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
		else if( (scDetector = qobject_cast<AMSingleControlDetector*>(tmpDI)) )
			scDetector->setControls( (AMDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );

	}

	beamlineInitialized_ = true;
	return beamlineInitialized_;
}

void SGMXASScanController::reinitialize(){
	/// \bug CRITICAL this was commented out. Why?
	delete specificScan_;

	QList<AMDetectorInfo*> scanDetectors;
	scanDetectors = pCfg_()->usingDetectors();
	//scanDetectors.prepend(SGMBeamline::sgm()->i0Detector());
	//scanDetectors.prepend(SGMBeamline::sgm()->eVFbkDetector());


	specificScan_ = new AMXASScan();
	_pScan_ = &specificScan_;
	pScan_()->setName("SGM XAS Scan");
	pScan_()->setFilePath(pCfg_()->filePath()+pCfg_()->fileName());
	pScan_()->setFileFormat("sgm2004");

	// Create space in raw data store, and create raw data channels, for each detector.

	for(int i=0; i<scanDetectors.count(); i++) {
		AMDetectorInfo* detectorInfo = scanDetectors.at(i);

		pScan_()->rawData()->addMeasurement(AMMeasurementInfo(*detectorInfo));
		pScan_()->addRawDataSource(new AMRawDataSource(pScan_()->rawData(), i));
	}

	/// \bug CRITICAL removed creating default channels. Was never used anyway.
}

SGMXASScanConfiguration* SGMXASScanController::pCfg_(){
	return *_pCfg_;
}

AMXASScan* SGMXASScanController::pScan_(){
	return *_pScan_;
}
