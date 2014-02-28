#include "IDEASXASScanConfigurationView.h"

#include <QBoxLayout>

#include "acquaman/AMScanController.h"
#include "acquaman/IDEAS/IDEASXASScanConfiguration.h"
#include "beamline/IDEAS/IDEASBeamline.h"
#include "ui/acquaman/AMRegionsView.h"
#include "ui/AMTopFrame.h"

IDEASXASScanConfigurationView::IDEASXASScanConfigurationView(IDEASXASScanConfiguration *configuration, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	configuration_ = configuration;

        topFrame_ = new AMTopFrame("Configure an XAS Scan");
	topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

        regionsView_ = new AMRegionsView(configuration_->regions(), this);

        autoRegionButton_ = new QPushButton("Auto Set Regions");
        connect(autoRegionButton_, SIGNAL(clicked()), this, SLOT(onAutoRegionButtonClicked()));

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(topFrame_);
	mainVL->addWidget(regionsView_);
        mainVL->addWidget(autoRegionButton_);

	mainVL->setContentsMargins(0,0,0,0);
	mainVL->setSpacing(1);
	setLayout(mainVL);
}

IDEASXASScanConfigurationView::~IDEASXASScanConfigurationView()
{

}

const AMScanConfiguration* IDEASXASScanConfigurationView::configuration() const
{
	return configuration_;
}


void IDEASXASScanConfigurationView::onAutoRegionButtonClicked()
{
    bool ok;
    double edgeEnergy = QInputDialog::getDouble(this,"Auto Region Setup","Enter desired edge enegry:",IDEASBeamline::ideas()->monoEnergyControl()->value(),IDEASBeamline::ideas()->monoLowEV()->value(),IDEASBeamline::ideas()->monoHighEV()->value(),1,&ok);

    if(ok)
    {
        while (configuration_->regionCount() != 0)
                configuration_->deleteRegion(0);

        configuration_->addRegion(0, edgeEnergy - 200, 10, edgeEnergy -30, 1);
        configuration_->addRegion(1, edgeEnergy - 30, 0.75, edgeEnergy + 40, 1);
        configuration_->addRegion(2, edgeEnergy + 40 , 5, edgeEnergy + 300, 1);

    }



}
