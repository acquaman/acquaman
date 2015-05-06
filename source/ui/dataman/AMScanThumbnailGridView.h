#ifndef AMTHUMBNAILGRIDVIEW_H
#define AMTHUMBNAILGRIDVIEW_H

#include <QAbstractItemView>
#include <QScrollBar>
#include <QMouseEvent>

/**
 * View class which displays a list of Scan Thumbnails in a flow grid style
 */
class AMScanThumnailGridGeometryManager;
class AMScanThumbnailGridInputManager;
class AMScanThumbnailGridView : public QAbstractItemView
{
	Q_OBJECT
public:
	/**
	 * Creates a new instance of an AMScanThumbnailGridView which has no attached
	 * model
	 * @param parent::QObject ~ The parent QObject
	 */
	explicit AMScanThumbnailGridView(QWidget *parent = 0);

	QModelIndex indexAt(const QPoint& point) const;

	QRect visualRect(const QModelIndex &index) const;

	void scrollTo(const QModelIndex &index, ScrollHint hint);
	
signals:
	
public slots:
	
protected slots:
	void onItemSelected(int itemIndex, QItemSelectionModel::SelectionFlags);
	void onSelectionExtended(int itemIndex);
	void onItemDoubleClicked(int itemIndex);
	void onDragBegun();
	void onSelectionRectangleChanged(const QRect& selectionRectangle);
	void onSelectionRectangleEnded(QItemSelectionModel::SelectionFlags);
	void onHoverMove(int itemIndex, int deltaX);
protected:
	int horizontalOffset() const;

	bool isIndexHidden(const QModelIndex &index) const;

	QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);

	void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);

	int verticalOffset() const;

	QRegion visualRegionForSelection(const QItemSelection &selection) const;

	void paintEvent(QPaintEvent *event);

	void resizeEvent(QResizeEvent *event);

	void mousePressEvent(QMouseEvent *event);

	void mouseMoveEvent(QMouseEvent *event);

	void mouseReleaseEvent(QMouseEvent *event);

	void updateScrollBars();
private slots:

private:

	// Private Member Variables
	AMScanThumnailGridGeometryManager* geometryManager_;
	AMScanThumbnailGridInputManager* inputManager_;
};


#endif // AMTHUMBNAILGRIDVIEW_H
