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
	addMultipleSpectra_ = false;

	QFont newFont = font();
	newFont.setPointSize(18);
	newFont.setBold(true);

	title_ = new QLabel;
	title_->setFont(newFont);

	x_.resize(0);
	sourceButtons_ = new QButtonGroup;
	sourceButtons_->setExclusive(false);
	connect(sourceButtons_, SIGNAL(buttonClicked(int)), this, SLOT(onCheckBoxChanged(int)));

	setupPlot();

	/// NEW FUNCTION

	table_ = new AMSelectablePeriodicTable(this);
	table_->buildPeriodicTable();
	connect(table_, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementSelected(AMElement*)));
	connect(table_, SIGNAL(elementDeselected(AMElement*)), this, SLOT(onElementDeselected(AMElement*)));
	tableView_ = new AMSelectablePeriodicTableView(table_);
	tableView_->buildPeriodicTableView();
	connect(tableView_, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementClicked(AMElement*)));

	currentElement_ = table_->elementBySymbol("Fe");
	combinationElement_ = table_->elementBySymbol("Ca");

	////////////////////////////////////////////////////////////

	QPushButton *removeAllEmissionLinesButton = new QPushButton(QIcon(":/trashcan.png"), "Clear Emission Lines");
	removeAllEmissionLinesButton->setMaximumHeight(25);

	rowAbovePeriodicTableLayout_ = new QHBoxLayout;
	rowAbovePeriodicTableLayout_->addWidget(removeAllEmissionLinesButton);
	rowAbovePeriodicTableLayout_->addStretch();

	connect(removeAllEmissionLinesButton, SIGNAL(clicked()), this, SLOT(removeAllEmissionLineMarkers()));

	buildPileUpPeakButtons();

	QVBoxLayout *plotLayout = new QVBoxLayout;
	plotLayout->addWidget(plot_);
	plotLayout->addLayout(rowAbovePeriodicTableLayout_);
	plotLayout->addWidget(tableView_, 0, Qt::AlignCenter);

	emissionLineValidator_ = new AMNameAndRangeValidator(this);
	pileUpPeakValidator_ = new AMNameAndRangeValidator(this);
	combinationPileUpPeakValidator_ = new AMNameAndRangeValidator(this);

	sourceButtonsLayout_ = new QVBoxLayout;
	sourceButtonsLayout_->addWidget(new QLabel("Available Spectra"), 0, Qt::AlignLeft);
	sourceButtonsLayout_->addStretch();

	logEnableButton_ = new QPushButton("Logarithmic");
	logEnableButton_->setCheckable(true);
	connect(logEnableButton_, SIGNAL(toggled(bool)), this, SLOT(onLogScaleEnabled(bool)));

	buildEnergyRangeSpinBoxView();

	connect(emissionLineValidator_, SIGNAL(validatorChanged()), this, SLOT(updateEmissionLineMarkers()));

	QVBoxLayout *sourcesLayout = new QVBoxLayout;
	sourcesLayout->addLayout(sourceButtonsLayout_);
	sourcesLayout->addStretch();
	sourcesLayout->addWidget(new QLabel("Left Axis Scale"));
	sourcesLayout->addWidget(logEnableButton_);
	sourcesLayout->addWidget(new QLabel("Min. Energy"));
	sourcesLayout->addWidget(minimum_);
	sourcesLayout->addWidget(new QLabel("Max. Energy"));
	sourcesLayout->addWidget(maximum_);
	sourcesLayout->addWidget(exportButton_);

	QHBoxLayout *plotAndSourcesLayout = new QHBoxLayout;
	plotAndSourcesLayout->addLayout(plotLayout);
	plotAndSourcesLayout->addLayout(sourcesLayout);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(title_, 0, Qt::AlignLeft);
	layout->addLayout(plotAndSourcesLayout);

	setLayout(layout);
}

void AMSpectrumAndPeriodicTableView::setupPlot()
{
	MPlot *plot = new MPlot;
	plot_ = new MPlotWidget(this);
	plot_->setPlot(plot);

	plot_->plot()->plotArea()->setBrush(QBrush(Qt::white));
	plot_->plot()->axisBottom()->setTicks(5);
	plot_->plot()->axisLeft()->setTicks(5);
	plot_->plot()->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->plot()->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->plot()->axisBottom()->showAxisName(true);
	plot_->plot()->axisLeft()->showAxisName(false);

	// Set the margins for the plot.
	plot_->plot()->setMarginLeft(10);
	plot_->plot()->setMarginBottom(15);
	plot_->plot()->setMarginRight(2);
	plot_->plot()->setMarginTop(2);

	plot_->plot()->addTool(new MPlotDragZoomerTool());
	plot_->plot()->addTool(new MPlotWheelZoomerTool());

	plot_->setMinimumSize(600, 400);
	plot_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

void AMSpectrumAndPeriodicTableView::buildEnergyRangeSpinBoxView()
{
	minimum_ = new QDoubleSpinBox;
	minimum_->setSuffix(" eV");
	minimum_->setDecimals(0);
	minimum_->setRange(0, 1000000);
	connect(minimum_, SIGNAL(editingFinished()), this, SLOT(onMinimumEnergyChanged()));

	maximum_ = new QDoubleSpinBox;
	maximum_->setSuffix(" eV");
	maximum_->setDecimals(0);
	maximum_->setRange(0, 1000000);
	connect(maximum_, SIGNAL(editingFinished()), this, SLOT(onMaximumEnergyChanged()));

	exportButton_ = new QPushButton(QIcon(":/save.png"), "Save to file...");
	exportButton_->setEnabled(false);
	connect(exportButton_, SIGNAL(clicked()), this, SLOT(onExportClicked()));
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

void AMSpectrumAndPeriodicTableView::setAxisInfo(AMAxisInfo info, bool propogateToPlotRange)
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
	AMAxisInfo info = sources_.first()->axisInfoAt(sources_.first()->rank()-1);

	if (info.units.isEmpty())
		plot_->plot()->axisBottom()->setAxisName(info.name);

	else
		plot_->plot()->axisBottom()->setAxisName(info.name % ", " % info.units);

	x_.resize(info.size);

	for (int i = 0; i < info.size; i++)
		x_[i] = double(info.start) + i*double(info.increment);

	setEnergyRange(double(info.start), double(info.start) + info.size*double(info.increment));
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
		plot_->plot()->addItem(newMarker);

		if (firstLine.elementSymbol() == secondLine.elementSymbol())
			pileUpPeakMarkers_ << newMarker;

		else
			combinationPileUpPeakMarkers_ << newMarker;
	}
}






