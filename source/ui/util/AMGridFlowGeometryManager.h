#ifndef AMGRIDFLOWGEOMETRYMANAGER_H
#define AMGRIDFLOWGEOMETRYMANAGER_H

#include <QRect>
#include <QList>
/**
 * Class which handles all the geometry information for a GridFlow layout (ie
 * contains a grid of items, the cells of which are uniform sizes and the number
 * of cells in a given row is dynamically calculated based on the current width
 * of the layout.
 * NOTE: This doesn't actually store any of the child items, instead it serves
 * as a helper class to answer geometry questions to assist in performing a
 * layout or performing manual painting.
 */
class AMGridFlowGeometryManager
{
public:

	/**
	 * Creates a new AMGridFlowGeometryManager with cells of the provided height
	 * and width, to fill a space of the provided width and height
	 * @param width::int ~ The width of the space being managed
	 * @param itemWidth::int ~ The width of the widget item whose geometry is
	 * being managed
	 * @param itemHeight::int ~ The height of the widget item whose geomerty is
	 * being managed
	 * @param cellWidth::int ~ The width of each cell within the grid
	 * @param cellHeight::int ~ The height of each cell within the grid
	 */
	AMGridFlowGeometryManager(int width, int cellWidth, int cellHeight,
							  int itemWidth, int itemHeight,
							  Qt::Alignment cellContentAlignment = Qt::AlignCenter);

	virtual ~AMGridFlowGeometryManager() {}
	/**
	 * The width of the whole space being managed
	 */
	int width() const;

	/**
	 * The height of the whole space being managed when storing the provided number
	 * of items
	 * @param cellCount::int ~ The number of items being stored
	 */
	int heightFor(int itemCount) const;

	/**
	 * Sets the width of the whole space being managed
	 * @param width::int ~ The width of the whole space being managed
	 */
	void setWidth(int width);

	/**
	 * The width of a single cell within the grid
	 */
	int cellWidth() const;

	/**
	 * Sets the width of a single cell within the grid
	 * @param cellWidth::int ~ The new width of a cell within the grid
	 */
	void setCellWidth(int cellWidth);

	/**
	 * The height of a single cell within the grid
	 */
	int cellHeight() const;

	/**
	 * Sets the height of a single cell within the grid
	 * @param cellHeight::int ~ The new height of a single cell within the grid
	 */
	void setCellHeight(int cellHeight);

	/**
	 * The width of the item within each cell
	 */
	int itemWidth() const;

	/**
	 * The height of the item within each cell
	 */
	int itemHeight() const;

	/**
	 * The gap between the left most side of the layout and the first column of
	 * the grid
	 */
	int marginLeft() const;

	/**
	 * Sets the gap between the left most side of the layout and the first column
	 * of the grid
	 * @param margineLeft::int ~ The new gap between the left most side of the
	 * layout and the first column of the grid
	 */
	void setMarginLeft(int marginLeft);

	/**
	 * The gap between the last column and the right most side of the layout
	 */
	int marginRight() const;

	/**
	 * Sets the gap between the last column and the right most side of the layout
	 * @param marginRight::int ~ The new gap between the last column and the right
	 * most side of the layout
	 */
	void setMarginRight(int marginRight);

	/**
	 * The gap between the top side of the layout and the first row of the grid
	 */
	int marginTop() const;

	/**
	 * Sets the gap between the top side of the layout and the first row of the
	 * grid
	 * @param marginTop::int ~ The new gap between the top side of the layout
	 * and the first row of the grid
	 */
	void setMarginTop(int marginTop);

	/**
	 * The gap between the last row of the grid and the bottom edge of the layout
	 */
	int marginBottom() const;

	/**
	 * Sets the gap between the last row of the grid and the bottom edge of the
	 * layout
	 * @param marginBottom::int ~ The new gap between the last row of the grid
	 * and the bottom edge of the layout
	 */
	void setMarginBottom(int marginBottom);

	/**
	 * How an item is arranged within each grid cell
	 */
	Qt::Alignment cellContentAlignment() const;

	/**
	 * Sets how items are arranged within each grid cell
	 * @param cellContentAlignment
	 */
	void setCellContentAlignment(Qt::Alignment cellContentAlignment);

	/**
	 * The index of the content rectangle at the given point, or -1 if the point lies between
	 * items or in the margins
	 * @param point::QPoint ~ The point to return the content index underneath
	 */
	int contentIndexAt(const QPoint& point, int horizontalOffset = 0,
				int verticalOffset = 0) const;

	/**
	  * The index of the cell at the given point
	  * @param point::QPoint ~ The point to return the cell index underneat
	  */
	int cellIndexAt(const QPoint& point, int horizontalOffset = 0,
					int verticalOffset = 0) const;

	/**
	 * Returns a list of the indices of cells which are contained within the
	 * provided rectangle.
	 * @param rect::QRect ~ The rectangle to check for cell indices within
	 * @return The list of indices which are contained within the past rectangle
	 */
	QList<int> indicesWithin(const QRect& rect, int horizontalOffset = 0,
							 int verticalOffset = 0) const;

	/**
	 * The rectangle which represents the whole cell at the provided index.
	 * @param index::int ~ The index of the cell whose rectangle geometry is to
	 * be returned
	 */
	QRect cellGeometryAt(int index, int horizontalOffset = 0,
						 int verticalOffset = 0) const;

	/**
	 * The rectangle which represents the whole cell at the given co ordinate
	 * point
	 * @param point::QPoint ~ The co ordinate point which is within the returned
	 * rectangle
	 */
	QRect cellGeometryAt(const QPoint& point, int horizontalOffset = 0,
						 int verticalOffset = 0) const;

	/**
	 * The rectangle which represents the item contained within the grid cell
	 * at the provided index.
	 * @param index::int ~ The index of the cell whose contents geometry is to be
	 * returned
	 */
	QRect contentGeometryAt(int index, int horizontalOffset = 0,
							int verticalOffset = 0) const;

	/**
	 * The rectangle which represents the item contained within the grid cell
	 * at the given co ordinates. If the co ordinates are not over an item in
	 * the grid cell, then an invalid QRect is returned
	 * @param point::QPoint ~ The co ordinates of the item whose rectangle is to
	 * be returned
	 */
	QRect contentGeometryAt(const QPoint& point, int horizontalOffset = 0,
							int verticalOffset = 0) const;

protected:
	/**
	 * Helper method which calculates the number of cells in a given full row
	 * taking into account the width of a cell, the width of the layout and
	 * the margins - left and right
	 */
	int cellsPerRow() const;

	/**
	 * The entire rectangle which is being managed
	 */
	QRect managedGeometry() const;

	/**
	 * Helper method which calculates the content geometry for an item within
	 * a cell based on the current cellContentAlignment, the cell size and the
	 * item size.
	 */
	QRect contentGeometryFromCell(const QRect& cellGeometry) const;

	int width_;
	int cellWidth_;
	int cellHeight_;
	int itemWidth_;
	int itemHeight_;
	int marginLeft_;
	int marginRight_;
	int marginTop_;
	int marginBottom_;
	Qt::Alignment cellContentAlignment_;
};

#endif // AMGRIDFLOWGEOMETRYMANAGER_H
