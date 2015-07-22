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


#include "BioXASSideXASScanConfigurationView.h"

#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QInputDialog>
#include <QStringBuilder>

#include "acquaman/AMScanController.h"
#include "ui/AMTopFrame.h"
#include "ui/dataman/AMEXAFSScanAxisView.h"
#include "util/AMEnergyToKSpaceCalculator.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMPeriodicTable.h"


BioXASSideXASScanConfigurationView::BioXASSideXASScanConfigurationView(BioXASSideXASScanConfiguration *configuration, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	regionsView_ = new AMEXAFSScanAxisView("BioXAS-Side Region Configuration", configuration_);

	usingXRFDetectorCheckBox_ = new QCheckBox("Use 32-Element");
	usingXRFDetectorCheckBox_->setChecked(configuration_->usingXRFDetector());
	connect(configuration_->dbObject(), SIGNAL(usingXRFDetectorChanged(bool)), usingXRFDetectorCheckBox_, SLOT(setChecked(bool)));
	connect(usingXRFDetectorCheckBox_, SIGNAL(toggled(bool)), configuration_->dbObject(), SLOT(setUsingXRFDetector(bool)));

	autoRegionButton_ = new QPushButton("Auto Set XANES Regions");
	connect(autoRegionButton_, SIGNAL(clicked()), this, SLOT(setupDefaultXANESScanRegions()));

	pseudoXAFSButton_ = new QPushButton("Auto Set EXAFS Regions");
	connect(pseudoXAFSButton_, SIGNAL(clicked()), this, SLOT(setupDefaultEXAFSScanRegions()));

	scanName_ = new QLineEdit();
	scanName_->setText(configuration_->userScanName());

	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
    connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));

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

	connect(configuration_->dbObject(), SIGNAL(edgeChanged(QString)), this, SLOT(onEdgeChanged()));

	QFormLayout *energySetpointLayout = new QFormLayout;
	energySetpointLayout->addRow("Energy:", energy_);

	QHBoxLayout *energyLayout = new QHBoxLayout;
	energyLayout->addLayout(energySetpointLayout);
	energyLayout->addWidget(elementChoice_);
	energyLayout->addWidget(lineChoice_);

	QVBoxLayout *energyAndRegionLayout = new QVBoxLayout;
	energyAndRegionLayout->addLayout(energyLayout);
	energyAndRegionLayout->addWidget(regionsView_);

	QVBoxLayout *optionsLayout = new QVBoxLayout();
	optionsLayout->addWidget(usingXRFDetectorCheckBox_);
	optionsLayout->addStretch();

	QVBoxLayout *regionButtonsLayout = new QVBoxLayout();
	regionButtonsLayout->addWidget(autoRegionButton_);
	regionButtonsLayout->addWidget(pseudoXAFSButton_);
	regionButtonsLayout->addStretch();

	QFormLayout *estimatedTimeLayout = new QFormLayout();
    estimatedTimeLayout->setWidget(0, QFormLayout::LabelRole, new QLabel("Estimated Time: "));
	estimatedTimeLayout->setWidget(0, QFormLayout::FieldRole, estimatedTimeLabel_);

	QHBoxLayout *miscLayout = new QHBoxLayout();
	miscLayout->addLayout(regionButtonsLayout);
	miscLayout->addStretch();
	miscLayout->addLayout(optionsLayout);
    miscLayout->addStretch();
    miscLayout->addLayout(estimatedTimeLayout);

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addLayout(energyAndRegionLayout);
	mainVL->addLayout(miscLayout);

	mainVL->setContentsMargins(20,0,0,20);
	mainVL->setSpacing(1);
	setLayout(mainVL);
}

BioXASSideXASScanConfigurationView::~BioXASSideXASScanConfigurationView()
{

}

const AMScanConfiguration* BioXASSideXASScanConfigurationView::configuration() const
{
	return configuration_;
}


void BioXASSideXASScanConfigurationView::setupDefaultXANESScanRegions()
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

void BioXASSideXASScanConfigurationView::setupDefaultEXAFSScanRegions()
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

void BioXASSideXASScanConfigurationView::onScanNameEdited()
{
	configuration_->setName(scanName_->text());
	configuration_->setUserScanName(scanName_->text());
}

void BioXASSideXASScanConfigurationView::setEnergy()
{
	configuration_->setEnergy(energy_->value());
	regionsView_->setEdgeEnergy(energy_->value());
}

void BioXASSideXASScanConfigurationView::onElementChoiceClicked()
{
	AMElement *el = AMPeriodicTableDialog::getElement(this);

	if (el){

		elementChoice_->setText(el->symbol());
		fillLinesComboBox(el);
		lineChoice_->setCurrentIndex(0);
	}
}

void BioXASSideXASScanConfigurationView::fillLinesComboBox(AMElement *el)
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

void BioXASSideXASScanConfigurationView::onLinesComboBoxIndexChanged(int index)
{
	if (lineChoice_->count() == 0 || index == -1)
		return;

	energy_->setValue(lineChoice_->itemData(index).toDouble());
	setEnergy();
	configuration_->setEdge(lineChoice_->itemText(index).split(":").first());
}

void BioXASSideXASScanConfigurationView::onEdgeChanged()
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

void BioXASSideXASScanConfigurationView::onEstimatedTimeChanged()
{
    configuration_->blockSignals(true);
    double time = configuration_->totalTime(true);
    configuration_->blockSignals(false);

    QString timeString = AMDateTimeUtils::convertTimeToString(time);

    estimatedTimeLabel_->setText(timeString);
}

