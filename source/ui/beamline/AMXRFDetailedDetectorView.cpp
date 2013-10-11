#include "AMXRFDetailedDetectorView.h"

#include <QHBoxLayout>
#include <QStringBuilder>
#include <QComboBox>

#include "util/AMDataSourcePlotSettings.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"
#include "ui/AMSelectionDialog.h"
#include "ui/util/AMPeriodicTableDialog.h"

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

	currentElement_ = periodicTable_->elementBySymbol("Fe");
	combinationElement_ = periodicTable_->elementBySymbol("Ca");
}

void AMXRFDetailedDetectorView::buildDetectorView()
{
	// This is going to extend the base view, so we will simply call it's build method too.
	AMXRFBaseDetectorView::buildDetectorView();

	buildPeriodicTableViewAndElementView();
	buildShowSpectraButtons();

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
}

void AMXRFDetailedDetectorView::buildShowSpectraButtons()
{
	QComboBox *spectraComboBox = new QComboBox;

	for (int i = 0, size = detector_->allSpectrumSources().size(); i < size; i++){

		AMDataSource *source = detector_->allSpectrumSources().at(i);
		spectraComboBox->insertItem(i, source->name());
	}

	QPushButton *showMultipleSpectraButton = new QPushButton("Show Multiple Spectra");

	QHBoxLayout *showSpectraLayout = new QHBoxLayout;
	showSpectraLayout->addStretch();
	showSpectraLayout->addWidget(spectraComboBox);
	showSpectraLayout->addWidget(showMultipleSpectraButton);

	topLayout_->addLayout(showSpectraLayout);

	connect(spectraComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSpectrumComboBoxIndexChanged(int)));
	connect(showMultipleSpectraButton, SIGNAL(clicked()), this, SLOT(onShowMultipleSpectraButtonClicked()));

	spectraComboBox->setCurrentIndex(detector_->allSpectrumSources().size()-1);
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

	bottomLayout_->addLayout(completeBottomLayout);

	connect(emissionLineValidator_, SIGNAL(validatorChanged()), this, SLOT(updateEmissionLineMarkers()));
	connect(periodicTable_, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementSelected(AMElement*)));
	connect(periodicTable_, SIGNAL(elementDeselected(AMElement*)), this, SLOT(onElementDeselected(AMElement*)));
	connect(periodicTable_, SIGNAL(emissionLineSelected(AMEmissionLine)), this, SLOT(onEmissionLineSelected(AMEmissionLine)));
	connect(periodicTable_, SIGNAL(emissionLineDeselected(AMEmissionLine)), this, SLOT(onEmissionLineDeselected(AMEmissionLine)));
	connect(periodicTableView_, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementClicked(AMElement*)));
	connect(removeAllEmissionLinesButton, SIGNAL(clicked()), this, SLOT(removeAllEmissionLineMarkers()));
	connect(removeAllRegionsOfInterestButton, SIGNAL(clicked()), this, SLOT(removeAllRegionsOfInterest()));
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

		if (item->description().contains(QRegExp(symbol % " (K|L|M)")))
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
}

void AMXRFDetailedDetectorView::setMaximumEnergy(double newMaximum)
{
	emissionLineValidator_->setMaximum(newMaximum);
	periodicTableView_->setMaximumEnergy(newMaximum);
	elementView_->setMaximumEnergy(newMaximum);
}

void AMXRFDetailedDetectorView::onEmissionLineSelected(const AMEmissionLine &emissionLine)
{
	detector_->addRegionOfInterest(emissionLine);
	AMRegionOfInterest *newRegion = detector_->regionOfInterest(emissionLine);

	MPlotMarkerTransparentVerticalRectangle *newMarker = new MPlotMarkerTransparentVerticalRectangle(newRegion->name(), newRegion->energy(), newRegion->lowerBound(), newRegion->upperBound());
	plot_->insertItem(newMarker);
	newMarker->setYAxisTarget(plot_->axisScale(MPlot::VerticalRelative));
	regionOfInterestMarkers_.insert(emissionLine, newMarker);

	updatePeriodicTableButtonColors(emissionLine);
}

void AMXRFDetailedDetectorView::onEmissionLineDeselected(const AMEmissionLine &emissionLine)
{
	MPlotItem *itemToBeRemoved = regionOfInterestMarkers_.value(emissionLine);

	if (itemToBeRemoved){

		detector_->removeRegionOfInterest(emissionLine);
		plot_->removeItem(itemToBeRemoved);
		regionOfInterestMarkers_.remove(emissionLine);
		delete itemToBeRemoved;

		updatePeriodicTableButtonColors(emissionLine);
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

			if (source->name() == spectraNames.at(spectraNamesIterator)){

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
	plot_->setAxisScaleWaterfall(MPlot::Left, double(spectraPlotItems_.at(0)->dataRect().bottom())/double(spectraPlotItems_.size()));
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
//	AMElement *el = AMPeriodicTableDialog::getElement();

//	AMPeriodicTableDialog dialog(this);
//	dialog.exec();

//	if (el){

//		combinationChoiceButton_->setText(el->symbol());
//		combinationElement_ = el;
//		updateCombinationPileUpPeaks();
//	}
}
