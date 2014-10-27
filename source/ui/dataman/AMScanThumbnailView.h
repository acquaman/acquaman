#ifndef AMSCANTHUMBNAILVIEW_H
#define AMSCANTHUMBNAILVIEW_H

#include "AMQEvents.h"

#include <QWidget>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QLabel>
#include <QItemDelegate>
#include <QPainter>
#include <QRegion>
#include <QTimer>

/**
 * Class which overrides the standard QItemDelegate. Handles drawing of the thumbnail data in the
 * way required by the AMScanThumbnailView
 */
class AMScanThumbnailViewItemDelegate : public QItemDelegate {
	Q_OBJECT
public:
	explicit AMScanThumbnailViewItemDelegate(QObject* parent = 0);
	/// Paints the data associated with the provided index according to options stored in the given
	/// StyleOptionViewItem
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

/// Class which inherits from QAbstractItemView in such a way as to represent a AMLightweightScanInfo[FilterProxy]Model
/// in a standard Qt way.
class AMScanThumbnailView : public QAbstractItemView {
	Q_OBJECT
public:
	explicit AMScanThumbnailView(QWidget* parent = 0);
	/// Returns the model index of the ScanThumbnailView item at the given point in the currently
	/// displayed view (i.e. takes into account the scroll offset)
	QModelIndex indexAt(const QPoint &point) const;
	/// Scrolls the view such the the item with the given index is at the top of the view.
	/// Does not alter the horizontal offset. If no item is located at the given point, then an
	/// invalid QModelIndex is returned
	void scrollTo(const QModelIndex &index, ScrollHint hint);
	/// Returns the rectangle which entirely encompasses the ScanThumbnailView item at the given
	/// model index. If no item matches the index, then an invalid QRect is returned
	QRect visualRect(const QModelIndex &index) const;
	/// Sets the model for the view to use to obtain data. Also calculates the vertical scroll maximum
	/// by polling the models rowCount
	void setModel(QAbstractItemModel *newModel);
protected slots:
	/// Handles changes to the data in place. Resets the displayed thumbnail index for all
	/// the scans back to zero and updates the scroll bars.
	void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
	/// Handles rows being inserted into the source model. Resets the displayed thumbnail index for all
	/// the scans back to zero and updates the scroll bars.
	void rowsInserted(const QModelIndex &parent, int start, int end);
	/// Handles rows being removed from the source model. Resets the displayed thumbnail index for all
	/// the scans back to zero and updates the scroll bars.
	void rowsRemoved(const QModelIndex& parent, int start, int end);
	/// Handles the timer used to decide whether the mouse in its current position can be considered
	/// 'hovering'
	void onTimerTimout();
protected:
	/// Returns the rectangle which represents the model row provided
	QRect rectangleGridRow(int row) const;
	/// Returns the size of a single ScanThumbnailView item, including spacing
	QSize gridDimensions() const;
	/// Return the current offset of the horizontal scroll bar
	int horizontalOffset() const;
	/// Returns true if the item at the given QModelIndex is has the hidden property set to true,
	/// or if the row it is in is hidden, or if the column it is in is hidden. This property does
	/// not apply to the AMScanThumbnailView, which does not allow hiding of a given scan, and thus
	/// it always returns false
	bool isIndexHidden(const QModelIndex &index) const;
	/// Returns a QModelIndex object pointing to the next object in the view, based on the given
	/// cursorAction and keyboard modifiers specified by modifiers.
	QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
	/// Sets the selection state of all the ScanThumbnailView items which intersect the provided
	/// QRect to a state indicated by the provided selection flags
	void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
	/// Returns the current offset of the vertical scroll bar
	int verticalOffset() const;
	/// Returns the visual region which makes up the selection. In this case we simple return the
	/// viewport()
	QRegion visualRegionForSelection(const QItemSelection &selection) const;
	/// Implementation of the paint event for the view. Calls the delegate's paint function for
	/// all model indices in the visible viewport
	void paintEvent(QPaintEvent *pe);
	/// Implementation of the resize event for the view. Sets the spacing between thumbnails so that
	/// they are evenly spaced and centralized within the view horizontally and vertically
	void resizeEvent(QResizeEvent *event);
	/// Updates the scroll bars based on the size of the model and the size of the view
	void updateScrollBars();
	/// Starts drawing the selection rubber band
	void mousePressEvent(QMouseEvent *event);
	/// Updates the rectangle associated with the selection rubber band, and marks the items contained within
	/// it as selected
	void mouseMoveEvent(QMouseEvent *event);
	/// Ends drawing the selection rubber band
	void mouseReleaseEvent(QMouseEvent *event);

	/// Each of these three helper functions calculates the dimensions of the position of the particular
	/// data element relative to the passed thumbnailRectangle

	/// Obtains the poisition within thumbnailRectangle of the Scan Name
	QRect getScanNameRectangle(const QRect& thumbnailRectangle) const;
	/// Obtains the position within thumbnailRectangle of the Scan Image
	QRect getImageRectangle(const QRect& thumbnailRectangle) const;
	/// Obtains the position within thumbnailRectangle of the Scan Title
	QRect getTitleRectangle(const QRect& thumbnailRectangle) const;
	/// Obtains the position within the thumbnailRectangle of the Scan Start Date
	QRect getScanStartDateRectangle(const QRect& thumbnailRectangle) const;
	/// Obtains the position within the thumbnailRectangle of the Scan Technique
	QRect getScanTechniqueRectangle(const QRect& thumbnailRectangle) const;
private:
	/// The size of a single thumbnailView item
	QSize itemDimensions_;
	/// The size of the grid used to layout the thumbnailView items
	QSize gridDimensions_;
	/// The size of the actual thumbnail within the thumbnailView item
	QSize imageDimensions_;
	/// Selection drag widget
	QRubberBand* selectionRubberBand_;
	/// Point which stores where the current rubberBand selection was started (needed for rectangle normalization)
	QPoint rubberBandStart_;
	/// A hash which maps the grid row's current displayed thumbnail index. If non is stored, then 0 is assumed
	QHash<int, int> rowCurrentDisplayedThumbnailMap_;
	/// Timer used to determine if the mouse has been over a grid data row long enought to consider it hovering
	QTimer hoverTimer_;
	/// The grid data row index which the mouse is currently over
	int currentGridRowMouseOver_;
	/// Whether the mouse, in its current position, could be considered to be hovering
	bool isMouseHovering_;
};





#endif // AMSCANTHUMBNAILVIEW_H
