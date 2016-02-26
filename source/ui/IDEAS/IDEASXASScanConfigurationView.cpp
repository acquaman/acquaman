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
#include "util/AMEnergyToKSpaceCalculator.h"
#include "util/AMPeriodicTable.h"
#include "util/AMDateTimeUtils.h"
#include "ui/AMTopFrame.h"
#include "ui/dataman/AMEXAFSScanAxisView.h"
#include "ui/util/AMPeriodicTableDialog.h"



IDEASXASScanConfigurationView::IDEASXASScanConfigurationView(IDEASXASScanConfiguration *configuration, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	regionsView_ = new AMEXAFSScanAxisView("IDEAS Region Configuration", configuration_);

	autoRegionButton_ = new QPushButton("Auto Set XANES Regions");
	connect(autoRegionButton_, SIGNAL(clicked()), this, SLOT(setupDefaultXANESScanRegions()));

	pseudoXAFSButton_ = new QPushButton("Auto Set EXAFS Regions");
	connect(pseudoXAFSButton_, SIGNAL(clicked()), this, SLOT(setupDefaultEXAFSScanRegions()));

	scanName_ = new QLineEdit();
	scanName_->setText(configuration_->name());

	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));

	isTransScanCheckBox_ = new QCheckBox("transmission");
	isTransScanCheckBox_->setChecked(configuration->usingTransmission());
	connect(isTransScanCheckBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setUsingTransmission(bool)));

	useRefCheckBox_ = new QCheckBox("reference sample");
	useRefCheckBox_->setChecked(configuration->usingReference());
	useRefCheckBox_->setEnabled(configuration->usingTransmission());
	connect(useRefCheckBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setUsingReference(bool)));
	connect(isTransScanCheckBox_, SIGNAL(clicked(bool)),this, SLOT(onIsTransScanChecked(bool)));


	// The fluorescence detector setup
	fluorescenceDetectorComboBox_  = createFluorescenceComboBox();
	connect(fluorescenceDetectorComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onFluorescenceChoiceChanged(int)));
	connect(IDEASBeamline::ideas()->ge13Element(), SIGNAL(connected(bool)), this, SLOT(updateFluorescenceDetectorComboBoxGe13Element(bool)));
	connect(configuration_->dbObject(), SIGNAL(fluorescenceDetectorChanged(int)), this, SLOT(updateFluorescenceDetectorComboBox(int)));


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
	pointPerScan_ = new QLabel;
	scanEnergyRange_ = new QLabel;
	ROIsLabel_ = new QLabel;
	peakingSetting_ = new QLabel;

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

	connect(IDEASBeamline::ideas()->ketek(), SIGNAL(peakingTimeChanged(double)), this, SLOT(onPeakingTimeChanged(double)));
//	connect(IDEASBeamline::ideas()->ge13Element(), SIGNAL(peakingTimeChanged(double)), this, SLOT(onPeakingTimeChanged(double)));

	onROIChange();

	fluorescenceDetectorComboBox_->setCurrentIndex((int)configuration_->fluorescenceDetector());

	QFormLayout *numberOfScansLayout = new QFormLayout;
	numberOfScansLayout->addRow("Estimated time per scan:", estimatedTime_);
	numberOfScansLayout->addRow("Energy point pers scan:", pointPerScan_);
	numberOfScansLayout->addRow("Scan energy range:", scanEnergyRange_);
	numberOfScansLayout->addRow("Selected XRF Detector Regions:", new QLabel(""));
	numberOfScansLayout->addRow("", ROIsLabel_);
	numberOfScansLayout->addRow("Peaking Settings: ", peakingSetting_);

	QFormLayout *energySetpointLayout = new QFormLayout;
	energySetpointLayout->addRow("Energy:", energy_);

	QHBoxLayout *energyLayout = new QHBoxLayout;
	energyLayout->addLayout(energySetpointLayout);
	energyLayout->addWidget(elementChoice_);
	energyLayout->addWidget(lineChoice_);

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addLayout(energyLayout);
	mainVL->addWidget(regionsView_);
	mainVL->addLayout(numberOfScansLayout);

	QLabel *settingsLabel = new QLabel("Scan Settings:");
	settingsLabel->setFont(QFont("Lucida Grande", 12, QFont::Bold));

	QFormLayout *configFL = new QFormLayout();
	configFL->setAlignment(Qt::AlignLeft);
	configFL->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
	configFL->addRow("Scan Name: ", scanName_);
	configFL->addRow("Include: ",fluorescenceDetectorComboBox_);
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
	region->setRegionStart(configuration_->energy() - 60);
	region->setRegionStep(5.0);
	region->setRegionEnd(configuration_->energy() - 20);
	region->setRegionTime(2.0);
	regionsView_->insertEXAFSRegion(0, region);

	region = new AMScanAxisEXAFSRegion;
	region->setEdgeEnergy(configuration_->energy());
	region->setRegionStart(configuration_->energy() - 20);
	region->setRegionStep(0.5);
	region->setRegionEnd(configuration_->energy() + 60);
	region->setRegionTime(2.0);
	regionsView_->insertEXAFSRegion(1, region);

	region = new AMScanAxisEXAFSRegion;
	region->setEdgeEnergy(configuration_->energy());
	region->setRegionStart(configuration_->energy() + 60);
	region->setRegionStep(2.0);
	region->setRegionEnd(configuration_->energy() + 100);
	region->setRegionTime(2.0);
	regionsView_->insertEXAFSRegion(2, region);

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
	configuration_->blockSignals(true);
	double time = configuration_->totalTime(true);
	configuration_->blockSignals(false);

	if (time < 0)
	{
		estimatedTime_->setText("WARNING!     SCAN CONFIGURATION IS INVALID     START OR QUEUE SCAN AT YOUR OWN RISK!     WARNING!");
		pointPerScan_->setText("WARNING!     SCAN CONFIGURATION IS INVALID     START OR QUEUE SCAN AT YOUR OWN RISK!     WARNING!");
		scanEnergyRange_->setText("WARNING!     SCAN CONFIGURATION IS INVALID     START OR QUEUE SCAN AT YOUR OWN RISK!     WARNING!");
		return;
	}

	pointPerScan_->setText(QString("%1").arg(configuration_->scanAxisAt(0)->numberOfPoints()));

	AMScanAxisEXAFSRegion *lastRegion = qobject_cast<AMScanAxisEXAFSRegion *>(configuration_->scanAxisAt(0)->regionAt(configuration_->scanAxisAt(0)->regionCount()-1));
	if(lastRegion->inKSpace())
		scanEnergyRange_->setText(QString("%1 eV - %2 eV")
							  .arg(double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()))
							  .arg(double(AMEnergyToKSpaceCalculator::energy(lastRegion->edgeEnergy(), lastRegion->regionEnd()))));
	else
		scanEnergyRange_->setText(QString("%1 eV - %2 eV")
						  .arg(double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()))
						  .arg(double(lastRegion->regionEnd())));


	QString timeString = AMDateTimeUtils::convertTimeToString(time);
	estimatedTime_->setText(timeString);
}

void IDEASXASScanConfigurationView::onIsTransScanChecked(bool checked)
{
	if (checked)
	{
		useRefCheckBox_->setEnabled(true);
		useRefCheckBox_->setChecked(true);
	}
	else
	{
		useRefCheckBox_->setEnabled(false);
		useRefCheckBox_->setChecked(false);
	}


}

void IDEASXASScanConfigurationView::onROIChange()
{
	AMXRFDetector *detector = 0;

	if (configuration_->fluorescenceDetector().testFlag(IDEAS::NoXRF))
		ROIsLabel_->setText("");

	else if (configuration_->fluorescenceDetector().testFlag(IDEAS::Ketek))
		detector = IDEASBeamline::ideas()->ketek();

	else if (configuration_->fluorescenceDetector().testFlag(IDEAS::Ge13Element) && IDEASBeamline::ideas()->ge13Element()->isConnected())
		detector = IDEASBeamline::ideas()->ge13Element();

	if (detector){

		if (detector->regionsOfInterest().isEmpty())
			ROIsLabel_->setText("No XRF detector regions of interest selected.");

		else {

			QString regionsText;

			foreach (AMRegionOfInterest *region, detector->regionsOfInterest())
				regionsText.append(QString("%1 (%2 eV - %3 eV)\n").arg(region->name()).arg(int(region->lowerBound())).arg(int(region->upperBound())));

			ROIsLabel_->setStyleSheet("QLabel { color : black; }");
			ROIsLabel_->setText(regionsText);
		}
	}
}

QComboBox *IDEASXASScanConfigurationView::createFluorescenceComboBox()
{
	QComboBox *newComboBox = new QComboBox;
	newComboBox->insertItem((int)IDEAS::NoXRF, "None");
	newComboBox->insertItem((int)IDEAS::Ketek, "KETEK");
	if (IDEASBeamline::ideas()->ge13Element()->isConnected())
		newComboBox->insertItem((int)IDEAS::Ge13Element, "13-el Ge");
	return newComboBox;
}

void IDEASXASScanConfigurationView::updateFluorescenceDetectorComboBox(IDEAS::FluorescenceDetectors detector)
{
	fluorescenceDetectorComboBox_->setCurrentIndex(detector);
}

void IDEASXASScanConfigurationView::updateFluorescenceDetectorComboBoxGe13Element(bool connected)
{
	int currentIndex = fluorescenceDetectorComboBox_->currentIndex();
	fluorescenceDetectorComboBox_->removeItem((int)IDEAS::Ge13Element);

	if (connected)
		fluorescenceDetectorComboBox_->insertItem((int)IDEAS::Ge13Element, "13-el Ge");

	updateFluorescenceDetectorComboBox((IDEAS::FluorescenceDetector)currentIndex);
}


void IDEASXASScanConfigurationView::onFluorescenceChoiceChanged(int id)
{
	configuration_->setFluorescenceDetector(id);
}

void IDEASXASScanConfigurationView::onPeakingTimeChanged(double value)
{
	if (value == 0.3)
		peakingSetting_->setText("High Rate / Low Resolution");
	else if (value == 2.0)
		peakingSetting_->setText("High Resolution / Low Rate");
	else if (value == 4.0)
		peakingSetting_->setText("Ultra Resolution / Slow Rate");
	else
		peakingSetting_->setText("High Rate / Low Resolution");

}
