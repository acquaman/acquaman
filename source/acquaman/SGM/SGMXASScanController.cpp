#include "SGMXASScanController.h"

SGMXASScanController::SGMXASScanController(SGMXASScanConfiguration *cfg){
	specificCfg_ = pCfg_ = cfg;
	beamlineInitialized_ = false;

	QStringList scanDetectors;
	scanDetectors << "eV_Fbk" << "reading" << "tfy" << "pgt";
	/*
	  BIG NOTE TO DAVE:
	  YOU NEW'D THE SCAN ... SOMEONE ELSE HAS TO TAKE OWNERSHIP OF IT FOR DELETION
	  opts: function call, new in scan viewer ...
	  */
	scan_ = new AMXASScan(scanDetectors);
	scan_->setName("Dave's Scan");
	scan_->addChannel("eV", "eV");
	scan_->addChannel("Jitter", "eV_Fbk");
	scan_->addChannel("TEY", "reading");
	scan_->addChannel("TFY", "tfy");
	scan_->addChannel("PGT", "pgt");
}

bool SGMXASScanController::beamlineInitialize(){
	SGMBeamline::sgm()->exitSlitGap()->move( pCfg_->exitSlitGap() );
	SGMBeamline::sgm()->grating()->move( pCfg_->grating() );
	SGMBeamline::sgm()->undulatorTracking()->move( pCfg_->undulatorTracking() );
	SGMBeamline::sgm()->monoTracking()->move( pCfg_->monoTracking() );
	SGMBeamline::sgm()->exitSlitTracking()->move( pCfg_->exitSlitTracking() );
	beamlineInitialized_ = true;
	return beamlineInitialized_;
}
