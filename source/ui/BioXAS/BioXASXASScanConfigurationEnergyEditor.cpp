#include "BioXASXASScanConfigurationEnergyEditor.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMPeriodicTable.h"

#include <QDebug>

BioXASXASScanConfigurationEnergyEditor::BioXASXASScanConfigurationEnergyEditor(BioXASXASScanConfiguration *configuration, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	// Initialize class variables.

	configuration_ = 0;
	element_ = 0;

	// Create UI elements.

	QLabel *energyPrompt = new QLabel("Energy: ");

	energy_ = new QDoubleSpinBox;
	energy_->setSuffix(" eV");
	energy_->setMinimum(0);
	energy_->setMaximum(30000);

	elementChoice_ = new QToolButton;

	lineChoice_ = new QComboBox;

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout;
	layout->setMargin(0);
	layout->addWidget(energyPrompt);
	layout->addWidget(energy_);
	layout->addStretch();
	layout->addWidget(elementChoice_);
	layout->addWidget(lineChoice_);

	setLayout(layout);

	// Make connections.

	connect( energy_, SIGNAL(editingFinished()), this, SLOT(updateConfigurationEnergy()) );
	connect( elementChoice_, SIGNAL(clicked()), this, SLOT(onElementChoiceClicked()) );
	connect( lineChoice_, SIGNAL(currentIndexChanged(int)), this, SLOT(onLineChoiceCurrentIndexChanged(int)) );

	// Current settings.

	setConfiguration(configuration);
}

BioXASXASScanConfigurationEnergyEditor::~BioXASXASScanConfigurationEnergyEditor()
{

}

const AMScanConfiguration* BioXASXASScanConfigurationEnergyEditor::configuration() const
{
	return configuration_;
}

void BioXASXASScanConfigurationEnergyEditor::setConfiguration(BioXASXASScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_) {

			disconnect( configuration_->dbObject(), 0, this, 0 );

		}

		configuration_ = newConfiguration;

		if (configuration_) {

			connect( configuration_->dbObject(), SIGNAL(edgeChanged(QString)), this, SLOT(update()) );
			connect( configuration_->dbObject(), SIGNAL(energyChanged(double)), this, SLOT(update()) );

		}

		refresh();

		emit configurationChanged(configuration_);
	}
}

void BioXASXASScanConfigurationEnergyEditor::clear()
{
	energy_->clear();
	elementChoice_->setText("");
	lineChoice_->clear();
}

void BioXASXASScanConfigurationEnergyEditor::update()
{
	updateElementChoiceButton();
	updateLineChoiceComboBox();
	updateEnergySpinBox();
}

void BioXASXASScanConfigurationEnergyEditor::refresh()
{
	// Clear the view.

	clear();

	// Set the initial element selected.

	if (configuration_) {

		if (configuration_->edge().isEmpty()) {
			setElement(AMPeriodicTable::table()->elementBySymbol("Cu"));

		} else {
			setElement(AMPeriodicTable::table()->elementBySymbol(configurationElement()));
		}
	}

	// Update the view.

	update();
}

void BioXASXASScanConfigurationEnergyEditor::setElement(AMElement *newElement)
{
	if (element_ != newElement) {
		element_ = newElement;
	}
}

void BioXASXASScanConfigurationEnergyEditor::addLineChoiceEdge(const AMAbsorptionEdge &edge)
{
	if (!edge.isNull()) {
		lineChoice_->addItem(edge.name() + ": " + edge.energyString() + " eV", edge.energy());
	}
}

void BioXASXASScanConfigurationEnergyEditor::updateEnergySpinBox()
{
	if (configuration_) {
		energy_->setEnabled(true);
		energy_->setValue(configuration_->energy());

	} else {
		energy_->setEnabled(false);
	}
}

void BioXASXASScanConfigurationEnergyEditor::updateElementChoiceButton()
{
	if (configuration_ && element_) {
		elementChoice_->setEnabled(true);
		elementChoice_->setText(element_->symbol());

	} else {
		elementChoice_->setText(QString());
		elementChoice_->setEnabled(false);
	}
}

void BioXASXASScanConfigurationEnergyEditor::updateLineChoiceComboBox()
{
	// Clear the combobox.

	lineChoice_->clear();

	// Iterate through the element's absorption edges and add each edge to the list
	// of line choices.

	if (element_) {

		lineChoice_->blockSignals(true);

		for (int edgeIndex = 0, edgeCount = element_->absorptionEdges().size(); edgeIndex < edgeCount; edgeIndex++) {

			AMAbsorptionEdge edge = element_->absorptionEdges().at(edgeIndex);

			if (edge.energy() <= 24000 && edge.energy() >= 1500)
				addLineChoiceEdge(edge);
		}

		lineChoice_->blockSignals(false);

		// Set the current line choice item to correspond to the configuration edge.

		if (configuration_) {
			lineChoice_->setEnabled(true);

			if (configuration_->edge().isEmpty())
				lineChoice_->setCurrentIndex(0);
			else
				lineChoice_->setCurrentIndex(lineChoice_->findText(configuration_->edge(), Qt::MatchStartsWith | Qt::MatchCaseSensitive));

		} else {
			lineChoice_->setEnabled(false);
		}
	}
}

void BioXASXASScanConfigurationEnergyEditor::updateConfigurationEnergy()
{	
	setConfigurationEnergy(energy_->value());
}

void BioXASXASScanConfigurationEnergyEditor::updateConfigurationEdge()
{
	setConfigurationEdge(currentLineChoice());
}

void BioXASXASScanConfigurationEnergyEditor::updateElement()
{
	if (configuration_) {
		setElement(AMPeriodicTable::table()->elementBySymbol( configurationElement() ));
	}
}

void BioXASXASScanConfigurationEnergyEditor::setConfigurationEnergy(double newEnergy)
{
	if (configuration_) {
		configuration_->setEnergy(newEnergy);
	}
}

void BioXASXASScanConfigurationEnergyEditor::setConfigurationEdge(const QString &edge)
{
	if (configuration_) {
		configuration_->setEdge(edge);
	}
}

void BioXASXASScanConfigurationEnergyEditor::onElementChoiceClicked()
{
	// Open up an element selection dialog, where the user can pick the desired
	// element.

	AMElement *elementSelection = AMPeriodicTableDialog::getElement(this);
	setElement(elementSelection);
}

void BioXASXASScanConfigurationEnergyEditor::onLineChoiceCurrentIndexChanged(int newIndex)
{
	// Update configuration energy to reflect the energy associated with a particular line.
	// The configuration edge needs to be updated to reflect the current line selection.

	if (newIndex > -1 && lineChoice_->count() > 0) {

		setConfigurationEnergy(lineChoiceEnergyAt(newIndex));
		updateConfigurationEdge();
	}
}

QString BioXASXASScanConfigurationEnergyEditor::lineChoiceAt(int index) const
{
	QString result;

	if (index > -1 && index < lineChoice_->count()) {
		QStringList lineChoiceText = lineChoice_->itemText(index).split(":");
		if (lineChoiceText.count() > 1)
			result = lineChoiceText.first();
	}

	return result;
}

QString BioXASXASScanConfigurationEnergyEditor::currentLineChoice() const
{
	QString result = lineChoiceAt(lineChoice_->currentIndex());
	return result;
}

double BioXASXASScanConfigurationEnergyEditor::lineChoiceEnergyAt(int index) const
{
	double result = lineChoice_->itemData(index).toDouble();
	return result;
}

double BioXASXASScanConfigurationEnergyEditor::currentLineChoiceEnergy() const
{
	double result = lineChoiceEnergyAt(lineChoice_->currentIndex());
	return result;
}

QString BioXASXASScanConfigurationEnergyEditor::configurationElement() const
{
	QString result;

	if (configuration_) {
		QStringList configurationEdgeText = configuration_->edge().split(" ");
		if (configurationEdgeText.count() > 1)
			result = configurationEdgeText.first();
	}

	return result;
}
