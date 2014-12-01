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

#include "application/SXRMB/SXRMB.h"
#include "acquaman/AMScanController.h"
#include "acquaman/SXRMB/SXRMBEXAFSScanConfiguration.h"
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
	scanName_ = new QLineEdit(configuration_->userScanName());
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

	// Bruker detector setting
	enableBrukerDetector_ = new QCheckBox("Enable Bruker Detector");
	enableBrukerDetector_->setChecked(configuration_->enableBrukerDetector());
	connect(enableBrukerDetector_, SIGNAL(stateChanged(int)), this, SLOT(onEnableBrukerDetectorChanged(int)));

	QVBoxLayout *detectorBoxLayout = new QVBoxLayout;
	detectorBoxLayout->addWidget(enableBrukerDetector_);

	QGroupBox *detectorGroupBox = new QGroupBox("Detector Setting");
	detectorGroupBox->setLayout(detectorBoxLayout);

	QHBoxLayout * scanSettingBoxLayout = new QHBoxLayout;
	scanSettingBoxLayout->addWidget(scanInfoGroupBox);
	scanSettingBoxLayout->addWidget(detectorGroupBox);

	// Scan content layout
	QVBoxLayout *scanSettingContentVL = new QVBoxLayout();
	scanSettingContentVL->setSpacing(10);
	scanSettingContentVL->addWidget(scanRegionConfigurationGroupBox);
	scanSettingContentVL->addLayout(scanSettingBoxLayout);

	// Beamline setting layout
	SXRMBBeamline *sxrmbBL = SXRMBBeamline::sxrmb();
	sampleStageXSpinBox_ = createSampleStageSpinBox("mm", sxrmbBL->microprobeSampleStageX()->minimumValue(), sxrmbBL->microprobeSampleStageX()->maximumValue(), configuration_->microprobeSampleStageX());
	sampleStageZSpinBox_ = createSampleStageSpinBox("mm", sxrmbBL->microprobeSampleStageZ()->minimumValue(), sxrmbBL->microprobeSampleStageZ()->maximumValue(), configuration_->microprobeSampleStageZ());
	sampleStageNormalSpinBox_ = createSampleStageSpinBox("mm", sxrmbBL->microprobeSampleStageY()->minimumValue(), sxrmbBL->microprobeSampleStageY()->maximumValue(), configuration_->normalPosition());
	sampleStageWarningLabel_ = new QLabel("Settings do not match beamline.");
	sampleStageWarningLabel_->setStyleSheet("QLabel {color: red}");
	setSampleStageFromBeamlineButton_ = new QPushButton("Set From Beamline");

	connect(sampleStageXSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageXSpinBoxEditingFinished()));
	connect(sampleStageZSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageZSpinBoxEditingFinished()));
	connect(sampleStageNormalSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageNormalSpinBoxEditingFinished()));
	connect(setSampleStageFromBeamlineButton_, SIGNAL(clicked()), this, SLOT(onSetSampleStageFromBeamlineButtonClicked()));

	connect(sxrmbBL->microprobeSampleStageX(), SIGNAL(valueChanged(double)), this, SLOT(onMicroprobeSampleStagePositionChanged(double)));
	connect(sxrmbBL->microprobeSampleStageY(), SIGNAL(valueChanged(double)), this, SLOT(onMicroprobeSampleStagePositionChanged(double)));
	connect(sxrmbBL->microprobeSampleStageZ(), SIGNAL(valueChanged(double)), this, SLOT(onMicroprobeSampleStagePositionChanged(double)));
	if(sxrmbBL->isConnected())
		onMicroprobeSampleStagePositionChanged(-1);

	connect(configuration, SIGNAL(microprobeSampleStageXChanged(double)), this, SLOT(onScanConfigurationMicroprobeSampleStageXChanged(double)));
	connect(configuration, SIGNAL(microprobeSampleStageZChanged(double)), this, SLOT(onScanConfigurationMicroprobeSampleStageZChanged(double)));
	connect(configuration->dbObject(), SIGNAL(normalPositionChanged(double)), this, SLOT(onScanConfigurationMicroprobeNormalChanged(double)));

	QFormLayout *sampleStageFL = new QFormLayout();
	sampleStageFL->addRow("X Position", sampleStageXSpinBox_);
	sampleStageFL->addRow("Z Position", sampleStageZSpinBox_);
	sampleStageFL->addRow("Normal Position", sampleStageNormalSpinBox_);

	QVBoxLayout *beamlineSettingsGroupBoxVL = new QVBoxLayout();
	beamlineSettingsGroupBoxVL->addLayout(sampleStageFL);
	beamlineSettingsGroupBoxVL->addStretch();
	beamlineSettingsGroupBoxVL->addWidget(sampleStageWarningLabel_);
	beamlineSettingsGroupBoxVL->addWidget(setSampleStageFromBeamlineButton_);

	QGroupBox *beamlineSettingsGroupBox = new QGroupBox("Beamline Settings");
	beamlineSettingsGroupBox->setLayout(beamlineSettingsGroupBoxVL);

	// setup the squeeze contents
	QHBoxLayout *squeezeContents = new QHBoxLayout;
	squeezeContents->setSpacing(10);
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
	estimatedTime_->setText("Estimated time per scan:\t" + SXRMB::convertTimeToString(configuration_->totalTime()));
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

void SXRMBEXAFSScanConfigurationView::onEdgeChanged(){
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

	configuration_->setMicroprobeSampleStageX(sampleStageXSpinBox_->value());
	onMicroprobeSampleStagePositionChanged(-1);
}

void SXRMBEXAFSScanConfigurationView::onSampleStageZSpinBoxEditingFinished(){

	configuration_->setMicroprobeSampleStageZ(sampleStageZSpinBox_->value());
	onMicroprobeSampleStagePositionChanged(-1);
}

void SXRMBEXAFSScanConfigurationView::onSampleStageNormalSpinBoxEditingFinished(){

	configuration_->setNormalPosition(sampleStageNormalSpinBox_->value());
	onMicroprobeSampleStagePositionChanged(-1);
}

void SXRMBEXAFSScanConfigurationView::onSetSampleStageFromBeamlineButtonClicked(){
	sampleStageXSpinBox_->setValue(SXRMBBeamline::sxrmb()->microprobeSampleStageX()->value());
	sampleStageZSpinBox_->setValue(SXRMBBeamline::sxrmb()->microprobeSampleStageZ()->value());
	sampleStageNormalSpinBox_->setValue(SXRMBBeamline::sxrmb()->microprobeSampleStageY()->value());

	onSampleStageXSpinBoxEditingFinished();
	onSampleStageZSpinBoxEditingFinished();
	onSampleStageNormalSpinBoxEditingFinished();
}

void SXRMBEXAFSScanConfigurationView::onMicroprobeSampleStagePositionChanged(double value){
	Q_UNUSED(value)

	if(!SXRMBBeamline::sxrmb()->microprobeSampleStageX()->withinTolerance(sampleStageXSpinBox_->value()) || !SXRMBBeamline::sxrmb()->microprobeSampleStageZ()->withinTolerance(sampleStageZSpinBox_->value()) || !SXRMBBeamline::sxrmb()->microprobeSampleStageY()->withinTolerance(sampleStageNormalSpinBox_->value()))
		sampleStageWarningLabel_->show();
	else
		sampleStageWarningLabel_->hide();
}

void SXRMBEXAFSScanConfigurationView::onScanConfigurationMicroprobeSampleStageXChanged(double value){
	sampleStageXSpinBox_->setValue(value);

	onSampleStageXSpinBoxEditingFinished();
}

void SXRMBEXAFSScanConfigurationView::onScanConfigurationMicroprobeSampleStageZChanged(double value){
	sampleStageZSpinBox_->setValue(value);

	onSampleStageZSpinBoxEditingFinished();
}

void SXRMBEXAFSScanConfigurationView::onScanConfigurationMicroprobeNormalChanged(double value){
	sampleStageNormalSpinBox_->setValue(value);

	onSampleStageNormalSpinBoxEditingFinished();
}

void SXRMBEXAFSScanConfigurationView::onEnableBrukerDetectorChanged(int state)
{
	if(state == Qt::Checked)
		configuration_->setEnableBrukerDetector(true);
	else
		configuration_->setEnableBrukerDetector(false);}

QDoubleSpinBox *SXRMBEXAFSScanConfigurationView::createSampleStageSpinBox(QString units, double minimumValue, double maximumValue, double defaultValue) {
	QDoubleSpinBox *sampleStageSpinBox = new QDoubleSpinBox();
	sampleStageSpinBox->setSuffix(" " % units);
	sampleStageSpinBox->setSingleStep(0.001);
	sampleStageSpinBox->setDecimals(3);
	sampleStageSpinBox->setAlignment(Qt::AlignCenter);
	sampleStageSpinBox->setFixedWidth(110);

	sampleStageSpinBox->setRange(minimumValue, maximumValue);
	sampleStageSpinBox->setValue(defaultValue);

	return sampleStageSpinBox;
}
