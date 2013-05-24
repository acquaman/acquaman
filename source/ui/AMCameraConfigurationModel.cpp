#include "AMCameraConfigurationModel.h"
#include "dataman/database/AMDbObject.h"

#include <QVector3D>


AMCameraConfigurationModel::AMCameraConfigurationModel(QObject *parent) :
    AMDbObject(parent)
{
    cameraRotation_ = 0;
}

double AMCameraConfigurationModel::cameraPositionX()
{
    return cameraPosition_.x();
}

double AMCameraConfigurationModel::cameraPositionY()
{
    return cameraPosition_.y();
}

double AMCameraConfigurationModel::cameraPositionZ()
{
    return cameraPosition_.z();
}

QVector3D AMCameraConfigurationModel::cameraPosition()
{
    return cameraPosition_;
}

double AMCameraConfigurationModel::cameraCenterX()
{
    return cameraCenter_.x();
}

double AMCameraConfigurationModel::cameraCenterY()
{
    return cameraCenter_.y();
}

double AMCameraConfigurationModel::cameraCenterZ()
{
    return cameraCenter_.z();
}

QVector3D AMCameraConfigurationModel::cameraCenter()
{
    return cameraCenter_;
}

double AMCameraConfigurationModel::cameraFOV()
{
    return cameraFOV_;
}

double AMCameraConfigurationModel::cameraFocalLength()
{
    return cameraFocalLength_;
}

double AMCameraConfigurationModel::cameraDistortion()
{
    return cameraDistortion_;
}

double AMCameraConfigurationModel::cameraRotation()
{
    return cameraRotation_;
}

void AMCameraConfigurationModel::setCameraPosition(QVector3D position)
{
    cameraPosition_ = position;
}

void AMCameraConfigurationModel::setCameraPositionX(double x)
{
    cameraPosition_.setX(x);
}

void AMCameraConfigurationModel::setCameraPositionY(double y)
{
    cameraPosition_.setY(y);
}

void AMCameraConfigurationModel::setCameraPositionZ(double z)
{
    cameraPosition_.setZ(z);
}

void AMCameraConfigurationModel::setCameraCenter(QVector3D center)
{
    cameraCenter_ = center;
}

void AMCameraConfigurationModel::setCameraCenterX(double x)
{
    cameraCenter_.setX(x);
}

void AMCameraConfigurationModel::setCameraCenterY(double y)
{
    cameraCenter_.setY(y);
}

void AMCameraConfigurationModel::setCameraCenterZ(double z)
{
    cameraCenter_.setZ(z);
}

void AMCameraConfigurationModel::setCameraFOV(double fov)
{
    cameraFOV_ = fov;
}

void AMCameraConfigurationModel::setCameraFocalLength(double focalLength)
{
    cameraFocalLength_ = focalLength;
}

void AMCameraConfigurationModel::setCameraDistortion(double distortion)
{
    cameraDistortion_ = distortion;
}

void AMCameraConfigurationModel::setCameraRotation(double rotation)
{
    cameraRotation_ = rotation;
}
