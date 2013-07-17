#include "AMShapeDataSet.h"

#include <QGraphicsRectItem>
#include <QResizeEvent>
#include <QVector3D>

#include <QMediaObject>
#include <QGraphicsVideoItem>
#include <QDebug>
#include <math.h>
#include "ui/AMCameraConfiguration.h"
//#include "ui/AMBeamConfiguration.h"


#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/SGM/SGMMAXvMotor.h"

#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMSample.h"

#include <QVector4D>



#define TOPLEFT 0
#define TOPRIGHT 1
#define BOTTOMRIGHT 2
#define BOTTOMLEFT 3
#define TOPCLOSE 4
#define RECTANGLE_POINTS 5

using namespace Eigen;


/// Constructor
AMShapeDataSet::AMShapeDataSet(QObject *parent) :
    QObject(parent)
{
    crosshair_ = QPointF(0.5,0.5);
    crosshairLocked_ = false;
    index_ = -1;
    cameraModel_ = new AMCameraConfiguration();
    beamModel_ = new AMBeamConfiguration();
    centerOfRotation_ = QVector3D(0,0,0);
    directionOfRotation_ = QVector3D(0,-1,0);

    enableMotorMovement_ = false;
    drawOnShapeEnabled_ = false;
    drawOnShapeSelected_ = false;

    for(int i= 0; i < SAMPLEPOINTS; i++)
    {
        calibrationPoints_[i] = new AMShapeData();
    }

    calibrationRun_ = false;

    // create the database
    AMDatabase *db = AMDatabase::createDatabase("user", "/home/sgm/AcquamanData/userdata.db");
    if(!db)
        qDebug() << "Uh oh, no database created";
    else
    {
        bool success = true;

        success &= AMDbObjectSupport::s()->registerDatabase(db);
        success &= AMDbObjectSupport::s()->registerClass<AMDbObject>();
        success &= AMDbObjectSupport::s()->registerClass<AMSample>();
        success &= AMDbObjectSupport::s()->registerClass<AMCameraConfiguration>();
        success &= AMDbObjectSupport::s()->registerClass<AMBeamConfiguration>();

        qDebug() << "Status of registration is " << success;
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
        if(!success)qDebug()<<"Failed to store default beam to database, in AMShapeDataSet constructor";
    }
    else
    {
        beamModel_->loadFromDb(db,matchIDs.first());
    }


    /// add motor manipulators

    ssaManipulatorX_ = new SGMMAXvMotor("ssaManipulatorX", "SMTR16114I1022", "SSA Inboard/Outboard", true, 0.2, 2.0, this);
    ssaManipulatorX_->setContextKnownDescription("X");

    ssaManipulatorY_ = new SGMMAXvMotor("ssaManipulatorY", "SMTR16114I1023", "SSA Upstream/Downstream", true, 0.2, 2.0, this);
    ssaManipulatorY_->setContextKnownDescription("Y");

    ssaManipulatorZ_ = new SGMMAXvMotor("ssaManipulatorZ", "SMTR16114I1024", "SSA Up/Down", true, 0.2, 2.0, this);
    ssaManipulatorZ_->setContextKnownDescription("Z");

    ssaManipulatorRot_ = new SGMMAXvMotor("ssaManipulatorRot", "SMTR16114I1025", "SSA Rotation", false, 0.2, 2.0, this);
    ssaManipulatorRot_->setContextKnownDescription("R");




}

/// returns the highest index of shapes in shapeList
int AMShapeDataSet::shapeListLength()
{
    return index_;
}

/// returns the current index
int AMShapeDataSet::currentIndex()
{
    return current_;
}

/// returns the cameraConfiguration
AMCameraConfiguration *AMShapeDataSet::cameraConfiguration()
{
    return cameraModel_;
}

/// returns the beam configuration
AMBeamConfiguration *AMShapeDataSet::beamConfiguration()
{
    return beamModel_;
}

/// returns the group rectangle in screen coordinates
QPolygonF AMShapeDataSet::groupRectangle()
{
    return screenShape(groupRectangle_);
}

/// returns motor x coordinate
double AMShapeDataSet::motorX()
{
    return motorCoordinate_.x();
}

/// returns motor y coordinate
double AMShapeDataSet::motorY()
{
    return motorCoordinate_.y();
}

/// returns motor z coordinate
double AMShapeDataSet::motorZ()
{
    return motorCoordinate_.z();
}

/// returns the motor rotation
double AMShapeDataSet::motorRotation()
{
    return motorRotation_;
}

/// returns the intersections
QVector<QPolygonF> AMShapeDataSet::intersections()
{
    return intersections_;
}

/// returns the crosshair x value
double AMShapeDataSet::crosshairX()
{
    return crosshair_.x();
}

/// returns the crosshair y value
double AMShapeDataSet::crosshairY()
{
    return crosshair_.y();
}

/// returns the crosshair point
QPointF AMShapeDataSet::crosshair()
{
    return crosshair_;
}

/// is the crossHair locked?
bool AMShapeDataSet::crosshairLocked()
{
    return crosshairLocked_;
}

/// returns the view size of the video widget
QSizeF AMShapeDataSet::viewSize()
{
    return viewSize_;
}

/// returns the scaled size of the video widget
QSizeF AMShapeDataSet::scaledSize()
{
    return scaledSize_;
}

/// is the cameraMatrix being used?
bool AMShapeDataSet::useCameraMatrix()
{
    return useCameraMatrix_;
}

/// The polyon currently being drawn
QPolygonF AMShapeDataSet::currentPolygon()
{
    return screenShape(currentPolygon_);
}

/// return the current name
QString AMShapeDataSet::currentName()
{
    return name(current_);
}

/// returns the name of the specified index
QString AMShapeDataSet::name(int index)
{
    if(isValid(index))
    {
        return shapeList_[index]->name();
    }
    else return "";
}

/// return the current info
QString AMShapeDataSet::currentInfo()
{
    return data(current_);
}

/// return the data of the specified index
QString AMShapeDataSet::data(int index)
{
    if(isValid(index))
    {
        return shapeList_[index]->otherData();
    }
    else
        return "";
}

/// returns the idNumber of the specified index
double AMShapeDataSet::idNumber(int index)
{
    if(isValid(index))
    {
        return shapeList_[index]->idNumber();
    }
    else
        return 0;
}

/// returns the rotation of the given index
double AMShapeDataSet::rotation(int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    return shapeList_[index]->rotation();
}

/// returns the tilt for the given index
double AMShapeDataSet::tilt(int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    return shapeList_[index]->tilt();
}

/// returns a pointer to the AMShapeData with  index, by default current_
AMShapeData *AMShapeDataSet::currentShape(int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    if(isValid(index))
        return shapeList_[index];
    else return 0;
}

/// returns the shape in screen coordinates
QPolygonF AMShapeDataSet::shape(int index)
{
    QPolygonF shape = *shapeList_[index]->shape();

    return screenShape(shape);

}

/// returns the coordinate of index
QVector3D AMShapeDataSet::coordinate(int index)
{
    return centerCoordinate(index);
}

/// calls coordinate with index equal to current_
QVector3D AMShapeDataSet::currentCoordinate()
{
    return coordinate(current_);
}

/// set the current index
void AMShapeDataSet::setCurrentIndex(int current)
{
    current_ = current;
}

/// changes the current camera model
void AMShapeDataSet::setCameraModel(AMCameraConfiguration *model)
{
    if(cameraModel_ != model)
    {
        cameraModel_ = model;
    }
    calibrationRun_ = cameraModel_->hasMatrix();
    updateAllShapes();
}

/// sets the beam model to the given model
void AMShapeDataSet::setBeamModel(AMBeamConfiguration *model)
{
    beamModel_ = model;
}

/// sets the motor coordinate to xyzr
void AMShapeDataSet::setMotorCoordinate(double x, double y, double z, double r)
{
    motorMovement(x,y,z,r);
    motorCoordinate_.setX(x);
    motorCoordinate_.setY(y);
    motorCoordinate_.setZ(z);

    motorRotation_ = r;
}

/// toggles distortion on or off
void AMShapeDataSet::toggleDistortion()
{
    distortion_ = !distortion_;
    updateAllShapes();
}

/// set the crosshair x value
void AMShapeDataSet::setCrosshairX(double x)
{
    crosshair_.setX(x);
}

/// set the crosshair y value
void AMShapeDataSet::setCrosshairY(double y)
{
    crosshair_.setY(y);
}

/// set the crosshair point
void AMShapeDataSet::setCrosshair(QPointF crosshair)
{
    crosshair_ = crosshair;
}

/// set whether the crosshair is locked
void AMShapeDataSet::setCrosshairLocked(bool locked)
{
    crosshairLocked_ = locked;
}

/// set the view size
void AMShapeDataSet::setViewSize(QSizeF viewSize)
{
    viewSize_ = viewSize;
}

/// set the scaled size
void AMShapeDataSet::setScaledSize(QSizeF scaledSize)
{
    scaledSize_ = scaledSize;
}

/// set the current name
void AMShapeDataSet::setCurrentName(QString name)
{
    setName(name,current_);
}

/// set th current info
void AMShapeDataSet::setCurrentInfo(QString info)
{
    setData(info,current_);
}

/// sets the rotation for the given index
void AMShapeDataSet::setRotation(double rotation, int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    shapeList_[index]->setRotation(rotation);
}

/// sets the tilt for the given index
void AMShapeDataSet::setTilt(double tilt, int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    shapeList_[index]->setTilt(tilt);
}

/// sets the name of the specified index
void AMShapeDataSet::setName(QString name, int index)
{
    if(isValid(index))
    {
        shapeList_[index]->setName(name);
    }
}

/// sets the data of the specifed index
void AMShapeDataSet::setData(QString data, int index)
{
    if(isValid(index))
    {
        shapeList_[index]->setOtherData(data);
    }
}

/// sets the id number of the specifed index
void AMShapeDataSet::setIdNumber(double number, int index)
{
    if(isValid(index))
    {
        shapeList_[index]->setIdNumber(number);
    }
}

/// sets whether to use camera matrix for transforms
void AMShapeDataSet::setUseCameraMatrix(bool use)
{
    useCameraMatrix_ = use;
}

/// checks if an index is valid
bool AMShapeDataSet::isValid(int index)
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
bool AMShapeDataSet::isBackwards(int index)
{
    if(index == -1) index = current_;
    return shapeList_[index]->backwards();
}

/// figures out camera parameters given a set of points and coordinates
/// the points should be based on top left corner = 0,0
/// and bottom right corner = 1,1
/// coordinates should be 6 unique coordinates
/// which define a shape
/// (no three of the points should lie upon the same line)
void AMShapeDataSet::findCamera(QPointF points [], QVector3D coordinates[])
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
        shapeList_<<shapes[i];
        calibrationPoints_[i] = shapeList_[index_];
        updateShape(index_);
        current_ = index_;
    }

    /// find the camera parameters
    getTransforms(point,coordinates);

    calibrationRun_ = true;

}

/// finds all the intersections, returns false if there is no valid beam
bool AMShapeDataSet::findIntersections()
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


/// creates a new rectangle, and initializes its data
void AMShapeDataSet::startRectangle(QPointF position)
{

    index_++;
    position = undistortPoint(position);
    QVector<QVector3D> newShape;
    QVector3D coordinate[RECTANGLE_POINTS];
    double depth = cameraModel_->cameraFocalLength();
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
        QVector3D start = transform2Dto3D(position,depth);
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
    shapeList_<<new AMShapeData();
    shapeList_[index_]->setName("Shape " + QString::number(index_));
    shapeList_[index_]->setOtherData("Coordinate:");
    shapeList_[index_]->setIdNumber(index_ * 13);
    shapeList_[index_]->setRotation(0);
    shapeList_[index_]->setTilt(0);
    shapeList_[index_]->setYAxisRotation(0);

    shapeList_[index_]->setCoordinateShape(newShape,5);
    updateShape(index_);

    current_ = index_;
}

/// Changes the bottom right corner of the current rectangle
void AMShapeDataSet::finishRectangle(QPointF position)
{
    if(!isValid(current_))return;
    position = undistortPoint(position);
    QVector3D topLeft = shapeList_[current_]->coordinate(TOPLEFT);
    QVector3D bottomRight = transform2Dto3D(position,depth(topLeft));

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



    shapeList_[current_]->setCoordinate(topLeft + topRight,TOPRIGHT);
    shapeList_[current_]->setCoordinate(bottomRight,BOTTOMRIGHT);//(topLeft+topRight+bottomLeft,BOTTOMRIGHT);
    shapeList_[current_]->setCoordinate(topLeft + bottomLeft,BOTTOMLEFT);
    updateShape(current_);

}

/// deletes a rectangle from the list
void AMShapeDataSet::deleteRectangle(QPointF position)
{
    bool deleted = false;
    for(int i = 0; i < index_ + 1; i++)
    {
        if(contains(position,i))
        {
            AMShapeData* polygon = shapeList_.takeAt(i);

            for(int j = 0; j < SAMPLEPOINTS; j++)
            {
                if(polygon == calibrationPoints_[j])
                {
                    delete calibrationPoints_[j];
                    calibrationPoints_[j] = 0;
                    j = SAMPLEPOINTS;
                    deleted = true;

                }
            }
            if(!deleted)
            {
                delete polygon;
                polygon = 0;
            }
           index_--;
        }
    }
}

/// selects a rectangle which is under the cursor
void AMShapeDataSet::selectCurrentShape(QPointF position)
{
    int i = 0;
    while(isValid(i) && !contains(position,i))
    {
       i++;
    }
    if(i <= index_)
    {
        current_ = i;
        currentVector_ = undistortPoint(position);
    }
    else current_ = index_ + 1;
}

/// moves the currently selected rectangle by position + currentVector_
void AMShapeDataSet::moveCurrentShape(QPointF position, int index)
{
    /// \todo store oldPosition as a vector?
    position = undistortPoint(position);
    if(index == -1) index = current_;
    if(index <= index_ && isValid(index))
    {
        QVector3D coordinate = shapeList_[index]->coordinate(TOPLEFT);
        double distance = depth(coordinate);
        QVector3D newPosition = transform2Dto3D(position,distance);
        QVector3D oldPosition = transform2Dto3D(currentVector_,distance);
        QVector3D shift = newPosition - oldPosition;
        shiftCoordinates(shift,index);
    }
    currentVector_ = (position);
}

/// moves all rectangles by position + rectangleVector_[index]
void AMShapeDataSet::moveAllShapes(QPointF position)
{
    /// \todo store oldPosition as a vector?
    position = undistortPoint(position);
    for(int i = 0; i <= index_; i++)
    {
        double distance = cameraModel_->cameraFocalLength();
        QVector3D newPosition = transform2Dto3D(position,distance);
        QVector3D oldPosition = transform2Dto3D(currentVector_,distance);
        QVector3D shift = newPosition - oldPosition;
        shiftCoordinates(shift,i);
    }
    currentVector_ = (position);
}

/// assigns current vector to mouse position
void AMShapeDataSet::setShapeVectors(QPointF position)
{
    position = undistortPoint(position);
    currentVector_ = position;
}

/// changes the depth of all shapes by the same amount
void AMShapeDataSet::zoomAllShapes(QPointF position)
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
void AMShapeDataSet::setZoomPoint(QPointF position)
{
    zoomPoint_ = position;
}

/// change the current rectangles rotation
void AMShapeDataSet::rotateRectangle(QPointF position)
{
    if(isValid(current_))
    {
        double rotation = shapeList_[current_]->rotation()+(position - currentVector_).x();
        shapeList_[current_]->setRotation(rotation);
        updateShape(current_);
        currentVector_ = position;
    }
}

/// change the current rectangles depth
void AMShapeDataSet::zoomShape(QPointF position)
{
    if(isValid(current_))
    {
        QVector3D coordinate = shapeList_[current_]->coordinate(TOPLEFT);
        QVector3D change = depthVector(coordinate)*(zoomPoint_.y()/position.y() - 1);
        shiftCoordinates(change,current_);
    }
    zoomPoint_ = position;

}

/// moves the clicked point to appear under the crosshair, and gives the predicted motor coordinate
void AMShapeDataSet::shiftToPoint(QPointF position, QPointF crosshairPosition)
{
    position = undistortPoint(position);
    if(isValid(current_))
    {
        QVector3D oldCoordinate = shapeList_[current_]->coordinate(TOPLEFT);
        QVector3D currentPosition = transform2Dto3D(position, depth(oldCoordinate));
        QVector3D newPosition = transform2Dto3D(crosshairPosition, depth(oldCoordinate));
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

            qDebug()<<"AMShapeDataSet::shiftToPoint - Attempting to move motors";
            qDebug()<<"in/out currently at"<<ssaManipulatorX_->value();
            qDebug()<<"shifting in/out to"<<inboardOutboard;
            ssaManipulatorX_->move(inboardOutboard);

            qDebug()<<"y is currently at:"<<ssaManipulatorY_->value();
            qDebug()<<"Shifting upstream/downstream to:"<<upStreamDownStream;

            qDebug()<<"up/down currently at"<<ssaManipulatorZ_->value();
            qDebug()<<"changing up/down to "<<upDown;
            ssaManipulatorZ_->move(upDown);

        }

    }
}

/// finishes the grouping rectangle
void AMShapeDataSet::finishGroupRectangle(QPointF position)
{
        position = undistortPoint(position);
    QPointF topLeft = groupRectangle_.first();
    QPolygonF newPolygon = constructRectangle(topLeft,position);
    groupRectangle_ = newPolygon;
}

/// constructs the group rectangle
void AMShapeDataSet::startGroupRectangle(QPointF position)
{
        position = undistortPoint(position);
    QPolygonF polygon = constructRectangle(position,position);
    groupRectangle_ = polygon;
}

void AMShapeDataSet::drawShape(QPointF position)
{
    currentPolygon_<<position;
}

void AMShapeDataSet::finishShape()
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
    double distance = cameraModel_->cameraFocalLength();
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
            coordinates<<transform2Dto3D(currentPolygon_.at(i),distance);
        }
    }

    AMShapeData* polygon;
    polygon->setCoordinateShape(coordinates,coordinates.count());
    polygon->setName("Shape " + QString::number(index_));
    polygon->setRotation(0);
    polygon->setTilt(0);
    polygon->setYAxisRotation(0);
    shapeList_<<polygon;
    current_ = index_;
    updateShape(index_);
    currentPolygon_.clear();

}


void AMShapeDataSet::startMultiDraw()
{
    currentPolygon_.clear();
}

/// updates the shape for the given index
void AMShapeDataSet::updateShape(int index)
{
   /// must set shape based on coordinates
    shapeList_[index]->setShape(subShape(*shapeList_.at(index)));

}

/// updates all shapes
void AMShapeDataSet::updateAllShapes()
{
    for(int i = 0; i <= index_; i++)
    {
        updateShape(i);
    }
}

/// sets the coordinates for index
void AMShapeDataSet::setCoordinates(QVector3D coordinate, int index)
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
void AMShapeDataSet::setCoordinates(double x, double y, double z, int index)
{
    if(-1 == index) index = current_;
    QVector3D coordinate;
    coordinate.setX(x);
    coordinate.setY(y);
    coordinate.setZ(z);
    setCoordinates(coordinate, index);
}

/// places a grid
void AMShapeDataSet::placeGrid(QPointF position)
{
    for(double i = 0; i < 20.0; i++)
    {
        for(double j = 0; j < 20.0; j++)
        {
            index_ += 1;
            QPointF topLeft(i*0.05,j*0.05);
            QPointF bottomRight((i+1)*0.05,(j+1)*0.05);
            shapeList_<<new AMShapeData(constructRectangle(topLeft,bottomRight));
            QVector<QVector3D> shape;
            for(int k = 0; k < 5; k++)
            {
               shape<<transform2Dto3D(shapeList_[index_]->shape()->at(k),cameraModel_->cameraFocalLength());
            }
            shapeList_[index_]->setCoordinateShape(shape,5);
            shapeList_[index_]->setIdNumber(position.x());
            shapeList_[index_]->setTilt(0);
            shapeList_[index_]->setRotation(0);
            shapeList_[index_]->setYAxisRotation(0);
            updateShape(index_);
            current_ = index_;
        }
    }

}

/// selects the first point for beam configuration
void AMShapeDataSet::oneSelect()
{
    if(isValid(current_))
    {
        QVector<QVector3D> newBeamPosition = rotateShape(*shapeList_[current_]);
        beamModel_->setPositionOne(newBeamPosition);


        emit beamChanged(beamModel_);

    }
    else
    {
        qDebug()<<"Invalid index, cannot add beam shape";
    }
}

/// selects the second point for beam configuration
void AMShapeDataSet::twoSelect()
{

    if(isValid(current_))
    {
        QVector<QVector3D> newBeamPosition = rotateShape(*shapeList_[current_]);
        beamModel_->setPositionTwo(newBeamPosition);
        emit beamChanged(beamModel_);
    }
    else
    {
        qDebug()<<"Invalid index, cannot add beam shape";
    }

}

/// sets whether to actually move the motors
void AMShapeDataSet::enableMotorMovement(bool isEnabled)
{
    enableMotorMovement_ = isEnabled;
}

/// starts or stops motor tracking
void AMShapeDataSet::enableMotorTracking(bool isEnabled)
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
void AMShapeDataSet::deleteCalibrationPoints()
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
                    shapeList_.removeAt(index);
                    index_--;
                }
                delete calibrationPoints_[i];
                calibrationPoints_[i] = 0;
            }
        }
    }

}

/// sets the shape to draw on, and checks if it is valid
void AMShapeDataSet::setDrawOnShape()
{
    if(isValid(current_))
    {
        drawOnShape_ = *shapeList_[current_];
        drawOnShapeSelected_ = true;
    }
}

void AMShapeDataSet::setDrawOnShapeEnabled(bool enable)
{
    drawOnShapeEnabled_ = enable;
}



/// tracks motor movement and shifts drawings accordingly
void AMShapeDataSet::motorTracking(double)
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


/// shifts all coordinates by  the specified shift
void AMShapeDataSet::shiftCoordinates(QVector3D shift, int index)
{
    if(isValid(index))
    {
        shapeList_[index]->shift(shift);
        updateShape(index);
    }
}

/// applies the rotation to the shape
AMShapeData AMShapeDataSet::applyRotation(AMShapeData shape)
{
    QVector3D center = shape.centerCoordinate();
    double rotation = shape.rotation();
    for(int i = 0; i < shape.count(); i++)
    {
        shape.setCoordinate(getRotatedPoint(shape.coordinate(i), rotation, center),i);
    }
    return shape;
}

/// finds the new coordinate of a point, given its coordinate, rotation, and center point
QVector3D AMShapeDataSet::getRotatedPoint(QVector3D point, double rotation, QVector3D center)
{
    QVector3D direction = QVector3D(0,0,1);
    point = rotateCoordinate(point,center,direction,rotation);
    return point;
}

/// Applies tilt to a shape
AMShapeData AMShapeDataSet::applyTilt(AMShapeData shape)
{
    double tilt = shape.tilt();
    QVector3D center = shape.centerCoordinate();
    for(int i = 0; i<shape.count(); i++)
    {
        shape.setCoordinate(getTiltedPoint(shape.coordinate(i),tilt,center),i);
    }
    return shape;
}

/// Applies tilt to a point
QVector3D AMShapeDataSet::getTiltedPoint(QVector3D point, double tilt, QVector3D center)
{
    QVector3D direction(1,0,0);
    QVector3D coordinate = rotateCoordinate(point,center,direction,tilt);
    return coordinate;
}

AMShapeData AMShapeDataSet::applyYAxisRotation(AMShapeData shape)
{
    double rotation = shape.yAxisRotation();
    QVector3D center = shape.centerCoordinate();
    for(int i = 0; i<shape.count(); i++)
    {
        shape.setCoordinate(getYAxisRotatedPoint(shape.coordinate(i),rotation,center),i);
    }
    return shape;
}

QVector3D AMShapeDataSet::getYAxisRotatedPoint(QVector3D point, double rotation, QVector3D center)
{
    QVector3D direction(0,1,0);
    QVector3D coordinate = rotateCoordinate(point, center, direction, rotation);
    return coordinate;
}

AMShapeData AMShapeDataSet::applySpecifiedRotation(AMShapeData shape, QVector3D direction, double angle)
{
    QVector3D centre = shape.centerCoordinate();
    for(int i = 0; i < shape.count(); i++)
    {
        shape.setCoordinate(rotateCoordinate(shape.coordinate(i),centre,direction,angle),i);
    }
    return shape;
}

AMShapeData AMShapeDataSet::applySpecifiedRotation(AMShapeData shape, AMShapeDataSet::AxisDirection direction)
{
    double angle;
    QVector3D axis;
    if(direction == XAXIS)
    {
        angle = shape.tilt();
        axis = QVector3D(1,0,0);
    }
    else if(direction == YAXIS)
    {

        angle = shape.yAxisRotation();
        axis = QVector3D(0,1,0);
    }
    else if(direction == ZAXIS)
    {
        angle = shape.rotation();
        axis = QVector3D(0,0,1);
    }
    return applySpecifiedRotation(shape,axis,angle);
}

/// rotates a coordinate about the given line, by the given amount
/// \param coordinate: the coordinate to rotate
/// \param center: the point about which to rotate the coordinate
/// \param direction: the direction of the axis of rotation
/// \param rotation: the amount to rotate by, in radians
QVector3D AMShapeDataSet::rotateCoordinate(QVector3D coordinate, QVector3D center, QVector3D direction, double rotation)
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
QPolygonF AMShapeDataSet::subShape(AMShapeData shape)
{
    if(shape.rotation() != 0) shape = applyRotation(shape);
    if(shape.tilt() != 0) shape = applyTilt(shape);
    if(shape.yAxisRotation() != 0) shape = applySpecifiedRotation(shape,YAXIS);


    QPolygonF newShape;
    for(int i = 0; i < shape.count(); i++)
    {
        newShape<<transform3Dto2D(shape.coordinate(i));
    }
//    newShape<<newShape.first();


    if(distortion_)
    {
        newShape = applyDistortion(newShape);
//        if(motorCoordinate_.z() != 0)
//        newShape = removeDistortion(newShape);
    }


    return newShape;
}

/// applies distortion to the shape
QPolygonF AMShapeDataSet::applyDistortion(QPolygonF shape)
{
    QPolygonF polygon;
    for(int i = 0; i < shape.count(); i++)
    {
        polygon<<distortPoint(shape[i]);
    }
    return polygon;
}

/// applies distortion to the point
QPointF AMShapeDataSet::distortPoint(QPointF point)
{
    /// Distortion is given by:
    // newRadius = oldRadius*(1+k1*oldRadius^2)
    /// where k1 is the distortion  factor
    double distortionFactor = cameraModel_->cameraDistortion();
    QPointF newPoint;
    double x = point.x()-0.5;
    double y = point.y()-0.5;

    newPoint.setX((x*(1+distortionFactor*(x*x+y*y)))+0.5);
    newPoint.setY((y*(1+distortionFactor*(x*x+y*y)))+0.5);
    return newPoint;
}

/// removes distortion from the shape
QPolygonF AMShapeDataSet::removeDistortion(QPolygonF shape)
{
    QPolygonF polygon;
    for(int i = 0; i < shape.count(); i++)
    {
        polygon<<undistortPoint(shape[i]);
    }
    return polygon;
}

/// removes distortion from the point
QPointF AMShapeDataSet::undistortPoint(QPointF point)
{
    /// to remove distortion have to solve the equation for distorting the point
    /// for the original radius
    /// ie: distortedRadius = k1*r^3 + r -- solve for r
    /// description of method below

    ///this does not work at the center, so ignore it (it doesn't move)
    if(point.x() == 0.5 && point.y() == 0.5)
        return point;
    // set up
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

    /// a (+-) b = a (+-) sqrt(c)/2 = rd/2k (+,-) sqrt((rd/k)^2+(4/(27*k^3)))/2

    double c = distortedRadius*distortedRadius/(distortionFactor*distortionFactor) + (4)/(27*pow(distortionFactor,3));
    double a = distortedRadius/(2*distortionFactor);
    double b;
    if(c<0)
    {
        /// the root is imaginary, so take the absolute value of c and square root it, theta = atan(b/a)
        /// b(0,1) = (+-)sqrt(c)/2
        // negate c, sow we can square root it
        c = c*(-1);
        b = sqrt(c)/2.0;

        // have a + j*b
        // put into polar coordinates
        // tan(theta) = b/a

        theta = atan(b/a);

        /// a+b and a-b give the same absolute value
        /// sqrt(a^2 + b^2)
        // wo here is actually wo^3
        // wo^3*e^(j*theta)
        // wo = cube root of ^
        wo = sqrt(a*a + b*b);
        QPointF complexPoint [6];
        QPointF conjugatePoint [6];
        double pointAngles[6];

        // set all the angles for cube root calculations
        // six possibilites (two differen wo^3's, three cube roots for each)
        pointAngles[0] = theta/3.0;
        pointAngles[3] = theta/-3.0;
        pointAngles[1] = pointAngles[0] + 2.0*M_PI/3.0;
        pointAngles[4] = pointAngles[3] + 2.0*M_PI/3.0;
        pointAngles[2] = pointAngles[0] - 2.0*M_PI/3.0;
        pointAngles[5] = pointAngles[3] - 2.0*M_PI/3.0;
        /// calculate the six possible points for r

        //|wo| = |cubeRoot(|wo^3|)|
        wo = pow(wo,1.0/3.0);

        for(int i = 0; i < 6; i++)
        {
            //calculate the two components (x = real axis, y = j axis)
            complexPoint[i].setX(wo*cos(pointAngles[i]));
            complexPoint[i].setY(wo*sin(pointAngles[i]));
            // find the conjugate
            conjugatePoint[i].setX(complexPoint[i].x());
            conjugatePoint[i].setY(-1*complexPoint[i].y());
            // factor to find r from wo (see above)
            conjugatePoint[i] *= 1/(distortionFactor*3*wo*wo);
            complexPoint[i] = complexPoint[i] - conjugatePoint[i];
            // complex point is now one possible undistorted radius
           // qDebug()<<"coordinate"<<QString::number(i)<<QString::number(complexPoint[i].x())<<QString::number(complexPoint[i].y());

        }
        /// for points within the radius of the screen, and for small distortions, complexPoint[1] gives the correct transform
        /// if distortion goes too high, the overlap creating a loss of image will not be corrected
        newRadius = sqrt(complexPoint[1].x()*complexPoint[1].x() + complexPoint[1].y()*complexPoint[1].y());

    }
    /// if c>=0 the number is entirely real
    /// use a + b;
    else
    {
        b = sqrt(c)/2.0;
        wo = a+b;
        //w1 = a-b;
        // cannot cube root a negative wo
        if(wo<0)
        {
            wo *=-1.0;
        }
        rootFactor = pow(wo,1.0/3.0);
        // calculate radius from wo (see above)
        newRadius = rootFactor - 1.0/(3.0*distortionFactor*rootFactor);
    }


    newPoint.setX(x);
    newPoint.setY(y);
    /// undistort the point
    newPoint = newPoint/distortedRadius*newRadius;
    /// move to screen coordinates
    newPoint.setX(newPoint.x()+0.5);
    newPoint.setY(newPoint.y()+0.5);
    return newPoint;


}

/// moves all the shapes based on change in motor movement
void AMShapeDataSet::motorMovement(double x, double y, double z, double r)
{
    QVector3D rotationalOffset = QVector3D(-0.5,0,-0.35);
    QVector3D newPosition(x,y,z);
    QVector3D shift = newPosition - motorCoordinate_;
//    shift *= -0.1;
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
QVector<QVector3D> AMShapeDataSet::rotateShape(AMShapeData shape)
{
    if(shape.rotation() != 0) shape = applyRotation(shape);
    if(shape.tilt() != 0) shape = applyTilt(shape);
    if(shape.yAxisRotation() != 0) shape = applyYAxisRotation(shape);
    QVector<QVector3D> returnShape;
    for(int i = 0; i < shape.count(); i++)
    {
        returnShape<<shape.coordinate(i);
    }
    return returnShape;
}

/// transforms a coordinate point for display on the screen
QPointF AMShapeDataSet::coordinateTransform(QPointF coordinate)
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

/// takes a 2D point on screen, as well as its depth, and transforms it into a 3D coordinate
QVector3D AMShapeDataSet::transform2Dto3D(QPointF point, double depth)
{
    if(useCameraMatrix_ && calibrationRun_)
    {
        return transform2Dto3DMatrix(point,depth);
    }
    /// Use the position of the item on screen, with knowledge of the
    /// camera position to determine its xyz coordinate.
    /// first convert to a point in an ideal plane, then shift and rotate to match camera
    /// see 3D to 2D for more detailed explanation
    QVector3D coordinate;
    QVector3D shift = cameraModel_->cameraPosition();
    QVector3D actualCameraCenter = cameraModel_->cameraCentre();
    double focalLength = cameraModel_->cameraFocalLength();
    double fieldOfView = cameraModel_->cameraFOV();
    double aspectRatio = scaledSize().height()/scaledSize().width();
    double pixelAspectRatio = cameraModel_->pixelAspectRatio();
    double fov = 2*tan(fieldOfView)*focalLength;
    double theta;
    double oldRotation;
    point -= cameraModel_->imageCentre();

    // find the coordinate corresponding to the point
    coordinate.setX(-1*((point.x()-0.5)*fov*depth/focalLength));
    coordinate.setY(-1*((point.y()-0.5)*fov*aspectRatio*pixelAspectRatio*depth/focalLength));
    coordinate.setZ(depth);
    /// coordinate is the point in for camera at (0,0,0) looking down the z-axis
    /// must now shift and rotate


    // rotation to keep the 0 rotation view consistent
    double naturalRotation = 0;
    if(shift.z() - actualCameraCenter.z() < 0)
    {
       naturalRotation = -1*M_PI;
    }
    else
    {
        double naturalX = shift.x() - actualCameraCenter.x();
        double naturalY = shift.y() - actualCameraCenter.y();
        if(naturalX != 0)
        {
            naturalRotation = -1*(M_PI - 2*atan(naturalY/naturalX));
        }
        else if(naturalY == 0 && naturalX == 0)
        {
            naturalRotation = M_PI;
        }
        else
        {
            naturalRotation = 0;
        }
    }



    // undo the rotation
    if((cameraModel_->cameraRotation() != 0 || naturalRotation != 0)&& (coordinate.x() != 0 || coordinate.y() != 0))
    {
        double z = coordinate.z();
        coordinate.setZ(0);
        double radius = coordinate.length();
        oldRotation = acos(coordinate.x()/radius);
        if(coordinate.y() < 0) oldRotation*= -1;
        double rotation = cameraModel_->cameraRotation()*-1;
        double newRotation = oldRotation+rotation+naturalRotation;
        coordinate.setX(radius*cos(newRotation));
        coordinate.setY(radius*sin(newRotation));
        coordinate.setZ(z);
    }

    // build the orthogonal components, z, u, v
    QVector3D zHat = QVector3D(0,0,1);
    QVector3D uHat;
    QVector3D shiftedCameraCenter = actualCameraCenter - shift;
    QVector3D idealCameraCenter = shiftedCameraCenter.length()*zHat;
    QVector3D vHat = QVector3D::normal(idealCameraCenter,shiftedCameraCenter);

    if(vHat.isNull())
    {
//        shiftedCameraCenter += QVector3D(0,0.001,0);
//        vHat = QVector3D::normal(idealCameraCenter,shiftedCameraCenter);
        vHat = QVector3D(0,1,0);
        if(vHat.isNull()) qDebug()<<"still a null vHat...";
    }

    // perform the rotation
    uHat = QVector3D::normal(vHat,zHat);
    // find the angle of rotation
    theta = -1*acos((dot(idealCameraCenter,shiftedCameraCenter))/(shiftedCameraCenter.lengthSquared()));
    double coordinateLength = coordinate.length();
    // find the coordinate's current angle
    double cosAlpha = dot(uHat,coordinate)/coordinateLength;
    double sinAlpha = dot(zHat,coordinate)/coordinateLength;

    // find the resultant components
    double uLength = coordinateLength*(cosAlpha*cos(theta)-sinAlpha*sin(theta));
    double vLength = dot(coordinate,vHat);
    double zLength = coordinateLength*(sinAlpha*cos(theta)+cosAlpha*sin(theta));

    // create the new vector
    QVector3D newPosition = uLength*uHat + vLength*vHat + zLength*zHat;
    // shift
    QVector3D finalPosition = newPosition + shift;

    return finalPosition;
}

/// performs the 2D to 3D transform using the precomputed camera matrix
QVector3D AMShapeDataSet::transform2Dto3DMatrix(QPointF point, double depth)
{

    /// \todo - not fully using the matrix?
    point = point - QPointF(0.5,0.5);
    MatrixXd matrixPoint (3,1);
    matrixPoint<<point.x(),point.y(),1;
    MatrixXd matrixP = cameraModel_->cameraMatrixToMatrix();

    MatrixXd coordinate = matrixP.colPivHouseholderQr().solve(matrixPoint);
    coordinate /= coordinate(3,0);
    QVector3D location = QVector3D(coordinate(0,0),coordinate(1,0),coordinate(2,0));
    QVector3D worldDirection = location - cameraModel_->cameraPosition();
    worldDirection.normalize();
    QVector3D centre = cameraModel_->cameraCentre()-cameraModel_->cameraPosition();
    worldDirection *= depth/dot(worldDirection,centre);
    worldDirection += cameraModel_->cameraPosition();

    return worldDirection;

}

/// takes a 3D coordinate and changes it to a 2D location on screen
QPointF AMShapeDataSet::transform3Dto2D(QVector3D coordinate)
{
    if(useCameraMatrix_ && calibrationRun_)
    {
        return transform3Dto2DMatrix(coordinate);
    }

    /// Shifts and rotates the entire scene so that it is looking down the z-axis
    /// then converts that to screen coordinates by taking the proportion of
    /// focal length over the coordinate's distance from the camera.
    /// Shift the coordinate by the same amount as the camera would be shifted to center it
    // coordinate unrotated position = coordinate - cameraPosition
    /// rotate the scene so that the camera center is looking down the z-axis
    // angle of rotation: cos(angle) = v1.v2/|v1||v2|
    /// find the new location of the coordinate, by rotating it
    // phi = theta + alpha
    // theta = angle of rotation (above)
    // alpha = original angle with u axis (perpindicular to z, along plane of rotation)
    // cos(alpha) = u^.shiftedCoordinate/|shiftedCoordinate|
    // same as calculation for angle of rotation, but  u^ is normalized
    /// rather than calculating alpha directly, just calculate sin(alpha) and cos(alpha)
    // sin(alpha) = z^.shiftedCoordinate/|shiftedCoordinate|
    /// then calculate each component
    // uComponent= |shiftedCoordinate|*cos(phi)
    // cos(phi) = cos(theta)cos(alpha)-sin(theta)sin(alpha)
    // vComponent = shiftedCoordinate.v^
    // zComponet = |shiftedCoordinate|*sin(phi)
    // sin(phi) = cos(theta)sin(alpha)+cos(alpha)sin(theta)
    /// once each component has been calculated, add them up
    // vector = uComponet*u^ + vComponent*v^ + zComponent*z^


    double rotation = cameraModel_->cameraRotation();
    double focalLength = cameraModel_->cameraFocalLength();
    QVector3D cameraPosition = cameraModel_->cameraPosition();
    QVector3D cameraCenter = cameraModel_->cameraCentre();
    QVector3D newPosition;
    double oldRotation;
    double newRotation;
    double radius;
    double zLength;
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
//        cameraRotation += QVector3D(0,0.001,0);
//        vHat = QVector3D::normal(zHat,cameraRotation);
        vHat = QVector3D(0,-1,0);
        if(vHat.isNull()) qDebug()<<"Vhat still null, in 3-2";
    }

    // rotate the camera
    double cameraCenterLength = cameraRotation.length();
    // the rotated center looks down the positive z axis
    QVector3D newCameraCenter = -1*cameraCenterLength*zHat;
    // find the angle of rotation
    // angle of rotation: cos(angle) = v1.v2/|v1||v2|
    double theta = acos((dot(newCameraCenter,cameraRotation))/((newCameraCenter.length())*(cameraCenterLength)));
    // find the third orthogonal component (along the plane of rotation)
    QVector3D uHat = QVector3D::normal(zHat,vHat);
    double shiftedLength = shiftedCoordinate.length();
    // find the angle of the coordinate before rotation
    double cosAlpha = dot(uHat,shiftedCoordinate)/shiftedLength;
    double sinAlpha = dot(zHat,shiftedCoordinate)/shiftedLength;

    // get the components along the u,v, and z axes
    double uLength = shiftedLength*(cosAlpha*cos(theta)-sinAlpha*sin(theta));
    double vLength = dot(shiftedCoordinate,vHat);
    zLength = shiftedLength*(sinAlpha*cos(theta)+cosAlpha*sin(theta));

    // add the three orthogonal components together
    newPosition = uLength*uHat + vLength*vHat + zLength*zHat;



    double adjustRotation = 0;
    if(cameraPosition.z()-cameraCenter.z()<0)
    {
        adjustRotation = M_PI;
    }
    else
    {
        double x = cameraPosition.x()-cameraCenter.x();
        double y = cameraPosition.y()-cameraCenter.y();
        if(x != 0)
        {
            adjustRotation = M_PI - 2*atan(y/x);
        }
        else if(y == 0 && x==0)
        {
            adjustRotation = M_PI;
        }
        else
        {
            adjustRotation = 0;
        }

    }



    // apply rotation
    if((rotation != 0 || adjustRotation != 0) && (newPosition.x() != 0 || newPosition.y() != 0))
    {
        double z = newPosition.z();
        newPosition.setZ(0);
        radius = newPosition.length();
        oldRotation = acos(newPosition.x()/radius);
        if(newPosition.y()<0) oldRotation *= -1;
        newRotation = oldRotation+rotation+adjustRotation;
        newPosition.setX(radius*cos(newRotation));
        newPosition.setY(radius*sin(newRotation));
        newPosition.setZ(z);
    }

    // stops objects behind from displaying
    if(newPosition.z() < 0)
    {
        zLength*=0.0001;
    }

    double fieldOfView = cameraModel_->cameraFOV();
    double aspectRatio = (scaledSize().height())/(scaledSize().width());
    double pixelAspectRatio = cameraModel_->pixelAspectRatio();
    double fov = 2*tan(fieldOfView)*focalLength;


    // find the scaled point location, and shift to the center of the screen
    position = (focalLength/zLength)/fov*newPosition.toPointF();
    position.setY(position.y()/aspectRatio/pixelAspectRatio);
    position += QPointF(0.5,0.5);
    position += cameraModel_->imageCentre();
    return position;
}

/// does the 3D to 2D transform using the precomputed camera matrix
QPointF AMShapeDataSet::transform3Dto2DMatrix(QVector3D coordinate)
{
    MatrixXd worldCoordinate(4,1);
    worldCoordinate<<coordinate.x(),coordinate.y(), coordinate.z(),1;
    MatrixXd screenPosition = cameraModel_->cameraMatrixToMatrix()*worldCoordinate;
    screenPosition /= screenPosition(2,0);
    return QPointF(screenPosition(0,0)+0.5,screenPosition(1,0)+0.5);
}


///// takes a vector and finds how it appears
//QPointF AMShapeDataSet::transformVector(QPointF vector, QVector3D coordinate)
//{
//    double focalLength = cameraModel_->cameraFocalLength();
//    double distance = depth(coordinate);
//    double scale = focalLength/distance;
//    QPointF newVector = vector*scale;
//    return newVector;
//}

///// based on the appearance of a vector, finds the actual vector
//QPointF AMShapeDataSet::inverseVectorTransform(QPointF vector, QVector3D coordinate)
//{
//    double focalLength = cameraModel_->cameraFocalLength();
//    QPointF newVector = vector*depth(coordinate)/focalLength;
//    return newVector;
//}


QPolygonF AMShapeDataSet::screenShape(QPolygonF shape)
{
    QPolygonF newShape;
    for(int i = 0; i < shape.count(); i++)
    {
        newShape<<coordinateTransform(shape.at(i));
    }

    return newShape;
}

/// constructs a rectangle given the desired top and bottom corners
QPolygonF AMShapeDataSet::constructRectangle(QPointF topLeft, QPointF bottomRight)
{
    QPolygonF polygon;
    QPointF topRight(bottomRight.x(),topLeft.y());
    QPointF bottomLeft(topLeft.x(),bottomRight.y());
    polygon<<topLeft<<topRight<<bottomRight<<bottomLeft<<topLeft;
    return polygon;

}

/// returns the size of a rectangle with the given points
QSizeF AMShapeDataSet::size(QPointF topLeft, QPointF bottomRight)
{
    QSizeF size;
    QPointF difference = bottomRight - topLeft;
    size.setHeight(difference.y());
    size.setWidth(difference.x());
    return size;
}

/// Checks if the shape contains the point
bool AMShapeDataSet::contains(QPointF position, int index)
{
    return shapeList_[index]->shape()->containsPoint(position, Qt::OddEvenFill);
}

/// finds the center of the given polygon
QPointF AMShapeDataSet::findCenter(QPolygonF polygon)
{
    double xMid = (polygon[TOPLEFT].x()+polygon[TOPRIGHT].x())/2.0;
    double yMid = (polygon[TOPLEFT].y()+polygon[BOTTOMLEFT].y())/2.0;
    return QPointF(xMid,yMid);
}

/// finds the depth of the given coordinate, returns the length of depthVector
double AMShapeDataSet::depth(QVector3D coordinate)
{
    QVector3D depth = depthVector(coordinate);
    return depth.length();
}

/** returns the depth vector
/// the depth vector gives the distance from the center of the camera to an object the camera is centered on.
/// depth vector is the same for all coordinates on the plane that is normal to depth vector, with the same distance */
QVector3D AMShapeDataSet::depthVector(QVector3D coordinate)
{
    QVector3D cameraCenterClose = transform2Dto3D(QPointF(0.5,0.5),1);// actual camera center
    QVector3D cameraCenterFar = transform2Dto3D(QPointF(0.5,0.5),2);// a second vector in the direction of the center
    QVector3D cameraDirection = cameraCenterFar - cameraCenterClose;
    cameraDirection.normalize();
    QVector3D cameraToCoordinate = coordinate - cameraCenterClose;
    double length = dot(cameraToCoordinate, cameraDirection) + 1;
    QVector3D depth = length * cameraDirection;
    return depth;
}

/// finds the intersection between the beam and the shape with given index
QVector<QVector3D> AMShapeDataSet::findIntersectionShape(int index)
{
    /// First find the shape on the same plane
    QVector<QVector3D> rotatedShape = rotateShape(*shapeList_[index]);
    QVector<QVector3D> shape;
    QVector3D topLeft = rotatedShape.at(TOPLEFT);
    QVector3D hHat = getHeightNormal(*shapeList_[index]);
    QVector3D wHat = getWidthNormal(*shapeList_[index]);
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
//    beamShape<<beamShape.first();
//    originalShape<<originalShape.first();

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
QPolygonF AMShapeDataSet::intersectionScreenShape(QVector<QVector3D> shape3D)
{

    AMShapeData newShape;
    newShape.setCoordinateShape(shape3D,shape3D.count());
    newShape.setRotation(0);
    newShape.setTilt(0);
    newShape.setYAxisRotation(0);


    QPolygonF shape;
    shape = subShape(newShape);

    shape = screenShape(shape);

    return shape;

}

/// finds the coordinate of the center of the shape
QVector3D AMShapeDataSet::centerCoordinate(int index)
{
    return shapeList_[index]->centerCoordinate();
}


/// compares to doubles to find inequality within the tolerance level as a percent of the difference
bool AMShapeDataSet::notEqual(double a, double b, double tolerance)
{
    double percent = fabs((a-b)/a);
    return percent>tolerance;
}

/// finds if a number is within a tolerance level around zero
double AMShapeDataSet::nearZero(double a,double tolerance)
{
    if(fabs(a)<tolerance)
    {
        return 0;
    }
    else return a;
}

/// finds the relative error in two numbers
double AMShapeDataSet::absError(double a, double b, double tolerance)
{
    a = nearZero(a,tolerance);
    b = nearZero(b,tolerance);
    if(a == 0 && b == 0)
    {
        return 0;
    }
    else if (a == 0)
    {
        return fabs((b - a)/b);
    }
    else
    {
        return fabs((a-b)/a);
    }
}




/// finds the camera transform to produce the image; used to calibrate the camera
void AMShapeDataSet::getTransforms(QPointF points[], QVector3D coordinates[])
{
    // use least squares to find the matrix of parameters
    MatrixXd matrixP = directLinearTransform(coordinates,points);

    /// [B b] = P
    MatrixXd matrixB = matrixP.block(0,0,3,3);
    MatrixXd matrixSubB = matrixP.col(3);
    /// find the intrinsic parameters
    MatrixXd matrixA = intrinsicParameters(matrixB);
    /// find the extrinsic parameters
    MatrixXd matrixR = rotationParameters(matrixA,matrixB);
    MatrixXd matrixT = translationParameters(matrixA,matrixSubB);
    /// extrinsicMatrix = [R T]
    MatrixXd matrixExtrinsic(3,4);
    matrixExtrinsic<<matrixR,matrixT;

    /// zero the almost-zero terms
    for(int i = 0; i < matrixExtrinsic.rows(); i++)
    {
        for(int j = 0; j < matrixExtrinsic.cols(); j++)
        {
            matrixExtrinsic(i,j) = nearZero(matrixExtrinsic(i,j));
        }
    }

    MatrixXd coordinate [6];
    for(int i = 0; i<6; i++)
    {
        coordinate[i].resize(4,1);
        coordinate[i]<<coordinates[i].x(),coordinates[i].y(),coordinates[i].z(),1;
    }

    // set the camera matrix to A[R t]
    cameraModel_->setCameraMatrixFromMatrix(matrixA*matrixExtrinsic);
    MatrixXd point [6];
    for(int i= 0; i<6; i++)
    {
        point[i] = matrixA*matrixExtrinsic*coordinate[i];
        point[i] = point[i]/((point[i](2,0)));

    }



    MatrixXd coordZero(4,1);
    coordZero<<0,0,0,1;
    QVector3D rotationVector = QVector3D(matrixExtrinsic(2,0),matrixExtrinsic(2,1),matrixExtrinsic(2,2));
    double factorTwo = 1/rotationVector.length();

    matrixExtrinsic *= factorTwo;
    Matrix3d rotationMatrix = matrixExtrinsic.block(0,0,3,3);
    double sign = rotationMatrix.determinant();
    if(notEqual(sign,1) && notEqual(sign,-1))
    {
        qDebug()<<"determinant not close to 1, it is:"<<sign;
    }
    matrixExtrinsic *= sign;

    // the location of the camera is given by the negative of each component times the shift
    QVector3D cameraCoordinateShift;
    cameraCoordinateShift.setX(-1*((matrixExtrinsic(0,0)*matrixExtrinsic(0,3))+(matrixExtrinsic(1,0)*matrixExtrinsic(1,3))+(matrixExtrinsic(2,0)*matrixExtrinsic(2,3))));
    cameraCoordinateShift.setY(-1*((matrixExtrinsic(0,1)*matrixExtrinsic(0,3))+(matrixExtrinsic(1,1)*matrixExtrinsic(1,3))+(matrixExtrinsic(2,1)*matrixExtrinsic(2,3))));
    cameraCoordinateShift.setZ(-1*((matrixExtrinsic(0,2)*matrixExtrinsic(0,3))+(matrixExtrinsic(1,2)*matrixExtrinsic(1,3))+(matrixExtrinsic(2,2)*matrixExtrinsic(2,3))));



    MatrixXd manualCameraCenter;
    MatrixXd cameraCenterMatrix(3,1);
    cameraCenterMatrix<<0,0,1;
    manualCameraCenter = findWorldCoordinate(cameraCenterMatrix,matrixExtrinsic);

//    MatrixXd cameraCalculatedWorldCenter(4,1);
//    cameraCalculatedWorldCenter<<manualCameraCenter;
//    MatrixXd idealCenter = matrixExtrinsic*cameraCalculatedWorldCenter;
//    QVector3D coordinateIdealCenter = QVector3D(idealCenter(0,0),idealCenter(1,0),idealCenter(2,0));
//    qDebug()<<"The calculated camera center resolves to:"<<coordinateIdealCenter;
//    qDebug()<<"Should be: QVector3D(0,0,1)";


    double uO = matrixA(0,2);
    double vO = matrixA(1,2);
//    double skew = matrixA(0,1);

    double aX = matrixA(0,0);
    double aY = matrixA(1,1);

    double pixelAspectRatio = aX/aY;
    double focalLength = aX;

    QVector3D cameraPosition = cameraCoordinateShift;
    QVector3D cameraCenter = QVector3D(manualCameraCenter(0,0),manualCameraCenter(1,0),manualCameraCenter(2,0));


    // don't change the pixel aspect ratio
    pixelAspectRatio = cameraModel_->pixelAspectRatio();
    double rotation;
    QVector3D cameraDirection = cameraCenter - cameraPosition;
    // the rotation is
    // (xyz)/|xyz|*(y/|y|-1)/-2*pi-xyz
    double product = cameraDirection.x()*cameraDirection.y()*cameraDirection.z();
    double piFactor = (product/fabs(product))*(cameraDirection.y()/fabs(cameraDirection.y())-1)/-2;
    rotation = piFactor*M_PI-product;



    double fov = atan(1/(2*focalLength));
    cameraModel_->setCameraCentre(cameraCenter);
    cameraModel_->setCameraPosition(cameraPosition);
    cameraModel_->setCameraFocalLength(focalLength);
    cameraModel_->setCameraFOV(fov);
    cameraModel_->setPixelAspectRatio(pixelAspectRatio);
    cameraModel_->setImageCentre(QPointF(uO,vO));
    cameraModel_->setCameraRotation(rotation);

//    for(int i = 0; i<6; i++)
//    {
//        qDebug()<<coordinates[i]<<transform2Dto3D(points[i]+QPointF(0.5,0.5),depth(coordinates[i]))<<transform3Dto2D(coordinates[i])<<point[i](0,0)+0.5<<point[i](1,0)+0.5<<points[i]+QPointF(0.5,0.5);
//    }


}

/// returns the dot product of two vectors
double AMShapeDataSet::dot(QVector3D a, QVector3D b)
{
    double value = QVector3D::dotProduct(a,b);
    return value;
}

/// solves for the transform Matrix
MatrixXd AMShapeDataSet::directLinearTransform(QVector3D coordinate[], QPointF screenPosition[])
{
    /// solves for Matrix P where x = PX
    /// x is given by screenPosition
    /// X is given by coordinate

    /// construct the 12x11 x matrix from coordinate and screen position
    MatrixXd matrix = constructMatrix(coordinate,screenPosition);
    MatrixXd rhs(matrix.rows(),1);
    rhs.setZero(matrix.rows(),1);
    JacobiSVD<MatrixXd> svd(matrix,ComputeThinV|ComputeThinU);
    svd.compute(matrix,ComputeThinU|ComputeThinV);


    MatrixXd matrixV = svd.matrixV();
    MatrixXd solution = matrixV.col(matrixV.cols()-1);
    /// solution is given by the right eigenvector corresponding to the smallest eigen value
    /// (SVD decomposition)
    /// This means the solution is the last column of the V matrix
//    MatrixXd solutionTwo = svd.solve(rhs);
//    if(solutionTwo != rhs)
//    {
//        qDebug()<<"Solved successfully";
//        return solutionTwo;
//    }





    /// change solution into a 3x4 matrix
    /// resize goes column by column, and places column by column
    /// so have to resize to the transpose, then transpose to get the
    /// desired matrix
    solution.resize(4,3);
    solution.transposeInPlace();


    return solution;





}

/// creates the x matrix for the dlt, each row is stored sequentially in the vector
MatrixXd AMShapeDataSet::constructMatrix(QVector3D coordinate[], QPointF screenposition[])
{

    MatrixXd matrix(12,12);
    for(int i = 0; i<6; i++)
    {
         /// row one
        matrix(2*i,0) = coordinate[i].x();
        matrix(2*i,1) = coordinate[i].y();
        matrix(2*i,2) = coordinate[i].z();
        matrix(2*i,3) = 1;

        matrix(2*i,4) = 0;
        matrix(2*i,5) = 0;
        matrix(2*i,6) = 0;
        matrix(2*i,7) = 0;

        matrix(2*i,8) = -1*screenposition[i].x()*coordinate[i].x();
        matrix(2*i,9) = -1*screenposition[i].x()*coordinate[i].y();
        matrix(2*i,10) = -1*screenposition[i].x()*coordinate[i].z();
        matrix(2*i,11) = -1*screenposition[i].x();

        /// row two

        matrix(2*i + 1,0) = 0;
        matrix(2*i + 1,1) = 0;
        matrix(2*i + 1,2) = 0;
        matrix(2*i + 1,3) = 0;

        matrix(2*i + 1,4) = coordinate[i].x();
        matrix(2*i + 1,5) = coordinate[i].y();
        matrix(2*i + 1,6) = coordinate[i].z();
        matrix(2*i + 1,7) = 1;

        matrix(2*i + 1,8) = -1*screenposition[i].y()*coordinate[i].x();
        matrix(2*i + 1,9) = -1*screenposition[i].y()*coordinate[i].y();
        matrix(2*i + 1,10) = -1*screenposition[i].y()*coordinate[i].z();
        matrix(2*i + 1,11) = -1*screenposition[i].y();
    }
    return matrix;

}



/// Finds the matrix of intrinsic parameters
MatrixXd AMShapeDataSet::intrinsicParameters(MatrixXd matrixB)
{
    /// matrix B is equal to A*R
    /// since R is orthogonal, R*R^T = I
    /// so BB^T == kAA^T
    /// where k is some constant
    /// AA^T(3,3) == 1, so BB^T(3,3) == k
    MatrixXd matrixK = matrixB*matrixB.transpose();
    matrixK = matrixK/matrixK(2,2);
    double alphaX, alphaY, uO, vO, gamma;
    /// the five intrinsic parameters.
    /// ideally uO,vO and gamma should be nearZero
    uO = matrixK(0,2);
    vO = matrixK(1,2);
    alphaY = sqrt(matrixK(1,1) - pow(vO,2.0));
    gamma = (matrixK(0,1)-uO*vO)/alphaY;
    alphaX = sqrt(matrixK(0,0) - pow(uO,2.0) - pow(gamma,2.0));
    MatrixXd matrixA(3,3);
    matrixA<<alphaX, gamma,uO,
                  0,alphaY,vO,
                  0,     0, 1;

    return matrixA;


}

/// Finds the rotation matrix
MatrixXd AMShapeDataSet::rotationParameters(MatrixXd matrixA, MatrixXd matrixB)
{
    MatrixXd matrixR = matrixA.inverse()*matrixB;
    return matrixR;
}

/// finds the translation
MatrixXd AMShapeDataSet::translationParameters(MatrixXd matrixA, MatrixXd matrixSubB)
{
    MatrixXd matrixT = matrixA.inverse()*matrixSubB;
    return matrixT;
}

/// solves a camera coordinate for the corresponding world coordinate
MatrixXd AMShapeDataSet::findWorldCoordinate(MatrixXd matrix, MatrixXd extrinsicMatrix)
{
    MatrixXd fullMatrix(4,4);
    // add the constraint that the solution is xyz1
    fullMatrix<<extrinsicMatrix,0,0,0,1;
    JacobiSVD<MatrixXd> solver(fullMatrix);
    solver.compute(fullMatrix,ComputeThinU|ComputeThinV);
    MatrixXd fullSolution(4,1);
    fullSolution<<matrix,1;
    MatrixXd solution = solver.solve(fullSolution);
    //divide by the fourth term, ensure it is 1
    solution /= solution(3,0);
    return solution;
}

QVector3D AMShapeDataSet::getHeightNormal(AMShapeData shape)
{
    QVector<QVector3D> rotatedShape = rotateShape(shape);
    QVector3D heightVector = rotatedShape.at(shape.count()-1) - rotatedShape.at(0);
    return heightVector.normalized();
}

QVector3D AMShapeDataSet::getWidthNormal(AMShapeData shape)
{
    QVector<QVector3D> rotatedShape = rotateShape(shape);
    QVector3D widthVector = rotatedShape.at(1) - rotatedShape.at(0);
    return widthVector.normalized();
}

/// gets the normal vector pointing out of a shape
QVector3D AMShapeDataSet::getNormal(QVector3D heightVector, QVector3D widthVector)
{
    return QVector3D::normal(heightVector,widthVector);
}

/// overload of getNormal, takes an AMShapeData
QVector3D AMShapeDataSet::getNormal(AMShapeData shape)
{
    return getNormal(getHeightNormal(shape),getWidthNormal(shape));
}

QVector3D AMShapeDataSet::getPointOnShape(QPointF position, QVector3D nHat)
{
    /// figure out line
    /// pick a depth to use, doesn't really matter what it is
    double depth = cameraModel_->cameraFocalLength();
    QVector3D l0 = transform2Dto3D(position,depth);
    QVector3D lVector = transform2Dto3D(position,2*depth) - l0;
    QVector<QVector3D> rotatedShape = rotateShape(drawOnShape_);
    double numerator = dot((rotatedShape.at(TOPLEFT) - l0), nHat);
    double denominator = dot(lVector,nHat);
    double distance = 0;
    if(denominator == 0)
    {
        if(numerator == 0)
        {
            qDebug()<<"AMShapeDataSet::getPointOnShape - Line is on plane";
        }
        else
        {
            qDebug()<<"AMShapeDataSet::getPointOnShape - Line is parallel to plane";
        }
    }
    else
    {
        distance = numerator/denominator;
    }
    QVector3D pointOnShape = l0 + distance*lVector;
    return pointOnShape;
}

QVector3D AMShapeDataSet::downVector()
{
    QVector3D top = transform2Dto3D(QPointF(0.5,0),1);
    QVector3D bottom = transform2Dto3D(QPointF(0.5,1),1);
    QVector3D downVector = bottom - top;
    return downVector;
}

QVector3D AMShapeDataSet::rightVector()
{
    QVector3D left = transform2Dto3D(QPointF(0,0.5),1);
    QVector3D right = transform2Dto3D(QPointF(1,0.5),1);
    QVector3D rightVector = right - left;
    return rightVector;
}




