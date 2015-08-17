#include "BioXASXASScanConfigurationRegionsEditor.h"

#include "util/AMEnergyToKSpaceCalculator.h"
#include "util/AMDateTimeUtils.h"

BioXASXASScanConfigurationRegionsEditor::BioXASXASScanConfigurationRegionsEditor(BioXASXASScanConfiguration *configuration, QWidget *parent) :
	BioXASXASScanConfigurationView(parent)
{
	// Initialize class variables.

	regionsView_ = 0;
	autoRegionButton_ = 0;
	pseudoXAFSButton_ = 0;
	estimatedTimePrompt_ = 0;
	estimatedTimeLabel_ = 0;

	// Create and set layouts.

	regionsViewLayout_ = new QHBoxLayout();
	estimatedTimeLayout_ = new QHBoxLayout();
	buttonsLayout_ = new QHBoxLayout();

	QHBoxLayout *extrasLayout = new QHBoxLayout();
	extrasLayout->addLayout(estimatedTimeLayout_);
	extrasLayout->addStretch();
	extrasLayout->addLayout(buttonsLayout_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addLayout(regionsViewLayout_);
	layout->addLayout(extrasLayout);

	setLayout(layout);

	// Current settings.

	setConfiguration(configuration);
}

BioXASXASScanConfigurationRegionsEditor::~BioXASXASScanConfigurationRegionsEditor()
{

}

void BioXASXASScanConfigurationRegionsEditor::setConfiguration(BioXASXASScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_)
			disconnect( configuration_, 0, this, 0 );

		configuration_ = newConfiguration;

		if (configuration_)
			connect( configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(updateEstimatedTimeLabel()) );

		refresh();

		emit configurationChanged(configuration_);
	}
}

void BioXASXASScanConfigurationRegionsEditor::clear()
{
	// Clear the regions view.

	if (regionsView_) {
		regionsViewLayout_->removeWidget(regionsView_);
		regionsView_->deleteLater();
		regionsView_ = 0;
	}

	// Clear the estimated time.

	if (estimatedTimePrompt_) {
		estimatedTimeLayout_->removeWidget(estimatedTimePrompt_);
		estimatedTimePrompt_->deleteLater();
		estimatedTimePrompt_ = 0;
	}

	if (estimatedTimeLabel_) {
		estimatedTimeLayout_->removeWidget(estimatedTimeLabel_);
		estimatedTimeLabel_->deleteLater();
		estimatedTimeLabel_ = 0;
	}

	// Clear the buttons.

	if (autoRegionButton_) {
		buttonsLayout_->removeWidget(autoRegionButton_);
		autoRegionButton_->deleteLater();
		autoRegionButton_ = 0;
	}

	if (pseudoXAFSButton_) {
		buttonsLayout_->removeWidget(pseudoXAFSButton_);
		pseudoXAFSButton_->deleteLater();
		pseudoXAFSButton_ = 0;
	}
}

void BioXASXASScanConfigurationRegionsEditor::update()
{
	updateEstimatedTimeLabel();
}

void BioXASXASScanConfigurationRegionsEditor::refresh()
{
	// Clear view.

	clear();

	// Construct new view using configuration information.

	if (configuration_) {

		regionsView_ = new AMEXAFSScanAxisView("Region Configuration", configuration_);

		estimatedTimePrompt_ = new QLabel("Estimated time: ");
		estimatedTimeLabel_ = new QLabel();

		autoRegionButton_ = new QPushButton("Auto Set XANES Regions");
		pseudoXAFSButton_ = new QPushButton("Auto Set EXAFS Regions");

		// Update layouts.

		regionsViewLayout_->addWidget(regionsView_);

		estimatedTimeLayout_->addWidget(estimatedTimePrompt_);
		estimatedTimeLayout_->addWidget(estimatedTimeLabel_);

		buttonsLayout_->addWidget(autoRegionButton_);
		buttonsLayout_->addWidget(pseudoXAFSButton_);

		// Make connections.

		connect( autoRegionButton_, SIGNAL(clicked()), this, SLOT(setupDefaultXANESRegions()) );
		connect( pseudoXAFSButton_, SIGNAL(clicked()), this, SLOT(setupDefaultEXAFSRegions()) );

	}

	// Update view.

	update();
}

void BioXASXASScanConfigurationRegionsEditor::updateEstimatedTimeLabel()
{
	if (configuration_ && estimatedTimeLabel_) {
		estimatedTimeLabel_->setText(AMDateTimeUtils::convertTimeToString(configuration_->totalTime()));
	}
}

void BioXASXASScanConfigurationRegionsEditor::setupDefaultXANESRegions()
{
	if (configuration_ && regionsView_) {

		// Clear regions view of any previous regions.

		while (configuration_->scanAxisAt(0)->regionCount())
		{
			regionsView_->removeEXAFSRegion(0);
		}

		// Create new region and add to the regions view.

		double edgeEnergy = configuration_->energy();

		AMScanAxisEXAFSRegion *region = createDefaultXANESRegion(edgeEnergy);
		regionsView_->insertEXAFSRegion(0, region);
	}
}

void BioXASXASScanConfigurationRegionsEditor::setupDefaultEXAFSRegions()
{
	if (configuration_ && regionsView_) {

		// Clear regions view of any previous regions.

		while (configuration_->scanAxisAt(0)->regionCount())
		{
			regionsView_->removeEXAFSRegion(0);
		}

		// Create new regions and add to the regions view.

		AMScanAxisEXAFSRegion *newRegion;
		double edgeEnergy = configuration_->energy();

		newRegion = createEXAFSRegion(edgeEnergy, edgeEnergy - 200, 10, edgeEnergy - 30, 1.0);
		regionsView_->insertEXAFSRegion(0, newRegion);

		newRegion = createEXAFSRegion(edgeEnergy, edgeEnergy - 30, 0.5, edgeEnergy + 40, 1.0);
		regionsView_->insertEXAFSRegion(1, newRegion);

		newRegion = createEXAFSRegionInKSpace(edgeEnergy, AMEnergyToKSpaceCalculator::k(edgeEnergy, edgeEnergy + 40), 0.05, 10, 1.0, 10.0);
		regionsView_->insertEXAFSRegion(2, newRegion);
	}
}

AMScanAxisEXAFSRegion* BioXASXASScanConfigurationRegionsEditor::createDefaultXANESRegion(double edgeEnergy)
{
	return createXANESRegion(edgeEnergy, edgeEnergy - 30, 0.5, edgeEnergy + 40, 1.0);
}

AMScanAxisEXAFSRegion* BioXASXASScanConfigurationRegionsEditor::createXANESRegion(double edgeEnergy, double regionStart, double regionStep, double regionEnd, double regionTime) const
{
	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	region->setEdgeEnergy(edgeEnergy);
	region->setRegionStart(regionStart);
	region->setRegionStep(regionStep);
	region->setRegionEnd(regionEnd);
	region->setRegionTime(regionTime);

	return region;
}

AMScanAxisEXAFSRegion* BioXASXASScanConfigurationRegionsEditor::createEXAFSRegion(double edgeEnergy, double regionStart, double regionStep, double regionEnd, double regionTime) const
{
	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	region->setEdgeEnergy(edgeEnergy);
	region->setRegionStart(regionStart);
	region->setRegionStep(regionStep);
	region->setRegionEnd(regionEnd);
	region->setRegionTime(regionTime);

	return region;
}

AMScanAxisEXAFSRegion* BioXASXASScanConfigurationRegionsEditor::createEXAFSRegionInKSpace(double edgeEnergy, double regionStart, double regionStep, double regionEnd, double regionTime, double maximumTime) const
{
	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	region->setEdgeEnergy(edgeEnergy);
	region->setInKSpace(true);
	region->setRegionStart(regionStart);
	region->setRegionStep(regionStep);
	region->setRegionEnd(regionEnd);
	region->setRegionTime(regionTime);
	region->setMaximumTime(maximumTime);

	return region;
}
