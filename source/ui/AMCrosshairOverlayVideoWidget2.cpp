#include "AMCrosshairOverlayVideoWidget2.h"
#include <QGraphicsLineItem>
#include <QResizeEvent>

#include <QMap>
#include <QMediaObject>
#include <QGraphicsVideoItem>
#include <QDebug>
#include "ui/AMShapeOverlayVideoWidgetModel2.h"

AMCrosshairOverlayVideoWidget2::AMCrosshairOverlayVideoWidget2(QWidget *parent, bool useOpenGlViewport) :
    AMOverlayVideoWidget2(parent, useOpenGlViewport)
{
    shapeModel_ = new AMShapeOverlayVideoWidgetModel2();
	crosshairX_ = 0.5;
	crosshairY_ = 0.5;
    index_ = 0;



	QPen pen(QColor(Qt::red));

    QBrush brush(QColor(Qt::blue));

	crosshairXLine_ = scene()->addLine(0.5,0,0.5,1,pen);
	crosshairYLine_ = scene()->addLine(0,0.5,0,1,pen);

    rectangle_.insert(index_, scene()->addRect(0.5,0.5,20,20,pen,brush));

	reviewCrosshairLinePositions();

	doubleClickInProgress_ = false;

	connect(videoItem_, SIGNAL(nativeSizeChanged(QSizeF)), this, SLOT(reviewCrosshairLinePositions()));

	// Leave this up to user-programmers to decide if they want to move the crosshair with a double-click:
	// connect(this, SIGNAL(mouseDoubleClicked(QPointF)), this, SLOT(setCrosshairPosition(QPointF)));

    //mouse is not held down before signal connected

    //mouse press -
    connect(this, SIGNAL(mousePressed(QPointF)), shapeModel_, SLOT(startRectangle(QPointF)));
    connect(this, SIGNAL(mouseReleased(QPointF)),shapeModel_, SLOT(finishRectangle(QPointF)));
    connect(this, SIGNAL(mouseRightClicked(QPointF)), shapeModel_, SLOT(deleteRectangle(QPointF)));
    connect(this, SIGNAL(mouseMovePressed(QPointF)), shapeModel_, SLOT(selectCurrentRectangle(QPointF)));
    connect(this, SIGNAL(mouseMovedMoveMode(QPointF)), shapeModel_,SLOT(moveCurrentRectangle(QPointF)));



}


void AMCrosshairOverlayVideoWidget2::reviewCrosshairLinePositions()
{

	QSizeF viewSize = videoItem_->size();
	// first we need to find out the native size of the video. (Well, actually just the aspect ratio, but...)
	QSizeF videoSize = videoItem_->nativeSize();

	// scale the video size to the view size, obeying the transformation mode
	QSizeF scaledSize = videoSize;
	scaledSize.scale(viewSize, videoItem_->aspectRatioMode());

    shapeModel_->setViewSize(viewSize);
    shapeModel_->setScaledSize(scaledSize);
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

    if(index_ < shapeModel_->rectangleListLength())
    {
        addNewRectangle();
    }
    else if(index_ > shapeModel_->rectangleListLength())
    {
        deleteRectangle();
    }

    for(int i = 0; i < index_+ 1; i++)
    {
        qDebug()<<"index:"<<i<<" ,"<<shapeModel_->rectangleListLength()<<","<<index_;
        if(rectangle_.contains(i))
            rectangle_[i]->setRect(shapeModel_->rectangle(i));
        else
            qDebug()<<"Missing rectangle"<<i;
    }



}

QPen AMCrosshairOverlayVideoWidget2::crosshairPen() const
{
	return crosshairXLine_->pen();
}

bool AMCrosshairOverlayVideoWidget2::crosshairVisible() const
{
    return crosshairXLine_->isVisible();
}

void AMCrosshairOverlayVideoWidget2::setDrawMode()
{
    mode_ = DRAW;
}

void AMCrosshairOverlayVideoWidget2::setMoveMode()
{
    mode_ = MOVE;
}

void AMCrosshairOverlayVideoWidget2::setEditMode()
{
    mode_ = EDIT;
}

void AMCrosshairOverlayVideoWidget2::setCrosshairPen(const QPen &pen)
{
	crosshairXLine_->setPen(pen);
	crosshairYLine_->setPen(pen);
}

void AMCrosshairOverlayVideoWidget2::setCrosshairVisible(bool crosshairVisible)
{
	crosshairXLine_->setVisible(crosshairVisible);
	crosshairYLine_->setVisible(crosshairVisible);
}

void AMCrosshairOverlayVideoWidget2::resizeEvent(QResizeEvent *event)
{
    AMOverlayVideoWidget2::resizeEvent(event);

	reviewCrosshairLinePositions();
}

void AMCrosshairOverlayVideoWidget2::mousePressEvent(QMouseEvent *e)
{
    AMOverlayVideoWidget2::mousePressEvent(e);

    if(e->button() == Qt::LeftButton && mode_ == DRAW)
    {
		emit mousePressed(mapSceneToVideo(mapToScene(e->pos())));
        connect(this,SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(finishRectangle(QPointF)));
    }
    else if(e->button() == Qt::LeftButton && mode_ == MOVE)
    {
        emit mouseMovePressed(mapSceneToVideo(mapToScene(e->pos())));
    }
    else if (e->button() == Qt::RightButton)
        emit mouseRightClicked(mapSceneToVideo(mapToScene(e->pos())));

    reviewCrosshairLinePositions();
}

void AMCrosshairOverlayVideoWidget2::mouseReleaseEvent(QMouseEvent *e)
{
    AMOverlayVideoWidget2::mouseReleaseEvent(e);

	if(e->button() == Qt::LeftButton) {
		if(doubleClickInProgress_) {
			emit mouseDoubleClicked(mapSceneToVideo(mapToScene(e->pos())));
			doubleClickInProgress_ = false;
		}
		else {
			emit mouseReleased(mapSceneToVideo(mapToScene(e->pos())));
		}


        disconnect(shapeModel_, SLOT(finishRectangle(QPointF)));
         reviewCrosshairLinePositions();

	}
}

void AMCrosshairOverlayVideoWidget2::mouseDoubleClickEvent(QMouseEvent *e)
{
    AMOverlayVideoWidget2::mouseDoubleClickEvent(e);

	if(e->button() == Qt::LeftButton) {
		doubleClickInProgress_ = true;
	}

}

QPointF AMCrosshairOverlayVideoWidget2::mapSceneToVideo(const QPointF &sceneCoordinate) const
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



void AMCrosshairOverlayVideoWidget2::mouseMoveEvent(QMouseEvent *e)
{
    AMOverlayVideoWidget2::mouseMoveEvent(e);

    if(e->MouseMove)
    {
        emit mouseMoved(mapSceneToVideo(mapToScene(e->pos())));
        if(mode_ == MOVE)
        {
            emit mouseMovedMoveMode(mapSceneToVideo(mapToScene(e->pos())));
        }
    }
     reviewCrosshairLinePositions();
}


void AMCrosshairOverlayVideoWidget2::addNewRectangle()
{
    index_++;
    QPen pen(QColor(Qt::red));

    QBrush brush(QColor(Qt::blue));

    rectangle_.insert(index_, scene()->addRect(0.5,0.5,20,20,pen,brush));


}

void AMCrosshairOverlayVideoWidget2::deleteRectangle()
{
    scene()->removeItem(rectangle_[index_]);
    rectangle_.remove(index_);
    index_--;
}
