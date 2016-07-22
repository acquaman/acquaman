#include "BioXASGenericStepScanConfiguration.h"

#include "acquaman/BioXAS/BioXASGenericStepScanController.h"
#include "beamline/BioXAS/BioXASBeamline.h"

BioXASGenericStepScanConfiguration::BioXASGenericStepScanConfiguration(QObject *parent) :
	AMGenericStepScanConfiguration(parent), BioXASScanConfiguration()
{

}

BioXASGenericStepScanConfiguration::BioXASGenericStepScanConfiguration(const BioXASGenericStepScanConfiguration &original) :
	AMGenericStepScanConfiguration(original), BioXASScanConfiguration(original)
{

}

BioXASGenericStepScanConfiguration::~BioXASGenericStepScanConfiguration()
{

}

AMScanConfiguration* BioXASGenericStepScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new BioXASGenericStepScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController* BioXASGenericStepScanConfiguration::createController()
{
	AMScanActionController *controller = new BioXASGenericStepScanController(this);
	controller->buildScanController();

	return controller;
}

bool BioXASGenericStepScanConfiguration::usingAnyGeDetector() const
{
	bool usingGeDetector = false;

	AMDetectorSet *geDetectors = BioXASBeamline::bioXAS()->ge32ElementDetectors();
	if (geDetectors && !geDetectors->isEmpty()) {

		for (int i = 0, count = geDetectors->count(); i < count && !usingGeDetector; i++)
			usingGeDetector = geDetectors->at(i) && usingDetector(geDetectors->at(i)->name());
	}

	return usingGeDetector;
}

bool BioXASGenericStepScanConfiguration::usingScaler() const
{
	bool result = false;

	BioXASBeamline *bioxas = BioXASBeamline::bioXAS();
	CLSSIS3820Scaler *scaler = bioxas->scaler();
	if (scaler) {
		AMDetector *i0Detector = bioxas->i0Detector();
		AMDetector *i1Detector = bioxas->i1Detector();
		AMDetector *i2Detector = bioxas->i2Detector();

		result =   (i0Detector && usingDetector(i0Detector->name()))
				|| (i1Detector && usingDetector(i1Detector->name()))
				|| (i2Detector && usingDetector(i2Detector->name()));
	}

	return result;
}

bool BioXASGenericStepScanConfiguration::usingZebra() const
{
	bool result = false;

	BioXASZebra *zebra = BioXASBeamline::bioXAS()->zebra();
	if (zebra)
		result = ( usingAnyGeDetector() || usingScaler() );

	return result;
}

bool BioXASGenericStepScanConfiguration::usingMono() const
{
	bool result = false;

	BioXASMonochromator *mono = BioXASBeamline::bioXAS()->mono();
	if (mono) {
		BioXASMonochromatorEnergyControl *energy = mono->energy();
		if (energy) {
			result =   usingControl(energy->toInfo())
					|| (energy->braggControl() && usingControl(energy->braggControl()->toInfo()));
		}
	}

	return result;
}
