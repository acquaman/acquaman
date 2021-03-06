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


#ifndef AMBEAMLINESAMPLEMANAGEMENTVIEW_H
#define AMBEAMLINESAMPLEMANAGEMENTVIEW_H

#include <QWidget>

class QPushButton;
class QVBoxLayout;

class AMBeamline;
class AMSamplePlate;
class AMSamplePlateView;
class AMSamplePlateBrowserView;
class AMSampleCameraBrowserView;
class AMSampleCameraWizardSelector;
class AMShapeData;
class AMMotorGroup;
class AMMotorGroupView;

class AMBeamlineSampleManagementView : public QWidget
{
Q_OBJECT
public:
	/// Constructor takes pointer to beamline to operate on (samplePlate and samplePlateBrowser live in AMBeamline)
	AMBeamlineSampleManagementView(AMBeamline *beamline, AMMotorGroup *motorGroup, QWidget *parent = 0);

	virtual ~AMBeamlineSampleManagementView();

public slots:
	void requestAdvancedCameraOptionsWindow();

protected slots:
	/// Handles launching the AMSamplePlateCreationDialog to make a new sample plate. Also, automatically adds the new plate to the samplePlateBrowser and tells the beamline to use this as the current sample plate
	void onCreateSamplePlateButtonClicked();
	/// Handles showing the samplePlateBrowserView
	void onLoadSamplePlateButtonClicked();

	/// Listens to signal from the beamline that the sample plate is about to change. If the old sample plate was valid, then this removes the drawn shapes from the sampleCameraView
	void onBeamlineSamplePlateAboutToChange(AMSamplePlate *lastSamplePlate);
	/// Listens to signal from the beamline that the sample plate is about to change. If the new sample plate is valid, then this adds new shapes to the sampleCameraView
	void onBeamlineSamplePlateChanged(AMSamplePlate *samplePlate);
	/// Forwards signal from the samplePlateBrowserView to AMBeamline
	void onSamplePlateSelected(AMSamplePlate *samplePlate);

	void onSampleShapeDeleted(AMShapeData *sampleShape);
	void onSampleAboutToBeRemoved(int index);

protected:
	bool checkSamplePlateModifiedHelper();

protected:
	/// Pointer to the beamline we're using
	AMBeamline *beamline_;

	/// We instantiate a samplePlateBrowserView in the constructor, we're responsible for showing it when requested
	AMSamplePlateBrowserView *samplePlateBrowserView_;
	AMMotorGroupView *motorGroupView_;
	/// We instantiate a samplePlateView and keep it updated with the right sample plate by listening to the beamline
	AMSamplePlateView *samplePlateView_;
	/// We instantiate a cameraBrowserView and a new cameraBrowser
	AMSampleCameraBrowserView *cameraBrowserView_;
	/// We instantiate the view that holds the wizard buttons
	AMSampleCameraWizardSelector* wizardSelectorView_;

	/// Button to request the dialog to create a new sample plate
	QPushButton *createSamplePlateButton_;
	/// Button to request reloading a sample plate from the database
	QPushButton *loadSamplePlateButton_;

	/// Left hand vertical layout
	QVBoxLayout *leftVL_;
	/// Right hand vertical layout
	QVBoxLayout *rightVL_;
};

#endif // AMBEAMLINESAMPLEMANAGEMENTVIEW_H
