#include "AMScanThumnailGridGeometryManager.h"

AMScanThumnailGridGeometryManager::AMScanThumnailGridGeometryManager(int width)
	: AMGridFlowGeometryManager(width, GRID_CELL_WIDTH, GRID_CELL_HEIGHT,
								THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, Qt::AlignCenter) {

}

QRect AMScanThumnailGridGeometryManager::scanNameRectangle(const QRect &contentRect) const {

	int nameRectWidth = contentRect.width() * 0.9;
	int nameRectHeight = contentRect.height() * 0.1;
	int nameRectX = contentRect.x() + (contentRect.width() - nameRectWidth) /2;
	int nameRectY = contentRect.y() + 10;

	return QRect(nameRectX, nameRectY, nameRectWidth, nameRectHeight);
}

QRect AMScanThumnailGridGeometryManager::scanDateRectangle(const QRect &contentRect) const {

	int dateRectWidth = contentRect.width() * 0.9;
	int dateRectHeight = contentRect.height() * 0.1;
	int dateRectX = contentRect.x() + (contentRect.width() - dateRectWidth) /2;
	int dateRectY = contentRect.y() + 30;

	return QRect(dateRectX, dateRectY, dateRectWidth, dateRectHeight);
}

QRect AMScanThumnailGridGeometryManager::scanTechniqueRectangle(const QRect &contentRect) const {
	int techniqueRectWidth = contentRect.width() * 0.9;
	int techniqueRectHeight = contentRect.height() * 0.1;

	int techniqueRectX = contentRect.x() + (contentRect.width() - techniqueRectWidth) /2;
	int techniqueRectY = contentRect.y() + (contentRect.height() - 45);

	return QRect(techniqueRectX, techniqueRectY, techniqueRectWidth, techniqueRectHeight);
}

QRect AMScanThumnailGridGeometryManager::thumbnailTitleRectangle(const QRect &contentRect) const {
	int titleRectWidth = contentRect.width() * 0.9;
	int titleRectHeight = contentRect.height() * 0.1;

	int titleRectX = contentRect.x() + (contentRect.width() - titleRectWidth) / 2;
	int titleRectY = contentRect.y() + (contentRect.height() - 25);

	return QRect(titleRectX, titleRectY, titleRectWidth, titleRectHeight);
}

QRect AMScanThumnailGridGeometryManager::thumbnailImageRectangle(const QRect &contentRect) const {
	int imageRectWidth = contentRect.width() * 0.9;
	int imageRectHeight = contentRect.height() * 0.65;

	int imageRectX = contentRect.x() + (contentRect.width() - imageRectWidth) / 2;
	int imageRectY = contentRect.y() + 50;

	return QRect(imageRectX, imageRectY, imageRectWidth, imageRectHeight);
}
