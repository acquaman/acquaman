#include "AMSpectrumAndPeriodicTableView.h"

#include "dataman/AMScan.h"
#include "ui/dataman/AMColoredTextToolButton.h"
#include "ui/util/AMPeriodicTableDialog.h"

#include "MPlot/MPlotTools.h"

AMSpectrumAndPeriodicTableView::AMSpectrumAndPeriodicTableView(AMDetector *detector, QWidget *parent)
	: QWidget(parent)
{
//	addMultipleSpectra_ = false;

//	QFont newFont = font();
//	newFont.setPointSize(18);
//	newFont.setBold(true);

//	title_ = new QLabel;
//	title_->setFont(newFont);

//	x_.resize(0);
//	sourceButtons_ = new QButtonGroup;
//	sourceButtons_->setExclusive(false);
	//	connect(sourceButtons_, SIGNAL(buttonClicked(int)))
}

void AMSpectrumAndPeriodicTableView::setupPlot()
{

}

void AMSpectrumAndPeriodicTableView::removeAllPlotItems(QList<MPlotItem *> &items)
{
	foreach (MPlotItem *item, items)
		if (plot_->plot()->removeItem(item)){

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
	tableView_->setEnergyRange(newRange);
	minimum_->setValue(newRange.minimum());
	maximum_->setValue(newRange.maximum());
}

void AMSpectrumAndPeriodicTableView::setMinimumEnergy(double newMinimum)
{
	emissionLineValidator_->setMinimum(newMinimum);
	pileUpPeakValidator_->setMinimum(newMinimum);
	combinationPileUpPeakValidator_->setMinimum(newMinimum);
	tableView_->setMinimumEnergy(newMinimum);
}

void AMSpectrumAndPeriodicTableView::setMaximumEnergy(double newMaximum)
{
	emissionLineValidator_->setMaximum(newMaximum);
	pileUpPeakValidator_->setMaximum(newMaximum);
	combinationPileUpPeakValidator_->setMaximum(newMaximum);
	tableView_->setMaximumEnergy(newMaximum);
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
			plot_->plot()->addItem(newLine);
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
			if (plot_->plot()->removeItem(item)){

				emissionLineMarkers_.removeOne(item);
				delete item;
			}
	}

	showPileUpPeaksButton_->setEnabled(table_->hasSelectedElements());
}

void AMSpectrumAndPeriodicTableView::updateEmissionLineMarkers()
{
	foreach (AMElement *element, table_->selectedElements())
		onElementDeselected(element);

	foreach (AMElement *element, table_->selectedElements())
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

	if (showPileUpPeaksButton_->isChecked() && showPileUpPeaksButton_->isEnabled() && table_->isSelected(currentElement_)){

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
		if (plot_->plot()->removeItem(item))
			delete item;

	emissionLineMarkers_.clear();
	table_->deselectAllElements();
	showPileUpPeaksButton_->setEnabled(false);
}

void AMSpectrumAndPeriodicTableView::onMinimumEnergyChanged()
{
	setMinimumEnergy(minimum_->value());
}

void AMSpectrumAndPeriodicTableView::onMaximumEnergyChanged()
{
	setMaximumEnergy(maximum_->value());
}

void AMSpectrumAndPeriodicTableView::onLogScaleEnabled(bool enable)
{
	if (enable){

		plot_->plot()->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(1, MPLOT_POS_INFINITY));
		logEnableButton_->setText("Linear");
	}

	else {

		plot_->plot()->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(MPLOT_NEG_INFINITY, MPLOT_POS_INFINITY));
		logEnableButton_->setText("Logarithmic");
	}

	plot_->plot()->axisScaleLeft()->setLogScaleEnabled(enable);
}

void AMSpectrumAndPeriodicTableView::onAxisInfoChanged()
{

	if (info.units.isEmpty())
		plot_->plot()->axisBottom()->setAxisName(info.name);

	else
		plot_->plot()->axisBottom()->setAxisName(info.name % ", " % info.units);

	x_.resize(info.size);

	for (int i = 0; i < info.size; i++)
		x_[i] = double(info.start) + i*double(info.increment);

	if (propogateToPlotRange)
		setEnergyRange(double(info.start), double(info.start) + info.size*double(info.increment));
}






