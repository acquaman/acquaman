#include "AMRegionOfInterestView.h"

#include <QHBoxLayout>

// AMRegionOfInterestElementView
/////////////////////////////////////////////////////

 AMRegionOfInterestElementView::~AMRegionOfInterestElementView(){}
AMRegionOfInterestElementView::AMRegionOfInterestElementView(AMRegionOfInterest *region, QWidget *parent)
	: QWidget(parent)
{
	region_ = region;
	connect(region_, SIGNAL(destroyed()), this, SLOT(onRegionDestroyed()));

	QLabel *name = new QLabel(region_->name());
	name->setAlignment(Qt::AlignCenter);
	name->setFixedWidth(100);

	lowerBound_ = new QDoubleSpinBox;
	lowerBound_->setMinimum(0);
	lowerBound_->setMaximum(100000);
	lowerBound_->setAlignment(Qt::AlignCenter);
	lowerBound_->setSuffix(" eV");
	lowerBound_->setDecimals(0);
	lowerBound_->setValue(region_->lowerBound());
	lowerBound_->setFixedWidth(100);
	lowerBound_->setSingleStep(((AMAnalysisBlock *)region_->valueSource())->inputDataSourceAt(0)->axisInfoAt(0).increment);
	connect(lowerBound_, SIGNAL(valueChanged(double)), region_, SLOT(setLowerBound(double)));
	connect(region_, SIGNAL(lowerBoundChanged(double)), this, SLOT(updateLowerBound(double)));

	upperBound_ = new QDoubleSpinBox;
	upperBound_->setMinimum(0);
	upperBound_->setMaximum(100000);
	upperBound_->setAlignment(Qt::AlignCenter);
	upperBound_->setSuffix(" eV");
	upperBound_->setDecimals(0);
	upperBound_->setValue(region_->upperBound());
	upperBound_->setFixedWidth(100);
	upperBound_->setSingleStep(((AMAnalysisBlock *)region_->valueSource())->inputDataSourceAt(0)->axisInfoAt(0).increment);
	connect(upperBound_, SIGNAL(valueChanged(double)), region_, SLOT(setUpperBound(double)));
	connect(region_, SIGNAL(upperBoundChanged(double)), this, SLOT(updateUpperBound(double)));

	connect(((AMAnalysisBlock *)region_->valueSource())->inputDataSourceAt(0)->signalSource(), SIGNAL(axisInfoChanged(int)), this, SLOT(updateSpinBoxSingleStep()));

	value_ = new QLabel;
	value_->setFixedWidth(100);
	onValueChanged(region_->value());
	connect(region_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));

	QHBoxLayout *elementViewLayout = new QHBoxLayout;
	elementViewLayout->addWidget(name, 0, Qt::AlignCenter);
	elementViewLayout->addWidget(lowerBound_, 0, Qt::AlignCenter);
	elementViewLayout->addWidget(upperBound_, 0, Qt::AlignCenter);
	elementViewLayout->addWidget(value_, 0, Qt::AlignCenter);

	setLayout(elementViewLayout);
}

void AMRegionOfInterestElementView::onRegionDestroyed()
{
	if (region_ && region_->valueSource() && ((AMAnalysisBlock *)region_->valueSource())->inputDataSourceAt(0))
		disconnect(((AMAnalysisBlock *)region_->valueSource())->inputDataSourceAt(0)->signalSource(), SIGNAL(axisInfoChanged(int)), this, SLOT(updateSpinBoxSingleStep()));

	region_ = 0;
}

void AMRegionOfInterestElementView::onValueChanged(double value)
{
	value_->setText(QString("%1 counts").arg(int(value)));
}

void AMRegionOfInterestElementView::updateLowerBound(double value)
{
	if (value != lowerBound_->value()){

		lowerBound_->blockSignals(true);
		lowerBound_->setValue(int(value));
		lowerBound_->blockSignals(false);
	}
}

void AMRegionOfInterestElementView::updateUpperBound(double value)
{
	if (value != upperBound_->value()){

		upperBound_->blockSignals(true);
		upperBound_->setValue(int(value));
		upperBound_->blockSignals(false);
	}
}

void AMRegionOfInterestElementView::updateSpinBoxSingleStep()
{
	if( ((AMAnalysisBlock *)region_->valueSource())->inputDataSourceCount() > 0 && ((AMAnalysisBlock *)region_->valueSource())->inputDataSourceAt(0)->axes().count() > 0){

		lowerBound_->setSingleStep(((AMAnalysisBlock *)region_->valueSource())->inputDataSourceAt(0)->axisInfoAt(0).increment);
		upperBound_->setSingleStep(((AMAnalysisBlock *)region_->valueSource())->inputDataSourceAt(0)->axisInfoAt(0).increment);
	}
}

// AMRegionOfInterestView
//////////////////////////////////////////////////////

 AMRegionOfInterestView::~AMRegionOfInterestView(){}
AMRegionOfInterestView::AMRegionOfInterestView(QList<AMRegionOfInterest *> regions, QWidget *parent)
	: QWidget(parent)
{
	regionOfInterestViewLayout_ = new QVBoxLayout;

	QHBoxLayout *topRow = new QHBoxLayout;
	topRow->addWidget(new QLabel("Name"), 0, Qt::AlignCenter);
	topRow->addWidget(new QLabel("Lower Bound"), 0, Qt::AlignCenter);
	topRow->addWidget(new QLabel("Upper Bound"), 0, Qt::AlignCenter);
	topRow->addWidget(new QLabel("Value"), 0, Qt::AlignCenter);

	regionOfInterestViewLayout_->addLayout(topRow);

	foreach (AMRegionOfInterest *region, regions){

		AMRegionOfInterestElementView *view = new AMRegionOfInterestElementView(region);
		regionsAndViews_.insert(region, view);
		regionOfInterestViewLayout_->addWidget(view);
	}

	setLayout(regionOfInterestViewLayout_);
}

void AMRegionOfInterestView::addRegionOfInterest(AMRegionOfInterest *newRegion)
{
	AMRegionOfInterestElementView *view = new AMRegionOfInterestElementView(newRegion);
	regionsAndViews_.insert(newRegion, view);
	regionOfInterestViewLayout_->addWidget(view);
}

void AMRegionOfInterestView::removeRegionOfInterest(AMRegionOfInterest *region)
{
	regionOfInterestViewLayout_->removeWidget(regionsAndViews_.value(region));
	AMRegionOfInterestElementView *view = regionsAndViews_.take(region);

	if (view)
		delete view;
}
