#include "AMXASRegionsView.h"

class RegionItem : public QGraphicsItem
{
public:
	RegionItem(double start, double delta, double end, double min, double max, int pixRange) : color(qrand() % 256, qrand() % 256, qrand() % 256)
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

	QRectF boundingRect() const{
		return QRectF(0, -15, width_, 30);
	}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
		Q_UNUSED(option);
		Q_UNUSED(widget);
		painter->setPen(Qt::NoPen);
		painter->setBrush(Qt::darkGray);
//		painter->drawEllipse(-12, -12, 30, 30);
//		painter->drawRoundedRect(-(int)(0.5*width_), -15, width_, 30, 20, 20, Qt::RelativeSize);
		painter->drawRoundedRect(0, -15, width_, 30, 1, 1, Qt::RelativeSize);
		painter->setPen(QPen(Qt::black, 1));
//		painter->setBrush(QBrush(color));
		painter->setBrush(Qt::gray);
//		painter->drawEllipse(-15, -15, 30, 30);
		painter->drawRoundedRect(0, -15, width_, 30, 1, 1, Qt::RelativeSize);
		QString deltaVal;
		deltaVal.setNum(delta_);
		QChar deltaChar(0x0394);
		deltaVal.prepend(" = ");
		deltaVal.prepend(deltaChar);
		QRectF box(0, -15, width_, 30);
		painter->drawText(box, Qt::AlignHCenter, deltaVal, &box);
//		painter->drawText(width_/2, 0, deltaVal);
	}

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event){
		if (event->button() != Qt::LeftButton) {
			event->ignore();
			return;
		}

		setCursor(Qt::ClosedHandCursor);
	}
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event){
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
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
		setCursor(Qt::OpenHandCursor);
	}

private:
	QColor color;
	double start_;
	double delta_;
	double end_;
	double min_;
	double max_;
	int pixRange_;
	int width_;
};

class EnergyIndexItem : public QGraphicsItem
{
public:
	EnergyIndexItem(double energy, double min, double max, int pixRange) : color(qrand() % 256, qrand() % 256, qrand() % 256)
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
//		width_ = (int)floor( (end-start)/((max-min)/pixRange) );
	}

	QRectF boundingRect() const{
		return QRectF(textBox_->x(), textBox_->y(), textBox_->width(), textBox_->height());
	}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
		Q_UNUSED(option);
		Q_UNUSED(widget);
		painter->setPen(Qt::NoPen);
//		painter->setBrush(Qt::darkGray);
//		painter->drawRoundedRect(0, -15, width_, 30, 1, 1, Qt::RelativeSize);
		painter->setPen(QPen(Qt::black, 1));
		painter->setBrush(Qt::gray);
//		painter->drawRoundedRect(0, -15, width_, 30, 1, 1, Qt::RelativeSize);
		QString energyVal;
//		painter->drawText(0, 0, energyVal.setNum(energy_));
		QRectF box(boundingRect());
		qDebug() << "Before drawing " << textBox_->width();
		painter->drawText(box, Qt::AlignHCenter, energyVal.setNum(energy_), textBox_);
		qDebug() << "After drawing " << textBox_->height() << " vs " << boundingRect().width();
		qDebug() << "Energy is " << energyVal << " width is " << boundingRect().width();
	}

private:
	QColor color;
	QRectF *textBox_;
	double energy_;
	double min_;
	double max_;
	int pixRange_;
	int width_;
};

/// Creates buttons for add and delete region.
/// Defines a new model and views using table view.
/// Adds all items to form layout.
/// \todo Connect add and delete to something.
AMXASRegionsView::AMXASRegionsView(AMXASRegionsList *regions, QWidget *parent) :
	QWidget(parent)
{
	this->setMaximumSize(800, 300);
	this->setMinimumSize(800, 300);
	addButton_ = new QPushButton("Add Region", this);
	deleteButton_ = new QPushButton("Delete Region", this);
	QHBoxLayout *hl_ = new QHBoxLayout();
	hl_->addWidget(addButton_);
	hl_->addWidget(deleteButton_);
	regions_ = regions;
	tv_ = new QTableView(this);
	tv_->setModel(regions_->model());
	tv_->hideColumn(0);
	tv_->hideColumn(4);
	tv_->hideColumn(5);
	tv_->setMaximumWidth(400);
	tv_->resize(tv_->sizeHint());
	tv_->horizontalHeader()->setStretchLastSection(true);
	int totalWidth = tv_->columnWidth(1) + tv_->columnWidth(2) + tv_->columnWidth(3);
	tv_->setColumnWidth(1, 2*totalWidth/5);
	tv_->setColumnWidth(3, 2*totalWidth/5);
	tv_->setColumnWidth(2, totalWidth-tv_->columnWidth(1)-tv_->columnWidth(3));

	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	double range = regions_->maxEnergy() - regions_->minEnergy();
	double ratio = range/740;
	scene = new QGraphicsScene(0, -10, 750, 40);
	for (int i = 0; i < regions_->count(); ++i) {
		qDebug() << "Width will be " << (int)floor((regions_->end(i)-regions_->start(i))/ratio) << " pos will be " << 5+(int)floor((regions_->start(i)-regions_->minEnergy())/ratio);
		RegionItem *item = new RegionItem(regions_->start(i), regions_->delta(i), regions_->end(i), regions_->minEnergy(), regions_->maxEnergy(), 740);
		item->setPos( 10+(int)floor((regions_->start(i)-regions_->minEnergy())/ratio), 0);
		scene->addItem(item);
		EnergyIndexItem *eItem = new EnergyIndexItem(regions_->start(i), regions_->minEnergy(), regions_->maxEnergy(), 740);
		eItem->setPos(10+(int)floor((regions_->start(i)-regions_->minEnergy())/ratio), 30);
		scene->addItem(eItem);
	}
	EnergyIndexItem *eItem = new EnergyIndexItem(regions_->end(regions_->count()-1), regions_->minEnergy(), regions_->maxEnergy(), 740);
	qDebug() << "Want to offset by width of eItem? " << eItem->boundingRect().width();
	eItem->setPos(10+(int)floor((regions_->end(regions_->count()-1)-regions_->minEnergy())/ratio) - eItem->boundingRect().width(), 30);
	scene->addItem(eItem);

	qDebug() << "Min is " << regions_->minEnergy() << " Max is " << regions_->maxEnergy();
	view = new QGraphicsView(scene);
	view->setRenderHint(QPainter::Antialiasing);
	view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	view->setBackgroundBrush(QColor(230, 200, 167));
	view->setWindowTitle("Drag and Drop Dave Test");
	view->resize(750, 40);
	view->setMaximumHeight(60);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QFormLayout *fl_ = new QFormLayout(this);
	fl_->addRow(view);
	fl_->addRow(tv_);
	fl_->addRow(hl_);
	setLayout(fl_);
	addRegionMenu_= NULL;
	deleteRegionMenu_ = NULL;
//	connect(addButton_, SIGNAL(clicked()), this, SIGNAL(addRegionClicked()));
	connect(addButton_, SIGNAL(clicked()), this, SLOT(addRegion()));
	connect(deleteButton_, SIGNAL(clicked()), this, SLOT(deleteRegion()));
	connect(regions_->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(redrawRegionsLine(QModelIndex,QModelIndex)));
}

AMXASRegionsView::~AMXASRegionsView(){
	disconnect(addButton_, SIGNAL(clicked()), this, SIGNAL(addRegionClicked()));
	disconnect(deleteButton_, SIGNAL(clicked()), this, SLOT(deleteRegion()));
	regions_ = NULL;
}

void AMXASRegionsView::redrawRegionsLine(QModelIndex topLeft, QModelIndex bottomRight){
	qDebug() << "What did I just get? topLeft.row " << topLeft.row() << " topLeft.column " << topLeft.column() << " bottomRight.row " << bottomRight.row() << " bottomRight.column " << bottomRight.column();
	if(topLeft.column() == 1 || topLeft.column() == 2 || topLeft.column() == 3){
		scene->clear();
		double range = regions_->maxEnergy() - regions_->minEnergy();
		double ratio = range/740;
		for (int i = 0; i < regions_->count(); ++i) {
			qDebug() << "Width will be " << (int)floor((regions_->end(i)-regions_->start(i))/ratio) << " pos will be " << 5+(int)floor((regions_->start(i)-regions_->minEnergy())/ratio);
//			RegionItem *item = new RegionItem( (int)floor((regions_->end(i)-regions_->start(i))/ratio) );
			RegionItem *item = new RegionItem(regions_->start(i), regions_->delta(i), regions_->end(i), regions_->minEnergy(), regions_->maxEnergy(), 740);
			item->setPos( 10+(int)floor((regions_->start(i)-regions_->minEnergy())/ratio), 0);
			scene->addItem(item);
			EnergyIndexItem *eItem = new EnergyIndexItem(regions_->start(i), regions_->minEnergy(), regions_->maxEnergy(), 740);
			eItem->setPos(10+(int)floor((regions_->start(i)-regions_->minEnergy())/ratio), 30);
			scene->addItem(eItem);
		}
		EnergyIndexItem *eItem = new EnergyIndexItem(regions_->end(regions_->count()-1), regions_->minEnergy(), regions_->maxEnergy(), 740);
		qDebug() << "Want to offset by width of eItem? " << eItem->boundingRect().width();
		eItem->setPos(10+(int)floor((regions_->end(regions_->count()-1)-regions_->minEnergy())/ratio) - eItem->boundingRect().width(), 30);
		scene->addItem(eItem);
	}
}
