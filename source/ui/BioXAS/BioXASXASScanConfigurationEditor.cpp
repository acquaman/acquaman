#include "BioXASXASScanConfigurationEditor.h"

#include "acquaman/AMScanController.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "ui/dataman/AMEXAFSScanAxisView.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMEnergyToKSpaceCalculator.h"
#include "util/AMPeriodicTable.h"
#include "util/AMDateTimeUtils.h"

BioXASXASScanConfigurationEditor::BioXASXASScanConfigurationEditor(BioXASXASScanConfiguration *configuration, QWidget *parent) :
	BioXASXASScanConfigurationView(parent)
{
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

	QVBoxLayout *scanBoxLayout = new QVBoxLayout();
	scanBoxLayout->addLayout(scanNameLayout);
	scanBoxLayout->addWidget(energyEditor_);
	scanBoxLayout->addWidget(regionsEditor_);

	QGroupBox *scanBox = new QGroupBox("Scan");
	scanBox->setLayout(scanBoxLayout);

	QVBoxLayout *detectorBoxLayout = new QVBoxLayout();
	detectorBoxLayout->addWidget(detectorsView_);

	QGroupBox *detectorBox = new QGroupBox("Detectors");
	detectorBox->setLayout(detectorBoxLayout);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(scanBox);
	layout->addWidget(detectorBox);

	setLayout(layout);

	// Make connections.

	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(updateConfigurationName()));

	// Current settings.

	setConfiguration(configuration);
}

BioXASXASScanConfigurationEditor::~BioXASXASScanConfigurationEditor()
{

}

void BioXASXASScanConfigurationEditor::setConfiguration(BioXASXASScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_)
			disconnect( configuration_, 0, this, 0 );

		configuration_ = newConfiguration;

		initializeConfiguration(configuration_);

		if (configuration_)
			connect(configuration_, SIGNAL(nameChanged(QString)), this, SLOT(updateName()) );

		refresh();

		emit configurationChanged(configuration_);
	}
}

void BioXASXASScanConfigurationEditor::clear()
{
	scanName_->clear();
	energyEditor_->clear();
	regionsEditor_->clear();
	detectorsView_->clear();
}

void BioXASXASScanConfigurationEditor::update()
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

void BioXASXASScanConfigurationEditor::refresh()
{
	// Clear view.

	clear();

	// Initialize member widgets.

	energyEditor_->setConfiguration(configuration_);
	regionsEditor_->setConfiguration(configuration_);
	detectorsView_->setConfiguration(configuration_);

	// Update the view.

	update();
}

void BioXASXASScanConfigurationEditor::updateName()
{
	if (configuration_) {
		scanName_->setEnabled(true);
		scanName_->setText(configuration_->userScanName());

	} else {
		scanName_->setText(QString());
		scanName_->setEnabled(false);
	}
}

void BioXASXASScanConfigurationEditor::updateConfigurationName()
{
	setConfigurationName(configuration_, scanName_->text());
}
