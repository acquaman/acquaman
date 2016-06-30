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

#include <QStringBuilder>
#include <QComboBox>
#include <QDir>

#include "acquaman/AMXRFScanController.h"
#include "actions3/actions/AMScanAction.h"
#include "dataman/AMAnalysisBlock.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"
#include "dataman/export/AMExportController.h"
#include "dataman/export/AMExporterOptionGeneralAscii.h"
#include "ui/AMHeaderButton.h"
#include "ui/AMSelectionDialog.h"
#include "ui/beamline/AMDeadTimeButton.h"
#include "ui/beamline/AMRegionOfInterestView.h"
#include "ui/dataman/AMChooseScanDialog.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "ui/util/AMDialog.h"
#include "util/AMDataSourcePlotSettings.h"


AMXRFDetailedDetectorView::~AMXRFDetailedDetectorView(){}

AMXRFDetailedDetectorView::AMXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent)
	: AMXRFBaseDetectorView(detector, parent)
{
	chooseScanDialog_ = 0;

	emissionLineLegendColors_.insert("K", QColor(0, 255, 0));
	emissionLineLegendColors_.insert("L", QColor(255, 255, 0));
	emissionLineLegendColors_.insert("M", QColor(255, 0, 0));
	emissionLineLegendColors_.insert("Default", palette().button().color());
	pileUpPeakColor_ = QColor(42, 149, 77);
	combinationPileUpPeakColor_ = QColor(42, 149, 77);

	regionOfInterestMapper_ = new QSignalMapper(this);
	connect(regionOfInterestMapper_, SIGNAL(mapped(QObject*)), this, SLOT(onRegionOfInterestBoundsChanged(QObject*)));

	deadTimeViewFactor_ = int(sqrt(double(detector->elements())));
	// this is to make sure we have more columns than rows, which will looks nicer
	deadTimeViewFactor_ = (detector->elements() / deadTimeViewFactor_ ) > deadTimeViewFactor_ ? deadTimeViewFactor_ + 1 : deadTimeViewFactor_;
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

	energyRangeLayout_->insertWidget(0, editRegionsOfInterestButton_);
}

void AMXRFDetailedDetectorView::buildDeadTimeView()
{
	bool deadTimeEnabled = detector_->hasDeadTimeCorrection();
	bool hasICRControls = !detector_->inputCountSources().isEmpty();

	deadTimeLabel_ = new QLabel("Dead Time: 0%");
	connect(detector_, SIGNAL(deadTimeChanged()), this, SLOT(onDeadTimeChanged()));

	deadTimeButtons_ = new QButtonGroup(this);
	deadTimeButtons_->setExclusive(false);
	QGridLayout *deadTimeButtonLayout = new QGridLayout;

	if (deadTimeEnabled){
		for (int i = 0, elements = detector_->elements(); i < elements; i++){
			AMDeadTimeButton *deadTimeButton = new AMDeadTimeButton(detector_->inputCountSourceAt(i), detector_->outputCountSourceAt(i), 30.0, 50.0);
			deadTimeButton->setCheckable(true);
			deadTimeButtonLayout->addWidget(deadTimeButton, int(i/deadTimeViewFactor_), i%deadTimeViewFactor_);
			deadTimeButtons_->addButton(deadTimeButton, i);
		}
	}
	else if (hasICRControls) {
		for (int i = 0, elements = detector_->elements(); i < elements; i++){
			AMDeadTimeButton *deadTimeButton = new AMDeadTimeButton(detector_->inputCountSourceAt(i), 0, 300000, 1000000, AMDeadTimeButton::CountRate);
			deadTimeButton->setCheckable(true);
			deadTimeButton->setAcquireTimeControl(detector_->acquireTimeControl());
			deadTimeButton->setChecked(detector_->isElementDisabled(i)); // Elements are disabled by checking the corresponding toolbutton.
			deadTimeButton->setEnabled(detector_->canEnableElement(i)); // Elements that are not enabled initially will always be disabled (ie. permanently disabled elements).
			deadTimeButton->setIndex(i+1);

			if (!detector_->canEnableElement(i))
				deadTimeButton->setCountsMode(AMDeadTimeButton::None);

			deadTimeButtonLayout->addWidget(deadTimeButton, int(i/deadTimeViewFactor_), i%deadTimeViewFactor_);
			deadTimeButtons_->addButton(deadTimeButton, i);
		}
	}
	else {
		for (int i = 0, elements = detector_->elements(); i < elements; i++){
			AMDeadTimeButton *deadTimeButton = new AMDeadTimeButton;
			deadTimeButton->setCheckable(true);
			deadTimeButtonLayout->addWidget(deadTimeButton, int(i/deadTimeViewFactor_), i%deadTimeViewFactor_);
			deadTimeButtons_->addButton(deadTimeButton, i);
		}
	}

	if (deadTimeButtons_->buttons().size() > 1) {
		connect(deadTimeButtons_, SIGNAL(buttonClicked(int)), this, SLOT(onDeadTimeButtonClicked(int)));
	}
	else {
		deadTimeButtons_->button(0)->setCheckable(false);
	}

	connect(detector_, SIGNAL(elementEnabled(int)), this, SLOT(onElementEnabledOrDisabled(int)));
	connect(detector_, SIGNAL(elementDisabled(int)), this, SLOT(onElementEnabledOrDisabled(int)));

	QHBoxLayout *squeezedDeadTimeButtonsLayout = new QHBoxLayout;
	squeezedDeadTimeButtonsLayout->addStretch();
	squeezedDeadTimeButtonsLayout->addLayout(deadTimeButtonLayout);
	squeezedDeadTimeButtonsLayout->addStretch();

	QVBoxLayout *deadTimeLayout = new QVBoxLayout;
	deadTimeLayout->addWidget(deadTimeLabel_, 0, Qt::AlignLeft);
	deadTimeLayout->addLayout(squeezedDeadTimeButtonsLayout);

	// Remove the "addStretch" from the basic layout before adding the new elements.
	rightLayout_->removeItem(rightLayout_->itemAt(rightLayout_->count()-1));
	rightLayout_->addLayout(deadTimeLayout);
	rightLayout_->addStretch();

	deadTimeLabel_->setVisible(deadTimeEnabled);
}

void AMXRFDetailedDetectorView::buildEnergyRangeSpinBoxView()
{
	AMSpectrumAndPeriodicTableView::buildEnergyRangeSpinBoxView();

	showEnergyRangeSpinBoxes_ = new QPushButton(QIcon(":/system-run.png"), "Settings");
	showEnergyRangeSpinBoxes_->setCheckable(true);

	minimum_->setAlignment(Qt::AlignCenter);
	minimum_->hide();

	maximum_->setAlignment(Qt::AlignCenter);
	maximum_->hide();

	connect(showEnergyRangeSpinBoxes_, SIGNAL(toggled(bool)), minimum_, SLOT(setVisible(bool)));
	connect(showEnergyRangeSpinBoxes_, SIGNAL(toggled(bool)), maximum_, SLOT(setVisible(bool)));

	energyRangeLayout_->addWidget(showEnergyRangeSpinBoxes_);
	energyRangeLayout_->addWidget(minimum_);
	energyRangeLayout_->addWidget(maximum_);
	energyRangeLayout_->addWidget(exportButton_);

	rightLayout_->addLayout(energyRangeLayout_);
}

void AMXRFDetailedDetectorView::buildShowSpectraButtons()
{
	AMSpectrumAndPeriodicTableView::buildShowSpectraButtons();

	spectraComboBox_ = new QComboBox;

	for (int i = 0, size = detector_->allSpectrumSources().size(); i < size; i++){
		AMDataSource *source = detector_->allSpectrumSources().at(i);
		spectraComboBox_->insertItem(i, source->name());
	}

	QPushButton *showMultipleSpectraButton = new QPushButton("Show Multiple Spectra");
	showWaterfall_ = new QCheckBox("Waterfall");
	showWaterfall_->setChecked(true);

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
	connect(logScaleButton_, SIGNAL(toggled(bool)), showWaterfall_, SLOT(setDisabled(bool)));

	spectraComboBox_->setCurrentIndex(detector_->allSpectrumSources().size()-1);
}

void AMXRFDetailedDetectorView::buildPeriodicTableViewAndElementView()
{

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

	QPushButton *removeAllRegionsOfInterestButton = new QPushButton(QIcon(":/trashcan.png"), "Clear Regions Of Interest");
	removeAllRegionsOfInterestButton->setMaximumHeight(25);

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
	connect(periodicTable_, SIGNAL(emissionLineSelected(AMEmissionLine)), this, SLOT(onEmissionLineSelected(AMEmissionLine)));
	connect(periodicTable_, SIGNAL(emissionLineDeselected(AMEmissionLine)), this, SLOT(onEmissionLineDeselected(AMEmissionLine)));
	connect(removeAllRegionsOfInterestButton, SIGNAL(clicked()), this, SLOT(removeAllRegionsOfInterest()));
	connect(detector_, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
	connect(detector_, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));

	connect(periodicTableHeaderButton_, SIGNAL(clicked()), this, SLOT(onPeriodicTableHeaderButtonClicked()));
}

void AMXRFDetailedDetectorView::onElementClicked(AMElement *element)
{
	// This is always safe because we know the periodic table is an AMSelectablePeriodicTable.
	elementView_->setElement(qobject_cast<AMSelectableElement *>(element));

	highlightCurrentElementRegionOfInterest();

	AMSpectrumAndPeriodicTableView::onElementClicked(element);
}

void AMXRFDetailedDetectorView::setEnergyRange(const AMRange &newRange)
{
	AMSpectrumAndPeriodicTableView::setEnergyRange(newRange);
	elementView_->setEnergyRange(newRange);
}

void AMXRFDetailedDetectorView::setEnergyRange(double minimum, double maximum)
{
	setEnergyRange(AMRange(minimum, maximum));
}

void AMXRFDetailedDetectorView::setMinimumEnergy(double newMinimum)
{
	AMSpectrumAndPeriodicTableView::setMinimumEnergy(newMinimum);

	elementView_->setMinimumEnergy(newMinimum);
	setMinimumEnergyImplementation(newMinimum);
}

void AMXRFDetailedDetectorView::setMaximumEnergy(double newMaximum)
{
	AMSpectrumAndPeriodicTableView::setMaximumEnergy(newMaximum);

	elementView_->setMaximumEnergy(newMaximum);
	setMaximumEnergyImplementation(newMaximum);
}

void AMXRFDetailedDetectorView::expandPeriodicTableViews(){
	hidePeriodicTableViews(false);
}

void AMXRFDetailedDetectorView::collapsePeriodTableViews(){
	hidePeriodicTableViews(true);
}

void AMXRFDetailedDetectorView::startAcquisition()
{
	AMXRFScanConfiguration *configuration = new AMXRFScanConfiguration;

	AMDetectorInfoSet detectorSet;
	detectorSet.addDetectorInfo(detector_->toInfo());
	configuration->setDetectorConfigurations(detectorSet);

	AMScanAction *scanAction = new AMScanAction(new AMScanActionInfo(configuration));

	connect(scanAction, SIGNAL(cancelled()), scanAction, SLOT(scheduleForDeletion()));
	connect(scanAction, SIGNAL(failed()), scanAction, SLOT(scheduleForDeletion()));
	connect(scanAction, SIGNAL(succeeded()), scanAction, SLOT(scheduleForDeletion()));

	scanAction->start();
}

void AMXRFDetailedDetectorView::onSaveButtonClicked()
{
	if(!chooseScanDialog_) {
		chooseScanDialog_ = new AMChooseScanDialog(AMDatabase::database("user"), "Choose XRF Spectrum...", "Choose the XRF Spectrum you want to export.", this);
		chooseScanDialog_->setAttribute(Qt::WA_DeleteOnClose, false);
		connect(chooseScanDialog_, SIGNAL(accepted()), this, SLOT(exportScan()));
	}

	chooseScanDialog_->setFilterKeyColumn(1);
	chooseScanDialog_->setFilterRegExp(QString("XRF Scan - %1").arg(detector_->name()));
	chooseScanDialog_->show();
}

void AMXRFDetailedDetectorView::exportScan()
{
	QList<AMScan *> scans;

	foreach (QUrl scanUrl, chooseScanDialog_->getSelectedScans()){

		AMScan *scan = AMScan::createFromDatabaseUrl(scanUrl, false);

		QString scanName = AMLineEditDialog::retrieveAnswer("Choose a scan name...",
									"Please choose the name you wish to name the scan.",
									scan->name());

		if (!scanName.isEmpty()){
			scan->setName(scanName);
			scan->storeToDb(AMDatabase::database("user"));
			scans << scan;
		}
	}

	if (!scans.isEmpty()){
		exportController_ = new AMExportController(scans);
		connect(exportController_, SIGNAL(stateChanged(int)), this, SLOT(onExportControllerStateChanged(int)));

		exportController_->setDefaultDestinationFolderPath();
		exportController_->chooseExporter("AMExporterGeneralAscii");
		exportController_->setOption(buildExporterOption());
		exportController_->option()->setFileName("$name_$number.dat");
		exportController_->start(true);
	}
}

void AMXRFDetailedDetectorView::onExportControllerStateChanged(int state)
{
	if (state == AMExportController::Finished){
		exportController_->disconnect();
		exportController_->deleteLater();
	}
}

void AMXRFDetailedDetectorView::onEmissionLineSelected(const AMEmissionLine &emissionLine)
{
	if (!detector_->regionOfInterest(emissionLine))
		detector_->addRegionOfInterest(emissionLine);

	AMRegionOfInterest *newRegion = detector_->regionOfInterest(emissionLine);
	regionOfInterestMapper_->setMapping(newRegion, newRegion);
	connect(newRegion, SIGNAL(boundingRangeChanged(AMRange)), regionOfInterestMapper_, SLOT(map()));

	MPlotMarkerTransparentVerticalRectangle *newMarker = new MPlotMarkerTransparentVerticalRectangle(newRegion->name(), newRegion->energy(), newRegion->lowerBound(), newRegion->upperBound());
	plot_->insertItem(newMarker);
	newMarker->setYAxisTarget(plot_->axisScale(MPlot::VerticalRelative));
	regionOfInterestMarkers_.insert(newRegion, newMarker);

	updatePeriodicTableButtonColors(emissionLine.elementSymbol());
	editRegionsOfInterestButton_->setEnabled(detector_->regionsOfInterestCount() > 0);
}

void AMXRFDetailedDetectorView::onEmissionLineDeselected(const AMEmissionLine &emissionLine)
{
	AMRegionOfInterest *region = detector_->regionOfInterest(emissionLine);
	removeRegionOfInterestItems(region);
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
		removeRegionOfInterestItems(region);
		elementView_->updateEmissionLineViewList();
	}
}

void AMXRFDetailedDetectorView::removeAllRegionsOfInterest()
{
	foreach (AMRegionOfInterest *region, detector_->regionsOfInterest())
		removeRegionOfInterestItems(region);

	elementView_->setElement(elementView_->element());
}

void AMXRFDetailedDetectorView::updatePeriodicTableButtonColors(const QString &symbol)
{
	AMSelectableElement *element = qobject_cast<AMSelectableElement *>(periodicTable_->elementBySymbol(symbol));

	if (element){

		if (element->hasSelectedEmissionLines()){

			QStringList keyString;

			foreach (AMEmissionLine line, element->selectedEmissionLines())
				keyString << line.lineName().left(1);

			keyString.removeDuplicates();
			periodicTableView_->button(periodicTable_->elementBySymbol(symbol))->setStyleSheet(buildStyleSheet(keyString.join("")));
		}

		else{
			periodicTableView_->button(periodicTable_->elementBySymbol(symbol))->setStyleSheet(buildStyleSheet("Default"));
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

void AMXRFDetailedDetectorView::onDeadTimeChanged()
{
		deadTimeLabel_->setText(QString("Dead Time:\t%1%").arg(detector_->deadTime()*100, 0, 'f', 0));
}

void AMXRFDetailedDetectorView::onDeadTimeButtonClicked(int deadTimeButtonId)
{
	if (detector_->isElementEnabled(deadTimeButtonId))
		detector_->disableElement(deadTimeButtonId);
	else
		detector_->enableElement(deadTimeButtonId);
}

void AMXRFDetailedDetectorView::onElementEnabledOrDisabled(int elementId)
{
	deadTimeButtons_->blockSignals(true);
	deadTimeButtons_->button(elementId)->setChecked(!detector_->isElementEnabled(elementId));
	deadTimeButtons_->blockSignals(false);
}

void AMXRFDetailedDetectorView::onRegionOfInterestBoundsChanged(QObject *id)
{
	AMRegionOfInterest *region = qobject_cast<AMRegionOfInterest *>(id);
	regionOfInterestMarkers_.value(region)->setLowEnd(region->lowerBound());
	regionOfInterestMarkers_.value(region)->setHighEnd(region->upperBound());
}

void AMXRFDetailedDetectorView::onLogScaleEnabled(bool logScale)
{
	AMSpectrumAndPeriodicTableView::onLogScaleEnabled(logScale);

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

AMExporterOption *AMXRFDetailedDetectorView::buildExporterOption() const
{
	QList<int> matchIDs = AMDatabase::database("user")->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMExporterOptionGeneralAscii>(), "name", "XRFDefault");

	AMExporterOptionGeneralAscii *option = new AMExporterOptionGeneralAscii;

	if (matchIDs.count() != 0)
		option->loadFromDb(AMDatabase::database("user"), matchIDs.at(0));

	option->setName("XRFDefault");
	option->setFileName("$name_$number.dat");
	option->setHeaderText("Scan: $name #$number\nDate: $dateTime\nSample: $sample\nFacility: $facilityDescription\n\n$notes\n\n");
	option->setHeaderIncluded(true);
	option->setColumnHeader("$dataSetName $dataSetInfoDescription");
	option->setColumnHeaderIncluded(true);
	option->setColumnHeaderDelimiter("");
	option->setSectionHeader("");
	option->setSectionHeaderIncluded(true);
	option->setIncludeAllDataSources(true);
	option->setFirstColumnOnly(true);
	option->setIncludeHigherDimensionSources(true);
	option->setSeparateHigherDimensionalSources(true);
	option->setSeparateSectionFileName("$name_$dataSetName_$number.dat");
	option->storeToDb(AMDatabase::database("user"));

	return option;
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

void AMXRFDetailedDetectorView::removeRegionOfInterestItems(AMRegionOfInterest *region)
{
	MPlotItem *itemToBeRemoved = regionOfInterestMarkers_.value(region);

	if (itemToBeRemoved){
		QString regionName = region->name();
		regionOfInterestMapper_->removeMappings(region);
		plot_->removeItem(itemToBeRemoved);
		regionOfInterestMarkers_.remove(region);
		delete itemToBeRemoved;
		detector_->removeRegionOfInterest(region);

		updatePeriodicTableButtonColors(regionName.split(" ").first());
		editRegionsOfInterestButton_->setEnabled(detector_->regionsOfInterestCount() > 0);
	}
}
