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


#include "AMXRFDetailedDetectorView.h"

#include <QHBoxLayout>
#include <QStringBuilder>
#include <QComboBox>

#include "util/AMDataSourcePlotSettings.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"
#include "ui/AMSelectionDialog.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "ui/beamline/AMDeadTimeButton.h"
#include "dataman/AMAnalysisBlock.h"
#include "ui/beamline/AMRegionOfInterestView.h"

#include "ui/AMHeaderButton.h"

AMXRFDetailedDetectorView::~AMXRFDetailedDetectorView(){}

AMXRFDetailedDetectorView::AMXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent)
	: AMXRFBaseDetectorView(detector, parent)
{
	emissionLineValidator_ = new AMNameAndRangeValidator(this);
	pileUpPeakValidator_ = new AMNameAndRangeValidator(this);
	combinationPileUpPeakValidator_ = new AMNameAndRangeValidator(this);

	periodicTable_ = new AMSelectablePeriodicTable(this);
	periodicTable_->buildPeriodicTable();

	emissionLineLegendColors_.insert("K", QColor(0, 255, 0));
	emissionLineLegendColors_.insert("L", QColor(255, 255, 0));
	emissionLineLegendColors_.insert("M", QColor(255, 0, 0));
	emissionLineLegendColors_.insert("Default", palette().button().color());
	pileUpPeakColor_ = QColor(42, 149, 77);
	combinationPileUpPeakColor_ = QColor(42, 149, 77);

	currentElement_ = periodicTable_->elementBySymbol("Fe");
	combinationElement_ = periodicTable_->elementBySymbol("Ca");

	regionOfInterestMapper_ = new QSignalMapper(this);
	connect(regionOfInterestMapper_, SIGNAL(mapped(int)), this, SLOT(onRegionOfInterestBoundsChanged(int)));
}

void AMXRFDetailedDetectorView::buildDetectorView()
{
	// This is going to extend the base view, so we will simply call it's build method too.
	AMXRFBaseDetectorView::buildDetectorView();

	buildPeriodicTableViewAndElementView();
	buildShowSpectraButtons();
	buildPileUpPeakButtons();
	buildDeadTimeView();
	buildEnergyRangeSpinBoxView();
	buildRegionOfInterestViews();
}

void AMXRFDetailedDetectorView::buildRegionOfInterestViews()
{
	AMRegionOfInterestView *view = new AMRegionOfInterestView(detector_->regionsOfInterest());
	connect(detector_, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), view, SLOT(addRegionOfInterest(AMRegionOfInterest*)));
	connect(detector_, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), view, SLOT(removeRegionOfInterest(AMRegionOfInterest*)));

	editRegionsOfInterestButton_ = new QPushButton("Edit ROIs");
	editRegionsOfInterestButton_->setEnabled(detector_->regionsOfInterestCount() > 0);
	connect(editRegionsOfInterestButton_, SIGNAL(clicked()), view, SLOT(show()));
	connect(editRegionsOfInterestButton_, SIGNAL(clicked()), view, SLOT(raise()));

	rightLayout_->addWidget(editRegionsOfInterestButton_);
}

void AMXRFDetailedDetectorView::buildDeadTimeView()
{
	bool deadTimeEnabled = detector_->hasDeadTimeCorrection();

	deadTimeLabel_ = new QLabel("Dead Time: 0%");
	connect(detector_, SIGNAL(deadTimeChanged()), this, SLOT(onDeadTimeChanged()));

	deadTimeButtons_ = new QButtonGroup(this);
	deadTimeButtons_->setExclusive(false);
	QGridLayout *deadTimeButtonLayout = new QGridLayout;

	for (int i = 0, elements = detector_->elements(), factor = int(sqrt(elements)); i < elements && deadTimeEnabled; i++){

		AMDeadTimeButton *deadTimeButton = new AMDeadTimeButton(detector_->inputCountSourceAt(i), detector_->outputCountSourceAt(i), 30.0, 50.0);
		deadTimeButton->setCheckable(true);
		deadTimeButton->setFixedSize(20, 20);
		deadTimeButtonLayout->addWidget(deadTimeButton, int(i/factor), i%factor);
		deadTimeButtons_->addButton(deadTimeButton, i);
	}

	connect(deadTimeButtons_, SIGNAL(buttonClicked(int)), this, SLOT(onDeadTimeButtonClicked()));

	QHBoxLayout *squeezedDeadTimeButtonsLayout = new QHBoxLayout;
	squeezedDeadTimeButtonsLayout->addStretch();
	squeezedDeadTimeButtonsLayout->addLayout(deadTimeButtonLayout);
	squeezedDeadTimeButtonsLayout->addStretch();

	QVBoxLayout *deadTimeLayout = new QVBoxLayout;
	deadTimeLayout->addWidget(deadTimeLabel_, 0, Qt::AlignCenter);
	deadTimeLayout->addLayout(squeezedDeadTimeButtonsLayout);

	// Remove the "addStretch" from the basic layout before adding the new elements.
	rightLayout_->removeItem(rightLayout_->itemAt(rightLayout_->count()-1));
	rightLayout_->addLayout(deadTimeLayout);
	rightLayout_->addStretch();

	deadTimeLabel_->setVisible(deadTimeEnabled);
}

void AMXRFDetailedDetectorView::buildEnergyRangeSpinBoxView()
{
	showEnergyRangeSpinBoxes_ = new QPushButton(QIcon(":/system-run.png"), "Settings");
	showEnergyRangeSpinBoxes_->setCheckable(true);

	minimumEnergySpinBox_ = new QDoubleSpinBox;
	minimumEnergySpinBox_->setMinimum(0);
	minimumEnergySpinBox_->setMaximum(100000);
	minimumEnergySpinBox_->setPrefix("Minimum: ");
	minimumEnergySpinBox_->setSuffix(" eV");
	minimumEnergySpinBox_->setValue(energyRange().minimum());
	minimumEnergySpinBox_->setAlignment(Qt::AlignCenter);
	minimumEnergySpinBox_->hide();

	maximumEnergySpinBox_ = new QDoubleSpinBox;
	maximumEnergySpinBox_->setMinimum(0);
	maximumEnergySpinBox_->setMaximum(100000);
	maximumEnergySpinBox_->setPrefix("Maximum: ");
	maximumEnergySpinBox_->setSuffix(" eV");
	maximumEnergySpinBox_->setValue(energyRange().maximum());
	maximumEnergySpinBox_->setAlignment(Qt::AlignCenter);
	maximumEnergySpinBox_->hide();

	connect(showEnergyRangeSpinBoxes_, SIGNAL(toggled(bool)), minimumEnergySpinBox_, SLOT(setVisible(bool)));
	connect(showEnergyRangeSpinBoxes_, SIGNAL(toggled(bool)), maximumEnergySpinBox_, SLOT(setVisible(bool)));
	connect(minimumEnergySpinBox_, SIGNAL(editingFinished()), this, SLOT(onMinimumEnergyChanged()));
	connect(maximumEnergySpinBox_, SIGNAL(editingFinished()), this, SLOT(onMaximumEnergyChanged()));

	QVBoxLayout *energyLayout = new QVBoxLayout;
	energyLayout->addWidget(showEnergyRangeSpinBoxes_);
	energyLayout->addWidget(minimumEnergySpinBox_);
	energyLayout->addWidget(maximumEnergySpinBox_);
	energyLayout->addStretch();

	rightLayout_->addLayout(energyLayout);
}

void AMXRFDetailedDetectorView::buildPileUpPeakButtons()
{
	showPileUpPeaksButton_ = new QPushButton("Show Fe Pile Up Peaks");
	showPileUpPeaksButton_->setMaximumHeight(25);
	showPileUpPeaksButton_->setCheckable(true);
	showPileUpPeaksButton_->setEnabled(false);
	showCombinationPileUpPeaksButton_ = new QPushButton("Show Combination Peaks");
	showCombinationPileUpPeaksButton_->setMaximumHeight(25);
	showCombinationPileUpPeaksButton_->setCheckable(true);
	showCombinationPileUpPeaksButton_->setEnabled(false);
	combinationChoiceButton_ = new QToolButton;
	combinationChoiceButton_->setMaximumHeight(25);
	combinationChoiceButton_->setText("Ca");
	combinationChoiceButton_->setEnabled(false);

	rowAbovePeriodicTableLayout_->addWidget(showPileUpPeaksButton_);
	rowAbovePeriodicTableLayout_->addWidget(showCombinationPileUpPeaksButton_);
	rowAbovePeriodicTableLayout_->addWidget(combinationChoiceButton_);

	connect(showPileUpPeaksButton_, SIGNAL(clicked()), this, SLOT(updatePileUpPeaks()));
	connect(showCombinationPileUpPeaksButton_, SIGNAL(clicked()), this, SLOT(updateCombinationPileUpPeaks()));
	connect(showPileUpPeaksButton_, SIGNAL(toggled(bool)), this, SLOT(updatePileUpPeaksButtonText()));
	connect(showCombinationPileUpPeaksButton_, SIGNAL(toggled(bool)), this, SLOT(updateCombinationPileUpPeaksButtonText()));
	connect(showPileUpPeaksButton_, SIGNAL(toggled(bool)), showCombinationPileUpPeaksButton_, SLOT(setEnabled(bool)));
	connect(showPileUpPeaksButton_, SIGNAL(toggled(bool)), combinationChoiceButton_, SLOT(setEnabled(bool)));
	connect(combinationChoiceButton_, SIGNAL(clicked()), this, SLOT(onCombinationChoiceButtonClicked()));
}

void AMXRFDetailedDetectorView::buildShowSpectraButtons()
{
	spectraComboBox_ = new QComboBox;

	for (int i = 0, size = detector_->allSpectrumSources().size(); i < size; i++){

		AMDataSource *source = detector_->allSpectrumSources().at(i);
		spectraComboBox_->insertItem(i, source->name());
	}

	QPushButton *showMultipleSpectraButton = new QPushButton("Show Multiple Spectra");
	showWaterfall_ = new QCheckBox("Waterfall");
	showWaterfall_->setChecked(true);

	logScaleButton_ = new QPushButton("Log scale");
	logScaleButton_->setCheckable(true);

	QHBoxLayout *showSpectraLayout = new QHBoxLayout;
	showSpectraLayout->addStretch();
	showSpectraLayout->addWidget(logScaleButton_);
	showSpectraLayout->addWidget(showWaterfall_);
	showSpectraLayout->addWidget(showMultipleSpectraButton);
	showSpectraLayout->addWidget(spectraComboBox_);

	topLayout_->addLayout(showSpectraLayout);

	connect(spectraComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSpectrumComboBoxIndexChanged(int)));
	connect(showMultipleSpectraButton, SIGNAL(clicked()), this, SLOT(onShowMultipleSpectraButtonClicked()));
	connect(showWaterfall_, SIGNAL(toggled(bool)), this, SLOT(onWaterfallUpdateRequired()));
	connect(logScaleButton_, SIGNAL(toggled(bool)), this, SLOT(onLogScaleClicked(bool)));
	connect(logScaleButton_, SIGNAL(toggled(bool)), showWaterfall_, SLOT(setDisabled(bool)));

	spectraComboBox_->setCurrentIndex(detector_->allSpectrumSources().size()-1);
}

void AMXRFDetailedDetectorView::buildPeriodicTableViewAndElementView()
{
	periodicTableView_ = new AMSelectablePeriodicTableView(periodicTable_);
	periodicTableView_->buildPeriodicTableView();

	elementView_ = new AMSelectableElementView(qobject_cast<AMSelectableElement *>(periodicTable_->elementBySymbol("Fe")));
	elementView_->setAbsorptionEdgeVisibility(false);

	QFont font(this->font());
	font.setBold(true);

	QLabel *legend = new QLabel(QString("Legend"));
	legend->setFont(font);
	QLabel *kLines = new QLabel(QString("K-lines"));
	kLines->setFont(font);
	kLines->setStyleSheet(QString("QLabel { background-color: rgb(%1,%2,%3) ; border-width: 2px ; border-style: solid }").arg(emissionLineLegendColors_.value("K").red()).arg(emissionLineLegendColors_.value("K").green()).arg(emissionLineLegendColors_.value("K").blue()));
	QLabel *lLines = new QLabel(QString("L-lines"));
	lLines->setFont(font);
	lLines->setStyleSheet(QString("QLabel { background-color: rgb(%1,%2,%3) ; border-width: 2px ; border-style: solid }").arg(emissionLineLegendColors_.value("L").red()).arg(emissionLineLegendColors_.value("L").green()).arg(emissionLineLegendColors_.value("L").blue()));
	QLabel *mLines = new QLabel(QString("M-lines"));
	mLines->setFont(font);
	mLines->setStyleSheet(QString("QLabel { background-color: rgb(%1,%2,%3) ; border-width: 2px ; border-style: solid }").arg(emissionLineLegendColors_.value("M").red()).arg(emissionLineLegendColors_.value("M").green()).arg(emissionLineLegendColors_.value("M").blue()));

	QVBoxLayout *legendLayout = new QVBoxLayout;
	legendLayout->addWidget(legend);
	legendLayout->addWidget(kLines);
	legendLayout->addWidget(lLines);
	legendLayout->addWidget(mLines);
	legendLayout->addStretch();

	QHBoxLayout *periodicTableAndElementViewLayout = new QHBoxLayout;
	periodicTableAndElementViewLayout->addLayout(legendLayout);
	periodicTableAndElementViewLayout->addWidget(periodicTableView_);
	periodicTableAndElementViewLayout->addWidget(elementView_);

	QPushButton *removeAllEmissionLinesButton = new QPushButton(QIcon(":/trashcan.png"), "Clear Emission Lines");
	removeAllEmissionLinesButton->setMaximumHeight(25);
	QPushButton *removeAllRegionsOfInterestButton = new QPushButton(QIcon(":/trashcan.png"), "Clear Regions Of Interest");
	removeAllRegionsOfInterestButton->setMaximumHeight(25);

	rowAbovePeriodicTableLayout_ = new QHBoxLayout;
	rowAbovePeriodicTableLayout_->addWidget(removeAllEmissionLinesButton);
	rowAbovePeriodicTableLayout_->addWidget(removeAllRegionsOfInterestButton);
	rowAbovePeriodicTableLayout_->addStretch();

	QVBoxLayout *completeBottomLayout = new QVBoxLayout;
	completeBottomLayout->addLayout(rowAbovePeriodicTableLayout_);
	completeBottomLayout->addLayout(periodicTableAndElementViewLayout);

	periodicTableHeaderButton_ = new AMHeaderButton();
	periodicTableHeaderButton_->setText("Periodic Table");
	periodicTableHeaderButton_->setArrowType(Qt::DownArrow);

	bottomLayoutWidget_ = new QWidget();
	bottomLayoutWidget_->setLayout(completeBottomLayout);

	QVBoxLayout *bottomLayoutWithHeader = new QVBoxLayout();
	bottomLayoutWithHeader->addWidget(periodicTableHeaderButton_);
	bottomLayoutWithHeader->addWidget(bottomLayoutWidget_);

	bottomLayout_->addLayout(bottomLayoutWithHeader);

	connect(emissionLineValidator_, SIGNAL(validatorChanged()), this, SLOT(updateEmissionLineMarkers()));
	connect(periodicTable_, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementSelected(AMElement*)));
	connect(periodicTable_, SIGNAL(elementDeselected(AMElement*)), this, SLOT(onElementDeselected(AMElement*)));
	connect(periodicTable_, SIGNAL(emissionLineSelected(AMEmissionLine)), this, SLOT(onEmissionLineSelected(AMEmissionLine)));
	connect(periodicTable_, SIGNAL(emissionLineDeselected(AMEmissionLine)), this, SLOT(onEmissionLineDeselected(AMEmissionLine)));
	connect(periodicTableView_, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementClicked(AMElement*)));
	connect(removeAllEmissionLinesButton, SIGNAL(clicked()), this, SLOT(removeAllEmissionLineMarkers()));
	connect(removeAllRegionsOfInterestButton, SIGNAL(clicked()), this, SLOT(removeAllRegionsOfInterest()));
	connect(detector_, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
	connect(detector_, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));

	connect(periodicTableHeaderButton_, SIGNAL(clicked()), this, SLOT(onPeriodicTableHeaderButtonClicked()));
}

void AMXRFDetailedDetectorView::onElementClicked(AMElement *element)
{
	// This is always safe because we know the periodic table is an AMSelectablePeriodicTable.
	elementView_->setElement(qobject_cast<AMSelectableElement *>(element));
	currentElement_ = element;
	highlightCurrentElementRegionOfInterest();
	updatePileUpPeaksButtonText();
	updatePileUpPeaks();
}

void AMXRFDetailedDetectorView::onElementSelected(AMElement *element)
{
	QColor color = AMDataSourcePlotSettings::nextColor();

	foreach (AMEmissionLine emissionLine, element->emissionLines()){

		if (emissionLineValidator_->isValid(emissionLine.name(), emissionLine.energy())){

			MPlotPoint *newLine = new MPlotPoint(QPointF(emissionLine.energy(), 0));
			newLine->setMarker(MPlotMarkerShape::VerticalBeam, 1e6, QPen(color), QBrush(color));
			newLine->setDescription(emissionLine.greekName() % ": " % emissionLine.energyString() % " eV");
			plot_->addItem(newLine);
			emissionLineMarkers_ << newLine;
		}
	}

	showPileUpPeaksButton_->setEnabled(true);
}

void AMXRFDetailedDetectorView::onElementDeselected(AMElement *element)
{
	QString symbol = element->symbol();

	foreach(MPlotItem *item, emissionLineMarkers_){

				if (item->description().contains(QRegExp(QString("^%1 (K|L|M)").arg(symbol))))
			if (plot_->removeItem(item)){

				emissionLineMarkers_.removeOne(item);
				delete item;
			}
	}

	showPileUpPeaksButton_->setEnabled(periodicTable_->selectedElements().size() > 0);
}

void AMXRFDetailedDetectorView::updateEmissionLineMarkers()
{
	foreach (AMElement *element, periodicTable_->selectedElements())
		onElementDeselected(element);

	foreach (AMElement *element, periodicTable_->selectedElements())
		onElementSelected(element);
}

void AMXRFDetailedDetectorView::addEmissionLineNameFilter(const QRegExp &newNameFilter)
{
	emissionLineValidator_->addNameFilter(newNameFilter);
	elementView_->addEmissionLineNameFilter(newNameFilter);
}

bool AMXRFDetailedDetectorView::removeEmissionLineNameFilter(int index)
{
	return emissionLineValidator_->removeNameFilter(index) && elementView_->removeEmissionLineNameFilter(index);
}

bool AMXRFDetailedDetectorView::removeEmissionLineNameFilter(const QRegExp &filter)
{
	return emissionLineValidator_->removeNameFilter(filter) && elementView_->removeEmissionLineNameFilter(filter);
}

void AMXRFDetailedDetectorView::addPileUpPeakNameFilter(const QRegExp &newNameFilter)
{
	pileUpPeakValidator_->addNameFilter(newNameFilter);
}

bool AMXRFDetailedDetectorView::removePileUpPeakNameFilter(int index)
{
	return pileUpPeakValidator_->removeNameFilter(index);
}

bool AMXRFDetailedDetectorView::removePileUpPeakNameFilter(const QRegExp &filter)
{
	return pileUpPeakValidator_->removeNameFilter(filter);
}

void AMXRFDetailedDetectorView::addCombinationPileUpPeakNameFilter(const QRegExp &newNameFilter)
{
	combinationPileUpPeakValidator_->addNameFilter(newNameFilter);
}

bool AMXRFDetailedDetectorView::removeCombinationPileUpPeakNameFilter(int index)
{
	return combinationPileUpPeakValidator_->removeNameFilter(index);
}

bool AMXRFDetailedDetectorView::removeCombinationPileUpPeakNameFilter(const QRegExp &filter)
{
	return combinationPileUpPeakValidator_->removeNameFilter(filter);
}

void AMXRFDetailedDetectorView::setEnergyRange(const AMRange &newRange)
{
	emissionLineValidator_->setRange(newRange);
	pileUpPeakValidator_->setRange(newRange);
	combinationPileUpPeakValidator_->setRange(newRange);
	periodicTableView_->setEnergyRange(newRange);
	elementView_->setEnergyRange(newRange);
	minimumEnergySpinBox_->setValue(newRange.minimum());
	maximumEnergySpinBox_->setValue(newRange.maximum());
}

void AMXRFDetailedDetectorView::setEnergyRange(double minimum, double maximum)
{
	setEnergyRange(AMRange(minimum, maximum));
}

void AMXRFDetailedDetectorView::setMinimumEnergy(double newMinimum)
{
	emissionLineValidator_->setMinimum(newMinimum);
	periodicTableView_->setMinimumEnergy(newMinimum);
	elementView_->setMinimumEnergy(newMinimum);
	minimumEnergySpinBox_->setValue(newMinimum);

	setMinimumEnergyImplementation(newMinimum);
}

void AMXRFDetailedDetectorView::setMaximumEnergy(double newMaximum)
{
	emissionLineValidator_->setMaximum(newMaximum);
	periodicTableView_->setMaximumEnergy(newMaximum);
	elementView_->setMaximumEnergy(newMaximum);
	maximumEnergySpinBox_->setValue(newMaximum);

	setMaximumEnergyImplementation(newMaximum);
}

void AMXRFDetailedDetectorView::onMinimumEnergyChanged()
{
	setMinimumEnergy(minimumEnergySpinBox_->value());
}


void AMXRFDetailedDetectorView::onMaximumEnergyChanged()
{
	setMaximumEnergy(maximumEnergySpinBox_->value());
}

void AMXRFDetailedDetectorView::expandPeriodicTableViews(){
	hidePeriodicTableViews(false);
}

void AMXRFDetailedDetectorView::collapsePeriodTableViews(){
	hidePeriodicTableViews(true);
}

void AMXRFDetailedDetectorView::onEmissionLineSelected(const AMEmissionLine &emissionLine)
{
	if (!detector_->regionOfInterest(emissionLine))
		detector_->addRegionOfInterest(emissionLine);

	AMRegionOfInterest *newRegion = detector_->regionOfInterest(emissionLine);
	regionOfInterestMapper_->setMapping(newRegion, regionOfInterestMarkers_.size());
	connect(newRegion, SIGNAL(boundingRangeChanged(AMRange)), regionOfInterestMapper_, SLOT(map()));

	MPlotMarkerTransparentVerticalRectangle *newMarker = new MPlotMarkerTransparentVerticalRectangle(newRegion->name(), newRegion->energy(), newRegion->lowerBound(), newRegion->upperBound());
	plot_->insertItem(newMarker);
	newMarker->setYAxisTarget(plot_->axisScale(MPlot::VerticalRelative));
	regionOfInterestMarkers_.insert(emissionLine, newMarker);

	updatePeriodicTableButtonColors(emissionLine);
	editRegionsOfInterestButton_->setEnabled(detector_->regionsOfInterestCount() > 0);
}

void AMXRFDetailedDetectorView::onEmissionLineDeselected(const AMEmissionLine &emissionLine)
{
	MPlotItem *itemToBeRemoved = regionOfInterestMarkers_.value(emissionLine);

	if (itemToBeRemoved){

			regionOfInterestMapper_->removeMappings(detector_->regionOfInterest(emissionLine));
			detector_->removeRegionOfInterest(emissionLine);
			plot_->removeItem(itemToBeRemoved);
			regionOfInterestMarkers_.remove(emissionLine);
			delete itemToBeRemoved;

			updatePeriodicTableButtonColors(emissionLine);
			editRegionsOfInterestButton_->setEnabled(detector_->regionsOfInterestCount() > 0);
	}
}

void AMXRFDetailedDetectorView::onRegionOfInterestAdded(AMRegionOfInterest *newRegion)
{
	QString regionName = newRegion->name();
	AMSelectableElement *element = qobject_cast<AMSelectableElement *>(periodicTable_->elementBySymbol(regionName.split(" ").first()));
	AMEmissionLine emissionLine = AMEmissionLine();

	foreach (AMEmissionLine line, element->emissionLines())
		if (regionName == line.name())
			emissionLine = line;

	if (!emissionLine.isNull() && !element->isSelected(emissionLine)){

		element->selectEmissionLine(emissionLine);
		elementView_->updateEmissionLineViewList();
	}
}

void AMXRFDetailedDetectorView::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	QString regionName = region->name();
	AMSelectableElement *element = qobject_cast<AMSelectableElement *>(periodicTable_->elementBySymbol(regionName.split(" ").first()));
	AMEmissionLine emissionLine = AMEmissionLine();

	foreach (AMEmissionLine line, element->emissionLines())
		if (regionName == line.name())
			emissionLine = line;

	if (!emissionLine.isNull()){

		element->deselectEmissionLine(emissionLine);
		elementView_->updateEmissionLineViewList();
	}
}

void AMXRFDetailedDetectorView::removeAllEmissionLineMarkers()
{
	foreach (MPlotItem *item, emissionLineMarkers_)
		if (plot_->removeItem(item))
			delete item;

	emissionLineMarkers_.clear();
	periodicTable_->deselectAllElements();
}

void AMXRFDetailedDetectorView::removeAllRegionsOfInterest()
{
	foreach (MPlotMarkerTransparentVerticalRectangle *item, regionOfInterestMarkers_){

		AMEmissionLine line = regionOfInterestMarkers_.key(item);
		AMSelectableElement *element = qobject_cast<AMSelectableElement *>(periodicTable_->elementBySymbol(line.elementSymbol()));
		element->deselectEmissionLine(line);
	}

	elementView_->setElement(elementView_->element());
}

void AMXRFDetailedDetectorView::updatePeriodicTableButtonColors(const AMEmissionLine &line)
{
	AMSelectableElement *element = qobject_cast<AMSelectableElement *>(periodicTable_->elementBySymbol(line.elementSymbol()));

	if (element){

		if (element->hasSelectedEmissionLines()){

			QStringList keyString;

			foreach (AMEmissionLine line, element->selectedEmissionLines())
				keyString << line.lineName().left(1);

			keyString.removeDuplicates();
			periodicTableView_->button(periodicTable_->elementBySymbol(line.elementSymbol()))->setStyleSheet(buildStyleSheet(keyString.join("")));
		}

		else{

			periodicTableView_->button(periodicTable_->elementBySymbol(line.elementSymbol()))->setStyleSheet(buildStyleSheet("Default"));
		}
	}
}

const QString AMXRFDetailedDetectorView::buildStyleSheet(const QString &colorMapKey) const
{
	QString styleSheet;

	// Default is unselected.  It can only have one value in the key.
	if (colorMapKey.contains("Default")){

		QColor color = emissionLineLegendColors_.value(colorMapKey);
		styleSheet = QString("QToolButton { background-color: rgb(%1,%2,%3); }").arg(color.red()).arg(color.green()).arg(color.blue());
	}

	else {

		QColor color = emissionLineLegendColors_.value(colorMapKey.left(1));
		styleSheet = QString("QToolButton { background-color: rgb(%1,%2,%3); }").arg(color.red()).arg(color.green()).arg(color.blue());
	}

	return styleSheet;
}

void AMXRFDetailedDetectorView::highlightCurrentElementRegionOfInterest()
{
	foreach (MPlotMarkerTransparentVerticalRectangle *marker, regionOfInterestMarkers_)
		marker->setHighlighted(marker->description().contains(QRegExp(currentElement_->symbol() % " (K|L|M)")));
}

void AMXRFDetailedDetectorView::setLineColors(const QColor &kColor, const QColor &lColor, const QColor &mColor, const QColor &defaultColor)
{
	emissionLineLegendColors_.clear();
	emissionLineLegendColors_.insert("K", kColor);
	emissionLineLegendColors_.insert("L", lColor);
	emissionLineLegendColors_.insert("M", mColor);
	emissionLineLegendColors_.insert("Default", defaultColor);
}

void AMXRFDetailedDetectorView::setKEmissionLineColor(const QColor &color)
{
	emissionLineLegendColors_.remove("K");
	emissionLineLegendColors_.insert("K", color);
}

void AMXRFDetailedDetectorView::setLEmissionLineColor(const QColor &color)
{
	emissionLineLegendColors_.remove("L");
	emissionLineLegendColors_.insert("L", color);
}

void AMXRFDetailedDetectorView::setMEmissionLineColor(const QColor &color)
{
	emissionLineLegendColors_.remove("M");
	emissionLineLegendColors_.insert("M", color);
}

void AMXRFDetailedDetectorView::setDefaultEmissionLineColor(const QColor &color)
{
	emissionLineLegendColors_.remove("Default");
	emissionLineLegendColors_.insert("Default", color);
}

void AMXRFDetailedDetectorView::setPileUpPeakColor(const QColor &color)
{
	pileUpPeakColor_ = color;
}

void AMXRFDetailedDetectorView::setCombinationPileUpPeakColor(const QColor &color)
{
	combinationPileUpPeakColor_ = color;
}

void AMXRFDetailedDetectorView::onSpectrumComboBoxIndexChanged(int index)
{
	removeAllPlotItems(spectraPlotItems_);
	plot_->setAxisScaleWaterfall(MPlot::Left, 0);

	AMDataSource *source = detector_->allSpectrumSources().at(index);
	MPlotSeriesBasic *newSpectrum = new MPlotSeriesBasic;
	newSpectrum->setModel(new AMDataSourceSeriesData(source), true);
	newSpectrum->setMarker(MPlotMarkerShape::None);
	newSpectrum->setDescription(source->name());
	newSpectrum->setLinePen(QPen(Qt::red));

	spectraPlotItems_ << newSpectrum;
	plot_->insertItem(newSpectrum, 0);
}

void AMXRFDetailedDetectorView::onShowMultipleSpectraButtonClicked()
{
	QStringList spectraNames;

	foreach (AMDataSource *source, detector_->allSpectrumSources())
		spectraNames << source->name();

	AMSelectionDialog dialog("Choose Spectra", spectraNames);

	if (dialog.exec()){

		removeAllPlotItems(spectraPlotItems_);
		spectraNames = dialog.selectedItems();

		// These will still be in order.
		int spectraNamesIterator = 0;

		foreach (AMDataSource *source, detector_->allSpectrumSources()){

			if (spectraNames.contains(source->name())){

				MPlotSeriesBasic *newSpectrum = new MPlotSeriesBasic;
				newSpectrum->setModel(new AMDataSourceSeriesData(source), true);
				newSpectrum->setMarker(MPlotMarkerShape::None);
				newSpectrum->setDescription(source->name());
				newSpectrum->setLinePen(QPen(AMDataSourcePlotSettings::nextColor()));

				spectraPlotItems_ << newSpectrum;
				plot_->insertItem(newSpectrum, spectraNamesIterator++);
			}
		}

		connect(detector_->dataSource()->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onWaterfallUpdateRequired()));
		onWaterfallUpdateRequired();
	}
}

void AMXRFDetailedDetectorView::onWaterfallUpdateRequired()
{
	if (showWaterfall_->isChecked() && !logScaleButton_->isChecked())
		plot_->setAxisScaleWaterfall(MPlot::Left, double(spectraPlotItems_.at(0)->dataRect().bottom())/double(spectraPlotItems_.size()));

	else
		plot_->setAxisScaleWaterfall(MPlot::Left, 0);
}

void AMXRFDetailedDetectorView::removeAllPlotItems(QList<MPlotItem *> &items)
{
	foreach (MPlotItem *item, items)
		if (plot_->removeItem(item)){

			item->signalSource()->disconnect();
			delete item;
		}

	items.clear();
}

void AMXRFDetailedDetectorView::updatePileUpPeaksButtonText()
{
	showPileUpPeaksButton_->setText(QString("%1 %2 Pile Up Peaks").arg(showPileUpPeaksButton_->isChecked() ? "Hide" : "Show").arg(currentElement_->symbol()));
}

void AMXRFDetailedDetectorView::updateCombinationPileUpPeaksButtonText()
{
	showCombinationPileUpPeaksButton_->setText(QString("%1 Combination Peaks").arg(showPileUpPeaksButton_->isChecked() ? "Hide" : "Show"));
}

void AMXRFDetailedDetectorView::updatePileUpPeaks()
{
	removeAllPlotItems(pileUpPeakMarkers_);

	if (showPileUpPeaksButton_->isChecked() && showPileUpPeaksButton_->isEnabled() && periodicTable_->isSelected(currentElement_)){

		for (int i = 0, size = currentElement_->emissionLines().size(); i < size; i++)
			for (int j = i; j < size; j++)
				addPileUpMarker(currentElement_->emissionLines().at(i), currentElement_->emissionLines().at(j));
	}

	updateCombinationPileUpPeaks();
}

void AMXRFDetailedDetectorView::updateCombinationPileUpPeaks()
{
	removeAllPlotItems(combinationPileUpPeakMarkers_);

	if (showCombinationPileUpPeaksButton_->isChecked() && showCombinationPileUpPeaksButton_->isEnabled()){

		for (int i = 0, iSize = currentElement_->emissionLines().size(); i < iSize; i++)
			for (int j = 0, jSize = combinationElement_->emissionLines().size(); j < jSize; j++)
				addPileUpMarker(currentElement_->emissionLines().at(i), combinationElement_->emissionLines().at(j));
	}
}

void AMXRFDetailedDetectorView::addPileUpMarker(const AMEmissionLine &firstLine, const AMEmissionLine &secondLine)
{
	// You can't have pile up if the original line is below your threshold.
	if (!emissionLineValidator_->isValid(firstLine.name(), firstLine.energy()))
		return;

	AMNameAndRangeValidator *validator;
	QColor markerColor;

	if (firstLine.elementSymbol() == secondLine.elementSymbol()){

		validator = pileUpPeakValidator_;
		markerColor = QColor(42, 149, 77);
	}

	else{

		validator = combinationPileUpPeakValidator_;
		markerColor = QColor(24, 116, 205);
	}

	double energy = firstLine.energy() + secondLine.energy();

	if (validator->isValid(firstLine.name(), firstLine.energy())
			&& validator->isValid(secondLine.name(), secondLine.energy())
			&& validator->isValid(energy)){

		MPlotPoint *newMarker = new MPlotPoint(QPointF(energy, 0));
		newMarker->setMarker(MPlotMarkerShape::VerticalBeam, 1e6, QPen(markerColor), QBrush(markerColor));
		newMarker->setDescription(QString("%1 + %2: %3 eV").arg(firstLine.greekName()).arg(secondLine.greekName()).arg(energy));
		plot_->addItem(newMarker);

		if (firstLine.elementSymbol() == secondLine.elementSymbol())
			pileUpPeakMarkers_ << newMarker;

		else
			combinationPileUpPeakMarkers_ << newMarker;
	}
}

void AMXRFDetailedDetectorView::onCombinationChoiceButtonClicked()
{
	AMElement *el = AMPeriodicTableDialog::getElement();

	if (el){

		combinationChoiceButton_->setText(el->symbol());
		combinationElement_ = el;
		updateCombinationPileUpPeaks();
	}
}

void AMXRFDetailedDetectorView::onDeadTimeChanged()
{
		deadTimeLabel_->setText(QString("Dead Time:\t%1%").arg(detector_->deadTime()*100, 0, 'f', 0));
}

void AMXRFDetailedDetectorView::onDeadTimeButtonClicked()
{
	if (detector_->elements() > 1){

		QList<AMDataSource *> newSum;

		for (int i = 0, size = deadTimeButtons_->buttons().size(); i < size; i++)
			if (!deadTimeButtons_->button(i)->isChecked())
				newSum << (detector_->hasDeadTimeCorrection() ? detector_->analyzedSpectrumSources().at(i) : detector_->rawSpectrumSources().at(i));

		((AMAnalysisBlock *)detector_->dataSource())->setInputDataSources(newSum);
	}
}

void AMXRFDetailedDetectorView::onRegionOfInterestBoundsChanged(int id)
{
	AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(regionOfInterestMapper_->mapping(id));
	regionOfInterestMarkers_.values().at(id)->setLowEnd(region->lowerBound());
	regionOfInterestMarkers_.values().at(id)->setHighEnd(region->upperBound());
}

void AMXRFDetailedDetectorView::onLogScaleClicked(bool logScale)
{
	if (logScale){

		logScaleButton_->setText("Linear scale");
		plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(1, MPLOT_POS_INFINITY));
	}

	else {

		logScaleButton_->setText("Log scale");
		plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(0, MPLOT_POS_INFINITY));
	}

	plot_->axisScaleLeft()->setLogScaleEnabled(logScale);
	onWaterfallUpdateRequired();
}

void AMXRFDetailedDetectorView::onPeriodicTableHeaderButtonClicked(){
	if(bottomLayoutWidget_->isHidden())
		hidePeriodicTableViews(false);
	else
		hidePeriodicTableViews(true);
}

void AMXRFDetailedDetectorView::resizeToMinimumHeight(){
	resize(size().width(), minimumSizeHint().height());
	emit resized();
}

void AMXRFDetailedDetectorView::hidePeriodicTableViews(bool setHidden){
	if(setHidden && !bottomLayoutWidget_->isHidden()){
		periodicTableHeaderButton_->setArrowType(Qt::RightArrow);
		bottomLayoutWidget_->hide();
		QTimer::singleShot(0, this, SLOT(resizeToMinimumHeight()));
	}
	else if(!setHidden && bottomLayoutWidget_->isHidden()){
		periodicTableHeaderButton_->setArrowType(Qt::DownArrow);
		bottomLayoutWidget_->show();
	}
}
