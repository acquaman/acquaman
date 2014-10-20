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


#include "ui/beamline/camera/AMSampleCameraGraphicsView.h"

#ifdef AM_MOBILITY_VIDEO_ENABLED
#include <QGraphicsVideoItem>
#endif

#pragma GCC diagnostic ignored "-Wunused-private-field"
#include <QMouseEvent>
#pragma GCC diagnostic warning "-Wunused-private-field"

#include <QDebug>

AMSampleCameraGraphicsView::AMSampleCameraGraphicsView(QWidget *parent, bool useOpenGlViewport) :
	AMOverlayVideoWidget2(parent, useOpenGlViewport)
{

}


void AMSampleCameraGraphicsView::resizeEvent(QResizeEvent *event)
{
	AMOverlayVideoWidget2::resizeEvent(event);

	emit resize();

}


void AMSampleCameraGraphicsView::mousePressEvent(QMouseEvent *event)
{
	AMOverlayVideoWidget2::mousePressEvent(event);

	if(event->button() == Qt::LeftButton)
	{
		emit mousePressed(mapSceneToVideo(mapToScene(event->pos())));

	}
	else if (event->button() == Qt::RightButton)
		emit mouseRightClicked(mapSceneToVideo(mapToScene(event->pos())));
}

void AMSampleCameraGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	AMOverlayVideoWidget2::mouseReleaseEvent(event);

	if(event->button() == Qt::LeftButton)
		emit mouseLeftReleased(mapSceneToVideo(mapToScene(event->pos())));
	else if(event->button() == Qt::RightButton)
		emit mouseRightReleased(mapSceneToVideo(mapToScene(event->pos())));
}

void AMSampleCameraGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
	AMOverlayVideoWidget2::mouseDoubleClickEvent(event);

	if(event->button() == Qt::LeftButton)
		emit mouseDoubleClicked(mapSceneToVideo(mapToScene(event->pos())));

}

void AMSampleCameraGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	AMOverlayVideoWidget2::mouseMoveEvent(event);
	if(event->MouseMove)
	{
		emit mouseMoved(mapSceneToVideo(mapToScene(event->pos())));
	}
}

#ifdef AM_MOBILITY_VIDEO_ENABLED
void AMSampleCameraGraphicsView::setVideoItem(QGraphicsVideoItem *item)
{
	if(videoItem_)
		scene()->removeItem(videoItem_);
	videoItem_->deleteLater();
	videoItem_ = item;
	scene()->addItem(videoItem_);
	mediaPlayer()->setVideoOutput(videoItem_);
}
#endif


QPointF AMSampleCameraGraphicsView::mapSceneToVideo(const QPointF &sceneCoordinate) const
{
	// for more comments, see the more verbose implementation in reviewCrosshairLinePostions()
	QSizeF viewSize = sceneRect().size();
	#ifdef AM_MOBILITY_VIDEO_ENABLED
	QSizeF scaledSize = videoItem_->nativeSize();
	scaledSize.scale(viewSize, videoItem_->aspectRatioMode());
	double videoTop = videoItem_->boundingRect().topLeft().y();
	#else
	QSizeF scaledSize(1, 1);
	double videoTop = 0;
	#endif

	QRectF activeRect = QRectF(QPointF((viewSize.width()-scaledSize.width())/2, videoTop),
				   //                                       (viewSize.height()-scaledSize.height())/2),
				   scaledSize);

	// activeRect is now a rectangle in scene coordinates that covers the actual area of the video [not the area of the videoWidget, which may be smaller or larger depending on the aspect ratio mode and aspect ratio of the actual video feed]

	qreal yScene = (sceneCoordinate.y() - activeRect.top())/activeRect.height();
	qreal xScene = (sceneCoordinate.x() - activeRect.left())/activeRect.width();

	return QPointF(xScene, yScene);
}

QPointF AMSampleCameraGraphicsView::mapVideoToScene(const QPointF &videoCoordinate) const
{

	QSizeF viewSize = sceneRect().size();

	#ifdef AM_MOBILITY_VIDEO_ENABLED
	QSizeF scaledSize = videoItem_->nativeSize();
	scaledSize.scale(viewSize, videoItem_->aspectRatioMode());
	double videoTop = videoItem_->boundingRect().topLeft().y();
	#else
	QSizeF scaledSize(1, 1);
	double videoTop = 0;
	#endif

	QRectF activeRect = QRectF(QPointF((viewSize.width()-scaledSize.width())/2, videoTop),
				   //                                       (viewSize.height()-scaledSize.height())/2),
				   scaledSize);

	QPointF fixedCoordinate;
	qreal xCoord = activeRect.left() + videoCoordinate.x()*activeRect.width();
	qreal yCoord = activeRect.top() + videoCoordinate.y()*activeRect.height();
	fixedCoordinate.setX(xCoord);
	fixedCoordinate.setY(yCoord);
	return fixedCoordinate;
}

void AMSampleCameraGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
	Q_UNUSED(event)
	/// do nothing
}


