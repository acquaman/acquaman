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

	/**
	  * Cleans up resources owned by the AMScanThumbnailGridView
	  */
	virtual ~AMScanThumbnailGridView();

	/**
	  * Returns the model index of the item at the provided point. If there is
	  * no item at the provided point then an invalid QModelIndex is returned
	  * @param point::QPoint ~ The point at which the index to be returned lies
	  */
	QModelIndex indexAt(const QPoint& point) const;

	/**
	  * Returns a rectangle in which the visual representation of the data contained
	  * at the provided index is contains
	  * @param index::QModelIndex ~ The index whose visual geometry is to be returned
	  */
	QRect visualRect(const QModelIndex &index) const;

    /**
      * Returns a rectangle in which the visual representation for the background
      * of the data at the provided index.
      * @param index::QModelIndex ~ The index whose background visual geometry is to
      * be returned
      */
    QRect backgroundRect(const QModelIndex &index) const;

	/**
	  * Scrolls the view to ensure that the visual representation of the provided
	  * index is visible.
	  * @param index::QModelIndex ~ The index which the view will be moved to show
	  * @param hint::StrollHint ~ Currently ignored.
	  */
	void scrollTo(const QModelIndex &index, ScrollHint hint);
	
signals:
	
public slots:
	
protected slots:
	/**
	  * Handles the input manager indicated that the provided index has been selected
	  * with the provided selection flags
	  * @param itemIndex::int ~ The index of the item which the input manager indicates
	  *	has been selected
	  * @param flags::SelectionFlags ~ The selection behaviour to be used
	  */
	void onItemSelected(int itemIndex, QItemSelectionModel::SelectionFlags flags);

	/**
	  * Handles the input manager indicating that the current selection has been
	  * extended to include the provided itemIndex and everything inbetween
	  * @param itemIndex::int ~ The index of the item which the selection should
	  * be extended to
	  */
	void onSelectionExtended(int itemIndex);

	/**
	  * Handles the input manager indicating that the provided item has been double
	  * clicked
	  * @param itemIndex::int ~ The index of the item which has been double clicked
	  */
	void onItemDoubleClicked(int itemIndex);

	/**
	  * Handles the input manager indicating a drag has begun
	  */
	void onDragBegun();

	/**
	  * Handles the input manager indicating that the bounds of the selection rectangle
	  * currently being drawn in being extended. This function also handles the
	  * start of a selection rectangle being drawn
	  * @param selectionRectangle::QRect ~ The new bounds of the selection rectangle
	  */
	void onSelectionRectangleChanged(const QRect& selectionRectangle);

	/**
	  * Handles the input manager indicating that a selection rectangle has been
	  * completed with the provided selection flags
	  * @param selectionRectangle::QRect ~ The final bounds of the selection rectangle
	  * @param SelectionFlags::flags ~ The flags indicating selection behaviour
	  */
	void onSelectionRectangleEnded(const QRect& selectionRectangle, QItemSelectionModel::SelectionFlags flags);

	/**
	  * Handles the input manager indicating that, while hovering over a single
	  * scan the mouse has moved horizontally
	  * @param itemIndex::int ~ The index of the item which is being hovered over
	  * @param positionX::int ~ The x position which the mouse is currently at
	  * @param positionY::int ~ The y position which the mouse is currently at
	  */
	void onHoverMove(int itemIndex, int positionX, int positionY);
protected:
	/**
	  * The current value of the horizontal scroll bar
	  */
	int horizontalOffset() const;

	/**
	  * Whether the provided index is currently hidden. Always returns false for
	  * AMScanThumbnailGridView
	  * @param index::QModelIndex ~ The index to report the visible status of
	  */
	bool isIndexHidden(const QModelIndex &index) const;

	/**
	  * Currently does nothing. Not clear what a clear keyboard action should be
	  * for a grid based view, and the view maintains no concept of focus at the
	  * moment anyway
	  */
	QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);

	/**
	  * Sets the selection status of all the items contained within the provided
	  * rectangle based on the provided selection flags
	  * @param rect::QRect ~ The rectangle which defines the bounds of the items
	  * whose selection status is to be changed
	  */
	void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);

	/**
	  * The current value of the vertical scroll bar
	  */
	int verticalOffset() const;

	/**
	  * The region which contains all items in the provided item selection. At the
	  * moment this simply returns the entire viewport, for ease sake
	  */
	QRegion visualRegionForSelection(const QItemSelection &selection) const;

	/**
	  * Handles a paint being requested of the AMScanThumbnailGridView
	  */
	void paintEvent(QPaintEvent *event);

	/**
	  * Handles the AMScanThumbnailGridView being resized
	  */
	void resizeEvent(QResizeEvent *event);

	/**
	  * Handles a mouse button being pressed over the AMScanThumbnailGridView
	  */
	void mousePressEvent(QMouseEvent *event);

	/**
	  * Handles the mouse being moved over the AMScanThumbnailGridView
	  */
	void mouseMoveEvent(QMouseEvent *event);

	/**
	  * Handles a mouse button being released over the AMScanThumbnailGridView
	  */
	void mouseReleaseEvent(QMouseEvent *event);

	/**
	  * Handles some of the model's data being altered. In this case the entire
	  * viewport is refreshed, as there is no concept of top left and bottom right
	  * indices based on the table structure of the QAbstractItemModel.
	  */
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

	/**
	  * Handles a row of data being inserted into the model
	  */
	void rowsInserted(const QModelIndex &parent, int start, int end);

	/**
	  * Handles the fact that a row is about to be removed from the model
	  */
	void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

	/**
	  * Refreshes the size and visibility of the scroll bars based on the current
	  * number of rows and the size of the viewport
	  */
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
