#include "SXRMBOxidationMapScanConfigurationViewHolder.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMScanAction.h"

SXRMBOxidationMapScanConfigurationViewHolder::SXRMBOxidationMapScanConfigurationViewHolder(SXRMB2DOxidationMapScanConfigurationView *view, QWidget *parent)
	: AMScanConfigurationViewHolder3(view, false, parent)
{
}

SXRMBOxidationMapScanConfigurationViewHolder::~SXRMBOxidationMapScanConfigurationViewHolder()
{

}

AMAction3 * SXRMBOxidationMapScanConfigurationViewHolder::createScan()
{
	if(view_){

		SXRMB2DOxidationMapScanConfigurationView *configurationView = qobject_cast<SXRMB2DOxidationMapScanConfigurationView *>(view_);
		AMSequentialListAction3 *scanList = new AMSequentialListAction3(new AMSequentialListActionInfo3(configurationView->configuration()->name()));

		SXRMB2DMapScanConfiguration *configuration;

		foreach (double energy, configurationView->oxidationEnergies()){

			configuration = qobject_cast<SXRMB2DMapScanConfiguration *>(configurationView->configuration()->createCopy());
			configuration->setName(QString("%1_%2eV").arg(configuration->name()).arg(energy, 0, 'f', 2));
			configuration->setUserScanName(configuration->name());
			configuration->setEnergy(energy);
			scanList->addSubAction(new AMScanAction(new AMScanActionInfo(configuration->createCopy())));
		}

		return scanList;
	}

	return 0;
}
