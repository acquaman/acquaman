#include "SGMXASScanController.h"

SGMXASScanController::SGMXASScanController(SGMXASScanConfiguration *cfg){
	specificCfg_ = cfg;
	_pCfg_ = & specificCfg_;
	beamlineInitialized_ = false;

	QStringList scanDetectors;
//	scanDetectors << "eV_Fbk" << "reading" << "tfy" << "pgt";
	scanDetectors = pCfg_()->usingDetectors();
	scanDetectors.prepend("eV_Fbk");

	qDebug() << "CURRENT DETECTORS ARE " << scanDetectors;
	/*
	  BIG NOTE TO DAVE:
	  YOU NEW'D THE SCAN ... SOMEONE ELSE HAS TO TAKE OWNERSHIP OF IT FOR DELETION
	  opts: function call, new in scan viewer ...
	  */
	specificScan_ = new AMXASScan(scanDetectors);
	_pScan_ = &specificScan_;
	pScan_()->setName("Dave's Scan");
	pScan_()->addChannel("eV", "eV");
	foreach(QString str, scanDetectors){
		if(str != SGMBeamline::sgm()->pgtDetector()->name())
			pScan_()->addChannel(str.toUpper(), str);
		else{
//			pScan_()->addChannel("PGT_COUNTS", "pgt.pgtCounts[199]");

			QString indexer = "";
			for(int x = 0; x < 1024; x++){
				indexer.setNum(x);
				pScan_()->addChannel("PGT_COUNTS"+indexer, "pgt.pgtCounts["+indexer+"]");
			}

		}
	}
	pScan_()->addChannel("pgtTest1", "pgt.pgtCounts[130]");
	pScan_()->addChannel("pgtTest2", "pgt[130].pgtCounts");

//	pScan_()->addChannel("eV", "eV");
//	pScan_()->addChannel("Jitter", "eV_Fbk");
//	pScan_()->addChannel("TEY", "reading");
//	pScan_()->addChannel("TFY", "tfy");
//	pScan_()->addChannel("PGT", "pgt");
}

bool SGMXASScanController::beamlineInitialize(){
	qDebug() << "Initializing to slit gap: " << pCfg_()->exitSlitGap() << " und track: " << pCfg_()->undulatorTracking() << " mono track: " << pCfg_()->monoTracking() << " slit track: " << pCfg_()->exitSlitTracking();
	SGMBeamline::sgm()->exitSlitGap()->move( pCfg_()->exitSlitGap() );
	SGMBeamline::sgm()->grating()->move( pCfg_()->grating() );
	qDebug() << "Trying to set harmonic to " << pCfg_()->harmonic();
	SGMBeamline::sgm()->harmonic()->move( pCfg_()->harmonic());
	SGMBeamline::sgm()->undulatorTracking()->move( pCfg_()->undulatorTracking() );
	SGMBeamline::sgm()->monoTracking()->move( pCfg_()->monoTracking() );
	SGMBeamline::sgm()->exitSlitTracking()->move( pCfg_()->exitSlitTracking() );
	beamlineInitialized_ = true;
	return beamlineInitialized_;
}
