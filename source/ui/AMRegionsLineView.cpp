/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMRegionsLineView.h"

RegionItem::RegionItem(double start, double delta, double end, double min, double max, int pixRange) : color(qrand() % 256, qrand() % 256, qrand() % 256)
{
	setToolTip(QString("QColor(%1, %2, %3)\n%4")
			  .arg(color.red()).arg(color.green()).arg(color.blue())
			  .arg("Click and drag this color onto the robot!"));
	setCursor(Qt::OpenHandCursor);
	start_ = start;
	delta_ = delta;
	end_ = end;
	min_ = min;
	max_ = max;
	pixRange_ = pixRange;
	width_ = (int)floor( (end-start)/((max-min)/pixRange) );
}

QRectF RegionItem::boundingRect() const{
	return QRectF(0, -15, width_, 30);
}

void RegionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(option);
	Q_UNUSED(widget);
	painter->setPen(Qt::NoPen);
	painter->setBrush(Qt::darkGray);
	painter->drawRoundedRect(0, -15, width_, 30, 1, 1, Qt::RelativeSize);
	painter->setPen(QPen(Qt::black, 1));
	painter->setBrush(Qt::gray);
	painter->drawRoundedRect(0, -15, width_, 30, 1, 1, Qt::RelativeSize);
	QString deltaVal;
	deltaVal.setNum(delta_);
	QChar deltaChar(0x0394);
	deltaVal.prepend(" = ");
	deltaVal.prepend(deltaChar);
	QRectF box(0, -15, width_, 30);
	painter->drawText(box, Qt::AlignHCenter, deltaVal, &box);
}

void RegionItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
	if (event->button() != Qt::LeftButton) {
		event->ignore();
		return;
	}

	setCursor(Qt::ClosedHandCursor);
}

void RegionItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
	if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance()) {
		return;
	}

	QDrag *drag = new QDrag(event->widget());
	QMimeData *mime = new QMimeData;
	drag->setMimeData(mime);


	mime->setColorData(color);
	mime->setText(QString("#%1%2%3")
				  .arg(color.red(), 2, 16, QLatin1Char('0'))
				  .arg(color.green(), 2, 16, QLatin1Char('0'))
				  .arg(color.blue(), 2, 16, QLatin1Char('0')));

	QPixmap pixmap(60, 40);
	pixmap.fill(Qt::white);

	QPainter painter(&pixmap);
	painter.translate(26, 16);
	painter.setRenderHint(QPainter::Antialiasing);
	paint(&painter, 0, 0);
	painter.end();
	pixmap.setMask(pixmap.createHeuristicMask());

	drag->setPixmap(pixmap);
	drag->setHotSpot(QPoint(15, 20));


	drag->exec();
	setCursor(Qt::OpenHandCursor);
}

void RegionItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
	Q_UNUSED(event)

	setCursor(Qt::OpenHandCursor);
}


EnergyIndexItem::EnergyIndexItem(double energy, double min, double max, int pixRange) : color(qrand() % 256, qrand() % 256, qrand() % 256)
{
	setToolTip(QString("QColor(%1, %2, %3)\n%4")
			  .arg(color.red()).arg(color.green()).arg(color.blue())
			  .arg("Click and drag this color onto the robot!"));
	setCursor(Qt::OpenHandCursor);
	energy_ = energy;
	min_ = min;
	max_ = max;
	pixRange_ = pixRange;
	width_ = 33;
	textBox_ = new QRectF(0, -15, width_, 30);
}

QRectF EnergyIndexItem::boundingRect() const{
	return QRectF(textBox_->x(), textBox_->y(), textBox_->width(), textBox_->height());
}

void EnergyIndexItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(option);
	Q_UNUSED(widget);
	painter->setPen(Qt::NoPen);
	painter->setPen(QPen(Qt::black, 1));
	painter->setBrush(Qt::gray);
	QString energyVal;
	QRectF box(boundingRect());
	painter->drawText(box, Qt::AlignHCenter, energyVal.setNum(energy_), textBox_);
}

AMRegionsLineView::AMRegionsLineView(AMXASRegionsList *regions, QWidget *parent) : QWidget(parent)
{
	regions_ = regions;
	int nlSize = 800;
	setFixedWidth(nlSize);
	setFixedHeight(62);
	double range = regions_->maxEnergy() - regions_->minEnergy();
	double ratio = range/(nlSize-60);
	scene = new QGraphicsScene(0, -10, nlSize-50, 40);

	redrawRegionsLine();
	view = new QGraphicsView(scene);
	view->setFrameStyle(QFrame::NoFrame);
	view->setFrameShadow(QFrame::Plain);
	view->setRenderHint(QPainter::Antialiasing);
	view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	view->setBackgroundBrush(palette().color(QPalette::Window));
	view->resize(nlSize-50, 40);
	view->setMaximumHeight(60);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(view);
	vl->setContentsMargins(0,0,0,0);
	setLayout(vl);
	connect(regions_->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(handleDataChanged(QModelIndex,QModelIndex)));
	connect(regions_->model(), SIGNAL(rowsInserted(const QModelIndex,int,int)), this, SLOT(handleRowsInsert(QModelIndex,int,int)));
	connect(regions_->model(), SIGNAL(rowsRemoved(const QModelIndex,int,int)), this, SLOT(handleRowsRemoved(QModelIndex,int,int)));
}

void AMRegionsLineView::handleDataChanged(QModelIndex topLeft, QModelIndex bottomRight){
	Q_UNUSED(bottomRight)
	if(topLeft.column() == 1 || topLeft.column() == 2 || topLeft.column() == 3){
		redrawRegionsLine();
	}
}

void AMRegionsLineView::handleRowsInsert(const QModelIndex &parent, int start, int end){
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)
	redrawRegionsLine();
}

void AMRegionsLineView::handleRowsRemoved(const QModelIndex &parent, int start, int end){
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)
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
