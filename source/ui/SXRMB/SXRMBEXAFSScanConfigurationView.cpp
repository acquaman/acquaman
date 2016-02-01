#include "SXRMBEXAFSScanConfigurationView.h"

#include <QBoxLayout>
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
#include "beamline/SXRMB/SXRMBBeamline.h"

#include "ui/dataman/AMEXAFSScanAxisView.h"
#include "ui/util/AMPeriodicTableDialog.h"

#include "util/AMDateTimeUtils.h"
#include "util/AMEnergyToKSpaceCalculator.h"
#include "util/AMPeriodicTable.h"
#include "util/AMElement.h"


SXRMBEXAFSScanConfigurationView::SXRMBEXAFSScanConfigurationView(SXRMBEXAFSScanConfiguration *configuration, QWidget *parent) :
	SXRMBScanConfigurationView(parent)
{
	SXRMBBeamline *sxrmbBL = SXRMBBeamline::sxrmb();

	configuration_ = configuration;

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
		energy_->setValue(configuration_->energy());
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

	// Beamline setting layout
	QGroupBox *beamlineSettingsGroupBox = createAndLayoutBeamlineSettings();

	// Bruker detector setting
	QGroupBox *detectorSettingGroupBox = createAndLayoutDetectorSettings(configuration_);

	// layout the contents
	QGridLayout *contentLayout = new QGridLayout();
	contentLayout->addWidget(scanRegionConfigurationGroupBox, 0, 0, 1, 1);
	contentLayout->addWidget(scanInfoGroupBox, 1, 0, 1, 1);
	contentLayout->addWidget(beamlineSettingsGroupBox, 0, 4, 1, 1);
	contentLayout->addWidget(detectorSettingGroupBox, 1, 4, 1, 1);

	contentLayout->setContentsMargins(20,0,0,20);
	contentLayout->setSpacing(1);

	setLayout(contentLayout);

	connect(configuration_->dbObject(), SIGNAL(xChanged(double)), this, SLOT(onScanConfigurationSampleStageXChanged(double)));
	connect(configuration_->dbObject(), SIGNAL(zChanged(double)), this, SLOT(onScanConfigurationSampleStageZChanged(double)));
	connect(configuration_->dbObject(), SIGNAL(yChanged(double)), this, SLOT(onScanConfigurationNormalChanged(double)));
	connect(configuration_->dbObject(), SIGNAL(rotationChanged(double)), this, SLOT(onScanConfigurationRotationChanged(double)));

	connect(sxrmbBL, SIGNAL(endstationChanged(SXRMB::Endstation, SXRMB::Endstation)), this, SLOT(onBeamlineEndstationChanged(SXRMB::Endstation, SXRMB::Endstation)));
	if(sxrmbBL->isConnected())
		onEndstationSampleStagePositionChanged(-1);
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
	region->setEdgeEnergy(configuration_->energy());
	region->setRegionStart(configuration_->energy() - 30);
	region->setRegionStep(0.5);
	region->setRegionEnd(configuration_->energy() + 40);
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

void SXRMBEXAFSScanConfigurationView::onBeamlineEndstationChanged(SXRMB::Endstation fromEndstation, SXRMB::Endstation toEndstation)
{
    Q_UNUSED(toEndstation)

	// disconnect the signal/slot from the prior endstation
	SXRMBBeamline *sxrmbBL = SXRMBBeamline::sxrmb();
	AMPVwStatusControl* sampleStageXControl = sxrmbBL->endstationSampleStageX(fromEndstation);
	AMPVwStatusControl* sampleStageYControl = sxrmbBL->endstationSampleStageY(fromEndstation);
	AMPVwStatusControl* sampleStageZControl = sxrmbBL->endstationSampleStageZ(fromEndstation);
	AMPVwStatusControl* sampleStageRControl = sxrmbBL->endstationSampleStageR(fromEndstation);

	if (sampleStageXSpinBox_) {
		disconnect(sampleStageXSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageXSpinBoxEditingFinished()));
		disconnect(sampleStageXControl, SIGNAL(valueChanged(double)), this, SLOT(onEndstationSampleStagePositionChanged(double)));
	}
	if (sampleStageZSpinBox_) {
		disconnect(sampleStageZSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageZSpinBoxEditingFinished()));
		disconnect(sampleStageZControl, SIGNAL(valueChanged(double)), this, SLOT(onEndstationSampleStagePositionChanged(double)));
	}
	if (sampleStageNormalSpinBox_) {
		disconnect(sampleStageNormalSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageNormalSpinBoxEditingFinished()));
		disconnect(sampleStageYControl, SIGNAL(valueChanged(double)), this, SLOT(onEndstationSampleStagePositionChanged(double)));
	}
	if (sampleStageRotationSpinBox_) {
		disconnect(sampleStageRotationSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageRotationSpinBoxEditingFinished()));
		disconnect(sampleStageRControl, SIGNAL(valueChanged(double)), this, SLOT(onEndstationSampleStagePositionChanged(double)));
	}

	// recreate the component for the new endstation
	createAndLayoutSampleStageSpinBox(sampleStageFL_);
	updatePowerOnHVControlCheckBoxText();

	onEndstationSampleStagePositionChanged(-1);
}

void SXRMBEXAFSScanConfigurationView::onFluorescenceDetectorChanged(int detector)
{
	configuration_->setFluorescenceDetector((SXRMB::FluorescenceDetectors)detector);
}

void SXRMBEXAFSScanConfigurationView::onPowerOnTEYHVControlEnabled(bool value)
{
	configuration_->setPowerOnHVControlEnabled(value);
}

void SXRMBEXAFSScanConfigurationView::onScanNameEdited()
{
	configuration_->setName(scanName_->text());
	configuration_->setUserScanName(scanName_->text());
}

void SXRMBEXAFSScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + AMDateTimeUtils::convertTimeToString(configuration_->totalTime()));
}

void SXRMBEXAFSScanConfigurationView::setEnergy()
{
	configuration_->setEnergy(energy_->value());
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

	if (energy_->value() != configuration_->energy())
		energy_->setValue(configuration_->energy());
}

void SXRMBEXAFSScanConfigurationView::onSampleStageXSpinBoxEditingFinished(){

	configuration_->setX(sampleStageXSpinBox_->value());
	onEndstationSampleStagePositionChanged(-1);
}

void SXRMBEXAFSScanConfigurationView::onSampleStageZSpinBoxEditingFinished(){

	configuration_->setZ(sampleStageZSpinBox_->value());
	onEndstationSampleStagePositionChanged(-1);
}

void SXRMBEXAFSScanConfigurationView::onSampleStageNormalSpinBoxEditingFinished(){

	configuration_->setY(sampleStageNormalSpinBox_->value());
	onEndstationSampleStagePositionChanged(-1);
}

void SXRMBEXAFSScanConfigurationView::onSampleStageRotationSpinBoxEditingFinished(){

	configuration_->setRotation(sampleStageRotationSpinBox_->value());
	onEndstationSampleStagePositionChanged(-1);
}

void SXRMBEXAFSScanConfigurationView::onSetSampleStageFromBeamlineButtonClicked(){
	SXRMBBeamline *sxrmbBL = SXRMBBeamline::sxrmb();
	AMPVwStatusControl* sampleStageXControl = sxrmbBL->endstationSampleStageX(sxrmbBL->currentEndstation());
	AMPVwStatusControl* sampleStageYControl = sxrmbBL->endstationSampleStageY(sxrmbBL->currentEndstation());
	AMPVwStatusControl* sampleStageZControl = sxrmbBL->endstationSampleStageZ(sxrmbBL->currentEndstation());
	AMPVwStatusControl* sampleStageRControl = sxrmbBL->endstationSampleStageR(sxrmbBL->currentEndstation());

	if (sampleStageXControl) {
		sampleStageXSpinBox_->setValue(sampleStageXControl->value());
		onSampleStageXSpinBoxEditingFinished();
	}

	if (sampleStageZControl) {
		sampleStageZSpinBox_->setValue(sampleStageZControl->value());
		onSampleStageZSpinBoxEditingFinished();
	}

	if (sampleStageYControl) {
		sampleStageNormalSpinBox_->setValue(sampleStageYControl->value());
		onSampleStageNormalSpinBoxEditingFinished();
	}

	if (sampleStageRControl) {
		sampleStageRotationSpinBox_->setValue(sampleStageRControl->value());
		onSampleStageRotationSpinBoxEditingFinished();
	}

}

void SXRMBEXAFSScanConfigurationView::onEndstationSampleStagePositionChanged(double value){
	Q_UNUSED(value)

	SXRMBBeamline *sxrmbBL = SXRMBBeamline::sxrmb();
	AMPVwStatusControl* sampleStageXControl = sxrmbBL->endstationSampleStageX(sxrmbBL->currentEndstation());
	AMPVwStatusControl* sampleStageYControl = sxrmbBL->endstationSampleStageY(sxrmbBL->currentEndstation());
	AMPVwStatusControl* sampleStageZControl = sxrmbBL->endstationSampleStageZ(sxrmbBL->currentEndstation());
	AMPVwStatusControl* sampleStageRControl = sxrmbBL->endstationSampleStageR(sxrmbBL->currentEndstation());

	bool showWarningLabel = false;
	if (sampleStageXControl) {
		showWarningLabel = showWarningLabel || !sampleStageXControl->withinTolerance(sampleStageXSpinBox_->value());
	}

	if (sampleStageYControl) {
		showWarningLabel = showWarningLabel || !sampleStageYControl->withinTolerance(sampleStageNormalSpinBox_->value());
	}

	if (sampleStageZControl) {
		showWarningLabel = showWarningLabel || !sampleStageZControl->withinTolerance(sampleStageZSpinBox_->value());
	}

	if (sampleStageRControl) {
		showWarningLabel = showWarningLabel || !sampleStageRControl->withinTolerance(sampleStageRotationSpinBox_->value());
	}

	sampleStageWarningLabel_->setVisible(showWarningLabel);
}

void SXRMBEXAFSScanConfigurationView::onScanConfigurationSampleStageXChanged(double value){
	if (sampleStageXSpinBox_) {
		sampleStageXSpinBox_->setValue(value);

		onSampleStageXSpinBoxEditingFinished();
	}
}

void SXRMBEXAFSScanConfigurationView::onScanConfigurationSampleStageZChanged(double value){
	if (sampleStageZSpinBox_) {
		sampleStageZSpinBox_->setValue(value);
		onSampleStageZSpinBoxEditingFinished();
	}
}

void SXRMBEXAFSScanConfigurationView::onScanConfigurationNormalChanged(double value){
	if (sampleStageNormalSpinBox_) {
		sampleStageNormalSpinBox_->setValue(value);
		onSampleStageNormalSpinBoxEditingFinished();
	}
}

void SXRMBEXAFSScanConfigurationView::onScanConfigurationRotationChanged(double value){
	if (sampleStageRotationSpinBox_) {
		sampleStageRotationSpinBox_->setValue(value);
		onSampleStageRotationSpinBoxEditingFinished();
	}
}

void SXRMBEXAFSScanConfigurationView::clearLayout(QLayout * layout, bool deleteWidgets)
{
	if (layout->count() == 0)
		return;

	QLayoutItem* item ;
	while ((layout->count() > 0) && (item = layout->takeAt(0))) {
		QWidget* widget;
		if ( (deleteWidgets) && (widget = item->widget()) ) {
			delete widget;
		}

		if (QLayout* childLayout = item->layout()) {
			clearLayout(childLayout, deleteWidgets);
		}

		delete item;
	}
}

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

void SXRMBEXAFSScanConfigurationView::createAndLayoutSampleStageSpinBox(QFormLayout *formLayout)
{
	// reinitialize the variables and clear the layout
	clearLayout(formLayout);
	sampleStageXSpinBox_ = 0;
	sampleStageZSpinBox_ = 0;
	sampleStageNormalSpinBox_ = 0;
	sampleStageRotationSpinBox_ = 0;

	// recreate the spinboxes and signal/slot connections
	SXRMBBeamline *sxrmbBL = SXRMBBeamline::sxrmb();
	AMPVwStatusControl* sampleStageXControl = sxrmbBL->endstationSampleStageX(sxrmbBL->currentEndstation());
	AMPVwStatusControl* sampleStageYControl = sxrmbBL->endstationSampleStageY(sxrmbBL->currentEndstation());
	AMPVwStatusControl* sampleStageZControl = sxrmbBL->endstationSampleStageZ(sxrmbBL->currentEndstation());
	AMPVwStatusControl* sampleStageRControl = sxrmbBL->endstationSampleStageR(sxrmbBL->currentEndstation());

	if (sampleStageXControl) {
		sampleStageXSpinBox_ = createSampleStageSpinBox("mm", sampleStageXControl->minimumValue(), sampleStageXControl->maximumValue(), configuration_->x());
		formLayout->addRow("X Position", sampleStageXSpinBox_);

		connect(sampleStageXSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageXSpinBoxEditingFinished()));
		connect(sampleStageXControl, SIGNAL(valueChanged(double)), this, SLOT(onEndstationSampleStagePositionChanged(double)));
	}

	if (sampleStageZControl) {
		sampleStageZSpinBox_ = createSampleStageSpinBox("mm", sampleStageZControl->minimumValue(), sampleStageZControl->maximumValue(), configuration_->z());
		formLayout->addRow("Z Position", sampleStageZSpinBox_);

		connect(sampleStageZSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageZSpinBoxEditingFinished()));
		connect(sampleStageZControl, SIGNAL(valueChanged(double)), this, SLOT(onEndstationSampleStagePositionChanged(double)));
	}
	if (sampleStageYControl) {
		sampleStageNormalSpinBox_ = createSampleStageSpinBox("mm", sampleStageYControl->minimumValue(), sampleStageYControl->maximumValue(), configuration_->y());
		formLayout->addRow("Normal Position", sampleStageNormalSpinBox_);

		connect(sampleStageNormalSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageNormalSpinBoxEditingFinished()));
		connect(sampleStageYControl, SIGNAL(valueChanged(double)), this, SLOT(onEndstationSampleStagePositionChanged(double)));
	}
	if (sampleStageRControl) {
		sampleStageRotationSpinBox_ = createSampleStageSpinBox("deg", sampleStageRControl->minimumValue(), sampleStageRControl->maximumValue(), configuration_->rotation());
		formLayout->addRow("Rotation Position", sampleStageRotationSpinBox_);

		connect(sampleStageRotationSpinBox_, SIGNAL(editingFinished()), this, SLOT(onSampleStageRotationSpinBoxEditingFinished()));
		connect(sampleStageRControl, SIGNAL(valueChanged(double)), this, SLOT(onEndstationSampleStagePositionChanged(double)));
	}
}

QGroupBox *SXRMBEXAFSScanConfigurationView::createAndLayoutBeamlineSettings()
{
	sampleStageFL_ = new QFormLayout();
	createAndLayoutSampleStageSpinBox(sampleStageFL_);

	sampleStageWarningLabel_ = new QLabel("Settings do not match beamline.");
	sampleStageWarningLabel_->setStyleSheet("QLabel {color: red}");

	setSampleStageFromBeamlineButton_ = new QPushButton("Set From Beamline");
	connect(setSampleStageFromBeamlineButton_, SIGNAL(clicked()), this, SLOT(onSetSampleStageFromBeamlineButtonClicked()));

	QVBoxLayout *beamlineSettingsGroupBoxVL = new QVBoxLayout();
	beamlineSettingsGroupBoxVL->addLayout(sampleStageFL_);
	beamlineSettingsGroupBoxVL->addStretch();
	beamlineSettingsGroupBoxVL->addWidget(sampleStageWarningLabel_);
	beamlineSettingsGroupBoxVL->addWidget(setSampleStageFromBeamlineButton_);

	QGroupBox *beamlineSettingsGroupBox = new QGroupBox("Beamline Settings");
	beamlineSettingsGroupBox->setLayout(beamlineSettingsGroupBoxVL);

	return beamlineSettingsGroupBox;
}
