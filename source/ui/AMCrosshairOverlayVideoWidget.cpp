/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMCrosshairOverlayVideoWidget.h"
#include <QGraphicsLineItem>

#pragma clang diagnostic ignored "-Wunused-private-field"
#include <QResizeEvent>
#pragma clang diagnostic warning "-Wunused-private-field"

#include <QMediaObject>
#include <QGraphicsVideoItem>

 AMCrosshairOverlayVideoWidget::~AMCrosshairOverlayVideoWidget(){}
AMCrosshairOverlayVideoWidget::AMCrosshairOverlayVideoWidget(QWidget *parent, bool useOpenGlViewport) :
	AMOverlayVideoWidget(parent, useOpenGlViewport)
{
	crosshairX_ = 0.5;
	crosshairY_ = 0.5;

	QPen pen(Qt::red);

	crosshairXLine_ = scene()->addLine(0.5,0,0.5,1,pen);
	crosshairYLine_ = scene()->addLine(0,0.5,0,1,pen);

	crosshairXLine_->setVisible(false);
	crosshairYLine_->setVisible(false);

	reviewCrosshairLinePositions();

	doubleClickInProgress_ = false;

	connect(videoItem_, SIGNAL(nativeSizeChanged(QSizeF)), this, SLOT(reviewCrosshairLinePositions()));
}


void AMCrosshairOverlayVideoWidget::reviewCrosshairLinePositions()
{

	QSizeF viewSize = videoItem_->size();
	// first we need to find out the native size of the video. (Well, actually just the aspect ratio, but...)
	QSizeF videoSize = videoItem_->nativeSize();

	// scale the video size to the view size, obeying the transformation mode
	QSizeF scaledSize = videoSize;
	scaledSize.scale(viewSize, videoItem_->aspectRatioMode());

	// now, scaledSize will either be:
		// same as viewSize, if view and video have same aspect ratio, or the video is being stretched with Qt::IgnoreAspectRatio
		// One dimension the same, other dimension smaller than viewSize, if Qt::KeepAspectRatio
		// or One dimension the same, other dimension LARGER than viewSize, if Qt::KeepAspectRatioByExpanding

	QRectF activeRect = QRectF(QPointF((viewSize.width()-scaledSize.width())/2,
									   (viewSize.height()-scaledSize.height())/2),
							   scaledSize);

	// activeRect is now a rectangle in scene coordinates that covers the actual area of the video [not the area of the videoWidget, which may be smaller or larger depending on the aspect ratio mode and aspect ratio of the actual video feed]

	qreal xSceneCoord = activeRect.left() + crosshairX_*activeRect.width();
	qreal ySceneCoord = activeRect.top() + crosshairY_*activeRect.height();

	crosshairXLine_->setLine(xSceneCoord, activeRect.top(), xSceneCoord, activeRect.bottom());
	crosshairYLine_->setLine(activeRect.left(), ySceneCoord, activeRect.right(), ySceneCoord);
}

QPen AMCrosshairOverlayVideoWidget::crosshairPen() const
{
	return crosshairXLine_->pen();
}

bool AMCrosshairOverlayVideoWidget::crosshairVisible() const
{
	return crosshairXLine_->isVisible();
}

void AMCrosshairOverlayVideoWidget::setCrosshairPen(const QPen &pen)
{
	crosshairXLine_->setPen(pen);
	crosshairYLine_->setPen(pen);
}

void AMCrosshairOverlayVideoWidget::setCrosshairVisible(bool crosshairVisible)
{
	crosshairXLine_->setVisible(crosshairVisible);
	crosshairYLine_->setVisible(crosshairVisible);
}

void AMCrosshairOverlayVideoWidget::resizeEvent(QResizeEvent *event)
{
	AMOverlayVideoWidget::resizeEvent(event);

	reviewCrosshairLinePositions();
}

void AMCrosshairOverlayVideoWidget::mousePressEvent(QMouseEvent *e)
{
	AMOverlayVideoWidget::mousePressEvent(e);

	if(e->button() == Qt::LeftButton)
		emit mousePressed(mapSceneToVideo(mapToScene(e->pos())));
}

void AMCrosshairOverlayVideoWidget::mouseReleaseEvent(QMouseEvent *e)
{
	AMOverlayVideoWidget::mouseReleaseEvent(e);

	if(e->button() == Qt::LeftButton) {
		if(doubleClickInProgress_) {
			emit mouseDoubleClicked(mapSceneToVideo(mapToScene(e->pos())));
			doubleClickInProgress_ = false;
		}
		else {
			emit mouseReleased(mapSceneToVideo(mapToScene(e->pos())));
		}
	}
}

void AMCrosshairOverlayVideoWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
	AMOverlayVideoWidget::mouseDoubleClickEvent(e);

	if(e->button() == Qt::LeftButton) {
		doubleClickInProgress_ = true;
	}

}

QPointF AMCrosshairOverlayVideoWidget::mapSceneToVideo(const QPointF &sceneCoordinate) const
{
	// for more comments, see the more verbose implementation in reviewCrosshairLinePostions()
	QSizeF viewSize = videoItem_->size();
	QSizeF scaledSize = videoItem_->nativeSize();
	scaledSize.scale(viewSize, videoItem_->aspectRatioMode());

	QRectF activeRect = QRectF(QPointF((viewSize.width()-scaledSize.width())/2,
									   (viewSize.height()-scaledSize.height())/2),
							   scaledSize);

	// activeRect is now a rectangle in scene coordinates that covers the actual area of the video [not the area of the videoWidget, which may be smaller or larger depending on the aspect ratio mode and aspect ratio of the actual video feed]

	qreal yScene = (sceneCoordinate.y() - activeRect.top())/activeRect.height();
	qreal xScene = (sceneCoordinate.x() - activeRect.left())/activeRect.width();

	return QPointF(xScene, yScene);
}
