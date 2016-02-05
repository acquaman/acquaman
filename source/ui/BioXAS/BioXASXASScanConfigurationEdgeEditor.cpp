#include "BioXASXASScanConfigurationEdgeEditor.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMPeriodicTable.h"

BioXASXASScanConfigurationEdgeEditor::BioXASXASScanConfigurationEdgeEditor(BioXASXASScanConfiguration *configuration, QWidget *parent) :
	BioXASXASScanConfigurationView(parent)
{
	// Create UI elements.

	elementButton_ = new QToolButton();

	edgeComboBox_ = new QComboBox();
	edgeComboBox_->setMinimumWidth(150);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(elementButton_);
	layout->addWidget(edgeComboBox_);

	setLayout(layout);

	// Make connections.

	connect( elementButton_, SIGNAL(clicked()), this, SLOT(onElementButtonClicked()) );
	connect( edgeComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateConfiguration()) );

	// Current settings.

	setConfiguration(configuration);
}

BioXASXASScanConfigurationEdgeEditor::~BioXASXASScanConfigurationEdgeEditor()
{

}

void BioXASXASScanConfigurationEdgeEditor::setConfiguration(BioXASXASScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_)
			disconnect( configuration_->dbObject(), 0, this, 0 );

		configuration_ = newConfiguration;

		if (configuration_)
			connect( configuration_->dbObject(), SIGNAL(edgeChanged(QString)), this, SLOT(update()) );

		refresh();

		emit configurationChanged(configuration_);
	}
}

void BioXASXASScanConfigurationEdgeEditor::clear()
{
	elementButton_->setText("");
	edgeComboBox_->clear();
}

void BioXASXASScanConfigurationEdgeEditor::update()
{
	updateElementButton();
	updateEdgeComboBox();
}

void BioXASXASScanConfigurationEdgeEditor::refresh()
{
	// Clear the view.

	clear();

	// Update the view.

	update();
}

void BioXASXASScanConfigurationEdgeEditor::updateElementButton()
{
	if (configuration_)
		setElement(configurationElement(configuration_));
}

void BioXASXASScanConfigurationEdgeEditor::setElement(AMElement *element)
{
	if (element)
		elementButton_->setText(element->symbol());
}

void BioXASXASScanConfigurationEdgeEditor::updateEdgeComboBox()
{
	// Clear the combo box of previous edges and update the displayed edges to correspond
	// to the configuration element.

	edgeComboBox_->blockSignals(true);

	edgeComboBox_->clear();

	AMElement *newElement = configurationElement(configuration_);
	addEdges(newElement);

	edgeComboBox_->blockSignals(false);

	// Set the current edge to be the configuration edge, or the first edge.

	if (configuration_ && !configuration_->edge().isEmpty())
		edgeComboBox_->setCurrentIndex(edgeComboBox_->findText(configuration_->edge(), Qt::MatchStartsWith | Qt::MatchCaseSensitive));
	else
		edgeComboBox_->setCurrentIndex(0);
}

void BioXASXASScanConfigurationEdgeEditor::addEdges(AMElement *element)
{
	// Iterate through the element's absorption edges and add each edge to the list
	// of edge choices.

	if (element) {

		for (int edgeIndex = 0, edgeCount = element->absorptionEdges().size(); edgeIndex < edgeCount; edgeIndex++) {

			AMAbsorptionEdge edge = element->absorptionEdges().at(edgeIndex);
			addEdge(edge);
		}
	}
}

void BioXASXASScanConfigurationEdgeEditor::addEdge(const AMAbsorptionEdge &edge)
{
	if (configuration_ && !edge.isNull())
		edgeComboBox_->addItem(edge.toString(), edge.energy());
}

void BioXASXASScanConfigurationEdgeEditor::updateConfiguration()
{
	updateConfigurationEdge();
	updateConfigurationEnergy();
}

void BioXASXASScanConfigurationEdgeEditor::updateConfigurationEdge()
{
	setConfigurationEdge(configuration_, currentEdge());
}

void BioXASXASScanConfigurationEdgeEditor::updateConfigurationEnergy()
{
	setConfigurationEnergy(configuration_, currentEdgeEnergy());
}

void BioXASXASScanConfigurationEdgeEditor::onElementButtonClicked()
{
	// Show the element selection dialog.

	AMElement *newElement = AMPeriodicTableDialog::getElement(this);

	// Update the configuration with the first edge of the new element.

	if (newElement) {
		QList<AMAbsorptionEdge> edges = newElement->absorptionEdges();

		if (!edges.isEmpty()) {
			AMAbsorptionEdge edge = edges.first();
			setConfigurationEdge(configuration_, edge);
			setConfigurationEnergy(configuration_, edge.energy());
		}
	}
}

QString BioXASXASScanConfigurationEdgeEditor::currentEdge() const
{
	QString result = edgeAt(edgeComboBox_->currentIndex());
	return result;
}

QString BioXASXASScanConfigurationEdgeEditor::edgeAt(int index) const
{
	QString result;

	if (index > -1 && index < edgeComboBox_->count()) {
		QStringList edgeText = edgeComboBox_->itemText(index).split(":");
		if (edgeText.count() > 1)
			result = edgeText.first();
	}

	return result;
}

double BioXASXASScanConfigurationEdgeEditor::currentEdgeEnergy()
{
	double result = edgeEnergyAt(edgeComboBox_->currentIndex());
	return result;
}

double BioXASXASScanConfigurationEdgeEditor::edgeEnergyAt(int index)
{
	double result = -1;

	if (index > -1 && index < edgeComboBox_->count())
		result = edgeComboBox_->itemData(index).toDouble();

	return result;
}
