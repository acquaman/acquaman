#include "AMSelectableElementView.h"

#include <QStringBuilder>
#include <QGroupBox>
#include <QScrollArea>

// AMSelectableAbsorptionEdgeView
////////////////////////////////////////////////

AMSelectableAbsorptionEdgeView::AMSelectableAbsorptionEdgeView(const AMAbsorptionEdge &edge, QWidget *parent)
	: AMSelectableItemView(parent)
{
	edge_ = edge;

	if (!edge_.isNull())
		checkBox_->setText(edge_.name() % ": " % edge_.energyString() % " eV");

	connect(this, SIGNAL(itemSelected(bool)), this, SLOT(onItemSelectedChanged(bool)));

	setVisible(!edge_.isNull());
}

void AMSelectableAbsorptionEdgeView::setAbsorptionEdge(const AMAbsorptionEdge &edge, bool alreadySelected)
{
	blockSignals(true);
	edge_ = edge;
	setVisible(!edge_.isNull());
	checkBox_->setChecked(alreadySelected);
	checkBox_->setText(edge_.name() % ": " % edge_.energyString() % " eV");
	blockSignals(false);
}

void AMSelectableAbsorptionEdgeView::onItemSelectedChanged(bool isSelected)
{
	emit absorptionEdgeSelected(isSelected, edge_);
}

// AMSelectableEmissionLineView
////////////////////////////////////////////////

AMSelectableEmissionLineView::AMSelectableEmissionLineView(const AMEmissionLine &line, QWidget *parent)
	: AMSelectableItemView(parent)
{
	emissionLine_ = line;

	if (!emissionLine_.isNull())
		checkBox_->setText(emissionLine_.name() % ": " % emissionLine_.energyString());

	connect(this, SIGNAL(itemSelected(bool)), this, SLOT(onItemSelectedChanged(bool)));

	setVisible(!emissionLine_.isNull());
}

void AMSelectableEmissionLineView::setEmissionLine(const AMEmissionLine &line, bool alreadySelected)
{
	blockSignals(true);
	emissionLine_ = line;
	setVisible(!emissionLine_.isNull());
	checkBox_->setChecked(alreadySelected);
	checkBox_->setText(emissionLine_.name() % ": " % emissionLine_.energyString() % " eV");
	blockSignals(false);
}

void AMSelectableEmissionLineView::onItemSelectedChanged(bool isSelected)
{
	emit emissionLineSelected(isSelected, emissionLine_);
}

// AMSelectableElementView
////////////////////////////////////////////////

AMSelectableElementView::AMSelectableElementView(AMSelectableElement *element, QWidget *parent)
	: QWidget(parent)
{
	element_ = element;
	energyRange_ = AMRange();

	elementName_ = new QLabel;
	elementName_->setFont(QFont("Times New Roman", 16));
	elementSymbol_ = new QLabel;
	elementSymbol_->setFont(QFont("Times New Roman", 45, 10));

	QVBoxLayout *emissionLineGroupBoxLayout = new QVBoxLayout;
	QGroupBox *emissionLineGroupBox = new QGroupBox;
	emissionLineGroupBox->setLayout(emissionLineGroupBoxLayout);
	QScrollArea *emissionLineScrollArea = new QScrollArea;
	emissionLineScrollArea->setWidget(emissionLineGroupBox);

	for (int i = 0; i < 9; i++){

		AMSelectableEmissionLineView *lineView = new AMSelectableEmissionLineView(AMEmissionLine(), this);
		connect(lineView, SIGNAL(emissionLineSelected(bool,AMEmissionLine)), this, SLOT(onEmissionLineSelected(bool,AMEmissionLine)));
		emissionLineViews_ << lineView;
		emissionLineGroupBoxLayout->addWidget(lineView);
	}

	QVBoxLayout *absorptionEdgeGroupBoxLayout = new QVBoxLayout;
	QGroupBox *absorptionEdgeGroupBox = new QGroupBox;
	absorptionEdgeGroupBox->setLayout(absorptionEdgeGroupBoxLayout);
	QScrollArea *absorptionEdgeScrollArea = new QScrollArea;
	absorptionEdgeScrollArea->setWidget(absorptionEdgeGroupBox);

	for (int i = 0; i < 24; i++){

		AMSelectableAbsorptionEdgeView *edgeView = new AMSelectableAbsorptionEdgeView(AMAbsorptionEdge(), this);
		connect(edgeView, SIGNAL(absorptionEdgeSelected(bool,AMAbsorptionEdge)), this, SLOT(onAbsorptionEdgeSelected(bool,AMAbsorptionEdge)));
		absorptionEdgeViews_ << edgeView;
		absorptionEdgeGroupBoxLayout->addWidget(edgeView);
	}

	QHBoxLayout *titleLayout = new QHBoxLayout;
	titleLayout->addWidget(elementName_);
	titleLayout->addWidget(elementSymbol_);

	QHBoxLayout *lineAndEdgeLayout = new QHBoxLayout;
	lineAndEdgeLayout->addWidget(emissionLineScrollArea);
	lineAndEdgeLayout->addWidget(absorptionEdgeScrollArea);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(titleLayout);
	mainLayout->addLayout(lineAndEdgeLayout);

	setLayout(mainLayout);

	setMaximumSize(300, 300);
}

void AMSelectableElementView::addAbsorptionEdgeNameFilter(const QString &newNameFilter)
{
	if (!absorptionEdgeNameFilters_.contains(newNameFilter)){

		absorptionEdgeNameFilters_ << newNameFilter;
		updateAbsorptionEdgeViewList();
	}
}

bool AMSelectableElementView::removeAbsorptionEdgeNameFilter(int index)
{
	bool successfullyRemoved = (index >= 0) && (index < absorptionEdgeNameFilters_.size());

	if (successfullyRemoved){

		absorptionEdgeNameFilters_.removeAt(index);
		updateAbsorptionEdgeViewList();
	}

	return successfullyRemoved;
}

bool AMSelectableElementView::removeAbsorptionEdgeNameFilter(const QString &filter)
{
	bool successfullyRemoved = absorptionEdgeNameFilters_.removeOne(filter);

	if (successfullyRemoved)
		updateAbsorptionEdgeViewList();

	return successfullyRemoved;
}

void AMSelectableElementView::addEmissionLineNameFilter(const QString &newNameFilter)
{
	if (!emissionLineNameFilters_.contains(newNameFilter)){

		emissionLineNameFilters_ << newNameFilter;
		updateEmissionLineViewList();
	}
}

bool AMSelectableElementView::removeEmissionLineNameFilter(int index)
{
	bool successfullyRemoved = (index >= 0) && (index < emissionLineNameFilters_.size());

	if (successfullyRemoved){

		emissionLineNameFilters_.removeAt(index);
		updateAbsorptionEdgeViewList();
	}

	return successfullyRemoved;
}

bool AMSelectableElementView::removeEmissionLineNameFilter(const QString &filter)
{
	bool successfullyRemoved = emissionLineNameFilters_.removeOne(filter);

	if (successfullyRemoved)
		updateAbsorptionEdgeViewList();

	return successfullyRemoved;
}

void AMSelectableElementView::setElement(AMSelectableElement *element)
{
	element_ = element;

	if (element_){

		elementName_->setText(element_->name());
		elementSymbol_->setText(element_->symbol());
	}

	updateAbsorptionEdgeViewList();
	updateEmissionLineViewList();
}

void AMSelectableElementView::setEnergyRange(const AMRange &newRange)
{
	if (energyRange_ != newRange){

		energyRange_ = newRange;
		updateAbsorptionEdgeViewList();
		updateEmissionLineViewList();
	}
}

void AMSelectableElementView::setMinimumEnergy(double newMinimum)
{
	if (energyRange_.minimum() != newMinimum){

		energyRange_.setMinimum(newMinimum);
		updateAbsorptionEdgeViewList();
		updateEmissionLineViewList();
	}
}

void AMSelectableElementView::setMaximumEnergy(double newMaximum)
{
	if (energyRange_.maximum() != newMaximum){

		energyRange_.setMaximum(newMaximum);
		updateAbsorptionEdgeViewList();
		updateEmissionLineViewList();
	}
}

void AMSelectableElementView::updateAbsorptionEdgeViewList()
{
	QList<AMAbsorptionEdge> edges(element_->absorptionEdges());
	int viewIterator = 0;

	for (int i = 0, size = edges.size(); i < size; i++){

		AMAbsorptionEdge edge = edges.at(i);

		if (isAbsorptionEdgeValid(edge.name()) && energyRange_.withinRange(edge.energy()))
			absorptionEdgeViews_.at(viewIterator++)->setAbsorptionEdge(edge, element_->isSelected(edge));
	}

	// Then hide the rest.
	for (int listSize = absorptionEdgeViews_.size(); viewIterator < listSize; viewIterator++)
		absorptionEdgeViews_.at(viewIterator)->setAbsorptionEdge(AMAbsorptionEdge());
}

void AMSelectableElementView::updateEmissionLineViewList()
{
	QList<AMEmissionLine> lines(element_->emissionLines());
	int viewIterator = 0;

	for (int i = 0, size = lines.size(); i < size; i++){

		AMEmissionLine line = lines.at(i);

		if (isEmissionLineValid(line.name()) && energyRange_.withinRange(line.energy()))
			emissionLineViews_.at(viewIterator++)->setEmissionLine(line, element_->isSelected(line));
	}

	// Then hide the rest.
	for (int listSize = emissionLineViews_.size(); viewIterator < listSize; viewIterator++)
		emissionLineViews_.at(viewIterator)->setEmissionLine(AMEmissionLine());
}

bool AMSelectableElementView::isAbsorptionEdgeValid(const QString &name) const
{
	bool isValid = true;

	foreach (QString nameFilter, absorptionEdgeNameFilters_)
		isValid = isValid && name.contains(nameFilter);

	return isValid;
}

bool AMSelectableElementView::isEmissionLineValid(const QString &name) const
{
	bool isValid = true;

	foreach (QString nameFilter, emissionLineNameFilters_)
		isValid = isValid && name.contains(nameFilter);

	return isValid;
}

void AMSelectableElementView::onAbsorptionEdgeSelected(bool isSelected, const AMAbsorptionEdge &edge)
{
	if (isSelected)
		element_->selectAbsorptionEdge(edge);

	else
		element_->deselectAbsorptionEdge(edge);
}

void AMSelectableElementView::onEmissionLineSelected(bool isSelected, const AMEmissionLine &line)
{
	if (isSelected)
		element_->selectEmissionLine(line);

	else
		element_->deselectEmissionLine(line);
}
