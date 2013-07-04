#include "AMCameraConfiguration.h"
#include "dataman/database/AMDbObject.h"

#include <QVector3D>


AMCameraConfiguration::AMCameraConfiguration(QObject *parent) :
    AMDbObject(parent)
{
    cameraRotation_ = 0;
    imageCentre_ = QPointF(0,0);
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

double AMCameraConfiguration::cameraCentreX()
{
    return cameraCentre_.x();
}

double AMCameraConfiguration::cameraCentreY()
{
    return cameraCentre_.y();
}

double AMCameraConfiguration::cameraCentreZ()
{
    return cameraCentre_.z();
}

QVector3D AMCameraConfiguration::cameraCentre()
{
    return cameraCentre_;
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

double AMCameraConfiguration::pixelAspectRatio()
{
    return pixelAspectRatio_;
}

QPointF AMCameraConfiguration::imageCentre()
{
    return imageCentre_;
}

#include <QDebug>
bool AMCameraConfiguration::loadFromDb(AMDatabase *db, int id)
{
    return AMDbObject::loadFromDb(db, id);
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

void AMCameraConfiguration::setCameraCentre(QVector3D centre)
{
    cameraCentre_ = centre;
}

void AMCameraConfiguration::setCameraCentreX(double x)
{
    cameraCentre_.setX(x);
}

void AMCameraConfiguration::setCameraCentreY(double y)
{
    cameraCentre_.setY(y);
}

void AMCameraConfiguration::setCameraCentreZ(double z)
{
    cameraCentre_.setZ(z);
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

void AMCameraConfiguration::setPixelAspectRatio(double pixelAspectRatio)
{
    pixelAspectRatio_ = pixelAspectRatio;
}

void AMCameraConfiguration::setImageCentre(QPointF imageCentre)
{
    imageCentre_ = imageCentre;
}

void AMCameraConfiguration::setImageCentreX(double imageCentreX)
{
    imageCentre_.setX(imageCentreX);
}

void AMCameraConfiguration::setImageCentreY(double imageCentreY)
{
    imageCentre_.setY(imageCentreY);
}
