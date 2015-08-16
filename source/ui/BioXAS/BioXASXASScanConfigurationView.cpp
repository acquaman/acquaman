#include "BioXASXASScanConfigurationView.h"

#include "acquaman/AMScanController.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "ui/dataman/AMEXAFSScanAxisView.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMEnergyToKSpaceCalculator.h"
#include "util/AMPeriodicTable.h"
#include "util/AMDateTimeUtils.h"


BioXASXASScanConfigurationView::BioXASXASScanConfigurationView(BioXASXASScanConfiguration *configuration, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	// Initialize member variables.

	configuration_ = 0;

	// Create scan name view.

	QLabel *scanNamePrompt = new QLabel("Name: ");

	scanName_ = new QLineEdit();

	QHBoxLayout *scanNameLayout = new QHBoxLayout();
	scanNameLayout->addWidget(scanNamePrompt);
	scanNameLayout->addWidget(scanName_);
	scanNameLayout->addStretch();

	// Create energy editor.

	energyEditor_ = new BioXASXASScanConfigurationEnergyEditor(0);

	// Create regions editor.

	regionsEditor_ = new BioXASXASScanConfigurationRegionsEditor(0);

	// Create detectors view.

	detectorsView_ = new AMGenericStepScanConfigurationDetectorsView(0, AMBeamline::bl()->exposedDetectors());

	// Create and set main layouts

	QVBoxLayout *scanSettingsBoxLayout = new QVBoxLayout();
	scanSettingsBoxLayout->addLayout(scanNameLayout);
	scanSettingsBoxLayout->addWidget(energyEditor_);
	scanSettingsBoxLayout->addWidget(regionsEditor_);

	QGroupBox *scanSettingsBox = new QGroupBox("Scan settings");
	scanSettingsBox->setLayout(scanSettingsBoxLayout);

	QVBoxLayout *detectorBoxLayout = new QVBoxLayout();
	detectorBoxLayout->addWidget(detectorsView_);

	QGroupBox *detectorBox = new QGroupBox("Detectors");
	detectorBox->setLayout(detectorBoxLayout);

	QScrollArea *detectorScrollArea = new QScrollArea;
	detectorScrollArea->setWidget(detectorBox);
	detectorScrollArea->setFrameStyle(QFrame::NoFrame);

	QHBoxLayout *finalHorizontalLayout = new QHBoxLayout();
	finalHorizontalLayout->addWidget(scanSettingsBox);
	finalHorizontalLayout->addWidget(detectorScrollArea);
	finalHorizontalLayout->setContentsMargins(20,0,0,20);
	finalHorizontalLayout->setSpacing(1);

	QVBoxLayout *finalVerticalLayout = new QVBoxLayout();
	finalVerticalLayout->addStretch();
	finalVerticalLayout->addLayout(finalHorizontalLayout);
	finalVerticalLayout->addStretch();

	setLayout(finalVerticalLayout);

	// Make connections.

	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));

	// Current settings.

	setConfiguration(configuration);
}

BioXASXASScanConfigurationView::~BioXASXASScanConfigurationView()
{

}

const AMScanConfiguration* BioXASXASScanConfigurationView::configuration() const
{
	return configuration_;
}

void BioXASXASScanConfigurationView::setConfiguration(BioXASXASScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_) {

			disconnect( configuration_, 0, this, 0 );

		}

		configuration_ = newConfiguration;


		if (configuration_) {

			connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));

		}

		refresh();

		emit configurationChanged(configuration_);
	}
}

void BioXASXASScanConfigurationView::clear()
{
	scanName_->clear();
	energyEditor_->clear();
	regionsEditor_->clear();
	detectorsView_->clear();
}

void BioXASXASScanConfigurationView::update()
{

	if (configuration_) {

		// Update scan name.

		scanName_->setText(configuration_->userScanName());

		// Update energy selection.

		energyEditor_->update();

		// Update regions editor.

		regionsEditor_->update();

		// Update detectors view.

		detectorsView_->update();
	}
}

void BioXASXASScanConfigurationView::refresh()
{
	// Clear view.

	clear();

	if (configuration_) {
		energyEditor_->setConfiguration(configuration_);
		regionsEditor_->setConfiguration(configuration_);
		detectorsView_->setConfiguration(configuration_);
	}

	// Update the view.

	update();
}

void BioXASXASScanConfigurationView::updateName()
{
	if (configuration_) {
		scanName_->setEnabled(true);
		scanName_->setText(configuration_->userScanName());

	} else {
		scanName_->setText(QString());
		scanName_->setEnabled(false);
	}
}

void BioXASXASScanConfigurationView::updateConfigurationName()
{
	if (configuration_) {
		configuration_->setUserScanName(scanName_->text());
	}
}
