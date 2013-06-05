#ifndef AMCAMERACONFIGURATIONMODEL_H
#define AMCAMERACONFIGURATIONMODEL_H

#include <QVector3D>
#include "dataman/database/AMDbObject.h"

class QVector3D;

class AMCameraConfiguration : public AMDbObject
{
    Q_OBJECT
    Q_PROPERTY(double cameraFOV READ cameraFOV WRITE setCameraFOV)
    Q_PROPERTY(double cameraFocalLength READ cameraFocalLength WRITE setCameraFocalLength)
    Q_PROPERTY(double cameraDistortion READ cameraDistortion WRITE setCameraDistortion)
    Q_PROPERTY(QVector3D cameraCenter READ cameraCenter WRITE setCameraCenter)
    Q_PROPERTY(QVector3D cameraPosition READ cameraPosition WRITE setCameraPosition)
    Q_PROPERTY(double cameraRotation READ cameraRotation WRITE setCameraRotation)
    Q_PROPERTY(double pixelAspectRatio READ pixelAspectRatio WRITE setPixelAspectRatio)

public:
    explicit AMCameraConfiguration(QObject *parent = 0);

    /// Accessors
    double cameraPositionX();
    double cameraPositionY();
    double cameraPositionZ();
    QVector3D cameraPosition();

    double cameraCenterX();
    double cameraCenterY();
    double cameraCenterZ();
    QVector3D cameraCenter();

    double cameraFOV();
    double cameraFocalLength();
    double cameraDistortion();

    double cameraRotation();

    double pixelAspectRatio();

    virtual bool loadFromDb(AMDatabase *db, int id);

    /// Mutators
public slots:
    void setCameraPosition(QVector3D position);
    void setCameraPositionX(double x);
    void setCameraPositionY(double y);
    void setCameraPositionZ(double z);

    void setCameraCenter(QVector3D center);
    void setCameraCenterX(double x);
    void setCameraCenterY(double y);
    void setCameraCenterZ(double z);

    void setCameraFOV(double fov);
    void setCameraFocalLength(double focalLength);
    void setCameraDistortion(double distortion);

    void setCameraRotation(double rotation);

    void setPixelAspectRatio(double pixelAspectRatio);



protected:
    QVector3D cameraPosition_;
    QVector3D cameraCenter_;
    double cameraFOV_;
    double cameraFocalLength_;
    double cameraDistortion_;
    double cameraRotation_;
    double pixelAspectRatio_;
};

#endif // AMCAMERACONFIGURATIONMODEL_H
