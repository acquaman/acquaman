/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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

	// The fluorescence detector setup
	fluorescenceDetectorComboBox_  = createFluorescenceComboBox();
	connect(fluorescenceDetectorComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onFluorescenceChoiceChanged(int)));
	connect(configuration_, SIGNAL(fluorescenceDetectorChanged(int)), this, SLOT(updateFluorescenceDetectorComboBox(int)));

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
		energy_->setValue(configuration_->energy());
	}

	estimatedTime_ = new QLabel;
	estimatedSetTime_ = new QLabel;
	pointPerScan_ = new QLabel;
	scanEnergyRange_ = new QLabel;
	ROIsLabel_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	connect(configuration_, SIGNAL(scanAxisAdded(AMScanAxis*)), this, SLOT(onEstimatedTimeChanged()));
	connect(configuration_, SIGNAL(scanAxisRemoved(AMScanAxis*)), this, SLOT(onEstimatedTimeChanged()));
	connect(configuration_, SIGNAL(modifiedChanged(bool)), this, SLOT(onEstimatedTimeChanged()));
	connect(configuration_, SIGNAL(configurationChanged()), this, SLOT(onEstimatedTimeChanged()));

	connect(configuration_, SIGNAL(edgeChanged(QString)), this, SLOT(onEdgeChanged()));

	connect(IDEASBeamline::ideas()->ketek(),SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)),this,SLOT(onROIChange()));
	connect(IDEASBeamline::ideas()->ketek(),SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)),this,SLOT(onROIChange()));
	connect(IDEASBeamline::ideas()->ge13Element(),SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)),this,SLOT(onROIChange()));
	connect(IDEASBeamline::ideas()->ge13Element(),SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)),this,SLOT(onROIChange()));
	connect(fluorescenceDetectorComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onROIChange()));


	connect(isXRFScanCheckBox_, SIGNAL(clicked()),this,SLOT(onROIChange()));
	onROIChange();

	fluorescenceDetectorComboBox_->setCurrentIndex((int)configuration_->fluorescenceDetector());

	QFormLayout *detectorLayout = new QFormLayout;
	detectorLayout->addRow("XRF:", fluorescenceDetectorComboBox_);

	QSpinBox *numberOfScans = new QSpinBox;
	numberOfScans->setMinimum(1);
	numberOfScans->setValue(configuration_->numberOfScans());
	numberOfScans->setAlignment(Qt::AlignCenter);
	connect(numberOfScans, SIGNAL(valueChanged(int)), configuration_, SLOT(setNumberOfScans(int)));
	connect(configuration_, SIGNAL(numberOfScansChanged(int)), this, SLOT(onEstimatedTimeChanged()));

	QFormLayout *numberOfScansLayout = new QFormLayout;
	numberOfScansLayout->addRow("Number of Scans:", numberOfScans);
	numberOfScansLayout->addRow("Estimated time per scan:", estimatedTime_);
	numberOfScansLayout->addRow("Estimated time for set:", estimatedSetTime_);
	numberOfScansLayout->addRow("Energy point pers scan:", pointPerScan_);
	numberOfScansLayout->addRow("Scan energy range:", scanEnergyRange_);
	numberOfScansLayout->addRow("Selected XRF Detector Regions:", new QLabel(""));
	numberOfScansLayout->addRow("", ROIsLabel_);

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
	mainVL->addLayout(numberOfScansLayout);


	QLabel *settingsLabel = new QLabel("Scan Settings:");
	settingsLabel->setFont(QFont("Lucida Grande", 12, QFont::Bold));

	QFormLayout *configFL = new QFormLayout();
	configFL->setAlignment(Qt::AlignLeft);
	configFL->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	configFL->addRow("Scan Name: ", scanName_);
	configFL->addRow("Include: ",isXRFScanCheckBox_);
	configFL->addRow("", isTransScanCheckBox_);
	configFL->addRow("", useRefCheckBox_);

	QHBoxLayout *regionsHL = new QHBoxLayout();
	regionsHL->addStretch();
	regionsHL->addWidget(autoRegionButton_);
	regionsHL->addWidget(pseudoXAFSButton_);

	QVBoxLayout *settingsVL = new QVBoxLayout();
	settingsVL->addLayout(regionsHL);
	settingsVL->addWidget(settingsLabel);
	settingsVL->addLayout(configFL);
	settingsVL->addLayout(detectorLayout);

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

	if (time < 0)
	{
		estimatedTime_->setText("WARNING!     SCAN CONFIGURATION IS INVALID     START OR QUEUE SCAN AT YOUR OWN RISK!     WARNING!");
		estimatedSetTime_->setText("WARNING!     SCAN CONFIGURATION IS INVALID     START OR QUEUE SCAN AT YOUR OWN RISK!     WARNING!");
		pointPerScan_->setText("WARNING!     SCAN CONFIGURATION IS INVALID     START OR QUEUE SCAN AT YOUR OWN RISK!     WARNING!");
		scanEnergyRange_->setText("WARNING!     SCAN CONFIGURATION IS INVALID     START OR QUEUE SCAN AT YOUR OWN RISK!     WARNING!");
		return;
	}

	pointPerScan_->setText(QString("%1").arg(configuration_->totalPoints()));
	scanEnergyRange_->setText(QString("%1 eV - %2 eV").arg(configuration_->minEnergy()).arg(configuration_->maxEnergy()));

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

	estimatedTime_->setText(timeString);




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

	estimatedSetTime_->setText(timeString);

}

void IDEASXASScanConfigurationView::onROIChange()
{
	AMXRFDetector *detector = 0;

	if (configuration_->fluorescenceDetector() == IDEASXASScanConfiguration::None)
	{
		ROIsLabel_->setText("");
		return;
	}


	if (configuration_->fluorescenceDetector() == IDEASXASScanConfiguration::Ketek)
		detector = IDEASBeamline::ideas()->ketek();

	else if (configuration_->fluorescenceDetector() == IDEASXASScanConfiguration::Ge13Element)
		detector = IDEASBeamline::ideas()->ge13Element();

	if (detector->regionsOfInterest().isEmpty())
	{
		ROIsLabel_->setText("No XRF detector regions of interest selected.");
		if (isXRFScanCheckBox_->isChecked())
			 ROIsLabel_->setStyleSheet("QLabel { background-color : red; color : white}");
		    else
			 ROIsLabel_->setStyleSheet("QLabel { color : black; }");
		return;
	}

	QString regionsText;

	foreach (AMRegionOfInterest *region, detector->regionsOfInterest())
	{
		regionsText.append(QString("%1 (%2 eV - %3 eV)\n").arg(region->name()).arg(int(region->lowerBound())).arg(int(region->upperBound())));

	}
	ROIsLabel_->setStyleSheet("QLabel { color : black; }");
	ROIsLabel_->setText(regionsText);
}

QComboBox *IDEASXASScanConfigurationView::createFluorescenceComboBox()
{
	QComboBox *newComboBox = new QComboBox;
	newComboBox->insertItem(0, "None");
	newComboBox->insertItem(1, "KETEK");
	newComboBox->insertItem(2, "13-el Ge");

	return newComboBox;
}

void IDEASXASScanConfigurationView::updateFluorescenceDetectorComboBox(int detector)
{
	fluorescenceDetectorComboBox_->setCurrentIndex(detector);
}

void IDEASXASScanConfigurationView::onFluorescenceChoiceChanged(int id)
{
	configuration_->setFluorescenceDetector(id);
}
