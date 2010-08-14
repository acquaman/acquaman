#ifndef AMTHUMBNAILSCROLLVIEWER_H
#define AMTHUMBNAILSCROLLVIEWER_H

#include <QLabel>
#include <QList>

#include "dataman/AMDbObject.h"
#include "dataman/AMDatabase.h"

/// This widget shows a single thumbnail, either from an AMDbObject or accessed directly from the database. However, by placing the mouse over the thumbnail, one can scroll through a related set of thumbnails.  The thumbnail title and subtitle are super-posed over top of the thumbnail image.

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


#include <QGraphicsWidget>
#include <QDebug>

/// This is a high-performance version of AMThumbnailScrollWidget for use inside the QGraphicsView system
/*! NEEDED!
  You can start by reimplementing important functions: the protected sizeHint() function, as well as the public setGeometry() function. If you want your items to be aware of immediate geometry changes, you can also reimplement updateGeometry().
  */

class AMThumbnailScrollGraphicsWidget : public QGraphicsWidget {
	Q_OBJECT
public:
	explicit AMThumbnailScrollGraphicsWidget(QGraphicsItem* parent = 0);

	virtual QRectF boundingRect() const {
		return QRectF(0,0,width_, width_*3/4 + textHeight_);
	}

	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);


	/// display the thumbnails from a given AMDbObject
	void setSource(AMDbObject* source) {
		sourceIsDb_ = false;
		sourceObject_ = source;
		tIndex_ = 0;
		if(sourceObject_)
			displayThumbnail(sourceObject_->thumbnail(tIndex_));
		else
			displayThumbnail(AMDbThumbnail());
	}

	/// Display thumbnails directly out of a database (from the thumbnail table, with id's given in \c ids)
	void setSource(AMDatabase* db, QList<int> ids) {
		sourceIsDb_ = true;
		sourceDb_ = db;
		ids_ = ids;
		tIndex_ = 0;
		displayThumbnail(sourceDb_, tIndex_);
	}

	/// Display \c count thumbnails directly out of a database (from the thumbnail table, with sequential id's starting at \c startId)
	void setSource(AMDatabase* db, int startId, int count) {
		sourceIsDb_ = true;
		sourceDb_ = db;
		ids_.clear();
		for(int i=startId; i<startId+count; i++)
			ids_ << i;
		tIndex_ = 0;
		displayThumbnail(sourceDb_, ids_.at(tIndex_));
	}

	void setWidth(double width) {
		/*
		prepareGeometryChange();
		width_ = width;
		update();
		*/
		preferredWidth_ = width;
		updateGeometry();
	}

	void setGeometry(const QRectF &rect) {
		prepareGeometryChange();
		setPos(rect.left(), rect.top());
		width_ = rect.width();
		update();
	}

	static double textLineSpacing() { return 2; }

public slots:
	void setCaption1(const QString& text) {
		c1_ = text;
	}
	void setCaption2(const QString& text) {
		c2_ = text;
	}

protected:
	double preferredWidth_, width_, textHeight_;
	QPixmap pixmap_;

	/// title and subtitle are written on top of the thumbnail itself.  They are found from the content inside the thumbnail
	QString title_, subtitle_;
	/// captions (c1_, c2_) are written below the thumbnail
	QString c1_, c2_;

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

	/// Change the view to display this thumbnail object
	void displayThumbnail(AMDbThumbnail t);
	/// Change the view to display the thumbnail data pulled from database \c db at row \c id.
	void displayThumbnail(AMDatabase* db, int id);

	/// returns a pixmap (240 x 180) suitable for an invalid/blank background
	static QPixmap invalidPixmap();


	/// re-implemented from QGraphicsItem to change the thumbnail when the mouse is moved over top
	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);



	QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {

		Q_UNUSED(which)
		Q_UNUSED(constraint)

		return QSizeF(preferredWidth_, preferredWidth_*3/4 + textHeight_);
	}

};


#endif // AMTHUMBNAILSCROLLVIEWER_H
