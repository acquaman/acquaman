#include "AMShapeOverlayVideoWidgetModel2.h"

#include <QGraphicsRectItem>
#include <QResizeEvent>
#include <QVector3D>

#include <QMediaObject>
#include <QGraphicsVideoItem>
#include <QDebug>
#include <math.h>
#include "ui/AMCameraConfigurationModel.h"

#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMSample.h"

int const AMShapeOverlayVideoWidgetModel2::TOPLEFT = 0;
int const AMShapeOverlayVideoWidgetModel2::TOPRIGHT = 1;
int const AMShapeOverlayVideoWidgetModel2::BOTTOMRIGHT = 2;
int const AMShapeOverlayVideoWidgetModel2::BOTTOMLEFT = 3;
int const AMShapeOverlayVideoWidgetModel2::TOPCLOSE = 4;
int const AMShapeOverlayVideoWidgetModel2::RECTANGLE_POINTS = 5;



/// These values approximate the distortion of the camera
double const AMShapeOverlayVideoWidgetModel2::X_XMOVEMENT = 0.015695;
double const AMShapeOverlayVideoWidgetModel2::X_YMOVEMENT = -0.00003425;
double const AMShapeOverlayVideoWidgetModel2::Y_YMOVEMENT = -0.0193235;
double const AMShapeOverlayVideoWidgetModel2::Y_XMOVEMENT = -0.001015967;

/// Constructor
AMShapeOverlayVideoWidgetModel2::AMShapeOverlayVideoWidgetModel2(QObject *parent) :
    QObject(parent)
{
    index_ = -1;

    AMDatabase *db = AMDatabase::createDatabase("user", "/home/sgm/AcquamanData/userdata.db");
    if(!db)
        qDebug() << "Uh oh, no database created";

    bool success = true;

    success &= AMDbObjectSupport::s()->registerDatabase(db);
    success &= AMDbObjectSupport::s()->registerClass<AMDbObject>();
    success &= AMDbObjectSupport::s()->registerClass<AMSample>();
    success &= AMDbObjectSupport::s()->registerClass<AMCameraConfigurationModel>();

    qDebug() << "Status of registration is " << success;
}


/// creates a new rectangle, and initializes its data
void AMShapeOverlayVideoWidgetModel2::startRectangle(QPointF position)
{
    qDebug()<<"Here in startRectangle";
    index_++;
    QVector<QVector3D> newShape;
    QVector3D coordinate[RECTANGLE_POINTS];
    double depth = cameraModel_->cameraFocalLength();
    coordinate[TOPLEFT] = transform2Dto3D(position,depth);
    coordinate[TOPRIGHT] = transform2Dto3D(position + QPointF(0.01,0),depth);
    coordinate[BOTTOMRIGHT] = transform2Dto3D(position + QPointF(0.01,0.01),depth);
    coordinate[BOTTOMLEFT] = transform2Dto3D(position + QPointF(0,0.01),depth);
    coordinate[TOPCLOSE] = coordinate[TOPLEFT];
    for(int i = 0; i<RECTANGLE_POINTS; i++)
    {
        newShape<<coordinate[i];
    }
    QPolygonF polygon;
    shapeList_.insert(index_,AMShapeData2());
    shapeList_[index_].setName("Shape " + QString::number(index_));
    shapeList_[index_].setOtherData("Coordinate:");
    shapeList_[index_].setIdNumber(index_ * 13);
    shapeList_[index_].setRotation(0);
    shapeList_[index_].setTilt(0);
    shapeList_[index_].setCoordinateShape(newShape,5);
    updateShape(index_);

    current_ = index_;
}

/// Changes the bottom right corner of the current rectangle
void AMShapeOverlayVideoWidgetModel2::finishRectangle(QPointF position)
{
    if(!isValid(current_))return;
    QVector3D topLeft = shapeList_[current_].coordinate(TOPLEFT);
    QVector3D bottomRight = transform2Dto3D(position,depth(topLeft));
    QVector3D shift = bottomRight - topLeft;


    QVector3D topRight = shapeList_[current_].coordinate(TOPRIGHT) - topLeft;
    topRight.normalize();
    QVector3D bottomLeft = shapeList_[current_].coordinate(BOTTOMLEFT) - topLeft;
    bottomLeft.normalize();
    if(topRight.isNull() && bottomLeft.isNull()) qDebug()<<"null shape";
    else if(topRight.isNull()) topRight = QVector3D::normal(QVector3D::normal(bottomLeft,shift),bottomLeft);
    else if(bottomLeft.isNull()) bottomLeft = QVector3D::normal(QVector3D::normal(topRight,shift),topRight);


    double topRightlength = (QVector3D::dotProduct(topRight,shift));
    topRight = topRight*topRightlength;

    double topLeftlength = (QVector3D::dotProduct(bottomLeft,shift));
    bottomLeft = bottomLeft*topLeftlength;



    shapeList_[current_].setCoordinate(topLeft + topRight,TOPRIGHT);
    shapeList_[current_].setCoordinate(bottomRight,BOTTOMRIGHT);
    shapeList_[current_].setCoordinate(topLeft + bottomLeft,BOTTOMLEFT);
    updateShape(current_);

}

/// deletes a rectangle from the list
void AMShapeOverlayVideoWidgetModel2::deleteRectangle(QPointF position)
{
    for(int i = 0; i < index_ + 1; i++)
    {
        if(contains(position,i))
        {
           shapeList_.remove(i);
           shapeList_.insert(i,shapeList_[index_]);
           shapeList_.remove(index_);
           index_--;
        }
    }
}

/// selects a rectangle which is under the cursor
void AMShapeOverlayVideoWidgetModel2::selectCurrentShape(QPointF position)
{
    int i = 0;
    while(isValid(i) && !contains(position,i))
    {
       i++;
    }
    if(i <= index_)
    {
        current_ = i;
        currentVector_ = position;
    }
    else current_ = index_ + 1;
}

/// moves the currently selected rectangle by position + currentVector_
void AMShapeOverlayVideoWidgetModel2::moveCurrentShape(QPointF position, int index)
{
    if(index == -1) index = current_;
    if(index <= index_)
    {
        QVector3D coordinate = shapeList_[index].coordinate(TOPLEFT);
        double distance = depth(coordinate);
        QVector3D newPosition = transform2Dto3D(position,distance);
        QVector3D oldPosition = transform2Dto3D(currentVector_,distance);
        QVector3D shift = newPosition - oldPosition;
        shiftCoordinates(shift,index);
    }
    currentVector_ = position;
}

/// moves all rectangles by position + rectangleVector_[index]
void AMShapeOverlayVideoWidgetModel2::moveAllShapes(QPointF position)
{
    QPointF currentVector = currentVector_;
    for(int i = 0; i <= index_; i++)
    {
        currentVector_ = currentVector;
        moveCurrentShape(position, i);

    }
    currentVector_ = position;
}

/// assigns current vector to mouse position
void AMShapeOverlayVideoWidgetModel2::setShapeVectors(QPointF position)
{
      currentVector_ = position;

}

/// changes the depth of all shapes by the same amount
void AMShapeOverlayVideoWidgetModel2::zoomAllShapes(QPointF position)
{
    for(int i = 0; i <= index_; i++)
    {

        QVector3D coordinate = shapeList_[i].coordinate(TOPLEFT);
        QVector3D change = depthVector(coordinate)*(zoomPoint_.y()/position.y() - 1);
        shiftCoordinates(change,i);
    }
    zoomPoint_ = position;
}



/// sets the coordinates for index
void AMShapeOverlayVideoWidgetModel2::setCoordinates(QVector3D coordinate, int index)
{
    // get the current center
    QVector3D center = centerCoordinate(index);
    QVector3D shift = coordinate - center;
    shiftCoordinates(shift,index);
}

/// Overload of setCoordinates, takes three doubles and an index
void AMShapeOverlayVideoWidgetModel2::setCoordinates(double x, double y, double z, int index)
{
    if(-1 == index) index = current_;
    QVector3D coordinate;
    coordinate.setX(x);
    coordinate.setY(y);
    coordinate.setZ(z);
    setCoordinates(coordinate, index);
}

/// returns the coordinate of index
QVector3D AMShapeOverlayVideoWidgetModel2::coordinate(int index)
{
    return centerCoordinate(index);
}

/// calls coordinate with index equal to current_
QVector3D AMShapeOverlayVideoWidgetModel2::currentCoordinate()
{
    return coordinate(current_);
}

/// takes a 3D coordinate and changes it to a 2D location on screen
QPointF AMShapeOverlayVideoWidgetModel2::transform3Dto2D(QVector3D coordinate)
{
    /// Shifts and rotates the entire scene so that it is looking down the z-axis
    /// then converts that to screen coordinates by taking the proportion of
    /// focal length over the coordinate's distance from the camera.
    QVector3D newPosition;
    double rotation = cameraModel_->cameraRotation();
    double oldRotation;
    double newRotation;
    double radius;
    double zLength;
    double focalLength = cameraModel_->cameraFocalLength();
    QVector3D cameraPosition = cameraModel_->cameraPosition();
    QVector3D cameraCenter = cameraModel_->cameraCenter();
    // the center in shifted coordinates
    QVector3D cameraRotation = cameraCenter - cameraPosition;
    QVector3D zHat = QVector3D(0,0,-1);
    QPointF position;
    // shifted position of the desired coordinate
    QVector3D shiftedCoordinate = coordinate - cameraPosition;
    // axis of rotation
    QVector3D vHat = QVector3D::normal(zHat,cameraRotation);
    if(vHat.isNull())
    {
        // the camera is not rotated (already looks along the z-axis)
        // simply perform the shift
        cameraRotation += QVector3D(0,0.01,0);
        vHat = QVector3D::normal(zHat,cameraRotation);
        if(vHat.isNull()) qDebug()<<"Vhat still null, in 3-2";
    }
    // rotate the camera
    double cameraCenterLength = cameraRotation.length();
    QVector3D newCameraCenter = QVector3D(0,0,cameraCenterLength);
    // find the angle of rotation
    double theta = acos((QVector3D::dotProduct(newCameraCenter,cameraRotation))/((newCameraCenter.length())*(cameraCenterLength)));
    // find the third orthogonal component (along the plane of rotation)
    QVector3D uHat = QVector3D::normal(zHat,vHat);
    double shiftedLength = shiftedCoordinate.length();
    // find the angle of the coordinate before rotation
    double cosAlpha = QVector3D::dotProduct(uHat,shiftedCoordinate)/shiftedLength;
    double sinAlpha = QVector3D::dotProduct(zHat,shiftedCoordinate)/shiftedLength;
    // get the components along the u,v, and z axes
    double uLength = shiftedLength*(cosAlpha*cos(theta)-sinAlpha*sin(theta));
    double vLength = QVector3D::dotProduct(shiftedCoordinate,vHat);
    zLength = shiftedLength*(sinAlpha*cos(theta)+cosAlpha*sin(theta));
    // add the three orthogonal components together
    newPosition = uLength*uHat + vLength*vHat + zLength*zHat;
    // scale and shift to the center of the screen



    if(rotation != 0)
    {
        double z = newPosition.z();
        newPosition.setZ(0);
        radius = newPosition.length();
        oldRotation = acos(newPosition.x()/radius);
        if(newPosition.y()<0) oldRotation *= -1;
        newRotation = oldRotation+rotation;
        newPosition.setX(radius*cos(newRotation));
        newPosition.setY(radius*sin(newRotation));
        newPosition.setZ(z);
    }
    position = (focalLength/zLength)*newPosition.toPointF() + QPointF(0.5,0.5);
    return position;
}

/// takes a 2D point on screen, as well as its depth, and transforms it into a 3D coordinate
QVector3D AMShapeOverlayVideoWidgetModel2::transform2Dto3D(QPointF point, double depth)
{
    /// Use the position of the item on screen, with knowledge of the
    /// camera position to determine its xyz coordinate.
    /// first convert to a point in an ideal plane, then shift and rotate to match camera
    QVector3D coordinate;
    double focalLength = cameraModel_->cameraFocalLength();
    double theta;
    coordinate.setX(-1*((point.x()-0.5)*depth/focalLength));
    coordinate.setY(-1*((point.y()-0.5)*depth/focalLength));
    coordinate.setZ(depth);
    /// coordinate is the point in for camera at (0,0,0) looking down the z-axis
    /// must now shift and rotate
    // undo the rotation
    if(cameraModel_->cameraRotation() != 0)
    {
        double z = coordinate.z();
        coordinate.setZ(0);
        double radius = coordinate.length();
        double oldRotation = acos(coordinate.x()/radius);
        if(coordinate.y() < 0) oldRotation*= -1;
        double rotation = cameraModel_->cameraRotation()*-1;
        double newRotation = oldRotation+rotation;
        coordinate.setX(radius*cos(newRotation));
        coordinate.setY(radius*sin(newRotation));
        coordinate.setZ(z);
    }
    // build the orthogonal components, z, u, v
    QVector3D zHat = QVector3D(0,0,1);
    QVector3D uHat;
    QVector3D shift = cameraModel_->cameraPosition();
    QVector3D actualCameraCenter = cameraModel_->cameraCenter();
    QVector3D shiftedCameraCenter = actualCameraCenter - shift;
    QVector3D idealCameraCenter = shiftedCameraCenter.length()*zHat;
    QVector3D vHat = QVector3D::normal(idealCameraCenter,shiftedCameraCenter);
    if(vHat.isNull())
    {
//        // no rotation
//        //return shifted point
//        double z = coordinate.z();
//        coordinate.setZ(0);
//        double radius = coordinate.length();
//        double oldRotation = acos(coordinate.x()/radius);
//        if(coordinate.y() < 0) oldRotation*= -1;
//        double rotation = 3.14;
//        double newRotation = oldRotation+rotation;
//        coordinate.setX(radius*cos(newRotation));
//        coordinate.setY(radius*sin(newRotation));
//        coordinate.setZ(z);
////        return coordinate+shift
//        vHat = QVector3D(0,-1,0);
//        uHat = QVector3D(-1,0,0);
        shiftedCameraCenter += QVector3D(0.01,0,0);
        vHat = QVector3D::normal(idealCameraCenter,shiftedCameraCenter);
        if(vHat.isNull()) qDebug()<<"still a null vHat...";
    }
    // perform the rotation
    uHat = QVector3D::normal(vHat,zHat);
    // find the angle of rotation
    theta = -1*acos((QVector3D::dotProduct(idealCameraCenter,shiftedCameraCenter))/(shiftedCameraCenter.lengthSquared()));
    double coordinateLength = coordinate.length();
    // find the coordinate's current angle
    double cosAlpha = QVector3D::dotProduct(uHat,coordinate)/coordinateLength;
    double sinAlpha = QVector3D::dotProduct(zHat,coordinate)/coordinateLength;
    // find the resultant components
    double uLength = coordinateLength*(cosAlpha*cos(theta)-sinAlpha*sin(theta));
    qDebug()<<"U length"<<uLength;
    double vLength = QVector3D::dotProduct(coordinate,vHat);
    qDebug()<<"V length"<<vLength;
    double zLength = coordinateLength*(sinAlpha*cos(theta)+cosAlpha*sin(theta));
    qDebug()<<"Z length"<<zLength;
    // create the new vector
    QVector3D newPosition = uLength*uHat + vLength*vHat + zLength*zHat;
    // shift
    QVector3D finalPosition = newPosition + shift;

    return finalPosition;
}

/// scales a dimension based on distance
double AMShapeOverlayVideoWidgetModel2::transformDimension(double dimension, QVector3D coordinate)
{
    double focalLength = cameraModel_->cameraFocalLength();
    double newDimension = dimension*focalLength/depth(coordinate);
    return newDimension;
}

/// finds the length of a dimension based on its appearance and distance
double AMShapeOverlayVideoWidgetModel2::inverseDimensionTransform(double dimension, QVector3D coordinate)
{
    double focalLength = cameraModel_->cameraFocalLength();
    double newDimension = dimension*depth(coordinate)/focalLength;
    return newDimension;
}


/// takes a vector and finds how it appears
QPointF AMShapeOverlayVideoWidgetModel2::transformVector(QPointF vector, QVector3D coordinate)
{
    double focalLength = cameraModel_->cameraFocalLength();
    double distance = depth(coordinate);
    qDebug()<<"Focal Length:"<<focalLength;
    qDebug()<<"Depth"<<distance;
    double scale = focalLength/distance;
    qDebug()<<"Scaling factor"<<scale;
    QPointF newVector = vector*scale;
    return newVector;
}

/// based on the appearance of a vector, finds the actual vector
QPointF AMShapeOverlayVideoWidgetModel2::inverseVectorTransform(QPointF vector, QVector3D coordinate)
{
    double focalLength = cameraModel_->cameraFocalLength();
    QPointF newVector = vector*depth(coordinate)/focalLength;
    return newVector;
}

/// finds the center of the given polygon
QPointF AMShapeOverlayVideoWidgetModel2::findCenter(QPolygonF polygon)
{
//    QPointF* data = polygon.data();
    double xMid = (polygon[TOPLEFT].x()+polygon[TOPRIGHT].x())/2.0;
    double yMid = (polygon[TOPLEFT].y()+polygon[BOTTOMLEFT].y())/2.0;
    return QPointF(xMid,yMid);
}

/// moves all the shapes based on change in motor movement
void AMShapeOverlayVideoWidgetModel2::motorMovement(double x, double y, double z, double r)
{
    for(int i = 0; i <= index_; i++)
    {
        double xOffset = (x - motorCoordinate_.x())*(X_XMOVEMENT) + (y - motorCoordinate_.y())*(X_YMOVEMENT);
        double yOffset = (y - motorCoordinate_.y())*(Y_YMOVEMENT) + (x - motorCoordinate_.x())*(Y_XMOVEMENT);
        QPointF offset(xOffset,yOffset);
        QVector3D coordinate = shapeList_[i].coordinate(TOPLEFT);
        double distance = depth(coordinate);
        QVector3D shift = transform2Dto3D(transformVector(offset,coordinate), distance);
        shiftCoordinates(shift,i);

    }
}

double AMShapeOverlayVideoWidgetModel2::depth(QVector3D coordinate)
{
    QVector3D depth = depthVector(coordinate);
    return depth.length();
}

QVector3D AMShapeOverlayVideoWidgetModel2::depthVector(QVector3D coordinate)
{
    QVector3D depthDirection = cameraModel_->cameraCenter() - cameraModel_->cameraPosition();
    QVector3D cameraToCoordinate = coordinate - cameraModel_->cameraPosition();
    depthDirection.normalize();
    QVector3D depth = QVector3D::dotProduct(cameraToCoordinate,depthDirection)*depthDirection;
    return depth;
}



/// changes the coordinate of the shape
void AMShapeOverlayVideoWidgetModel2::changeCoordinate(int index)
{
    if(-1 == index) index = current_;
    qDebug()<<"changeCoordinate was called -  should not have happened";
//    double height = transformDimension(shapeList_[index].height(),shapeList_[index].coordinate());
//    double width = transformDimension(shapeList_[index].width(),shapeList_[index].coordinate());
//    qDebug()<<QString::number(height)<<QString::number(shapeList_[index].height());
//    qDebug()<<QString::number(width)<<QString::number(shapeList_[index].width());
//    QSizeF size = QSizeF(width,height);
//    QPointF* data = shapeList_[index].shape()->data();
//    QPointF topLeft = data[TOPLEFT];
//    QRectF rectangle = QRectF(topLeft, size);
//    rectangle.moveCenter(transform3Dto2D(shapeList_[index].coordinate()));
//    shapeList_[index].setShape(QPolygonF(rectangle));

}

/// moves the clicked point to appear under the crosshair, and gives the predicted motor coordinate
void AMShapeOverlayVideoWidgetModel2::shiftToPoint(QPointF position, QPointF crosshairPosition)
{
    if(isValid(current_))
    {
        currentVector_ = inverseVectorTransform(position, shapeList_[current_].coordinate(TOPLEFT));
        moveAllShapes(inverseVectorTransform(crosshairPosition, shapeList_[current_].coordinate(TOPLEFT)));
        double motorX = motorCoordinate_.x() + ((Y_YMOVEMENT)*(crosshairPosition.x() - position.x())-(X_YMOVEMENT)*(crosshairPosition.y() - position.y()))/((Y_YMOVEMENT)*(X_XMOVEMENT)-(X_YMOVEMENT)*(Y_XMOVEMENT));
        double motorY = motorCoordinate_.y() + ((Y_XMOVEMENT)*(crosshairPosition.x() - position.x())-(X_XMOVEMENT)*(crosshairPosition.y() - position.y()))/((X_YMOVEMENT)*(Y_XMOVEMENT) - (Y_YMOVEMENT)*(X_XMOVEMENT));
        motorCoordinate_.setX(motorX);
        motorCoordinate_.setY(motorY);

    }



}



/// transforms a coordinate point for display on the screen
QPointF AMShapeOverlayVideoWidgetModel2::coordinateTransform(QPointF coordinate)
{

    QRectF activeRect = QRectF(QPointF((viewSize().width()-scaledSize().width())/2,
                                       (viewSize().height()-scaledSize().height())/2),
                               scaledSize());

    QPointF fixedCoordinate;
    qreal xCoord = activeRect.left() + coordinate.x()*activeRect.width();
    qreal yCoord = activeRect.top() + coordinate.y()*activeRect.height();
    fixedCoordinate.setX(xCoord);
    fixedCoordinate.setY(yCoord);
    return fixedCoordinate;


}

/// applies the rotation to the shape
QPolygonF AMShapeOverlayVideoWidgetModel2::applyRotation(int index)
{
    QPolygonF shape(*shapeList_[index].shape());
    if(shape.isEmpty()) return shape;
    double distance = depth(shapeList_[index].coordinate(TOPLEFT));
    double rotation = shapeList_[index].rotation();
    double length = (shapeList_[index].shape()->data()[TOPRIGHT].x() - shapeList_[index].shape()->data()[TOPLEFT].x())/2;
    QPolygonF polygon;
    polygon<<getRotatedPoint(shape.first(),distance,rotation, QPointF(shape.first().x()+length,shape.first().y()));
    shape.remove(0);
    polygon<<getRotatedPoint(shape.first(),distance,rotation, QPointF(shape.first().x()-length,shape.first().y()));
    shape.remove(0);
    polygon<<getRotatedPoint(shape.first(),distance,rotation, QPointF(shape.first().x()-length,shape.first().y()));
    shape.remove(0);
    polygon<<getRotatedPoint(shape.first(),distance,rotation, QPointF(shape.first().x()+length,shape.first().y()));
    shape.remove(0);
    polygon<<polygon.first();
    return polygon;

}

/// finds the new coordinate of a point, given its coordinate, rotation, and center point
QPointF AMShapeOverlayVideoWidgetModel2::getRotatedPoint(QPointF point, double z, double rotation, QPointF center)
{

    QVector3D coordinate = transform2Dto3D(point,z);
    QVector3D newCenter = transform2Dto3D(center,z);
    QVector3D direction = newCenter - coordinate;
    ///should be parallel to the x axis
    QVector3D newX = ((1 - cos(rotation))*direction);
    QVector3D zDirection(0,0,-1*direction.x());
    QVector3D newZ = sin(rotation)*zDirection;
    coordinate = coordinate + newX + newZ;
    return transform3Dto2D(coordinate);





}

QPolygonF AMShapeOverlayVideoWidgetModel2::applyTilt(QPolygonF shape, int index)
{
    if(shape.isEmpty()) return shape;
    double z = depth(shapeList_[index].coordinate(TOPLEFT));
    double tilt = shapeList_[index].tilt();
    //double height = (shape.data()[BOTTOMRIGHT].y() - shape.data()[TOPRIGHT].y())/2.0;
    QPointF center = findCenter(shape);
    QPolygonF polygon;
    polygon<<getTiltedPoint(shape.first(),z,tilt,center);
    shape.remove(0);
    polygon<<getTiltedPoint(shape.first(),z,tilt,center);
    shape.remove(0);
    polygon<<getTiltedPoint(shape.first(),z,tilt,center);
    shape.remove(0);
    polygon<<getTiltedPoint(shape.first(),z,tilt,center);
    shape.remove(0);
    polygon<<polygon.first();
    return polygon;
}

QPointF AMShapeOverlayVideoWidgetModel2::getTiltedPoint(QPointF point, double z, double tilt, QPointF center)
{
    QVector3D coordinate = transform2Dto3D(point,z);
    QVector3D newCenter = transform2Dto3D(center,z);
    newCenter.setX(coordinate.x());
    QVector3D direction = newCenter- coordinate;
    QVector3D zDirection(0,0,direction.y());
    QVector3D newZ = sin(tilt)*zDirection;
    QVector3D newY = ((1-cos(tilt))*direction);
    coordinate = coordinate + newZ + newY;
    return transform3Dto2D(coordinate);
}



/// constructs a rectangle given the desired top and bottom corners
QPolygonF AMShapeOverlayVideoWidgetModel2::constructRectangle(QPointF topLeft, QPointF bottomRight)
{
    QPolygonF polygon;
    QPointF topRight(bottomRight.x(),topLeft.y());
    QPointF bottomLeft(topLeft.x(),bottomRight.y());
    polygon<<topLeft<<topRight<<bottomRight<<bottomLeft<<topLeft;
    return polygon;

}

/// returns the size of a rectangle with the given points
QSizeF AMShapeOverlayVideoWidgetModel2::size(QPointF topLeft, QPointF bottomRight)
{
    QSizeF size;
    QPointF difference = bottomRight - topLeft;
    size.setHeight(difference.y());
    size.setWidth(difference.x());
    return size;
}

bool AMShapeOverlayVideoWidgetModel2::contains(QPointF position, int index)
{
    return shapeList_[index].shape()->containsPoint(position, Qt::OddEvenFill);
}



void AMShapeOverlayVideoWidgetModel2::setViewSize(QSizeF viewSize)
{
    viewSize_ = viewSize;
}

void AMShapeOverlayVideoWidgetModel2::setScaledSize(QSizeF scaledSize)
{
    scaledSize_ = scaledSize;
}

QPointF AMShapeOverlayVideoWidgetModel2::shapeTopLeft(int index)
{
    return coordinateTransform(shapeList_[index].shape()->data()[TOPLEFT]);
}

QPointF AMShapeOverlayVideoWidgetModel2::shapeBottomRight(int index)
{
    return coordinateTransform(shapeList_[index].shape()->data()[BOTTOMRIGHT]);
}

/// checks if an index is valid
bool AMShapeOverlayVideoWidgetModel2::isValid(int index)
{
    if(index <= index_ && index >= 0)
    {
        return true;
    }
    else
    {
        qDebug()<<"Invalid index"<<QString::number(index);
        return false;
    }
}

double AMShapeOverlayVideoWidgetModel2::rotation(int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    return shapeList_[index].rotation();
}

void AMShapeOverlayVideoWidgetModel2::setRotation(double rotation, int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    shapeList_[index].setRotation(rotation);
}

double AMShapeOverlayVideoWidgetModel2::tilt(int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    return shapeList_[index].tilt();
}

void AMShapeOverlayVideoWidgetModel2::setTilt(double tilt, int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    shapeList_[index].setTilt(tilt);
}

QPolygonF AMShapeOverlayVideoWidgetModel2::groupRectangle()
{
    QPolygonF shape = groupRectangle_;
    QPointF topLeft = shape.first();
    shape.remove(0);
    QPointF topRight = shape.first();
    shape.remove(0);
    QPointF bottomRight = shape.first();
    shape.remove(0);
    QPointF bottomLeft = shape.first();
    shape.remove(0);
    shape.clear();
    QPointF newTopLeft = coordinateTransform(topLeft);
    QPointF newTopRight = coordinateTransform(topRight);
    QPointF newBottomRight = coordinateTransform(bottomRight);
    QPointF newBottomLeft = coordinateTransform(bottomLeft);
    QPolygonF newShape;
    newShape<<newTopLeft<<newTopRight<<newBottomRight<<newBottomLeft<<newTopLeft;
    return newShape;
}

void AMShapeOverlayVideoWidgetModel2::setMotorCoordinate(double x, double y, double z, double r)
{
    motorMovement(x,y,z,r);
    motorCoordinate_.setX(x);
    motorCoordinate_.setY(y);
    motorCoordinate_.setZ(z);

    motorRotation_ = r;
}

double AMShapeOverlayVideoWidgetModel2::motorRotation()
{
    return motorRotation_;
}

double AMShapeOverlayVideoWidgetModel2::motorX()
{
    return motorCoordinate_.x();
}

double AMShapeOverlayVideoWidgetModel2::motorY()
{
    return motorCoordinate_.y();
}

double AMShapeOverlayVideoWidgetModel2::motorZ()
{
    return motorCoordinate_.z();
}

void AMShapeOverlayVideoWidgetModel2::toggleDistortion()
{
    distortion_ = !distortion_;
    updateAllShapes();
}

void AMShapeOverlayVideoWidgetModel2::setCameraModel(AMCameraConfigurationModel *model)
{
    cameraModel_ = model;
    updateAllShapes();
}


QPolygonF AMShapeOverlayVideoWidgetModel2::shape(int index)
{
    QPolygonF shape = *shapeList_[index].shape();

    QPointF topLeft = shape.first();
    shape.remove(0);
    QPointF topRight = shape.first();
    shape.remove(0);
    QPointF bottomRight = shape.first();
    shape.remove(0);
    QPointF bottomLeft = shape.first();
    shape.remove(0);
    shape.clear();

    QPointF newTopLeft = coordinateTransform(topLeft);
    QPointF newTopRight = coordinateTransform(topRight);
    QPointF newBottomRight = coordinateTransform(bottomRight);
    QPointF newBottomLeft = coordinateTransform(bottomLeft);
    QPolygonF newShape;
    newShape<<newTopLeft<<newTopRight<<newBottomRight<<newBottomLeft<<newTopLeft;
    return newShape;

}

QPolygonF AMShapeOverlayVideoWidgetModel2::subShape(int index, QPolygonF shape)
{
    if(shape.isEmpty()) return shape;
    if(shapeList_[index].rotation() != 0) shape = applyRotation(index);
    if(shapeList_[index].tilt() != 0) shape = applyTilt(shape, index);

    if(distortion_)
    {
        shape = applyDistortion(shape);
        if(motorCoordinate_.z() != 0)
        shape = removeDistortion(shape);
    }
    return shape;
}

QPolygonF AMShapeOverlayVideoWidgetModel2::applyDistortion(QPolygonF shape)
{
    QPolygonF polygon;
    for(int i = 0; i <= TOPCLOSE; i++)
    {
        polygon<<distortPoint(shape[i]);
    }
    return polygon;
}

QPointF AMShapeOverlayVideoWidgetModel2::distortPoint(QPointF point)
{
    double distortionFactor = cameraModel_->cameraDistortion();
    QPointF newPoint;
    double x = point.x()-0.5;
    double y = point.y()-0.5;

    newPoint.setX((x*(1+distortionFactor*(x*x+y*y)))+0.5);
    newPoint.setY((y*(1+distortionFactor*(x*x+y*y)))+0.5);
    return newPoint;
}

QPolygonF AMShapeOverlayVideoWidgetModel2::removeDistortion(QPolygonF shape)
{
    QPolygonF polygon;
    for(int i = 0; i <= TOPCLOSE; i++)
    {
        polygon<<undistortPoint(shape[i]);
    }
    return polygon;
}

QPointF AMShapeOverlayVideoWidgetModel2::undistortPoint(QPointF point)
{
    /// this does not work at the center, so ignore it (it doesn't move)
    if(point.x() == 0.5 && point.y() == 0.5)
        return point;
    double distortionFactor = cameraModel_->cameraDistortion();
    QPointF newPoint;
    double x = point.x()-0.5;
    double y = point.y()-0.5;

    double distortedRadius = x*x + y*y;
    distortedRadius = pow(distortedRadius,0.5);

    double wo;
   // double w1;
    double rootFactor;
    double theta;
    double newRadius;

    /// must check to see if there is a valid solution
    /// take the cubic equation r^3 + r/distortionFactor - distortedRadius/distortionFactor = 0
    /// using Vieta's Substitution we get
    /// wo^3 = distortedRadius/(2*distortionFactor) (+,-) sqrt(distortedRadius^2/distortionFactor^2 + 4/(27*distortionFactor^3))/2
    /// wo^3 = rd/2k (+,-) sqrt((rd/k)^2+(4/(27*k^3)))/2
    /// if the square root part is negative, take its absolute value, multiply by j, otherwise it's purely real
    /// will get two results, one for wo^3 + sqrt part, and one for minus.
    /// if the square root part is negative, determine the angle, and see if there is a real solution
    /// take the cube root of w0^3, make sure the sign is correct (theta = zero or pi)
    /// r = wo-1/(3*k*wo)

    /// inside the root is c
    /// the entire second term is b
    /// the first term is a

    double c = distortedRadius*distortedRadius/(distortionFactor*distortionFactor) + (4)/(27*pow(distortionFactor,3));
    double a = distortedRadius/(2*distortionFactor);
    double b;
    if(c<0)
    {
        /// the root is imaginary, so take the absolute value of c and square root it, theta = atan(b/a)
        /// b(0,1) = (+-)sqrt(c)/2
        /// with a real root, b0 and b1 will be the other two roots, so it is redundant to calculate both
        c = c*(-1);
        b = sqrt(c)/2.0;
        /// six locations result in a real root (theta = npi/3)
        /// check to see if theta is any of these, within some error
        theta = atan(b/a);

        /// a+b and a-b give the same absolute value
        /// sqrt(a^2 + b^2)
        // wo^3
        wo = sqrt(a*a + b*b);
        QPointF complexPoint [6];
        QPointF conjugatePoint [6];
        double pointAngles[6];
        pointAngles[0] = theta/3.0;
        pointAngles[3] = theta/-3.0;
        pointAngles[1] = pointAngles[0] + 2.0*M_PI/3.0;
        pointAngles[4] = pointAngles[3] + 2.0*M_PI/3.0;
        pointAngles[2] = pointAngles[0] - 2.0*M_PI/3.0;
        pointAngles[5] = pointAngles[3] - 2.0*M_PI/3.0;
        /// calculate the six possible points for r
//        for(int i = 0; i<6;i++)
//        {
//            qDebug()<<"Point"<<QString::number(i)<<QString::number(pointAngles[i]);
//        }
        //wo
        wo = pow(wo,1.0/3.0);

        for(int i = 0; i < 6; i++)
        {
            complexPoint[i].setX(wo*cos(pointAngles[i]));
            complexPoint[i].setY(wo*sin(pointAngles[i]));
            conjugatePoint[i].setX(complexPoint[i].x());
            conjugatePoint[i].setY(-1*complexPoint[i].y());
            conjugatePoint[i] *= 1/(distortionFactor*3*wo*wo);
            complexPoint[i] = complexPoint[i] - conjugatePoint[i];
           // qDebug()<<"coordinate"<<QString::number(i)<<QString::number(complexPoint[i].x())<<QString::number(complexPoint[i].y());

        }
        /// new radius is calculated using complexPoint[1], just because it works well...
        newRadius = sqrt(complexPoint[1].x()*complexPoint[1].x() + complexPoint[1].y()*complexPoint[1].y());

    }
    /// if c>=0 the number is entirely real, just do a check for theta = 0 or pi (pos or neg)
    /// this can have two distinct values (a+b, a-b)
    else
    {
        b = sqrt(c)/2.0;
        wo = a+b;
        //w1 = a-b;
        if(wo>=0)
        {
            theta = 1.0;
        }
        else
        {
            theta = -1.0;
            wo *=-1.0;
        }
        rootFactor = pow(wo,1.0/3.0);
        newRadius = rootFactor - 1.0/(3.0*distortionFactor*rootFactor);
    }


    newPoint.setX(x);
    newPoint.setY(y);
    newPoint = newPoint/distortedRadius*newRadius;
    newPoint.setX(newPoint.x()+0.5);
    newPoint.setY(newPoint.y()+0.5);
    return newPoint;


}



QString AMShapeOverlayVideoWidgetModel2::currentName()
{
    if(isValid(current_)) return shapeList_[current_].name();
    else return "";
}

void AMShapeOverlayVideoWidgetModel2::setCurrentName(QString name)
{
    shapeList_[current_].setName(name);
}

QString AMShapeOverlayVideoWidgetModel2::currentInfo()
{
    if(isValid(current_)) return shapeList_[current_].otherData();
    else return "";
}

void AMShapeOverlayVideoWidgetModel2::setCurrentInfo(QString info)
{
    shapeList_[current_].setOtherData(info);
}

int AMShapeOverlayVideoWidgetModel2::currentIndex()
{
    return current_;
}

void AMShapeOverlayVideoWidgetModel2::setCurrentIndex(int current)
{
    current_ = current;
}


void AMShapeOverlayVideoWidgetModel2::setZoomPoint(QPointF position)
{
    zoomPoint_ = position;
    zoomCenter_ = position;
}

void AMShapeOverlayVideoWidgetModel2::rotateRectangle(QPointF position)
{
    double rotation = shapeList_[current_].rotation()+(position - currentVector_).x();
    shapeList_[current_].setRotation(rotation);
    currentVector_ = position;
}

void AMShapeOverlayVideoWidgetModel2::zoomShape(QPointF position)
{
    if(isValid(current_))
    {
        QVector3D coordinate = shapeList_[current_].coordinate(TOPLEFT);
        QVector3D change = depthVector(coordinate)*(zoomPoint_.y()/position.y() - 1);
        shiftCoordinates(change,current_);
    }
    zoomPoint_ = position;

}


void AMShapeOverlayVideoWidgetModel2::startGroupRectangle(QPointF position)
{
    qDebug()<<"Creating the group rectangle model";
    QPolygonF polygon = constructRectangle(position,position);
    groupRectangle_ = polygon;

}

void AMShapeOverlayVideoWidgetModel2::finishGroupRectangle(QPointF position)
{

    //QPointF* data = shapeList_[index_].shape()->data();
     qDebug()<<"Changing the group rectangle model";
    QPointF topLeft = groupRectangle_.first();
    QPolygonF newPolygon = constructRectangle(topLeft,position);
    groupRectangle_ = newPolygon;

}

void AMShapeOverlayVideoWidgetModel2::placeGrid(QPointF position)
{
    for(double i = 0; i<20.0; i++)
    {
        for(double j = 0; j<20.0; j++)
        {
            index_ += 1;
            QPointF topLeft(i*0.05,j*0.05);
            QPointF bottomRight((i+1)*0.05,(j+1)*0.05);
            shapeList_.insert(index_,AMShapeData2(constructRectangle(topLeft,bottomRight)));
            shapeList_[current_].setIdNumber(position.x());
            shapeList_[current_].setTilt(0);
            shapeList_[current_].setRotation(0);
            current_ = index_;
        }
    }

}

void AMShapeOverlayVideoWidgetModel2::updateShape(int index)
{
   /// must set shape based on coordinates
    QPolygonF shape;
    for(int i = 0; i < RECTANGLE_POINTS; i++)
    {
        shape<<transform3Dto2D(shapeList_[index].coordinate(i));
    }
    shape = subShape(index, shape);
    shapeList_[index].setShape(shape);

}

void AMShapeOverlayVideoWidgetModel2::updateAllShapes()
{
    for(int i = 0; i <= index_; i++)
    {
        updateShape(i);
    }
}


QVector3D AMShapeOverlayVideoWidgetModel2::centerCoordinate(int index)
{
    QVector3D center = QVector3D(0,0,0);
    for(int i = 0; i < TOPCLOSE; i++)//only want the first four points
    {
        center += shapeList_[index].coordinate(i);
    }
    return center/4.0;
}

void AMShapeOverlayVideoWidgetModel2::shiftCoordinates(QVector3D shift, int index)
{
    for(int k = 0; k < RECTANGLE_POINTS; k++)
    {
        shapeList_[index].setCoordinate(shapeList_[index].coordinate(k) + shift,k);
    }
    updateShape(index);
}
