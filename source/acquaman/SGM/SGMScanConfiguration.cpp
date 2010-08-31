#include "SGMScanConfiguration.h"

SGMScanConfiguration::SGMScanConfiguration()
{
	setExitSlitGap(SGMBeamline::sgm()->exitSlitGap()->value());
	setGrating((SGMBeamline::sgm()->grating()->value() < 1) ? SGMBeamline::lowGrating : ((SGMBeamline::sgm()->grating()->value() < 2) ? SGMBeamline::mediumGrating : SGMBeamline::highGrating));
	setHarmonic((SGMBeamline::sgm()->harmonic()->value() < 1) ? SGMBeamline::firstHarmonic : ((SGMBeamline::sgm()->grating()->value() < 2) ? SGMBeamline::firstHarmonic : SGMBeamline::thirdHarmonic));
	setUndulatorTracking(SGMBeamline::sgm()->undulatorTracking()->value());
	setMonoTracking(SGMBeamline::sgm()->monoTracking()->value());
	setExitSlitTracking(SGMBeamline::sgm()->exitSlitTracking()->value());
	if(SGMBeamline::sgm()->teyDetector())
		setUsingTEY(SGMBeamline::sgm()->XASDetectors()->isDefaultByName(SGMBeamline::sgm()->teyDetector()->name()) );
	if(SGMBeamline::sgm()->tfyDetector())
		setUsingTFY(SGMBeamline::sgm()->XASDetectors()->isDefaultByName(SGMBeamline::sgm()->tfyDetector()->name()) );
	if(SGMBeamline::sgm()->pgtDetector())
		setUsingPGT(SGMBeamline::sgm()->XASDetectors()->isDefaultByName(SGMBeamline::sgm()->pgtDetector()->name()) );
}

bool SGMScanConfiguration::setExitSlitGap(double exitSlitGap){
	if(SGMBeamline::sgm()->exitSlitGap()->valueOutOfRange(exitSlitGap))
		return FALSE;
	exitSlitGap_ = exitSlitGap;
	return TRUE;
}
