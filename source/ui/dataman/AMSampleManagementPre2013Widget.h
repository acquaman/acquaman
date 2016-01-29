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


#ifndef AMSAMPLEPOSITIONPRE2013VIEW_H
#define AMSAMPLEPOSITIONPRE2013VIEW_H

#include <QWidget>

class QGridLayout;
class QUrl;
class QGroupBox;

class AMTopFrame;

#include "AMSamplePlatePre2013View.h"
class AMSamplePlatePre2013View;
class AMSampleManipulator;

#ifdef AM_MOBILITY_VIDEO_ENABLED
class AMBeamlineCameraWidgetWithSourceTabs;
#endif

/// This widget provides a complete full-screen view for users to view, move, align, and tag samples in the machine. It simply combines three widgets: an AMCrosshairVideoWidget for the video, whatever widget you want to provide for controlling the sample manipulator, and an AMSamplePlateView. We create the video widget and sample plate view for you.
class AMSampleManagementPre2013Widget : public QWidget
{
Q_OBJECT
public:
	/// Construct the widget.
	/*!
\param manipulatorWidget Any kind of widget you want to include below the video in the lower-left (usually, for directly controlling the sample manipulator.)
\param sampleCameraUrl The url of the video feed to open. (You can add more video feeds later by accessing the cameraWidget() and calling addSource().)
\param sampleCameraDescription The title the user will see for the camera view (ex: "Endstation Top View").
\param samplePlate A pointer that will be passed to the AMSamplePlateView constructor as the sample plate to work with; it can be 0 if the plate view should create its own plate.
\param manipulator [Optional] If you want the AMSamplePlateView to be able to mark and move sample positions, you should include an AMSampleManipulator instance \c manipulator, which will be passed to the AMSamplePlateView.*/
	AMSampleManagementPre2013Widget(QWidget *manipulatorWidget, const QUrl& sampleCameraUrl, const QString& sampleCameraDescription, AMSamplePlatePre2013* samplePlate = 0, AMSampleManipulator* manipulator = 0, QWidget *parent = 0);
	virtual ~AMSampleManagementPre2013Widget();

	/// Accesses the sample plate view
	AMSamplePlatePre2013View* samplePlateView() { return plateView_; }
#ifdef AM_MOBILITY_VIDEO_ENABLED
	/// Access the camera widget.
	AMBeamlineCameraWidgetWithSourceTabs* cameraWidget() { return cameraWidget_; }
#endif
	/// Access the user-suppplied manipulator widget
	QWidget* manipulatorWidget() { return manipulatorWidget_; }
signals:
	/// Forwards the newSamplePlateSelected() from the AMSamplePlateView.
	void newSamplePlateSelected(AMSamplePlatePre2013 *selectedPlate);

protected slots:
	/// responds to and forwards the newSamplePlateSelected() signal from AMSamplePlate.
	void onNewSamplePlateSelected();

protected:

#ifdef AM_MOBILITY_VIDEO_ENABLED
	AMBeamlineCameraWidgetWithSourceTabs *cameraWidget_;
#endif

	AMSamplePlatePre2013View *plateView_;
	QWidget *manipulatorWidget_;

	AMTopFrame *topFrame_;
};



#endif // AMSAMPLEPOSITIONPRE2013VIEW_H
