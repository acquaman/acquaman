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

#ifndef PGMPERSISTENTVIEW_H
#define PGMPERSISTENTVIEW_H

#include <QWidget>
#include <QTimer>

#include "ui/beamline/AMControlMoveButton.h"
#include "source/StripTool2/STWidget.h"

class QPushButton;
class QLabel;
class QProgressBar;
class QCheckBox;
class QGroupBox;

class AMExtendedControlEditor;

// ToDo:  class PGMScalerStripTool;

/// The persistent panel that lives on the right side of the application.
class PGMPersistentView : public QWidget
{
    Q_OBJECT
public:
    /// Constructor.
    explicit PGMPersistentView(QWidget *parent = 0);

signals:

public slots:

protected slots:
    /// Handles the state change to turn the beam on for this beamline.
    void onBeamOnClicked();
    /// Handles the state change to turn the beam off for this beamline.
    void onBeamOffClicked();
    /// Handles changing the state of the beam on and beam off buttons based on the shutter state.
    void onShutterStatusChanged(bool state);
    /// Calls dialog to calibrate beamline energy
    void onCalibrateClicked();
    /// Re-enables scaler contiunous mode
    void onScalerContinuousButtonClicked();

    /// Handles updating displayed crystal and energy range
    void onCrystalChanged();
    /// Handles updating displayed storage ring current
    void onRingCurrentChanged(double);

protected:
    /// The beam on button.
    QPushButton *beamOnButton_;
    /// The beam off button.
    QPushButton *beamOffButton_;
    /// The label for the beam status.
    QLabel *beamStatusLabel_;
    /// The calibrate beam button.
    QPushButton *calibrateButton_;
    /// Checkbox to enable or disable scaler continous mode.
    QPushButton *scalerContinuousButton_;

    //Group box editor with extended controls.
    // ToDo: May need to add more (i.e jjslits, etc)
    AMExtendedControlEditor *energyControlEditor_;

    /// Value in storage ring
    QLabel *ringCurrent_;
    /// Labels for mono information. Will be used with energyControlEditor info.
    QLabel *monoCrystal_;
    QLabel *monoEnergyRange_;

    //ToDo:  Lables and progressbar for I0 data.
    //ToDo: AMControlMoveButtons for jjSlits

    //ToDo: PGMSCalerStripTool *stripTool_;



};

#endif // PGMPERSISTENTVIEW_H
