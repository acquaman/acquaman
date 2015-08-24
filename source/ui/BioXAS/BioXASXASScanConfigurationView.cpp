#include "BioXASXASScanConfigurationView.h"

BioXASXASScanConfigurationView::BioXASXASScanConfigurationView(QWidget *parent) :
    AMScanConfigurationView(parent)
{
	// Initialize class variables.

	configuration_ = 0;
}

BioXASXASScanConfigurationView::~BioXASXASScanConfigurationView()
{

}

const AMScanConfiguration* BioXASXASScanConfigurationView::configuration() const
{
	return configuration_;
}

void BioXASXASScanConfigurationView::setConfigurationName(BioXASXASScanConfiguration *configuration, const QString &newName)
{
	if (configuration)
		configuration->setUserScanName(newName);
}

void BioXASXASScanConfigurationView::setConfigurationEdge(BioXASXASScanConfiguration *configuration, const QString &edgeString)
{
	if (configuration && configuration->edge() != edgeString) {
		configuration->setEdge(edgeString);
	}
}

void BioXASXASScanConfigurationView::setConfigurationEnergy(BioXASXASScanConfiguration *configuration, double newEnergy)
{
	if (configuration && configuration->energy() != newEnergy) {
		configuration->setEnergy(newEnergy);
	}
}

void BioXASXASScanConfigurationView::initializeConfiguration(BioXASXASScanConfiguration *configuration)
{
	if (configuration && configuration->edge().isEmpty()) {

		AMElement *initialElement = AMPeriodicTable::table()->elementBySymbol("Cu");
		QList<AMAbsorptionEdge> edges = initialElement->absorptionEdges();

		if (!edges.isEmpty()) {
			AMAbsorptionEdge initialEdge = edges.first();
			double initialEnergy = initialEdge.energy();

			setConfigurationEdge(configuration, edgeToString(initialEdge));
			setConfigurationEnergy(configuration, initialEnergy);
		}
	}
}

AMElement* BioXASXASScanConfigurationView::configurationElement(BioXASXASScanConfiguration *configuration) const
{
	AMElement *result = 0;

	if (configuration) {
		QString elementSymbol;

		QStringList configurationEdgeText = configuration->edge().split(" ");
		if (configurationEdgeText.count() > 1)
			elementSymbol = configurationEdgeText.first();

		if (!elementSymbol.isEmpty())
			result = AMPeriodicTable::table()->elementBySymbol(elementSymbol);
	}

	return result;
}

QString BioXASXASScanConfigurationView::configurationEdge(BioXASXASScanConfiguration *configuration) const
{
	QString result;

	if (configuration)
		result = configuration->edge();

	return result;
}

QString BioXASXASScanConfigurationView::edgeToString(const AMAbsorptionEdge &edge) const
{
	QString result;

	if (!edge.isNull())
		result = edge.name() + ": " + edge.energyString() + " eV";

	return result;
}
