#ifndef AMSAMPLECAMERAWIZARDSELECTOR_H
#define AMSAMPLECAMERAWIZARDSELECTOR_H

#include <QWidget>

class QPushButton;

class AMSampleCameraWizardSelector : public QWidget
{
    Q_OBJECT
public:
    AMSampleCameraWizardSelector(QWidget* parent = 0);

    bool beamWizardComplete() const;
    bool cameraWizardComplete() const;
    bool samplePlateWizardComplete() const;

public slots:
    void setBeamWizardComplete(const bool &complete);
    void setCameraWizardComplete(const bool &complete);
    void setSamplePlateWizardComplete(const bool &complete);

    void onBeamWizardFinished();
    void onCameraWizardFinished();
    void onSamplePlateWizardFinished();

    void onBeamCompleteChanged();
    void onCameraCompleteChanged();
    void onSamplePlateCompleteChanged();

signals:
    void beamWizardPressed();
    void cameraWizardPressed();
    void samplePlateWizardPressed();

    void requestLoadBeamConfiguration();
    void requestLoadCameraConfiguration();
    void requestLoadSamplePlate();

    void beamCompleteChanged();
    void cameraCompleteChanged();
    void samplePlateCompleteChanged();


protected:
    /// buttons for the wizards
    QPushButton* beamWizardButton_;
    QPushButton* cameraWizardButton_;
    QPushButton* samplePlateWizardButton_;

    QPushButton* loadBeamConfigurationButton_;
    QPushButton* loadCameraConfigurationButton_;
    QPushButton* loadSamplePlateButton_;

    bool beamWizardComplete_;
    bool cameraWizardComplete_;
    bool samplePlateWizardComplete_;


};

#endif // AMSAMPLECAMERAWIZARDSELECTOR_H
