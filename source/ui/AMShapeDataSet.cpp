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

int AMShapeDataSet::shapeListLength()
{
    return index_;
}

int AMShapeDataSet::currentIndex()
{
    return current_;
}

AMCameraConfiguration *AMShapeDataSet::cameraConfiguration()
{
    return cameraModel_;
}

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

QVector<QPolygonF> AMShapeDataSet::intersections()
{
    return intersections_;
}

double AMShapeDataSet::crosshairX()
{
    return crosshair_.x();
}

double AMShapeDataSet::crosshairY()
{
    return crosshair_.y();
}

QPointF AMShapeDataSet::crosshair()
{
    return crosshair_;
}

bool AMShapeDataSet::crosshairLocked()
{
    return crosshairLocked_;
}

QSizeF AMShapeDataSet::viewSize()
{
    return viewSize_;
}

QSizeF AMShapeDataSet::scaledSize()
{
    return scaledSize_;
}

/// return the current name
QString AMShapeDataSet::currentName()
{
    if(isValid(current_)) return shapeList_[current_].name();
    else return "";
}

/// return the current info
QString AMShapeDataSet::currentInfo()
{
    if(isValid(current_)) return shapeList_[current_].otherData();
    else return "";
}

/// returns the rotation of the given index
double AMShapeDataSet::rotation(int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    return shapeList_[index].rotation();
}

/// returns the tilt for the given index
double AMShapeDataSet::tilt(int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    return shapeList_[index].tilt();
}

AMShapeData *AMShapeDataSet::currentShape(int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    return &shapeList_[index];
}

/// returns the shape in screen coordinates
QPolygonF AMShapeDataSet::shape(int index)
{
    QPolygonF shape = *shapeList_[index].shape();

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
    cameraModel_ = model;
    updateAllShapes();
}

void AMShapeDataSet::setBeamModel(AMBeamConfiguration *model)
{
    beamModel_ = model;
}

void AMShapeDataSet::setMotorCoordinate(double x, double y, double z, double r)
{
    motorMovement(x,y,z,r);
    motorCoordinate_.setX(x);
    motorCoordinate_.setY(y);
    motorCoordinate_.setZ(z);

    motorRotation_ = r;
}

void AMShapeDataSet::toggleDistortion()
{
    distortion_ = !distortion_;
    updateAllShapes();
}

void AMShapeDataSet::setCrosshairX(double x)
{
    crosshair_.setX(x);
}

void AMShapeDataSet::setCrosshairY(double y)
{
    crosshair_.setY(y);
}

void AMShapeDataSet::setCrosshair(QPointF crosshair)
{
    crosshair_ = crosshair;
}

void AMShapeDataSet::setCrosshairLocked(bool locked)
{
    crosshairLocked_ = locked;
}

void AMShapeDataSet::setViewSize(QSizeF viewSize)
{
    viewSize_ = viewSize;
}

void AMShapeDataSet::setScaledSize(QSizeF scaledSize)
{
    scaledSize_ = scaledSize;
}

/// set the current name
void AMShapeDataSet::setCurrentName(QString name)
{
    shapeList_[current_].setName(name);
}

/// set th current info
void AMShapeDataSet::setCurrentInfo(QString info)
{
    shapeList_[current_].setOtherData(info);
}

/// sets the rotation for the given index
void AMShapeDataSet::setRotation(double rotation, int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    shapeList_[index].setRotation(rotation);
}

/// sets the tilt for the given index
void AMShapeDataSet::setTilt(double tilt, int index)
{
    if(-1 == index)
    {
        index = current_;
    }
    shapeList_[index].setTilt(tilt);
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
        qDebug()<<"Invalid index"<<QString::number(index);
        return false;
    }
}

bool AMShapeDataSet::isBackwards(int index)
{
    if(index == -1) index = current_;
    return shapeList_[index].backwards();
}

void AMShapeDataSet::findCamera(QPointF points [], QVector3D coordinates[], QVector3D cameraCenter, double fieldOfView)
{
    qDebug()<<"AMShapeDataSet::findCamera - started";
    /// given the four (could be cut down to three?) points on screen and their corresponding positions, find out the camera parameters...
    /// zHat is (0,0,1)
    /// given the point we know it's coordinates in some arbitrary coordinate system, or, given the coordinates,
    /// we know the point position for some arbitrary camera...
    /// point one's x position is given by: [sqrt(a+d^2)cos(r+aR)+dsin(r+aR)]/m2tanV
    /// where a = [sC.(IuH-JzH)]^2 + [sC.vH]^2 + [sC.(JuH+IzH)]^2
        /// sC is coordinateOne - P
        /// P is the position of the camera (ie the desired cameraModel_->cameraPostion_;
        /// I is (Cz-Pz)/|cR|
        /// C is the center position of the camera (desired cameraModel_->cameraCenter_;
        /// |cR| is sqrt[(Cx-Px)^2 + (Cy-Py)^2 + (Cz-Pz)^2]
        /// J is sqrt[1-(I)^2] = sqrt[(Cx-Px)^2 + (Cy-Py)^2]/|cR|
        /// uH = zH x vH
        /// vH = zH x cR
        /// cR = camerCenter - cameraPosition
    /// d is [(sC.(IuH-JzH))(uH.xH)] + [(sC.vH)(vH.xH)] (zH.xH == 0, so no zH term)
    /// xH = (1,0,0)
    /// r is the rotation
    /// ar is the adjustedRotation which is:
        /// ar = M_PI if z<0 or x and y are 0
        /// ar = M_PI - 2atan(y/x) if x is not zero, z >= 0
        /// ar = 0 if x is 0 and z >= 0
        /// x, y , z are given by P-C
    /// m is sC.(JuH+IzH)
    /// V is the field of view
    /// we need to find P, C, V, R, A, and F
    /// A is the pixel Aspect Ratio
    /// F is the focal Length
    /// the equation above uses PCVR -> 8 unknowns
    /// similarly pointOne's y position is given by [dcos(r+aR)-sqrt(a+d^2)sin(r+aR)]/m2tanVasRA
    /// asR is the screen aspect ratio
    /// one additional variable is used -> 9 unknowns

    /// additionally
    /// vH = zH x cR -> (Cy-Py/|vH|,Px-Cx/|vH|,0)
    /// |vH| = sqrt[(Cy-Py)^2 + (Px-Cx)^2]
    /// as well,
    /// uH = zH x vH = (vHy, -vHx, 0)

    /// d is a function of (C,P, coordinate)
    /// a is a funtion of (C, P, coordinate)
    /// aR is a function of (C, P)
    /// m is a funtion of (C, P, coordinate)

    /// we want to find  C, P, V, R, A, and F such that
    /// transform3Dto2D(coordinateOne) == pointOne
    /// and transform3Dto2D(coordinateOne + shift) == pointTwo

    /// to find the relationship between pointOne and  point Two we take
    /// the above equations and substitute in coordinateOne + shift instead of coordinateOne
    /// the result is
    /// pointTwo.x = [sqrt(a'+d'^2)cos(r+aR)+d'sin(r+aR)]/m'2tanV
    /// a' = a(sC) + a0 + a(shift)
    /// a0 = 2*[(sC.(IuH-JzH))(shift.(IuH-JzH)+(sC.vH)(shift.vH)+(sC.(JuH+IzH))(shift.(JuH+IzH))]
    /// note that shift takes the place of coordinateOne - P, not just coordinateOne
    /// d' = d(sC) + d(shift)
    /// m' = m(sC) + m(shift)
    /// JuH + IzH == G == C-P/|C-P| except negate z component...

    /// start with finding a candidate coordinate in the ideal coordinate system (facing down the z axis, with center at origin)
    /// given the two screen points and the shift between them, we will find two coordinates that are separated by a scalar*shift
    /// that scalar will be related to field of view and depth

//    double focalLength = 1;
//    double aspectRatio = scaledSize().width()/scaledSize().height();





    QPointF point[6];
    for(int i = 0; i < 6; i++)
    {
        point[i].setX((points[i].x()-0.5));
        point[i].setY((points[i].y()-0.5));
    }
    QVector3D fullCoordinates [6];
    fullCoordinates[0] = coordinates[0];
    for(int i = 1; i < 6; i++)
    {
        fullCoordinates[i] = coordinates[0]+coordinates[i];
    }

    getTransforms(point,fullCoordinates);



//    QVector3D normalizedCoordinate;
//    normalizedCoordinate.setX((points[0].x()-0.5)*fieldofView/focalLength);
//    normalizedCoordinate.setY((points[0].y()-0.5)*fieldofView*aspectRatio/focalLength);
//    normalizedCoordinate.setZ(1);
//    normalizedCoordinate.normalize();
//    ///  here we have a vector representing coordinateOne, must find the relative coordinateTwo representation

//    QVector3D normalizedCoordinateTwo;
//    normalizedCoordinateTwo.setX((points[0].x()-0.5)*fieldofView/focalLength);
//    normalizedCoordinateTwo.setY((points[0].y()-0.5)*fieldofView*aspectRatio/focalLength);
//    normalizedCoordinateTwo.setZ(1);
//    normalizedCoordinateTwo.normalize();

//    QVector3D normalizedCoordinateThree;
//    normalizedCoordinateThree.setX((pointThree.x()-0.5)*fieldofView/focalLength);
//    normalizedCoordinateThree.setY((pointThree.y()-0.5)*fieldofView*aspectRatio/focalLength);
//    normalizedCoordinateThree.setZ(1);
//    normalizedCoordinateThree.normalize();

//    QVector3D normalizedCoordinateFour;
//    normalizedCoordinateFour.setX((pointFour.x()-0.5)*fieldofView/focalLength);
//    normalizedCoordinateFour.setY((pointFour.y()-0.5)*fieldofView*aspectRatio/focalLength);
//    normalizedCoordinateFour.setZ(1);
//    normalizedCoordinateFour.normalize();

//    qDebug()<<"Direction of coordOne"<<normalizedCoordinate;
//    qDebug()<<"Direction of coordTwo"<<normalizedCoordinateTwo;
//    qDebug()<<"Direction of coordThree"<<normalizedCoordinateThree;
//    qDebug()<<"Direction of coordFour"<<normalizedCoordinateFour;

//    /// The four vectors represent four rays into the representational coordinate system
//    ///

////    QPointF apparentLengthOne = pointTwo-pointOne;
////    QPointF apparentLengthTwo = pointThree-pointOne;

////    double screenLengthOne = (pow(apparentLengthOne.x(),2.0)+pow(apparentLengthOne.y(),2.0));
////    double screenLengthTwo = (pow(apparentLengthTwo.x(),2.0)+pow(apparentLengthTwo.y(),2.0));

//    /// must find the scale between the two vectors, which will give the angle between them

//    double shiftOneLength = shift.length();
//    double shiftTwoLength = shiftTwo.length();
//    double shiftThreeLength = shiftThree.length();
////    double coordinateLength = coordinateOne.length();

////    double shiftRatio = shiftOneLength/shiftTwoLength;

////    double screenRatio = screenLengthOne/screenLengthTwo;

//    /// if the two ratios are not close, it means that they are different angles from the normal.
//    /// if they are close to being the same they are at the same angle, but not necessarily in the same direction


//    double shiftAngleOneTwo = acos(QVector3D::dotProduct(shift,shiftTwo)/(shiftOneLength*shiftTwoLength));
//    double shiftAngleOneThree = acos(QVector3D::dotProduct(shift,shiftThree)/(shiftOneLength*shiftThreeLength));
//    double shiftAngleTwoThree = acos(QVector3D::dotProduct(shiftThree,shiftTwo)/(shiftThreeLength*shiftTwoLength));
////    qDebug()<<"ShiftAngle"<<shiftAngle;





//    /// to find d, which is the length of the vector of coordinate One
//    /// d = +-beta-+gamma/[(a.c)(1-(a.c)^2)/{(a.c)^2-cos(Theta2)^2}]
//    /// however, beta and gamma are both functions of d, so this will be computed iteratively


//    qDebug()<<"Finding the coordinateDistance";
////    coordinateDistance = findCoordinateDistance(coordinateDistance, normalizedCoordinate, normalizedCoordinateTwo, normalizedCoordinateThree,  shiftOneLength, shiftTwoLength, shiftOneAngle, shiftTwoAngle);


//    /// Here is what is actually happening, ignore most of the stuff above
//    /// the three shifts have the same angle between them no matter where they are
//    /// as well, when creating representational coordinates , the distances between the points must be scaled to the lengths of the shifts
//    /// so that gives six equations and five variables
//    /// three equations for solving the point where a ray intersects a sphere of radius shiftlength*scalingFactor
//    /// three equations for equality between angles (angle12,23,and 13)
//    /// however, this set of equations does not have one single solution, it may have many solutions, depending on where the
//    /// center coordinate is placed along its ray.
//    /// make an initial guess at the distance along the central ray, as well as the distance along one of the shifts.
//    double initialTDistance = 0.1;
//    double initialDDistance = 0.1;
//    QList<double> parameters = getCoordinateSystem(initialTDistance,initialDDistance,shiftOneLength,shiftTwoLength,shiftThreeLength,normalizedCoordinate,normalizedCoordinateTwo,normalizedCoordinateThree,normalizedCoordinateFour,shiftAngleOneTwo,shiftAngleOneThree,shiftAngleTwoThree);
//    /// the list returns d,t1,t2,t3,r
//    /// d is the length along the ray going through pointOne, t1-t3 are the same for points2-4
//    /// and r is the factor by which the vectors are scaled - should be proportional to fov only.
//    /// once the fov is found, the depth is just scaled so it fits -- fov affects only the two dimensional shape, so will have to redo?
//    double d, t1, t2, t3, r;
//    d = parameters.at(0);
//    t1 = parameters.at(1);
//    t2 = parameters.at(2);
//    t3 = parameters.at(3);
//    r = parameters.at(4);

//    qDebug()<<"d"<<d;
//    qDebug()<<"t1"<<t1;
//    qDebug()<<"t2"<<t2;
//    qDebug()<<"t3"<<t3;
//    qDebug()<<"r"<<r;

//    QVector3D A;
//    QVector3D B;
//    QVector3D C;
//    QVector3D E;



//    A = d/r*normalizedCoordinate;
//    B = t1/r*normalizedCoordinateTwo;
//    C = t2/r*normalizedCoordinateThree;
//    E = t3/r*normalizedCoordinateFour;

////    double distance = A.z();

//    B = B-A;
//    C = C-A;
//    E = E-A;

//    qDebug()<<"A"<<A;
//    qDebug()<<"B"<<B;
//    qDebug()<<"C"<<C;
//    qDebug()<<"E"<<E;

//    qDebug()<<"B length:"<<B.length();
//    qDebug()<<"C length:"<<C.length();
//    qDebug()<<"E length:"<<E.length();



//    /// acquire rotation in 3D space.


//    /// define the rotation matrix
//    /// R = [ux vx wx
//    ///      uy vy wy
//    ///      uz vx wz]
//    /// Ra = A
//    /// where a is an unrotated point, and A is a rotated point
//    /// cos(theta) = (ux + vy + wz - 1)/2
//    /// w is the cross product of u and v
//    /// u and v are perpendicular to each other
//    /// u and v are both normalized

//    /// R has to satisfy
//    /// Ra = A
//    /// for all the shifts:
//    /// b' = B/|B|
//    /// c' = C/|C|
//    /// e' = E/|E|
//    /// B' = shift/|shift|
//    /// etc

//    /// Rb' = B'
//    /// etc.

//    cameraCenter = findOrientation(B,C,E,shift,shiftTwo,shiftThree);



//    /// distance now is centerOfCamera (or cameraCenter) dot the vector




//    qDebug()<<"Camera center:"<<cameraCenter;

//    double factor = 1;
//    double bFactor;
//    double cFactor;
//    double eFactor;
//    double bZ =nearZero(B.z());
//    double cZ =nearZero(C.z());
//    double eZ = nearZero(E.z());
//    /// "z" length of the shifts are the lengths along the camera center
//    double sOneZ = nearZero(QVector3D::dotProduct(shift,cameraCenter));
//    double sTwoZ = nearZero(QVector3D::dotProduct(shiftTwo,cameraCenter));
//    double sThreeZ = nearZero(QVector3D::dotProduct(shiftThree,cameraCenter));



//    if(notEqual(bZ,sOneZ) || notEqual(cZ,sTwoZ) || notEqual(eZ,sThreeZ))
//    {
//        qDebug()<<bZ<<sOneZ;
//        qDebug()<<cZ<<sTwoZ;
//        qDebug()<<eZ<<sThreeZ;

//        if(bZ != 0)
//        {
//            bFactor = sOneZ/bZ;
//        }
//        else if(sOneZ == 0)
//        {
//            bFactor = 1;
//        }
//        else
//        {
//            bFactor = 1;
//            qDebug()<<"Should not have a denominator of zero in bFactor";
//            qDebug()<<"or else shift z is almost 0";
//        }

//        if(cZ != 0)
//        {
//            cFactor = sTwoZ/cZ;
//        }
//        else if(sTwoZ == 0)
//        {
//            cFactor = 1;
//        }
//        else
//        {
//            cFactor = 1;
//            qDebug()<<"Should not have a denominator of zero in cFactor";
//        }

//        if(eZ != 0)
//        {
//            eFactor = sThreeZ/eZ;
//        }
//        else if(sThreeZ == 0)
//        {
//            eFactor = 1;
//        }
//        else
//        {
//            eFactor = 1;
//            qDebug()<<"Should not have a denominator of zero in eFactor";
//        }

//        if(bFactor == 1 || cFactor == 1 || eFactor == 1)
//        {
//            qDebug()<<"some of the factors are 1, must be on the same depth plane as the central point";
//            if(bFactor == 1 && cFactor == 1 && eFactor == 1)
//            {
//                factor = 1;
//                // probably good, or else they are all on the same z
//                if(bZ == cZ && cZ == eZ)
//                {
//                    qDebug()<<"Points can't detect field of view";
//                }
//            }
//            else
//            {
//                factor = bFactor;
//                if(factor == 1)
//                {
//                    factor = cFactor;
//                    bFactor = cFactor;
//                    if(factor == 1)
//                    {
//                        factor = eFactor;
//                        bFactor = eFactor;
//                        cFactor = eFactor;
//                        if(factor ==1)
//                        {
//                            qDebug()<<"Error in findCamera: all factors are one, but did not enter the all 1 branch";
//                        }
//                    }
//                    else
//                    {
//                        if(eFactor != 1 && eFactor !=cFactor)
//                        {
//                                qDebug()<<"contradictory field of view information occured in findCamera first";
//                        }
//                    }
//                }
//                else
//                {
//                    if(cFactor != 1 && cFactor !=bFactor)
//                    {
//                            qDebug()<<"contradictory field of view information occured in findCamera second";
//                    }
//                    if(eFactor != 1 && eFactor !=bFactor)
//                    {
//                            qDebug()<<"contradictory field of view information occured in findCamera third";
//                    }
//                }

//            }
//        }
//        else if(bFactor == cFactor && cFactor == eFactor)
//        {
//            factor = bFactor;
//        }
//        else
//        {
//            qDebug()<<"contradictory field of view information occured in findCamera fourth";
//            double bPrimeFactor = fabs(bFactor);
//            if(bPrimeFactor < 1) bPrimeFactor = 1/bPrimeFactor;

//            double cPrimeFactor = fabs(cFactor);
//            if(cPrimeFactor < 1) cPrimeFactor = 1/cPrimeFactor;

//            double ePrimeFactor = fabs(eFactor);
//            if(ePrimeFactor < 1) ePrimeFactor = 1/ePrimeFactor;


//            if(bPrimeFactor >= cPrimeFactor && bPrimeFactor >= ePrimeFactor) factor = bFactor;
//            else if (cPrimeFactor >= bPrimeFactor && cPrimeFactor >= ePrimeFactor) factor = cFactor;
//            else if (ePrimeFactor >= bPrimeFactor && ePrimeFactor >= cPrimeFactor) factor = eFactor;
//        }
//        // factor is the field of view scaling
//        // it is given by 2tanVF
//        // F is field of view
//        // F is focal length
//        qDebug()<<"Factor is:"<<factor;
//        if(notEqual(factor,fieldofView) && fieldofView == 1)
//        {
//            findCamera(pointOne,pointTwo,pointThree,pointFour,coordinateOne,shift,shiftTwo,shiftThree,cameraCenter,1.0/factor);
//            return;
//        }
//    }
//    double tanFOV = fieldofView/2/focalLength;
//    double fov = atan(tanFOV);
//    /// Acquired the fov;
//    qDebug()<<"field of view has been calculated to be:"<<fov;
//    double rotation = 0;
//    if(fov < 0)
//    {
//        fov *=-1;
//        rotation = M_PI;
//    }

//    /// to find the shift rotate point a, then move it to coordinateOne
////    QVector3D center = QVector3D(0,0,0);
//    QVector3D oldCameraCenter = QVector3D(0,0,1);
//    QVector3D rotatedAVector = rotateVector(A, cameraCenter, oldCameraCenter);
//    QVector3D newCenter = coordinateOne - rotatedAVector;
//    cameraCenter = newCenter + cameraCenter;

//    qDebug()<<newCenter;
//    qDebug()<<cameraCenter;

//    cameraModel_->setCameraCenter(cameraCenter);
//    cameraModel_->setCameraPosition(newCenter);
//    cameraModel_->setCameraFocalLength(focalLength);
//    cameraModel_->setCameraFOV(fov);
//    cameraModel_->setCameraRotation(rotation);

//    QVector3D one = shift+coordinateOne;
//    QVector3D two = shiftTwo + coordinateOne;
//    QVector3D three = shiftThree + coordinateOne;

//    qDebug()<<"Test results";
//    qDebug()<<"point coordinate pairs should match the input";
//    qDebug()<<transform3Dto2D(coordinateOne)<<transform2Dto3D(pointOne,depth(coordinateOne));
//    qDebug()<<transform3Dto2D(one)<<transform2Dto3D(pointTwo,depth(one));
//    qDebug()<<transform3Dto2D(two)<<transform2Dto3D(pointThree,depth(two));
//    qDebug()<<transform3Dto2D(three)<<transform2Dto3D(pointFour,depth(three));






//    /// pixel aspect ratio
//    /// the only thing not yet accounted for is distortion...
//    /// may not be able to do that
//    /// basically a non linear version of fov

}


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
    shapeList_.insert(index_,AMShapeData());
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
void AMShapeDataSet::finishRectangle(QPointF position)
{
    if(!isValid(current_))return;
    position = undistortPoint(position);
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
void AMShapeDataSet::deleteRectangle(QPointF position)
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
    position = undistortPoint(position);
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
    currentVector_ = (position);
}

/// moves all rectangles by position + rectangleVector_[index]
void AMShapeDataSet::moveAllShapes(QPointF position)
{
    position = undistortPoint(position);
//    QPointF currentVector = currentVector_;
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

        QVector3D coordinate = shapeList_[i].coordinate(TOPLEFT);
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
    double rotation = shapeList_[current_].rotation()+(position - currentVector_).x();
    shapeList_[current_].setRotation(rotation);
    updateShape(current_);
    currentVector_ = position;
}

/// change the current rectangles depth
void AMShapeDataSet::zoomShape(QPointF position)
{
    if(isValid(current_))
    {
        QVector3D coordinate = shapeList_[current_].coordinate(TOPLEFT);
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
        QVector3D oldCoordinate = shapeList_[current_].coordinate(TOPLEFT);
        QVector3D currentPosition = transform2Dto3D(position, depth(oldCoordinate));
        QVector3D newPosition = transform2Dto3D(crosshairPosition, depth(oldCoordinate));
        QVector3D shift = newPosition - currentPosition;


//        currentVector_ = inverseVectorTransform(position, oldCoordinate);
//        moveAllShapes(inverseVectorTransform(crosshairPosition, oldCoordinate));
//        double motorX = motorCoordinate_.x() + ((Y_YMOVEMENT)*(crosshairPosition.x() - position.x())-(X_YMOVEMENT)*(crosshairPosition.y() - position.y()))/((Y_YMOVEMENT)*(X_XMOVEMENT)-(X_YMOVEMENT)*(Y_XMOVEMENT));
//        double motorY = motorCoordinate_.y() + ((Y_XMOVEMENT)*(crosshairPosition.x() - position.x())-(X_XMOVEMENT)*(crosshairPosition.y() - position.y()))/((X_YMOVEMENT)*(Y_XMOVEMENT) - (Y_YMOVEMENT)*(X_XMOVEMENT));
//        motorCoordinate_.setX(motorX);
//        motorCoordinate_.setY(motorY);
//        QVector3D shift = shapeList_[current_].coordinate(TOPLEFT) - oldCoordinate;

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
            shift *=-10;
            motorCoordinate_ += shift;
            inboardOutboard = motorCoordinate_.x();
            upStreamDownStream = motorCoordinate_.z();
            upDown = motorCoordinate_.y();

        }
        else
        {
            shift *= -10;
            shift += motorCoordinate_;
            inboardOutboard = shift.x();
            upStreamDownStream = shift.z();
            upDown = shift.y();

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
     qDebug()<<"Changing the group rectangle model";
    QPointF topLeft = groupRectangle_.first();
    QPolygonF newPolygon = constructRectangle(topLeft,position);
    groupRectangle_ = newPolygon;
}

/// constructs the group rectangle
void AMShapeDataSet::startGroupRectangle(QPointF position)
{
        position = undistortPoint(position);
    qDebug()<<"Creating the group rectangle model";
    QPolygonF polygon = constructRectangle(position,position);
    groupRectangle_ = polygon;
}

/// updates the shape for the given index
void AMShapeDataSet::updateShape(int index)
{
   /// must set shape based on coordinates
    shapeList_[index].setShape(subShape(shapeList_[index]));

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
    // get the current center
    QVector3D center = centerCoordinate(index);
    QVector3D shift = coordinate - center;
    // shift the coordinates to the new center
    shiftCoordinates(shift,index);
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
    for(double i = 0; i<20.0; i++)
    {
        for(double j = 0; j<20.0; j++)
        {
            index_ += 1;
            QPointF topLeft(i*0.05,j*0.05);
            QPointF bottomRight((i+1)*0.05,(j+1)*0.05);
            shapeList_.insert(index_,AMShapeData(constructRectangle(topLeft,bottomRight)));
            QVector<QVector3D> shape;
            for(int i = 0; i < 5; i++)
            {
               shape<<transform2Dto3D(shapeList_[index_].shape()->at(i),cameraModel_->cameraFocalLength());
            }
            shapeList_[index_].setCoordinateShape(shape,5);
            shapeList_[index_].setIdNumber(position.x());
            shapeList_[index_].setTilt(0);
            shapeList_[index_].setRotation(0);
            updateShape(index_);
            current_ = index_;
        }
    }

}

void AMShapeDataSet::oneSelect()
{
    if(isValid(current_))
    {
        QVector<QVector3D> newBeamPosition = rotateShape(shapeList_[current_]);
        beamModel_->setPositionOne(newBeamPosition);

//        qDebug()<<newBeamPosition.at(0);
//        qDebug()<<transform3Dto2D(newBeamPosition.at(0));
//        qDebug()<<newBeamPosition.at(1);
//        qDebug()<<transform3Dto2D(newBeamPosition.at(1));

        emit beamChanged(beamModel_);

    }
    else
    {
        qDebug()<<"Invalid index, cannot add beam shape";
    }
}

void AMShapeDataSet::twoSelect()
{

    if(isValid(current_))
    {
        QVector<QVector3D> newBeamPosition = rotateShape(shapeList_[current_]);
        beamModel_->setPositionTwo(newBeamPosition);
        emit beamChanged(beamModel_);
    }
    else
    {
        qDebug()<<"Invalid index, cannot add beam shape";
    }

}

void AMShapeDataSet::enableMotorMovement(bool isEnabled)
{
    enableMotorMovement_ = isEnabled;
}

void AMShapeDataSet::enableMotorTracking(bool isEnabled)
{
    qDebug()<<isEnabled;
    if(isEnabled)
    {
        qDebug()<<"Connecting motor tracking";
        connect(ssaManipulatorX_, SIGNAL(valueChanged(double)), this, SLOT(motorTracking(double)));
    }
    else
    {
        qDebug()<<"Disconnecting motor tracking";
        disconnect(ssaManipulatorX_,SIGNAL(valueChanged(double)),this,SLOT(motorTracking(double)));
    }
}




/// shifts all coordinates by  the specifies shift
void AMShapeDataSet::shiftCoordinates(QVector3D shift, int index)
{
    shapeList_[index].shift(shift);
    updateShape(index);
}

/// applies the rotation to the shape
AMShapeData AMShapeDataSet::applyRotation(AMShapeData shape)
{
    double rotation = shape.rotation();
    for(int i = 0; i<4; i++)
    {
        shape.setCoordinate(getRotatedPoint(shape.coordinate(i), rotation),i);
    }
    return shape;
}

/// finds the new coordinate of a point, given its coordinate, rotation, and center point
QVector3D AMShapeDataSet::getRotatedPoint(QVector3D point, double rotation)
{
    // changed the axis of rotation
//    center += QVector3D(-0.5,0,-0.35);// should correspond to rotation of the plate
    ///changed the axis of rotation
    point = rotateCoordinate(point,centerOfRotation_,directionOfRotation_,rotation);
    return point;
}

/// Applies tilit to a shape
AMShapeData AMShapeDataSet::applyTilt(AMShapeData shape)
{
    double tilt = shape.tilt();
    QVector3D center = shape.centerCoordinate();
    for(int i = 0; i<4; i++)
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

    double directionTerm = QVector3D::dotProduct(coordinate,direction);

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
        vector[i] = QVector3D(QVector3D::dotProduct(center,terms[i]) + directions[i]*directionTerm,coord[i],centerXDistance[i]+distanceXCoordinate[i]);
        component[i] = QVector3D::dotProduct(vector[i],trigTerms);
    }

    QVector3D rotatedCoordinate(component[0],component[1],component[2]);
    return rotatedCoordinate;


}

/// applies rotation, tilt, and distortion to the shape
QPolygonF AMShapeDataSet::subShape(AMShapeData shape)
{
    if(shape.rotation() != 0) shape = applyRotation(shape);
    if(shape.tilt() != 0) shape = applyTilt(shape);


    QPolygonF newShape;
    for(int i = 0; i < 4; i++)
    {
        newShape<<transform3Dto2D(shape.coordinate(i));
    }
    newShape<<newShape.first();


    if(distortion_)
    {
        newShape = applyDistortion(newShape);
        if(motorCoordinate_.z() != 0)
        newShape = removeDistortion(newShape);
    }


    return newShape;
}

/// applies distortion to the shape
QPolygonF AMShapeDataSet::applyDistortion(QPolygonF shape)
{
    QPolygonF polygon;
    for(int i = 0; i <= TOPCLOSE; i++)
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
    for(int i = 0; i <= TOPCLOSE; i++)
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
    shift *= -0.1;
    QVector3D centerOfRotation = -0.1*newPosition + rotationalOffset;
    QVector3D directionOfRotation = QVector3D(0,1,0);
    centerOfRotation_ = centerOfRotation;
    directionOfRotation_ = directionOfRotation;
    double rotation = -1*r/180*M_PI;


    for(int i = 0; i <= index_; i++)
    {
        shiftCoordinates(shift,i);
        shapeList_[i].setRotation(rotation);
    }



}

QVector<QVector3D> AMShapeDataSet::rotateShape(AMShapeData shape)
{
    if(shape.rotation() != 0) shape = applyRotation(shape);
    if(shape.tilt() != 0) shape = applyTilt(shape);
    QVector<QVector3D> returnShape;
    for(int i = 0; i < 4; i++)
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
    /// Use the position of the item on screen, with knowledge of the
    /// camera position to determine its xyz coordinate.
    /// first convert to a point in an ideal plane, then shift and rotate to match camera
    /// see 3D to 2D for more detailed explanation
    QVector3D coordinate;
    QVector3D shift = cameraModel_->cameraPosition();
    QVector3D actualCameraCenter = cameraModel_->cameraCenter();
    double focalLength = cameraModel_->cameraFocalLength();
    double fieldOfView = cameraModel_->cameraFOV();
    double aspectRatio = scaledSize().height()/scaledSize().width();
    double pixelAspectRatio = cameraModel_->pixelAspectRatio();
    double fov = 2*tan(fieldOfView)*focalLength;
    double theta;
    double oldRotation;

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
    theta = -1*acos((QVector3D::dotProduct(idealCameraCenter,shiftedCameraCenter))/(shiftedCameraCenter.lengthSquared()));
    double coordinateLength = coordinate.length();
    // find the coordinate's current angle
    double cosAlpha = QVector3D::dotProduct(uHat,coordinate)/coordinateLength;
    double sinAlpha = QVector3D::dotProduct(zHat,coordinate)/coordinateLength;

    // find the resultant components
    double uLength = coordinateLength*(cosAlpha*cos(theta)-sinAlpha*sin(theta));
    double vLength = QVector3D::dotProduct(coordinate,vHat);
    double zLength = coordinateLength*(sinAlpha*cos(theta)+cosAlpha*sin(theta));

    // create the new vector
    QVector3D newPosition = uLength*uHat + vLength*vHat + zLength*zHat;
    // shift
    QVector3D finalPosition = newPosition + shift;

    return finalPosition;
}

/// takes a 3D coordinate and changes it to a 2D location on screen
QPointF AMShapeDataSet::transform3Dto2D(QVector3D coordinate)
{
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
    QVector3D cameraCenter = cameraModel_->cameraCenter();
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
    return position;
}

/// scales a dimension based on distance
double AMShapeDataSet::transformDimension(double dimension, QVector3D coordinate)
{
    double focalLength = cameraModel_->cameraFocalLength();
    double newDimension = dimension*focalLength/depth(coordinate);
    return newDimension;
}

/// finds the length of a dimension based on its appearance and distance
double AMShapeDataSet::inverseDimensionTransform(double dimension, QVector3D coordinate)
{
    double focalLength = cameraModel_->cameraFocalLength();
    double newDimension = dimension*depth(coordinate)/focalLength;
    return newDimension;
}

/// takes a vector and finds how it appears
QPointF AMShapeDataSet::transformVector(QPointF vector, QVector3D coordinate)
{
    double focalLength = cameraModel_->cameraFocalLength();
    double distance = depth(coordinate);
    double scale = focalLength/distance;
    QPointF newVector = vector*scale;
    return newVector;
}

/// based on the appearance of a vector, finds the actual vector
QPointF AMShapeDataSet::inverseVectorTransform(QPointF vector, QVector3D coordinate)
{
    double focalLength = cameraModel_->cameraFocalLength();
    QPointF newVector = vector*depth(coordinate)/focalLength;
    return newVector;
}


QPolygonF AMShapeDataSet::screenShape(QPolygonF shape)
{
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
    return shapeList_[index].shape()->containsPoint(position, Qt::OddEvenFill);
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
    QVector3D depthDirection = cameraModel_->cameraCenter() - cameraModel_->cameraPosition();
    QVector3D cameraToCoordinate = coordinate - cameraModel_->cameraPosition();
    depthDirection.normalize();
    QVector3D depth = QVector3D::dotProduct(cameraToCoordinate,depthDirection)*depthDirection;
    return depth;
}

QVector<QVector3D> AMShapeDataSet::findIntersectionShape(int index)
{
    /// First find the shape on the same plane
    QVector<QVector3D> rotatedShape = rotateShape(shapeList_[index]);
    QVector<QVector3D> shape;
    QVector3D topLeft = rotatedShape.at(TOPLEFT);
    QVector3D heightVector = topLeft - rotatedShape.at(BOTTOMLEFT);
    QVector3D widthVector = rotatedShape.at(TOPRIGHT) - topLeft;
    QVector3D hHat = heightVector.normalized();
    QVector3D wHat = widthVector.normalized();
    QVector3D nHat = QVector3D::normal(wHat,hHat);
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
        numerator = QVector3D::dotProduct((topLeft - l0[i]),nHat);
        denominator = QVector3D::dotProduct(lHat[i],nHat);
        if(0 == denominator)
        {
            if(0 == numerator)
            {
                qDebug()<<"Line is on plane";
            }
            else
            {
                qDebug()<<"Line is parallel to plane";
            }
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
    double beamHComponent [4];
    double beamWComponent [4];
//    double beamNComponent [4];

    double shapeHComponent [4];
    double shapeWComponent [4];
    double shapeNComponent [4];
    for(int i = 0; i < 4; i++)
    {
        beamHComponent[i] = QVector3D::dotProduct(hHat,shape[i]);
        beamWComponent[i] = QVector3D::dotProduct(wHat,shape[i]);
//        beamNComponent[i] = QVector3D::dotProduct(nHat,shape[i]);

        beamShape<<QPointF(beamWComponent[i],beamHComponent[i]);

        shapeHComponent[i] = QVector3D::dotProduct(hHat,rotatedShape.at(i));
        shapeWComponent[i] = QVector3D::dotProduct(wHat,rotatedShape.at(i));
        shapeNComponent[i] = QVector3D::dotProduct(nHat,rotatedShape.at(i));

        originalShape<<QPointF(shapeWComponent[i],shapeHComponent[i]);
    }
    beamShape<<beamShape.first();
    originalShape<<originalShape.first();

    QPolygonF intersection = originalShape.intersected(beamShape);
    if(intersection.isEmpty()) return QVector<QVector3D>();

    QVector3D point;
    QPointF oldPoint;
    QVector<QVector3D> intersectionShape;
    for(int i = 0; i < intersection.count(); i++)
    {
        oldPoint = intersection.at(i);
        point = oldPoint.x()*wHat + oldPoint.y()*hHat + shapeNComponent[i]*nHat;
        intersectionShape<<point;
    }
    intersectionShape<<intersectionShape.at(0);

    return intersectionShape;


}

QPolygonF AMShapeDataSet::intersectionScreenShape(QVector<QVector3D> shape3D)
{

    AMShapeData newShape;
    newShape.setCoordinateShape(shape3D,4);
    newShape.setRotation(0);
    newShape.setTilt(0);


    QPolygonF shape;
    shape = subShape(newShape);

    shape = screenShape(shape);

    return shape;

}

/// finds the coordinate of the center of the shape
QVector3D AMShapeDataSet::centerCoordinate(int index)
{
    QVector3D center = QVector3D(0,0,0);
    for(int i = 0; i < 4; i++)//only want the first four points
    {
        center += shapeList_[index].coordinate(i);
    }
    return center/4.0;
}

double AMShapeDataSet::findCoordinateDistance(double coordinateDistance, QVector3D a, QVector3D b, QVector3D c, double shiftOne, double shiftTwo, double thetaOne, double thetaTwo, double oldError)
{
    if(isnan(coordinateDistance))
    {
        qDebug()<<"Not a number";
        return 0;
    }
    double oldDistance = coordinateDistance;
    QPair<double, double> betaResult = beta(coordinateDistance,a,b,c,shiftOne,shiftTwo,thetaOne);
    double betaTerm[2];
    betaTerm[0] = betaResult.first;
    betaTerm[1] = betaResult.second;
    double gammaTerm = gamma(coordinateDistance, a, c, thetaTwo);
    /// eight possible outcomes
    double signedTerm[8];
    double sign[2] = {1,-1};
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            for(int k = 0; k < 2; k++)
            {
                signedTerm[k+2*j+4*i] = sign[j]*betaTerm[i]+sign[k]*gammaTerm;
            }
        }
    }
    double aDotC = QVector3D::dotProduct(a,c);
    double aDotCSquared = pow(aDotC,2.0);
    qDebug()<<aDotCSquared;
    double coefficientNumerator = (aDotCSquared-pow(cos(thetaTwo),2.0));
    double coefficientDenominator = ((1-aDotCSquared)*aDotC);
    qDebug()<<"ThetaTwo"<<thetaTwo;
    qDebug()<<"costhetaTwo"<<cos(thetaTwo);

    qDebug()<<"coefficient"<<coefficientNumerator<<"/"<<coefficientDenominator;
    double coefficient = coefficientNumerator/coefficientDenominator;
    double result[8];
    double error;
    for(int i = 0; i < 8; i++)
    {

        result[i] = signedTerm[i]*coefficient;
        qDebug()<<result[i];
        error = fabs((result[i] - oldDistance)/result[i]);
        qDebug()<<"Error"<<error<<"oldValue="<<oldDistance;
        if(error < 0.001)
            return result[i];
        if(error < oldError)
        {
            qDebug()<<"finding new result";
            result[i] = findCoordinateDistance(result[i],a,b,c,shiftOne,shiftTwo,thetaOne,thetaTwo, error);
        }
    }
    qDebug()<<"finalResults:";
    for(int i = 0; i < 8; i++)
    {
        qDebug()<<result[i];
        if(result[i] !=0 && (result[i] < 1000 && result[i] > -1000))
            return result[i];
    }

    return result[2];

}

QPair<double,double> AMShapeDataSet::beta(double coordinateDistance, QVector3D a, QVector3D b, QVector3D c, double shiftOne, double shiftTwo, double thetaOne)
{
    QPair<double,double> alphaResult = alpha(coordinateDistance,a,b,thetaOne);
    double alphaTerm [2];
    alphaTerm[0] = alphaResult.first;
    alphaTerm[1] = alphaResult.second;
    double termOne = pow(coordinateDistance,2.0)*(pow(QVector3D::dotProduct(a,c),2.0)-1);
    double termTwo[2];
    double result[2];

    for(int i = 0; i < 2; i++)
    {
        termTwo[i] = pow(alphaTerm[i],2.0)-2*coordinateDistance*(QVector3D::dotProduct(a,b))*alphaTerm[i]+pow(coordinateDistance,2.0);
        termTwo[i] *= pow(shiftOne,2.0)/pow(shiftTwo,2.0);
        result[i] = termOne + termTwo[i];
        result[i] = sqrt(result[i]);
    }

    return QPair<double,double>(result[0],result[1]);
}

double AMShapeDataSet::gamma(double coordinateDistance, QVector3D a, QVector3D c, double thetaTwo)
{
    double aDotC = QVector3D::dotProduct(a,c);
    double termOne = sqrt((1-pow(aDotC,2.0)));
    double numerator = termOne*coordinateDistance*sin(2*thetaTwo)/2;
    double denominator = pow(aDotC,2.0)-pow(cos(thetaTwo),2.0);
    if(denominator == 0)
    {
        qDebug()<<"gamma: zero in the denominator";
        return 0;
    }
    return numerator/denominator;
}

QPair<double,double> AMShapeDataSet::alpha(double coordinateDistance, QVector3D a, QVector3D b, double thetaOne)
{
    double aDotB = QVector3D::dotProduct(a,b);
    double squareRootTerm = 1-pow(aDotB,2.0);
    if(squareRootTerm < 0)
    {
        qDebug()<<"alpha: negative under squareRoot";
        qDebug()<<"a and or b must not be normalized...";
        return QPair<double,double>(0,0);
    }
    squareRootTerm = sqrt(squareRootTerm);
    double secondTermNumerator = coordinateDistance*sin(2*thetaOne);
    double secondTermDenominator = 2*(aDotB*aDotB-pow(cos(thetaOne),2.0));
    double secondTerm = secondTermNumerator*squareRootTerm/secondTermDenominator;
    double firstTermNumerator = pow(sin(thetaOne),2.0)*aDotB*coordinateDistance;
    double firstTermDenominator = pow(aDotB,2.0) - pow(cos(thetaOne),2.0);
    double firstTerm = firstTermNumerator/firstTermDenominator;
    QPair<double,double> result;
    result.first = firstTerm + secondTerm;
    result.second = firstTerm - secondTerm;
    return result;
}

QList<double> AMShapeDataSet::getCoordinateSystem(double t2, double d, double shift1Length, double shift2Length, double shift3Length, QVector3D a, QVector3D b, QVector3D c, QVector3D e, double angleBC, double angleBE, double angleCE)
{
    double oldT2 = t2;
    double oldD = d;
    double error = 100;
    while(error > 0.101)
    {
        t2 = calculateT2(t2,d,shift1Length,shift2Length,shift3Length,a,b,c,e,angleBC,angleBE,angleCE);
        d = calculateD(t2,d,shift1Length,shift2Length,a,b,c,angleBC);
//        if(d < 0) d*=-1;
        error = std::max(fabs((t2-oldT2)/t2),fabs((d-oldD)/d));
        oldT2 = t2;
        oldD = d;
        // hope it converges
        /// \todo
    }
    double t1,t3,rSquared, r;
//    t2 = calculateT2(d,shift1Length,shift2Length,a,b,c,angleBC);
    t1 = delta(t2,d,shift1Length,shift2Length,a,b,c,angleBC);
    t3 = omega(t2,d,shift1Length,shift2Length,shift3Length,a,b,c,e,angleBC,angleBE);
    rSquared = (beta(t2,d,shift2Length,a,c));
    if(rSquared < 0)
    {
        qDebug()<<"AMShapeDataSet::getCoordinateSystem radius squared is negative";
    }
    else
    {
        r = sqrt(rSquared);
    }
    QList<double> result;
    result<<d<<t1<<t2<<t3<<r;
    return result;
}

double AMShapeDataSet::calculateT2(double t2, double d, double shift1Length, double shift2Length, double shift3Length, QVector3D a, QVector3D b, QVector3D c, QVector3D e, double angleBC, double angleBE, double angleCE)
{
//    double aTerm;
//    double bTerm;
//    double cTerm;
//    double aOne = d*d*(pow(QVector3D::dotProduct(a,c),2.0)-pow(QVector3D::dotProduct(b,c),2.0));
//    double aTwo = 2*pow(d,3.0)*QVector3D::dotProduct(a,b)*QVector3D::dotProduct(a,c)*QVector3D::dotProduct(b,c);
//    double aThree = shift1Length*shift1Length*pow(QVector3D::dotProduct(b,c),2.0);
//    aTerm = aOne-aTwo-aThree;
//    double bOne = pow(d,3.0)*(4*QVector3D::dotProduct(a,b)*QVector3D::dotProduct(c,b)-2*QVector3D::dotProduct(a,c) + 2*pow(QVector3D::dotProduct(a,b),2.0)*QVector3D::dotProduct(a,c));
//    double bTwo = 2*d*((shift1Length*shift2Length*cos(angleBC))*(QVector3D::dotProduct(a,c)-QVector3D::dotProduct(a,b)*QVector3D::dotProduct(c,b))+(pow(shift1Length,2.0)*pow(shift2Length,2.0)*pow(cos(angleBC),2.0)));
//    bTerm = bOne + bTwo;
//    double cOne = pow(d,4.0)*(1-3*pow(QVector3D::dotProduct(a,b),2.0));
//    double cTwo = pow(d,2.0)*((2*shift1Length*shift2Length*cos(angleBC)*pow(QVector3D::dotProduct(a,b),2.0))-(pow(shift1Length,2.0)*pow(QVector3D::dotProduct(a,b),2.0))-(shift1Length*shift2Length*cos(angleBC)));
//    double cThree = pow((shift1Length*shift2Length*cos(angleBC)),2.0);
//    cTerm = cOne + cTwo + cThree;

//    double firstTerm = -1*bTerm/(2*aTerm);
//    double squareRootTerm = pow(bTerm,2.0)-4*aTerm*cTerm;
//    if(squareRootTerm < 0)
//    {
//        qDebug()<<"complex t2 result";
//        squareRootTerm *= -1;
//        throw 1;
//    }
//    double secondTerm = sqrt(squareRootTerm)/(2*aTerm);
//    double result[2];
//    result[0] = firstTerm + secondTerm;
//    result[1] = firstTerm - secondTerm;
//    if(result[0] > 0)
//    {
//        if(result[1] <= 0)
//        {
//            return result[1];
//        }
//        else if(result[1] < result[0])
//        {
//            return result[1];
//        }
//        else return result[0];
//    }
//    else if(result[1] > 0)
//    {
//            return result[0];
//    }
//    else
//    {
//        if(result[0]>result[1])
//        {
//            return result[0];
//        }
//        else return result[1];
//    }
    double betaTerm = beta(t2,d,shift2Length,a,c);
    double omegaTerm = omega(t2,d,shift1Length,shift2Length, shift3Length, a,b,c,e,angleBC,angleBE);
    double termOne = betaTerm*shift2Length*shift3Length*cos(angleCE);
    double termTwo = omegaTerm*d*QVector3D::dotProduct(a,e) - pow(d,2.0);
    double numerator = termOne + termTwo;
    double denominator = omegaTerm*QVector3D::dotProduct(c,e) - d*QVector3D::dotProduct(a,c);
    if(denominator == 0)
    {
        denominator = 0.00001;
    }
    double result = numerator/denominator;
    return result;
}

double AMShapeDataSet::calcT2(double d, QVector3D a, QVector3D c, double shift2Length)
{
    double termOne = d*QVector3D::dotProduct(a,c);
    double squareTermTwo = d*d*(QVector3D::dotProduct(a,c)-1)+shift2Length*shift2Length;
    if(squareTermTwo < 0)
    {
        qDebug()<<"Complex t2";
        return termOne;
    }
    double termTwo = sqrt(squareTermTwo);
    double result[4];
    result[0] = termOne + termTwo;
    result[1] = termOne - termTwo;

    result[2] = fabs(result[0] - d);
    result[3] = fabs(result[1] - d);
    if(result[2]<result[3])return result[0];
    else return result[1];

}

double AMShapeDataSet::calculateD(double t2, double d, double shift1Length, double shift2Length, QVector3D a, QVector3D b, QVector3D c, double angleBC)
{
    double deltaTerm;
    double betaTerm;
    bool success;
    do
    {
        success = true;
        try
        {
//            t2 = calculateT2(d,shift1Length,shift2Length,a,b,c,angleBC);
            deltaTerm = delta(t2,d,shift1Length,shift2Length,a,b,c,angleBC);
            betaTerm = beta(t2,d,shift2Length,a,c);
        }
        catch (int n)
        {
            qDebug()<<"Exception"<<n;
            d--;
            success = false;
        }
    }while(!success);

    double numerator = pow(deltaTerm,2.0) + pow(d,2.0) - betaTerm*pow(shift1Length,2.0);
    double denominator = 2*deltaTerm*QVector3D::dotProduct(a,b);
    if(denominator == 0)
    {
        denominator = 0.00001;
    }
    double result = numerator/denominator;
    return result;
}

double AMShapeDataSet::delta(double t2, double d, double shift1Length, double shift2Length, QVector3D a, QVector3D b, QVector3D c, double angleBC)
{
//    t2 = calculateT2(d,shift1Length,shift2Length,a,b,c,angleBC);
    double betaTerm = beta(t2,d,shift2Length,a,c);
    double termOne = betaTerm*shift1Length*shift2Length*cos(angleBC);
    double termTwo = t2*d*QVector3D::dotProduct(a,c) - pow(d,2.0);
    double numerator = termOne + termTwo;
    double denominator = t2*QVector3D::dotProduct(b,c) - d*QVector3D::dotProduct(a,b);
    if(denominator == 0)
    {
        denominator = 0.00001;
    }
    double result = numerator/denominator;
    return result;
}

double AMShapeDataSet::omega(double t2, double d, double shift1Length, double shift2Length, double shift3Length, QVector3D a, QVector3D b, QVector3D c, QVector3D e, double angleBC, double angleBE)
{
//   t2 = calculateT2(d,shift1Length,shift2Length,a,b,c,angleBC);
    double deltaTerm = delta(t2,d,shift1Length,shift2Length,a,b,c,angleBC);
    double betaTerm = beta(t2,d,shift2Length,a,c);
    double termOne = betaTerm*shift1Length*shift3Length*cos(angleBE);
    double termTwo = deltaTerm*d*QVector3D::dotProduct(a,b) - pow(d,2.0);
    double numerator = termOne + termTwo;
    double denominator = deltaTerm*QVector3D::dotProduct(b,e) - d*QVector3D::dotProduct(a,e);
    if(denominator == 0)
    {
        denominator = 0.00001;
    }
    double result = numerator/denominator;
    return result;
}

double AMShapeDataSet::beta(double t2, double d, double shift2Length, QVector3D a, QVector3D c )
{
//    t2 = 1;
    //t2 = calculateT2(d,shift1Length,shift2Length,a,b,c,angleBC);
    double numerator = pow(t2,2.0) - 2*t2*d*QVector3D::dotProduct(a,c) + pow(d,2.0);
    if(shift2Length == 0)
    {
        shift2Length = 0.00001;
    }
    double result = numerator/pow(shift2Length,2.0);
    return result;
}

bool AMShapeDataSet::notEqual(double a, double b, double tolerance)
{
    double percent = fabs((a-b)/a);
    return percent>tolerance;
}

QVector3D AMShapeDataSet::findOrientation(QVector3D b, QVector3D c, QVector3D e, QVector3D shiftB, QVector3D shiftC, QVector3D shiftE)
{

    /// find using matrix elimination
    /// [shiftbx shiftby shiftbz  [camcenx  [-bz
    ///  shiftcx shiftcy shiftcz * camceny = -cz
    ///  shiftex shiftey shiftez]  camcenz]  -ez]

    QVector3D shift1 = shiftB/shiftB.length();
    double bz = 1*b.z()/b.length();

    QVector3D shift2 = shiftC/shiftC.length();
    double cz = 1*c.z()/c.length();

    QVector3D shift3 = shiftE/shiftE.length();
    double ez = 1*e.z()/e.length();

    /// augmented matrix
    QVector4D s1 = QVector4D(shift1,bz);
    QVector4D s2 = QVector4D(shift2,cz);
    QVector4D s3 = QVector4D(shift3,ez);
    QVector4D temp;

    int zeroCount = 0;
    if(nearZero(s1.x()) == 0) zeroCount++;
    if(nearZero(s2.y()) == 0) zeroCount++;
    if(nearZero(s3.z()) == 0) zeroCount++;

    if(zeroCount > 0)
    {
        /// look at each rows zeroCount
        int count1 = 0;
        int count2 = 0;
        int count3 = 0;

        if(nearZero(s1.x()) != 0) count1++;
        if(nearZero(s1.y()) != 0) count1++;
        if(nearZero(s1.z()) != 0) count1++;

        if(nearZero(s2.x()) != 0) count2++;
        if(nearZero(s2.y()) != 0) count2++;
        if(nearZero(s2.z()) != 0) count2++;

        if(nearZero(s3.x()) != 0) count3++;
        if(nearZero(s3.y()) != 0) count3++;
        if(nearZero(s3.z()) != 0) count3++;

        if(count1 == 0 || count2 == 0 || count3 == 0)
        {
            //failure
            qDebug()<<"invalid shift";
        }
        else if(count1 == 1 || count2 == 1 || count3 == 1)
        {
            if(count1 == 1)
            {
                if(nearZero(s1.x()) != 0)
                {
                    if(nearZero(s2.y()) == 0 || nearZero(s3.z()) == 0)
                    {
                        temp = s2;
                        s2 = s3;
                        s3 = temp;
                        if(nearZero(s2.y()) == 0 || nearZero(s3.z()) == 0)
                        {
                            //failure
                            qDebug()<<"Bad vectors";
                        }
                    }
                }
                else if(nearZero(s1.y()) != 0)
                {
                    //swap s1 and s2
                    temp = s2;
                    s2 = s1;
                    s1 = temp;
                    if(nearZero(s1.x()) == 0 || nearZero(s3.z()) == 0)
                    {
                        temp = s1;
                        s1 = s3;
                        s3 = temp;
                        if(nearZero(s1.x()) == 0 || nearZero(s3.z()) == 0)
                        {
                            //failure
                            qDebug()<<"Bad vectors";
                        }
                    }
                }
                else if (nearZero(s1.z()) != 0)
                {
                    //swap s3 and s1
                    temp = s3;
                    s3 = s1;
                    s1 = temp;
                    if(nearZero(s2.y()) == 0 || nearZero(s1.x()) == 0)
                    {
                        temp = s2;
                        s2 = s1;
                        s1 = temp;
                        if(nearZero(s2.y()) == 0 || nearZero(s1.x()) == 0)
                        {
                            //failure
                            qDebug()<<"Bad vectors";
                        }
                    }
                }
            }
            else if(count2 == 1)
            {
                if(nearZero(s2.y()) != 0)
                {
                    if(nearZero(s1.x()) == 0 || nearZero(s3.z()) == 0)
                    {
                        temp = s1;
                        s1 = s3;
                        s3 = temp;
                        if(nearZero(s1.x()) == 0 || nearZero(s3.z()) == 0)
                        {
                            //failure
                            qDebug()<<"Bad vectors";
                        }
                    }
                }
                else if(nearZero(s2.x()) != 0)
                {
                    //swap s1 and s2
                    temp = s2;
                    s2 = s1;
                    s1 = temp;
                    if(nearZero(s2.y()) == 0 || nearZero(s3.z()) == 0)
                    {
                        temp = s2;
                        s2 = s3;
                        s3 = temp;
                        if(nearZero(s2.y()) == 0 || nearZero(s3.z()) == 0)
                        {
                            //failure
                            qDebug()<<"Bad vectors";
                        }
                    }
                }
                else if (nearZero(s2.z()) != 0)
                {
                    //swap s3 and s2
                    temp = s3;
                    s3 = s2;
                    s2 = temp;
                    if(nearZero(s1.x()) == 0 || nearZero(s2.y()) == 0)
                    {
                        temp = s1;
                        s1 = s2;
                        s2 = temp;
                        if(nearZero(s1.x()) == 0 || nearZero(s2.y()) == 0)
                        {
                            //failure
                            qDebug()<<"Bad vectors";
                        }
                    }
                }
            }
            else if(count3 == 1)
            {
                if(nearZero(s3.z()) != 0)
                {
                    if(nearZero(s1.x()) == 0 || nearZero(s2.y()) == 0)
                    {
                        temp = s1;
                        s1 = s2;
                        s2 = temp;
                        if(nearZero(s1.x()) == 0 || nearZero(s2.y()) == 0)
                        {
                            //failure
                            qDebug()<<"Bad vectors";
                        }
                    }
                }
                else if(nearZero(s3.x()) != 0)
                {
                    //swap s1 and s3
                    temp = s3;
                    s3 = s1;
                    s1 = temp;
                    if(nearZero(s3.z()) == 0 || nearZero(s2.y()) == 0)
                    {
                        temp = s3;
                        s3 = s2;
                        s2 = temp;
                        if(nearZero(s3.z()) == 0 || nearZero(s2.y()) == 0)
                        {
                            //failure
                            qDebug()<<"Bad vectors";
                        }
                    }
                }
                else if (nearZero(s3.y()) != 0)
                {
                    //swap s3 and s2
                    temp = s3;
                    s3 = s2;
                    s2 = temp;
                    if(nearZero(s1.x()) == 0 || nearZero(s3.z()) == 0)
                    {
                        temp = s1;
                        s1 = s3;
                        s3 = temp;
                        if(nearZero(s1.x()) == 0 || nearZero(s3.z()) == 0)
                        {
                            //failure
                            qDebug()<<"Bad vectors";
                        }
                    }
                }
            }
        }
        else
        {
            /// all counts at least two
            /// add one that has a zero in its place to the others
            if(nearZero(s1.x()) == 0)
            {
                s2 += s1;
                s3 += s1;
                if(nearZero(s2.x()) != 0)
                {
                    s1 = s2 - s1;
                }
                else
                {
                    s1 = s3 - s2;
                }
            }
            else if(nearZero(s2.y()) == 0)
            {
                s1 += s2;
                s3 += s2;
                if(nearZero(s1.y()) != 0)
                {
                    s2 = s1 - s2;
                }
                else
                {
                    s2 = s3 - s2;
                }
            }
            else if(nearZero(s3.z()) == 0)
            {
                s1 += s3;
                s2 += s3;
                if(nearZero(s1.z()) != 0)
                {
                    s3 = s1 - s3;
                }
                else
                {
                    s3 = s2 - s3;
                }
            }

        }
    }
    /// matrix should be correct now
    s2 = s2 -(s2.x()/s1.x())*s1;
    s3 = s3 - (s3.x()/s1.x())*s1;
    if(nearZero(s2.y()) == 0)qDebug()<<"Improper matrix, xy dependent";
    s3 = s3 - (s3.y()/s2.y())*s2;
    if(nearZero(s3.z()) == 0)qDebug()<<"Improper matrix yz dependent";
    s2 = s2 -(s2.z()/s3.z())*s3;
    if(nearZero(s2.y()) == 0) qDebug()<<"Improper matrix zy dependent";
    s1 = s1 - (s1.y()/s2.y())*s2 - (s1.z()/s3.z())*s3;

    if(nearZero(s1.x()) == 0)qDebug()<<"Improper matrix x dependent";

    s1 = s1/s1.x();
    s2 = s2/s2.y();
    s3 = s3/s3.z();

    QVector3D cameraCenter = QVector3D(s1.w(),s2.w(),s3.w());
    cameraCenter.normalize();

    return cameraCenter;




//    b.normalize();
//    c.normalize();
//    e.normalize();

//    shiftB.normalize();
//    shiftC.normalize();
//    shiftE.normalize();

//    QVector3D u = QVector3D(1, 1, 1);
//    QVector3D v = QVector3D(1, 1, 1);
//    u.normalize();
//    v.normalize();
//    double error = 100;
//    QVector3D oldU = u;
//    while(error > 0.001)
//    {
//        calculateVectors(u,v,b,shiftB);
//        error = std::max(absError(u.x(),oldU.x()),std::max(absError(u.y(),oldU.y()),absError(u.z(),oldU.z())));
//        // assuming v is fine if u is...
//        oldU = u;
//    }
//    // "calculate" w
//    // confirm the results
//    // calculate theta
//    // need to find the rotation unit vector
//    // Rv = v;
//    qDebug()<<u.x();




//    QVector3D w;
//    w.setX(u.y()*v.z()-u.z()*v.y());
//    w.setY(u.z()*v.x()-u.x()*v.z());
//    w.setZ(u.x()*v.y()-u.y()*v.x());

//    qDebug()<<"Determinant is";
//    double det = u.x()*(v.y()*w.z()-w.y()*v.z()) + v.x()*(w.y()*u.z()-u.y()*w.z()) + w.x()*(u.y()*v.z()-v.y()*u.z());
//    qDebug()<<det;


//    double theta = acos((u.x()+v.y()+w.z()-1)/2);
//    QVector3D rotationNormal;

//    /// to solve for rotationNormal,solve Rv = v for v
//    double aTerm = v.x()*u.z()+v.z()-v.z()*v.x();
//    double bTerm = (1-u.x())*(1-w.z())-w.x()*u.z();
//    if(bTerm == 0)
//    {
//        qDebug()<<"AMShapeDataSet::findOrientation: bTerm is zero...";
//        rotationNormal.setY(0);
//        rotationNormal.setZ(1);
//        rotationNormal.setX(0);
//    }
//    else
//    {
//        double termOneNumerator = v.x()+aTerm/bTerm*w.x();
//        double termOneDenominator = 1-u.x();
//        double termOne;
//        if(termOneDenominator == 0)
//        {
//            qDebug()<<"AMShapeDataSet::findOrientation: termOneDenominatoris 0";
//            if(nearZero(termOneNumerator) == 0)
//            {
//                qDebug()<<"Undefined";
//                termOne = 1;

//            }
//            else
//            {
//                qDebug()<<"Indeterminate";
//                termOne = 0;
//            }
//        }
//        else
//        {
//            termOne = termOneNumerator/termOneDenominator;
//        }
//        termOne*=termOne;
//        double termTwo = 1+aTerm*aTerm/(bTerm*bTerm);
//        double denominator = termOne + termTwo;
//        if(denominator == 0)
//        {
//            qDebug()<<"AMShapeDataSet::findOrientation: denominator is zero";
//            rotationNormal.setY(0);
//        }
//        else
//        {
//            rotationNormal.setY(sqrt(1/denominator));
//        }

//        rotationNormal.setZ(rotationNormal.y()*aTerm/bTerm);

//        if(termOneDenominator == 0)
//        {
//            qDebug()<<"AMShapeDataSet::findOrientation: termOneDenominator is 0";
//            rotationNormal.setX(0);
//        }
//        else
//        {
//            rotationNormal.setX((rotationNormal.y()*v.x()+rotationNormal.z()*w.x())/termOneDenominator);
//        }

//    }

//    /// have the rotation, and the rotation unit vector, perform the rotation, and return the rotated camera center;
//    /// first check to see that there actually is a rotation performed, as the rotation unit vector will be
//    /// arbitrary with no rotation

//    QVector3D cameraCenter = QVector3D(0,0,1);

//    if(nearZero(theta) == 0 || isnan(theta))
//    {
//        /// skip the rotation
//        cameraCenter = QVector3D(0,0,1);
//        // or is it -1?
//    }
//    else
//    {
//        /// calculate the rotation
//        /// exact same process as calculating the rotation in transform
//        /// just rotate the camera center from 0,0,-1 to its new position
//        /// which is given by rotating it
//        /// could just apply the rotation matrix, that could be easier...
//        /// since there are two zeros it's just
//        /// rotatedCameraCenter = w*cameraCenter.z
//        cameraCenter = w*cameraCenter.z();
//        cameraCenter.normalize();

//    }




//    return cameraCenter;

}

void AMShapeDataSet::calculateVectors(QVector3D &u, QVector3D &v, QVector3D a, QVector3D newA)
{
    /// calculate uz, vx, vy from the simultaneous equations
    /// calculate ux, uy , vz
    u.normalize();
    v.normalize();

    double newAX = newA.x();
    double newAY = newA.y();
    double newAZ = newA.z();

    u.setZ(complexAbs(uZ(u.z(), v.x(), v.y(), a, newAX)));
    u.normalize();
    v.setX(complexAbs(vX(u.z(),v.x(),v.y(),a,newAX,newAZ)));
    v.normalize();
    v.setY(complexAbs(vY(u.z(),v.x(),v.y(),a,newAX,newAY)));
    v.normalize();
    v.setZ(complexAbs(vZ(v.x(),v.y())));
    v.normalize();
    u.setY(complexAbs(uY(u.z(),v.x(),v.y(),a,newAX)));
    u.normalize();
    u.setX(complexAbs(uX(u.z(),v.x(),v.y(),a,newAX)));
    u.normalize();



}

/// returns uX in square coordinates
QPointF AMShapeDataSet::uX(double uz, double vx, double vy, QVector3D a, double newAX)
{
    QPointF vz = vZ(vx,vy);

    if(vx == 0)
    {
        qDebug()<<"AMShapeDataSet::uX: vx is 0; indeterminate result";
        return QPointF(0.57,0);
    }
    QPointF uy = uY(uz,vx,vy,a,newAX);

    QPointF result = -1*(uy*vy-uz*vz)/vx;

    return result;
}

QPointF AMShapeDataSet::uY(double uz, double vx, double vy, QVector3D a, double newAX)
{
    QPointF vz = vZ(vx,vy);
    QPointF vxComplex = QPointF(vx,0);
    QPointF vyComplex = QPointF(vy, 0);
    QPointF numerator = newAX*vxComplex+uz*vz*a.x()-vx*vxComplex*a.y()+uz*vxComplex*vy*a.z();
    QPointF denominator =(vz*vx*a.z()-vyComplex*a.x());
    if(complexAbs(denominator) == 0)
    {
        qDebug()<<"AMShapeDataSet::uY: denominator is 0; indeterminate result";
        return QPointF(0.57,0);
    }
    numerator = convertToPolar(numerator);
    denominator = convertToPolar(denominator);
    QPointF result = QPointF(numerator.x()/denominator.x(),numerator.y()-denominator.y());
    result = convertFromPolar(result);
    return result;
}

/// returns the complex number uZ in square coordinates
QPointF AMShapeDataSet::uZ(double uz, double vx, double vy, QVector3D a, double newAX)
{
    QPointF ux = convertToPolar(uX(uz,vx,vy,a,newAX));
    QPointF uy = convertToPolar(uY(uz,vx,vy,a,newAX));
    QPointF uxSquared = QPointF(ux.x()*ux.x(),2*ux.y());
    QPointF uySquared = QPointF(uy.x()*uy.x(),2*uy.y());
    uxSquared = convertFromPolar(uxSquared);
    uySquared = convertFromPolar(uySquared);
    QPointF innerTerm = QPointF(1,0) - uxSquared - uySquared;
//    if(innerTerm < 0)
//    {
//        qDebug()<<"AMShapeDataSet::uZ: inner term is negative; complex result";
//        innerTerm*=-1;
//    }
    QPointF resultOnePolar = complexSquareRoot(convertToPolar(innerTerm)).first;
    QPointF resultTwoPolar = complexSquareRoot(convertToPolar(innerTerm)).second;
    if(fabs(resultOnePolar.y())<fabs(resultTwoPolar.y()))
    {
        return convertFromPolar(resultOnePolar);
    }
    return convertFromPolar(resultTwoPolar);
}

QPointF AMShapeDataSet::vX(double uz, double vx, double vy, QVector3D a, double newAX, double newAZ)
{
    QPointF ux = uX(uz,vx,vy,a,newAX);
    QPointF uy = uY(uz,vx,vy,a,newAX);
    QPointF vz = vZ(vx,vy);
    QPointF uzComplex = QPointF(uz,0);
    QPointF newAZComplex = QPointF(newAZ,0);
    QPointF numerator = uzComplex*a.x()+vz*a.y()+ux*vy*a.z()-newAZComplex;
    QPointF denominator = uy*a.z();
    if(complexAbs(denominator) == 0)
    {
        qDebug()<<"AMShapeDataSet::vX: denominator is zero; invalid solution";
        return QPointF(0,0);
    }
    numerator = convertToPolar(numerator);
    denominator = convertToPolar(denominator);
    QPointF result = QPointF(numerator.x()/denominator.x(),numerator.y()-denominator.y());
    result = convertFromPolar(result);
    return result;
}


QPointF AMShapeDataSet::vY(double uz, double vx, double vy, QVector3D a, double newAX, double newAY)
{
    QPointF ux = uX(uz,vx,vy,a,newAX);
    QPointF uy = uY(uz,vx,vy,a,newAX);
    QPointF vz = vZ(vx,vy);
    QPointF newAYComplex = QPointF(newAY,0);
    QPointF uzComplex = QPointF(uz,0);
    ux = convertToPolar(ux);
    vz = convertToPolar(vz);
    QPointF uxVz = QPointF(ux.x()*vz.x(),ux.y()+vz.y());
    uxVz = convertFromPolar(uxVz);
    if(a.y() == 0)
    {
        qDebug()<<"AMShapeDataSet::vY: ay is zero; invalid solution";
    }
    QPointF result = (newAYComplex - uy*a.x() + uzComplex*vx*a.z() - uxVz*a.z())/a.y();
//    return (newAY - uy*a.x() +  uz*vx*a.z() - ux*vz*a.z())/a.y()
    return result;
}

QPointF AMShapeDataSet::vZ(double vx, double vy)
{
    QPointF innerTerm = QPointF(1-vx*vx-vy*vy,0);
//    if(innerTerm < 0)
//    {
//        qDebug()<<"AMShapeDataSet::vZ: inner term is negative, imaginary result";
//       innerTerm*=-1;
//    }
//    return sqrt(innerTerm);
    innerTerm = convertToPolar(innerTerm);
    QPair<QPointF,QPointF> result = complexSquareRoot(innerTerm);
    if(fabs(result.first.y()) < fabs(result.second.y()))
    {
        return convertFromPolar(result.first);
    }
    return convertFromPolar(result.second);

}

double AMShapeDataSet::nearZero(double a,double tolerance)
{
    if(fabs(a)<tolerance)
    {
        return 0;
    }
    else return a;
}

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

QVector3D AMShapeDataSet::rotateVector(QVector3D coordinate, QVector3D directionOfRotation, QVector3D directionBeforeRotation)
{
    directionBeforeRotation.normalize();
    directionOfRotation.normalize();
    QVector3D zHat = QVector3D(0,0,1);
    QVector3D vHat = QVector3D::normal(directionBeforeRotation,directionOfRotation);
    if(vHat.isNull())
    {
        return coordinate;
    }
    double theta = -1*acos((QVector3D::dotProduct(directionBeforeRotation,directionOfRotation)));
    double coordinateLength = coordinate.length();
    QVector3D uHat = QVector3D::normal(vHat,zHat);
    double cosAlpha = QVector3D::dotProduct(uHat,coordinate)/coordinateLength;
    double sinAlpha = QVector3D::dotProduct(zHat,coordinate)/coordinateLength;

    double uLength = coordinateLength*(cosAlpha*cos(theta) - sinAlpha*sin(theta));
    double vLength = QVector3D::dotProduct(coordinate,vHat);
    double zLength = coordinateLength*(sinAlpha*cos(theta) + cosAlpha*sin(theta));

    QVector3D newPosition = uLength*uHat + vLength*vHat + zLength*zHat;
    return newPosition;

}

double AMShapeDataSet::complexAbs(QPointF complexNumber)
{
    complexNumber = convertToPolar(complexNumber);
    return complexNumber.x();

}

/// \todo move this
void AMShapeDataSet::motorTracking(double)
{
    /// update coordinates, line edits, don't move the motor
    double motorX = ssaManipulatorX_->value();
    double motorY = ssaManipulatorZ_->value();
    double motorZ = ssaManipulatorY_->value();
    double motorRotation = ssaManipulatorRot_->value();


    emit motorMoved();

    setMotorCoordinate(motorX,motorY,motorZ,motorRotation);


}


QPointF AMShapeDataSet::convertToPolar(QPointF squareNumber)
{
    double rSquared = squareNumber.x()*squareNumber.x() + squareNumber.y()*squareNumber.y();
    double theta;
    if(squareNumber.x() == 0)
    {
        theta = 0;
    }
    else
    {
        theta = atan(squareNumber.y()/squareNumber.x());
    }
    double r = sqrt(rSquared);
    return QPointF(r,theta);
}

QPointF AMShapeDataSet::convertFromPolar(QPointF polarNumber)
{
    double radius = polarNumber.x();
    double theta = polarNumber.y();
    return QPointF(radius*cos(theta),radius*sin(theta));
}

QPair<QPointF,QPointF> AMShapeDataSet::complexSquareRoot(QPointF polarNumber)
{
    QPointF first;
    QPointF second;
    double r = sqrt(polarNumber.x());
    double thetaOne = polarNumber.y()/2;
    double thetaTwo = polarNumber.y()/2 + M_PI;
    first = QPointF(r,thetaOne);
    second = QPointF(r,thetaTwo);
    return QPair<QPointF,QPointF>(first,second);
}

void AMShapeDataSet::getTransforms(QPointF points[], QVector3D coordinates[])
{
    QVector3D iHat, kHat;
    QVector3D shift;
    /// R = [iHat|jHat|kHat] T = shift^T
    /// x = [point^T|1]
    /// X = [coord^T|1]
    /// K = [R T]
    /// A = I*[ax|ay|1]
    /// x = A*K*X
    /// cameraCoords = K*X


    // test of solve matrix
    QVector4D test[4];
    test[0] = QVector4D(8,2,1,-1);
    test[1] = QVector4D(-1,0,2,-2);
    test[2] = QVector4D(2,2,0,-3);
    test[3] = QVector4D(-1,1,-3,0);
    QVector4D testAnswer = QVector4D(11,-3,-6,-8);
    QVector4D result = solveMatrix(test,testAnswer);
    qDebug()<<result;


    /// initial kHat guess = 0,0,1
    kHat = QVector3D(0,0,1);
    /// initial shift guess = 0,0,0
    shift = coordinates[0];
    QVector4D kMatrix = QVector4D(kHat,shift.z());
    /// E = [Xn][kMatrix]^T-[1|1|1|1]
    /// want to minimize E

    QVector4D coordTermOne = QVector4D(coordinates[0],1);
    QVector4D coordTermTwo = QVector4D(coordinates[1],1);
    QVector4D coordTermThree = QVector4D(coordinates[2],1);
    QVector4D coordTermFour = QVector4D(coordinates[3],1);



    QVector4D matrix[4];
    matrix[0] = coordTermOne;
    matrix[1] = coordTermTwo;
    matrix[2] = coordTermThree;
    matrix[3] = coordTermFour;

    QVector4D answers = QVector4D(1,1,1,1);

    answers = solveMatrix(matrix,answers);


//    double xSum = pointOne.x()+pointTwo.x()+pointThree.x()+pointFour.x();
    QVector4D xAnswers = QVector4D(points[0].x(),points[1].x(), points[2].x(), points[3].x());
    xAnswers = solveMatrix(matrix,xAnswers);


    kHat = QVector3D(answers);
    iHat = QVector3D(xAnswers);


//    double ySum = pointOne.y() + pointTwo.y() + pointThree.y() + pointFour.y();
    QVector4D yAnswers = QVector4D(points[0].y(), points[1].y(), points[2].y(), points[3].y());
    yAnswers = solveMatrix(matrix,yAnswers);


    QVector4D iVector = QVector4D(xAnswers);
    QVector4D jVector = QVector4D(yAnswers);


    double alphaX = points[2].x()/(QVector4D::dotProduct(iVector,coordTermThree));
    double alphaY = points[2].y()/(QVector4D::dotProduct(jVector,coordTermThree));

    qDebug()<<alphaX<<alphaY;

    QVector4D xTerm = xAnswers;
    QVector4D yTerm = yAnswers;
    QVector4D zTerm = answers;

    QVector3D offset = QVector3D(0.5,0.5,0);

    QVector3D firstPoint = QVector3D(QVector4D::dotProduct(xTerm,coordTermOne),QVector4D::dotProduct(yTerm,coordTermOne),QVector4D::dotProduct(zTerm,coordTermOne));
    qDebug()<<firstPoint + offset;
    QVector3D secondPoint = QVector3D(QVector4D::dotProduct(xTerm,coordTermTwo),QVector4D::dotProduct(yTerm,coordTermTwo),QVector4D::dotProduct(zTerm,coordTermTwo));
    qDebug()<<secondPoint + offset;

    QVector3D thirdPoint = QVector3D(QVector4D::dotProduct(xTerm,coordTermThree),QVector4D::dotProduct(yTerm,coordTermThree),QVector4D::dotProduct(zTerm,coordTermThree));
    qDebug()<<thirdPoint + offset;

    QVector3D fourthPoint = QVector3D(QVector4D::dotProduct(xTerm,coordTermFour),QVector4D::dotProduct(yTerm,coordTermFour),QVector4D::dotProduct(zTerm,coordTermFour));
    qDebug()<<fourthPoint + offset;

    QVector4D testPoint = QVector4D(2,0,2,0);
    testPoint += coordTermOne;
    QVector3D testLocation = QVector3D(QVector4D::dotProduct(xTerm,testPoint),QVector4D::dotProduct(yTerm,testPoint),QVector4D::dotProduct(zTerm,testPoint));
    qDebug()<<testLocation + offset;
    qDebug()<<coordTermOne<<coordTermTwo<<coordTermThree<<coordTermFour<<testPoint;
    qDebug()<<xAnswers;
    qDebug()<<yAnswers;
    qDebug()<<answers;
    QVector3D kR [3];
    kR[0] = QVector3D(xAnswers);
    kR[1] = QVector3D(yAnswers);
    kR[2] = QVector3D(answers);
    factorTransformMatrix(kR);

    MatrixXd matrixP = directLinearTransform(coordinates,points);

    MatrixXd matrixB = matrixP.block(0,0,3,3);
    MatrixXd matrixSubB = matrixP.col(3);
    MatrixXd matrixA = intrinsicParameters(matrixB);
    MatrixXd matrixR = rotationParameters(matrixA,matrixB);
    MatrixXd matrixT = translationParameters(matrixA,matrixSubB);

    MatrixXd matrixExtrinsic(3,4);
    matrixExtrinsic<<matrixR,matrixT;

    MatrixXd coordinate [6];
    for(int i = 0; i<6; i++)
    {
        coordinate[i].resize(4,1);
        coordinate[i]<<coordinates[i].x(),coordinates[i].y(),coordinates[i].z(),1;
    }

    MatrixXd point [6];
    for(int i= 0; i<6; i++)
    {
        point[i] = matrixA*matrixExtrinsic*coordinate[i];
        point[i] = point[i]/(-1*(point[i](2,0)));
        qDebug()<<"Point"<<i;
        qDebug()<<point[i](0,0)<<point[i](1,0)<<points[i];

    }

    MatrixXd localCoords[6];
    for(int i = 0; i < 6; i++)
    {
        localCoords[i].resize(3,1);
    }
    localCoords[0]<<0,0,2;
    localCoords[1]<<0.2,1.2,2;
    localCoords[2]<<1,0,2;
    localCoords[3]<<0,-0.6,3;
    localCoords[4]<<0.9,0,3;
    localCoords[5]<<0.3,0.3,3;

    for(int i= 0; i<6; i++)
    {
        point[i] = matrixA*localCoords[i];
        point[i] = point[i]/point[i](2,0);
        qDebug()<<"Point"<<i;
        qDebug()<<point[i](0,0)<<point[i](1,0)<<points[i];

    }

    MatrixXd testCoordinate(4,1);
    testCoordinate<<13.2,52.5,12.8,1;
    MatrixXd testScreenPoint = matrixA*matrixExtrinsic*testCoordinate;
    testScreenPoint = testScreenPoint/(-1*(testScreenPoint(2,0)));
    qDebug()<<"test point at"<<testScreenPoint(0,0)<<testScreenPoint(1,0);
    qDebug()<<"should be 0.4, 0.5";

    MatrixXd testTwo(3,1);
    testTwo<<1.2,1.5,3;
    testScreenPoint = matrixA*testTwo;
    testScreenPoint = testScreenPoint/testScreenPoint(2,0);
    qDebug()<<"test point at"<<testScreenPoint(0,0)<<testScreenPoint(1,0);
    qDebug()<<"should be 0.4, 0.5";

    MatrixXd intermediateCoords[6];
    double factor;
    for(int i = 0; i<6; i++)
    {
        intermediateCoords[i] = matrixExtrinsic*coordinate[i];
        if(i == 0)factor = 2/intermediateCoords[i](2,0);
        intermediateCoords[i] *= factor;
        qDebug()<<intermediateCoords[i](0,0)<<intermediateCoords[i](1,0)<<intermediateCoords[i](2,0);
        point[i] = matrixA*intermediateCoords[i];
        point[i] /= point[i](2,0)*-1;
         qDebug()<<point[i](0,0)<<point[i](1,0)<<points[i];
    }
/*
//    QVector4D transform [3];
//    transform[0] = xAnswers;
//    transform[1] = yAnswers;
//    transform[2] = answers;
//    QVector3D screenPositions [4];
//    screenPositions[0] = QVector3D(pointOne.x(),pointOne.y(),1);
//    screenPositions[1] = QVector3D(pointTwo.x(),pointTwo.y(),1);
//    screenPositions[2] = QVector3D(pointThree.x(),pointThree.y(),1);
//    screenPositions[3] = QVector3D(pointFour.x(),pointFour.y(),1);
//    for(int i = 0; i < 4; i++)
//    {
//        qDebug()<<findCoordinate(transform,screenPositions[i]);
//    }



//    QVector4D xTerm = QVector4D(termOne, termTwo, termThree, termFour);
//    QVector4D yTerm = QVector4D(1,1,1,1);
//    QVector4D eEstimate = xTerm - yTerm;
//    double estimateValue = eEstimate.length();

//    double bestEstimate = estimateValue;
//    QVector4D bestKMatrix = kMatrix;
//    bool smaller = true;
//    double sign;
//    double lastEstimate = estimateValue;

//    while(bestEstimate > tolerance)
//    {
//        if(smaller)
//        {
//            sign = -1;
//        }
//        else sign = 1;
//        kMatrix.setX(kMatrix.x() + sign*kMatrix.x()*bestEstimate/100 + sign*bestEstimate/1000);
//        kMatrix.setY(kMatrix.y() + sign*kMatrix.y()*bestEstimate/100 + sign*bestEstimate/1000);
//        kMatrix.setZ(kMatrix.z() + sign*kMatrix.z()*bestEstimate/100 + sign*bestEstimate/1000);
//        kMatrix.setW(kMatrix.w() + sign*kMatrix.w()*bestEstimate/100 + sign*bestEstimate/1000);
//        kHat = kMatrix.toVector3D();
//        kHat.normalize();
//        kMatrix = QVector4D(kHat,kMatrix.w());

//        termOne = QVector4D::dotProduct(coordTermOne,kMatrix);
//        termTwo = QVector4D::dotProduct(coordTermTwo,kMatrix);
//        termThree = QVector4D::dotProduct(coordTermThree,kMatrix);
//        termFour = QVector4D::dotProduct(coordTermFour,kMatrix);

//        eEstimate = QVector4D(termOne, termTwo, termThree, termFour) - yTerm;

//        estimateValue = eEstimate.length();


//        if(estimateValue < bestEstimate)
//        {
//            bestEstimate = estimateValue;
//            bestKMatrix = kMatrix;
//        }
//        else if(estimateValue > lastEstimate)
//        {
//            smaller = !smaller;

//        }
//        lastEstimate = estimateValue;


//      }
    */


    qDebug()<<"Best kMatrix = "<<kMatrix;
}

/// solves a 4x4 matrix A with the form y = Ax for the values of x, given y
/// y and x are 4x1 both
QVector4D AMShapeDataSet::solveMatrix(QVector4D coefficients[], QVector4D answers)
{
    QVector4D coordTermOne = coefficients[0];
    QVector4D coordTermTwo = coefficients[1];
    QVector4D coordTermThree = coefficients[2];
    QVector4D coordTermFour = coefficients[3];

    QVector4D matrix[4];
    matrix[0] = 2*coordTermOne + coordTermTwo + coordTermThree + coordTermFour;
    matrix[1] = coordTermOne + 2*coordTermTwo + coordTermThree + coordTermFour;
    matrix[2] = coordTermOne + coordTermTwo + 2*coordTermThree + coordTermFour;
    matrix[3] = coordTermOne + coordTermTwo + coordTermThree + 2*coordTermFour;

    QVector4D newAnswers;

    newAnswers.setX(2*answers.x() + answers.y() + answers.z() + answers.w());
    newAnswers.setY(answers.x() + 2*answers.y() + answers.z() + answers.w());
    newAnswers.setZ(answers.x() + answers.y() + 2*answers.z() + answers.w());
    newAnswers.setW(answers.x() + answers.y() + answers.z() + 2*answers.w());




    if(nearZero(matrix[0].x()) == 0)
    {
        if(!(nearZero(coordTermOne.x()) == 0))
        {
            matrix[0] += coordTermOne;
            newAnswers.setX(newAnswers.x()+answers.x());
        }
        else if(!(nearZero(coordTermTwo.x()) == 0))
        {
            matrix[0] -= coordTermTwo;
            newAnswers.setX(newAnswers.x()-answers.y());
        }
        else if(!(nearZero(coordTermThree.x()) == 0))
        {
            matrix[0] -= coordTermThree;
            newAnswers.setX(newAnswers.x()-answers.z());
        }
        else if(!(nearZero(coordTermFour.x()) == 0))
        {
            matrix[0] -= coordTermFour;
            newAnswers.setX(newAnswers.x()-answers.w());
        }
        else
        {
            qDebug()<<"Not enough x's";
        }
    }
    if(nearZero(matrix[1].y()) == 0)
    {
        if(nearZero(coordTermTwo.y()) != 0)
        {
            matrix[1] += coordTermTwo;
            newAnswers.setY(newAnswers.y()+answers.y());
        }
        else if(nearZero(coordTermThree.y()) != 0)
        {
            matrix[1] -= coordTermThree;
            newAnswers.setY(newAnswers.y()-answers.z());
        }
        else if(nearZero(coordTermFour.y()) != 0)
        {
            matrix[1] -= coordTermFour;
            newAnswers.setY(newAnswers.y()-answers.w());
        }
        else if(nearZero(coordTermOne.y()) != 0)
        {
            matrix[1] -= coordTermOne;
            newAnswers.setY(newAnswers.y()-answers.x());
        }
        else
        {
            qDebug()<<"Not enough y's";
        }
    }
    if(nearZero(matrix[2].z()) == 0)
    {
        if(!nearZero(coordTermThree.z()) == 0)
        {
            matrix[2] += coordTermThree;
            newAnswers.setZ(newAnswers.z()+answers.z());
        }
        else if(!nearZero(coordTermFour.z()) == 0)
        {
            matrix[2] -= coordTermFour;
            newAnswers.setZ(newAnswers.z()-answers.w());
        }
        else if(!(nearZero(coordTermOne.z()) == 0))
        {
            matrix[2] -= coordTermOne;
            newAnswers.setZ(newAnswers.z()-answers.x());
        }
        else if(!nearZero(coordTermTwo.z()) == 0)
        {
            matrix[2] -= coordTermTwo;
            newAnswers.setZ(newAnswers.z()-answers.y());
        }
        else
        {
            qDebug()<<"Not enough z's";
        }
    }
    if(nearZero(matrix[3].w()) == 0)
    {
        if(!nearZero(coordTermFour.w()) == 0)
        {
            matrix[3] += coordTermFour;
            newAnswers.setW(newAnswers.w()+answers.w());
        }
        else if(!nearZero(coordTermOne.w()) == 0)
        {
            matrix[3] -= coordTermOne;
            newAnswers.setW(newAnswers.w()-answers.x());
        }
        else if(!nearZero(coordTermTwo.w()) == 0)
        {
            matrix[3] -= coordTermTwo;
            newAnswers.setW(newAnswers.w()-answers.y());
        }
        else if(!nearZero(coordTermThree.w()) == 0)
        {
            matrix[3] -= coordTermThree;
            newAnswers.setW(newAnswers.w()-answers.z());
        }
        else
        {
            qDebug()<<"Not enough w's";
        }
    }
    answers = newAnswers;



    answers.setY(answers.y()-(matrix[1].x()/matrix[0].x())*answers.x());
    matrix[1] = matrix[1] - (matrix[1].x()/matrix[0].x())*matrix[0];
    answers.setZ(answers.z()-(matrix[2].x()/matrix[0].x())*answers.x());
    matrix[2] = matrix[2] - (matrix[2].x()/matrix[0].x())*matrix[0];
    answers.setW(answers.w()-(matrix[3].x()/matrix[0].x())*answers.x());
    matrix[3] = matrix[3] - (matrix[3].x()/matrix[0].x())*matrix[0];


    if(nearZero(matrix[1].y()) == 0)
    {
        if(nearZero(matrix[2].y()) != 0)
        {
            answers.setY(answers.y() - answers.z());
            matrix[1] = matrix[1] - matrix[2];
        }
        else if(nearZero(matrix[3].y()) != 0)
        {
            answers.setY(answers.y() - answers.w());
            matrix[1] = matrix[1] - matrix[3];
        }
    }

    answers.setX(answers.x()-(matrix[0].y()/matrix[1].y())*answers.y());
    matrix[0] = matrix[0] - (matrix[0].y()/matrix[1].y())*matrix[1];
    answers.setZ(answers.z()-(matrix[2].y()/matrix[1].y())*answers.y());
    matrix[2] = matrix[2] - (matrix[2].y()/matrix[1].y())*matrix[1];
    answers.setW(answers.w()-(matrix[3].y()/matrix[1].y())*answers.y());
    matrix[3] = matrix[3] - (matrix[3].y()/matrix[1].y())*matrix[1];

    if(nearZero(matrix[2].z()) == 0)
    {
        if(nearZero(matrix[3].z()) != 0)
        {
            answers.setZ(answers.z() - answers.w());
            matrix[2] = matrix[2] - matrix[3];
        }

    }

    answers.setX(answers.x()-(matrix[0].z()/matrix[2].z())*answers.z());
    matrix[0] = matrix[0] - (matrix[0].z()/matrix[2].z())*matrix[2];
    answers.setY(answers.y()-(matrix[1].z()/matrix[2].z())*answers.z());
    matrix[1] = matrix[1] - (matrix[1].z()/matrix[2].z())*matrix[2];
    answers.setW(answers.w()-(matrix[3].z()/matrix[2].z())*answers.z());
    matrix[3] = matrix[3] - (matrix[3].z()/matrix[2].z())*matrix[2];


    if(nearZero(matrix[3].w()) == 0)
    {
        qDebug()<<"broken";
        return QVector4D(0,0,0,0);
    }

    answers.setX(answers.x()-(matrix[0].w()/matrix[3].w())*answers.w());
    matrix[0] = matrix[0] - (matrix[0].w()/matrix[3].w())*matrix[3];
    answers.setY(answers.y()-(matrix[1].w()/matrix[3].w())*answers.w());
    matrix[1] = matrix[1] - (matrix[1].w()/matrix[3].w())*matrix[3];
    answers.setZ(answers.z()-(matrix[2].w()/matrix[3].w())*answers.w());
    matrix[2] = matrix[2] - (matrix[2].w()/matrix[3].w())*matrix[3];



    answers.setX(answers.x()/matrix[0].x());
    matrix[0] = matrix[0]/matrix[0].x();


    answers.setY(answers.y()/matrix[1].y());
    matrix[1] = matrix[1]/matrix[1].y();


    answers.setZ(answers.z()/matrix[2].z());
    matrix[2] = matrix[2]/matrix[2].z();


    answers.setW(answers.w()/matrix[3].w());
    matrix[3] = matrix[3]/matrix[3].w();

    return answers;
}

/// solves for the coordinate of a screen position, given the coefficient matrix,
// should give the correct line
// doesn't work at all - indeterminate z
QVector3D AMShapeDataSet::findCoordinate(QVector4D coefficients[], QVector3D screenPosition)
{

    QVector4D coeffTermOne = coefficients[0];
    QVector4D coeffTermTwo = coefficients[1];
    QVector4D coeffTermThree = coefficients[2];

    QVector4D matrix[3];
    matrix[0] = 2*coeffTermOne + coeffTermTwo + coeffTermThree;
    matrix[1] = coeffTermOne + 2*coeffTermTwo + coeffTermThree;
    matrix[2] = 6*coeffTermOne + coeffTermTwo + 2*coeffTermThree;

    QVector3D newAnswers;

    newAnswers.setX(2*screenPosition.x() + screenPosition.y() + screenPosition.z());
    newAnswers.setY(screenPosition.x() + 2*screenPosition.y() + screenPosition.z());
    newAnswers.setZ(6*screenPosition.x() + screenPosition.y() + 2*screenPosition.z());





    if(nearZero(matrix[0].x()) == 0)
    {
        if(!(nearZero(coeffTermOne.x()) == 0))
        {
            matrix[0] += coeffTermOne;
            newAnswers.setX(newAnswers.x()+screenPosition.x());
        }
        else if(!(nearZero(coeffTermTwo.x()) == 0))
        {
            matrix[0] -= coeffTermTwo;
            newAnswers.setX(newAnswers.x()-screenPosition.y());
        }
        else if(!(nearZero(coeffTermThree.x()) == 0))
        {
            matrix[0] -= coeffTermThree;
            newAnswers.setX(newAnswers.x()-screenPosition.z());
        }
        else
        {
            qDebug()<<"Not enough x's";
        }
    }
    if(nearZero(matrix[1].y()) == 0)
    {
        if(nearZero(coeffTermTwo.y()) != 0)
        {
            matrix[1] += coeffTermTwo;
            newAnswers.setY(newAnswers.y()+screenPosition.y());
        }
        else if(nearZero(coeffTermThree.y()) != 0)
        {
            matrix[1] -= coeffTermThree;
            newAnswers.setY(newAnswers.y()-screenPosition.z());
        }
        else if(nearZero(coeffTermOne.y()) != 0)
        {
            matrix[1] -= coeffTermOne;
            newAnswers.setY(newAnswers.y()-screenPosition.x());
        }
        else
        {
            qDebug()<<"Not enough y's";
        }
    }
    if(nearZero(matrix[2].z()) == 0)
    {
        if(!nearZero(coeffTermThree.z()) == 0)
        {
            matrix[2] += coeffTermThree;
            newAnswers.setZ(newAnswers.z()+screenPosition.z());
        }
        else if(!(nearZero(coeffTermOne.z()) == 0))
        {
            matrix[2] -= coeffTermOne;
            newAnswers.setZ(newAnswers.z()-screenPosition.x());
        }
        else if(!nearZero(coeffTermTwo.z()) == 0)
        {
            matrix[2] -= coeffTermTwo;
            newAnswers.setZ(newAnswers.z()-screenPosition.y());
        }
        else
        {
            qDebug()<<"Not enough z's";
        }
    }

    screenPosition = newAnswers;



    screenPosition.setY(screenPosition.y()-(matrix[1].x()/matrix[0].x())*screenPosition.x());
    matrix[1] = matrix[1] - (matrix[1].x()/matrix[0].x())*matrix[0];
    screenPosition.setZ(screenPosition.z()-(matrix[2].x()/matrix[0].x())*screenPosition.x());
    matrix[2] = matrix[2] - (matrix[2].x()/matrix[0].x())*matrix[0];


    if(nearZero(matrix[1].y()) == 0)
    {
        if(nearZero(matrix[2].y()) != 0)
        {
            screenPosition.setY(screenPosition.y() - screenPosition.z());
            matrix[1] = matrix[1] - matrix[2];
        }
    }

    screenPosition.setX(screenPosition.x()-(matrix[0].y()/matrix[1].y())*screenPosition.y());
    matrix[0] = matrix[0] - (matrix[0].y()/matrix[1].y())*matrix[1];
    screenPosition.setZ(screenPosition.z()-(matrix[2].y()/matrix[1].y())*screenPosition.y());
    matrix[2] = matrix[2] - (matrix[2].y()/matrix[1].y())*matrix[1];


    if(nearZero(matrix[2].z()) == 0)
    {
        qDebug()<<"broken";
//        return QVector3D(0,0,0);

    }

    screenPosition.setX(screenPosition.x()-(matrix[0].z()/matrix[2].z())*screenPosition.z());
    matrix[0] = matrix[0] - (matrix[0].z()/matrix[2].z())*matrix[2];
    screenPosition.setY(screenPosition.y()-(matrix[1].z()/matrix[2].z())*screenPosition.z());
    matrix[1] = matrix[1] - (matrix[1].z()/matrix[2].z())*matrix[2];






    screenPosition.setX(screenPosition.x()/matrix[0].x());
    matrix[0] = matrix[0]/matrix[0].x();
    screenPosition.setX(screenPosition.x() - matrix[0].w());



    screenPosition.setY(screenPosition.y()/matrix[1].y());
    matrix[1] = matrix[1]/matrix[1].y();
    screenPosition.setY(screenPosition.y() - matrix[1].w());


    screenPosition.setZ(screenPosition.z()/matrix[2].z());
    matrix[2] = matrix[2]/matrix[2].z();
    screenPosition.setZ(screenPosition.z() - matrix[2].w());



    return screenPosition;
}

void AMShapeDataSet::factorTransformMatrix(QVector3D coefficients[])
{
    QVector3D rowOne = coefficients[0];
    QVector3D rowTwo = coefficients[1];
    QVector3D rowThree = coefficients[2];

    double denominator = sqrt(pow(rowThree.z(),2.0) + pow(rowThree.y(),2.0));
    double c;
    double s;
    if((denominator) == 0)
    {
        c = 21;
        s = 17;
    }
    else
    {
        c = -1*rowThree.z()/denominator;
        s = rowThree.y()/denominator;
    }
    /// matrix stored by columns
    QVector3D rx[3];
    rx[0] = QVector3D(1, 0, 0);
    rx[1] = QVector3D(0, c, s);
    rx[2] = QVector3D(0, -1*s, c);

    QVector3D mPrime [3];
    for(int i = 0; i < 3; i++)
    {
        mPrime[i] = QVector3D(dot(coefficients[i],rx[0]),dot(coefficients[i],rx[1]), dot(coefficients[i],rx[2]));
    }

    double cPrime;
    double sPrime;
    cPrime = mPrime[3].z();
    sPrime = mPrime[3].x();

    /// matrix stored by columns
    QVector3D ry[3];
    ry[0] = QVector3D(cPrime, 0, -1*sPrime);
    ry[1] = QVector3D(0, 1, 0);
    ry[2] = QVector3D(sPrime, 0, cPrime);

    QVector3D mDoublePrime [3];
    for(int i = 0; i < 3; i++)
    {
        mDoublePrime[i] = QVector3D(dot(mPrime[i],ry[0]),dot(mPrime[i],ry[1]), dot(mPrime[i],ry[2]));
    }

    double cDoublePrime = mDoublePrime[2].y();
    double sDoublePrime = -1*mDoublePrime[2].x();

    /// matrix stored by columns
    QVector3D rz[3];
    rz[0] = QVector3D(cDoublePrime, sDoublePrime, 0);
    rz[1] = QVector3D(-1*sDoublePrime, cDoublePrime, 0);
    rz[2] = QVector3D(0, 0, 1);

    QVector3D kMatrix [3];
    for(int i = 0; i < 3; i++)
    {
        kMatrix[i] = QVector3D(dot(mDoublePrime[i],rz[0]),dot(mDoublePrime[i],rz[1]), dot(mDoublePrime[i],rz[2]));
    }

    /// r is rzTryTrxT
    // rz is in the correct state to multiply
    // ry just needs to negat the upper and bottom corners
    ry[0].setZ(ry[0].z()*-1);
    ry[2].setX(ry[2].x()*-1);

    QVector3D rzRy [3];
    for(int i = 0; i < 3; i++)
    {
        rzRy[i] = QVector3D(dot(rz[i],ry[0]),dot(rz[i],ry[1]), dot(rz[i],ry[2]));
    }

    //rx needs the s terms negated as well
    rx[2].setY(rx[2].y()*-1);
    rx[1].setZ(rx[1].z()*-1);

    QVector3D finalR [3];
    for(int i = 0; i < 3; i++)
    {
        finalR[i] = QVector3D(dot(rzRy[i],rx[0]),dot(rzRy[i],rx[1]), dot(rzRy[i],rx[2]));
    }

    for(int i = 0; i < 3; i++)
    {
        qDebug()<<kMatrix[i];
    }
    for(int i = 0; i < 3; i++)
    {
        qDebug()<<finalR[i];
    }

    /// kR = [a | b | c]
    /// uo = a.c/c.c
    QVector3D aTerm,bTerm, cTerm;
    aTerm = coefficients[0];
    bTerm = coefficients[1];
    cTerm = coefficients[2];

    double normal = dot(cTerm,cTerm);
    double uO = dot(aTerm,cTerm)/normal;
    double vO = dot(bTerm,cTerm)/normal;
    double beta = sqrt(dot(bTerm,bTerm)/normal-pow(vO,2.0));
    double gamma = ((dot(bTerm,aTerm)/normal)-uO*vO)/beta;
    double alpha = sqrt((dot(aTerm,aTerm)/normal)-pow(uO,2.0)-pow(gamma,2.0));

    qDebug()<<uO<<vO<<beta<<gamma<<alpha;






}

double AMShapeDataSet::dot(QVector3D a, QVector3D b)
{
    double value = QVector3D::dotProduct(a,b);
    return value;
}


MatrixXd AMShapeDataSet::directLinearTransform(QVector3D coordinate[], QPointF screenPosition[])
{


//        /// construct the 12x11 x matrix from coordinate and screen position
//    QGenericMatrix<12,11,double> matrix = constructMatrix(coordinate,screenposition,1);
//    /// transpose is an 11x12 matrix
//    QGenericMatrix<11,12,double> transpose = matrix.transposed();
//    /// w is an 11x11 matrix, start with identity
//    QGenericMatrix<12,12,double> wMatrix = constructWeightMatrix(1,1);
//    QGenericMatrix<11,12,double> leftInverse = wMatrix*transpose;
//    QGenericMatrix<11,11,double> inverse = matrix*leftInverse;
//    QVector<double> inverseData;
//    for(int i = 0; i<121; i++)
//    {
//        inverseData<<inverse.data()[i];
//    }
//    const int k = 11/2;
//    const int l = 11-k;
//    QGenericMatrix<11,11,double> leftTerm = QGenericMatrix<11,11,double>(invertMatrix(k,l,inverseData).data());

    /// construct the 12x11 x matrix from coordinate and screen position
    MatrixXd matrix = constructMatrix(coordinate,screenPosition);
    MatrixXd rhs(matrix.rows(),1);
    rhs.setZero(matrix.rows(),1);
    JacobiSVD<MatrixXd> svd(matrix,ComputeThinV|ComputeThinU);
    svd.compute(matrix,ComputeThinU|ComputeThinV);


    MatrixXd singularValues = svd.singularValues();
    MatrixXd matrixV = svd.matrixV();
    MatrixXd solution = matrixV.col(matrixV.cols()-1);

    MatrixXd test = matrix*solution;

    /// normalize it
    QVector3D rThree = QVector3D(solution(8,0),solution(9,0),solution(10,0));
    double factor = rThree.length();
//    solution = solution/factor;

    MatrixXd testTwo = matrix*solution;


    /// change solution into a 3x4 matrix
    /// resize goes column by column, and places column by column
    /// so have to resize to the transpose, then transpose to get the
    /// desired matrix
    solution.resize(4,3);
    solution.transposeInPlace();

    qDebug()<<"done";

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

        matrix(2*i,8) = screenposition[i].x()*coordinate[i].x();
        matrix(2*i,9) = screenposition[i].x()*coordinate[i].y();
        matrix(2*i,10) = screenposition[i].x()*coordinate[i].z();
        matrix(2*i,11) = screenposition[i].x();

        /// row two

        matrix(2*i + 1,0) = 0;
        matrix(2*i + 1,1) = 0;
        matrix(2*i + 1,2) = 0;
        matrix(2*i + 1,3) = 0;

        matrix(2*i + 1,4) = coordinate[i].x();
        matrix(2*i + 1,5) = coordinate[i].y();
        matrix(2*i + 1,6) = coordinate[i].z();
        matrix(2*i + 1,7) = 1;

        matrix(2*i + 1,8) = screenposition[i].y()*coordinate[i].x();
        matrix(2*i + 1,9) = screenposition[i].y()*coordinate[i].y();
        matrix(2*i + 1,10) = screenposition[i].y()*coordinate[i].z();
        matrix(2*i + 1,11) = screenposition[i].y();
    }
    return matrix;

}


QGenericMatrix<12,12,double> AMShapeDataSet::constructWeightMatrix(double sigmaU, double sigmaV)
{
    QVector<double> matrix;
    for(int i = 0; i < 11; i++)
    {
        for(int j = 0; j< 11; j++)
        {
            if(i == j)
            {
                // check for even
                if(2*(j/2) == j)
                {
                    matrix<<1.0/sigmaU;
                }
                else
                {
                    matrix<<1.0/sigmaV;
                }
            }
            else
            {
                matrix<<0;
            }
        }
    }
    QGenericMatrix<12,12,double> newMatrix = QGenericMatrix<12,12,double>(matrix.data());
    return newMatrix;
}

QVector<double> AMShapeDataSet::invertMatrix(const int k, const int l, QVector<double> matrix)
{
//    /// find the inverse of this matrix
//    if(matrix.isEmpty())
//    {
//        qDebug()<<"AMShapeDataSet::invertMatrix - error, cannot invert null matrix";
//        return matrix;
//    }
//    if(a != b)
//    {
//        qDebug()<<"Attempting to invert a non-square matrix";
//        return matrix;
//    }
//    if(a<2)
//    {
//        if(a == 1)
//        {
//            QVector<double> newMatrix;
//            newMatrix[0] = 1.0/matrix[0];
//            return newMatrix;
//        }
//        else
//        {
//            qDebug()<<"AMShapeDataSet::invertMatrix - error, attempting to invert matrix with size less than 1";
//            return matrix;
//        }
//    }
//    // construct the submatrices a, b, c, and d

//    QVector<double> aMatrix;
//    for(int i = 0; i < k; i++)
//    {
//        for(int j = 0; j < k; j++)
//        {
//            aMatrix[k*i+j]=matrix[a*i+j];
//        }
//    }
//    QVector<double> bMatrix;
//    for(int i = 0; i < k; i++)
//    {
//        for(int j = 0; j < l; j++)
//        {
//            bMatrix[l*i+j] = matrix[a*i+j+k];
//        }
//    }
//    QVector<double>  cMatrix;
//    for(int i = 0; i < l; i++)
//    {
//        for(int j = 0; j < k; j++)
//        {
//            cMatrix[k*i+j] = matrix[a*(i+k)+j];
//        }
//    }
//    QVector<double> dMatrix;
//    for(int i = 0; i < l; i++)
//    {
//        for(int j = 0; j < l; j++)
//        {
//            dMatrix[l*i+j] = matrix[a*(i+k)+j+k];
//        }
//    }
//    QGenericMatrix<1,1,double> matrixA;
//    template<k,k,1,1>getMatrix(&matrixA);
//    QGenericMatrix<k,l,double> matrixB;
//    QGenericMatrix<l,k,double> matrixC;
//    QGenericMatrix<l,l,double> matrixD;
//    matrixA = QGenericMatrix<k,k,double>(aMatrix);
//    matrixB = QGenericMatrix<k,l,double>(bMatrix);
//    matrixC = QGenericMatrix<l,k,double>(cMatrix);
//    matrixD = QGenericMatrix<l,l,double>(dMatrix);
//    QGenericMatrix<k,k,double> aInverse = QGenericMatrix<k,k,double>(invertMatrix(k,k,aMatrix).data());

//    QGenericMatrix<l,l,double> dCABMatrix = matrixD-(matrixB*aInverse*matrixC);
//    QGenericMatrix<l,l,double> dCABInverse = QGenericMatrix<l,l,double>(invertMatrix(l,l,dCABMatrix.data()).data());

//    QGenericMatrix<k,k,double> matrixOne;
//    QGenericMatrix<k,l,double> matrixTwo;
//    QGenericMatrix<l,k,double> matrixThree;
//    QGenericMatrix<l,l,double> matrixFour;
//    matrixOne = aInverse + aInverse*matrixC*dCABInverse*matrixB*aInverse;
//    matrixTwo = -1*(dCABInverse*matrixB*aInverse);
//    matrixThree = -1*(aInverse*matrixC*dCABInverse);
//    matrixFour = dCABInverse;


//    QVector<double> partOne, partTwo, partThree, partFour;
//    partOne = QVector<double>(k*k,matrixOne.data());
//    partTwo = QVector<double>(k*l,matrixTwo.data());
//    partThree = QVector<double>(k*l,matrixThree.data());
//    partFour = QVector<double>(l*l,matrixFour.data());
//    QVector<double> fullMatrix;
//    fullMatrix<<partOne<<partTwo<<partThree<<partFour;

}

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

MatrixXd AMShapeDataSet::rotationParameters(MatrixXd matrixA, MatrixXd matrixB)
{
    MatrixXd matrixR = matrixA.inverse()*matrixB;
    return matrixR;
}

MatrixXd AMShapeDataSet::translationParameters(MatrixXd matrixA, MatrixXd matrixSubB)
{
    MatrixXd matrixT = matrixA.inverse()*matrixSubB;
    return matrixT;
}



