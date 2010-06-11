#include "SGMXASScanController.h"

SGMXASScanController::SGMXASScanController(SGMXASScanConfiguration *cfg){
	specificCfg_ = cfg;
	_pCfg_ = & specificCfg_;
	beamlineInitialized_ = false;

	QList<AMAbstractDetector*> scanDetectors;
	scanDetectors = pCfg_()->usingDetectors();
	scanDetectors.prepend(SGMBeamline::sgm()->i0Detector());
	scanDetectors.prepend(SGMBeamline::sgm()->eVFbkDetector());

	/*
	  BIG NOTE TO DAVE:
	  YOU NEW'D THE SCAN ... SOMEONE ELSE HAS TO TAKE OWNERSHIP OF IT FOR DELETION
	  opts: function call, new in scan viewer ...
	  */
	specificScan_ = new AMXASScan(scanDetectors);
	_pScan_ = &specificScan_;
	pScan_()->setName("Dave's Scan");
	pScan_()->addChannel("eV", "eV");
	foreach(AMAbstractDetector *dtctr, scanDetectors){
		if(!dtctr->isSpectralOutput())
			pScan_()->addChannel(dtctr->name().toUpper(), dtctr->name());
		// What to do if it is spectral?
	}
	pScan_()->addChannel("bigTEYNorm", "10000*tey/I0");
	pScan_()->addChannel("bigTFYNorm", "-10000*tfy/I0");

/*
	pScan_()->addChannel("bigTeyNorm", "10000*tey/I0");
	pScan_()->addChannel("pgtTest1", "pgt.pgtCounts[130]");
	pScan_()->addChannel("pgtTest2", "pgt[130].pgtCounts");


	QString indexer = "";
	for(int x = 0; x < 1024; x++){
		indexer.setNum(x);
		pScan_()->addChannel("PGT_COUNTS"+indexer, "pgt.pgtCounts["+indexer+"]");
	}
	*/
}

bool SGMXASScanController::beamlineInitialize(){
	SGMBeamline::sgm()->exitSlitGap()->move( pCfg_()->exitSlitGap() );
	SGMBeamline::sgm()->grating()->move( pCfg_()->grating() );
	SGMBeamline::sgm()->harmonic()->move( pCfg_()->harmonic());
	SGMBeamline::sgm()->undulatorTracking()->move( pCfg_()->undulatorTracking() );
	SGMBeamline::sgm()->monoTracking()->move( pCfg_()->monoTracking() );
	SGMBeamline::sgm()->exitSlitTracking()->move( pCfg_()->exitSlitTracking() );
	beamlineInitialized_ = true;
	return beamlineInitialized_;
}
