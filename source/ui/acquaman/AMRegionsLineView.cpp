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


#include "AMRegionsLineView.h"

 AMRegionItem::~AMRegionItem(){}
AMRegionItem::AMRegionItem(double start, double delta, double end, double min, double max, int pixRange, const QString &units) : color(qrand() % 256, qrand() % 256, qrand() % 256)
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
	units_ = units;
}

QRectF AMRegionItem::boundingRect() const{
	return QRectF(0, -15, width_, 30);
}

void AMRegionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
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
	deltaVal.append(units_);
	QRectF box(0, -15, width_, 30);
	painter->drawText(box, Qt::AlignHCenter, deltaVal, &box);
}

void AMRegionItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
	if (event->button() != Qt::LeftButton) {
		event->ignore();
		return;
	}

	setCursor(Qt::ClosedHandCursor);
}

void AMRegionItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
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

void AMRegionItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
	Q_UNUSED(event)

	setCursor(Qt::OpenHandCursor);
}


 AMEnergyIndexItem::~AMEnergyIndexItem(){}
AMEnergyIndexItem::AMEnergyIndexItem(double energy, double min, double max, int pixRange, const QString &units) : color(qrand() % 256, qrand() % 256, qrand() % 256)
{
	setToolTip(QString("QColor(%1, %2, %3)\n%4")
			  .arg(color.red()).arg(color.green()).arg(color.blue())
			  .arg("Click and drag this color onto the robot!"));
	setCursor(Qt::OpenHandCursor);
	energy_ = energy;
	min_ = min;
	max_ = max;
	pixRange_ = pixRange;
	width_ = 60;
	textBox_ = new QRectF(0, -15, width_, 30);
	units_ = units;
}

QRectF AMEnergyIndexItem::boundingRect() const{
	return QRectF(textBox_->x(), textBox_->y(), textBox_->width(), textBox_->height());
}

void AMEnergyIndexItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(option);
	Q_UNUSED(widget);
	painter->setPen(Qt::NoPen);
	painter->setPen(QPen(Qt::black, 1));
	painter->setBrush(Qt::gray);
	QString energyVal;
	QRectF box(boundingRect());
	painter->drawText(box, Qt::AlignHCenter, energyVal.setNum(energy_) + units_, textBox_);
}

 AMRegionsLineView::~AMRegionsLineView(){}
AMRegionsLineView::AMRegionsLineView(AMRegionsList *regions, QWidget *parent) : QWidget(parent)
{
	regions_ = regions;
	int nlSize = 800;
	setFixedWidth(nlSize);
	setFixedHeight(62);
	//double range = regions_->maxEnergy() - regions_->minEnergy();
	//double ratio = range/(nlSize-60);
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
	if(!regions_)
		return;

	scene->clear();

	if(regions_->count() == 0){
		QRectF rect(scene->itemsBoundingRect());
		scene->addRect(rect, QPen(Qt::black), QBrush(Qt::red));
		QFont font(this->font());
		font.setBold(true);
		font.setPointSize(24);
		QGraphicsTextItem *text = scene->addText("No Regions", font);
		text->setPos(10, 0);
		return;
	}

	int nlSize = 800;
	double range = regions_->maximumValue() - regions_->minimumValue();
	double ratio = range/(nlSize-60);

	if (regions_->isValid()){

		for (int i = 0; i < regions_->count(); ++i) {

			AMRegionItem *item = new AMRegionItem(regions_->start(i), regions_->delta(i), regions_->end(i), regions_->minimumValue(), regions_->maximumValue(), nlSize-60, regions_->units(i));
			item->setPos( 10+(int)floor((regions_->start(i)-regions_->minimumValue())/ratio), 0);
			scene->addItem(item);
			AMEnergyIndexItem *eItem = new AMEnergyIndexItem(regions_->start(i), regions_->minimumValue(), regions_->maximumValue(), nlSize-60, regions_->units(i));
			eItem->setPos(10+(int)floor((regions_->start(i)-regions_->minimumValue())/ratio), 30);
			scene->addItem(eItem);
		}
		AMEnergyIndexItem *eItem = new AMEnergyIndexItem(regions_->end(regions_->count()-1), regions_->minimumValue(), regions_->maximumValue(), nlSize-60, regions_->units(regions_->count()-1));
		eItem->setPos(10+(int)floor((regions_->end(regions_->count()-1)-regions_->minimumValue())/ratio) - eItem->boundingRect().width(), 30);
		scene->addItem(eItem);
	}

	else{

		for (int i = 0; i < regions_->count(); ++i) {
			AMRegionItem *item = new AMRegionItem(regions_->start(i), regions_->delta(i), regions_->end(i), regions_->minimumValue(), regions_->maximumValue(), nlSize-60, regions_->units(i));
			item->setPos( 10+(int)floor((regions_->start(i)-regions_->minimumValue())/ratio), 0);
			scene->addItem(item);
		}

		QRectF rect(scene->itemsBoundingRect());
		scene->addRect(rect, QPen(Qt::black), QBrush(Qt::red));
		QFont font(this->font());
		font.setBold(true);
		font.setPointSize(12);
		QGraphicsTextItem *text = scene->addText("Regions Are Invalid!", font);
		text->moveBy(rect.width()/2 - text->sceneBoundingRect().width()/2, -text->sceneBoundingRect().height()/2);
	}
}

// AMEXAFSLinesView
////////////////////////////////////////

 AMEXAFSLineView::~AMEXAFSLineView(){}
AMEXAFSLineView::AMEXAFSLineView(AMEXAFSRegionsList *regions, QWidget *parent)
	: AMRegionsLineView(regions, parent)
{
	redrawRegionsLine();
	connect(regions, SIGNAL(regionsHaveKSpaceChanged(bool)), this, SLOT(redrawRegionsLine()));
}

void AMEXAFSLineView::redrawRegionsLine()
{
	// This method needs a valid AMEXAFSRegions.  If we got a bad one then that would be awkward.
	AMEXAFSRegionsList *regions = qobject_cast<AMEXAFSRegionsList *>(regions_);
	if (!regions)
		return;

	int nlSize = 800;
	scene->clear();
	double range = regions->maximumValue() - regions->minimumValue();
	double ratio = range/(nlSize-60);

	if (regions->isValid()){

		for (int i = 0; i < regions->count(); ++i) {

			AMRegionItem *item = new AMRegionItem(regions->startByType(i, AMEXAFSRegion::Energy), regions->delta(i), regions->endByType(i, AMEXAFSRegion::Energy), regions->minimumValue(), regions->maximumValue(), nlSize-60, regions->units(i));
			item->setPos( (int)floor((regions->startByType(i, AMEXAFSRegion::Energy)-regions->minimumValue())/ratio), 0);
			scene->addItem(item);

			if (i > 0 && regions->type(i) == AMEXAFSRegion::kSpace && regions->type(i-1) == AMEXAFSRegion::Energy){

				AMEnergyIndexItem *eItem = new AMEnergyIndexItem(regions->end(i-1), regions->minimumValue(), regions->maximumValue(), nlSize-60, regions->units(i-1));
				eItem->setPos((int)floor((regions->endByType(i-1, AMEXAFSRegion::Energy)-regions->minimumValue())/ratio), 30);
				scene->addItem(eItem);
			}
			else{

				AMEnergyIndexItem *eItem = new AMEnergyIndexItem(regions->start(i), regions->minimumValue(), regions->maximumValue(), nlSize-60, regions->units(i));
				eItem->setPos((int)floor((regions->startByType(i, AMEXAFSRegion::Energy)-regions->minimumValue())/ratio), 30);
				scene->addItem(eItem);
			}
		}
		AMEnergyIndexItem *eItem = new AMEnergyIndexItem(regions->end(regions->count()-1), regions->minimumValue(), regions->maximumValue(), nlSize-60, regions->units(regions->count()-1));
		eItem->setPos((int)floor((regions->endByType(regions->count()-1, AMEXAFSRegion::Energy)-regions->minimumValue())/ratio) - eItem->boundingRect().width(), 30);
		scene->addItem(eItem);
	}

	else{

		for (int i = 0; i < regions->count(); ++i) {
			AMRegionItem *item = new AMRegionItem(regions->startByType(i, AMEXAFSRegion::Energy), regions->delta(i), regions->endByType(i, AMEXAFSRegion::Energy), regions->minimumValue(), regions->maximumValue(), nlSize-60, regions->units(i));
			item->setPos( (int)floor((regions->startByType(i, AMEXAFSRegion::Energy)-regions->minimumValue())/ratio), 0);
			scene->addItem(item);
		}

		QRectF rect(scene->itemsBoundingRect());
		scene->addRect(rect, QPen(Qt::black), QBrush(Qt::red));
		QFont font(this->font());
		font.setBold(true);
		font.setPointSize(12);
		QGraphicsTextItem *text = scene->addText("Regions Are Invalid!", font);
		text->moveBy(rect.width()/2 - text->sceneBoundingRect().width()/2, -text->sceneBoundingRect().height()/2);
	}
}
