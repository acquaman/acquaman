#include "SXRMBEXAFSScanConfigurationView.h"

#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QInputDialog>
#include <QStringBuilder>

#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QToolButton>
#include <QGroupBox>
#include <QFormLayout>

#include "acquaman/AMScanController.h"
#include "acquaman/SXRMB/SXRMBEXAFSScanConfiguration.h"
#include "beamline/IDEAS/IDEASBeamline.h"
#include "ui/AMTopFrame.h"
#include "ui/dataman/AMEXAFSScanAxisView.h"
#include "util/AMEnergyToKSpaceCalculator.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMPeriodicTable.h"
#include "util/AMElement.h"

#include "beamline/SXRMB/SXRMBBeamline.h"


SXRMBEXAFSScanConfigurationView::SXRMBEXAFSScanConfigurationView(SXRMBEXAFSScanConfiguration *configuration, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	topFrame_ = new AMTopFrame("Configure an XAS Scan");
	topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

	regionsView_ = new AMEXAFSScanAxisView("SXRMB Region Configuration", configuration_);

	autoRegionButton_ = new QPushButton("Auto Set XANES Regions");
	connect(autoRegionButton_, SIGNAL(clicked()), this, SLOT(setupDefaultXANESScanRegions()));

	pseudoXAFSButton_ = new QPushButton("Auto Set EXAFS Regions");
	connect(pseudoXAFSButton_, SIGNAL(clicked()), this, SLOT(setupDefaultEXAFSScanRegions()));

	// Energy (Eo) selection
	energy_ = new QDoubleSpinBox;
	energy_->setSuffix(" eV");
	energy_->setMinimum(0);
	energy_->setMaximum(10000);
	connect(energy_, SIGNAL(editingFinished()), this, SLOT(setEnergy()));

	elementChoice_ = new QToolButton;
	connect(elementChoice_, SIGNAL(clicked()), this, SLOT(onElementChoiceClicked()));

	lineChoice_ = new QComboBox;
	connect(lineChoice_, SIGNAL(currentIndexChanged(int)), this, SLOT(onLinesComboBoxIndexChanged(int)));

	if (configuration_->edge().isEmpty()){

		elementChoice_->setText("Cl");
		fillLinesComboBox(AMPeriodicTable::table()->elementBySymbol("Cl"));
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

	QHBoxLayout *regionsHL = new QHBoxLayout();
	regionsHL->addStretch();
	regionsHL->addWidget(autoRegionButton_);
	regionsHL->addWidget(pseudoXAFSButton_);

	QVBoxLayout *scanRegionConfigurationBoxLayout = new QVBoxLayout;
	scanRegionConfigurationBoxLayout->addLayout(energyLayout);
	scanRegionConfigurationBoxLayout->addWidget(regionsView_);
	scanRegionConfigurationBoxLayout->addLayout(regionsHL);

	QGroupBox *scanRegionConfigurationGroupBox = new QGroupBox("Scan Region Configuration");
	scanRegionConfigurationGroupBox->setLayout(scanRegionConfigurationBoxLayout);

	// Scan information:  scan name selection
	scanName_ = new QLineEdit();
	scanName_->setText(configuration_->userScanName());
	scanName_->setAlignment(Qt::AlignCenter);
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

	// Scan information: the estimated scan time.
	estimatedTime_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	QVBoxLayout *scanInfoBoxLayout = new QVBoxLayout;
	scanInfoBoxLayout->addLayout(scanNameLayout);
	scanInfoBoxLayout->addWidget(estimatedTime_);

	QGroupBox *scanInfoGroupBox = new QGroupBox("Scan Information");
	scanInfoGroupBox->setLayout(scanInfoBoxLayout);

	// Scan content layout
	QVBoxLayout *scanSettingContentVL = new QVBoxLayout();
	scanSettingContentVL->setSpacing(10);
	scanSettingContentVL->addWidget(scanRegionConfigurationGroupBox);
	scanSettingContentVL->addWidget(scanInfoGroupBox);

	QGroupBox *beamlineSettingsGroupBox = new QGroupBox("Beamline Settings");

	sampleStageXSpinBox_ = new QDoubleSpinBox();
	sampleStageZSpinBox_ = new QDoubleSpinBox();
	sampleStageNormalSpinBox_ = new QDoubleSpinBox();
	sampleStageXSpinBox_->setRange(SXRMBBeamline::sxrmb()->microprobeSampleStageX()->minimumValue(), SXRMBBeamline::sxrmb()->microprobeSampleStageX()->maximumValue());
	sampleStageZSpinBox_->setRange(SXRMBBeamline::sxrmb()->microprobeSampleStageZ()->minimumValue(), SXRMBBeamline::sxrmb()->microprobeSampleStageZ()->maximumValue());
	sampleStageNormalSpinBox_->setRange(SXRMBBeamline::sxrmb()->microprobeSampleStageY()->minimumValue(), SXRMBBeamline::sxrmb()->microprobeSampleStageY()->maximumValue());
	sampleStageWarningLabel_ = new QLabel();
	setSampleStageFromBeamlineButton_ = new QPushButton("Set From Beamline");

	connect(sampleStageXSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageXSpinBoxEditingFinished()));
	connect(sampleStageZSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageZSpinBoxEditingFinished()));
	connect(sampleStageNormalSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageNormalSpinBoxEditingFinished()));
	connect(setSampleStageFromBeamlineButton_, SIGNAL(clicked()), this, SLOT(onSetSampleStageFromBeamlineButtonClicked()));

	QFormLayout *sampleStageFL = new QFormLayout();
	sampleStageFL->addRow("X Position", sampleStageXSpinBox_);
	sampleStageFL->addRow("Z Position", sampleStageZSpinBox_);
	sampleStageFL->addRow("Normal Position", sampleStageNormalSpinBox_);

	QVBoxLayout *beamlineSettingsGroupBoxVL = new QVBoxLayout();
	beamlineSettingsGroupBoxVL->addLayout(sampleStageFL);
	beamlineSettingsGroupBoxVL->addWidget(sampleStageWarningLabel_);
	beamlineSettingsGroupBoxVL->addWidget(setSampleStageFromBeamlineButton_);

	if(SXRMBBeamline::sxrmb()->isConnected())
		onMicroprobeSampleStagePositionChanged(-1);

	beamlineSettingsGroupBox->setLayout(beamlineSettingsGroupBoxVL);

	QHBoxLayout *squeezeContents = new QHBoxLayout;
	squeezeContents->addStretch();
	squeezeContents->addLayout(scanSettingContentVL);
	squeezeContents->addWidget(beamlineSettingsGroupBox);
	squeezeContents->addStretch();

	// Main content layout
	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(topFrame_);
	mainVL->addStretch();
	mainVL->addLayout(squeezeContents);
	mainVL->addStretch();

	mainVL->setContentsMargins(20,0,0,20);
	mainVL->setSpacing(1);

	setLayout(mainVL);
}

SXRMBEXAFSScanConfigurationView::~SXRMBEXAFSScanConfigurationView()
{

}

const AMScanConfiguration* SXRMBEXAFSScanConfigurationView::configuration() const
{
	return configuration_;
}


void SXRMBEXAFSScanConfigurationView::setupDefaultXANESScanRegions()
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

void SXRMBEXAFSScanConfigurationView::setupDefaultEXAFSScanRegions()
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

void SXRMBEXAFSScanConfigurationView::onScanNameEdited()
{
	configuration_->setName(scanName_->text());
	configuration_->setUserScanName(scanName_->text());
}

void SXRMBEXAFSScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + convertTimeToString(configuration_->totalTime()));
}

void SXRMBEXAFSScanConfigurationView::setEnergy()
{
	configuration_->setEdgeEnergy(energy_->value());
	regionsView_->setEdgeEnergy(energy_->value());
}

void SXRMBEXAFSScanConfigurationView::onElementChoiceClicked()
{
	AMElement *el = AMPeriodicTableDialog::getElement(this);

	if (el){

		elementChoice_->setText(el->symbol());
		fillLinesComboBox(el);
		lineChoice_->setCurrentIndex(0);
	}
}

void SXRMBEXAFSScanConfigurationView::fillLinesComboBox(AMElement *el)
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

void SXRMBEXAFSScanConfigurationView::onLinesComboBoxIndexChanged(int index)
{
	if (lineChoice_->count() == 0 || index == -1)
		return;

	energy_->setValue(lineChoice_->itemData(index).toDouble());
	setEnergy();
	configuration_->setEdge(lineChoice_->itemText(index).split(":").first());
}

void SXRMBEXAFSScanConfigurationView::onEdgeChanged()
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

void SXRMBEXAFSScanConfigurationView::onSampleStageXSpinBoxEditingFinished(){

}

void SXRMBEXAFSScanConfigurationView::onSampleStageZSpinBoxEditingFinished(){

}

void SXRMBEXAFSScanConfigurationView::onSampleStageNormalSpinBoxEditingFinished(){

}

void SXRMBEXAFSScanConfigurationView::onSetSampleStageFromBeamlineButtonClicked(){

}

void SXRMBEXAFSScanConfigurationView::onMicroprobeSampleStagePositionChanged(double value){
	Q_UNUSED(value)
}

QString SXRMBEXAFSScanConfigurationView::convertTimeToString(double time)
{
	QString timeString;

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

	return timeString;
}
