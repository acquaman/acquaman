#include "SGMXASScanController.h"

#include "dataman/SGM2004FileLoader.h"

typedef QPair<QString, QString> chPair;

SGMXASScanController::SGMXASScanController(SGMXASScanConfiguration *cfg){
	specificCfg_ = cfg;
	_pCfg_ = & specificCfg_;
	beamlineInitialized_ = false;

	QList<AMDetectorInfo*> scanDetectors;
	scanDetectors = pCfg_()->usingDetectors();
	scanDetectors.prepend(SGMBeamline::sgm()->i0Detector());
	scanDetectors.prepend(SGMBeamline::sgm()->eVFbkDetector());

	QList<QPair<QString, QString> > scanChannels;
	scanChannels = pCfg_()->defaultChannels();

	/*
	  BIG NOTE TO DAVE:
	  YOU NEW'D THE SCAN ... SOMEONE ELSE HAS TO TAKE OWNERSHIP OF IT FOR DELETION
	  opts: function call, new in scan viewer ...
	  */
	specificScan_ = new AMXASScan(scanDetectors);
	_pScan_ = &specificScan_;
	pScan_()->setName("SGM XAS Scan");
	pScan_()->setMetaData("filePath", pCfg_()->filePath()+pCfg_()->fileName());
	pScan_()->setMetaData("fileFormat", "sgm2004");

	foreach(chPair tmpCh, scanChannels){
		pScan_()->addChannel(tmpCh.first, tmpCh.second);
	}

	/*
	pScan_()->addChannel("eV", "eV");
	foreach(AMDetectorInfo *dtctr, scanDetectors){
		if(!dtctr->isSpectralOutput())
			pScan_()->addChannel(dtctr->name().toUpper(), dtctr->name());
		// What to do if it is spectral?
	}
	*/
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
	for(int x = 0; x < pCfg_()->detectorSet()->count(); x++){
		tmpDI = pCfg_()->detectorSet()->detectorAt(x);
		if(tmpDI->typeDescription() == "PGT SDD Spectrum-Output Detector")
			((PGTDetector*)(pCfg_()->detectorSet()->detectorAt(x)))->setControls( (PGTDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
		else if(tmpDI->typeDescription() == "MCP Detector")
			((MCPDetector*)(pCfg_()->detectorSet()->detectorAt(x)))->setControls( (MCPDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
		else
			((AMSingleControlDetector*)(pCfg_()->detectorSet()->detectorAt(x)))->setControls( (AMDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
	}

	beamlineInitialized_ = true;
	return beamlineInitialized_;
}

void SGMXASScanController::reinitialize(){
	//delete specificScan_;

	QList<AMDetectorInfo*> scanDetectors;
	scanDetectors = pCfg_()->usingDetectors();
	scanDetectors.prepend(SGMBeamline::sgm()->i0Detector());
	scanDetectors.prepend(SGMBeamline::sgm()->eVFbkDetector());

	QList<QPair<QString, QString> > scanChannels;
	scanChannels = pCfg_()->defaultChannels();

	/*
	  BIG NOTE TO DAVE:
	  YOU NEW'D THE SCAN ... SOMEONE ELSE HAS TO TAKE OWNERSHIP OF IT FOR DELETION
	  opts: function call, new in scan viewer ...
	  */
	specificScan_ = new AMXASScan(scanDetectors);
	_pScan_ = &specificScan_;
	pScan_()->setName("SGM XAS Scan");
	pScan_()->setMetaData("filePath", pCfg_()->filePath()+pCfg_()->fileName());
	pScan_()->setMetaData("fileFormat", "sgm2004");

	foreach(chPair tmpCh, scanChannels){
		pScan_()->addChannel(tmpCh.first, tmpCh.second);
	}
}

SGMXASScanConfiguration* SGMXASScanController::pCfg_(){
	return *_pCfg_;
}

AMXASScan* SGMXASScanController::pScan_(){
	return *_pScan_;
}
