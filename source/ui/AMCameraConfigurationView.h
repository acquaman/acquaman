#ifndef AMCAMERACONFIGURATIONVIEW_H
#define AMCAMERACONFIGURATIONVIEW_H

#include <QWidget>
#include <QVector3D>

#define MATRIXSIZE 12

class QLineEdit;
class QPushButton;
class AMCameraConfiguration;
class QComboBox;
class QSlider;
class AMBeamConfigurationView;
class QFrame;

/// This class provides a configuration window for model parameters in the Camera Browser
class AMCameraConfigurationView : public QWidget
{
    Q_OBJECT
public:
    AMCameraConfigurationView(AMCameraConfiguration *cameraConfiguration = 0, QWidget *parent = 0);

    /// Accessors
    double positionX();
    double positionY();
    double positionZ();
    double centerX();
    double centerY();
    double centerZ();
    double cameraFOV();
    double cameraFocalLength();
    double cameraDistortion();
    double cameraRotation();
    double pixelAspectRatio();
    double centreOffsetX();
    double centreOffsetY();

    /// Mutators
    void setPositionX(double x);
    void setPositionY(double y);
    void setPositionZ(double z);
    void setCenterX(double x);
    void setCenterY(double y);
    void setCenterZ(double z);
    void setCameraFOV(double fov);
    void setCameraFocalLength(double focalLength);
    void setCameraDistortion(double distortion);
    void setCameraRotation(double rotation);
    void setPixelAspectRatio(double pixelAspectRatio);
    void setCentreOffsetX(double centreOffsetX);
    void setCentreOffsetY(double centreOffsetY);

public slots:

    void updateAll();
    void hideCameraParameters(bool hide);

signals:
    void update(AMCameraConfiguration*);

protected slots:
    void updatePositionX(QString x);
    void updatePositionY(QString y);
    void updatePositionZ(QString z);

    void updateCenterX(QString x);
    void updateCenterY(QString y);
    void updateCenterZ(QString z);

    void updateFOV(QString fov);
    void updateFocalLength(QString focalLength);

    void updateDistortion(QString distortion);

    void updateRotation(QString rotation);
    void updateRotationSlider(int);

    void updatePixelAspectRatio(QString pixelAspectRatio);

    void updateCentreOffsetX(QString centreOffsetX);
    void updateCentreOffsetY(QString centreOffsetY);

    void updateName(QString name);
    void updateSelection(int);
    void saveConfiguration();
    void overwriteConfiguration();

    void updateMatrix();

    void populateComboBox(int);

protected:
    QLineEdit* positionX_;
    QLineEdit* positionY_;
    QLineEdit* positionZ_;
    QLineEdit* centerX_;
    QLineEdit* centerY_;
    QLineEdit* centerZ_;
    QLineEdit* cameraFOV_;
    QLineEdit* cameraFocalLength_;
    QLineEdit* cameraDistortion_;
    QPushButton* setButton_;
    QPushButton* overwriteButton_;

    QLineEdit* cameraRotation_;
    QSlider* cameraRotationSlider_;

    QLineEdit* pixelAspectRatio_;

    QLineEdit* centreOffsetX_;
    QLineEdit* centreOffsetY_;


    QComboBox* configurationSelection_;
    QLineEdit* configurationName_;
    QPushButton* saveConfiguration_;

    AMCameraConfiguration* cameraModel_;

    double cameraRotationCentre_;

    QLineEdit* matrixElement_[MATRIXSIZE];

    QFrame* cameraFrame_;
    QFrame* matrixFrame_;













};

#endif // AMCAMERACONFIGURATION_H
