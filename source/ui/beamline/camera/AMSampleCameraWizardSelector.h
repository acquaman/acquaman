#ifndef AMSAMPLECAMERAWIZARDSELECTOR_H
#define AMSAMPLECAMERAWIZARDSELECTOR_H

#include <QWidget>

class QPushButton;

/** The AMSampleCameraWizardSelector is a widget that
 *	contains the buttons to open the AMSampleCamera wizards
 *	as well as the related load buttons.
 */
class AMSampleCameraWizardSelector : public QWidget
{
	Q_OBJECT
public:
	AMSampleCameraWizardSelector(QWidget* parent = 0);

	/// has the wizard been completed?
	/// (used for  setting the colour of the
	/// of the button)
	bool beamWizardComplete() const;
	bool cameraWizardComplete() const;
	bool samplePlateWizardComplete() const;
	bool rotationWizardComplete() const;

public slots:
	/// change completion state of wizard
	void setBeamWizardComplete(const bool &complete);
	void setCameraWizardComplete(const bool &complete);
	void setSamplePlateWizardComplete(const bool &complete);
	void setRotationWizardComplete(const bool &complete);

	void onBeamWizardFinished();
	void onCameraWizardFinished();
	void onSamplePlateWizardFinished();
	void onRotationWizardFinished();

	void onBeamCompleteChanged();
	void onCameraCompleteChanged();
	void onSamplePlateCompleteChanged();
	void onRotationCompleteChanged();

signals:
	/// wizard button pressed
	void beamWizardPressed();
	void cameraWizardPressed();
	void samplePlateWizardPressed();
	void rotationWizardPressed();

	/// load button pressed
	void requestLoadBeamConfiguration();
	void requestLoadCameraConfiguration();
	void requestLoadSamplePlate();
	void requestLoadRotationConfiguration();

	void beamCompleteChanged();
	void cameraCompleteChanged();
	void samplePlateCompleteChanged();
	void rotationCompleteChanged();

protected slots:
	void onLoadAllConfigurationsButtonClicked();


protected:
	/// buttons for the wizards
	QPushButton* beamWizardButton_;
	QPushButton* cameraWizardButton_;
	QPushButton* samplePlateWizardButton_;
	QPushButton* rotationWizardButton_;

	/// button to reload all configurations
	QPushButton *loadAllConfigurationsButton_;

	/// buttons to reload existing configurations
	QPushButton* loadBeamConfigurationButton_;
	QPushButton* loadCameraConfigurationButton_;
	QPushButton* loadSamplePlateButton_;
	QPushButton* loadRotationConfigurationButton_;

	/// completion state
	bool beamWizardComplete_;
	bool cameraWizardComplete_;
	bool samplePlateWizardComplete_;
	bool rotationWizardComplete_;


};

#endif // AMSAMPLECAMERAWIZARDSELECTOR_H
