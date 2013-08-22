#ifndef AMCAMERACONFIGURATIONMODEL_H
#define AMCAMERACONFIGURATIONMODEL_H

#include <QVector3D>

#include <Eigen/Core>

#include "dataman/database/AMDbObject.h"

class QVector3D;
//typedef QVector<QVector3D> AMQVector3DVector;


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
    Q_PROPERTY(AMQVector3DVector cameraMatrix READ cameraMatrix WRITE setCameraMatrix)



public:
    explicit AMCameraConfiguration(QObject *parent = 0);

    /// Accessors

    /// the 3D location of the camera in world space
    double cameraPositionX();
    double cameraPositionY();
    double cameraPositionZ();
    QVector3D cameraPosition();

    /// the 3D location of a point one unit along the camera's line of vision
    /// in world space.  Note that if the imageCentre is not 0,0 that this does not line up
    /// with the centre of the screen
    double cameraCentreX();
    double cameraCentreY();
    double cameraCentreZ();
    QVector3D cameraCentre();

    /// field of view of the camera, as an angle (angle of view)
    double cameraFOV();
    /// the focal length of the camera.
    /// note that changing this parameter will have no effect on the way things
    /// are drawn, as it is cancelled by the FOV
    double cameraFocalLength();
    /// the radial distortion of the camera
    /// negative distortion creates barrel effect
    /// positiove distortion creates pincushion effect
    double cameraDistortion();

    /// the rotation of the camera about it's line of vision
    double cameraRotation();

    /// the aspect ratio of pixel height and width
    double pixelAspectRatio();

    /// the centre of the projected image
    QPointF imageCentre();

    /// the camera Matrix, in a form suitable to save in the database
    QVector<QVector3D> cameraMatrix();

    /// the camera matrix, in matrix form (MatrixXd from Eigen/Core)
    MatrixXd cameraMatrixToMatrix();

    /// has the matrix been calculated (can it be used, or does it need to be calibrated?)
    bool hasMatrix();


    /// loads the configuration from the database
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

    /// sets the matrix as a QVector<QVector3D> - each QVector3D is a column, there should be four
    void setCameraMatrix(QVector<QVector3D> cameraMatrix);
    /// sets the matrix using a MatrixXd
    void setCameraMatrixFromMatrix(MatrixXd cameraMatrix);


protected:
    QVector3D cameraPosition_;
    QVector3D cameraCentre_;
    double cameraFOV_;
    double cameraFocalLength_;
    double cameraDistortion_;
    double cameraRotation_;
    double pixelAspectRatio_;
    QPointF imageCentre_;
    QVector<QVector3D> cameraMatrix_;
    /// the matrix form of cameraMatrix
    /// calculated form setCameraMatrixFromMatrix
    MatrixXd cameraTransformMatrix_;
    bool matrixCalculated_;
};

#endif // AMCAMERACONFIGURATIONMODEL_H
