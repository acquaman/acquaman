#include "BioXASAppController.h"

BioXASAppController::BioXASAppController(QObject *parent) :
    AMAppController(parent)
{

}

BioXASAppController::~BioXASAppController()
{

}

void BioXASAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

void BioXASAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<CLSSIS3820ScalerDarkCurrentMeasurementActionInfo>();
//	AMDbObjectSupport::s()->registerClass<AM1DDarkCurrentCorrectionAB>();
	AMDbObjectSupport::s()->registerClass<BioXASScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<BioXASUserConfiguration>();
//	AMDbObjectSupport::s()->registerClass<BioXASXRFScanConfiguration>();
}
