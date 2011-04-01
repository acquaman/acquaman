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


#include "AMThumbnailScrollViewer.h"
#include "dataman/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"

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

	if(sourceDb_ && sourceIds_.count() > 0)
		displayThumbnail(sourceDb_, sourceIds_.at(0));
	else {
		displayThumbnail(AMDbThumbnail());
	}
}


#include <QPainter>
#include <QGradient>
#include <QGradientStops>

QPixmap* AMThumbnailScrollViewer::invalidPixmapCache_ = 0;

QPixmap AMThumbnailScrollViewer::invalidPixmap() {
	if(!invalidPixmapCache_) {
		invalidPixmapCache_ = new QPixmap(240,180);

		QPainter painter(invalidPixmapCache_);
		QRadialGradient g(QPointF(120, 50), 120, QPointF(120,50));
		g.setColorAt(0, Qt::white);
		g.setColorAt(1, QColor::fromRgb(223,230,237));

		painter.setBrush(QBrush(g));
		painter.setPen(QColor::fromRgb(167,167,167));
		painter.drawRect(0,0,239,179);
		painter.setFont(QFont("Lucida Grande", 128, QFont::DemiBold));
		painter.drawText(QRectF(0,0,240,180), Qt::AlignHCenter | Qt::AlignVCenter, "?");
		painter.end();
	}

	return *invalidPixmapCache_;
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
	q.prepare(QString("SELECT title, subtitle, type, thumbnail FROM %1 WHERE id = ?").arg(AMDbObjectSupport::thumbnailTableName()));
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

	objectId_ = -1;

	// previously: pixmap_ = invalidPixmap();
	// This is a huge performance hog. Having to calculate this radial gradient for EACH thumbnail added was taking forever...
	/* Now optimized in two ways:
		- Don't calculate the invalid pixmap_ until we really need it. (ie: in paint() )
		- Cache the last pixmap in a static member, so we can reuse it if it's the right size.
		*/
	deferredUpdate_db_ = 0;
	deferredUpdate_id_ = 0;
	deferredUpdate_required_ = true;


	setEnabled(true);
	setAcceptHoverEvents(true);
	setAcceptsHoverEvents(true);

	/// this item is deleted when its QGraphicsItem parent is deleted. The layout does not own this item.
	setOwnedByLayout(false);

	this->setFlag(QGraphicsItem::ItemIsSelectable, true);

}


void AMThumbnailScrollGraphicsWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

	Q_UNUSED(option)
	Q_UNUSED(widget)

	if(deferredUpdate_required_)
		displayThumbnail(deferredUpdate_db_, deferredUpdate_id_);

	int height = width_*3/4;
	if(scaledPixmap_.size() != QSize(width_, height))
		scaledPixmap_ = pixmap_.scaled(width_, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);	// this caches the scaled version of the pixmap, in case we need it again.

	painter->drawPixmap(QPointF(0,0), scaledPixmap_);

	// draw title and subtitle (thumbnail content)
	QRectF tRect;
	painter->setFont(QFont("Lucida Grande", 12, QFont::DemiBold));
	painter->setPen(QColor::fromRgb(25,25,25));
	painter->drawText( QRectF(0,marginTop(), width_-marginLeft(), 30),
					   Qt::AlignRight | Qt::AlignTop,
					   title_,
					   &tRect);
	painter->setFont(QFont("Lucida Grande", 12, QFont::Light, true));
	painter->setPen(QColor::fromRgb(167,167,167));
	painter->drawText( QRectF(0,textLineSpacing()+marginTop()+tRect.height(),width_-marginLeft(), 30),
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

	double actualTextHeight = c1Rect.height() + c2Rect.height() + 2*textLineSpacing();
	if( !qFuzzyCompare(actualTextHeight, textHeight_ ) ) {
		prepareGeometryChange();
		textHeight_ = actualTextHeight;
	}

	if(isSelected()) {
		painter->drawRect(-10, -10, width_+20, height + textHeight_ + 20);

	}


}


void AMThumbnailScrollGraphicsWidget::displayThumbnail(AMDbThumbnail t) {

	deferredUpdate_required_ = false;

	title_ = t.title;
	subtitle_ = t.subtitle;

	if(t.type == AMDbThumbnail::PNGType) {
		pixmap_ = QPixmap(240, 180);
		pixmap_.loadFromData(t.thumbnail, "PNG");
		scaledPixmap_ = QPixmap();
	}
	else {
		pixmap_ = invalidPixmap();
		scaledPixmap_ = QPixmap();
	}

	update();
}

void AMThumbnailScrollGraphicsWidget::displayThumbnailDeferred(AMDatabase *db, int id) {
	deferredUpdate_db_ = db;
	deferredUpdate_id_ = id;
	deferredUpdate_required_ = true;
	update();
}

void AMThumbnailScrollGraphicsWidget::displayThumbnail(AMDatabase* db, int id) {

	if(db == 0) {
		title_ = QString();
		subtitle_ = QString();
		pixmap_ = invalidPixmap();
		scaledPixmap_ = QPixmap();
		if(!deferredUpdate_required_)
			update();
		deferredUpdate_required_ = false;
		return;
	}

	// qDebug() << "AMThumbnailScrollGraphicsWidget::displayThumbnail() -- Doing database lookup";

	QSqlQuery q = db->query();
	q.prepare(QString("SELECT title,subtitle,type,thumbnail FROM %1 WHERE id = ?").arg(AMDbObjectSupport::thumbnailTableName()));
	q.bindValue(0, id);
	if(q.exec() && q.first()) {
		title_ = q.value(0).toString();
		subtitle_ = q.value(1).toString();
		if(q.value(2).toString() == QString("PNG")) {
			QPixmap p;
			p.loadFromData(q.value(3).toByteArray(), "PNG");
			pixmap_ = p;
			scaledPixmap_ = QPixmap();
		}
		else {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -1, "AMThumbnailScrollViewerGraphicsWidget: Invalid/un-implemented thumbnail type."));
			pixmap_ = invalidPixmap();
			scaledPixmap_ = QPixmap();
		}
	}
	else {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -1, QString("AMThumbnailScrollViewerGraphicsWidget: Could not retrieve thumbnail from database, with query '%1'. (id: %2) Error: \"%3\".").arg(q.executedQuery()).arg(id).arg(q.lastError().text())));
	}

	// normally, issue an update() to trigger a re-paint. However, if deferredUpdate_required is true, this is already being called within a paint() event, so don't do it again.
	if(!deferredUpdate_required_)
		update();

	deferredUpdate_required_ = false;
}

QPixmap* AMThumbnailScrollGraphicsWidget::invalidPixmapCache_ = 0;

QPixmap AMThumbnailScrollGraphicsWidget::invalidPixmap() {

	if(!invalidPixmapCache_) {

		invalidPixmapCache_ = new QPixmap(240, 180);

		QPainter painter(invalidPixmapCache_);
		QRadialGradient g(QPointF(120, 50), 120, QPointF(120,50));
		g.setColorAt(0, Qt::white);
		g.setColorAt(1, QColor::fromRgb(223,230,237));

		painter.setBrush(QBrush(g));
		painter.setPen(QColor::fromRgb(167,167,167));
		painter.drawRect(0,0,239,179);
		painter.setFont(QFont("Lucida Grande", 128, QFont::DemiBold));
		painter.drawText(QRectF(0,0,240,180), Qt::AlignHCenter | Qt::AlignVCenter, "?");
		painter.end();
	}

	return *invalidPixmapCache_;
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
	e->setOffset(shadowOffsetX(), shadowOffsetY());
	e->setBlurRadius(shadowBlurRadius());

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


/// Re-implemented from QGraphicsItem to be a drag-and-drop source containing the database, table name and id of the object that this thumbnail represents.
void AMThumbnailScrollGraphicsWidget::mousePressEvent(QGraphicsSceneMouseEvent *event) {

	if (event->button() == Qt::LeftButton) {
		event->accept();
	}

	QGraphicsItem::mousePressEvent(event);
}

#include <QApplication>

/// Re-implemented from QGraphicsItem to be a drag-and-drop source containing the database, table name and id of the object that this thumbnail represents.
void AMThumbnailScrollGraphicsWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < QApplication::startDragDistance()) {
		return;
	}

	QDrag* drag = createDragObject(event->widget());

	if(drag) {
		/// \todo: what drag action to use? default uses Qt::MoveAction
		drag->exec(Qt::CopyAction);
		// setCursor(Qt::OpenHandCursor);
	}
}

/// Re-implemented from QGraphicsItem to be a drag-and-drop source containing the database, table name and id of the object that this thumbnail represents.
void AMThumbnailScrollGraphicsWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	// setCursor(Qt::OpenHandCursor);
}

#include <QUrl>
#include <QList>

/// This is a helper function that creates a new QDrag object and returns a pointer to it.  The QDrag object has the MIME type "text/uri-list" with one URL: 'amd://databaseName/tableName/id', which describes the object represented by this thumbnail.  It also has image data set (MIME type "image/x-") so that the drag icon is visible.  If it's impossible to determine which object this thumbnail represents (for ex: setSource() hasn't been called yet, or was called with an invalid object), this function returns 0.
QDrag* AMThumbnailScrollGraphicsWidget::createDragObject(QWidget* dragSourceWidget) {

	QString uri;

	// if our source is a database and set of rows (ids_) in the thumbnail table
	if(sourceIsDb_ && sourceDb_) {

		// do we have the object id and table name already?
		if(!tableName_.isEmpty() && objectId_ > 0) {
			uri = QString("amd://%1/%2/%3").arg(sourceDb_->connectionName()).arg(tableName_).arg(objectId_);
		}
		// otherwise, we're going to have to look this up in reverse from the thumbnail table... Assuming we actually have some valid thumbnails.
		else {
			if(ids_.isEmpty())
				return 0;

			QSqlQuery q = sourceDb_->query();
			q.prepare(QString("SELECT objectId,objectTableName FROM %1 WHERE id = ?").arg(AMDbObjectSupport::thumbnailTableName()));
			q.bindValue(0, ids_.at(0));
			if(q.exec() && q.first()) {
				uri = QString("amd://%1/%2/%3").arg(sourceDb_->connectionName()).arg(q.value(1).toString()).arg(q.value(0).toInt());
			}
			else {
				return 0;
			}
		}
	}

	// if our source is a pointer to an AMDbObject
	else if(!sourceIsDb_ && sourceObject_) {
		if(!sourceObject_->database())
			return 0;
		if(sourceObject_->id() < 1)
			return 0;
		uri = QString("amd://%1/%2/%3").arg(sourceObject_->database()->connectionName()).arg(sourceObject_->dbTableName()).arg(sourceObject_->id());
	}

	QDrag* drag = new QDrag(dragSourceWidget);
	QMimeData* mime = new QMimeData();
	drag->setMimeData(mime);

	QUrl url = QUrl(uri);
	QList<QUrl> urlList;
	urlList << url;
	mime->setUrls(urlList);
	mime->setText(uri);

	drag->setPixmap(pixmap_.scaledToHeight(100, Qt::SmoothTransformation));
	drag->setHotSpot(QPoint(15,15));

	return drag;
}


