#include "BioXASXASScanConfigurationEdgeEditor.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMPeriodicTable.h"

BioXASXASScanConfigurationEdgeEditor::BioXASXASScanConfigurationEdgeEditor(BioXASXASScanConfiguration *configuration, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	// Initialize class variables.

	configuration_ = 0;

	// Create UI elements.

	elementButton_ = new QToolButton;

	edgeComboBox_ = new QComboBox;

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

const AMScanConfiguration* BioXASXASScanConfigurationEdgeEditor::configuration() const
{
	return configuration_;
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

	// Set the initial element/edge selected.

	if (configuration_) {
		if (configuration_->edge().isEmpty()) {
			AMElement *element = AMPeriodicTable::table()->elementBySymbol("Cu");
			setConfigurationEdge(edgeToString(element->L1Edge()));
		}
	}

	// Update the view.

	update();
}

void BioXASXASScanConfigurationEdgeEditor::updateElementButton()
{
	if (configuration_)
		setElement(configurationElement());
}

void BioXASXASScanConfigurationEdgeEditor::setElement(AMElement *element)
{
	if (element)
		elementButton_->setText(element->symbol());
}

void BioXASXASScanConfigurationEdgeEditor::updateEdgeComboBox()
{
	// Clear the combo box of previous edges.

	edgeComboBox_->clear();

	// Update the displayed edges.

	AMElement *newElement = configurationElement();
	addEdges(newElement);

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

		edgeComboBox_->blockSignals(true);

		for (int edgeIndex = 0, edgeCount = element->absorptionEdges().size(); edgeIndex < edgeCount; edgeIndex++) {

			AMAbsorptionEdge edge = element->absorptionEdges().at(edgeIndex);
			addEdge(edge);
		}

		edgeComboBox_->blockSignals(false);
	}
}

void BioXASXASScanConfigurationEdgeEditor::addEdge(const AMAbsorptionEdge &edge)
{
	if (!edge.isNull())
		edgeComboBox_->addItem(edgeToString(edge), edge.energy());
}

void BioXASXASScanConfigurationEdgeEditor::updateConfiguration()
{
	updateConfigurationEdge();
	updateConfigurationEnergy();
}

void BioXASXASScanConfigurationEdgeEditor::updateConfigurationEdge()
{
	setConfigurationEdge(currentEdge());
}

void BioXASXASScanConfigurationEdgeEditor::setConfigurationEdge(const QString &edgeString)
{
	if (configuration_)
		configuration_->setEdge(edgeString);
}

void BioXASXASScanConfigurationEdgeEditor::updateConfigurationEnergy()
{
	setConfigurationEnergy(currentEdgeEnergy());
}

void BioXASXASScanConfigurationEdgeEditor::setConfigurationEnergy(double newEnergy)
{
	if (configuration_)
		configuration_->setEnergy(newEnergy);
}

void BioXASXASScanConfigurationEdgeEditor::onElementButtonClicked()
{
	// Show the element selection dialog.

	AMElement *newElement = AMPeriodicTableDialog::getElement(this);
	QList<AMAbsorptionEdge> edges = newElement->absorptionEdges();

	if (edges.size() > 0)
		setConfigurationEdge(edgeToString(edges.first()));
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

AMElement* BioXASXASScanConfigurationEdgeEditor::configurationElement() const
{
	AMElement *result = 0;

	if (configuration_) {
		QString elementSymbol;

		QStringList configurationEdgeText = configuration_->edge().split(" ");
		if (configurationEdgeText.count() > 1)
			elementSymbol = configurationEdgeText.first();

		if (!elementSymbol.isEmpty())
			result = AMPeriodicTable::table()->elementBySymbol(elementSymbol);
	}

	return result;
}

QString BioXASXASScanConfigurationEdgeEditor::configurationEdge() const
{
	QString result;

	if (configuration_)
		result = configuration_->edge();

	return result;
}

QString BioXASXASScanConfigurationEdgeEditor::edgeToString(const AMAbsorptionEdge &edge) const
{
	QString result;

	if (!edge.isNull())
		result = edge.name() + ": " + edge.energyString() + " eV";

	return result;
}
