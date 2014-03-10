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

	autoRegionButton_ = new QPushButton("Auto Set XANES Regions");
        connect(autoRegionButton_, SIGNAL(clicked()), this, SLOT(onAutoRegionButtonClicked()));

	pseudoXAFSButton_ = new QPushButton("Auto Set XAFS Regions");
	connect(pseudoXAFSButton_, SIGNAL(clicked()), this, SLOT(onXAFSRegionButtonClicked()));


	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(topFrame_);
	mainVL->addWidget(regionsView_);

	QHBoxLayout *regionsHL = new QHBoxLayout();
	regionsHL->addStretch();
	regionsHL->addWidget(autoRegionButton_);
	regionsHL->addWidget(pseudoXAFSButton_);

	mainVL->addStretch();
	mainVL->addLayout(regionsHL);

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

void IDEASXASScanConfigurationView::onXAFSRegionButtonClicked()
{
    bool ok;
    bool ok2;
    double edgeEnergy = QInputDialog::getDouble(this,"Auto XAFS Region Setup","Enter desired edge enegry:",IDEASBeamline::ideas()->monoEnergyControl()->value(),IDEASBeamline::ideas()->monoLowEV()->value(),IDEASBeamline::ideas()->monoHighEV()->value(),1,&ok);

    double kValue = QInputDialog::getDouble(this,"Auto XAFS Region Setup","Scan out to k = " ,9 ,0 , 99,1,&ok2);

    if(ok && ok2)
    {
	while (configuration_->regionCount() != 0)
    	    configuration_->deleteRegion(0);

	configuration_->addRegion(0, edgeEnergy - 200, 10, edgeEnergy -30, 1);
	configuration_->addRegion(1, edgeEnergy - 30, 0.75, edgeEnergy + 34, 1);

	//double maxE = 3.80998 * kValue * kValue + edgeEnergy;

	//qDebug() << "maxE = " << maxE;

//	double E = edgeEnergy + 34;
	//int region = 2;
	//double regionSize = 50;

	for (int k = 3; k < kValue; k++)
	{
	    configuration_->addRegion(k-1, floor(3.80998 * k * k + edgeEnergy), floor(3.80998 * k * k * 0.05), floor(3.80998 * (k + 1) * (k + 1) + edgeEnergy), qMin(int(0.13 * k * k),10));
	}

//	while (E + regionSize < maxE)
//	{
//	    configuration_->addRegion(region++, E, 2, E+regionSize, 1);
//	    E+=regionSize;
//	}


//	configuration_->addRegion(region, E, 2, floor(maxE), 1);


    }


}
