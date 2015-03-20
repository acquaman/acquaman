#include "BioXASMainXASScanConfigurationView.h"

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


BioXASMainXASScanConfigurationView::BioXASMainXASScanConfigurationView(BioXASMainXASScanConfiguration *configuration, QWidget *parent) :
    AMScanConfigurationView(parent)
{
    configuration_ = configuration;

    topFrame_ = new AMTopFrame("Configure an XAS Scan");
    topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

    regionsView_ = new AMEXAFSScanAxisView("BioXAS Region Configuration", configuration_);

    autoRegionButton_ = new QPushButton("Auto Set XANES Regions");
    connect(autoRegionButton_, SIGNAL(clicked()), this, SLOT(setupDefaultXANESScanRegions()));

    pseudoXAFSButton_ = new QPushButton("Auto Set EXAFS Regions");
    connect(pseudoXAFSButton_, SIGNAL(clicked()), this, SLOT(setupDefaultEXAFSScanRegions()));

    scanName_ = new QLineEdit();
    scanName_->setText(configuration_->userScanName());

    connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
    connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));

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
        elementChoice_->setText(configuration_->edge().split(" ").first());
        lineChoice_->blockSignals(true);
        fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol(elementChoice_->text()));
        lineChoice_->setCurrentIndex(lineChoice_->findText(configuration_->edge(), Qt::MatchStartsWith | Qt::MatchCaseSensitive));
        lineChoice_->blockSignals(false);
        energy_->setValue(configuration_->edgeEnergy());
    }

    connect(configuration_, SIGNAL(edgeChanged(QString)), this, SLOT(onEdgeChanged()));

    QFormLayout *energySetpointLayout = new QFormLayout;
    energySetpointLayout->addRow("Energy:", energy_);

    QHBoxLayout *energyLayout = new QHBoxLayout;
    energyLayout->addLayout(energySetpointLayout);
    energyLayout->addWidget(elementChoice_);
    energyLayout->addWidget(lineChoice_);

    QVBoxLayout *mainVL = new QVBoxLayout();
    mainVL->addWidget(topFrame_);
    mainVL->addLayout(energyLayout);
    mainVL->addWidget(regionsView_);

//	QLabel *settingsLabel = new QLabel("Scan Settings:");
//	settingsLabel->setFont(QFont("Lucida Grande", 12, QFont::Bold));

    QHBoxLayout *regionsHL = new QHBoxLayout();
    regionsHL->addStretch();
    regionsHL->addWidget(autoRegionButton_);
    regionsHL->addWidget(pseudoXAFSButton_);

    QVBoxLayout *settingsVL = new QVBoxLayout();
    settingsVL->addLayout(regionsHL);
//	settingsVL->addWidget(settingsLabel);

    mainVL->addStretch();
    mainVL->addLayout(settingsVL);
    mainVL->addStretch();

    mainVL->setContentsMargins(20,0,0,20);
    mainVL->setSpacing(1);
    setLayout(mainVL);
}

BioXASMainXASScanConfigurationView::~BioXASMainXASScanConfigurationView()
{

}

const AMScanConfiguration* BioXASMainXASScanConfigurationView::configuration() const
{
    return configuration_;
}


void BioXASMainXASScanConfigurationView::setupDefaultXANESScanRegions()
{
    while (configuration_->scanAxisAt(0)->regionCount())
    {
        regionsView_->removeEXAFSRegion(0);
    }

    AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
    region->setEdgeEnergy(configuration_->edgeEnergy());
    region->setRegionStart(configuration_->edgeEnergy() - 30);
    region->setRegionStep(0.5);
    region->setRegionEnd(configuration_->edgeEnergy() + 40);
    region->setRegionTime(1.0);
    regionsView_->insertEXAFSRegion(0, region);
}

void BioXASMainXASScanConfigurationView::setupDefaultEXAFSScanRegions()
{

    while (configuration_->scanAxisAt(0)->regionCount())
    {
        regionsView_->removeEXAFSRegion(0);
    }

    AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
    region->setEdgeEnergy(configuration_->edgeEnergy());
    region->setRegionStart(configuration_->edgeEnergy() - 200);
    region->setRegionStep(10);
    region->setRegionEnd(configuration_->edgeEnergy() - 30);
    region->setRegionTime(1.0);
    regionsView_->insertEXAFSRegion(0, region);

    region = new AMScanAxisEXAFSRegion;
    region->setEdgeEnergy(configuration_->edgeEnergy());
    region->setRegionStart(configuration_->edgeEnergy() - 30);
    region->setRegionStep(0.5);
    region->setRegionEnd(configuration_->edgeEnergy() + 40);
    region->setRegionTime(1.0);
    regionsView_->insertEXAFSRegion(1, region);

    region = new AMScanAxisEXAFSRegion;
    region->setEdgeEnergy(configuration_->edgeEnergy());
    region->setInKSpace(true);
    region->setRegionStart(AMEnergyToKSpaceCalculator::k(region->edgeEnergy(), configuration_->edgeEnergy() + 40));
    region->setRegionStep(0.05);
    region->setRegionEnd(10);
    region->setRegionTime(1.0);
    region->setMaximumTime(10.0);
    regionsView_->insertEXAFSRegion(2, region);
}

void BioXASMainXASScanConfigurationView::onScanNameEdited()
{
    configuration_->setName(scanName_->text());
    configuration_->setUserScanName(scanName_->text());
}

void BioXASMainXASScanConfigurationView::setEnergy()
{
    configuration_->setEdgeEnergy(energy_->value());
    regionsView_->setEdgeEnergy(energy_->value());
}

void BioXASMainXASScanConfigurationView::onElementChoiceClicked()
{
    AMElement *el = AMPeriodicTableDialog::getElement(this);

    if (el){

        elementChoice_->setText(el->symbol());
        fillLinesComboBox(el);
        lineChoice_->setCurrentIndex(0);
    }
}

void BioXASMainXASScanConfigurationView::fillLinesComboBox(AMElement *el)
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

void BioXASMainXASScanConfigurationView::onLinesComboBoxIndexChanged(int index)
{
    if (lineChoice_->count() == 0 || index == -1)
        return;

    energy_->setValue(lineChoice_->itemData(index).toDouble());
    setEnergy();
    configuration_->setEdge(lineChoice_->itemText(index).split(":").first());
}

void BioXASMainXASScanConfigurationView::onEdgeChanged()
{
    QString currentChoice = lineChoice_->itemText(lineChoice_->currentIndex()).split(":").first();
    if (configuration_->edge() == currentChoice)
        return;

    elementChoice_->setText(configuration_->edge().split(" ").first());
    lineChoice_->blockSignals(true);
    fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol(elementChoice_->text()));
    lineChoice_->blockSignals(false);
    lineChoice_->setCurrentIndex(lineChoice_->findText(configuration_->edge(), Qt::MatchStartsWith | Qt::MatchCaseSensitive));

    if (energy_->value() != configuration_->edgeEnergy())
        energy_->setValue(configuration_->edgeEnergy());
}