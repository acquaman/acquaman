#include "ui/beamline/camera/AMSampleCameraGraphicsView.h"

#include <QMouseEvent>
#include <QGraphicsVideoItem>
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
        emit mouseMoved(mapSceneToVideo(mapToScene(event->pos())));
}

void AMSampleCameraGraphicsView::setVideoItem(QGraphicsVideoItem *item)
{
    videoItem_ = item;
}


QPointF AMSampleCameraGraphicsView::mapSceneToVideo(const QPointF &sceneCoordinate) const
{
    // for more comments, see the more verbose implementation in reviewCrosshairLinePostions()
    QSizeF viewSize = sceneRect().size();
    QSizeF scaledSize = videoItem_->nativeSize();
    scaledSize.scale(viewSize, videoItem_->aspectRatioMode());
    double videoTop = videoItem_->boundingRect().topLeft().y();

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
	QSizeF scaledSize = videoItem_->nativeSize();
	scaledSize.scale(viewSize, videoItem_->aspectRatioMode());
	double videoTop = videoItem_->boundingRect().topLeft().y();

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
    /// do nothing;
}


