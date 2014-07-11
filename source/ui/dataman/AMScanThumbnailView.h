#ifndef AMSCANTHUMBNAILVIEW_H
#define AMSCANTHUMBNAILVIEW_H

#include <QTreeView>
#include <QWidget>
#include <QHeaderView>
#include <QItemDelegate>
#include <QPainter>



class AMScanThumbnailView : public QTreeView {
	Q_OBJECT
public:
	explicit AMScanThumbnailView(QWidget* parent = 0);
};



#endif // AMSCANTHUMBNAILVIEW_H
