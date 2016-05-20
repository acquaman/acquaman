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


#ifndef IDEASPERSISTENTVIEW_H
#define IDEASPERSISTENTVIEW_H


#include <QWidget>
#include <QTimer>

#include "ui/beamline/AMControlMoveButton.h"
#include "source/StripTool2/STWidget.h"
#include "beamline/AMXRFDetector.h"


class QPushButton;
class QLabel;
class QProgressBar;
class QCheckBox;
class QGroupBox;
class IDEASScalerStripTool;

class AMExtendedControlEditor;

/// The persistent panel that lives on the right side of the application.
class IDEASPersistentView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit IDEASPersistentView(QWidget *parent = 0);

signals:

public slots:

protected slots:
    /// Handles the state change to turn the beam on for the beamline.
    void onBeamOnClicked();
    /// Handles the state change to turn the beam off for the beamline.
    void onBeamOffClicked();
    /// Handles changing the state of the beam on and beam off buttons based on the state of the shutters.
    void onShutterStatusChanged(bool state);
    /// Calls dialogue to calibrate beamline energy
    void onCalibrateClicked();
    /// Re-enables scaler continuous mode
    void onScalerContinuousButtonClicked();


    /// Handles updating displayed crystal and energy range
    void onCrystalChanged();

    /// Handles updating displayed ring current
    void onRingCurrentChanged(double);

    /// Updates the status label when the detector initialization state changes.  By default does nothing.
    virtual void onInitializationStateChanged(AMDetector::InitializationState state) { Q_UNUSED(state); }
    /// Updates the status label when the detector acquisition state changes.
    virtual void onAcquisitionStateChanged(AMDetector::AcqusitionState state);
    /// Updates the status label when the detector cleanup state changes.  By default does nothing.
    virtual void onCleanupStateChanged(AMDetector::CleanupState state) { Q_UNUSED(state); }


protected:

    /// The beam on button.
    QPushButton *beamOnButton_;
    /// The beam off button.
    QPushButton *beamOffButton_;
    /// The label for the beam status.
    QLabel *beamStatusLabel_;
    /// The beam on button.
    QPushButton *calibrateButton_;
    /// Checkbox to enable or diable scaler continuous mode
    QPushButton *scalerContinuousButton_;



    AMExtendedControlEditor *energyControlEditor_, *jjSlitHorizontalGap_, *jjSlitHorizontalCenter_, *jjSlitVerticalGap_, *jjSlitVerticalCenter_;

    QLabel *ringCurrent_;
    QLabel *monoCrystal_;
    QLabel *monoEnergyRange_;

    /// The KETEK status indicator.
    QLabel *KETEKstatusLabel_;


    QLabel *IOldLabel_;
    QLabel *I0Label_;
    QLabel *ISampleLabel_;
    QLabel *IReferenceLabel_;

    QLabel *IOldValueLabel_;
    QLabel *I0ValueLabel_;
    QLabel *ISampleValueLabel_;
    QLabel *IReferenceValueLabel_;

    QProgressBar *IOldBar_;
    QProgressBar *I0Bar_;
    QProgressBar *ISampleBar_;
    QProgressBar *IReferenceBar_;


    AMControlMoveButton *jjSlitsVertOpen_, *jjSlitsVertClose_, *jjSlitsHorizClose_, *jjSlitsHorizOpen_, *jjSlitsRight_, *jjSlitsLeft_, *jjSlitsDown_, *jjSlitsUp_;

    IDEASScalerStripTool *stripTool_;




};

#endif // IDEASPERSISTENTVIEW_H
