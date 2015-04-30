#ifndef AMUNIFORMGRIDLAYOUT_H
#define AMUNIFORMGRIDLAYOUT_H

#include <QLayout>
#include <QList>
/**
 * A class used for layout of items in a grid, the cells of which are all of
 * uniform size. Items are added left to right, then top to bottom - with the
 * number of cells per row being dynamically determined from the grid's cell size
 * and the size of the layout itself.
 */
class AMUniformGridLayout : public QLayout
{
	Q_OBJECT
public:
	// Constructor

	/**
	 * Creates an AMUniformGridLayout with the provided cell width and height
	 * @param cellWidth::int ~ The width of each cell in the grid
	 * @param cellHeight::int ~ The height of each cell in the grid
	 * @param parent::QWidget ~ The parent QWidget of the AMUniformGridLayout
	 */
	AMUniformGridLayout(int cellWidth, int cellHeight, QWidget *parent = 0);

	// Desctructor

	/**
	 * Destructor for the AMUniformGridLayout. Deletes all the child items.
	 */
	virtual ~AMUniformGridLayout();

	// Override Methods

	/**
	 * Appends the provided QLayoutItem to the end of the layout
	 * @param item::QLayoutItem ~ The child item to add to the layout
	 */
	void addItem(QLayoutItem *item);

	/**
	 * Adds the provided QLayoutItem to the provided position in the layout. All
	 * subsequent layout items are moved one index up.
	 * @param item::QLayoutItem* ~ The layout item to add
	 * @param index::int ~ The index position to add the item at.
	 */
	void addItemAt(QLayoutItem* item, int index);

	/**
	 * The preferred size of this item
	 */
	QSize sizeHint() const;

	/**
	 * The smallest size that the layout can have
	 */
	QSize minimumSize() const;

	/**
	 * The number of child items within the layout
	 */
	int count() const;

	/**
	 * The index of the QLayoutItem at the provided point, or -1 if no
	 * QLayoutItem is found at the provided point.
	 * @param point::QPoint ~ The 2d point to return the QLayoutItem of
	 * NOTE: If a layout item is floating within a cell, and the point is in
	 * the padding area, then this is considered not on the QLayoutItem
	 */
	int layoutItemIndexAt(const QPoint& point);

	/**
	 * A list of all the indices of the QLayoutItems which are intersected by
	 * the provided rectangle.
	 * @param rect::QRect ~ The rectangle to check if each QLayoutItem intersects
	 */
	QList<int> layoutItemIndicesWithin(const QRect& rect);

	/**
	 * The child layout item at the provided index, or 0 if not item was
	 * found at the provided index.
	 * @param index::int ~ The index of the child item within the layout to
	 * return
	 */
	QLayoutItem* itemAt(int index) const;

	/**
	 * Removes and returns the child item within the layout at the provided
	 * index. If there is no such item, the function will do nothing and return
	 * 0
	 * @param index::int ~ The index of the child item within the layout to
	 * remove and return
	 * @return The child item at the provided index which was removed from the
	 * layout, or 0 if not item was found at the provided index.
	 */
	QLayoutItem* takeAt(int index);

	/**
	 * Sets this item's geometry to that of the provided rectangle
	 */
	void setGeometry(const QRect &rectangle);

	// Public Methods
	/**
	 * The height of an individual cell of the grid
	 */
	int cellHeight() const;

	/**
	 * Sets the height of all the grid cells
	 * @param cellHeight::int ~ The new height of all the grid cells
	 */
	void setCellHeight(int cellHeight);

	/**
	 * The width of an individual cell of the grid
	 */
	int cellWidth() const;

	/**
	 * Sets the width of all the grid cells
	 * @param cellWidth::int ~ The new width of all the grid cells
	 */
	void setCellWidth(int cellWidth);

	/**
	 * The current alignment of the child item in each cell
	 */
	Qt::Alignment cellContentAlignment() const;

	/**
	 * Sets the alignment of the child item in each cell
	 * @param cellContentAlignment::Qt::Alignment ~ The alignment flags which
	 * indicate where to place the child item within its cell
	 */
	void setCellContentAlignment(Qt::Alignment cellContentAlignment);

	/**
	 * The margin between the left most edge of the layout and the position
	 * of the first column of children.
	 */
	int marginLeft() const;
	/**
	 * Sets the margin between the left most edge of the layout and the position
	 * of the first column of children
	 * @param marginLeft::int ~ The new left margin width
	 */
	void setMarginLeft(int marginLeft);
	/**
	 * The margin between the right most edge of the layout and the position
	 * of the last column of children.
	 */
	int marginRight() const;
	/**
	 * Sets the margin between the right most edge of the layout and the position
	 * of the last column of children
	 * @param marginRight::int ~ The new right margin width
	 */
	void setMarginRight(int marginRight);
	/**
	 * The margin between the top most edge of the layout and the position
	 * of the first row of children
	 */
	int marginTop() const;
	/**
	 * Sets the margin between the top most edge of the layout and the position
	 * of the first row of children
	 * @param marginTop::int ~ The new top margin height
	 */
	void setMarginTop(int marginTop);
	/**
	 * The margin between the bottm most edge of the layout and the position
	 * of the last row of children
	 */
	int marginBottom() const;
	/**
	 * Sets the margin between the bottom most edge of the layout and the position
	 * of the last row of children
	 * @param marginBottom::int ~ The new bottom margin height
	 */
	void setMarginBotton(int marginBottom);

	/**
	 * The content rectangle within the cell at the provided index. Note, this
	 * does not return the whole cell rectangle, but the rectangle which
	 * encompasses the child within it
	 * @param cellIndex::int ~ The index of the cell whose child contents
	 * geometery is to be returned
	 * @return The rectangle of the child within the cell at the provided index
	 */
	QRect geometryWithinCellAt(int cellIndex);
protected:
	// Protected Methods
	/**
	 * The number of cells which are displayed in a row. This value depends
	 * on the current width of the layout, the width of the left and right margins,
	 * and the size specified for each cell.
	 */
	int cellsPerRow() const;
signals:
	
public slots:

private:
	Qt::Alignment cellContentAlignment_;
	QList<QLayoutItem*> children_;
	int cellWidth_;
	int cellHeight_;
	int marginLeft_;
	int marginRight_;
	int marginTop_;
	int marginBottom_;
	
};

#endif // AMUNIFORMGRIDLAYOUT_H
