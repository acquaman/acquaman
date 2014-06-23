#include "IDEASXASScanConfigurationView.h"

#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QInputDialog>
#include <QStringBuilder>

#include "acquaman/AMScanController.h"
#include "beamline/IDEAS/IDEASBeamline.h"
#include "ui/AMTopFrame.h"
#include "ui/dataman/AMEXAFSScanAxisView.h"
#include "util/AMEnergyToKSpaceCalculator.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMPeriodicTable.h"


IDEASXASScanConfigurationView::IDEASXASScanConfigurationView(IDEASXASScanConfiguration *configuration, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	configuration_ = configuration;

        topFrame_ = new AMTopFrame("Configure an XAS Scan");
	topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

	regionsView_ = new AMEXAFSScanAxisView("IDEAS Region Configuration", configuration_);

	autoRegionButton_ = new QPushButton("Auto Set XANES Regions");
	connect(autoRegionButton_, SIGNAL(clicked()), this, SLOT(setupDefaultXANESScanRegions()));

	pseudoXAFSButton_ = new QPushButton("Auto Set EXAFS Regions");
	connect(pseudoXAFSButton_, SIGNAL(clicked()), this, SLOT(setupDefaultEXAFSScanRegions()));

	//((AMScanAxisEXAFSRegion *)configuration_->scanAxisAt(0)->regionAt(0))->setEdgeEnergy(7112);
	//connect(edgeEnergy_, SIGNAL(valueChanged(double)), regionsView_, SLOT())

	scanName_ = new QLineEdit();
	scanName_->setText(configuration_->userScanName());

	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));

	isXRFScanCheckBox_ = new QCheckBox("XRF");
	isXRFScanCheckBox_->setChecked(configuration->isXRFScan());
	connect(isXRFScanCheckBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setIsXRFScan(bool)));

	isTransScanCheckBox_ = new QCheckBox("transmission");
	isTransScanCheckBox_->setChecked(configuration->isTransScan());
	connect(isTransScanCheckBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setIsTransScan(bool)));

	useRefCheckBox_ = new QCheckBox("reference sample");
	useRefCheckBox_->setChecked(configuration->useRef());
	useRefCheckBox_->setEnabled(configuration->isTransScan());
	connect(useRefCheckBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setUseRef(bool)));
	connect(isTransScanCheckBox_, SIGNAL(clicked(bool)),useRefCheckBox_,SLOT(setEnabled(bool)));

	// Energy (Eo) selection
	energy_ = new QDoubleSpinBox;
	energy_->setSuffix(" eV");
	energy_->setMinimum(0);
	energy_->setMaximum(30000);
	connect(energy_, SIGNAL(editingFinished()), this, SLOT(setEnergy()));

	elementChoice_ = new QToolButton;
	connect(elementChoice_, SIGNAL(clicked()), this, SLOT(onElementChoiceClicked()));

	lineChoice_ = new QComboBox;
	connect(lineChoice_, SIGNAL(currentIndexChanged(int)), this, SLOT(onLinesComboBoxIndexChanged(int)));

	if (configuration_->edge().isEmpty()){

		elementChoice_->setText("Cu");
		fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol("Cu"));
		lineChoice_->setCurrentIndex(0);
	}
	// Resets the view for the view to what it should be.  Using the saved for the energy in case it is different from the original line energy.
	else {
		//onEdgeChanged(); //Breaks custom energy set points
		elementChoice_->setText(configuration_->edge().split(" ").first());
		lineChoice_->blockSignals(true);
		fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol(elementChoice_->text()));
		lineChoice_->setCurrentIndex(lineChoice_->findText(configuration_->edge(), Qt::MatchStartsWith | Qt::MatchCaseSensitive));
		lineChoice_->blockSignals(false);
		energy_->setValue(configuration_->energy());
	}

	estimatedTime_ = new QLabel;
	estimatedSetTime_ = new QLabel;
	pointPerScan_ = new QLabel;
	scanEnergyRange_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	connect(configuration_, SIGNAL(scanAxisAdded(AMScanAxis*)), this, SLOT(onEstimatedTimeChanged()));
	connect(configuration_, SIGNAL(scanAxisRemoved(AMScanAxis*)), this, SLOT(onEstimatedTimeChanged()));
	connect(configuration_, SIGNAL(modifiedChanged(bool)), this, SLOT(onEstimatedTimeChanged()));
	connect(configuration_, SIGNAL(configurationChanged()), this, SLOT(onEstimatedTimeChanged()));

	connect(configuration_, SIGNAL(edgeChanged(QString)), this, SLOT(onEdgeChanged()));

	QSpinBox *numberOfScans = new QSpinBox;
	numberOfScans->setMinimum(1);
	numberOfScans->setValue(configuration_->numberOfScans());
	numberOfScans->setAlignment(Qt::AlignCenter);
	connect(numberOfScans, SIGNAL(valueChanged(int)), configuration_, SLOT(setNumberOfScans(int)));
	connect(configuration_, SIGNAL(numberOfScansChanged(int)), this, SLOT(onEstimatedTimeChanged()));

	QFormLayout *numberOfScansLayout = new QFormLayout;
	numberOfScansLayout->addRow("Number of Scans:", numberOfScans);
	numberOfScansLayout->addRow("", estimatedTime_);
	numberOfScansLayout->addRow("", estimatedSetTime_);
	numberOfScansLayout->addRow("", pointPerScan_);
	numberOfScansLayout->addRow("", scanEnergyRange_);


	QFormLayout *energySetpointLayout = new QFormLayout;
	energySetpointLayout->addRow("Energy:", energy_);

	QHBoxLayout *energyLayout = new QHBoxLayout;
	energyLayout->addLayout(energySetpointLayout);
	energyLayout->addWidget(elementChoice_);
	energyLayout->addWidget(lineChoice_);



//	I0ChannelComboBox_ = new QComboBox();
//	I0ChannelComboBox_->addItem("I_0");
//	I0ChannelComboBox_->addItem("I_vac_6485");
//	I0ChannelComboBox_->addItem("I_sample");
//	I0ChannelComboBox_->addItem("I_ref");
//	I0ChannelComboBox_->setCurrentIndex(0);
//	if(I0ChannelComboBox_->findText(configuration->I0Channel())) I0ChannelComboBox_->setCurrentIndex(I0ChannelComboBox_->findText(configuration->I0Channel()));
//	connect(I0ChannelComboBox_, SIGNAL(currentIndexChanged(QString)), configuration_, SLOT(setI0Channel(QString)));

//	ItChannelComboBox_ = new QComboBox();
//	ItChannelComboBox_->addItem("I_0");
//	ItChannelComboBox_->addItem("I_vac_6485");
//	ItChannelComboBox_->addItem("I_sample");
//	ItChannelComboBox_->addItem("I_ref");
//	ItChannelComboBox_->setCurrentIndex(2);
//	if(ItChannelComboBox_->findText(configuration->ItChannel())) ItChannelComboBox_->setCurrentIndex(ItChannelComboBox_->findText(configuration->ItChannel()));
//	connect(ItChannelComboBox_, SIGNAL(currentIndexChanged(QString)), configuration_, SLOT(setItChannel(QString)));
//	connect(isTransScanCheckBox_, SIGNAL(clicked(bool)), ItChannelComboBox_, SLOT(setEnabled(bool)));


//	IrChannelComboBox_ = new QComboBox();
//	IrChannelComboBox_->addItem("I_0");
//	IrChannelComboBox_->addItem("I_vac_6485");
//	IrChannelComboBox_->addItem("I_sample");
//	IrChannelComboBox_->addItem("I_ref");
//	IrChannelComboBox_->addItem("None");
//	IrChannelComboBox_->setCurrentIndex(3);
//	if(IrChannelComboBox_->findText(configuration->IrChannel())) IrChannelComboBox_->setCurrentIndex(IrChannelComboBox_->findText(configuration->IrChannel()));
//	connect(IrChannelComboBox_, SIGNAL(currentIndexChanged(QString)), configuration_, SLOT(setIrChannel(QString)));
//	connect(isTransScanCheckBox_, SIGNAL(clicked(bool)), IrChannelComboBox_, SLOT(setEnabled(bool)));




	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(topFrame_);
	mainVL->addLayout(energyLayout);
	mainVL->addWidget(regionsView_);
	mainVL->addLayout(numberOfScansLayout);


	QLabel *settingsLabel = new QLabel("Scan Settings:");
	settingsLabel->setFont(QFont("Lucida Grande", 12, QFont::Bold));

	QFormLayout *configFL = new QFormLayout();
	configFL->setAlignment(Qt::AlignLeft);
	configFL->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	configFL->addRow("Scan Name: ", scanName_);
//	configFL->addRow("I_0 Chamber: ", I0ChannelComboBox_);
	configFL->addRow("Include: ",isXRFScanCheckBox_);
	configFL->addRow("", isTransScanCheckBox_);
	configFL->addRow("", useRefCheckBox_);
	//	configFL->addRow("Sample Chamber: ", ItChannelComboBox_);
//	configFL->addRow("Reference Chamber: ", IrChannelComboBox_);

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


void IDEASXASScanConfigurationView::setupDefaultXANESScanRegions()
{
//    bool ok;
//    double edgeEnergy = QInputDialog::getDouble(this,"Auto Region Setup","Enter desired edge enegry:",IDEASBeamline::ideas()->monoEnergyControl()->value(),IDEASBeamline::ideas()->monoLowEV()->value(),IDEASBeamline::ideas()->monoHighEV()->value(),1,&ok);

//    if(ok)
//    {
//        while (configuration_->regionCount() != 0)
//                configuration_->deleteRegion(0);

//        configuration_->addRegion(0, edgeEnergy - 200, 10, edgeEnergy -30, 1);
//	configuration_->addRegion(1, edgeEnergy - 30, 0.75, edgeEnergy + 45, 1);
//	configuration_->addRegion(2, edgeEnergy + 45 , 5, edgeEnergy + 300, 1);

//    }
    while (configuration_->scanAxisAt(0)->regionCount())
    {
	    regionsView_->removeEXAFSRegion(0);
    }

    AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
    region->setEdgeEnergy(configuration_->energy());
    region->setRegionStart(configuration_->energy() - 30);
    region->setRegionStep(0.5);
    region->setRegionEnd(configuration_->energy() + 40);
    region->setRegionTime(1.0);
    regionsView_->insertEXAFSRegion(0, region);
}

void IDEASXASScanConfigurationView::setupDefaultEXAFSScanRegions()
{

    while (configuration_->scanAxisAt(0)->regionCount())
    {
	    regionsView_->removeEXAFSRegion(0);
    }

    AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
    region->setEdgeEnergy(configuration_->energy());
    region->setRegionStart(configuration_->energy() - 200);
    region->setRegionStep(10);
    region->setRegionEnd(configuration_->energy() - 30);
    region->setRegionTime(1.0);
    regionsView_->insertEXAFSRegion(0, region);

    region = new AMScanAxisEXAFSRegion;
    region->setEdgeEnergy(configuration_->energy());
    region->setRegionStart(configuration_->energy() - 30);
    region->setRegionStep(0.5);
    region->setRegionEnd(configuration_->energy() + 40);
    region->setRegionTime(1.0);
    regionsView_->insertEXAFSRegion(1, region);

    region = new AMScanAxisEXAFSRegion;
    region->setEdgeEnergy(configuration_->energy());
    region->setInKSpace(true);
    region->setRegionStart(AMEnergyToKSpaceCalculator::k(region->edgeEnergy(), configuration_->energy() + 40));
    region->setRegionStep(0.05);
    region->setRegionEnd(10);
    region->setRegionTime(1.0);
    region->setMaximumTime(10.0);
    regionsView_->insertEXAFSRegion(2, region);
}

void IDEASXASScanConfigurationView::onScanNameEdited()
{
	configuration_->setName(scanName_->text());
	configuration_->setUserScanName(scanName_->text());
}

void IDEASXASScanConfigurationView::setEnergy()
{
	configuration_->setEnergy(energy_->value());
	regionsView_->setEdgeEnergy(energy_->value());
}

void IDEASXASScanConfigurationView::onElementChoiceClicked()
{
	AMElement *el = AMPeriodicTableDialog::getElement(this);

	if (el){

		elementChoice_->setText(el->symbol());
		fillLinesComboBox(el);
		lineChoice_->setCurrentIndex(0);
	}
}

void IDEASXASScanConfigurationView::fillLinesComboBox(AMElement *el)
{
	if (!el)
		return;

	AMAbsorptionEdge edge;
	lineChoice_->clear();

	for (int i = 0; i < el->absorptionEdges().size(); i++){

		edge = el->absorptionEdges().at(i);

		if (edge.energy() <= 24000 && edge.energy() >= 1500)
			lineChoice_->addItem(edge.name()+": "+edge.energyString()+" eV", edge.energy());
	}

	lineChoice_->setCurrentIndex(-1);
}

void IDEASXASScanConfigurationView::onLinesComboBoxIndexChanged(int index)
{
	if (lineChoice_->count() == 0 || index == -1)
		return;

	energy_->setValue(lineChoice_->itemData(index).toDouble());
	setEnergy();
	configuration_->setEdge(lineChoice_->itemText(index).split(":").first());
}

void IDEASXASScanConfigurationView::onEdgeChanged()
{
	QString currentChoice = lineChoice_->itemText(lineChoice_->currentIndex()).split(":").first();
	if (configuration_->edge() == currentChoice)
		return;

	elementChoice_->setText(configuration_->edge().split(" ").first());
	lineChoice_->blockSignals(true);
	fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol(elementChoice_->text()));
	lineChoice_->blockSignals(false);
	lineChoice_->setCurrentIndex(lineChoice_->findText(configuration_->edge(), Qt::MatchStartsWith | Qt::MatchCaseSensitive));

	if (energy_->value() != configuration_->energy())
		energy_->setValue(configuration_->energy());
}

void IDEASXASScanConfigurationView::onEstimatedTimeChanged()
{
    QString timeString = "";

    configuration_->blockSignals(true);
    double time = configuration_->totalTime(true);
    configuration_->blockSignals(false);

    pointPerScan_->setText(QString("%1 points per scan").arg(configuration_->totalPoints()));
    scanEnergyRange_->setText(QString("from %1 to %2 eV").arg(configuration_->minEnergy()).arg(configuration_->maxEnergy()));

    int days = int(time/3600.0/24.0);

    if (days > 0){

	    time -= days*3600.0*24;
	    timeString += QString::number(days) + "d:";
    }

    int hours = int(time/3600.0);

    if (hours > 0){

	    time -= hours*3600;
	    timeString += QString::number(hours) + "h:";
    }

    int minutes = int(time/60.0);

    if (minutes > 0){

	    time -= minutes*60;
	    timeString += QString::number(minutes) + "m:";
    }

    int seconds = ((int)time)%60;
    timeString += QString::number(seconds) + "s";

    estimatedTime_->setText("Estimated time per scan:\t" + timeString);




    time = configuration_->totalTime()*configuration_->numberOfScans();
    timeString = "";

    days = int(time/3600.0/24.0);

    if (days > 0){

	    time -= days*3600.0*24;
	    timeString += QString::number(days) + "d:";
    }

    hours = int(time/3600.0);

    if (hours > 0){

	    time -= hours*3600;
	    timeString += QString::number(hours) + "h:";
    }

    minutes = int(time/60.0);

    if (minutes > 0){

	    time -= minutes*60;
	    timeString += QString::number(minutes) + "m:";
    }

    seconds = ((int)time)%60;
    timeString += QString::number(seconds) + "s";

    estimatedSetTime_->setText("Estimated time for set:\t" + timeString);

}
