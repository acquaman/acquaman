#include "IDEASXASScanConfigurationView.h"

#include <QBoxLayout>
#include <QFormLayout>

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

	isXRFScanCheckBox_ = new QCheckBox("XRF");
	isXRFScanCheckBox_->setChecked(configuration->isXRFScan());
	connect(isXRFScanCheckBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setIsXRFScan(bool)));

	isTransScanCheckBox_ = new QCheckBox("transmission");
	isTransScanCheckBox_->setChecked(configuration->isTransScan());
	connect(isTransScanCheckBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setIsTransScan(bool)));


	I0ChannelComboBox_ = new QComboBox();
	I0ChannelComboBox_->addItem("I_0");
	I0ChannelComboBox_->addItem("I_vac");
	I0ChannelComboBox_->addItem("I_sample");
	I0ChannelComboBox_->addItem("I_ref");
	I0ChannelComboBox_->setCurrentIndex(0);
	if(I0ChannelComboBox_->findText(configuration->I0Channel())) I0ChannelComboBox_->setCurrentIndex(I0ChannelComboBox_->findText(configuration->I0Channel()));
	connect(I0ChannelComboBox_, SIGNAL(currentIndexChanged(QString)), configuration_, SLOT(setI0Channel(QString)));

	ItChannelComboBox_ = new QComboBox();
	ItChannelComboBox_->addItem("I_0");
	ItChannelComboBox_->addItem("I_vac");
	ItChannelComboBox_->addItem("I_sample");
	ItChannelComboBox_->addItem("I_ref");
	ItChannelComboBox_->setCurrentIndex(2);
	if(ItChannelComboBox_->findText(configuration->ItChannel())) ItChannelComboBox_->setCurrentIndex(ItChannelComboBox_->findText(configuration->ItChannel()));
	connect(ItChannelComboBox_, SIGNAL(currentIndexChanged(QString)), configuration_, SLOT(setItChannel(QString)));
	connect(isTransScanCheckBox_, SIGNAL(clicked(bool)), ItChannelComboBox_, SLOT(setEnabled(bool)));


	IrChannelComboBox_ = new QComboBox();
	IrChannelComboBox_->addItem("I_0");
	IrChannelComboBox_->addItem("I_vac");
	IrChannelComboBox_->addItem("I_sample");
	IrChannelComboBox_->addItem("I_ref");
	IrChannelComboBox_->addItem("None");
	IrChannelComboBox_->setCurrentIndex(3);
	if(IrChannelComboBox_->findText(configuration->IrChannel())) IrChannelComboBox_->setCurrentIndex(IrChannelComboBox_->findText(configuration->IrChannel()));
	connect(IrChannelComboBox_, SIGNAL(currentIndexChanged(QString)), configuration_, SLOT(setIrChannel(QString)));
	connect(isTransScanCheckBox_, SIGNAL(clicked(bool)), IrChannelComboBox_, SLOT(setEnabled(bool)));




	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(topFrame_);
	mainVL->addWidget(regionsView_);

	QLabel *settingsLabel = new QLabel("Scan Settings:");
	settingsLabel->setFont(QFont("Lucida Grande", 12, QFont::Bold));

	QFormLayout *configFL = new QFormLayout();
	configFL->setAlignment(Qt::AlignLeft);
	configFL->addRow("I_0 Chamber:", I0ChannelComboBox_);
	configFL->addRow("Include:",isXRFScanCheckBox_);
	configFL->addRow("", isTransScanCheckBox_);
	configFL->addRow("Sample Chamber:", ItChannelComboBox_);
	configFL->addRow("Reference Chamber:", IrChannelComboBox_);

	QHBoxLayout *regionsHL = new QHBoxLayout();
	regionsHL->addStretch();
	regionsHL->addWidget(autoRegionButton_);
	regionsHL->addWidget(pseudoXAFSButton_);

	QVBoxLayout *settingsVL = new QVBoxLayout();
	settingsVL->addLayout(regionsHL);
	settingsVL->addWidget(settingsLabel);
	settingsVL->addLayout(configFL);

	mainVL->addStretch();
	mainVL->addLayout(settingsVL);
	mainVL->addStretch();

	mainVL->setContentsMargins(20,0,0,20);
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


	for (int k = 3; k < kValue; k++)
	{
	    configuration_->addRegion(k-1, floor(3.80998 * k * k + edgeEnergy), floor(3.80998 * k * k * 0.05), floor(3.80998 * (k + 1) * (k + 1) + edgeEnergy), qMin(int(0.13 * k * k),10));
	}



    }


}


