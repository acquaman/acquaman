#include "BioXASXASScanConfigurationEnergyEditor.h"

BioXASXASScanConfigurationEnergyEditor::BioXASXASScanConfigurationEnergyEditor(BioXASXASScanConfiguration *configuration, QWidget *parent) :
	BioXASXASScanConfigurationView(parent)
{
	// Create UI elements.

	QLabel *energyPrompt = new QLabel("Energy: ");

	energySpinBox_ = new QDoubleSpinBox;
	energySpinBox_->setSuffix(" eV");
	energySpinBox_->setMinimum(0);
	energySpinBox_->setMaximum(30000);

	edgeEditor_ = new BioXASXASScanConfigurationEdgeEditor(0);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(energyPrompt);
	layout->addWidget(energySpinBox_);
	layout->addStretch();
	layout->addWidget(edgeEditor_);

	setLayout(layout);

	// Make connections.

	connect( energySpinBox_, SIGNAL(editingFinished()), this, SLOT(updateConfigurationEnergy()) );

	// Current settings.

	setConfiguration(configuration);
}

BioXASXASScanConfigurationEnergyEditor::~BioXASXASScanConfigurationEnergyEditor()
{

}

void BioXASXASScanConfigurationEnergyEditor::setConfiguration(BioXASXASScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_)
			disconnect( configuration_->dbObject(), 0, this, 0 );

		configuration_ = newConfiguration;

		initializeConfiguration(configuration_);

		if (configuration_)
			connect( configuration_->dbObject(), SIGNAL(energyChanged(double)), this, SLOT(update()) );

		refresh();

		emit configurationChanged(configuration_);
	}
}

void BioXASXASScanConfigurationEnergyEditor::clear()
{
	energySpinBox_->clear();
	edgeEditor_->clear();
}

void BioXASXASScanConfigurationEnergyEditor::update()
{
	edgeEditor_->update();
	updateEnergy();
}

void BioXASXASScanConfigurationEnergyEditor::refresh()
{
	// Clear the view.

	clear();

	// Initialize member widgets.

	edgeEditor_->setConfiguration(configuration_);

	// Update the view.

	update();
}

void BioXASXASScanConfigurationEnergyEditor::updateEnergy()
{
	if (configuration_) {
		double newEnergy = configuration_->energy();

		energySpinBox_->setEnabled(true);
		energySpinBox_->setValue(newEnergy);

	} else {
		energySpinBox_->setEnabled(false);
	}
}

void BioXASXASScanConfigurationEnergyEditor::updateConfigurationEnergy()
{
	setConfigurationEnergy(configuration_, energySpinBox_->value());
}
