#include "BioXASXASScanConfigurationView.h"

#include "util/AMPeriodicTable.h"

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
		configuration->setName(newName);
}

void BioXASXASScanConfigurationView::setConfigurationEdge(BioXASXASScanConfiguration *configuration, const AMAbsorptionEdge &newEdge)
{
	if (configuration && !newEdge.isNull()) {

		QStringList edgeInfo = newEdge.toString().split(":");
		if (!edgeInfo.isEmpty()) {

			QString edgeString = edgeInfo.first();
			setConfigurationEdge(configuration, edgeString);
		}
	}
}

void BioXASXASScanConfigurationView::setConfigurationEdge(BioXASXASScanConfiguration *configuration, const QString &newEdgeString)
{
	if (configuration)
		configuration->setEdge(newEdgeString);
}

void BioXASXASScanConfigurationView::setConfigurationEnergy(BioXASXASScanConfiguration *configuration, double newEnergy)
{
	if (configuration)
		configuration->setEnergy(newEnergy);
}

AMElement* BioXASXASScanConfigurationView::configurationElement(BioXASXASScanConfiguration *configuration) const
{
	AMElement *result = 0;

	if (configuration) {
		QString elementSymbol;

		QString configurationEdgeText = configuration->edge();
		QStringList configurationEdgeTextParts = configurationEdgeText.split(" ");
		if (configurationEdgeTextParts.count() > 1)
			elementSymbol = configurationEdgeTextParts.first();

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
