#ifndef AMCAMERACONFIGURATIONMODEL_H
#define AMCAMERACONFIGURATIONMODEL_H

#include <QVector3D>

#include <Eigen/Core>

#include "dataman/database/AMDbObject.h"

class QVector3D;

using namespace Eigen;
class AMCameraConfiguration : public AMDbObject
{
    Q_OBJECT
    Q_PROPERTY(double cameraFOV READ cameraFOV WRITE setCameraFOV)
    Q_PROPERTY(double cameraFocalLength READ cameraFocalLength WRITE setCameraFocalLength)
    Q_PROPERTY(double cameraDistortion READ cameraDistortion WRITE setCameraDistortion)
    Q_PROPERTY(QVector3D cameraCentre READ cameraCentre WRITE setCameraCentre)
    Q_PROPERTY(QVector3D cameraPosition READ cameraPosition WRITE setCameraPosition)
    Q_PROPERTY(double cameraRotation READ cameraRotation WRITE setCameraRotation)
    Q_PROPERTY(double pixelAspectRatio READ pixelAspectRatio WRITE setPixelAspectRatio)
//    Q_PROPERTY(MatrixXd cameraMatrix READ cameraMatrix WRITE setCameraMatrix)
//    else if(columnType == qMetaTypeId<MatrixXd>())
//    {
//        MatrixXd val = property(columnName).value<MatrixXd();

//    }

public:
    explicit AMCameraConfiguration(QObject *parent = 0);

    /// Accessors
    double cameraPositionX();
    double cameraPositionY();
    double cameraPositionZ();
    QVector3D cameraPosition();

    double cameraCentreX();
    double cameraCentreY();
    double cameraCentreZ();
    QVector3D cameraCentre();

    double cameraFOV();
    double cameraFocalLength();
    double cameraDistortion();

    double cameraRotation();

    double pixelAspectRatio();

    QPointF imageCentre();

    MatrixXd cameraMatrix();

    virtual bool loadFromDb(AMDatabase *db, int id);

    /// Mutators
public slots:
    void setCameraPosition(QVector3D position);
    void setCameraPositionX(double x);
    void setCameraPositionY(double y);
    void setCameraPositionZ(double z);

    void setCameraCentre(QVector3D centre);
    void setCameraCentreX(double x);
    void setCameraCentreY(double y);
    void setCameraCentreZ(double z);

    void setCameraFOV(double fov);
    void setCameraFocalLength(double focalLength);
    void setCameraDistortion(double distortion);

    void setCameraRotation(double rotation);

    void setPixelAspectRatio(double pixelAspectRatio);

    void setImageCentre(QPointF imageCentre);
    void setImageCentreX(double imageCentreX);
    void setImageCentreY(double imageCentreY);

    void setCameraMatrix(MatrixXd cameraMatrix);



protected:
    QVector3D cameraPosition_;
    QVector3D cameraCentre_;
    double cameraFOV_;
    double cameraFocalLength_;
    double cameraDistortion_;
    double cameraRotation_;
    double pixelAspectRatio_;
    QPointF imageCentre_;
    MatrixXd cameraMatrix_;
};

#endif // AMCAMERACONFIGURATIONMODEL_H
