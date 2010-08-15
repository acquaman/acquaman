#include "AMThumbnailScrollViewer.h"
#include "dataman/AMDatabaseDefinition.h"

#include <QVBoxLayout>

AMThumbnailScrollViewer::AMThumbnailScrollViewer(QWidget *parent) :
		QLabel(parent)
{

	sourceIsDb_ = true;
	sourceDb_ = 0;
	sourceObject_ = 0;

	setScaledContents(true);
	QSizePolicy p(QSizePolicy::Fixed, QSizePolicy::Fixed);
	p.setHeightForWidth(true);
	setSizePolicy(p);

	title_ = new QLabel("Title");
	title_->setAlignment(Qt::AlignRight);
	title_->setStyleSheet("color: rgb(25,25,25); font: 12pt bold \"Lucida Grande\";");
	subtitle_ = new QLabel("Subtitle");
	subtitle_->setAlignment(Qt::AlignRight);
	subtitle_->setStyleSheet("color: rgb(168,168,168); font: 12pt italic \"Lucida Grande\";");

	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(title_);
	vl->addWidget(subtitle_);
	vl->addStretch(1);
	setLayout(vl);

	setMouseTracking(true);
}

/// display the thumbnails from a given AMDbObject
void AMThumbnailScrollViewer::setSource(AMDbObject* source) {
	tIndex_ = 0;

	sourceIsDb_ = false;
	sourceObject_ = source;

	if(sourceObject_ && sourceObject_->thumbnailCount() > 0)
		displayThumbnail(sourceObject_->thumbnail(0));
	else
		displayThumbnail(AMDbThumbnail());
}


/// Display thumbnails directly out of a database (from the thumbnail table, with id's given in \c ids)
void AMThumbnailScrollViewer::setSource(AMDatabase* db, QList<int> ids) {
	tIndex_ = 0;

	sourceIsDb_ = true;
	sourceDb_ = db;
	sourceIds_ = ids;

	if(sourceDb_ && sourceIds_.count() > 0)
		displayThumbnail(sourceDb_, sourceIds_.at(0));
	else {
		displayThumbnail(AMDbThumbnail());
	}
}

/// Display \c count thumbnails directly out of a database (from the thumbnail table, with sequential id's starting at \c startId)
void AMThumbnailScrollViewer::setSource(AMDatabase* db, int startId, int count) {
	tIndex_ = 0;

	sourceIsDb_ = true;
	sourceDb_ = db;

	/// \todo: can optimize this for speed
	sourceIds_.clear();
	for(int i=startId; i<startId+count; i++)
		sourceIds_ << i;
}


#include <QPainter>
#include <QGradient>
#include <QGradientStops>

QPixmap AMThumbnailScrollViewer::invalidPixmap() {
	QPixmap p(240,180);
	QPainter painter(&p);
	QRadialGradient g(QPointF(120, 50), 120, QPointF(120,50));
	g.setColorAt(0, Qt::white);
	g.setColorAt(1, QColor::fromRgb(223,230,237));

	painter.setBrush(QBrush(g));
	painter.setPen(QColor::fromRgb(167,167,167));
	painter.drawRect(0,0,239,179);
	painter.setFont(QFont("Lucida Grande", 128, QFont::DemiBold));
	painter.drawText(QRectF(0,0,240,180), Qt::AlignHCenter | Qt::AlignVCenter, "?");
	painter.end();
	return p;
}

void AMThumbnailScrollViewer::displayThumbnail(AMDbThumbnail t) {
	title_->setText(t.title);
	subtitle_->setText(t.subtitle);
	if(t.type == AMDbThumbnail::PNGType) {
		QPixmap p;
		p.loadFromData(t.thumbnail, "PNG");
		setPixmap(p);
	}
	else {
		setPixmap(invalidPixmap());
	}
}

void AMThumbnailScrollViewer::displayThumbnail(AMDatabase* db, int id) {

	if(db == 0) {
		title_->setText(QString());
		subtitle_->setText(QString());
		setPixmap(invalidPixmap());
		return;
	}

	QSqlQuery q = db->query();
	q.prepare(QString("SELECT title, subtitle, type, thumbnail FROM %1 WHERE id = ?").arg(AMDatabaseDefinition::thumbnailTableName()));
	q.bindValue(0, id);
	if(q.exec() && q.first()) {
		title_->setText(q.value(0).toString());
		subtitle_->setText(q.value(1).toString());
		if(q.value(2).toString() == QString("PNG")) {
			QPixmap p;
			p.loadFromData(q.value(3).toByteArray(), "PNG");
			setPixmap(p);

		}
		else {
			setPixmap(invalidPixmap());
		}
	}
}

#include <QMouseEvent>

void AMThumbnailScrollViewer::mouseMoveEvent ( QMouseEvent * event ) {

	double ratio = event->posF().x() / width();

	if(sourceIsDb_ && sourceDb_ && !sourceIds_.isEmpty()) {
		int index = sourceIds_.count()*ratio;
		if(index == sourceIds_.count())
			index--;
		if(index != tIndex_) {
			tIndex_ = index;
			displayThumbnail(sourceDb_, sourceIds_.at(tIndex_));
			update();
		}
	}

	if(!sourceIsDb_ && sourceObject_) {
		int thumbnailCount = sourceObject_->thumbnailCount();
		int index = thumbnailCount*ratio;
		if(index == thumbnailCount)
			index--;
		if(index != tIndex_) {
			tIndex_ = index;
			displayThumbnail(sourceObject_->thumbnail(tIndex_));
			update();
		}
	}

	QLabel::mouseMoveEvent(event);
}


AMThumbnailScrollWidget::AMThumbnailScrollWidget(const QString& caption1, const QString& caption2, QWidget* parent)
	: QWidget(parent) {
	tv_ = new AMThumbnailScrollViewer();
	c1_ = new QLabel(caption1);
	c1_->setAlignment(Qt::AlignCenter);
	c1_->setStyleSheet("font: bold 12pt \"Lucida Grande\"; color: rgb(25, 25, 25);");
	c1_->setWordWrap(true);
	c2_ = new QLabel(caption2);
	c2_->setAlignment(Qt::AlignHCenter);
	c2_->setStyleSheet("font: italic 12pt \"Lucida Grande\"; color: rgb(127, 127, 127);");
	c2_->setWordWrap(true);

	QVBoxLayout* vl = new QVBoxLayout();
	vl->setSpacing(0);
	vl->setContentsMargins(0,0,0,0);
	vl->addWidget(tv_);
	vl->addWidget(c1_);
	vl->addWidget(c2_);
	setLayout(vl);
}


AMThumbnailScrollGraphicsWidget::AMThumbnailScrollGraphicsWidget(QGraphicsItem* parent)
	: QGraphicsItem(parent), QGraphicsLayoutItem() {
	width_ = 240;
	preferredWidth_ = 240;
	textHeight_ = 30;

	sourceIsDb_ = true;
	sourceDb_ = 0;
	sourceObject_ = 0;

	pixmap_ = invalidPixmap();

	setEnabled(true);
	setAcceptHoverEvents(true);
	setAcceptsHoverEvents(true);

}


void AMThumbnailScrollGraphicsWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

	Q_UNUSED(option)
	Q_UNUSED(widget)

	double height = width_*3/4;

	painter->drawPixmap( QRectF(0,0,width_, height),
						 pixmap_,
						 QRectF(0,0,240,180) );

	// draw title and subtitle (thumbnail content)
	QRectF tRect;
	painter->setFont(QFont("Lucida Grande", 12, QFont::DemiBold));
	painter->setPen(QColor::fromRgb(25,25,25));
	painter->drawText( QRectF(0,0, width_, 30),
					   Qt::AlignRight | Qt::AlignTop,
					   title_,
					   &tRect);
	painter->setFont(QFont("Lucida Grande", 12, QFont::Light, true));
	painter->setPen(QColor::fromRgb(167,167,167));
	painter->drawText( QRectF(0,tRect.height(),width_, 30),
					   Qt::AlignRight | Qt::AlignTop,
					   subtitle_);


	// draw captions (lines 1 and 2): property of this widget
	QRectF c1Rect, c2Rect;
	painter->setFont(QFont("Lucida Grande", 12, QFont::DemiBold));
	painter->setPen(QColor::fromRgb(25,25,25));
	painter->drawText( QRectF(0,height + textLineSpacing(), width_, 30),
					   Qt::AlignHCenter | Qt::AlignTop,
					   c1_,
					   &c1Rect);
	painter->setFont(QFont("Lucida Grande", 12, QFont::Light, true));
	painter->setPen(QColor::fromRgb(167,167,167));
	painter->drawText( QRectF(0,height+c1Rect.height()+2*textLineSpacing(),width_, 30),
					   Qt::AlignHCenter | Qt::AlignTop,
					   c2_,
					   &c2Rect);

	painter->drawRect(0,0,width_, height);

	if( !qFuzzyCompare(c1Rect.height() + c2Rect.height() + 2*textLineSpacing(), textHeight_ ) ) {
		prepareGeometryChange();
		textHeight_ = c1Rect.height() + c2Rect.height() + 2*textLineSpacing();
	}


}


void AMThumbnailScrollGraphicsWidget::displayThumbnail(AMDbThumbnail t) {

	title_ = t.title;
	subtitle_ = t.subtitle;

	if(t.type == AMDbThumbnail::PNGType) {
		pixmap_ = QPixmap(240, 180);
		pixmap_.loadFromData(t.thumbnail, "PNG");
	}
	else {
		pixmap_ = invalidPixmap();
	}

	update();
}

void AMThumbnailScrollGraphicsWidget::displayThumbnail(AMDatabase* db, int id) {

	if(db == 0) {
		title_ = QString();
		subtitle_ = QString();
		pixmap_ = invalidPixmap();
		update();
		return;
	}

	QSqlQuery q = db->query();
	q.prepare(QString("SELECT title,subtitle,type,thumbnail FROM %1 WHERE id = ?").arg(AMDatabaseDefinition::thumbnailTableName()));
	q.bindValue(0, id);
	if(q.exec() && q.first()) {
		title_ = q.value(0).toString();
		subtitle_ = q.value(1).toString();
		if(q.value(2).toString() == QString("PNG")) {
			QPixmap p;
			p.loadFromData(q.value(3).toByteArray(), "PNG");
			pixmap_ = p;
		}
		else {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -1, "AMThumbnailScrollViewerGraphicsWidget: Invalid/un-implemented thumbnail type."));
			pixmap_ = invalidPixmap();
		}
	}
	else {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -1, QString("AMThumbnailScrollViewerGraphicsWidget: Could not retrieve thumbnail from database, with query '%1'. (id: %2) Error: \"%3\".").arg(q.executedQuery()).arg(id).arg(q.lastError().text())));
	}
	update();
}


QPixmap AMThumbnailScrollGraphicsWidget::invalidPixmap() {
	QPixmap p(240,180);
	QPainter painter(&p);
	QRadialGradient g(QPointF(120, 50), 120, QPointF(120,50));
	g.setColorAt(0, Qt::white);
	g.setColorAt(1, QColor::fromRgb(223,230,237));

	painter.setBrush(QBrush(g));
	painter.setPen(QColor::fromRgb(167,167,167));
	painter.drawRect(0,0,239,179);
	painter.setFont(QFont("Lucida Grande", 128, QFont::DemiBold));
	painter.drawText(QRectF(0,0,240,180), Qt::AlignHCenter | Qt::AlignVCenter, "?");
	painter.end();
	return p;
}


#include <QGraphicsSceneHoverEvent>

/// re-implemented from QGraphicsItem to change the thumbnail when the mouse is moved over top
void AMThumbnailScrollGraphicsWidget::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {

	double ratio = event->pos().x() / width_;

	if(sourceIsDb_ && sourceDb_ && !ids_.isEmpty()) {
		int index = ids_.count()*ratio;
		if(index == ids_.count())
			index--;

		if(index != tIndex_) {
			tIndex_ = index;
			displayThumbnail(sourceDb_, ids_.at(tIndex_));
		}
	}

	if(!sourceIsDb_ && sourceObject_) {
		int index = sourceObject_->thumbnailCount()*ratio;
		if(index != tIndex_) {
			tIndex_ = index;
			displayThumbnail(sourceObject_->thumbnail(tIndex_));
		}
	}

	QGraphicsItem::hoverMoveEvent(event);
}

#include <QGraphicsDropShadowEffect>
#include <QDebug>

void AMThumbnailScrollGraphicsWidget::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {

	QGraphicsDropShadowEffect* e = new QGraphicsDropShadowEffect();
	e->setOffset(6, 4);
	e->setBlurRadius(12);

	this->setGraphicsEffect(e);

	QGraphicsItem::hoverEnterEvent(event);
}

void AMThumbnailScrollGraphicsWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {

	setGraphicsEffect(0);

	// return to key frame (right now: thumbnail 0)
	/// \todo make system for default key-frame. (ie: set preferred thumbnail)
	if(sourceIsDb_ && sourceDb_) {
		if(tIndex_ != 0) {
			tIndex_ = 0;
			displayThumbnail(sourceDb_, ids_.at(tIndex_));
		}
	}
	if(!sourceIsDb_ && sourceObject_) {
		if(tIndex_ != 0) {
			tIndex_ = 0;
			displayThumbnail(sourceObject_->thumbnail(tIndex_));
		}
	}

	QGraphicsItem::hoverLeaveEvent(event);
}


