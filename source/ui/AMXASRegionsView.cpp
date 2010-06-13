#include "AMXASRegionsView.h"
#include <QtGui>
#include <QGraphicsItem>


class RegionItem : public QGraphicsItem
{
public:
	RegionItem(int width) : color(qrand() % 256, qrand() % 256, qrand() % 256)
	{
		setToolTip(QString("QColor(%1, %2, %3)\n%4")
				  .arg(color.red()).arg(color.green()).arg(color.blue())
				  .arg("Click and drag this color onto the robot!"));
		setCursor(Qt::OpenHandCursor);
		width_ = width;
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
		painter->setBrush(QBrush(color));
//		painter->drawEllipse(-15, -15, 30, 30);
		painter->drawRoundedRect(0, -15, width_, 30, 1, 1, Qt::RelativeSize);
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
	tv_->resize(tv_->sizeHint());

	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	double range = regions_->maxEnergy() - regions_->minEnergy();
	double ratio = range/740;
	QGraphicsScene *scene = new QGraphicsScene(0, -10, 750, 20);
	for (int i = 0; i < regions_->count(); ++i) {
		qDebug() << "Width will be " << (int)floor((regions_->end(i)-regions_->start(i))/ratio) << " pos will be " << 5+(int)floor((regions_->start(i)-regions_->minEnergy())/ratio);
		RegionItem *item = new RegionItem( (int)floor((regions_->end(i)-regions_->start(i))/ratio) );
		item->setPos( 10+(int)floor((regions_->start(i)-regions_->minEnergy())/ratio), 0);
		scene->addItem(item);
	}

	qDebug() << "Min is " << regions_->minEnergy() << " Max is " << regions_->maxEnergy();
	QGraphicsView *view = new QGraphicsView(scene);
	view->setRenderHint(QPainter::Antialiasing);
	view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	view->setBackgroundBrush(QColor(230, 200, 167));
	view->setWindowTitle("Drag and Drop Dave Test");
	view->resize(750, 20);
	view->setMaximumHeight(40);
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
}

AMXASRegionsView::~AMXASRegionsView(){
	disconnect(addButton_, SIGNAL(clicked()), this, SIGNAL(addRegionClicked()));
	disconnect(deleteButton_, SIGNAL(clicked()), this, SLOT(deleteRegion()));
	regions_ = NULL;
}
