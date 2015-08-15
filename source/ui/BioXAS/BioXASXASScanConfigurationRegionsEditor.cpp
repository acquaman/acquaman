#include "BioXASXASScanConfigurationRegionsEditor.h"

#include "util/AMDateTimeUtils.h"

BioXASXASScanConfigurationRegionsEditor::BioXASXASScanConfigurationRegionsEditor(BioXASXASScanConfiguration *configuration, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	configuration_ = 0;

	regionsView_ = 0;
	autoRegionButton_ = 0;
	pseudoXAFSButton_ = 0;
	estimatedTimeLabel_ = 0;

	// Create UI elements.

	regionsView_ = new AMEXAFSScanAxisView("Region Configuration", 0);

	autoRegionButton_ = new QPushButton("Auto Set XANES Regions");
	pseudoXAFSButton_ = new QPushButton("Auto Set EXAFS Regions");

	QLabel *estimatedTimePrompt = new QLabel("Estimated time: ");
	estimatedTimeLabel_ = new QLabel();

	// Create and set layouts.

	QHBoxLayout *regionsButtonsLayout = new QHBoxLayout();
	regionsButtonsLayout->addWidget(autoRegionButton_);
	regionsButtonsLayout->addWidget(pseudoXAFSButton_);

	QHBoxLayout *regionsExtrasLayout = new QHBoxLayout();
	regionsExtrasLayout->addWidget(estimatedTimePrompt);
	regionsExtrasLayout->addWidget(estimatedTimeLabel_);
	regionsExtrasLayout->addStretch();
	regionsExtrasLayout->addLayout(regionsButtonsLayout);

	layout_ = new QVBoxLayout();
	layout_->setMargin(0);
	layout_->addWidget(regionsView_);
	layout_->addLayout(regionsExtrasLayout);

	setLayout(layout_);

	// Current settings.

	setConfiguration(configuration);
}

BioXASXASScanConfigurationRegionsEditor::~BioXASXASScanConfigurationRegionsEditor()
{

}

void BioXASXASScanConfigurationRegionsEditor::setConfiguration(BioXASXASScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_) {

			disconnect( configuration_, 0, this, 0 );

		}

		configuration_ = newConfiguration;

		if (configuration_) {

			connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged(double)));

		}

		refresh();

		emit configurationChanged(configuration_);
	}
}

void BioXASXASScanConfigurationRegionsEditor::clear()
{

}

void BioXASXASScanConfigurationRegionsEditor::update()
{
	updateRegionsView();
	updateRegionsButtons();
	updateEstimatedTimeLabel();
}

void BioXASXASScanConfigurationRegionsEditor::refresh()
{
	// Clear view.

	clear();

	if (configuration_) {


	}

	// Update view.

	update();
}

void BioXASXASScanConfigurationRegionsEditor::clearRegionsView()
{
	if (regionsView_) {

	}
}

void BioXASXASScanConfigurationRegionsEditor::updateRegionsView()
{
	if (configuration_) {

	}
}

void BioXASXASScanConfigurationRegionsEditor::updateRegionsButtons()
{
	if (configuration_) {
		autoRegionButton_->setEnabled(true);
		pseudoXAFSButton_->setEnabled(true);

	} else {
		autoRegionButton_->setEnabled(false);
		pseudoXAFSButton_->setEnabled(false);
	}
}

void BioXASXASScanConfigurationRegionsEditor::updateEstimatedTimeLabel()
{
	if (configuration_) {
		estimatedTimeLabel_->setText(AMDateTimeUtils::convertTimeToString(configuration_->totalTime()));

	} else {
		estimatedTimeLabel_->setText(QString());
	}
}
