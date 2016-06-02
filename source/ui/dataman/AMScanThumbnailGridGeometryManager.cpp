#include "AMScanThumbnailGridGeometryManager.h"

AMScanThumbnailGridGeometryManager::AMScanThumbnailGridGeometryManager(int width)
	: AMGridFlowGeometryManager(width, GRID_CELL_WIDTH, GRID_CELL_HEIGHT,
								CONTENT_WIDTH, CONTENT_HEIGHT, Qt::AlignCenter)
{

}

QRect AMScanThumbnailGridGeometryManager::scanSerialRectangle(const QRect &contentRect) const
{

    int serialRectWidth = int(contentRect.width() * 0.2);
    int serialRectHeight = int(contentRect.height() * 0.1);
    int serialRectX = contentRect.x() + 10;
    int serialRectY = contentRect.y() + 10;

    return QRect(serialRectX, serialRectY, serialRectWidth, serialRectHeight);
}

QRect AMScanThumbnailGridGeometryManager::scanNameRectangle(const QRect &contentRect) const
{

    int nameRectWidth = int(contentRect.width() * 0.8);
	int nameRectHeight = int(contentRect.height() * 0.1);
    int nameRectX = contentRect.x() + 50;
	int nameRectY = contentRect.y() + 10;

	return QRect(nameRectX, nameRectY, nameRectWidth, nameRectHeight);
}

QRect AMScanThumbnailGridGeometryManager::scanDateRectangle(const QRect &contentRect) const
{

	int dateRectWidth = int(contentRect.width() * 0.9);
	int dateRectHeight = int(contentRect.height() * 0.1);
	int dateRectX = contentRect.x() + (contentRect.width() - dateRectWidth) /2;
	int dateRectY = contentRect.y() + 30;

	return QRect(dateRectX, dateRectY, dateRectWidth, dateRectHeight);
}

QRect AMScanThumbnailGridGeometryManager::scanTechniqueRectangle(const QRect &contentRect) const
{
	int techniqueRectWidth = int(contentRect.width() * 0.9);
	int techniqueRectHeight = int(contentRect.height() * 0.1);

	int techniqueRectX = contentRect.x() + (contentRect.width() - techniqueRectWidth) /2;
	int techniqueRectY = contentRect.y() + (contentRect.height() - 45);

	return QRect(techniqueRectX, techniqueRectY, techniqueRectWidth, techniqueRectHeight);
}

QRect AMScanThumbnailGridGeometryManager::thumbnailTitleRectangle(const QRect &contentRect) const
{
	int titleRectWidth = int(contentRect.width() * 0.9);
	int titleRectHeight = int(contentRect.height() * 0.1);

	int titleRectX = contentRect.x() + (contentRect.width() - titleRectWidth) / 2;
	int titleRectY = contentRect.y() + (contentRect.height() - 25);

	return QRect(titleRectX, titleRectY, titleRectWidth, titleRectHeight);
}

QRect AMScanThumbnailGridGeometryManager::thumbnailImageRectangle(const QRect &contentRect) const
{
	int imageRectWidth = int(contentRect.width() * 0.95);
	int imageRectHeight = int(contentRect.height() * 0.65);

	int imageRectX = contentRect.x() + (contentRect.width() - imageRectWidth) / 2;
	int imageRectY = contentRect.y() + 50;

	return QRect(imageRectX, imageRectY, imageRectWidth, imageRectHeight);
}
