#include "AMCamera.h"

#include <QVector3D>
#include <QPointF>
#include <QDebug>

#include "AMCameraConfiguration.h"

AMCamera::AMCamera(QObject* parent)
    : QObject(parent)
{
    cameraConfiguration_ = new AMCameraConfiguration();
    calibrationRun_ = false;
    useCameraMatrix_ = false;
}

AMCameraConfiguration *AMCamera::cameraConfiguration() const
{
    return cameraConfiguration_;
}

void AMCamera::setCameraConfiguration(AMCameraConfiguration *cameraConfiguration)
{
    if(cameraConfiguration_ != cameraConfiguration)
        cameraConfiguration_ = cameraConfiguration;
    calibrationRun_ = cameraConfiguration_->hasMatrix();
}

void AMCamera::getTransforms(QPointF points[], QVector3D coordinates[])
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
    cameraConfiguration_->setCameraMatrixFromMatrix(matrixA*matrixExtrinsic);
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
    pixelAspectRatio = cameraConfiguration_->pixelAspectRatio();
    double rotation;
    QVector3D cameraDirection = cameraCenter - cameraPosition;
    // the rotation is
    // (xyz)/|xyz|*(y/|y|-1)/-2*pi-xyz
    double product = cameraDirection.x()*cameraDirection.y()*cameraDirection.z();
    double piFactor = (product/fabs(product))*(cameraDirection.y()/fabs(cameraDirection.y())-1)/-2;
    rotation = piFactor*M_PI-product;



    double fov = atan(1/(2*focalLength));
    cameraConfiguration_->setCameraCentre(cameraCenter);
    cameraConfiguration_->setCameraPosition(cameraPosition);
    cameraConfiguration_->setCameraFocalLength(focalLength);
    cameraConfiguration_->setCameraFOV(fov);
    cameraConfiguration_->setPixelAspectRatio(pixelAspectRatio);
    cameraConfiguration_->setImageCentre(QPointF(uO,vO));
    cameraConfiguration_->setCameraRotation(rotation);


}

QVector3D AMCamera::transform2Dto3D(QPointF point, double depth) const
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
    QVector3D shift = cameraConfiguration_->cameraPosition();
    QVector3D actualCameraCenter = cameraConfiguration_->cameraCentre();
    double focalLength = cameraConfiguration_->cameraFocalLength();
    double fieldOfView = cameraConfiguration_->cameraFOV();
    double aspectRatio = scaledSize().height()/scaledSize().width();
    double pixelAspectRatio = cameraConfiguration_->pixelAspectRatio();
    double fov = 2*tan(fieldOfView)*focalLength;
    double theta;
    double oldRotation;
    point -= cameraConfiguration_->imageCentre();

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
    if((cameraConfiguration_->cameraRotation() != 0 || naturalRotation != 0)&& (coordinate.x() != 0 || coordinate.y() != 0))
    {
        double z = coordinate.z();
        coordinate.setZ(0);
        double radius = coordinate.length();
        oldRotation = acos(coordinate.x()/radius);
        if(coordinate.y() < 0) oldRotation*= -1;
        double rotation = cameraConfiguration_->cameraRotation()*-1;
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

QPointF AMCamera::transform3Dto2D(QVector3D coordinate) const
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


    double rotation = cameraConfiguration_->cameraRotation();
    double focalLength = cameraConfiguration_->cameraFocalLength();
    QVector3D cameraPosition = cameraConfiguration_->cameraPosition();
    QVector3D cameraCenter = cameraConfiguration_->cameraCentre();
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

    double fieldOfView = cameraConfiguration_->cameraFOV();
    double aspectRatio = (scaledSize().height())/(scaledSize().width());
    double pixelAspectRatio = cameraConfiguration_->pixelAspectRatio();
    double fov = 2*tan(fieldOfView)*focalLength;


    // find the scaled point location, and shift to the center of the screen
    position = (focalLength/zLength)/fov*newPosition.toPointF();
    position.setY(position.y()/aspectRatio/pixelAspectRatio);
    position += QPointF(0.5,0.5);
    position += cameraConfiguration_->imageCentre();
    return position;
}

bool AMCamera::useCameraMatrix() const
{
    return useCameraMatrix_;
}

bool AMCamera::calibrationRun() const
{
    return calibrationRun_;
}

void AMCamera::setUseCameraMatrix(bool useCameraMatrix)
{
    useCameraMatrix_ = useCameraMatrix;
}

void AMCamera::setCalibrationRun(bool calibrationRun)
{
    calibrationRun_ = calibrationRun;
}

QSizeF AMCamera::viewSize() const
{
    return viewSize_;
}

QSizeF AMCamera::scaledSize() const
{
    return scaledSize_;
}

void AMCamera::setViewSize(QSizeF viewSize)
{
    viewSize_ = viewSize;
}

void AMCamera::setScaledSize(QSizeF scaledSize)
{
    scaledSize_ = scaledSize;
}

double AMCamera::focalLength() const
{
//    if(calibrationRun_ && useCameraMatrix_)
//    {
//        return 1;
//    }
    return cameraConfiguration()->cameraFocalLength();
}

QPointF AMCamera::distortPoint(QPointF point) const
{
    /// Distortion is given by:
    // newRadius = oldRadius*(1+k1*oldRadius^2)
    /// where k1 is the distortion  factor
    double distortionFactor = cameraConfiguration_->cameraDistortion();
    QPointF newPoint;
    double x = point.x()-0.5;
    double y = point.y()-0.5;

    newPoint.setX((x*(1+distortionFactor*(x*x+y*y)))+0.5);
    newPoint.setY((y*(1+distortionFactor*(x*x+y*y)))+0.5);
    return newPoint;
}

QPointF AMCamera::undistortPoint(QPointF point) const
{
    /// to remove distortion have to solve the equation for distorting the point
    /// for the original radius
    /// ie: distortedRadius = k1*r^3 + r -- solve for r
    /// description of method below

    ///this does not work at the center, so ignore it (it doesn't move)
    if(point.x() == 0.5 && point.y() == 0.5)
        return point;
    // set up
    double distortionFactor = cameraConfiguration_->cameraDistortion();
    if(distortionFactor == 0)
        return point;
    QPointF newPoint;
    double x = point.x()-0.5;
    double y = point.y()-0.5;

    double distortedRadius = x*x + y*y;
    distortedRadius = pow(distortedRadius,0.5);

    double wo;
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

/// returns the dot product of two vectors
double AMCamera::dot(QVector3D a, QVector3D b) const
{
    double value = QVector3D::dotProduct(a,b);
    return value;
}


QVector3D AMCamera::transform2Dto3DMatrix(QPointF point, double depth) const
{
    bool isCentred = false;
    if(point == QPointF(0.5,0.5))
    {
        isCentred = true;
        qDebug()<<"AMCamera::transform2Dto3DMatrix - Is centered";
    }

    /// translate point to coordinate system
    point = point - QPointF(0.5,0.5);

    if(isCentred)
        point += QPointF(0.1,0.1);

    MatrixXd matrixPoint (3,1);
    matrixPoint<<point.x(),point.y(),1;
    MatrixXd matrixP = cameraConfiguration()->cameraMatrixToMatrix();

    MatrixXd coordinate = matrixP.colPivHouseholderQr().solve(matrixPoint);
    coordinate /= coordinate(3,0);
    QVector3D location = QVector3D(coordinate(0,0),coordinate(1,0),coordinate(2,0));

    MatrixXd matrixPointTwo (3,1);
    matrixPointTwo<<2000*point.x(),2000*point.y(),2000;
    MatrixXd coordinateTwo;
    JacobiSVD<MatrixXd> svdOfMatrixP(matrixP);
    svdOfMatrixP.compute(matrixP, ComputeThinU|ComputeThinV);
    coordinateTwo = svdOfMatrixP.solve(matrixPointTwo);
    coordinateTwo /= coordinateTwo(3,0);
    QVector3D locationTwo = QVector3D(coordinateTwo(0,0),coordinateTwo(1,0),coordinateTwo(2,0));



    QPointF centrePoint = QPointF(0,0);
    MatrixXd centrePointOne (3,1);
    MatrixXd centrePointTwo (3,1);
    centrePointOne<<centrePoint.x(),centrePoint.y(),1;
    centrePointTwo<<0,0,100;

    MatrixXd centreCoordinateOne = matrixP.colPivHouseholderQr().solve(centrePointOne);
    svdOfMatrixP.compute(matrixP, ComputeThinU|ComputeThinV);
    MatrixXd centreCoordinateTwo = svdOfMatrixP.solve(centrePointTwo);
    centreCoordinateOne /= centreCoordinateOne(3,0);
    centreCoordinateTwo /= centreCoordinateTwo(3,0);

    QVector3D centreLocationOne = QVector3D(centreCoordinateOne(0,0),centreCoordinateOne(1,0), centreCoordinateOne(2,0));
    QVector3D centreLocationTwo = QVector3D(centreCoordinateTwo(0,0),centreCoordinateTwo(1,0), centreCoordinateTwo(2,0));



    double pointT = (locationTwo - location).length();
    QVector3D parameters = lineParameters(location, locationTwo, pointT);

    double centreT = (centreLocationTwo - centreLocationOne).length();
    QVector3D centreParameters = lineParameters(centreLocationOne, centreLocationTwo,centreT);

    QVector3D centrePointsVector = location - centreLocationOne;
    MatrixXd centrePointMatrix (3,1);
    centrePointMatrix<<centrePointsVector.x(),centrePointsVector.y(),centrePointsVector.z();
    MatrixXd parameterMatrix (3,2);
    parameterMatrix<<centreParameters.x(),-1*parameters.x(),
            centreParameters.y(),-1*parameters.y(),
            centreParameters.z(),-1*parameters.z();
    /// svd solution of this should give [tc , t]
    /// where t is the parameter of the location line
    /// and tc is the parameter of the centre line.
    /// putting in the solution should give the same point, which should be
    /// the camera position.

    JacobiSVD<MatrixXd> solver(parameterMatrix);
    solver.compute(parameterMatrix, ComputeThinU|ComputeThinV);
    MatrixXd solution = solver.solve(centrePointMatrix);


    double coordinateT = solution(1);
    QVector3D calculatedPositionOne = location + coordinateT*parameters;

    QVector3D desiredPoint;
    QVector3D cameraCentre = centreLocationOne - calculatedPositionOne;
    cameraCentre.normalize();

    if(!isCentred)
    {

        QVector3D locationDirection = location - calculatedPositionOne;
        locationDirection.normalize();
        double cosTheta = dot(cameraCentre,locationDirection);
        double length = depth/cosTheta;
        locationDirection *= length;
        desiredPoint = calculatedPositionOne + locationDirection;

    }
    else
    {
        desiredPoint = calculatedPositionOne + cameraCentre*depth;
    }

    return desiredPoint;




//    QVector3D worldDirection = location - cameraConfiguration()->cameraPosition();
//    worldDirection.normalize();
//    QVector3D centre = cameraConfiguration()->cameraCentre()-cameraConfiguration()->cameraPosition();
//    worldDirection *= depth/dot(worldDirection,centre);
//    worldDirection += cameraConfiguration()->cameraPosition();

//    qDebug()<<worldDirection;
//    return worldDirection;
}

QPointF AMCamera::transform3Dto2DMatrix(QVector3D coordinate) const
{
    MatrixXd worldCoordinate(4,1);
    worldCoordinate<<coordinate.x(),coordinate.y(), coordinate.z(),1;
    MatrixXd screenPosition = cameraConfiguration()->cameraMatrixToMatrix()*worldCoordinate;
    screenPosition /= screenPosition(2,0);
    return QPointF(screenPosition(0,0)+0.5,screenPosition(1,0)+0.5);
}



MatrixXd AMCamera::directLinearTransform(QVector3D coordinate[], QPointF screenPosition[]) const
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

    /// change solution into a 3x4 matrix
    /// resize goes column by column, and places column by column
    /// so have to resize to the transpose, then transpose to get the
    /// desired matrix
    solution.resize(4,3);
    solution.transposeInPlace();


    return solution;

}

MatrixXd AMCamera::constructMatrix(QVector3D coordinate[], QPointF screenposition[]) const
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

MatrixXd AMCamera::intrinsicParameters(MatrixXd matrixB) const
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

MatrixXd AMCamera::rotationParameters(MatrixXd matrixA, MatrixXd matrixB) const
{
    MatrixXd matrixR = matrixA.inverse()*matrixB;
    return matrixR;
}

MatrixXd AMCamera::translationParameters(MatrixXd matrixA, MatrixXd matrixSubB) const
{
    MatrixXd matrixT = matrixA.inverse()*matrixSubB;
    return matrixT;
}

MatrixXd AMCamera::findWorldCoordinate(MatrixXd matrix, MatrixXd extrinsicMatrix) const
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

bool AMCamera::notEqual(double a, double b, double tolerance) const
{
    double percent = fabs((a-b)/a);
    return percent>tolerance;
}

double AMCamera::nearZero(double a, double tolerance) const
{
    if(fabs(a)<tolerance)
    {
        return 0;
    }
    else return a;
}

double AMCamera::absError(double a, double b, double tolerance) const
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

QVector3D AMCamera::lineParameters(QVector3D pointOne, QVector3D pointTwo, double t) const
{
    double a,b,c;
    a = (pointTwo.x() - pointOne.x())/t;
    b = (pointTwo.y() - pointOne.y())/t;
    c = (pointTwo.z() - pointOne.z())/t;
    return QVector3D(a,b,c);
}

