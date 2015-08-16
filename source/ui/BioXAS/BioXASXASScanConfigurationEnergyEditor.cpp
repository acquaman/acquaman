#include "BioXASXASScanConfigurationEnergyEditor.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMPeriodicTable.h"

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

			connect( configuration_->dbObject(), SIGNAL(edgeChanged(QString)), this, SLOT(updateElement()) );
			connect( configuration_->dbObject(), SIGNAL(edgeChanged(QString)), this, SLOT(updateLineChoiceComboBox()) );
			connect( configuration_->dbObject(), SIGNAL(edgeChanged(QString)), this, SLOT(updateEnergySpinBox()) );

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
	if (configuration_) {

		if (configuration_->edge().isEmpty()) {

			setElement(AMPeriodicTable::table()->elementBySymbol("Cu"));

		} else {

			updateEnergySpinBox();
			updateElementChoiceButton();
			updateLineChoiceComboBox();
		}
	}
}

void BioXASXASScanConfigurationEnergyEditor::refresh()
{
	// Clear the view.

	clear();

	// Update the view.

	update();
}

void BioXASXASScanConfigurationEnergyEditor::setElement(AMElement *newElement)
{
	if (element_ != newElement) {
		element_ = newElement;

		updateElementChoiceButton();
		updateLineChoiceComboBox();

		lineChoice_->setCurrentIndex(0);
	}
}

void BioXASXASScanConfigurationEnergyEditor::addLineChoiceEdge(const AMAbsorptionEdge &edge)
{
	if (!edge.isNull()) {
		lineChoice_->addItem(edge.name()+": "+edge.energyString()+" eV", edge.energy());
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

void BioXASXASScanConfigurationEnergyEditor::updateEnergySpinBox(double newEnergy)
{
	energy_->setEnabled(true);
	energy_->setValue(newEnergy);
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

		lineChoice_->setCurrentIndex(lineChoice_->findText(configuration_->edge(), Qt::MatchStartsWith | Qt::MatchCaseSensitive));
	}
}

void BioXASXASScanConfigurationEnergyEditor::updateConfigurationEnergy()
{
	if (configuration_) {
		configuration_->setEnergy(energy_->value());
	}
}

void BioXASXASScanConfigurationEnergyEditor::updateConfigurationEdge()
{
	if (configuration_) {
		configuration_->setEdge(currentLineChoice());
	}
}

void BioXASXASScanConfigurationEnergyEditor::updateElement()
{
	if (configuration_) {
		setElement(AMPeriodicTable::table()->elementBySymbol(configuration_->edge().split(" ").first()));
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
	// Update the energy spinbox to reflect the energy associated with a particular line.
	// The configuration energy should update automatically, with the change in spinbox value.
	// The configuration edge needs to be updated to reflect the current line selection.

	if (newIndex > -1 && lineChoice_->count() > 0) {

		updateEnergySpinBox(lineChoice_->itemData(newIndex).toDouble());
		updateConfigurationEdge();
	}
}

QString BioXASXASScanConfigurationEnergyEditor::currentLineChoice() const
{
	QString result;

	int currentIndex = lineChoice_->currentIndex();

	if (currentIndex > -1) {
		result = lineChoice_->itemText(currentIndex).split(":").first();
	}

	return result;
}
