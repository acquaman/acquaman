#include "AMRegionsLineView.h"

AMRegionsLineView::AMRegionsLineView(AMXASRegionsList *regions, QWidget *parent) : QWidget(parent)
{
	regions_ = regions;
	int nlSize = 800;
	this->setMaximumSize(nlSize, 100);
	this->setMinimumSize(nlSize, 100);
	double range = regions_->maxEnergy() - regions_->minEnergy();
	double ratio = range/(nlSize-60);
	scene = new QGraphicsScene(0, -10, nlSize-50, 40);

	redrawRegionsLine();
	view = new QGraphicsView(scene);
	view->setRenderHint(QPainter::Antialiasing);
	view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	view->setBackgroundBrush(QColor(230, 200, 167));
	view->setWindowTitle("Drag and Drop Dave Test");
	view->resize(nlSize-50, 40);
	view->setMaximumHeight(60);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QFormLayout *fl_ = new QFormLayout(this);
	fl_->addRow(view);
	setLayout(fl_);
	connect(regions_->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(handleDataChanged(QModelIndex,QModelIndex)));
	connect(regions_->model(), SIGNAL(rowsInserted(const QModelIndex,int,int)), this, SLOT(handleRowsInsert(QModelIndex,int,int)));
	connect(regions_->model(), SIGNAL(rowsRemoved(const QModelIndex,int,int)), this, SLOT(handleRowsRemoved(QModelIndex,int,int)));
}

void AMRegionsLineView::handleDataChanged(QModelIndex topLeft, QModelIndex bottomRight){
	if(topLeft.column() == 1 || topLeft.column() == 2 || topLeft.column() == 3){
		redrawRegionsLine();
	}
}

void AMRegionsLineView::handleRowsInsert(const QModelIndex &parent, int start, int end){
	redrawRegionsLine();
}

void AMRegionsLineView::handleRowsRemoved(const QModelIndex &parent, int start, int end){
	redrawRegionsLine();
}

void AMRegionsLineView::redrawRegionsLine(){
	int nlSize = 800;
	scene->clear();
	double range = regions_->maxEnergy() - regions_->minEnergy();
	double ratio = range/(nlSize-60);
	for (int i = 0; i < regions_->count(); ++i) {
		RegionItem *item = new RegionItem(regions_->start(i), regions_->delta(i), regions_->end(i), regions_->minEnergy(), regions_->maxEnergy(), nlSize-60);
		item->setPos( 10+(int)floor((regions_->start(i)-regions_->minEnergy())/ratio), 0);
		scene->addItem(item);
		EnergyIndexItem *eItem = new EnergyIndexItem(regions_->start(i), regions_->minEnergy(), regions_->maxEnergy(), nlSize-60);
		eItem->setPos(10+(int)floor((regions_->start(i)-regions_->minEnergy())/ratio), 30);
		scene->addItem(eItem);
	}
	EnergyIndexItem *eItem = new EnergyIndexItem(regions_->end(regions_->count()-1), regions_->minEnergy(), regions_->maxEnergy(), nlSize-60);
	eItem->setPos(10+(int)floor((regions_->end(regions_->count()-1)-regions_->minEnergy())/ratio) - eItem->boundingRect().width(), 30);
	scene->addItem(eItem);
}
