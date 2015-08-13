#include "BioXASXASScanConfigurationView.h"

#include "acquaman/AMScanController.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "ui/dataman/AMEXAFSScanAxisView.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMEnergyToKSpaceCalculator.h"
#include "util/AMPeriodicTable.h"
#include "util/AMDateTimeUtils.h"


BioXASXASScanConfigurationView::BioXASXASScanConfigurationView(BioXASXASScanConfiguration *configuration, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	// Initialize member variables.

	configuration_ = configuration;

	// Create scan name view.

	QLabel *scanNamePrompt = new QLabel("Name: ");

	scanName_ = new QLineEdit();

	QHBoxLayout *scanNameLayout = new QHBoxLayout();
	scanNameLayout->addWidget(scanNamePrompt);
	scanNameLayout->addWidget(scanName_);
	scanNameLayout->addStretch();

	// Create energy selection view.

	QLabel *energyPrompt = new QLabel("Energy: ");

	energy_ = new QDoubleSpinBox;
	energy_->setSuffix(" eV");
	energy_->setMinimum(0);
	energy_->setMaximum(30000);

	elementChoice_ = new QToolButton;

	lineChoice_ = new QComboBox;

	QHBoxLayout *energyLayout = new QHBoxLayout;
	energyLayout->addWidget(energyPrompt);
	energyLayout->addWidget(energy_);
	energyLayout->addStretch();
	energyLayout->addWidget(elementChoice_);
	energyLayout->addWidget(lineChoice_);

	// Create regions view.

	regionsView_ = new AMEXAFSScanAxisView("Region Configuration", configuration_);

	QLabel *estimatedTimePrompt = new QLabel("Estimated time: ");
	estimatedTimeLabel_ = new QLabel();

	autoRegionButton_ = new QPushButton("Auto Set XANES Regions");
	pseudoXAFSButton_ = new QPushButton("Auto Set EXAFS Regions");

	QHBoxLayout *regionsButtonsLayout = new QHBoxLayout();
	regionsButtonsLayout->addWidget(autoRegionButton_);
	regionsButtonsLayout->addWidget(pseudoXAFSButton_);

	QHBoxLayout *regionsExtrasLayout = new QHBoxLayout();
	regionsExtrasLayout->addWidget(estimatedTimePrompt);
	regionsExtrasLayout->addWidget(estimatedTimeLabel_);
	regionsExtrasLayout->addStretch();
	regionsExtrasLayout->addLayout(regionsButtonsLayout);

	QVBoxLayout *regionsLayout = new QVBoxLayout();
	regionsLayout->addWidget(regionsView_);
	regionsLayout->addLayout(regionsExtrasLayout);

	// Create detectors view.

	detectorsView_ = new AMGenericStepScanConfigurationDetectorsView(configuration_, AMBeamline::bl()->exposedDetectors());

	// Create and set main layouts

	QVBoxLayout *scanSettingsBoxLayout = new QVBoxLayout();
	scanSettingsBoxLayout->addLayout(scanNameLayout);
	scanSettingsBoxLayout->addLayout(energyLayout);
	scanSettingsBoxLayout->addLayout(regionsLayout);

	QGroupBox *scanSettingsBox = new QGroupBox("Scan settings");
	scanSettingsBox->setLayout(scanSettingsBoxLayout);

	QVBoxLayout *detectorBoxLayout = new QVBoxLayout();
	detectorBoxLayout->addWidget(detectorsView_);

	QGroupBox *detectorBox = new QGroupBox("Detectors");
	detectorBox->setLayout(detectorBoxLayout);

	QScrollArea *detectorScrollArea = new QScrollArea;
	detectorScrollArea->setWidget(detectorBox);
	detectorScrollArea->setFrameStyle(QFrame::NoFrame);

	QHBoxLayout *finalHorizontalLayout = new QHBoxLayout();
	finalHorizontalLayout->addWidget(scanSettingsBox);
	finalHorizontalLayout->addWidget(detectorScrollArea);
	finalHorizontalLayout->setContentsMargins(20,0,0,20);
	finalHorizontalLayout->setSpacing(1);

	QVBoxLayout *finalVerticalLayout = new QVBoxLayout();
	finalVerticalLayout->addStretch();
	finalVerticalLayout->addLayout(finalHorizontalLayout);
	finalVerticalLayout->addStretch();

	setLayout(finalVerticalLayout);

	// Make connections.

	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(energy_, SIGNAL(editingFinished()), this, SLOT(setEnergy()));
	connect(elementChoice_, SIGNAL(clicked()), this, SLOT(onElementChoiceClicked()));
	connect(lineChoice_, SIGNAL(currentIndexChanged(int)), this, SLOT(onLinesComboBoxIndexChanged(int)));
	connect(autoRegionButton_, SIGNAL(clicked()), this, SLOT(setupDefaultXANESScanRegions()));
	connect(pseudoXAFSButton_, SIGNAL(clicked()), this, SLOT(setupDefaultEXAFSScanRegions()));

	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged(double)));
	connect(configuration_->dbObject(), SIGNAL(edgeChanged(QString)), this, SLOT(onEdgeChanged()));

	// Current settings.

	scanName_->setText(configuration_->userScanName());


	if (configuration_->edge().isEmpty()){

		elementChoice_->setText("Cu");
		fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol("Cu"));
		lineChoice_->setCurrentIndex(0);

	} else {
		elementChoice_->setText(configuration_->edge().split(" ").first());
		lineChoice_->blockSignals(true);
		fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol(elementChoice_->text()));
		lineChoice_->setCurrentIndex(lineChoice_->findText(configuration_->edge(), Qt::MatchStartsWith | Qt::MatchCaseSensitive));
		lineChoice_->blockSignals(false);
		energy_->setValue(configuration_->energy());
	}
}

BioXASXASScanConfigurationView::~BioXASXASScanConfigurationView()
{

}

const AMScanConfiguration* BioXASXASScanConfigurationView::configuration() const
{
	return configuration_;
}

void BioXASXASScanConfigurationView::setScanConfiguration(BioXASXASScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_) {

		}

		configuration_ = newConfiguration;

		if (configuration_) {

		}

		emit scanConfigurationChanged(configuration_);
	}
}

void BioXASXASScanConfigurationView::clear()
{

}

void BioXASXASScanConfigurationView::update()
{

}

void BioXASXASScanConfigurationView::refresh()
{

}

void BioXASXASScanConfigurationView::setupDefaultXANESScanRegions()
{
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

void BioXASXASScanConfigurationView::setupDefaultEXAFSScanRegions()
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
	region->setRegionStart(AMEnergyToKSpaceCalculator::k(region->edgeEnergy(), double(region->edgeEnergy()) + 40));
	region->setRegionStep(0.05);
	region->setRegionEnd(10);
	region->setRegionTime(1.0);
	region->setMaximumTime(10.0);
	regionsView_->insertEXAFSRegion(2, region);
}

void BioXASXASScanConfigurationView::onScanNameEdited()
{
	configuration_->setUserScanName(scanName_->text());
}

void BioXASXASScanConfigurationView::setEnergy()
{
	configuration_->setEnergy(energy_->value());
	regionsView_->setEdgeEnergy(energy_->value());
}

void BioXASXASScanConfigurationView::onElementChoiceClicked()
{
	AMElement *el = AMPeriodicTableDialog::getElement(this);

	if (el){

		elementChoice_->setText(el->symbol());
		fillLinesComboBox(el);
		lineChoice_->setCurrentIndex(0);
	}
}

void BioXASXASScanConfigurationView::fillLinesComboBox(AMElement *el)
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

void BioXASXASScanConfigurationView::onLinesComboBoxIndexChanged(int index)
{
	if (lineChoice_->count() == 0 || index == -1)
		return;

	energy_->setValue(lineChoice_->itemData(index).toDouble());
	setEnergy();
	configuration_->setEdge(lineChoice_->itemText(index).split(":").first());
}

void BioXASXASScanConfigurationView::onEdgeChanged()
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

void BioXASXASScanConfigurationView::onEstimatedTimeChanged(double time)
{
   estimatedTimeLabel_->setText(AMDateTimeUtils::convertTimeToString(time));
}
