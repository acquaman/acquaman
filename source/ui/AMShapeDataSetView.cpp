#include "AMShapeDataSetView.h"
#include <QGraphicsLineItem>
#include <QResizeEvent>
#include <QGraphicsItem>

#include <QMap>
#include <QMediaObject>
#include <QGraphicsVideoItem>
#include <QDebug>
#include "ui/AMShapeDataSet.h"
#include "ui/AMCameraConfiguration.h"


QColor const AMShapeDataSetView::BORDERCOLOUR = QColor(Qt::red);
QColor const AMShapeDataSetView::ACTIVEBORDERCOLOUR = QColor(Qt::blue);


AMShapeDataSetView::AMShapeDataSetView(QWidget *parent, bool useOpenGlViewport) :
    AMOverlayVideoWidget2(parent, useOpenGlViewport)
{
    shapeModel_ = new AMShapeDataSet();
	crosshairX_ = 0.5;
	crosshairY_ = 0.5;
    index_ = 0;
    groupRectangleActive_= false;


    QPen pen(BORDERCOLOUR);

    QBrush brush(QColor(Qt::transparent));

	crosshairXLine_ = scene()->addLine(0.5,0,0.5,1,pen);
	crosshairYLine_ = scene()->addLine(0,0.5,0,1,pen);

    QPolygonF polygon(QRectF(5, 5, 0, 0));

    shapes_.insert(index_, scene()->addPolygon(polygon,pen,brush));

	reviewCrosshairLinePositions();

	doubleClickInProgress_ = false;

	connect(videoItem_, SIGNAL(nativeSizeChanged(QSizeF)), this, SLOT(reviewCrosshairLinePositions()));

	// Leave this up to user-programmers to decide if they want to move the crosshair with a double-click:
	// connect(this, SIGNAL(mouseDoubleClicked(QPointF)), this, SLOT(setCrosshairPosition(QPointF)));

    //mouse is not held down before signal connected

    //mouse press -
    connect(this, SIGNAL(mousePressed(QPointF)), shapeModel_, SLOT(startRectangle(QPointF)));
    connect(this, SIGNAL(mouseRightClicked(QPointF)), shapeModel_, SLOT(deleteRectangle(QPointF)));
    connect(this, SIGNAL(mouseMovePressed(QPointF)), shapeModel_, SLOT(selectCurrentShape(QPointF)));
    connect(this, SIGNAL(mouseMoveRightPressed(QPointF)), shapeModel_, SLOT(selectCurrentShape(QPointF)));
    connect(this, SIGNAL(mouseEditPressed(QPointF)), shapeModel_, SLOT(selectCurrentShape(QPointF)));
    connect(this, SIGNAL(mouseEditRightPressed(QPointF)), shapeModel_, SLOT(setZoomPoint(QPointF)));
    connect(this, SIGNAL(mouseEditRightPressed(QPointF)), shapeModel_, SLOT(selectCurrentShape(QPointF)));
    connect(this, SIGNAL(mouseShiftPressed(QPointF)), shapeModel_, SLOT(setShapeVectors(QPointF)));
    connect(this, SIGNAL(mouseShiftRightPressed(QPointF)), shapeModel_, SLOT(setZoomPoint(QPointF)));
    connect(this, SIGNAL(mouseOperationPressed(QPointF,QPointF)), shapeModel_, SLOT(shiftToPoint(QPointF,QPointF)));
    connect(this, SIGNAL(mouseOperationSelect(QPointF)), shapeModel_, SLOT(selectCurrentShape(QPointF)));
    connect(this, SIGNAL(mouseGroupPressed(QPointF)), shapeModel_, SLOT(startGroupRectangle(QPointF)));
    connect(this, SIGNAL(setCoordinate(double,double,double)), shapeModel_, SLOT(setCoordinates(double,double,double)));

    connect(this, SIGNAL(oneSelect()), shapeModel_, SLOT(oneSelect()));
    connect(this, SIGNAL(twoSelect()), shapeModel_, SLOT(twoSelect()));
    connect(shapeModel_, SIGNAL(beamChanged(QObject*)), this, SIGNAL(beamChanged(QObject*)));
}


void AMShapeDataSetView::reviewCrosshairLinePositions()
{

	QSizeF viewSize = videoItem_->size();
	// first we need to find out the native size of the video. (Well, actually just the aspect ratio, but...)
	QSizeF videoSize = videoItem_->nativeSize();

    // scale the video size to the view size, obeying the transformatiotretchn mode
	QSizeF scaledSize = videoSize;
	scaledSize.scale(viewSize, videoItem_->aspectRatioMode());

    shapeModel_->setViewSize(viewSize);
    shapeModel_->setScaledSize(scaledSize);
	// now, scaledSize will either be:
		// same as viewSize, if view and video have same aspect ratio, or the video is being stretched with Qt::IgnoreAspectRatio
        // One dimension the same, other dimension smalmouseShiftPressedler than viewSize, if Qt::KeepAspectRatio
		// or One dimension the same, other dimension LARGER than viewSize, if Qt::KeepAspectRatioByExpanding

	QRectF activeRect = QRectF(QPointF((viewSize.width()-scaledSize.width())/2,
                                       (viewSize.height()-scaledSize.height())/2),
                                        scaledSize);

    // activeRect is now a rectangle in scene coordinates that covers the actual area of the video [not the area of the videoWidget, which may be smaller or larger depending on the aspect ratio mode and aspect ratio of the actual video feed]

	qreal xSceneCoord = activeRect.left() + crosshairX_*activeRect.width();
	qreal ySceneCoord = activeRect.top() + crosshairY_*activeRect.height();



    crosshairXLine_->setLine(xSceneCoord, activeRect.top(), xSceneCoord, activeRect.bottom());
	crosshairYLine_->setLine(activeRect.left(), ySceneCoord, activeRect.right(), ySceneCoord);

    qDebug()<<"Looking for new Shapes";
    if(index_ < shapeModel_->shapeListLength())
    {
        while(index_<shapeModel_->shapeListLength())
        {
            addNewShape();
        }
    }
    else if(index_ > shapeModel_->shapeListLength())
    {
        while(index_ > shapeModel_->shapeListLength()) deleteShape();
        if(shapeModel_->isValid(current_))
            shapes_[current_]->setPen(QColor(Qt::red));
        current_ = -1;
        shapeModel_->setCurrentIndex(current_);
        emit currentChanged();
    }

    for(int i = 0; i < index_+ 1; i++)
    {
        if(shapes_.contains(i))
        {
            shapes_[i]->setPolygon(shapeModel_->shape(i));
        }
        else
            qDebug()<<"Missing shape"<<i;
    }

    if(groupRectangleActive_)
    {
        qDebug()<<"Changing the group rectangle";
        groupRectangle_->setPolygon(shapeModel_->groupRectangle());
    }

   /// print the intersection shapes
    {
        if(!intersections_.isEmpty())
        {
            intersection();
        }
    }


}

QPen AMShapeDataSetView::crosshairPen() const
{
	return crosshairXLine_->pen();
}

bool AMShapeDataSetView::crosshairVisible() const
{
    return crosshairXLine_->isVisible();
}

void AMShapeDataSetView::setDrawMode()
{
    mode_ = DRAW;
}

void AMShapeDataSetView::setMoveMode()
{
    mode_ = MOVE;
}

void AMShapeDataSetView::setEditMode()
{
    mode_ = EDIT;
}

void AMShapeDataSetView::setShiftMode()
{
    mode_ = SHIFT;
}

void AMShapeDataSetView::setOperationMode()
{
    mode_ = OPERATION;
}

void AMShapeDataSetView::setGroupMode()
{
    mode_ = GROUP;
}

void AMShapeDataSetView::setMotorCoordinate(double x, double y, double z, double r)
{
    shapeModel_->setMotorCoordinate(x,y,z,r);
    emit currentChanged();
    reviewCrosshairLinePositions();
}

double AMShapeDataSetView::motorRotation()
{
    return shapeModel_->motorRotation();
}

double AMShapeDataSetView::motorX()
{
    return shapeModel_->motorX();
}

double AMShapeDataSetView::motorY()
{
    return shapeModel_->motorY();
}

double AMShapeDataSetView::motorZ()
{
    return shapeModel_->motorZ();
}

QString AMShapeDataSetView::currentName()
{
    return shapeModel_->currentName();
}

void AMShapeDataSetView::setCurrentName(QString name)
{
   shapeModel_->setCurrentName(name);
}

QString AMShapeDataSetView::currentInfo()
{
    return shapeModel_->currentInfo();
}

void AMShapeDataSetView::setCurrentInfo(QString info)
{
    shapeModel_->setCurrentInfo(info);
}

int AMShapeDataSetView::currentIndex()
{
    return shapeModel_->currentIndex();
}

double AMShapeDataSetView::xCoordinate()
{
    double coordinate = shapeModel_->currentCoordinate().x();
    return coordinate;
}

double AMShapeDataSetView::yCoordinate()
{
    return shapeModel_->currentCoordinate().y();
}

double AMShapeDataSetView::zCoordinate()
{

    return shapeModel_->currentCoordinate().z();
}

double AMShapeDataSetView::rotation()
{
    return shapeModel_->rotation();
}

double AMShapeDataSetView::tilt()
{
    return shapeModel_->tilt();
}

void AMShapeDataSetView::setX(double x)
{

   QVector3D currentCoordinate = shapeModel_->currentCoordinate();
   double y = currentCoordinate.y();
   double z = currentCoordinate.z();
   shapeModel_->setCoordinates(x,y,z);
}

void AMShapeDataSetView::setY(double y)
{
    QVector3D currentCoordinate = shapeModel_->currentCoordinate();
    double x = currentCoordinate.x();
    double z = currentCoordinate.z();
    shapeModel_->setCoordinates(x,y,z);
}

void AMShapeDataSetView::setZ(double z)
{
    QVector3D currentCoordinate = shapeModel_->currentCoordinate();
    double y = currentCoordinate.y();
    double x = currentCoordinate.x();
    shapeModel_->setCoordinates(x,y,z);
}

void AMShapeDataSetView::setRotation(double rotation)
{
    shapeModel_->setRotation(rotation);
}

void AMShapeDataSetView::setTilt(double tilt)
{
    shapeModel_->setTilt(tilt);
}

void AMShapeDataSetView::moveCurrentToCoordinate()
{
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::toggleDistortion()
{
    shapeModel_->toggleDistortion();
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::setCrosshairPen(const QPen &pen)
{
	crosshairXLine_->setPen(pen);
	crosshairYLine_->setPen(pen);
}

void AMShapeDataSetView::setCrosshairVisible(bool crosshairVisible)
{
	crosshairXLine_->setVisible(crosshairVisible);
    crosshairYLine_->setVisible(crosshairVisible);
}

void AMShapeDataSetView::setCameraModel(AMCameraConfiguration *model)
{
    shapeModel_->setCameraModel(model);
    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::intersection()
{
    if(shapeModel_->findIntersections())
    {
        if(!intersections_.isEmpty()) clearIntersections();
        createIntersectionShapes(shapeModel_->intersections());
    }
}

void AMShapeDataSetView::createIntersectionShapes(QVector<QPolygonF> shapes)
{
    intersections_.clear();
    for(int i =0; !shapes.isEmpty(); i++)
    {
        qDebug()<<"Attempting to draw intersections";
        QPen pen(BORDERCOLOUR);
        QBrush brush(QColor(Qt::yellow));
        QPolygonF polygon(QRectF(5,5,20,20));
        intersections_<<scene()->addPolygon(polygon,pen,brush);
        intersections_[i]->setPolygon(shapes.first());
        shapes.remove(0);
    }
}

void AMShapeDataSetView::clearIntersections()
{
    while(!intersections_.isEmpty())
    {
        scene()->removeItem(intersections_.first());
        intersections_.remove(0);
    }
}




void AMShapeDataSetView::resizeEvent(QResizeEvent *event)
{
    AMOverlayVideoWidget2::resizeEvent(event);

	reviewCrosshairLinePositions();
}

void AMShapeDataSetView::mousePressEvent(QMouseEvent *e)
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
        currentSelectionChanged();
        connect(this, SIGNAL(mouseMovedMoveMode(QPointF)), shapeModel_,SLOT(moveCurrentShape(QPointF)));
        connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
    }
    else if (e->button() == Qt::RightButton && mode_ == MOVE)
    {
        emit mouseMoveRightPressed(mapSceneToVideo(mapToScene(e->pos())));
        currentSelectionChanged();
        connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(rotateRectangle(QPointF)));
        connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
    }
    else if(e->button() == Qt::LeftButton && mode_ == EDIT)
    {
        emit mouseEditPressed(mapSceneToVideo(mapToScene(e->pos())));
        currentSelectionChanged();
        connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(finishRectangle(QPointF)));
        connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
    }
    else if (e->button() == Qt::RightButton && mode_ == EDIT)
    {
        emit mouseEditRightPressed(mapSceneToVideo(mapToScene(e->pos())));
        currentSelectionChanged();
        connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(zoomShape(QPointF)));
        connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
    }

    else if(e->button() == Qt::LeftButton && mode_ == SHIFT)
    {
        emit mouseShiftPressed(mapSceneToVideo(mapToScene(e->pos())));
        connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(moveAllShapes(QPointF)));
        connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
    }
    else if (e->button() == Qt::RightButton && mode_ == SHIFT)
    {
        emit mouseShiftRightPressed(mapSceneToVideo(mapToScene(e->pos())));
        connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(zoomAllShapes(QPointF)));
        connect(this, SIGNAL(mouseMoved(QPointF)), this, SIGNAL(currentChanged()));
    }
    else if (e->button() == Qt::LeftButton && mode_ == OPERATION)
    {
        emit mouseOperationSelect(mapSceneToVideo(mapToScene(e->pos())));
        emit mouseOperationPressed(mapSceneToVideo(mapToScene(e->pos())),QPointF(crosshairX_,crosshairY_));
        currentSelectionChanged();
    }
    else if (e->button() == Qt::LeftButton && mode_ == GROUP)
    {
        emit mouseGroupPressed(mapSceneToVideo(mapToScene(e->pos())));
        connect(this, SIGNAL(mouseMoved(QPointF)), shapeModel_, SLOT(finishGroupRectangle(QPointF)));
        createGroupRectangle();

    }
    else if (e->button() == Qt::RightButton && mode_ == GROUP)
    {
        shapeModel_->placeGrid(mapSceneToVideo(mapToScene(e->pos())));
        reviewCrosshairLinePositions();
        currentSelectionChanged();

    }
    else if (e->button() == Qt::RightButton)
        emit mouseRightClicked(mapSceneToVideo(mapToScene(e->pos())));
    else{
        AMOverlayVideoWidget2::mousePressEvent(e);
        return;
    }

    reviewCrosshairLinePositions();
}

void AMShapeDataSetView::mouseReleaseEvent(QMouseEvent *e)
{


	if(e->button() == Qt::LeftButton) {
		if(doubleClickInProgress_) {
			emit mouseDoubleClicked(mapSceneToVideo(mapToScene(e->pos())));
			doubleClickInProgress_ = false;
		}
		else {
			emit mouseReleased(mapSceneToVideo(mapToScene(e->pos())));
		}

        disconnect(shapeModel_, SLOT(finishRectangle(QPointF)));
        disconnect(shapeModel_, SLOT(moveCurrentShape(QPointF)));
        disconnect(this, SIGNAL(currentChanged()));
        disconnect(shapeModel_, SLOT(moveAllShapes(QPointF)));
        disconnect(shapeModel_, SLOT(finishGroupRectangle(QPointF)));

        if(groupRectangleActive_)
        {
            destroyGroupRectangle();
        }

        reviewCrosshairLinePositions();


	}
    else if(e->button() == Qt::RightButton)
    {
        emit mouseReleased(mapSceneToVideo(mapToScene(e->pos())));
        disconnect(shapeModel_, SLOT(zoomAllShapes(QPointF)));
        disconnect(this, SIGNAL(currentChanged()));
        disconnect(shapeModel_, SLOT(rotateRectangle(QPointF)));
        disconnect(shapeModel_, SLOT(zoomShape(QPointF)));
        reviewCrosshairLinePositions();
    }
    else AMOverlayVideoWidget2::mouseReleaseEvent(e);
}

void AMShapeDataSetView::mouseDoubleClickEvent(QMouseEvent *e)
{


	if(e->button() == Qt::LeftButton) {
		doubleClickInProgress_ = true;
	}
    else   AMOverlayVideoWidget2::mouseDoubleClickEvent(e);

}

void AMShapeDataSetView::mouseMoveEvent(QMouseEvent *e)
{
    AMOverlayVideoWidget2::mouseMoveEvent(e);

    if(e->MouseMove)
    {
        emit mouseMoved(mapSceneToVideo(mapToScene(e->pos())));
        if(mode_ == MOVE)
        {
            emit mouseMovedMoveMode(mapSceneToVideo(mapToScene(e->pos())));
        }
        else if(mode_ == DRAW)
        {
            currentSelectionChanged();
        }

    }
     reviewCrosshairLinePositions();
}

QPointF AMShapeDataSetView::mapSceneToVideo(const QPointF &sceneCoordinate) const
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




/// append a new rectangle to the current list, add it to the scene
void AMShapeDataSetView::addNewShape()
{
    index_++;
    QPen pen(BORDERCOLOUR);
    QBrush brush(QColor(Qt::transparent));
    QPolygonF polygon(QRectF(5,5,20,20));
    shapes_.insert(index_, scene()->addPolygon(polygon,pen,brush));
}

/// Remove a rectangle from the scene
void AMShapeDataSetView::deleteShape()
{
    QGraphicsPolygonItem* polygon = shapes_[index_];
    scene()->removeItem(shapes_[index_]);
    shapes_.remove(index_);
    index_--;
    delete polygon;
}

/// change the currently selected item, outline it in yellow
void AMShapeDataSetView::currentSelectionChanged()
{
    emit currentChanged();
    if(shapeModel_->isValid(current_))
        shapes_[current_]->setPen(BORDERCOLOUR);
    current_ = shapeModel_->currentIndex();
    if(shapeModel_->isValid(current_))
    {
        shapes_[current_]->setPen(ACTIVEBORDERCOLOUR);
    }
}

void AMShapeDataSetView::createGroupRectangle()
{
    QBrush penBrush(QColor(Qt::magenta));
    QPen pen(penBrush,1,Qt::DotLine);
    QBrush brush(Qt::transparent);
    groupRectangle_ = scene()->addPolygon(QPolygonF(QRectF(5,5,20,20)),pen,brush);
    groupRectangleActive_ = true;

}

void AMShapeDataSetView::destroyGroupRectangle()
{
    scene()->removeItem(groupRectangle_);
    groupRectangleActive_ = false;
}
