#include "AMSpectrumAndPeriodicTableView.h"

#include "dataman/AMScan.h"
#include "ui/dataman/AMColoredTextToolButton.h"
#include "ui/util/AMPeriodicTableDialog.h"

#include "MPlot/MPlotTools.h"

#include <QStringBuilder>

 AMSpectrumAndPeriodicTableView::~AMSpectrumAndPeriodicTableView(){}
AMSpectrumAndPeriodicTableView::AMSpectrumAndPeriodicTableView(QWidget *parent)
	: QWidget(parent)
{
	emissionLineValidator_ = new AMNameAndRangeValidator(this);
	pileUpPeakValidator_ = new AMNameAndRangeValidator(this);
	combinationPileUpPeakValidator_ = new AMNameAndRangeValidator(this);

	periodicTable_ = new AMSelectablePeriodicTable(this);
	periodicTable_->buildPeriodicTable();

	periodicTableView_ = new AMSelectablePeriodicTableView(periodicTable_);
	periodicTableView_->buildPeriodicTableView();

	rowAbovePeriodicTableLayout_ = new QHBoxLayout;
}

void AMSpectrumAndPeriodicTableView::setupPlot()
{
	plot_ = new MPlot;
	plotView_ = new MPlotWidget(this);
	plotView_->setPlot(plot_);

	plotView_->plot()->plotArea()->setBrush(QBrush(Qt::white));
	plotView_->plot()->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
	plotView_->plot()->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	plotView_->plot()->axisBottom()->showAxisName(true);
	plotView_->plot()->axisLeft()->showAxisName(false);

	plotView_->plot()->addTool(new MPlotDragZoomerTool());
	plotView_->plot()->addTool(new MPlotWheelZoomerTool());

	plotView_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(0, MPLOT_POS_INFINITY));

	currentElement_ = periodicTable_->elementBySymbol("Fe");
	combinationElement_ = periodicTable_->elementBySymbol("Ca");

	QPushButton *removeAllEmissionLinesButton = new QPushButton(QIcon(":/trashcan.png"), "Clear Emission Lines");
	removeAllEmissionLinesButton->setMaximumHeight(25);
	rowAbovePeriodicTableLayout_->addWidget(removeAllEmissionLinesButton);
	connect(removeAllEmissionLinesButton, SIGNAL(clicked()), this, SLOT(removeAllEmissionLineMarkers()));
}

void AMSpectrumAndPeriodicTableView::buildEnergyRangeSpinBoxView()
{
	minimumEnergySpinBox_ = new QDoubleSpinBox;
	minimumEnergySpinBox_->setSuffix(" eV");
	minimumEnergySpinBox_->setDecimals(0);
	minimumEnergySpinBox_->setRange(0, 1000000);
	connect(minimumEnergySpinBox_, SIGNAL(editingFinished()), this, SLOT(onMinimumEnergyChanged()));

	maximumEnergySpinBox_ = new QDoubleSpinBox;
	maximumEnergySpinBox_->setSuffix(" eV");
	maximumEnergySpinBox_->setDecimals(0);
	maximumEnergySpinBox_->setRange(0, 1000000);
	connect(maximumEnergySpinBox_, SIGNAL(editingFinished()), this, SLOT(onMaximumEnergyChanged()));

	exportButton_ = new QPushButton(QIcon(":/save.png"), "Save to file...");
	exportButton_->setEnabled(false);
	connect(exportButton_, SIGNAL(clicked()), this, SLOT(onExportButtonClicked()));

	energyRangeLayout_ = new QVBoxLayout;

}

void AMSpectrumAndPeriodicTableView::buildPileUpPeakButtons()
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

void AMSpectrumAndPeriodicTableView::buildShowSpectraButtons()
{
	logScaleButton_ = new QPushButton("Log scale");
	logScaleButton_->setCheckable(true);
}

void AMSpectrumAndPeriodicTableView::removeAllPlotItems(QList<MPlotItem *> &items)
{
	foreach (MPlotItem *item, items)
		if (plotView_->plot()->removeItem(item)){
			item->signalSource()->disconnect();
			delete item;
		}

	items.clear();
}

void AMSpectrumAndPeriodicTableView::setEnergyRange(double low, double high)
{
	setEnergyRange(AMRange(low, high));
}

void AMSpectrumAndPeriodicTableView::setEnergyRange(const AMRange &newRange)
{
	emissionLineValidator_->setRange(newRange);
	pileUpPeakValidator_->setRange(newRange);
	combinationPileUpPeakValidator_->setRange(newRange);
	periodicTableView_->setEnergyRange(newRange);
	minimumEnergySpinBox_->setValue(newRange.minimum());
	maximumEnergySpinBox_->setValue(newRange.maximum());
}

void AMSpectrumAndPeriodicTableView::setMinimumEnergy(double newMinimum)
{
	emissionLineValidator_->setMinimum(newMinimum);
	pileUpPeakValidator_->setMinimum(newMinimum);
	combinationPileUpPeakValidator_->setMinimum(newMinimum);
	periodicTableView_->setMinimumEnergy(newMinimum);
}

void AMSpectrumAndPeriodicTableView::setMaximumEnergy(double newMaximum)
{
	emissionLineValidator_->setMaximum(newMaximum);
	pileUpPeakValidator_->setMaximum(newMaximum);
	combinationPileUpPeakValidator_->setMaximum(newMaximum);
	periodicTableView_->setMaximumEnergy(newMaximum);
}

void AMSpectrumAndPeriodicTableView::setAxisInfo(AMAxisInfo info, bool propogateToPlotRange)
{
	if (info.units.isEmpty())
		plotView_->plot()->axisBottom()->setAxisName(info.name);
	else
		plotView_->plot()->axisBottom()->setAxisName(info.name % ", " % info.units);

	x_ = QVector<double>(info.size);

	for (int i = 0; i < info.size; i++)
		x_[i] = double(info.start) + i*double(info.increment);

	if (propogateToPlotRange)
		setEnergyRange(double(info.start), double(info.start) + info.size*double(info.increment));
}

void AMSpectrumAndPeriodicTableView::addEmissionLineNameFilter(const QRegExp &newNameFilter)
{
	emissionLineValidator_->addNameFilter(newNameFilter);
}

bool AMSpectrumAndPeriodicTableView::removeEmissionLineNameFilter(int index)
{
	return emissionLineValidator_->removeNameFilter(index);
}

bool AMSpectrumAndPeriodicTableView::removeEmissionLineNameFilter(const QRegExp &filter)
{
	return emissionLineValidator_->removeNameFilter(filter);
}

void AMSpectrumAndPeriodicTableView::addPileUpPeakNameFilter(const QRegExp &newNameFilter)
{
	pileUpPeakValidator_->addNameFilter(newNameFilter);
}

bool AMSpectrumAndPeriodicTableView::removePileUpPeakNameFilter(int index)
{
	return pileUpPeakValidator_->removeNameFilter(index);
}

bool AMSpectrumAndPeriodicTableView::removePileUpPeakNameFilter(const QRegExp &filter)
{
	return pileUpPeakValidator_->removeNameFilter(filter);
}

void AMSpectrumAndPeriodicTableView::addCombinationPileUpPeakNameFilter(const QRegExp &newNameFilter)
{
	combinationPileUpPeakValidator_->addNameFilter(newNameFilter);
}

bool AMSpectrumAndPeriodicTableView::removeCombinationPileUpPeakNameFilter(int index)
{
	return combinationPileUpPeakValidator_->removeNameFilter(index);
}

bool AMSpectrumAndPeriodicTableView::removeCombinationPileUpPeakNameFilter(const QRegExp &filter)
{
	return combinationPileUpPeakValidator_->removeNameFilter(filter);
}

void AMSpectrumAndPeriodicTableView::onElementClicked(AMElement *element)
{
	currentElement_ = element;
	updatePileUpPeaksButtonText();
	updatePileUpPeaks();
}

void AMSpectrumAndPeriodicTableView::onElementSelected(AMElement *element)
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

void AMSpectrumAndPeriodicTableView::onElementDeselected(AMElement *element)
{
	QString symbol = element->symbol();

	foreach(MPlotItem *item, emissionLineMarkers_){
		if (item->description().contains(QRegExp(QString("^%1 (K|L|M)").arg(symbol))))
			if (plot_->removeItem(item)){
				emissionLineMarkers_.removeOne(item);
			}
	}

	showPileUpPeaksButton_->setEnabled(periodicTable_->hasSelectedElements());
}

void AMSpectrumAndPeriodicTableView::updateEmissionLineMarkers()
{
	foreach (AMElement *element, periodicTable_->selectedElements())
		onElementDeselected(element);

	foreach (AMElement *element, periodicTable_->selectedElements())
		onElementSelected(element);
}

void AMSpectrumAndPeriodicTableView::updatePileUpPeaksButtonText()
{
	showPileUpPeaksButton_->setText(QString("%1 %2 Pile Up Peaks").arg(showPileUpPeaksButton_->isChecked() ? "Hide" : "Show").arg(currentElement_->symbol()));
}

void AMSpectrumAndPeriodicTableView::updateCombinationPileUpPeaksButtonText()
{
	showCombinationPileUpPeaksButton_->setText(QString("%1 Combination Peaks").arg(showPileUpPeaksButton_->isChecked() ? "Hide" : "Show"));
}

void AMSpectrumAndPeriodicTableView::updatePileUpPeaks()
{
	removeAllPlotItems(pileUpPeakMarkers_);

	if (showPileUpPeaksButton_->isChecked() && showPileUpPeaksButton_->isEnabled() && periodicTable_->isSelected(currentElement_)){
		for (int i = 0, size = currentElement_->emissionLines().size(); i < size; i++)
			for (int j = i; j < size; j++)
				addPileUpMarker(currentElement_->emissionLines().at(i), currentElement_->emissionLines().at(j));
	}
	updateCombinationPileUpPeaks();
}

void AMSpectrumAndPeriodicTableView::updateCombinationPileUpPeaks()
{
	removeAllPlotItems(combinationPileUpPeakMarkers_);

	if (showCombinationPileUpPeaksButton_->isChecked() && showCombinationPileUpPeaksButton_->isEnabled()){
		for (int i = 0, iSize = currentElement_->emissionLines().size(); i < iSize; i++)
			for (int j = 0, jSize = combinationElement_->emissionLines().size(); j < jSize; j++)
				addPileUpMarker(currentElement_->emissionLines().at(i), combinationElement_->emissionLines().at(j));
	}
}

void AMSpectrumAndPeriodicTableView::onCombinationChoiceButtonClicked()
{
	AMElement *el = AMPeriodicTableDialog::getElement();

	if (el){
		combinationChoiceButton_->setText(el->symbol());
		combinationElement_ = el;
		updateCombinationPileUpPeaks();
	}
}

void AMSpectrumAndPeriodicTableView::removeAllEmissionLineMarkers()
{
	foreach (MPlotItem *item, emissionLineMarkers_)
		if (plotView_->plot()->removeItem(item))
			delete item;

	emissionLineMarkers_.clear();
	periodicTable_->deselectAllElements();
	showPileUpPeaksButton_->setEnabled(false);
}

void AMSpectrumAndPeriodicTableView::onMinimumEnergyChanged()
{
	setMinimumEnergy(minimumEnergySpinBox_->value());
}

void AMSpectrumAndPeriodicTableView::onMaximumEnergyChanged()
{
	setMaximumEnergy(maximumEnergySpinBox_->value());
}

void AMSpectrumAndPeriodicTableView::onLogScaleEnabled(bool enable)
{
	if (enable){
		plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(1, MPLOT_POS_INFINITY));
		logScaleButton_->setText("Linear Scale");
	}
	else {
		plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(0, MPLOT_POS_INFINITY));
		logScaleButton_->setText("Logarithmic Scale");
	}

	plot_->axisScaleLeft()->setLogScaleEnabled(enable);
}


void AMSpectrumAndPeriodicTableView::addPileUpMarker(const AMEmissionLine &firstLine, const AMEmissionLine &secondLine)
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
	else {
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






