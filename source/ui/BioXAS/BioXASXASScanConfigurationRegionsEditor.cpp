#include "BioXASXASScanConfigurationRegionsEditor.h"

#include "util/AMEnergyToKSpaceCalculator.h"
#include "util/AMDateTimeUtils.h"

BioXASXASScanConfigurationRegionsEditor::BioXASXASScanConfigurationRegionsEditor(BioXASXASScanConfiguration *configuration, QWidget *parent) :
	BioXASXASScanConfigurationView(parent)
{
	// Initialize class variables.

	regionsView_ = 0;

	// Create UI elements.

	QLabel *estimatedTimePrompt = new QLabel("Estimated time: ");
	estimatedTimeLabel_ = new QLabel();

	xanesButton_ = new QPushButton("Auto Set XANES Regions");
	exafsButton_ = new QPushButton("Auto Set EXAFS Regions");

	// Create and set layouts.

	regionsViewLayout_ = new QHBoxLayout();

	QHBoxLayout *estimatedTimeLayout = new QHBoxLayout();
	estimatedTimeLayout->addWidget(estimatedTimePrompt);
	estimatedTimeLayout->addWidget(estimatedTimeLabel_);

	QHBoxLayout *buttonsLayout = new QHBoxLayout();
	buttonsLayout->addWidget(xanesButton_);
	buttonsLayout->addWidget(exafsButton_);

	QHBoxLayout *extrasLayout = new QHBoxLayout();
	extrasLayout->addLayout(estimatedTimeLayout);
	extrasLayout->addStretch();
	extrasLayout->addLayout(buttonsLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addLayout(regionsViewLayout_);
	layout->addLayout(extrasLayout);

	setLayout(layout);

	// Make connections.

	connect( xanesButton_, SIGNAL(clicked()), this, SLOT(onXANESButtonClicked()) );
	connect( exafsButton_, SIGNAL(clicked()), this, SLOT(onEXAFSButtonClicked()) );

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

		if (configuration_) {
			connect( configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(updateEstimatedTimeLabel()) );
			connect( configuration_, SIGNAL(scanAxisAdded(AMScanAxis*)), this, SLOT(onConfigurationScanAxisAdded(AMScanAxis*)) );
			connect( configuration_, SIGNAL(scanAxisRemoved(AMScanAxis*)), this, SLOT(onConfigurationScanAxisRemoved(AMScanAxis*)) );
		}

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

	estimatedTimeLabel_->clear();
}

void BioXASXASScanConfigurationRegionsEditor::update()
{
	updateEstimatedTimeLabel();
	updateXANESButton();
	updateEXAFSButton();
}

void BioXASXASScanConfigurationRegionsEditor::refresh()
{
	// Clear view.

	clear();

	// Construct new view using configuration information.

	if (configuration_) {

		if (configuration_->scanAxisAt(0))
			regionsView_ = new AMEXAFSScanAxisView("Region Configuration", configuration_);

		// Update layouts.

		regionsViewLayout_->addWidget(regionsView_);
	}

	// Update view.

	update();
}

void BioXASXASScanConfigurationRegionsEditor::updateEstimatedTimeLabel()
{
	double newTime = 0;

	if (configuration_)
		newTime = configuration_->totalTime();

	if (estimatedTimeLabel_)
		estimatedTimeLabel_->setText(AMDateTimeUtils::convertTimeToString(newTime));
}

void BioXASXASScanConfigurationRegionsEditor::updateXANESButton()
{
	bool isEnabled = false;

	if (configuration_)
		isEnabled = true;

	if (xanesButton_)
		xanesButton_->setEnabled(isEnabled);
}

void BioXASXASScanConfigurationRegionsEditor::updateEXAFSButton()
{
	bool isEnabled = false;

	if (configuration_)
		isEnabled = true;

	if (exafsButton_)
		exafsButton_->setEnabled(isEnabled);
}

void BioXASXASScanConfigurationRegionsEditor::onXANESButtonClicked()
{
	if (configuration_)
		configuration_->setupDefaultXANESRegions();
}

void BioXASXASScanConfigurationRegionsEditor::onEXAFSButtonClicked()
{
	if (configuration_)
		configuration_->setupDefaultEXAFSRegions();
}

void BioXASXASScanConfigurationRegionsEditor::onConfigurationScanAxisAdded(AMScanAxis *newAxis)
{
	if (newAxis) {
		connect( newAxis, SIGNAL(regionAdded(AMScanAxisRegion*)), this, SLOT(refresh()) );
		connect( newAxis, SIGNAL(regionOverwritten(AMScanAxisRegion*)), this, SLOT(refresh()) );
		connect( newAxis, SIGNAL(regionRemoved(AMScanAxisRegion*)), this, SLOT(refresh()) );
		connect( newAxis, SIGNAL(regionsChanged()), this, SLOT(refresh()) );
	}
}

void BioXASXASScanConfigurationRegionsEditor::onConfigurationScanAxisRemoved(AMScanAxis *axis)
{
	if (axis) {
		disconnect( axis, 0, this, 0 );
	}
}

