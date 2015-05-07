#ifndef AMSCANTHUMNAILGRIDGEOMETRYMANAGER_H
#define AMSCANTHUMNAILGRIDGEOMETRYMANAGER_H

#include "ui/util/AMGridFlowGeometryManager.h"

#define GRID_CELL_WIDTH		265
#define GRID_CELL_HEIGHT	290
#define CONTENT_WIDTH		250
#define CONTENT_HEIGHT		275

/**
 * A Geometry manager class which is specifically designed for the AMScanThumbnailGridView.
 * Extends the AMGridFlowGeometryManager to, as well as answering the usual geometry
 * questions about flow geometry, also answer questions regarding the specific geometry
 * for the fields required by AMScanThumbnailGridView
 */
class AMScanThumbnailGridGeometryManager : public AMGridFlowGeometryManager
{
public:
	/**
	 * Creates a new instance of an AMScanThumbnailGridGeometryManager which manages
	 * an area of the provided width
	 * @param width::int ~ The width of the area which the geometry manager will
	 * be managing.
	 */
	explicit AMScanThumbnailGridGeometryManager(int width);

	/**
	 * The rectangle for displaying the scan name within the provided content
	 * rectangle
	 * @param contentRect::QRect ~ The rectangle which the scan name is to be
	 * displayed in
	 */
	QRect scanNameRectangle(const QRect& contentRect) const;

	/**
	 * The rectangle for displaying the scan date within the provided content
	 * rectangle
	 * @param contentRect::QRect ~ The rectangle which the scan date is to be
	 * displayed in
	 */
	QRect scanDateRectangle(const QRect& contentRect) const;

	/**
	 * The rectangle for displaying the scan technique within the provided content
	 * rectangle
	 * @param contentRect::QRect ~ The rectangle which the scan technique is to be
	 * displayed in
	 */
	QRect scanTechniqueRectangle(const QRect& contentRect) const;

	/**
	 * The rectangle for displaying the thumbnail title within the provided content
	 * rectangle
	 * @param contentRect::QRect ~ The rectangle which the thumbnail title is to be
	 * displayed in
	 */
	QRect thumbnailTitleRectangle(const QRect& contentRect) const;

	/**
	 * The rectangle for displaying the thumbnail image within the provided content
	 * rectangle
	 * @param contentRect::QRect ~ The rectangle which the thumbnail image is to be
	 * displayed in
	 */
	QRect thumbnailImageRectangle(const QRect& contentRect) const;
};

#endif // AMSCANTHUMNAILGRIDGEOMETRYMANAGER_H
