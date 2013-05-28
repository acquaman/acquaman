#include "AMCameraConfiguration.h"
#include "dataman/database/AMDbObject.h"

#include <QVector3D>


AMCameraConfiguration::AMCameraConfiguration(QObject *parent) :
    AMDbObject(parent)
{
    cameraRotation_ = 0;
}

double AMCameraConfiguration::cameraPositionX()
{
    return cameraPosition_.x();
}

double AMCameraConfiguration::cameraPositionY()
{
    return cameraPosition_.y();
}

double AMCameraConfiguration::cameraPositionZ()
{
    return cameraPosition_.z();
}

QVector3D AMCameraConfiguration::cameraPosition()
{
    return cameraPosition_;
}

double AMCameraConfiguration::cameraCenterX()
{
    return cameraCenter_.x();
}

double AMCameraConfiguration::cameraCenterY()
{
    return cameraCenter_.y();
}

double AMCameraConfiguration::cameraCenterZ()
{
    return cameraCenter_.z();
}

QVector3D AMCameraConfiguration::cameraCenter()
{
    return cameraCenter_;
}

double AMCameraConfiguration::cameraFOV()
{
    return cameraFOV_;
}

double AMCameraConfiguration::cameraFocalLength()
{
    return cameraFocalLength_;
}

double AMCameraConfiguration::cameraDistortion()
{
    return cameraDistortion_;
}

double AMCameraConfiguration::cameraRotation()
{
    return cameraRotation_;
}

void AMCameraConfiguration::setCameraPosition(QVector3D position)
{
    cameraPosition_ = position;
}

void AMCameraConfiguration::setCameraPositionX(double x)
{
    cameraPosition_.setX(x);
}

void AMCameraConfiguration::setCameraPositionY(double y)
{
    cameraPosition_.setY(y);
}

void AMCameraConfiguration::setCameraPositionZ(double z)
{
    cameraPosition_.setZ(z);
}

void AMCameraConfiguration::setCameraCenter(QVector3D center)
{
    cameraCenter_ = center;
}

void AMCameraConfiguration::setCameraCenterX(double x)
{
    cameraCenter_.setX(x);
}

void AMCameraConfiguration::setCameraCenterY(double y)
{
    cameraCenter_.setY(y);
}

void AMCameraConfiguration::setCameraCenterZ(double z)
{
    cameraCenter_.setZ(z);
}

void AMCameraConfiguration::setCameraFOV(double fov)
{
    cameraFOV_ = fov;
}

void AMCameraConfiguration::setCameraFocalLength(double focalLength)
{
    cameraFocalLength_ = focalLength;
}

void AMCameraConfiguration::setCameraDistortion(double distortion)
{
    cameraDistortion_ = distortion;
}

void AMCameraConfiguration::setCameraRotation(double rotation)
{
    cameraRotation_ = rotation;
}
