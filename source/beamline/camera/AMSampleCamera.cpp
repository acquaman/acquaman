#include "AMSampleCamera.h"

#include <QGraphicsRectItem>
#include <QResizeEvent>
#include <QVector3D>

#include <QMediaObject>
#include <QGraphicsVideoItem>
#include <QDebug>
#include <math.h>
#include "beamline/camera/AMCameraConfiguration.h"
#include "beamline/camera/AMCamera.h"


#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
//#include "beamline/SGM/SGMMAXvMotor.h"

#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMSamplePre2013.h"
#include "ui/AMSample.h"

#include <QVector4D>



#define TOPLEFT 0
#define TOPRIGHT 1
#define BOTTOMRIGHT 2
#define BOTTOMLEFT 3
#define TOPCLOSE 4
#define RECTANGLE_POINTS 5

using namespace Eigen;

AMSampleCamera* AMSampleCamera::set_;

AMSampleCamera* AMSampleCamera::set()
{
    if(!set_)
    {
        set_ = new AMSampleCamera();
    }
    return set_;

}


/// returns the highest index of shapes in shapeList
int AMSampleCamera::shapeListLength() const
{
    return index_;
}



/// returns the current index
int AMSampleCamera::currentIndex() const
{
    return currentIndex_;
}

/// returns the cameraConfiguration
AMCameraConfiguration *AMSampleCamera::cameraConfiguration() const
{
    return camera_->cameraConfiguration();
}

/// returns the beam configuration
AMBeamConfiguration *AMSampleCamera::beamConfiguration() const
{
    return beamModel_;
}

/// returns the group rectangle in screen coordinates
QPolygonF AMSampleCamera::groupRectangle() const
{
    return screenShape(groupRectangle_);
}

/// returns motor x coordinate
double AMSampleCamera::motorX() const
{
    return motorCoordinate_.x();
}

/// returns motor y coordinate
double AMSampleCamera::motorY() const
{
    return motorCoordinate_.y();
}

/// returns motor z coordinate
double AMSampleCamera::motorZ() const
{
    return motorCoordinate_.z();
}

/// returns the motor rotation
double AMSampleCamera::motorRotation() const
{
    return motorRotation_;
}

/// returns the intersections
QVector<QPolygonF> AMSampleCamera::intersections() const
{
    return intersections_;
}

/// returns the crosshair x value
double AMSampleCamera::crosshairX() const
{
    return crosshair_.x();
}

/// returns the crosshair y value
double AMSampleCamera::crosshairY() const
{
    return crosshair_.y();
}

/// returns the crosshair point
QPointF AMSampleCamera::crosshair() const
{
    return crosshair_;
}

/// is the crossHair locked?
bool AMSampleCamera::crosshairLocked() const
{
    return crosshairLocked_;
}

/// returns the view size of the video widget
QSizeF AMSampleCamera::viewSize() const
{
    return camera_->viewSize();
}

/// returns the scaled size of the video widget
QSizeF AMSampleCamera::scaledSize() const
{
    return camera_->scaledSize();
}

/// is the cameraMatrix being used?
bool AMSampleCamera::useCameraMatrix() const
{
    return camera_->useCameraMatrix();
}

/// The polyon currently being drawn
QPolygonF AMSampleCamera::currentPolygon() const
{
    return screenShape(currentPolygon_);
}

/// return the current name
QString AMSampleCamera::currentName() const
{
    return name(currentIndex_);
}

/// returns the name of the specified index
QString AMSampleCamera::name(int index) const
{
    if(isValid(index))
    {
        return shapeList_[index]->name();
    }
    else return "";
}

/// return the current info
QString AMSampleCamera::currentInfo() const
{
    return otherData(currentIndex_);
}

/// return the data of the specified index
QString AMSampleCamera::otherData(int index) const
{
    if(isValid(index))
    {
        return shapeList_[index]->otherData();
    }
    else
        return "";
}

/// returns the idNumber of the specified index
double AMSampleCamera::idNumber(int index) const
{
    if(isValid(index))
    {
        return shapeList_[index]->idNumber();
    }
    else
        return 0;
}

bool AMSampleCamera::visible() const
{
    return visible(currentIndex_);
}

bool AMSampleCamera::visible(int index) const
{
    return shapeList_.at(index)->visible();
}

/// returns the rotation of the given index
double AMSampleCamera::rotation(int index) const
{
    if(isValid(index))
        return shapeList_[index]->rotation();
    else return 0;
}

double AMSampleCamera::rotation() const
{
    return rotation(currentIndex_);
}

/// returns the tilt for the given index
double AMSampleCamera::tilt(int index) const
{
    if(isValid(index))
        return shapeList_[index]->tilt();
    else return 0;
}

double AMSampleCamera::tilt() const
{
    return tilt(currentIndex_);
}

/// returns a pointer to the AMShapeData with  index, by default current_
AMShapeData *AMSampleCamera::currentShape(int index) const
{
    if(isValid(index))
        return shapeList_[index];
    else return 0;
}

AMShapeData *AMSampleCamera::currentShape() const
{
    return currentShape(currentIndex_);
}

int AMSampleCamera::indexOfShape(AMShapeData *shape) const
{
    return shapeList_.indexOf(shape);
}

/// returns the shape in screen coordinates
QPolygonF AMSampleCamera::shape(int index) const
{
    QPolygonF shape = *shapeList_[index]->shape();

    return screenShape(shape);

}

/// returns the coordinate of index
QVector3D AMSampleCamera::coordinate(int index) const
{
    return centerCoordinate(index);
}

/// calls coordinate with index equal to current_
QVector3D AMSampleCamera::currentCoordinate() const
{
    return coordinate(currentIndex_);
}

/// set the current index
void AMSampleCamera::setCurrentIndex(int current)
{
    currentIndex_ = current;
    emit currentIndexChanged(currentIndex_);
}

/// changes the current camera model
void AMSampleCamera::setCameraModel(AMCameraConfiguration *model)
{
    camera_->setCameraConfiguration(model);
    updateAllShapes();
}

/// sets the beam model to the given model
void AMSampleCamera::setBeamModel(AMBeamConfiguration *model)
{
    beamModel_ = model;
}

/// sets the motor coordinate to xyzr
void AMSampleCamera::setMotorCoordinate(double x, double y, double z, double r)
{
    motorMovement(x,y,z,r);
    motorCoordinate_.setX(x);
    motorCoordinate_.setY(y);
    motorCoordinate_.setZ(z);

    motorRotation_ = r;
}

/// toggles distortion on or off
void AMSampleCamera::toggleDistortion()
{
    if(distortion_)
    {
        qDebug()<<"True";
    }
    if(!distortion_)
    {
        qDebug()<<"False";
    }
    bool newValue = !distortion_;
    distortion_ = newValue;
    updateAllShapes();
}

/// set the crosshair x value
void AMSampleCamera::setCrosshairX(double x)
{
    crosshair_.setX(x);
}

/// set the crosshair y value
void AMSampleCamera::setCrosshairY(double y)
{
    crosshair_.setY(y);
}

/// set the crosshair point
void AMSampleCamera::setCrosshair(QPointF crosshair)
{
    crosshair_ = crosshair;
}

/// set whether the crosshair is locked
void AMSampleCamera::setCrosshairLocked(bool locked)
{
    crosshairLocked_ = locked;
}

/// set the view size
void AMSampleCamera::setViewSize(QSizeF viewSize)
{
    camera_->setViewSize(viewSize);
}

/// set the scaled size
void AMSampleCamera::setScaledSize(QSizeF scaledSize)
{
    camera_->setScaledSize(scaledSize);
}

/// set the current name
void AMSampleCamera::setCurrentName(QString name)
{
    setName(name,currentIndex_);
}

/// set th current info
void AMSampleCamera::setCurrentInfo(QString info)
{
    setOtherData(info,currentIndex_);
}

/// sets the rotation for the given index
void AMSampleCamera::setRotation(double rotation, int index)
{
    if(-1 == index)
    {
        index = currentIndex_;
    }
    shapeList_[index]->setRotation(rotation);
}

/// sets the tilt for the given index
void AMSampleCamera::setTilt(double tilt, int index)
{
    if(-1 == index)
    {
        index = currentIndex_;
    }
    shapeList_[index]->setTilt(tilt);
}

/// sets the name of the specified index
void AMSampleCamera::setName(QString name, int index)
{
    if(isValid(index))
    {
        shapeList_[index]->setName(name);
    }
}

/// sets the data of the specifed index
void AMSampleCamera::setOtherData(QString data, int index)
{
    if(isValid(index))
    {
        shapeList_[index]->setOtherData(data);
    }
}

/// sets the id number of the specifed index
void AMSampleCamera::setIdNumber(double number, int index)
{
    if(isValid(index))
    {
        shapeList_[index]->setIdNumber(number);
    }
}

void AMSampleCamera::setVisible(bool visible)
{
    setVisible(visible,currentIndex_);
}

void AMSampleCamera::setVisible(bool visible, int index)
{
    shapeList_[index]->setVisible(visible);
}

/// sets whether to use camera matrix for transforms
void AMSampleCamera::setUseCameraMatrix(bool use)
{
    camera_->setUseCameraMatrix(use);
}

void AMSampleCamera::setOverrideMouseSelection(bool overrideMouseSelection)
{
    overrideMouseSelection_ = overrideMouseSelection;
}

void AMSampleCamera::setCurrentShapeIndex(int index)
{
    setOverrideMouseSelection(true);
    setCurrentIndex(index);
}

/// checks if an index is valid
bool AMSampleCamera::isValid(int index) const
{
    if(index <= index_ && index >= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/// checks to see if a shape is backwards
bool AMSampleCamera::isBackwards(int index) const
{
    if(isValid(index))
        return shapeList_[index]->backwards();
    else return 0;
}

bool AMSampleCamera::isBackwards() const
{
    return isBackwards(currentIndex_);
}

/// figures out camera parameters given a set of points and coordinates
/// the points should be based on top left corner = 0,0
/// and bottom right corner = 1,1
/// coordinates should be 6 unique coordinates
/// which define a shape
/// (no three of the points should lie upon the same line)
void AMSampleCamera::findCamera(QPointF points [], QVector3D coordinates[])
{

    /// center the image
    QPointF point[SAMPLEPOINTS];
    for(int i = 0; i < SAMPLEPOINTS; i++)
    {
        point[i].setX((points[i].x()-0.5));
        point[i].setY((points[i].y()-0.5));
    }

    /// auto draw shapes at the points, for convenience
    deleteCalibrationPoints();
    AMShapeData* shapes[SAMPLEPOINTS];
    for(int i = 0; i < SAMPLEPOINTS; i++)
    {
        index_++;
        shapes[i] = new AMShapeData();
        QVector<QVector3D> shapeCoordinates;
        shapeCoordinates<<QVector3D(-0.2,0.2,0)<<QVector3D(0.2,0.2,0)<<QVector3D(0.2,-0.2,0)<<QVector3D(-0.2,-0.2,0)<<QVector3D(-0.2,0.2,0);
        shapes[i]->setCoordinateShape(shapeCoordinates,5);
        shapes[i]->shiftTo(coordinates[i]);
        shapes[i]->setTilt(0);
        shapes[i]->setRotation(0);
        shapes[i]->setYAxisRotation(0);
        QString suffix;
        if(i == 1)
            suffix = "st";
        else if (i == 2)
            suffix = "nd";
        else if (i == 3)
            suffix = "rd";
        else
            suffix = "th";
        shapes[i]->setName(QString::number(i)+ suffix + " Calibration point");
        insertItem(shapes[i]);
        calibrationPoints_[i] = shapeList_[index_];
        updateShape(index_);
        setCurrentIndex(index_);
    }

    /// find the camera parameters
    camera_->getTransforms(point,coordinates);

    camera_->setCalibrationRun(true);

}

/// finds all the intersections, returns false if there is no valid beam
bool AMSampleCamera::findIntersections()
{
    if(beamModel_->positionOne().isEmpty() || beamModel_->positionTwo().isEmpty())
    {
        return false;
    }
    intersections_.clear();
    for(int i = 0; i <= index_; i++)
    {
        QVector<QVector3D> intersectionShape = findIntersectionShape(i);
        if(!intersectionShape.isEmpty())
        intersections_<<intersectionScreenShape(intersectionShape);

    }
    return true;
}

void AMSampleCamera::deleteShape(int index)
{
    if(isValid(index))
    {
        removeItem(index);
    }
}

int AMSampleCamera::samplePlateIndex() const
{
    return shapeList_.indexOf(samplePlateShape_);
}

int AMSampleCamera::rowCount(const QModelIndex &parent) const
{
    return shapeList_.count();
}

QVariant AMSampleCamera::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    switch(role)
    {
    case Qt::DisplayRole:
        return shapeList_.at(index.row())->name();
    default:
        return QVariant();
    }
}

bool AMSampleCamera::motorMovementenabled()
{
    return enableMotorMovement_;
}

const QList<AMShapeData *> AMSampleCamera::shapeList()
{
    return shapeList_;
}

bool AMSampleCamera::overrideMouseSelection()
{
    return overrideMouseSelection_;
}


/// creates a new rectangle, and initializes its data
void AMSampleCamera::startRectangle(QPointF position)
{

    index_++;
    position = undistortPoint(position);
    QVector<QVector3D> newShape;
    QVector3D coordinate[RECTANGLE_POINTS];
    double depth = camera_->focalLength();
    /// if drawing on shape, the point to start at lies
    /// on the shape to draw on.
    if(drawOnShapeEnabled_ && drawOnShapeSelected_)
    {
        QVector3D heightNormal = getHeightNormal(drawOnShape_);
        QVector3D widthNormal = getWidthNormal(drawOnShape_);
        QVector3D normal = getNormal(heightNormal, widthNormal);
        QVector3D rightNormal = getNormal(normal,heightNormal);
        QVector3D startPoint = getPointOnShape(position, normal);
        coordinate[TOPLEFT] = startPoint;
        coordinate[TOPRIGHT] = startPoint + 0.01*rightNormal;
        coordinate[BOTTOMRIGHT] =  startPoint + 0.01*rightNormal - 0.01*heightNormal;
        coordinate[BOTTOMLEFT] = startPoint - 0.01*heightNormal;
        coordinate[TOPCLOSE] = coordinate[TOPLEFT];

    }
    else
    {
        QVector3D start = camera_->transform2Dto3D(position,depth);
        QVector3D down = 0.001*downVector();
        QVector3D right = 0.001*rightVector();
        coordinate[TOPLEFT] = start;
        coordinate[TOPRIGHT] = start + right;
        coordinate[BOTTOMRIGHT] = start + right + down;
        coordinate[BOTTOMLEFT] = start + down;
        coordinate[TOPCLOSE] = start;
    }
    for(int i = 0; i<RECTANGLE_POINTS; i++)
    {
        newShape<<coordinate[i];
    }
    AMShapeData* newShapeData = new AMShapeData();

    newShapeData->setName("Shape " + QString::number(index_));
    newShapeData->setIdNumber(index_ * 13);
    newShapeData->setRotation(0);
    newShapeData->setTilt(0);
    newShapeData->setYAxisRotation(0);

    newShapeData->setCoordinateShape(newShape,5);
    insertItem(newShapeData);
//    updateShape(index_);

    setCurrentIndex(index_);
}

/// Changes the bottom right corner of the current rectangle
void AMSampleCamera::finishRectangle(QPointF position)
{
    if(!isValid(currentIndex_))return;
    position = undistortPoint(position);
    QVector3D topLeft = shapeList_[currentIndex_]->coordinate(TOPLEFT);
    QVector3D bottomRight = camera_->transform2Dto3D(position,depth(topLeft));
    QVector3D topRight;
    QVector3D bottomLeft;
    if(drawOnShapeEnabled_ && drawOnShapeSelected_)
    {
        bottomRight = getPointOnShape(position,getNormal(drawOnShape_));
        topRight = getWidthNormal(drawOnShape_);
        bottomLeft = QVector3D::normal(topRight,getNormal(drawOnShape_));
    }
    else
    {
        topRight = rightVector();
        bottomLeft = downVector();
    }
    topRight.normalize();
    bottomLeft.normalize();
    QVector3D shift = bottomRight - topLeft;
    if(topRight.isNull() && bottomLeft.isNull()) qDebug()<<"null shape";
    else if(topRight.isNull()) topRight = QVector3D::normal(QVector3D::normal(bottomLeft,shift),bottomLeft);
    else if(bottomLeft.isNull()) bottomLeft = QVector3D::normal(QVector3D::normal(topRight,shift),topRight);


    double topRightlength = (dot(topRight,shift));
    topRight = topRight*topRightlength;

    double topLeftlength = (dot(bottomLeft,shift));
    bottomLeft = bottomLeft*topLeftlength;


    shapeList_[currentIndex_]->setCoordinate(topLeft + topRight,TOPRIGHT);
    shapeList_[currentIndex_]->setCoordinate(topLeft+topRight+bottomLeft,BOTTOMRIGHT);
    shapeList_[currentIndex_]->setCoordinate(topLeft + bottomLeft,BOTTOMLEFT);
    updateShape(currentIndex_);

}

/// deletes a rectangle from the list
void AMSampleCamera::deleteRectangle(QPointF position)
{
    bool deleted = false;
    for(int i = 0; i < index_ + 1; i++)
    {
        deleted = false;
        if(contains(position,i))
        {
            AMShapeData* polygon = takeItem(i);
            for(int j = 0; j < SAMPLEPOINTS; j++)
            {
                if(polygon == calibrationPoints_[j])
                {
                    delete calibrationPoints_[j];
                    calibrationPoints_[j] = 0;
                    j = SAMPLEPOINTS;
                    deleted = true;

                }
                if(j < 3)
                {
                    if(polygon == beamMarkers_[j])
                    {
                        delete beamMarkers_[j];
                        beamMarkers_[j] = 0;
                        j = SAMPLEPOINTS;
                        deleted = true;
                    }
                }
            }
            if(!deleted && (polygon == samplePlateShape_))
            {
                delete samplePlateShape_;
                samplePlateShape_ = 0;
                deleted = true;
            }
            if(!deleted && (polygon == cameraConfigurationShape_))
            {
                delete cameraConfigurationShape_;
                cameraConfigurationShape_ = 0;
                deleted = true;
            }
            if(!deleted)
            {
                delete polygon;
                polygon = 0;
            }
        }
    }
}

/// selects a rectangle which is under the cursor
void AMSampleCamera::selectCurrentShape(QPointF position)
{
    int i = 0;
    while(isValid(i) && !contains(position,i))
    {
       i++;
    }
    if(!overrideMouseSelection())
    {
        if(i <= index_)
        {

            setCurrentIndex(i);

            currentVector_ = undistortPoint(position);
        }
        else
        {
            setCurrentIndex(index_ + 1);
        }
    }
    else
    {
        setOverrideMouseSelection(false);
    }
}

/// moves the currently selected rectangle by position + currentVector_
void AMSampleCamera::moveCurrentShape(QPointF position, int index)
{
    /// \todo store oldPosition as a vector?
    position = undistortPoint(position);
    if(index == -1) index = currentIndex_;
    if(index <= index_ && isValid(index))
    {
        QVector3D coordinate = shapeList_[index]->coordinate(TOPLEFT);
        double distance = depth(coordinate);
        QVector3D newPosition = camera_->transform2Dto3D(position,distance);
        QVector3D oldPosition = camera_->transform2Dto3D(currentVector_,distance);
        QVector3D shift = newPosition - oldPosition;
        shiftCoordinates(shift,index);
    }
    currentVector_ = (position);
}

/// moves all rectangles by position + rectangleVector_[index]
void AMSampleCamera::moveAllShapes(QPointF position)
{
    /// \todo store oldPosition as a vector?
    position = undistortPoint(position);
    for(int i = 0; i <= index_; i++)
    {
        double distance = camera_->focalLength();
        QVector3D newPosition = camera_->transform2Dto3D(position,distance);
        QVector3D oldPosition = camera_->transform2Dto3D(currentVector_,distance);
        QVector3D shift = newPosition - oldPosition;
        shiftCoordinates(shift,i);
    }
    currentVector_ = (position);
}

/// assigns current vector to mouse position
void AMSampleCamera::setShapeVectors(QPointF position)
{
    position = undistortPoint(position);
    currentVector_ = position;
}

/// changes the depth of all shapes by the same amount
void AMSampleCamera::zoomAllShapes(QPointF position)
{
    for(int i = 0; i <= index_; i++)
    {

        QVector3D coordinate = shapeList_[i]->coordinate(TOPLEFT);
        QVector3D change = depthVector(coordinate)*(zoomPoint_.y()/position.y() - 1);
        shiftCoordinates(change,i);
    }
    zoomPoint_ = position;
}

/// set the zoom position
void AMSampleCamera::setZoomPoint(QPointF position)
{
    zoomPoint_ = position;
}

/// change the current rectangles rotation
void AMSampleCamera::rotateRectangle(QPointF position)
{
    if(isValid(currentIndex_))
    {
        double rotation = shapeList_[currentIndex_]->rotation()+(position - currentVector_).x();
        shapeList_[currentIndex_]->setRotation(rotation);
        updateShape(currentIndex_);
        currentVector_ = position;
    }
}

/// change the current rectangles depth
void AMSampleCamera::zoomShape(QPointF position)
{
    if(isValid(currentIndex_))
    {
        QVector3D coordinate = shapeList_[currentIndex_]->coordinate(TOPLEFT);
        QVector3D change = depthVector(coordinate)*(zoomPoint_.y()/position.y() - 1);
        shiftCoordinates(change,currentIndex_);
    }
    zoomPoint_ = position;

}

/// moves the clicked point to appear under the crosshair, and gives the predicted motor coordinate
void AMSampleCamera::shiftToPoint(QPointF position, QPointF crosshairPosition)
{
    position = undistortPoint(position);
    if(isValid(currentIndex_))
    {
        QVector3D oldCoordinate = shapeList_[currentIndex_]->coordinate(TOPLEFT);
        QVector3D currentPosition = camera_->transform2Dto3D(position, depth(oldCoordinate));
        QVector3D newPosition = camera_->transform2Dto3D(crosshairPosition, depth(oldCoordinate));
        QVector3D shift = newPosition - currentPosition;

        /// current
        double inboardOutboard;
        double upStreamDownStream;
        double upDown;
        if(!enableMotorMovement_)
        {
            for(int i = 0; i <= index_; i++)
            {
                shiftCoordinates(shift,i);
            }
            motorCoordinate_ += shift;
            inboardOutboard = motorCoordinate_.x();
            upStreamDownStream = motorCoordinate_.y();
            upDown = motorCoordinate_.z();

        }
        else
        {
            shift += motorCoordinate_;
            inboardOutboard = shift.x();
            upStreamDownStream = shift.y();
            upDown = shift.z();

            moveMotors(inboardOutboard, upStreamDownStream, upDown);

        }

    }
}

/// finishes the grouping rectangle
void AMSampleCamera::finishGroupRectangle(QPointF position)
{
        position = undistortPoint(position);
    QPointF topLeft = groupRectangle_.first();
    QPolygonF newPolygon = constructRectangle(topLeft,position);
    groupRectangle_ = newPolygon;
}

/// constructs the group rectangle
void AMSampleCamera::startGroupRectangle(QPointF position)
{
        position = undistortPoint(position);
    QPolygonF polygon = constructRectangle(position,position);
    groupRectangle_ = polygon;
}

void AMSampleCamera::drawShape(QPointF position)
{
    currentPolygon_<<position;
}

void AMSampleCamera::finishShape()
{
    if(currentPolygon_.count() < 3)
    {
        currentPolygon_.clear();
        return;
    }
    currentPolygon_<<currentPolygon_.first();

    /// add the shape to shapeList_
    index_++;
    /// undistort all the points
    /// get the depth
    /// get the coordinates
    double distance = camera_->focalLength();
    QVector<QVector3D> coordinates;
    for(int i = 0; i < currentPolygon_.count(); i++)
    {
        currentPolygon_[i] = undistortPoint(currentPolygon_.at(i));
        if(drawOnShapeEnabled_ && drawOnShapeSelected_)
        {
             coordinates<<getPointOnShape(currentPolygon_.at(i),getNormal(drawOnShape_));
        }
        else
        {
            coordinates<<camera_->transform2Dto3D(currentPolygon_.at(i),distance);
        }
    }

    AMShapeData* polygon = new AMShapeData();
    polygon->setCoordinateShape(coordinates,coordinates.count());
    polygon->setName("Shape " + QString::number(index_));
    polygon->setRotation(0);
    polygon->setTilt(0);
    polygon->setYAxisRotation(0);
    insertItem(polygon);
    setCurrentIndex(index_);
    updateShape(index_);
    currentPolygon_.clear();

}


void AMSampleCamera::startMultiDraw()
{
    currentPolygon_.clear();
}

/// updates the shape for the given index
void AMSampleCamera::updateShape(int index)
{
   /// must set shape based on coordinates
    if(isValid(index))
    {
        shapeList_[index]->setShape(subShape(shapeList_.at(index)));
    }

}

/// updates all shapes
void AMSampleCamera::updateAllShapes()
{
    for(int i = 0; i <= index_; i++)
    {
        updateShape(i);
    }
}

/// sets the coordinates for index
void AMSampleCamera::setCoordinates(QVector3D coordinate, int index)
{
    if(isValid(index))
    {
        // get the current center
        QVector3D center = centerCoordinate(index);
        QVector3D shift = coordinate - center;
        // shift the coordinates to the new center
        shiftCoordinates(shift,index);
    }
}

/// Overload of setCoordinates, takes three doubles and an index
void AMSampleCamera::setCoordinates(double x, double y, double z, int index)
{
    if(-1 == index) index = currentIndex_;
    QVector3D coordinate;
    coordinate.setX(x);
    coordinate.setY(y);
    coordinate.setZ(z);
    setCoordinates(coordinate, index);
}

/// places a grid
void AMSampleCamera::placeGrid(QPointF position)
{
    for(double i = 0; i < 20.0; i++)
    {
        for(double j = 0; j < 20.0; j++)
        {
            index_ += 1;
            QPointF topLeft(i*0.05,j*0.05);
            QPointF bottomRight((i+1)*0.05,(j+1)*0.05);
            insertItem(new AMShapeData(constructRectangle(topLeft,bottomRight)));
            QVector<QVector3D> shape;
            for(int k = 0; k < 5; k++)
            {
               shape<<camera_->transform2Dto3D(shapeList_[index_]->shape()->at(k),camera_->focalLength());
            }
            shapeList_[index_]->setCoordinateShape(shape,5);
            shapeList_[index_]->setIdNumber(position.x());
            shapeList_[index_]->setTilt(0);
            shapeList_[index_]->setRotation(0);
            shapeList_[index_]->setYAxisRotation(0);
            updateShape(index_);
            setCurrentIndex(index_);
        }
    }

}

/// selects the first point for beam configuration
void AMSampleCamera::oneSelect()
{
    if(isValid(currentIndex_))
    {
        QVector<QVector3D> newBeamPosition = rotateShape(shapeList_[currentIndex_]);
        beamModel_->setPositionOne(newBeamPosition);


        emit beamChanged(beamModel_);

    }
    else
    {
        qDebug()<<"Invalid index, cannot add beam shape";
    }
}

/// selects the second point for beam configuration
void AMSampleCamera::twoSelect()
{

    if(isValid(currentIndex_))
    {
        QVector<QVector3D> newBeamPosition = rotateShape(shapeList_[currentIndex_]);
        beamModel_->setPositionTwo(newBeamPosition);
        emit beamChanged(beamModel_);
    }
    else
    {
        qDebug()<<"Invalid index, cannot add beam shape";
    }

}

/// sets whether to actually move the motors
void AMSampleCamera::enableMotorMovement(bool isEnabled)
{
    enableMotorMovement_ = isEnabled;
}

/// starts or stops motor tracking
void AMSampleCamera::enableMotorTracking(bool isEnabled)
{
    if(isEnabled)
    {
        connect(ssaManipulatorX_, SIGNAL(valueChanged(double)), this, SLOT(motorTracking(double)));
    }
    else
    {
        disconnect(ssaManipulatorX_,SIGNAL(valueChanged(double)),this,SLOT(motorTracking(double)));
    }
}

/// deletes all the shapes drawn in the camera calibration process
void AMSampleCamera::deleteCalibrationPoints()
{
    /// delete the calibration squares
    int index;
    for(int i = 0; i < SAMPLEPOINTS; i++)
    {
        if(index_ >= 0)
        {
            if(calibrationPoints_[i] != 0)
            {
                index = shapeList_.indexOf(calibrationPoints_[i]);
                if(isValid(index))
                {
                    removeItem(index);
                }
                delete calibrationPoints_[i];
                calibrationPoints_[i] = 0;
            }
        }
    }

}

/// sets the shape to draw on, and checks if it is valid
void AMSampleCamera::setDrawOnShape()
{
    if(isValid(currentIndex_))
    {
        drawOnShape_ = shapeList_[currentIndex_];
        drawOnShapeSelected_ = true;
    }
}

void AMSampleCamera::setDrawOnSamplePlate()
{
    if(samplePlateSelected_)
    {
        drawOnShape_ = samplePlateShape_;
        drawOnShapeSelected_ = true;
    }
}

void AMSampleCamera::setDrawOnShapeEnabled(bool enable)
{
    drawOnShapeEnabled_ = enable;
}

void AMSampleCamera::setBeamMarker(QPointF position, int index)
{

    int removalIndex = shapeList_.indexOf(beamMarkers_[index]);
    if(removalIndex >= 0)
    {
        removeItem(removalIndex);
    }

    startRectangle(position);
    beamMarkers_[index] = shapeList_[index_];
    beamMarkers_[index]->setName(QString("Beam Shape %1").arg(index + 1));

}

void AMSampleCamera::updateBeamMarker(QPointF position, int index)
{
    setCurrentIndex(shapeList_.indexOf(beamMarkers_[index]));
    finishRectangle(position);
}

void AMSampleCamera::beamCalibrate()
{

    setCurrentIndex(shapeList_.indexOf(beamMarkers_[0]));
    oneSelect();
    setCurrentIndex(shapeList_.indexOf(beamMarkers_[2]));
    twoSelect();
    if(shapeList_.isEmpty()) return;
    for(int i = 0; i < 3; i++)
    {
        int index = shapeList_.indexOf(beamMarkers_[i]);
        if(index >= 0)
        {
            AMShapeData* polygon = takeItem(index);
            if(polygon == beamMarkers_[i])
            {
                delete beamMarkers_[i];
                beamMarkers_[i] = 0;
                polygon = 0;
            }
            else
            {
                qDebug()<<"AMSampleCamera::beamCalibrate - error in deleting shapes";
                insertItem(polygon);
            }
        }
    }

}

void AMSampleCamera::setSamplePlate()
{
    if(isValid(currentIndex_))
    {
        for(int i = 0; i < 3; i++)
        {
            if(shapeList_[currentIndex_] == beamMarkers_[i])
            {
                qDebug()<<"AMSampleCamera::setSamplePlate - cannot set sample plate to be a beam shape.";
                samplePlateSelected_ = false;
                return;
            }
        }
        samplePlateShape_ = shapeList_[currentIndex_];
        samplePlateShape_->setName("Sample Plate");
        samplePlateShape_->setOtherData("Sample Plate");
        samplePlateSelected_ = true;
    }

}

void AMSampleCamera::setCameraConfigurationShape()
{
    if(isValid(currentIndex_))
    {
        cameraConfigurationShape_ = shapeList_[currentIndex_];
        cameraConfigurationShape_->setName("Configuration shape");
        cameraConfigurationShape_->setOtherData("Config");
    }
}

void AMSampleCamera::moveSamplePlateTo(QVector3D coordinate)
{
    if(samplePlateSelected_)
    {

        samplePlateShape_->shiftTo(coordinate);
        updateShape(samplePlateIndex());
    }
}

void AMSampleCamera::moveSamplePlate(int movement)
{
    if(samplePlateSelected_)
    {
        QVector3D direction = getWidthNormal(samplePlateShape_);
        double shapeWidth = (samplePlateShape_->coordinate(1) - samplePlateShape_->coordinate(0)).length();
        QVector3D shiftMovement = direction * movement/1000 * shapeWidth;
        samplePlateShape_->shift(shiftMovement);
        updateShape(samplePlateIndex());
    }
}

void AMSampleCamera::addBeamMarker(int index)
{
    if(beamMarkers_[index])
    {
        qDebug()<<"Beam marker is null";
        deleteShape(shapeList_.indexOf(beamMarkers_[index]));
    }
    qDebug()<<"creating new shape";
    AMShapeData* newBeamShape = new AMShapeData();
    int sampleIndex = shapeList_.indexOf(samplePlateShape_);
    if(sampleIndex >= 0)
    {
        QVector<QVector3D> coordinateShape = findIntersectionShape(shapeList_.indexOf(samplePlateShape_));
        if(!coordinateShape.isEmpty())
        {
            newBeamShape->setCoordinateShape(coordinateShape,coordinateShape.count());
            newBeamShape->setName(QString("Beam Marker %1").arg(index));
            newBeamShape->setRotation(0);
            newBeamShape->setTilt(0);
            newBeamShape->setYAxisRotation(0);
            insertItem(newBeamShape);
            beamMarkers_[index] = newBeamShape;
            index_++;
            setCurrentIndex(index_);
            updateShape(currentIndex_);
        }
    }
}

void AMSampleCamera::updateView()
{
    emit dataChanged(index(0), index(shapeList_.count()));
}

void AMSampleCamera::moveMotorTo(QVector3D coordinate)
{
    double x = coordinate.x();
    double y = coordinate.y();
    double z = coordinate.z();
    if(enableMotorMovement_)
    {
            bool success = moveMotors(x,y,z);
            if(!success)
                qDebug()<<"AMSampleCamera::moveMotorTo - failed to moveMotors";
    }
    else
    {
        qDebug()<<"Cannot move motors.";
    }

}

void AMSampleCamera::stopMotors()
{
    if(enableMotorMovement_)
    {
        ssaManipulatorRot_->stop();
        ssaManipulatorX_->stop();
        ssaManipulatorY_->stop();
        ssaManipulatorZ_->stop();
    }
    else
    {
        qDebug()<<"AMSampleCamera::stopMotors - cannot stop motors, movement is not enabled";
    }
}



/// tracks motor movement and shifts drawings accordingly
void AMSampleCamera::motorTracking(double)
{
    /// update coordinates, line edits, don't move the motor
    double tolerance = 0.01;
    double motorX = ssaManipulatorX_->value();
    double motorZ = ssaManipulatorZ_->value();
    double motorY = ssaManipulatorY_->value();
    double motorRotation = ssaManipulatorRot_->value();
    if(fabs(motorX-motorCoordinate_.x()) < tolerance && fabs(motorY-motorCoordinate_.y()) < tolerance && fabs(motorZ-motorCoordinate_.z()) < tolerance)
    {
        return;
    }

    emit motorMoved();

    setMotorCoordinate(motorX,motorY,motorZ,motorRotation);


}

void AMSampleCamera::motorsFinishedMoving()
{
    if(!ssaManipulatorX_->isMoving())
        if(!ssaManipulatorY_->isMoving())
            if(!ssaManipulatorZ_->isMoving())
                if(!ssaManipulatorRot_->isMoving())
                    emit moveSucceeded();
}

/// Constructor
AMSampleCamera::AMSampleCamera(QObject *parent) :
    QAbstractListModel(parent)
{
    crosshair_ = QPointF(0.5,0.5);
    crosshairLocked_ = false;
    index_ = -1;
    camera_ = new AMCamera();
    beamModel_ = new AMBeamConfiguration();
    centerOfRotation_ = QVector3D(0,0,0);
    directionOfRotation_ = QVector3D(0,-1,0);

    enableMotorMovement_ = false;
    drawOnShapeEnabled_ = false;
    drawOnShapeSelected_ = false;
    samplePlateSelected_ = false;

    distortion_ = true;

    for(int i= 0; i < SAMPLEPOINTS; i++)
    {
        calibrationPoints_[i] = new AMShapeData();
        if(i < 3)
        {
            beamMarkers_[i] = new AMShapeData();
        }
    }


    AMDatabase *db = AMDatabase::database("user");

    if(!db){
        // create the database
        db = AMDatabase::createDatabase("user", "/home/sgm/AcquamanData/userdata.db");
        if(!db)
            qDebug() << "Uh oh, no database created";
        else
        {
            bool success = true;

            AM::registerTypes();
            success &= AMDbObjectSupport::s()->registerDatabase(db);
            success &= AMDbObjectSupport::s()->registerClass<AMDbObject>();
            success &= AMDbObjectSupport::s()->registerClass<AMSamplePre2013>();
            success &= AMDbObjectSupport::s()->registerClass<AMCameraConfiguration>();
            success &= AMDbObjectSupport::s()->registerClass<AMBeamConfiguration>();
            success &= AMDbObjectSupport::s()->registerClass<AMSample>();
            success &= AMDbObjectSupport::s()->registerClass<AMSamplePlatePre2013>();

            qDebug() << "Status of registration is " << success;
        }
    }



    QList<int> matchIDs = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMBeamConfiguration>(),"name","defaultConfiguration");
    if(matchIDs.count() == 0)
    {
        QVector<QVector3D> positionOne;
        positionOne<<QVector3D(0.02,0.02,0)<<QVector3D(-0.02,0.02,0)<<QVector3D(-0.02,-0.02,0)<<QVector3D(0.02,-0.02,0)<<QVector3D(0.02,0.02,0);
        QVector<QVector3D> positionTwo;
        positionTwo<<QVector3D(0.02,0.02,1)<<QVector3D(-0.02,0.02,1)<<QVector3D(-0.02,-0.02,1)<<QVector3D(0.02,-0.02,1)<<QVector3D(0.02,0.02,1);
        beamModel_->setPositionOne(positionOne);
        beamModel_->setPositionTwo(positionTwo);
        beamModel_->setName("defaultConfiguration");
        bool success = beamModel_->storeToDb(db);
        if(!success)qDebug()<<"Failed to store default beam to database, in AMSampleCamera constructor";
    }
    else
    {
        beamModel_->loadFromDb(db,matchIDs.first());
    }


    /// add motor manipulators

    ssaManipulatorX_ = new CLSMAXvMotor("ssaManipulatorX", "SMTR16114I1022", "SSA Inboard/Outboard", true, 0.2, 2.0, this);
    ssaManipulatorX_->setContextKnownDescription("X");

    ssaManipulatorY_ = new CLSMAXvMotor("ssaManipulatorY", "SMTR16114I1023", "SSA Upstream/Downstream", true, 0.2, 2.0, this);
    ssaManipulatorY_->setContextKnownDescription("Y");

    ssaManipulatorZ_ = new CLSMAXvMotor("ssaManipulatorZ", "SMTR16114I1024", "SSA Up/Down", true, 0.2, 2.0, this);
    ssaManipulatorZ_->setContextKnownDescription("Z");

    ssaManipulatorRot_ = new CLSMAXvMotor("ssaManipulatorRot", "SMTR16114I1025", "SSA Rotation", false, 0.2, 2.0, this);
    ssaManipulatorRot_->setContextKnownDescription("R");

    //ssaManipulatorX_ = new SGMMAXvMotor("ssaManipulatorX", "SMTR16114I1022", "SSA Inboard/Outboard", true, 0.2, 2.0, this);
    //ssaManipulatorX_->setContextKnownDescription("X");

    //ssaManipulatorY_ = new SGMMAXvMotor("ssaManipulatorY", "SMTR16114I1023", "SSA Upstream/Downstream", true, 0.2, 2.0, this);
    //ssaManipulatorY_->setContextKnownDescription("Y");

    //ssaManipulatorZ_ = new SGMMAXvMotor("ssaManipulatorZ", "SMTR16114I1024", "SSA Up/Down", true, 0.2, 2.0, this);
    //ssaManipulatorZ_->setContextKnownDescription("Z");

    //ssaManipulatorRot_ = new SGMMAXvMotor("ssaManipulatorRot", "SMTR16114I1025", "SSA Rotation", false, 0.2, 2.0, this);
    //ssaManipulatorRot_->setContextKnownDescription("R");



    connect(ssaManipulatorX_, SIGNAL(moveSucceeded()), this, SLOT(motorsFinishedMoving()));
    connect(ssaManipulatorY_, SIGNAL(moveSucceeded()), this, SLOT(motorsFinishedMoving()));
    connect(ssaManipulatorZ_, SIGNAL(moveSucceeded()), this, SLOT(motorsFinishedMoving()));
    connect(ssaManipulatorRot_, SIGNAL(moveSucceeded()), this, SLOT(motorsFinishedMoving()));
}


/// shifts all coordinates by  the specified shift
void AMSampleCamera::shiftCoordinates(QVector3D shift, int index)
{
    if(isValid(index))
    {
        shapeList_[index]->shift(shift);
        updateShape(index);
    }
}


AMShapeData* AMSampleCamera::applySpecifiedRotation(const AMShapeData *shape, QVector3D direction, double angle) const
{
    AMShapeData* newShape = new AMShapeData();
    newShape->copy(shape);
    QVector3D centre = newShape->centerCoordinate();
    for(int i = 0; i < newShape->count(); i++)
    {
        newShape->setCoordinate(rotateCoordinate(newShape->coordinate(i),centre,direction,angle),i);
    }
    return newShape;
}

AMShapeData* AMSampleCamera::applySpecifiedRotation(const AMShapeData* shape, AMSampleCamera::AxisDirection direction) const
{
    double angle;
    QVector3D axis;
    if(direction == XAXIS)
    {
        angle = shape->tilt();
        axis = QVector3D(1,0,0);
    }
    else if(direction == YAXIS)
    {

        angle = shape->yAxisRotation();
        axis = QVector3D(0,1,0);
    }
    else if(direction == ZAXIS)
    {
        angle = shape->rotation();
        axis = QVector3D(0,0,1);
    }
    return applySpecifiedRotation(shape,axis,angle);
}

/// rotates a coordinate about the given line, by the given amount
/// \param coordinate: the coordinate to rotate
/// \param center: the point about which to rotate the coordinate
/// \param direction: the direction of the axis of rotation
/// \param rotation: the amount to rotate by, in radians
QVector3D AMSampleCamera::rotateCoordinate(QVector3D coordinate, QVector3D center, QVector3D direction, double rotation) const
{
    double u = direction.x();
    double v = direction.y();
    double w = direction.z();

    double x = coordinate.x();
    double y = coordinate.y();
    double z = coordinate.z();


    /// Each new coordinate has three terms - a 1-cos, a cos and a sin term
    /// to simplify the math, they will be computed as 3D vectors
    /// the xyz components will be the dot product of the terms with the trig terms

    QVector3D trigTerms(1-cos(rotation),cos(rotation),sin(rotation));

    /// as well, all terms contain a -coordinate.direction component

    double directionTerm = dot(coordinate,direction);

    /// all terms also use centerXdirection and directionXcoordinate
    QVector3D cXd = QVector3D::crossProduct(center,direction);
    QVector3D dXc = QVector3D::crossProduct(direction,coordinate);


    QVector3D terms [3];
    terms[0] = QVector3D((pow(v,2)+pow(w,2)),(-1*u*v),(-1*u*w));
    terms[1] = QVector3D((-1*v*u),(pow(u,2)+pow(w,2)),(-1*v*w));
    terms[2] = QVector3D((-1*u*w),(-1*v*w),(pow(u,2)+pow(v,2)));

    double coord [3];
    coord[0] = x;
    coord[1] = y;
    coord[2] = z;

    double centerXDistance[3];
    centerXDistance[0] = cXd.x();
    centerXDistance[1] = cXd.y();
    centerXDistance[2] = cXd.z();

    double distanceXCoordinate [3];
    distanceXCoordinate[0] = dXc.x();
    distanceXCoordinate[1] = dXc.y();
    distanceXCoordinate[2] = dXc.z();

    double directions [3];
    directions[0] = direction.x();
    directions[1] = direction.y();
    directions[2] = direction.z();
    ///complete the direction must do u*directionTerm


    QVector3D vector [3];
    double component [3];

    for(int i = 0; i < 3; i++)
    {
        vector[i] = QVector3D(dot(center,terms[i]) + directions[i]*directionTerm,coord[i],centerXDistance[i]+distanceXCoordinate[i]);
        component[i] = dot(vector[i],trigTerms);
    }

    QVector3D rotatedCoordinate(component[0],component[1],component[2]);
    return rotatedCoordinate;


}

/// applies rotation, tilt, and distortion to the shape
QPolygonF AMSampleCamera::subShape(const AMShapeData* shape) const
{
    AMShapeData* rotatedShape = new AMShapeData();
    rotatedShape->copy(shape);


    if(rotatedShape->rotation() != 0)
    {
        rotatedShape = applySpecifiedRotation(rotatedShape,ZAXIS);
    }
    if(rotatedShape->tilt() != 0)
    {
        rotatedShape = applySpecifiedRotation(rotatedShape,XAXIS);
    }
    if(rotatedShape->yAxisRotation() != 0)
    {
        rotatedShape = applySpecifiedRotation(rotatedShape,YAXIS);
    }

    QPolygonF newShape;
    for(int i = 0; i < rotatedShape->count(); i++)
    {
        newShape<<camera_->transform3Dto2D(rotatedShape->coordinate(i));
    }

    if(distortion_)
    {
        newShape = applyDistortion(newShape);
    }


    return newShape;
}

/// applies distortion to the shape
QPolygonF AMSampleCamera::applyDistortion(QPolygonF shape) const
{
    QPolygonF polygon;
    for(int i = 0; i < shape.count(); i++)
    {
        polygon<<camera_->distortPoint(shape[i]);
    }
    return polygon;
}

/// removes distortion from the shape
QPolygonF AMSampleCamera::removeDistortion(QPolygonF shape) const
{
    QPolygonF polygon;
    for(int i = 0; i < shape.count(); i++)
    {
        polygon<<undistortPoint(shape[i]);
    }
    return polygon;
}

/// removes distortion from the point
QPointF AMSampleCamera::undistortPoint(QPointF point) const
{
    return camera_->undistortPoint(point);
}

/// moves all the shapes based on change in motor movement
void AMSampleCamera::motorMovement(double x, double y, double z, double r)
{
    QVector3D rotationalOffset = QVector3D(-0.5,0,-0.35);
    QVector3D newPosition(x,y,z);
    QVector3D shift = newPosition - motorCoordinate_;
    QVector3D centerOfRotation = newPosition + rotationalOffset;
    QVector3D directionOfRotation = QVector3D(0,0,1);
    centerOfRotation_ = centerOfRotation;
    directionOfRotation_ = directionOfRotation;
    double rotation = r/180*M_PI;


    for(int i = 0; i <= index_; i++)
    {
        shiftCoordinates(shift,i);
        shapeList_[i]->setRotation(rotation);
    }



}

/// applies 3D rotation and tilt to the 3D shape in an AMShapeData
QVector<QVector3D> AMSampleCamera::rotateShape(const AMShapeData *shape) const
{
    AMShapeData* newShape = new AMShapeData();
    newShape->copy(shape);
    if(newShape->rotation() != 0) newShape = applySpecifiedRotation(newShape, ZAXIS);
    if(newShape->tilt() != 0) newShape = applySpecifiedRotation(newShape, XAXIS);
    if(newShape->yAxisRotation() != 0) newShape = applySpecifiedRotation(newShape, YAXIS);
    QVector<QVector3D> returnShape;
    for(int i = 0; i < newShape->count(); i++)
    {
        returnShape<<newShape->coordinate(i);
    }
    return returnShape;
}

/// transforms a coordinate point for display on the screen
QPointF AMSampleCamera::coordinateTransform(QPointF coordinate) const
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


QPolygonF AMSampleCamera::screenShape(QPolygonF shape) const
{
    QPolygonF newShape;
    for(int i = 0; i < shape.count(); i++)
    {
        newShape<<coordinateTransform(shape.at(i));
    }

    return newShape;
}

/// constructs a rectangle given the desired top and bottom corners
QPolygonF AMSampleCamera::constructRectangle(QPointF topLeft, QPointF bottomRight) const
{
    QPolygonF polygon;
    QPointF topRight(bottomRight.x(),topLeft.y());
    QPointF bottomLeft(topLeft.x(),bottomRight.y());
    polygon<<topLeft<<topRight<<bottomRight<<bottomLeft<<topLeft;
    return polygon;

}

/// returns the size of a rectangle with the given points
QSizeF AMSampleCamera::size(QPointF topLeft, QPointF bottomRight) const
{
    QSizeF size;
    QPointF difference = bottomRight - topLeft;
    size.setHeight(difference.y());
    size.setWidth(difference.x());
    return size;
}

/// Checks if the shape contains the point
bool AMSampleCamera::contains(QPointF position, int index) const
{
    return shapeList_[index]->shape()->containsPoint(position, Qt::OddEvenFill);
}

/// finds the depth of the given coordinate, returns the length of depthVector
double AMSampleCamera::depth(QVector3D coordinate) const
{
    QVector3D depth = depthVector(coordinate);
    return depth.length();
}

/** returns the depth vector
/// the depth vector gives the distance from the center of the camera to an object the camera is centered on.
/// depth vector is the same for all coordinates on the plane that is normal to depth vector, with the same distance */
QVector3D AMSampleCamera::depthVector(QVector3D coordinate) const
{
    QVector3D cameraCenterClose = camera_->transform2Dto3D(QPointF(0.5,0.5),1);// actual camera center
    QVector3D cameraCenterFar = camera_->transform2Dto3D(QPointF(0.5,0.5),2);// a second vector in the direction of the center
    QVector3D cameraDirection = cameraCenterFar - cameraCenterClose;
    cameraDirection.normalize();
    QVector3D cameraToCoordinate = coordinate - cameraCenterClose;
    double length = dot(cameraToCoordinate, cameraDirection) + 1;
    QVector3D depth = length * cameraDirection;
    return depth;
}

/// finds the intersection between the beam and the shape with given index
QVector<QVector3D> AMSampleCamera::findIntersectionShape(int index) const
{
    /// First find the shape on the same plane
    QVector<QVector3D> rotatedShape = rotateShape(shapeList_[index]);
    QVector<QVector3D> shape;
    QVector3D topLeft = rotatedShape.at(TOPLEFT);
    QVector3D hHat = getHeightNormal(shapeList_[index]);
    QVector3D wHat = getWidthNormal(shapeList_[index]);
    QVector3D nHat = QVector3D::normal(wHat,hHat);
    hHat = QVector3D::normal(nHat,wHat);
    int count = beamModel_->count();
    QVector3D l0 [count];
    QVector3D lHat [count];
    double distance[count];
    double numerator;
    double denominator;
    for(int i = 0; i < count; i++)
    {
        l0[i] = beamModel_->ray(i).at(0);
        lHat[i] = beamModel_->ray(i).at(1) - l0[i];
        lHat[i].normalize();
        numerator = dot((topLeft - l0[i]),nHat);
        denominator = dot(lHat[i],nHat);
        if(0 == denominator)
        {
            distance[i] = 1;
        }
        else
        {
            distance[i] = numerator/denominator;
        }
        shape<<l0[i]+lHat[i]*distance[i];
    }
    shape<<shape.first();

    /// need to find if the two shapes have any overlap
    /// transpose to h,w,n coordinates, and throw away the n part, to get two polygons
    // h component for each shape = h^(dot)coordinate
    QPolygonF originalShape;
    QPolygonF beamShape;
    double beamHComponent [count];
    double beamWComponent [count];
    int shapeCount = rotatedShape.count();
    double shapeHComponent [shapeCount];
    double shapeWComponent [shapeCount];
    double shapeNComponent [shapeCount];
    int totalCount = std::max(shapeCount,count);
    for(int i = 0; i < totalCount; i++)
    {
        if(i<count)
        {
            beamHComponent[i] = dot(hHat,shape[i]);
            beamWComponent[i] = dot(wHat,shape[i]);


            beamShape<<QPointF(beamWComponent[i],beamHComponent[i]);
        }

        if(i<shapeCount)
        {
            shapeHComponent[i] = dot(hHat,rotatedShape.at(i));
            shapeWComponent[i] = dot(wHat,rotatedShape.at(i));
            shapeNComponent[i] = dot(nHat,rotatedShape.at(i));

            originalShape<<QPointF(shapeWComponent[i],shapeHComponent[i]);
        }
    }

    QPolygonF intersection = originalShape.intersected(beamShape);
    if(intersection.isEmpty()) return QVector<QVector3D>();

    QVector3D point;
    QPointF oldPoint;
    QVector<QVector3D> intersectionShape;

    for(int i = 0; i < intersection.count(); i++)
    {
        oldPoint = intersection.at(i);
        point = oldPoint.x()*wHat + oldPoint.y()*hHat + shapeNComponent[i%shapeCount]*nHat;
        intersectionShape<<point;
    }

    return intersectionShape;


}

/// returns the QPolygonF of a set of QVector3D's that can be directly displayed on the screen
QPolygonF AMSampleCamera::intersectionScreenShape(QVector<QVector3D> shape3D) const
{

    AMShapeData* newShape = new AMShapeData();
    newShape->setCoordinateShape(shape3D,shape3D.count());
    newShape->setRotation(0);
    newShape->setTilt(0);
    newShape->setYAxisRotation(0);


    QPolygonF shape;
    shape = subShape(newShape);

    shape = screenShape(shape);

    return shape;

}

/// finds the coordinate of the center of the shape
QVector3D AMSampleCamera::centerCoordinate(int index) const
{
    return shapeList_[index]->centerCoordinate();
}



/// returns the dot product of two vectors
double AMSampleCamera::dot(QVector3D a, QVector3D b) const
{
    double value = QVector3D::dotProduct(a,b);
    return value;
}


QVector3D AMSampleCamera::getHeightNormal(const AMShapeData* shape) const
{
    QVector<QVector3D> rotatedShape = rotateShape(shape);
    QVector3D heightVector = rotatedShape.at(shape->count()-1) - rotatedShape.at(0);
    return heightVector.normalized();
}

QVector3D AMSampleCamera::getWidthNormal(const AMShapeData* shape) const
{
    QVector<QVector3D> rotatedShape = rotateShape(shape);
    QVector3D widthVector = rotatedShape.at(1) - rotatedShape.at(0);
    return widthVector.normalized();
}

/// gets the normal vector pointing out of a shape
QVector3D AMSampleCamera::getNormal(QVector3D heightVector, QVector3D widthVector) const
{
    return QVector3D::normal(heightVector,widthVector);
}

/// overload of getNormal, takes an AMShapeData
QVector3D AMSampleCamera::getNormal(const AMShapeData* shape) const
{
    return getNormal(getHeightNormal(shape),getWidthNormal(shape));
}

QVector3D AMSampleCamera::getPointOnShape(QPointF position, QVector3D nHat) const
{
    /// figure out line
    /// pick a depth to use, doesn't really matter what it is
    double depth = camera_->focalLength();
    QVector3D l0 = camera_->transform2Dto3D(position,depth);
    QVector3D lVector = camera_->transform2Dto3D(position,2*depth) - l0;
    QVector<QVector3D> rotatedShape = rotateShape(drawOnShape_);
    double numerator = dot((rotatedShape.at(TOPLEFT) - l0), nHat);
    double denominator = dot(lVector,nHat);
    double distance = 0;
    if(denominator == 0)
    {
        if(numerator == 0)
        {
            qDebug()<<"AMSampleCamera::getPointOnShape - Line is on plane";
        }
        else
        {
            qDebug()<<"AMSampleCamera::getPointOnShape - Line is parallel to plane";
        }
    }
    else
    {
        distance = numerator/denominator;
    }
    QVector3D pointOnShape = l0 + distance*lVector;
    return pointOnShape;
}

QVector3D AMSampleCamera::downVector() const
{
    QVector3D top = camera_->transform2Dto3D(QPointF(0.5,0),1);
    QVector3D bottom = camera_->transform2Dto3D(QPointF(0.5,1),1);
    QVector3D downVector = bottom - top;
    return downVector;
}

QVector3D AMSampleCamera::rightVector() const
{
    QVector3D left = camera_->transform2Dto3D(QPointF(0,0.5),1);
    QVector3D right = camera_->transform2Dto3D(QPointF(1,0.5),1);
    QVector3D rightVector = right - left;
    return rightVector;
}

void AMSampleCamera::insertItem(AMShapeData *item)
{
    beginInsertRows(QModelIndex(),index_,index_);
    shapeList_<<item;
    endInsertRows();
    connect(item, SIGNAL(nameChanged(QString)), this, SIGNAL(shapeNameChanged(QString)));
    updateShape(index_);
    emit shapesChanged();
}

void AMSampleCamera::removeItem(int index)
{
    index_--;
    beginRemoveRows(QModelIndex(),index,index);
    shapeList_.removeAt(index);
    endRemoveRows();
    emit shapesChanged();
}

AMShapeData *AMSampleCamera::takeItem(int index)
{
    index_--;
    beginRemoveRows(QModelIndex(),index,index);
    AMShapeData* oldShape = shapeList_.takeAt(index);
    endRemoveRows();
    emit shapesChanged();
    return oldShape;
}

QVector3D AMSampleCamera::beamIntersectionPoint(QVector3D samplePoint)
{
    double y = samplePoint.y();
    QList<QVector3D> beamOne = beamConfiguration()->beamOne();
    QVector3D midPointOne(0,0,0);
    foreach(QVector3D coordinate, beamOne)
    {
        midPointOne += coordinate;
    }
    midPointOne/=4;// not 4, number of points
    /// \todo finish
    return (QVector3D(0,0,0));
}

/// moves the x, y, z motors
bool AMSampleCamera::moveMotors(double x, double y, double z)
{
    bool success = false;
    if(motorMovementenabled())
    {
        success = true;
        AMControl::FailureExplanation failure [3];
//        const double XLIMITPOS = 0;
//        const double XLIMITNEG = 0;
//        const double YLIMITPOS = 0;
//        const double YLIMITNEG = 0;
//        const double ZLIMITPOS = 0;
//        const double ZLIMITNEG = 0;
//        if(x > XLIMITPOS)
//            x = XLIMIT;
//        if(x < XLIMITNEG)
//            x = XLIMITNEG;
        failure[0] = ssaManipulatorX_->move(x);
        failure[1] = ssaManipulatorY_->move(y);
        failure[2] = ssaManipulatorZ_->move(z);

        for(int i = 0; i < 3; i++)
        {
            if(failure[i] != AMControl::NoFailure)
            {
                success = false;
                qDebug()<<AMControl::failureExplanation(failure[i]);
            }
        }
    }
    return success;
}






