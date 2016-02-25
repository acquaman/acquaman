#include "BioXASXASScanActionController.h"

#include "acquaman/AMEXAFSScanActionControllerAssembler.h"
#include "acquaman/BioXAS/BioXASXASScanConfiguration.h"

#include "beamline/AMDetector.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "beamline/CLS/CLSStorageRing.h"

BioXASXASScanActionController::BioXASXASScanActionController(BioXASXASScanConfiguration *configuration, QObject *parent) :
	AMGenericStepScanController(configuration, parent)
{
	bioXASConfiguration_ = configuration;

	useFeedback_ = true;

	scan_->setNotes(scanNotes());

	if (bioXASConfiguration_) {
		AMExporterOptionXDIFormat *bioXASDefaultXAS = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", bioXASConfiguration_->edge().split(" ").first(), bioXASConfiguration_->edge().split(" ").last(), true);

		if (bioXASDefaultXAS->id() > 0)
			AMAppControllerSupport::registerClass<BioXASXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXAS->id());
	}
}

BioXASXASScanActionController::~BioXASXASScanActionController()
{

}

QString BioXASXASScanActionController::scanNotes()
{
	QString notes;

	// Note the storage ring current.

	notes.append(QString("SR1 Current:\t%1 mA\n").arg(QString::number(CLSStorageRing::sr1()->ringCurrent(), 'f', 1)));

	// Note the mono settling time, if applicable.

	BioXASSSRLMonochromator *mono = BioXASBeamline::bioXAS()->mono();
	if (mono) {
		double settlingTime = mono->bragg()->settlingTime();
		if (settlingTime > 0)
			notes.append(QString("Settling time:\t%1 s\n").arg(settlingTime));
	}

	return notes;
}

void BioXASXASScanActionController::createScanAssembler()
{
	scanAssembler_ = new AMEXAFSScanActionControllerAssembler(this);
}

void BioXASXASScanActionController::buildScanControllerImplementation()
{
	BioXASBeamline::bioXAS()->buildScan(configuration_, scan_);
}
