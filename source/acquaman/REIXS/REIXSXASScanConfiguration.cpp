#include "REIXSXASScanConfiguration.h"
#include <QDebug>

REIXSXASScanConfiguration::REIXSXASScanConfiguration(QObject *parent) :
	AMXASScanConfiguration(parent)
{
	regions()->appendRegion(380, 0.1, 420, 1);
	regions()->setSensibleRange(100, 1000);

	scanNumber_ = 0;
	sampleId_ = -1;
	namedAutomatically_ = true;

	slitWidth_ = 50;
	applySlitWidth_ = false;
	monoGrating_ = 0;
	applyMonoGrating_ = false;
	monoMirror_ = 0;
	applyMonoMirror_ = false;
	polarization_ = 3;
	polarizationAngle_ = -90;
	applyPolarization_ = false;
}

REIXSXASScanConfiguration::~REIXSXASScanConfiguration() {

}

REIXSXASScanConfiguration::REIXSXASScanConfiguration(const REIXSXASScanConfiguration &other)
	: AMXASScanConfiguration(other)
{
	/// \todo This belongs in AMRegionScanConfiguration: copy the regions.
	regions_->setSensibleStart(other.regions()->sensibleStart());
	regions_->setSensibleEnd(other.regions()->sensibleEnd());
	regions_->setDefaultUnits(other.regions()->defaultUnits());
	regions_->setDefaultTimeUnits(other.regions()->defaultTimeUnits());
	for(int x = 0; x < other.regionCount(); x++)
		regions_->addRegion(x, other.regionStart(x), other.regionDelta(x), other.regionEnd(x), other.regionTime(x));

	scanNumber_ = other.scanNumber_;
	sampleId_ = other.sampleId_;
	namedAutomatically_ = other.namedAutomatically_;
	////////////////////////
}

AMScanConfiguration * REIXSXASScanConfiguration::createCopy() const
{
	// using default copy constructor:
	return new REIXSXASScanConfiguration(*this);
}

#include "acquaman/REIXS/REIXSXASScanController.h"
AMScanController * REIXSXASScanConfiguration::createController()
{
	return new REIXSXASScanController(this);
}

#include "ui/dataman/AMRegionScanConfigurationView.h"
AMScanConfigurationView * REIXSXASScanConfiguration::createView()
{
	return new AMRegionScanConfigurationView(this);
}
