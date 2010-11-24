#include "SGMXASScanController.h"

#include "dataman/SGM2004FileLoader.h"

/// If this had a one-line documented comment, I would know how to interpret these two strings. Are they the name and the channel expression? The expression and the x-expression?
typedef QPair<QString, QString> chPair;

SGMXASScanController::SGMXASScanController(SGMXASScanConfiguration *cfg){
	specificCfg_ = cfg;
	_pCfg_ = & specificCfg_;
	beamlineInitialized_ = false;

	QList<AMDetectorInfo*> scanDetectors = pCfg_()->usingDetectors();
	scanDetectors.prepend(SGMBeamline::sgm()->i0Detector());
	scanDetectors.prepend(SGMBeamline::sgm()->eVFbkDetector());

	/// \bug CRITICAL create columns in scan's rawData() and rawDataSources(), based on scanDetectors.  Remove detectors from dataman stuff, or expand role inside scan configuration. (application: XES detector?)

	specificScan_ = new AMXASScan(scanDetectors);
	_pScan_ = &specificScan_;
	pScan_()->setName("SGM XAS Scan");
	pScan_()->setFilePath(pCfg_()->filePath()+pCfg_()->fileName());
	pScan_()->setFileFormat("sgm2004");

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
	//delete specificScan_;

	QList<AMDetectorInfo*> scanDetectors;
	scanDetectors = pCfg_()->usingDetectors();
	scanDetectors.prepend(SGMBeamline::sgm()->i0Detector());
	scanDetectors.prepend(SGMBeamline::sgm()->eVFbkDetector());

	/// \bug CRITICAL removed creating default channels. Was never used anyway.

	/*
	  BIG NOTE TO DAVE:
	  YOU NEW'D THE SCAN ... SOMEONE ELSE HAS TO TAKE OWNERSHIP OF IT FOR DELETION
	  opts: function call, new in scan viewer ...
	  */
	specificScan_ = new AMXASScan(scanDetectors);
	_pScan_ = &specificScan_;
	pScan_()->setName("SGM XAS Scan");
	pScan_()->setFilePath(pCfg_()->filePath()+pCfg_()->fileName());
	pScan_()->setFileFormat("sgm2004");

}

SGMXASScanConfiguration* SGMXASScanController::pCfg_(){
	return *_pCfg_;
}

AMXASScan* SGMXASScanController::pScan_(){
	return *_pScan_;
}
