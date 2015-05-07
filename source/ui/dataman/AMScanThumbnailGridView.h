#ifndef AMTHUMBNAILGRIDVIEW_H
#define AMTHUMBNAILGRIDVIEW_H

#include <QAbstractItemView>
#include <QScrollBar>
#include <QMouseEvent>
#include <QRubberBand>

/**
 * View class which displays a list of Scan Thumbnails in a flow grid style
 */
class AMScanThumbnailGridGeometryManager;
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

	~AMScanThumbnailGridView();

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
	void onSelectionRectangleEnded(const QRect& selectionRectangle, QItemSelectionModel::SelectionFlags);
	void onHoverMove(int itemIndex, int positionX, int positionY);
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

	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

	void rowsInserted(const QModelIndex &parent, int start, int end);

	void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

	void updateScrollBars();
private slots:

private:

	// Private Member Variables
	AMScanThumbnailGridGeometryManager* geometryManager_;
	AMScanThumbnailGridInputManager* inputManager_;
	QRubberBand* rubberBand_;
	/// A hash which maps the grid row's current displayed thumbnail index. If non is stored, then 0 is assumed
	QHash<int, int> rowCurrentDisplayedThumbnailMap_;
};


#endif // AMTHUMBNAILGRIDVIEW_H
