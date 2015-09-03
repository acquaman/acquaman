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
	if (configuration && configuration->userScanName() != newName)
		configuration->setUserScanName(newName);
}

void BioXASXASScanConfigurationView::setConfigurationEdge(BioXASXASScanConfiguration *configuration, const AMAbsorptionEdge &newEdge)
{
	if (configuration) {
		QString edgeString = configuration_->edgeToString(newEdge);
		setConfigurationEdge(configuration, edgeString);
	}
}

void BioXASXASScanConfigurationView::setConfigurationEdge(BioXASXASScanConfiguration *configuration, const QString &newEdgeString)
{
	if (configuration_ && configuration_->edge() != newEdgeString)
		configuration_->setEdge(newEdgeString);
}

void BioXASXASScanConfigurationView::setConfigurationEnergy(BioXASXASScanConfiguration *configuration, double newEnergy)
{
	if (configuration && configuration->energy() != newEnergy)
		configuration->setEnergy(newEnergy);
}

void BioXASXASScanConfigurationView::clearConfigurationRegions(BioXASXASScanConfiguration *configuration)
{
	if (configuration)
		configuration->clearRegions();
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
