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
	// Create UI elements.

	QLabel *namePrompt = new QLabel("Name: ");

	nameLineEdit_ = new QLineEdit();

	QLabel *energyPrompt = new QLabel("Energy: ");

	energySpinBox_ = new QDoubleSpinBox();
	energySpinBox_->setSuffix(" eV");
	energySpinBox_->setMinimum(0);
	energySpinBox_->setMaximum(30000);

	edgeEditor_ = new BioXASXASScanConfigurationEdgeEditor(0);

	regionsEditor_ = new BioXASXASScanConfigurationRegionsEditor(0);

	detectorsView_ = new AMGenericStepScanConfigurationDetectorsView(0, AMBeamline::bl()->exposedDetectors());

	// Create and set main layouts

	QHBoxLayout *edgeEditorLayout = new QHBoxLayout();
	edgeEditorLayout->addStretch();
	edgeEditorLayout->addWidget(edgeEditor_);

	QGridLayout *propertiesLayout = new QGridLayout();
	propertiesLayout->addWidget(namePrompt, 0, 0, 1, 1, Qt::AlignRight);
	propertiesLayout->addWidget(nameLineEdit_, 0, 1);
	propertiesLayout->addWidget(energyPrompt, 1, 0, 1, 1, Qt::AlignRight);
	propertiesLayout->addWidget(energySpinBox_, 1, 1);
	propertiesLayout->addLayout(edgeEditorLayout, 1, 2);

	QVBoxLayout *scanBoxLayout = new QVBoxLayout();
	scanBoxLayout->addLayout(propertiesLayout);
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

	connect( nameLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateConfigurationName()) );
	connect( energySpinBox_, SIGNAL(valueChanged(double)), this, SLOT(updateConfigurationEnergy()) );

	// Current settings.

	setConfiguration(configuration);
}

BioXASXASScanConfigurationEditor::~BioXASXASScanConfigurationEditor()
{

}

void BioXASXASScanConfigurationEditor::setConfiguration(BioXASXASScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_) {
			disconnect( configuration_, 0, this, 0 );
			disconnect( configuration_->dbObject(), 0, this, 0 );
		}

		configuration_ = newConfiguration;

		if (configuration_) {
			connect( configuration_, SIGNAL(nameChanged(QString)), this, SLOT(updateNameLineEdit()) );
			connect( configuration_->dbObject(), SIGNAL(energyChanged(double)), this, SLOT(updateEnergySpinBox()) );
		}

		refresh();

		emit configurationChanged(configuration_);
	}
}

void BioXASXASScanConfigurationEditor::clear()
{
	nameLineEdit_->clear();
	energySpinBox_->clear();
	edgeEditor_->clear();
	regionsEditor_->clear();
	detectorsView_->clear();
}

void BioXASXASScanConfigurationEditor::update()
{
	updateNameLineEdit();
	updateEnergySpinBox();
	edgeEditor_->update();
	regionsEditor_->update();
	detectorsView_->update();
}

void BioXASXASScanConfigurationEditor::refresh()
{
	// Clear view.

	clear();

	// Initialize member widgets.

	edgeEditor_->setConfiguration(configuration_);
	regionsEditor_->setConfiguration(configuration_);
	detectorsView_->setConfiguration(configuration_);

	// Update the view.

	update();
}

void BioXASXASScanConfigurationEditor::updateNameLineEdit()
{
	QString text;
	bool enabled = false;

	if (configuration_) {
		text = configuration_->userScanName();
		enabled = true;
	}

	nameLineEdit_->setText(text);
	nameLineEdit_->setEnabled(enabled);
}

void BioXASXASScanConfigurationEditor::updateEnergySpinBox()
{
	double value = 0;
	bool enabled = false;

	if (configuration_) {
		value = configuration_->energy();
		enabled = true;
	}

	energySpinBox_->setValue(value);
	energySpinBox_->setEnabled(enabled);
}

void BioXASXASScanConfigurationEditor::updateConfigurationName()
{
	setConfigurationName(configuration_, nameLineEdit_->text());
}

void BioXASXASScanConfigurationEditor::updateConfigurationEnergy()
{
	setConfigurationEnergy(configuration_, energySpinBox_->value());
}
