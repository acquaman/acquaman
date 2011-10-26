/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMTHUMBNAILSCROLLVIEWER_H
#define AMTHUMBNAILSCROLLVIEWER_H

#include <QLabel>
#include <QList>

#include "dataman/database/AMDbObject.h"
#include "dataman/database/AMDatabase.h"

/// This widget shows a single thumbnail, either from an AMDbObject or accessed directly from the database. However, by placing the mouse over the thumbnail, one can scroll through a related set of thumbnails.  The thumbnail title and subtitle are super-posed over top of the thumbnail image.
/*! \todo Implement deferred database lookups for displayThumbnail(AMDatabase* db, int id) as done in AMThumbnailScrollGraphicsWidget*/

class AMThumbnailScrollViewer : public QLabel
{
	Q_OBJECT
public:
	explicit AMThumbnailScrollViewer(QWidget *parent = 0);

	/// display the thumbnails from a given AMDbObject
	void setSource(AMDbObject* source);
	/// Display thumbnails directly out of a database (from the thumbnail table, with id's given in \c ids)
	void setSource(AMDatabase* db, QList<int> ids);
	/// Display \c count thumbnails directly out of a database (from the thumbnail table, with sequential id's starting at \c startId)
	void setSource(AMDatabase* db, int startId, int count);

	QSize sizeHint() const {
		return QSize(240, 180);
	}

	int heightForWidth(int w) const {
		return w*3/4;
	}

signals:

public slots:

protected:
	QLabel* title_, *subtitle_;
	bool sourceIsDb_;

	AMDbObject* sourceObject_;
	AMDatabase* sourceDb_;
	QList<int> sourceIds_;

	// current thumbnail index:
	int tIndex_;

	void displayThumbnail(AMDbThumbnail t);
	void displayThumbnail(AMDatabase* db, int id);


	void mouseMoveEvent ( QMouseEvent * event );

	/// returns a pixmap (240 x 180) suitable for an invalid/blank background
	static QPixmap invalidPixmap();
	/// Cache for the invalidPixmap(), so that it doesn't need to be recalculated every time
	static QPixmap* invalidPixmapCache_;

};

/// This widget incorporates a thumbnail viewer (AMThumbnailScrollViewer) with a couple lines of caption text
class AMThumbnailScrollWidget : public QWidget {
	Q_OBJECT
public:
	explicit AMThumbnailScrollWidget(const QString& caption1 = QString(), const QString& caption2 = QString(), QWidget* parent = 0);

	/// display the thumbnails from a given AMDbObject
	void setSource(AMDbObject* source) {
		tv_->setSource(source);
	}

	/// Display thumbnails directly out of a database (from the thumbnail table, with id's given in \c ids)
	void setSource(AMDatabase* db, QList<int> ids) {
		tv_->setSource(db, ids);
	}

	/// Display \c count thumbnails directly out of a database (from the thumbnail table, with sequential id's starting at \c startId)
	void setSource(AMDatabase* db, int startId, int count) {
		tv_->setSource(db, startId, count);
	}

public slots:
	void setCaption1(const QString& text) {
		c1_->setText(text);
	}
	void setCaption2(const QString& text) {
		c2_->setText(text);
	}

protected:
	QLabel* c1_, *c2_;
	AMThumbnailScrollViewer* tv_;
};


#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QDebug>
#include <QPainterPath>

/// This is a high-performance version of AMThumbnailScrollWidget for use inside the QGraphicsView system
class AMThumbnailScrollGraphicsWidget : public QGraphicsItem, public QGraphicsLayoutItem {

public:
	/// Enable the use of qgraphicsitem_cast
	enum { Type = AM::ThumbnailScrollGraphicsWidgetType };
	/// Enable the use of qgraphicsitem_cast
	int type() const { return Type; }

	explicit AMThumbnailScrollGraphicsWidget(QGraphicsItem* parent = 0);
	virtual ~AMThumbnailScrollGraphicsWidget() {}



	/// Returns the name of the database table for the item that this widget represents (or an empty string if unknown)
	QString tableName() const { return tableName_; }
	/// Returns the database id of the object that this widget represents (or -1 if unknown)
	int objectId() const { return objectId_; }
	/// Returns the database pointer, if this widget is representing an object out of the database. If we're representing a temporary AMDbThumbnail instead, this will return 0.
	AMDatabase* database() const { return sourceDb_; }

/*
	/// This bounding rect is just big enough for the picture box, the text underneath, and some extra room to erase the shadow graphics effect we apply when hover-overed
	virtual QRectF boundingRect() const {
		return QRectF(0,
					  0,
					  width_ + shadowBlurRadius(),
					  width_*3.0/4.0 + textHeight_ + shadowBlurRadius());
	}
	*/

	/// This bounding rect is crude but fast. It gives generous room for blur effects and selection borders
	virtual QRectF boundingRect() const {
		return QRectF(-15,
					  -15,
					  width_ + 30,
					  width_*3.0/4.0 + textHeight_ + 30);
	}

	virtual QPainterPath shape() const {
		QPainterPath rv;
		rv.addRect(0,0,width_, width_*3.0/4.0 + textHeight_);
		return rv;
	}

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);


	/// display the thumbnails from a given AMDbObject
	void setSource(AMDbObject* source) {
		sourceIsDb_ = false;
		sourceObject_ = source;


		tIndex_ = 0;
		if(sourceObject_) {
			objectId_ = sourceObject_->id();
			tableName_ = sourceObject_->dbTableName();
			displayThumbnail(sourceObject_->thumbnail(tIndex_));
		}
		else {
			objectId_ = -1;
			tableName_.clear();
			displayThumbnail(AMDbThumbnail());
		}
	}

	/// Display thumbnails directly out of a database (from the thumbnail table, with id's given in \c ids)
	void setSource(AMDatabase* db, QList<int> ids) {
		sourceIsDb_ = true;
		sourceDb_ = db;

		// We don't know this...
		objectId_ = -1;
		tableName_.clear();

		ids_ = ids;
		tIndex_ = 0;
		if(ids_.count() > 0)
			displayThumbnailDeferred(sourceDb_, ids_.at(tIndex_));
		else
			displayThumbnailDeferred(0,0);
	}

	/// Display \c count thumbnails directly out of a database (from the thumbnail table, with sequential id's starting at \c startId)
	void setSource(AMDatabase* db, int startId, int count) {
		sourceIsDb_ = true;
		sourceDb_ = db;

		// We don't know this...
		objectId_ = -1;
		tableName_.clear();

		ids_.clear();
		for(int i=startId; i<startId+count; i++)
			ids_ << i;
		tIndex_ = 0;
		if(ids_.count() > 0)
			displayThumbnailDeferred(sourceDb_, ids_.at(tIndex_));
		else
			displayThumbnailDeferred(0,0);

	}

	/// Display thumbnails for the object with this \c objectId out of the \c tableName table. For performance, if you know the answers already, you can specify the \c thumbnailStartId and \c count of the thumbnails in the thumbnail table. Otherwise, this information will be retrieved from the Database. This is the preferred usage of setSource(), since it remembers the source object's real location, while avoiding any unnecessary database lookups.
	void setSource(AMDatabase* db, const QString& tableName, int objectId, int thumbnailStartId = -1, int count = -1) {

		// need to retrieve this from the db:
		if(thumbnailStartId == -1 || count == -1) {

			thumbnailStartId = 0; count = 0;

			if(db) {
				QSqlQuery q = db->query();
				q.prepare(QString("SELECT thumbnailFirstId,thumbnailCount FROM %1 WHERE id = ?").arg(tableName));
				q.bindValue(0,objectId);
				if(q.exec() && q.first()) {
					thumbnailStartId = q.value(0).toInt();
					count = q.value(1).toInt();
				}
			}
		}

		setSource(db, thumbnailStartId, count);
		tableName_ = tableName;
		objectId_ = objectId;
	}

	/// Use this to set the size hint that the thumbnail requests within layouts, if a constraint is not provided. You can use this to dynamically resize the thumbnail item in layouts that don't specify a QSizeF constraint when calling sizeHint(int which, QSizeF constraint).
	void setDefaultWidth(double width) {
		if(preferredWidth_ == width)
			return;

		preferredWidth_ = width;
		updateGeometry();
	}

	/// Re-implemented from QGraphicsLayoutItem to respond to size instructions from the layout
	void setGeometry(const QRectF &rect) {
		// QGraphicsLayoutItem::setGeometry(rect);
		prepareGeometryChange();
		setPos(rect.left(), rect.top());
		width_ = rect.width();
		// clear this cache
		c1_elided_.clear();
		c2_elided_.clear();
		update();
	}

	static double textLineSpacing() { return 2; }
	static double marginLeft() { return 10; }
	static double marginTop() { return 5; }

public:
	void setCaption1(const QString& text) {
		c1_ = text;
		c1_elided_.clear();
	}
	void setCaption2(const QString& text) {
		c2_ = text;
		c2_elided_.clear();
	}

protected:
	/// Some preferences/parameters:
	static double shadowOffsetX() { return 6; }
	static double shadowOffsetY() { return 4; }
	static double shadowBlurRadius() { return 12; }

	double preferredWidth_, width_, textHeight_;
	/// The pixmap holding the original thumbnail image
	QPixmap pixmap_;
	/// The pixmap_ scaled to the last size we had to draw it at
	QPixmap scaledPixmap_;

	/// title and subtitle are written on top of the thumbnail itself.  They are found from the content inside the thumbnail
	QString title_, subtitle_;
	/// captions (c1_, c2_) are written below the thumbnail
	QString c1_, c2_;
	/// Caches the elided text (the text we have room to display)
	QString c1_elided_, c2_elided_;

	/// index of the current thumbnail (if showing a set a thumbnails)
	int tIndex_;

	/// Indicates that the source for thumbnails is directly out of the database, \c sourceDb_. (If false, the source is from an AMDbObject \c sourceObject_.)
	bool sourceIsDb_;
	/// When sourceIsDb_ = true, this is a pointer to the database that should be queried for thumbnails.
	AMDatabase* sourceDb_;
	/// When sourceIsDb_ = false, this is pointer to the AMDbObject, that can be asked for its thumbnailCount() and thumbnail(int index);
	AMDbObject* sourceObject_;
	/// When sourceIsDb_ = true, this is the set of rows in the thumbnail table to use.
	QList<int> ids_;
	/// If we know it, this is the name of the table holding the actual object. If we don't know it, it's an empty string.
	QString tableName_;
	/// If we know it, this is the id of the source object (in tableName_) -- NOT the id of a thumbnail in the thumbnail table.  If we don't know it, it's -1.
	int objectId_;


	/// Change the view to display this thumbnail object
	void displayThumbnail(AMDbThumbnail t);
	/// Change the view to display the thumbnail data pulled from database \c db at row \c id.
	void displayThumbnail(AMDatabase* db, int id);
	/// Performance optimization: Change the view to display the thumbnail data pulled from the database \c db at row \c id... but DON'T actually do the database lookup right now. Instead, just flag that we need to complete this before the next paint operation.
	void displayThumbnailDeferred(AMDatabase* db, int id);

	/// Optimization flags for displayThumbnailDeferred():
	AMDatabase* deferredUpdate_db_;
	/// Optimization flags for displayThumbnailDeferred():
	int deferredUpdate_id_;
	/// Optimization flags for displayThumbnailDeferred():
	bool deferredUpdate_required_;

	/// returns a pixmap (240 x 180) suitable for an invalid/blank background
	static QPixmap invalidPixmap();
	/// cache of the invalidPixmap(), so that it doesn't need to be re-drawn every time
	static QPixmap* invalidPixmapCache_;


	/// re-implemented from QGraphicsItem to change the thumbnail when the mouse is moved over top
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

	/// Re-implemented from QGraphicsItem to be a drag-and-drop source containing the database, table name and id of the object that this thumbnail represents.
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	/// Re-implemented from QGraphicsItem to be a drag-and-drop source containing the database, table name and id of the object that this thumbnail represents.
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	/// Re-implemented from QGraphicsItem to be a drag-and-drop source containing the database, table name and id of the object that this thumbnail represents.
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	/// This is a helper function that creates a new QDrag object and returns a pointer to it.  The QDrag object has the MIME type "text/uri-list" with one URL: 'amd://databaseName/tableName/id', which describes the object represented by this thumbnail.  It also has image data set (MIME type "image/x-") so that the drag icon is visible.  If it's impossible to determine which object this thumbnail represents (for ex: setSource() hasn't been called yet, or was called with an invalid object), this function returns 0.
	QDrag* createDragObject(QWidget* dragSourceWidget);




	QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {

		Q_UNUSED(which)

		if(constraint.width() < 0 && constraint.height() < 0) {
			return QSizeF(preferredWidth_, preferredWidth_*3/4 + textHeight_);
		}

		// OK, a constraint has been specified. Assuming either width-constrained or height constrained.
		QSizeF widthConstrained, heightConstrained;

		if(constraint.width() > 0) {
			widthConstrained = QSizeF(constraint.width(), constraint.width()*3/4 + textHeight_);
			if(constraint.height() > 0) {	// constrained in both width and height
				heightConstrained = QSizeF((constraint.height()-textHeight_)*4/3, constraint.height());
				if(widthConstrained.height() > constraint.height()) {
					return heightConstrained;
				}
				else {
					return widthConstrained;
				}
			}
			else {
				// only width constrained
				return widthConstrained;
			}
		}
		else {
			// only height constrained
			return QSizeF((constraint.height()-textHeight_)*4/3, constraint.height());
		}
	}

};




#endif // AMTHUMBNAILSCROLLVIEWER_H
