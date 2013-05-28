#ifndef AMCAMERACONFIGURATIONVIEW_H
#define AMCAMERACONFIGURATIONVIEW_H

#include <QWidget>
#include <QVector3D>
class QLineEdit;
class QPushButton;
class AMCameraConfiguration;
class QComboBox;
class QSlider;
class AMBeamConfigurationView;

/// This class provides a configuration window for model parameters in the Camera Browser
class AMCameraConfigurationView : public QWidget
{
    Q_OBJECT
public:
    AMCameraConfigurationView(QWidget *parent = 0);

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

public slots:

    void updateAll();



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


    QComboBox* configurationSelection_;
    QLineEdit* configurationName_;
    QPushButton* saveConfiguration_;

    AMCameraConfiguration* cameraModel_;

    AMBeamConfigurationView* beamConfiguration_;


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

    void updateName(QString name);
    void updateSelection(int);
    void saveConfiguration();
    void overwriteConfiguration();

    void populateComboBox(int);




signals:
    void update(AMCameraConfiguration*);

    // for beam configuration
    void oneSelect();
    void twoSelect();

    void beamChanged(QObject*);
    void intersection();


};

#endif // AMCAMERACONFIGURATION_H
