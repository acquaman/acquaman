/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMSAMPLECAMERAWIZARDSELECTOR_H
#define AMSAMPLECAMERAWIZARDSELECTOR_H

#include <QWidget>
#include <QPoint>

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

	/// Request to load a database configuration
	void requestLoadBeamConfigurationFromDatabase();
	void requestLoadCameraConfigurationFromDatabase();
	void requestLoadSamplePlateConfigurationFromDatabase();
	void requestLoadRotationConfigurationFromDatabase();

	void beamCompleteChanged();
	void cameraCompleteChanged();
	void samplePlateCompleteChanged();
	void rotationCompleteChanged();

protected slots:
	/// Attempts to reload all of the configurations in order (rather than forcing the user to click on each in turn)
	void onLoadAllConfigurationsButtonClicked();

	/// Handles the camera wizard button click, which behaves differently based on command line arguements
	void onCameraWizardButtonClicked();
	/// Handles the rotation wizard button click, which behaves differently based on command line arguements
	void onRotationWizardButtonClicked();

	/// Handles custom context requests
	void onLoadBeamConfigurationButtonCustomContextMenuRequested(QPoint point);
	void onLoadCameraConfigurationButtonCustomContextMenuRequested(QPoint point);
	void onLoadSamplePlateConfigurationButtonCustomContextMenuRequested(QPoint point);
	void onLoadRotationConfigurationButtonCustomContextMenuRequested(QPoint point);

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
