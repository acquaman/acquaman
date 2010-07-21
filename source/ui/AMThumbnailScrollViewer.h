#ifndef AMTHUMBNAILSCROLLVIEWER_H
#define AMTHUMBNAILSCROLLVIEWER_H

#include <QLabel>
#include <QList>

#include "dataman/AMDbObject.h"
#include "dataman/AMDatabase.h"

/// This widget shows a single thumbnail, either from an AMDbObject or accessed directly from the database. However, by placing the mouse over the thumbnail, one can scroll through a related set of thumbnails.

class AMThumbnailScrollViewer : public QLabel
{
Q_OBJECT
public:
    explicit AMThumbnailScrollViewer(QWidget *parent = 0);

	/// display the thumbnails from a given AMDbObject
	void setSource(AMDbObject* source);
	/// Display thumbnails directly out of a database (from the thumbnail table, with id's given in \c ids)
	void setSource(AMDatabase* db, QList<int> ids);

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

};

#endif // AMTHUMBNAILSCROLLVIEWER_H
