#include "AMSelectableElementView.h"

#include <QStringBuilder>

// AMSelectableAbsorptionEdgeView
////////////////////////////////////////////////

 AMSelectableAbsorptionEdgeView::~AMSelectableAbsorptionEdgeView(){}
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

 AMSelectableEmissionLineView::~AMSelectableEmissionLineView(){}
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
	checkBox_->setText(emissionLine_.greekName() % ": " % emissionLine_.energyString() % " eV");
	blockSignals(false);
}

void AMSelectableEmissionLineView::onItemSelectedChanged(bool isSelected)
{
	emit emissionLineSelected(isSelected, emissionLine_);
}

// AMSelectableElementView
////////////////////////////////////////////////

 AMSelectableElementView::~AMSelectableElementView(){}
AMSelectableElementView::AMSelectableElementView(AMSelectableElement *element, QWidget *parent)
	: QWidget(parent)
{
	element_ = element;

	absorptionEdgeValidator_ = new AMNameAndRangeValidator(this);
	emissionLineValidator_ = new AMNameAndRangeValidator(this);
	connect(absorptionEdgeValidator_, SIGNAL(validatorChanged()), this, SLOT(updateAbsorptionEdgeViewList()));
	connect(emissionLineValidator_, SIGNAL(validatorChanged()), this, SLOT(updateEmissionLineViewList()));

	elementName_ = new QLabel;
	elementName_->setFont(QFont("Times New Roman", 16));
	elementSymbol_ = new QLabel;
	elementSymbol_->setFont(QFont("Times New Roman", 45, 10));

	QVBoxLayout *emissionLineGroupBoxLayout = new QVBoxLayout;
	emissionLineGroupBoxLayout->setContentsMargins(0,0,0,0);
	emissionLineGroupBoxLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	emissionLineGroupBox_ = new QGroupBox;
	emissionLineGroupBox_->setFlat(true);
	emissionLineGroupBox_->setLayout(emissionLineGroupBoxLayout);
	emissionLineScrollArea_ = new QScrollArea;
	emissionLineScrollArea_->setWidget(emissionLineGroupBox_);
	emissionLineScrollArea_->setFrameShape(QFrame::NoFrame);
	emissionLineScrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	for (int i = 0; i < 9; i++){

		AMSelectableEmissionLineView *lineView = new AMSelectableEmissionLineView(AMEmissionLine(), this);
		connect(lineView, SIGNAL(emissionLineSelected(bool,AMEmissionLine)), this, SLOT(onEmissionLineSelected(bool,AMEmissionLine)));
		emissionLineViews_ << lineView;
		emissionLineGroupBoxLayout->addWidget(lineView);
	}

	emissionLineGroupBoxLayout->addStretch();

	QVBoxLayout *absorptionEdgeGroupBoxLayout = new QVBoxLayout;
	absorptionEdgeGroupBox_ = new QGroupBox;
	absorptionEdgeGroupBox_->setFlat(true);
	absorptionEdgeGroupBox_->setLayout(absorptionEdgeGroupBoxLayout);
	absorptionEdgeScrollArea_ = new QScrollArea;
	absorptionEdgeScrollArea_->setWidget(absorptionEdgeGroupBox_);
	absorptionEdgeScrollArea_->setFrameShape(QFrame::NoFrame);
	absorptionEdgeScrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	for (int i = 0; i < 24; i++){

		AMSelectableAbsorptionEdgeView *edgeView = new AMSelectableAbsorptionEdgeView(AMAbsorptionEdge(), this);
		connect(edgeView, SIGNAL(absorptionEdgeSelected(bool,AMAbsorptionEdge)), this, SLOT(onAbsorptionEdgeSelected(bool,AMAbsorptionEdge)));
		absorptionEdgeViews_ << edgeView;
		absorptionEdgeGroupBoxLayout->addWidget(edgeView);
	}

	absorptionEdgeGroupBoxLayout->addStretch();

	QHBoxLayout *titleLayout = new QHBoxLayout;
	titleLayout->addWidget(elementName_);
	titleLayout->addWidget(elementSymbol_);

	QHBoxLayout *lineAndEdgeLayout = new QHBoxLayout;
	lineAndEdgeLayout->addWidget(emissionLineScrollArea_);
	lineAndEdgeLayout->addWidget(absorptionEdgeScrollArea_);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(titleLayout);
	mainLayout->addLayout(lineAndEdgeLayout);

	setLayout(mainLayout);

	setMinimumWidth(220);

	// Initialize the view if the element is valid.
	if (element_)
		setElement(element_);
}

void AMSelectableElementView::addAbsorptionEdgeNameFilter(const QRegExp &newNameFilter)
{
	absorptionEdgeValidator_->addNameFilter(newNameFilter);
}

bool AMSelectableElementView::removeAbsorptionEdgeNameFilter(int index)
{
	return absorptionEdgeValidator_->removeNameFilter(index);
}

bool AMSelectableElementView::removeAbsorptionEdgeNameFilter(const QRegExp &filter)
{
	return absorptionEdgeValidator_->removeNameFilter(filter);
}

void AMSelectableElementView::addEmissionLineNameFilter(const QRegExp &newNameFilter)
{
	emissionLineValidator_->addNameFilter(newNameFilter);
}

bool AMSelectableElementView::removeEmissionLineNameFilter(int index)
{
	return emissionLineValidator_->removeNameFilter(index);
}

bool AMSelectableElementView::removeEmissionLineNameFilter(const QRegExp &filter)
{
	return emissionLineValidator_->removeNameFilter(filter);
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

void AMSelectableElementView::setEmissionLineVisibility(bool visible)
{
	emissionLineScrollArea_->setVisible(visible);

	if (visible)
		updateEmissionLineViewList();
}

void AMSelectableElementView::setAbsorptionEdgeVisibility(bool visible)
{
	absorptionEdgeScrollArea_->setVisible(visible);

	if (visible)
		updateAbsorptionEdgeViewList();
}

void AMSelectableElementView::setEnergyRange(const AMRange &newRange)
{
	absorptionEdgeValidator_->setRange(newRange);
	emissionLineValidator_->setRange(newRange);
}

void AMSelectableElementView::setEnergyRange(double minimum, double maximum)
{
	setEnergyRange(AMRange(minimum, maximum));
}

void AMSelectableElementView::setMinimumEnergy(double newMinimum)
{
	absorptionEdgeValidator_->setMinimum(newMinimum);
	emissionLineValidator_->setMinimum(newMinimum);
}

void AMSelectableElementView::setMaximumEnergy(double newMaximum)
{
	absorptionEdgeValidator_->setMaximum(newMaximum);
	emissionLineValidator_->setMaximum(newMaximum);
}

void AMSelectableElementView::updateAbsorptionEdgeViewList()
{
	QList<AMAbsorptionEdge> edges(element_->absorptionEdges());
	int viewIterator = 0;

	foreach (AMAbsorptionEdge edge, edges)
		if (absorptionEdgeValidator_->isValid(edge.name(), edge.energy()))
			absorptionEdgeViews_.at(viewIterator++)->setAbsorptionEdge(edge, element_->isSelected(edge));

	// Then hide the rest.
	for (int listSize = absorptionEdgeViews_.size(); viewIterator < listSize; viewIterator++)
		absorptionEdgeViews_.at(viewIterator)->setAbsorptionEdge(AMAbsorptionEdge());
}

void AMSelectableElementView::updateEmissionLineViewList()
{
	QList<AMEmissionLine> lines(element_->emissionLines());
	int viewIterator = 0;

	foreach (AMEmissionLine line, lines)
		if (emissionLineValidator_->isValid(line.name(), line.energy()))
			emissionLineViews_.at(viewIterator++)->setEmissionLine(line, element_->isSelected(line));

	// Then hide the rest.
	for (int listSize = emissionLineViews_.size(); viewIterator < listSize; viewIterator++)
		emissionLineViews_.at(viewIterator)->setEmissionLine(AMEmissionLine());
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
