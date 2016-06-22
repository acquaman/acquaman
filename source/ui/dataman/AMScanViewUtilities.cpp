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


#include "AMScanViewUtilities.h"

#include "dataman/AMScan.h"
#include "ui/dataman/AMColoredTextToolButton.h"
#include "ui/util/AMPeriodicTableDialog.h"

#include <QStringBuilder>

AMScanViewScanBar::AMScanViewScanBar(AMScanSetModel* model, int scanIndex, QWidget* parent)
	: QFrame(parent)
{
	model_ = model;
	scanIndex_ = scanIndex;

	setObjectName("AMScanViewScanBar");
	setStyleSheet("QFrame#AMScanViewScanBar { "
				  "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
				  "border-bottom: 1px solid black;"
				  "}");


	exclusiveModeOn_ = false;
	sourceButtons_.setExclusive(false);

	AMScan* source = model->scanAt(scanIndex_);


	// UI Setup:
	///////////////////////

	QHBoxLayout* hl = new QHBoxLayout();
	nameLabel_ = new QLabel();
	if(source)
		nameLabel_->setText(source->name() + QString(" #%1").arg(source->number()));
	nameLabel_->setStyleSheet("color: white;");
	hl->addWidget(nameLabel_);
	hl->addStretch(0);

	cramBar_ = new AMCramBarHorizontal();


	if(source) {

		for(int i=0; i<source->dataSourceCount(); i++) {

			QColor color = model->plotColor(scanIndex, i);
			QToolButton* sourceButton = new AMColoredTextToolButton(color); /// \todo special buttons, with lines underneath that show the line color / style, and differentiate 1D, 2D datasets.

			sourceButton->setMaximumHeight(18);
			sourceButton->setText(source->dataSourceAt(i)->name());	/// \todo description or name? both? name if description is empty?
			sourceButton->setCheckable(true);
			sourceButton->setChecked(model->isVisible(scanIndex, i));
			sourceButtons_.addButton(sourceButton, i);
			cramBar_->addWidget(sourceButton);
			// hide the button if this data source should be hidden from users:
			sourceButton->setHidden(model->isHiddenFromUsers(scanIndex, i));

			sourceButton->setContextMenuPolicy(Qt::CustomContextMenu);
			connect(sourceButton, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onDataSourceButtonRightClicked(QPoint)));
		}
	}

	hl->addWidget(cramBar_);
	hl->addStretch(1);

	hl->setContentsMargins(6, 0, 6, 0);
	hl->setSpacing(24);
	setLayout(hl);

	connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowInserted(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowAboutToBeRemoved(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowRemoved(QModelIndex,int,int)));
	connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
	connect(model, SIGNAL(exclusiveDataSourceChanged(QString)), this, SLOT(onExclusiveDataSourceChanged(QString)));

	connect(&sourceButtons_, SIGNAL(buttonClicked(int)), this, SLOT(onSourceButtonClicked(int)));
}


void AMScanViewScanBar::onDataSourceButtonRightClicked(const QPoint& location) {

	QAbstractButton* button = qobject_cast<QAbstractButton*>(sender());
	if(!button)
		return;

	int dataSourceIndex = sourceButtons_.id(button);

	AMScanViewScanBarContextMenu* cm = new AMScanViewScanBarContextMenu(model_, scanIndex_, dataSourceIndex, button);
	cm->popup(button->mapToGlobal(location));
}


void AMScanViewScanBar::onRowInserted(const QModelIndex& parent, int start, int end) {
	// not for us...
	if(parent.internalId() != -1 || parent.row() != scanIndex_) {
		return;
	}


	// it is for us... (parent index is our Scan, and it is a new data source)
	AMScan* source = model_->scanAt(scanIndex_);
	// note: AMScanSetModel guarantees only one row inserted at a time, but we don't depend on that...
	for(int i=start; i<=end; i++) {
		AMColoredTextToolButton *newButton = new AMColoredTextToolButton(model_->plotColor(scanIndex_, i));
		newButton->setText(source->dataSourceAt(i)->name());
		newButton->setCheckable(true);
		newButton->setMaximumHeight(18);
		sourceButtons_.addButton(newButton, i);
		cramBar_->insertWidget(i, newButton);
		if(exclusiveModeOn_)
			newButton->setChecked( (model_->exclusiveDataSourceName() == source->dataSourceAt(i)->name()) );
		else
			newButton->setChecked(model_->isVisible(scanIndex_, i));
		// If this data source should be hidden from users, don't show the button to toggle its visibility.  (Above, isVisible() will automatically return false if hiddenFromUsers() is true.)
		if (source->dataSourceAt(i)->hiddenFromUsers())
			newButton->hide();

		newButton->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(newButton, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onDataSourceButtonRightClicked(QPoint)));
	}

}

// before a scan or data source is deleted in the model.
void AMScanViewScanBar::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// check if this isn't one of our data sources being deleted.
	if(parent.internalId() != -1 || parent.row() != scanIndex_) {
		return;
	}


	// (AMScanSetModel guarantees only one removed at once -- ie: start == end --, but we don't depend on that)
	for(int di = end; di>=start; di-- ) {

		sourceButtons_.button(di)->disconnect();
		sourceButtons_.button(di)->deleteLater();
		// the button group's id's from "start+1" to "count+1" are too high now...
		for(int i=di+1; i<sourceButtons_.buttons().count()+1; i++)
			sourceButtons_.setId(sourceButtons_.button(i), i-1);
	}
}

// after a scan or data source is deleted in the model:
void AMScanViewScanBar::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)
}

// when model data changes.  Possibilities we care about: scan name, and data sources visible/not visible.
/*! This assumes that topLeft == bottomRight, which is ok, given that AMScanSetModel always emits dataChanged() for single items. */
void AMScanViewScanBar::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	Q_UNUSED(bottomRight)

	// changes to our scan:
	if(topLeft.internalId() == -1 && topLeft.row() == scanIndex_) {

		nameLabel_->setText(model_->scanAt(scanIndex_)->name() + QString(" #%1").arg(model_->scanAt(scanIndex_)->number()));
	}

	// changes to one of our data sources:
	else if(topLeft.internalId() == scanIndex_) {

		int dataSourceIndex = topLeft.row();
		AMDataSource* dataSource = model_->dataSourceAt(scanIndex_, dataSourceIndex);
		QAbstractButton* button = sourceButtons_.button(dataSourceIndex);
		button->setText(dataSource->name());
		// setting visibility: depends on whether exclusiveMode is on or not
		if(exclusiveModeOn_)
			button->setChecked( (model_->exclusiveDataSourceName() == dataSource->name()) );
		else
			button->setChecked(model_->isVisible(scanIndex_, dataSourceIndex));

		// hide the button to toggle visibility, if this data source should be hidden from users.
		button->setHidden(model_->isHiddenFromUsers(scanIndex_, dataSourceIndex));

		qobject_cast<AMColoredTextToolButton*>(button)->setTextColor( model_->plotColor(scanIndex_, dataSourceIndex) );
	}
}

void AMScanViewScanBar::onSourceButtonClicked(int id) {
	// if in "exclusive" mode, when clicked, set that data source as the exclusive data source
	if(exclusiveModeOn_) {
		//if(visible == Qt::Checked)
		model_->setExclusiveDataSourceByName(model_->dataSourceAt(scanIndex_, id)->name());
		sourceButtons_.button(id)->setChecked(true);
	}
	else {
		model_->setVisible(scanIndex_, id, sourceButtons_.button(id)->isChecked());
	}
}


void AMScanViewScanBar::onExclusiveDataSourceChanged(const QString& exlusiveDataSourceName) {
	// when exclusive mode is on, set checked only when data source name matches the exclusive one
	if(exclusiveModeOn_) {
		int numSourceButtons = sourceButtons_.buttons().count();
		for(int di=0; di<numSourceButtons; di++)
			sourceButtons_.button(di)->setChecked( (model_->dataSourceAt(scanIndex_, di)->name() == exlusiveDataSourceName) );
	}
}


void AMScanViewScanBar::setExclusiveModeOn(bool exclusiveModeOn) {

	// turning exclusiveMode on:
	if(exclusiveModeOn && !exclusiveModeOn_) {
		exclusiveModeOn_ = true;
		int numSourceButtons = sourceButtons_.buttons().count();
		for(int di=0; di<numSourceButtons; di++) {
			sourceButtons_.button(di)->setChecked( (model_->dataSourceAt(scanIndex_, di)->name() == model_->exclusiveDataSourceName()) );
		}
	}

	// turning exclusiveMode off:
	if(!exclusiveModeOn && exclusiveModeOn_) {
		exclusiveModeOn_ = false;
		int numSourceButtons = sourceButtons_.buttons().count();
		for(int di=0; di<numSourceButtons; di++) {
			sourceButtons_.button(di)->setChecked( model_->isVisible(scanIndex_, di) );
		}
	}
}

AMScanViewScanBarContextMenu::AMScanViewScanBarContextMenu(AMScanSetModel *model, int scanIndex, int dataSourceIndex, QWidget* parent)
	: QMenu(parent)
{
	model_ = model;
	QModelIndex di = model_->indexForDataSource(scanIndex, dataSourceIndex);
	pi_ = QPersistentModelIndex(di);

	QString scanName = model_->data(di.parent(), AM::NameRole).toString();
	QString dataSourceName = model_->data(di, AM::NameRole).toString();
	QString dataSourceDescription = model_->data(di, AM::DescriptionRole).toString();
	QString title = scanName % ": " % dataSourceName;
	setTitle(title);

	addAction(title)->setDisabled(true);
	if(dataSourceDescription != dataSourceName)
		addAction(dataSourceDescription)->setDisabled(true);
	addSeparator();
	addAction("For this scan:")->setEnabled(false);
	connect(addAction("   Show all"), SIGNAL(triggered()), this, SLOT(showAll()));
	connect(addAction("   Hide all except " % dataSourceName), SIGNAL(triggered()), this, SLOT(hideAllExceptDataSource()));
	addSeparator();
	if(model_->scanCount() > 1)
		connect(addAction("Show only " % dataSourceName), SIGNAL(triggered()), this, SLOT(showAllDataSource()));
	addSeparator();
	connect(addAction("Color and style..."), SIGNAL(triggered()), this, SLOT(editColorAndStyle()));
	addSeparator();
	connect(addAction("Remember Visibility"), SIGNAL(triggered()), model_, SLOT(saveVisibility()));

	connect(this, SIGNAL(aboutToHide()), this, SLOT(deleteLater()));
}

AMScanViewScanBarContextMenu::~AMScanViewScanBarContextMenu() {
	// nothing required yet
}

void AMScanViewScanBarContextMenu::hideAllExceptDataSource()
{
	if(!pi_.isValid())
		return;

	int scanIndex = pi_.parent().row();
	int dataSourceIndex = pi_.row();

	AMScan *scan = model_->scanAt(scanIndex);
	if (!scan)
		return;

	int dataSourceCount = scan->dataSourceCount();

	for (int i = 0; i < dataSourceCount; i++)
			model_->setVisible(scanIndex, i, i == dataSourceIndex);
}

void AMScanViewScanBarContextMenu::showAllDataSource()
{
	if(!pi_.isValid())
		return;

	int scanIndex = pi_.parent().row();
	int dataSourceIndex = pi_.row();
	QString nameOfDataSource(model_->dataSourceAt(scanIndex, dataSourceIndex)->name());

	for (int i = 0, scanCount = model_->scanCount(); i < scanCount; i++)
		for (int j = 0, dataSourceCount = model_->scanAt(i)->dataSourceCount(); j < dataSourceCount; j++)
				model_->setVisible(i, j, model_->dataSourceAt(i, j)->name() == nameOfDataSource);
}

void AMScanViewScanBarContextMenu::showAll()
{
	if(!pi_.isValid())
		return;

	int scanIndex = pi_.parent().row();

	AMScan *scan = model_->scanAt(scanIndex);
	if (!scan)
		return;

	int dataSourceCount = scan->dataSourceCount();

	for (int i = 0; i < dataSourceCount; i++)
		model_->setVisible(scanIndex, i, true);
}

#include "ui/dataman/AMScanSetItemPropertyDialog.h"
void AMScanViewScanBarContextMenu::editColorAndStyle()
{
	if(pi_.isValid()) {
		AMScanSetItemPropertyDialog* pd = new AMScanSetItemPropertyDialog(model_, pi_.parent().row(), pi_.row(), parentWidget());
		pd->show();
	}
}

// AMScanViewSingleSpectrumView
//////////////////////////////////////////////////

#include "MPlot/MPlotTools.h"

#include <QCheckBox>
#include <QPushButton>
#include "ui/dataman/AMSpectrumAndPeriodicTableView.h"

 AMScanViewSingleSpectrumView::~AMScanViewSingleSpectrumView(){}
AMScanViewSingleSpectrumView::AMScanViewSingleSpectrumView(QWidget *parent)
	: AMSpectrumAndPeriodicTableView(parent)
{
	AMSpectrumAndPeriodicTableView();

//	addMultipleSpectra_ = false;

//	QFont newFont = font();
//	newFont.setPointSize(18);
//	newFont.setBold(true);

//	title_ = new QLabel;
//	title_->setFont(newFont);

//	x_.resize(0);
//	sourceButtons_ = new QButtonGroup;
//	sourceButtons_->setExclusive(false);
//	connect(sourceButtons_, SIGNAL(buttonClicked(int)), this, SLOT(onCheckBoxChanged(int)));

//	setupPlot();

//	plot_->setMinimumSize(600, 400);
//	plot_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

//	table_ = new AMSelectablePeriodicTable(this);
//	table_->buildPeriodicTable();
//	connect(table_, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementSelected(AMElement*)));
//	connect(table_, SIGNAL(elementDeselected(AMElement*)), this, SLOT(onElementDeselected(AMElement*)));
//	tableView_ = new AMSelectablePeriodicTableView(table_);
//	tableView_->buildPeriodicTableView();
//	connect(tableView_, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementClicked(AMElement*)));

//	currentElement_ = table_->elementBySymbol("Fe");
//	combinationElement_ = table_->elementBySymbol("Ca");

//	QPushButton *removeAllEmissionLinesButton = new QPushButton(QIcon(":/trashcan.png"), "Clear Emission Lines");
//	removeAllEmissionLinesButton->setMaximumHeight(25);

//	showPileUpPeaksButton_ = new QPushButton("Show Fe Pile Up Peaks");
//	showPileUpPeaksButton_->setMaximumHeight(25);
//	showPileUpPeaksButton_->setCheckable(true);
//	showPileUpPeaksButton_->setEnabled(false);
//	showCombinationPileUpPeaksButton_ = new QPushButton("Show Combination Peaks");
//	showCombinationPileUpPeaksButton_->setMaximumHeight(25);
//	showCombinationPileUpPeaksButton_->setCheckable(true);
//	showCombinationPileUpPeaksButton_->setEnabled(false);
//	combinationChoiceButton_ = new QToolButton;
//	combinationChoiceButton_->setMaximumHeight(25);
//	combinationChoiceButton_->setText("Ca");
//	combinationChoiceButton_->setEnabled(false);

//	rowAbovePeriodicTableLayout_ = new QHBoxLayout;
//	rowAbovePeriodicTableLayout_->addWidget(removeAllEmissionLinesButton);
//	rowAbovePeriodicTableLayout_->addStretch();
//	rowAbovePeriodicTableLayout_->addWidget(showPileUpPeaksButton_);
//	rowAbovePeriodicTableLayout_->addWidget(showCombinationPileUpPeaksButton_);
//	rowAbovePeriodicTableLayout_->addWidget(combinationChoiceButton_);

//	connect(removeAllEmissionLinesButton, SIGNAL(clicked()), this, SLOT(removeAllEmissionLineMarkers()));

//	connect(showPileUpPeaksButton_, SIGNAL(clicked()), this, SLOT(updatePileUpPeaks()));
//	connect(showCombinationPileUpPeaksButton_, SIGNAL(clicked()), this, SLOT(updateCombinationPileUpPeaks()));
//	connect(showPileUpPeaksButton_, SIGNAL(toggled(bool)), this, SLOT(updatePileUpPeaksButtonText()));
//	connect(showCombinationPileUpPeaksButton_, SIGNAL(toggled(bool)), this, SLOT(updateCombinationPileUpPeaksButtonText()));
//	connect(showPileUpPeaksButton_, SIGNAL(toggled(bool)), showCombinationPileUpPeaksButton_, SLOT(setEnabled(bool)));
//	connect(showPileUpPeaksButton_, SIGNAL(toggled(bool)), combinationChoiceButton_, SLOT(setEnabled(bool)));
//	connect(combinationChoiceButton_, SIGNAL(clicked()), this, SLOT(onCombinationChoiceButtonClicked()));

//	QVBoxLayout *plotLayout = new QVBoxLayout;
//	plotLayout->addWidget(plot_);
//	plotLayout->addLayout(rowAbovePeriodicTableLayout_);
//	plotLayout->addWidget(tableView_, 0, Qt::AlignCenter);

//	emissionLineValidator_ = new AMNameAndRangeValidator(this);
//	pileUpPeakValidator_ = new AMNameAndRangeValidator(this);
//	combinationPileUpPeakValidator_ = new AMNameAndRangeValidator(this);

//	sourceButtonsLayout_ = new QVBoxLayout;
//	sourceButtonsLayout_->addWidget(new QLabel("Available Spectra"), 0, Qt::AlignLeft);
//	sourceButtonsLayout_->addStretch();

//	logEnableButton_ = new QPushButton("Logarithmic");
//	logEnableButton_->setCheckable(true);
//	connect(logEnableButton_, SIGNAL(toggled(bool)), this, SLOT(onLogScaleEnabled(bool)));

//	minimum_ = new QDoubleSpinBox;
//	minimum_->setSuffix(" eV");
//	minimum_->setDecimals(0);
//	minimum_->setRange(0, 1000000);
//	connect(minimum_, SIGNAL(editingFinished()), this, SLOT(onMinimumChanged()));

//	maximum_ = new QDoubleSpinBox;
//	maximum_->setSuffix(" eV");
//	maximum_->setDecimals(0);
//	maximum_->setRange(0, 1000000);
//	connect(maximum_, SIGNAL(editingFinished()), this, SLOT(onMaximumChanged()));

//	exportButton_ = new QPushButton(QIcon(":/save.png"), "Save to file...");
//	exportButton_->setEnabled(false);
//	connect(exportButton_, SIGNAL(clicked()), this, SLOT(onExportClicked()));

//	connect(emissionLineValidator_, SIGNAL(validatorChanged()), this, SLOT(updateEmissionLineMarkers()));

//	QVBoxLayout *sourcesLayout = new QVBoxLayout;
//	sourcesLayout->addLayout(sourceButtonsLayout_);
//	sourcesLayout->addStretch();
//	sourcesLayout->addWidget(new QLabel("Left Axis Scale"));
//	sourcesLayout->addWidget(logEnableButton_);
//	sourcesLayout->addWidget(new QLabel("Min. Energy"));
//	sourcesLayout->addWidget(minimum_);
//	sourcesLayout->addWidget(new QLabel("Max. Energy"));
//	sourcesLayout->addWidget(maximum_);
//	sourcesLayout->addWidget(exportButton_);

//	QHBoxLayout *plotAndSourcesLayout = new QHBoxLayout;
//	plotAndSourcesLayout->addLayout(plotLayout);
//	plotAndSourcesLayout->addLayout(sourcesLayout);

//	QVBoxLayout *layout = new QVBoxLayout;
//	layout->addWidget(title_, 0, Qt::AlignLeft);
//	layout->addLayout(plotAndSourcesLayout);

//	setLayout(layout);
}

void AMScanViewSingleSpectrumView::setTitle(const QString &title)
{
	title_->setText(title);
}

void AMScanViewSingleSpectrumView::setupPlot()
{
	AMSpectrumAndPeriodicTableView::setupPlot();

//	spectrumAndPlot_ = AMSpectrumAndPeriodicTableView::setupPlot();

//	MPlot *plot = new MPlot;
//	plot_ = new MPlotWidget(this);
//	plot_->setPlot(plot);

//	plot_->plot()->plotArea()->setBrush(QBrush(Qt::white));
//	plot_->plot()->axisBottom()->setTicks(5);
//	plot_->plot()->axisLeft()->setTicks(5);
//	plot_->plot()->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
//	plot_->plot()->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
//	plot_->plot()->axisBottom()->showAxisName(true);
//	plot_->plot()->axisLeft()->showAxisName(false);

//	// Set the margins for the plot.
//	plot_->plot()->setMarginLeft(10);
//	plot_->plot()->setMarginBottom(15);
//	plot_->plot()->setMarginRight(2);
//	plot_->plot()->setMarginTop(2);

//	plot_->plot()->addTool(new MPlotDragZoomerTool());
//	plot_->plot()->addTool(new MPlotWheelZoomerTool());
}

void AMScanViewSingleSpectrumView::removeAllPlotItems(QList<MPlotItem *> &items)
{
	AMSpectrumAndPeriodicTableView::removeAllPlotItems(items);

//	foreach (MPlotItem *item, items)
//		if (plot_->plot()->removeItem(item)){

//			item->signalSource()->disconnect();
//			delete item;
//		}

//	items.clear();
}

void AMScanViewSingleSpectrumView::onElementSelected(AMElement *element)
{
	AMSpectrumAndPeriodicTableView::onElementSelected(element);

//	QColor color = AMDataSourcePlotSettings::nextColor();

//	foreach (AMEmissionLine emissionLine, element->emissionLines()){

//		if (emissionLineValidator_->isValid(emissionLine.name(), emissionLine.energy())){

//			MPlotPoint *newLine = new MPlotPoint(QPointF(emissionLine.energy(), 0));
//			newLine->setMarker(MPlotMarkerShape::VerticalBeam, 1e6, QPen(color), QBrush(color));
//			newLine->setDescription(emissionLine.greekName() % ": " % emissionLine.energyString() % " eV");
//			plot_->plot()->addItem(newLine);
//			emissionLineMarkers_ << newLine;
//		}
//	}

//	showPileUpPeaksButton_->setEnabled(true);
}

void AMScanViewSingleSpectrumView::onElementDeselected(AMElement *element)
{

	AMSpectrumAndPeriodicTableView::onElementDeselected(element);

//	QString symbol = element->symbol();

//	foreach(MPlotItem *item, emissionLineMarkers_){

//		if (item->description().contains(QRegExp(QString("^%1 (K|L|M)").arg(symbol))))
//			if (plot_->plot()->removeItem(item)){

//				emissionLineMarkers_.removeOne(item);
//				delete item;
//			}
//	}

//	showPileUpPeaksButton_->setEnabled(table_->hasSelectedElements());
}

void AMScanViewSingleSpectrumView::updateEmissionLineMarkers()
{
	AMSpectrumAndPeriodicTableView::updateEmissionLineMarkers();

//	foreach (AMElement *element, table_->selectedElements())
//		onElementDeselected(element);

//	foreach (AMElement *element, table_->selectedElements())
//		onElementSelected(element);
}

void AMScanViewSingleSpectrumView::updatePileUpPeaksButtonText()
{
	AMSpectrumAndPeriodicTableView::updatePileUpPeaksButtonText();
//	showPileUpPeaksButton_->setText(QString("%1 %2 Pile Up Peaks").arg(showPileUpPeaksButton_->isChecked() ? "Hide" : "Show").arg(currentElement_->symbol()));
}

void AMScanViewSingleSpectrumView::updateCombinationPileUpPeaksButtonText()
{
	AMSpectrumAndPeriodicTableView::updateCombinationPileUpPeaksButtonText();
//	showCombinationPileUpPeaksButton_->setText(QString("%1 Combination Peaks").arg(showPileUpPeaksButton_->isChecked() ? "Hide" : "Show"));
}

void AMScanViewSingleSpectrumView::updatePileUpPeaks()
{
	AMSpectrumAndPeriodicTableView::updatePileUpPeaks();

//	removeAllPlotItems(pileUpPeakMarkers_);

//	if (showPileUpPeaksButton_->isChecked() && showPileUpPeaksButton_->isEnabled() && table_->isSelected(currentElement_)){

//		for (int i = 0, size = currentElement_->emissionLines().size(); i < size; i++)
//			for (int j = i; j < size; j++)
//				addPileUpMarker(currentElement_->emissionLines().at(i), currentElement_->emissionLines().at(j));
//	}

//	updateCombinationPileUpPeaks();
}

void AMScanViewSingleSpectrumView::updateCombinationPileUpPeaks()
{
	AMSpectrumAndPeriodicTableView::updateCombinationPileUpPeaks();

//	removeAllPlotItems(combinationPileUpPeakMarkers_);

//	if (showCombinationPileUpPeaksButton_->isChecked() && showCombinationPileUpPeaksButton_->isEnabled()){

//		for (int i = 0, iSize = currentElement_->emissionLines().size(); i < iSize; i++)
//			for (int j = 0, jSize = combinationElement_->emissionLines().size(); j < jSize; j++)
//				addPileUpMarker(currentElement_->emissionLines().at(i), combinationElement_->emissionLines().at(j));
//	}
}

void AMScanViewSingleSpectrumView::onCombinationChoiceButtonClicked()
{
	AMSpectrumAndPeriodicTableView::onCombinationChoiceButtonClicked();

//	AMElement *el = AMPeriodicTableDialog::getElement();

//	if (el){

//		combinationChoiceButton_->setText(el->symbol());
//		combinationElement_ = el;
//		updateCombinationPileUpPeaks();
//	}
}

void AMScanViewSingleSpectrumView::removeAllEmissionLineMarkers()
{
	AMSpectrumAndPeriodicTableView::removeAllEmissionLineMarkers();

//	foreach (MPlotItem *item, emissionLineMarkers_)
//		if (plot_->plot()->removeItem(item))
//			delete item;

//	emissionLineMarkers_.clear();
//	table_->deselectAllElements();
//	showPileUpPeaksButton_->setEnabled(false);
}

void AMScanViewSingleSpectrumView::addPileUpMarker(const AMEmissionLine &firstLine, const AMEmissionLine &secondLine)
{
	AMSpectrumAndPeriodicTableView::addPileUpMarker(firstLine, secondLine);

//	// You can't have pile up if the original line is below your threshold.
//	if (!emissionLineValidator_->isValid(firstLine.name(), firstLine.energy()))
//		return;

//	AMNameAndRangeValidator *validator;
//	QColor markerColor;

//	if (firstLine.elementSymbol() == secondLine.elementSymbol()){

//		validator = pileUpPeakValidator_;
//		markerColor = QColor(42, 149, 77);
//	}

//	else{

//		validator = combinationPileUpPeakValidator_;
//		markerColor = QColor(24, 116, 205);
//	}

//	double energy = firstLine.energy() + secondLine.energy();

//	if (validator->isValid(firstLine.name(), firstLine.energy())
//			&& validator->isValid(secondLine.name(), secondLine.energy())
//			&& validator->isValid(energy)){

//		MPlotPoint *newMarker = new MPlotPoint(QPointF(energy, 0));
//		newMarker->setMarker(MPlotMarkerShape::VerticalBeam, 1e6, QPen(markerColor), QBrush(markerColor));
//		newMarker->setDescription(QString("%1 + %2: %3 eV").arg(firstLine.greekName()).arg(secondLine.greekName()).arg(energy));
//		plot_->plot()->addItem(newMarker);

//		if (firstLine.elementSymbol() == secondLine.elementSymbol())
//			pileUpPeakMarkers_ << newMarker;

//		else
//			combinationPileUpPeakMarkers_ << newMarker;
//	}
}

void AMScanViewSingleSpectrumView::onLogScaleEnabled(bool enable)
{
	AMSpectrumAndPeriodicTableView::onLogScaleEnabled(enable);
//	if (enable){

//		plot_->plot()->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(1, MPLOT_POS_INFINITY));
//		logEnableButton_->setText("Linear");
//	}

//	else {

//		plot_->plot()->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(MPLOT_NEG_INFINITY, MPLOT_POS_INFINITY));
//		logEnableButton_->setText("Logarithmic");
//	}

//	plot_->plot()->axisScaleLeft()->setLogScaleEnabled(enable);
}

void AMScanViewSingleSpectrumView::setEnergyRange(double low, double high)
{
	AMSpectrumAndPeriodicTableView::setEnergyRange(low, high);

//	AMRange range = AMRange(low, high);
//	emissionLineValidator_->setRange(range);
//	pileUpPeakValidator_->setRange(range);
//	combinationPileUpPeakValidator_->setRange(range);
//	tableView_->setEnergyRange(low, high);
//	minimum_->setValue(low);
//	maximum_->setValue(high);
}

void AMScanViewSingleSpectrumView::setMinimumEnergy(double newMinimum)
{
	AMSpectrumAndPeriodicTableView::setMinimumEnergy(newMinimum);

//	emissionLineValidator_->setMinimum(newMinimum);
//	pileUpPeakValidator_->setMinimum(newMinimum);
//	combinationPileUpPeakValidator_->setMinimum(newMinimum);
//	tableView_->setMinimumEnergy(newMinimum);
}

void AMScanViewSingleSpectrumView::setMaximumEnergy(double newMaximum)
{
	AMSpectrumAndPeriodicTableView::setMaximumEnergy(newMaximum);

//	emissionLineValidator_->setMaximum(newMaximum);
//	pileUpPeakValidator_->setMaximum(newMaximum);
//	combinationPileUpPeakValidator_->setMaximum(newMaximum);
//	tableView_->setMaximumEnergy(newMaximum);
}

void AMScanViewSingleSpectrumView::addEmissionLineNameFilter(const QRegExp &newNameFilter)
{
	AMSpectrumAndPeriodicTableView::emissionLineValidator_->addNameFilter(newNameFilter);
}

bool AMScanViewSingleSpectrumView::removeEmissionLineNameFilter(int index)
{
	return AMSpectrumAndPeriodicTableView::emissionLineValidator_->removeNameFilter(index);
}

bool AMScanViewSingleSpectrumView::removeEmissionLineNameFilter(const QRegExp &filter)
{
	return AMSpectrumAndPeriodicTableView::emissionLineValidator_->removeNameFilter(filter);
}

void AMScanViewSingleSpectrumView::addPileUpPeakNameFilter(const QRegExp &newNameFilter)
{
	AMSpectrumAndPeriodicTableView::pileUpPeakValidator_->addNameFilter(newNameFilter);
}

bool AMScanViewSingleSpectrumView::removePileUpPeakNameFilter(int index)
{
	return AMSpectrumAndPeriodicTableView::pileUpPeakValidator_->removeNameFilter(index);
}

bool AMScanViewSingleSpectrumView::removePileUpPeakNameFilter(const QRegExp &filter)
{
	return AMSpectrumAndPeriodicTableView::pileUpPeakValidator_->removeNameFilter(filter);
}

void AMScanViewSingleSpectrumView::addCombinationPileUpPeakNameFilter(const QRegExp &newNameFilter)
{
	AMSpectrumAndPeriodicTableView::combinationPileUpPeakValidator_->addNameFilter(newNameFilter);
}

bool AMScanViewSingleSpectrumView::removeCombinationPileUpPeakNameFilter(int index)
{
	return AMSpectrumAndPeriodicTableView::combinationPileUpPeakValidator_->removeNameFilter(index);
}

bool AMScanViewSingleSpectrumView::removeCombinationPileUpPeakNameFilter(const QRegExp &filter)
{
	return AMSpectrumAndPeriodicTableView::combinationPileUpPeakValidator_->removeNameFilter(filter);
}


void AMScanViewSingleSpectrumView::onMinimumChanged()
{
	AMSpectrumAndPeriodicTableView::onMinimumEnergyChanged();
//	setMinimumEnergy(minimum_->value());
}

void AMScanViewSingleSpectrumView::onMaximumChanged()
{
	AMSpectrumAndPeriodicTableView::onMaximumEnergyChanged();
//	setMaximumEnergy(maximum_->value());
}

void AMScanViewSingleSpectrumView::onDataPositionChanged(const AMnDIndex &index)
{
	if (isVisible()){

		addMultipleSpectra_ = false;
		updatePlot(index);
	}
}

void AMScanViewSingleSpectrumView::onSelectedRectChanged(const AMnDIndex &start, const AMnDIndex &end)
{
	if (isVisible()){

		addMultipleSpectra_ = true;
		updatePlot(start, end);
	}
}

void AMScanViewSingleSpectrumView::setAxisInfo(AMAxisInfo info, bool propogateToPlotRange)
{
	AMSpectrumAndPeriodicTableView::setAxisInfo(info, propogateToPlotRange);
//	if (info.units.isEmpty())
//		plot_->plot()->axisBottom()->setAxisName(info.name);

//	else
//		plot_->plot()->axisBottom()->setAxisName(info.name % ", " % info.units);

//	x_.resize(info.size);

//	for (int i = 0; i < info.size; i++)
//		x_[i] = double(info.start) + i*double(info.increment);

//	if (propogateToPlotRange)
//		setEnergyRange(double(info.start), double(info.start) + info.size*double(info.increment));
}

void AMScanViewSingleSpectrumView::onCheckBoxChanged(int id)
{
	if (sourceButtons_->button(id)->isChecked()){

		plot_->plot()->addItem(series_.at(id));
		updatePlot(id);
	}

	else
		plot_->plot()->removeItem(series_.at(id));

	exportButton_->setEnabled(plot_->plot()->numItems() > 0);
}

void AMScanViewSingleSpectrumView::updatePlot(const AMnDIndex &index)
{
	if (!sources_.isEmpty() && index.rank() == sources_.first()->rank()-1){

		int size = sources_.first()->size(index.rank())-1;

		switch(sources_.first()->rank()){

		case 0: // 0D data source.  Not possible.
			break;

		case 1:	// 1D data source.  0D scan rank.

			startIndex_ = AMnDIndex(0);
			endIndex_ = AMnDIndex(size);
			break;

		case 2:	// 2D data source.  1D scan rank.

			startIndex_ = AMnDIndex(index.i(), 0);
			endIndex_ = AMnDIndex(index.i(), size);
			break;

		case 3:	// 3D data source.  2D scan rank.

			startIndex_ = AMnDIndex(index.i(), index.j(), 0);
			endIndex_ = AMnDIndex(index.i(), index.j(), size);
			break;
		}
	}

	for (int i = 0, count = sourceButtons_->buttons().size(); i < count; i++)
		if (sourceButtons_->button(i)->isChecked())
			updatePlot(i);
}

void AMScanViewSingleSpectrumView::updatePlot(const AMnDIndex &start, const AMnDIndex &end)
{
	if (!sources_.isEmpty() && start.rank() == sources_.first()->rank()-1 && start.rank() == end.rank()){

		int size = sources_.first()->size(start.rank())-1;

		switch(sources_.first()->rank()){

		case 0: // 0D data source.  Not possible.
			break;

		case 1:	// 1D data source.  0D scan rank.

			startIndex_ = AMnDIndex(0);
			endIndex_ = AMnDIndex(size);
			break;

		case 2:	// 2D data source.  1D scan rank.

			startIndex_ = AMnDIndex(start.i(), 0);
			endIndex_ = AMnDIndex(end.i(), size);
			break;

		case 3:	// 3D data source.  2D scan rank.

			startIndex_ = AMnDIndex(start.i(), start.j(), 0);
			endIndex_ = AMnDIndex(end.i(), end.j(), size);
			break;
		}
	}

	for (int i = 0, count = sourceButtons_->buttons().size(); i < count; i++)
		if (sourceButtons_->button(i)->isChecked())
			updatePlot(i);
}

void AMScanViewSingleSpectrumView::updatePlot(int id)
{
	AMDataSource *source = sources_.at(id);

	// If any AMDataSource::values() calls fail, the output should be set to zero to minimize the chance of using a bad data inside of the model.
	if (!addMultipleSpectra_){

		QVector<double> data(source->size(source->rank()-1));

		if (!source->values(startIndex_, endIndex_, data.data()))
			data.fill(0);

		models_.at(id)->setValues(x_, data);
	}

	else {

		switch(startIndex_.rank()){

		case 1:{	// 1D data source.  0D scan rank.

			QVector<double> output = QVector<double>(source->size(source->rank()-1), 0);

			if (!source->values(AMnDIndex(0), AMnDIndex(output.size()-1), output.data()))
				output.fill(0);

			models_.at(id)->setValues(x_, output);

			break;
		}

		case 2:{	// 2D data source.  1D scan rank.

			QVector<double> output = QVector<double>(source->size(source->rank()-1), 0);
			QVector<double> data = QVector<double>(source->size(source->rank()-1), 0);
			bool valuesSuccess = true;

			for (int i = startIndex_.i(), iSize = endIndex_.i()+1; i < iSize && valuesSuccess; i++){

				valuesSuccess = source->values(AMnDIndex(i, 0), AMnDIndex(i, output.size()-1), data.data());

				for (int j = 0, jSize = output.size(); j < jSize && valuesSuccess; j++)
					output[j] += data.at(j);
			}

			if (!valuesSuccess)
				output.fill(0);

			models_.at(id)->setValues(x_, output);

			break;
		}

		case 3:{	// 3D data source.  2D scan rank.

			QVector<double> output = QVector<double>(source->size(source->rank()-1), 0);
			QVector<double> data = QVector<double>(source->size(source->rank()-1), 0);
			bool valuesSuccess = true;

			for (int i = startIndex_.i(), iSize = endIndex_.i()+1; i < iSize && valuesSuccess; i++)
				for (int j = startIndex_.j(), jSize = endIndex_.j()+1; j < jSize && valuesSuccess; j++){

					valuesSuccess = source->values(AMnDIndex(i, j, 0), AMnDIndex(i, j, output.size()-1), data.data());

					for (int k = 0, kSize = output.size(); k < kSize && valuesSuccess; k++)
						output[k] += data.at(k);
				}

			if (!valuesSuccess)
				output.fill(0);

			models_.at(id)->setValues(x_, output);

			break;
		}
		}
	}
}

void AMScanViewSingleSpectrumView::onElementClicked(AMElement *element)
{
	AMSpectrumAndPeriodicTableView::onElementClicked(element);
//	currentElement_ = element;
//	updatePileUpPeaksButtonText();
//	updatePileUpPeaks();
}

void AMScanViewSingleSpectrumView::setDataSourceByName(const QString &name)
{
	for (int i = 0, count = sources_.size(); i < count; i++)
		if (sources_.at(i)->name() == name)
			sourceButtons_->button(i)->click();

	updatePlot();
}

void AMScanViewSingleSpectrumView::setDataSources(const QList<AMDataSource *> &sources)
{
	// Clean out the old buttons and models.
	QList<QAbstractButton *> buttons = sourceButtons_->buttons();

	foreach (QAbstractButton *button, buttons){

		sourceButtons_->removeButton(button);
		sourceButtonsLayout_->removeWidget(button);
		button->deleteLater();
	}

	buttons.clear();

	foreach (MPlotItem *item, plot_->plot()->plotItems())
		if (item->type() == MPlotItem::Series)
			plot_->plot()->removeItem(item);

	foreach (MPlotSeriesBasic *series, series_)
		delete series;

	series_.clear();

	foreach (MPlotVectorSeriesData *model, models_)
		delete model;

	models_.clear();

	// Fill in the button groups and models.
	sources_ = sources;

	foreach (AMDataSource *source, sources_)
		connect(source->signalSource(), SIGNAL(axisInfoChanged(int)), this, SLOT(onAxisInfoChanged()));

	QAbstractButton *button = 0;
	AMDataSource *source = 0;
	MPlotVectorSeriesData *model = 0;
	MPlotSeriesBasic *series;

	for (int i = 0, count = sources_.size(); i < count; i++){

		source = sources_.at(i);
		button = new QCheckBox(source->name());
		sourceButtons_->addButton(button, i);
		sourceButtonsLayout_->insertWidget(sourceButtonsLayout_->count()-1, button, 0, Qt::AlignLeft);

		model = new MPlotVectorSeriesData;
		series = new MPlotSeriesBasic(model);
		series->setMarker(MPlotMarkerShape::None);
		series->setDescription(source->description());
		series->setLinePen(AMDataSourcePlotSettings().linePen);
		models_.append(model);
		series_.append(series);
	}

	// Setup the plot's independant axis and automatically show the spectra if the sources have already been checked off.
	if (!sources_.isEmpty()){

		onAxisInfoChanged();

		for (int i = 0, size = sources_.size(); i < size; i++)
			if (sourceButtons_->button(i)->isChecked())
				updatePlot(i);
	}
}

void AMScanViewSingleSpectrumView::onAxisInfoChanged()
{
	AMSpectrumAndPeriodicTableView::onAxisInfoChanged();

//	AMAxisInfo info = sources_.first()->axisInfoAt(sources_.first()->rank()-1);

//	if (info.units.isEmpty())
//		plot_->plot()->axisBottom()->setAxisName(info.name);

//	else
//		plot_->plot()->axisBottom()->setAxisName(info.name % ", " % info.units);

//	x_.resize(info.size);

//	for (int i = 0; i < info.size; i++)
//		x_[i] = double(info.start) + i*double(info.increment);

//	setEnergyRange(double(info.start), double(info.start) + info.size*double(info.increment));
}

#include <QFileDialog>
#include <QMessageBox>

void AMScanViewSingleSpectrumView::onExportClicked()
{
	QString filename = QFileDialog::getSaveFileName(this, "Choose file name for data.", QString(), "Data files (*.dat);;All files (*)");

	if (!filename.isEmpty() && !filename.endsWith(".dat"))
		filename.append(".dat");

	if (!filename.isEmpty() && !exportToFile(filename))
		QMessageBox::warning(this, "Unable to save!", "The file was unable to save correctly.  Maybe the file path does not exist?", QMessageBox::Ok);
}

#include <QFile>
#include <QTextStream>

bool AMScanViewSingleSpectrumView::exportToFile(const QString &filename) const
{
	QFile file(filename);

	if (!file.open(QFile::WriteOnly))
		return false;

	// Get the checked sources.
	QList<int> sources;

	for (int i = 0, size = sourceButtons_->buttons().size(); i < size; i++)
		if (sourceButtons_->button(i)->isChecked())
			sources << i;

	QTextStream out(&file);

	out << "Spectra from the show spectra view\n";
	out << "Energy";

	for (int i = 0, size = sources.size(); i < size; i++)
		out << QString("\t%1").arg(sourceButtons_->button(sources.at(i))->text());

	out << "\n";

	for (int i = 0, size = x_.size(); i < size; i++){

		out << QString::number(x_.at(i));

		for (int j = 0, modelSize = sources.size(); j < modelSize; j++)
			out << QString("\t%1").arg(models_.at(sources.at(j))->y(i));

		out << "\n";
	}

	file.close();

	return true;
}

AMScanViewSourceSelector::~AMScanViewSourceSelector(){}
