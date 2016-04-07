#ifndef PGMPERSISTENTVIEW_H
#define PGMPERSISTENTVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>

#include "beamline/PGM/PGMBeamline.h"

class PGMPersistentView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMPersistentView(QWidget *parent = 0);

signals:

public slots:

protected slots:
    /// Handles the valve A button push. Opens or closes based on current state.
    void onValveAButtonPushed();
    /// Handles the valve B button push. Opens or closes based on current state.
    void onValveBButtonPushed();

    /// Handles the logic for opening and closing photon shutter 3
    void onPhotonShutter3Clicked();
    /// Handles the state changes for the button appearance for photon shutter 3.
    void onPhotonShutter3ValueChanged(double state);
    /// Handles the logic for opening and closing photon shutter 2
    void onPhotonShutter2Clicked();
    /// Handles the state changes for the button appearance for photon shutter 2.
    void onPhotonShutter2ValueChanged(double state);
    /// Handles the state changes for the button appearance for photon shutter 1.
    void onPhotonShutter1ValueChanged(double state);
    /// Handles the logic for opening and closing safety shutter 1.
    void onSafetyShutter1Clicked();
    /// Handles the state changes for the button appearance for safety shutter 1.
    void onSafetyShutter1ValueChanged(double state);

    /// Handles state change in valves
    void onValveStatusChanged();

protected:
    /// Helper method that returns the color based on the current state of the shutter
    QColor colorFromShutterState(double state) const;

    /// Button and label for the valves.
    QPushButton *valveAButton_;
    QPushButton *valveBButton_;


    /// The valve control set (Branch A is index 0, Branch B is index 1)
    AMControlSet *valves_;

    /// The button for photon shutter 1.
    QToolButton *photonShutter1Button_;
    /// The button for photon shutter 2.
    QToolButton *photonShutter2Button_;
    /// The button for photon shutter 3.
    QToolButton *photonShutter3Button_;
    /// The button for safety shutter 1.
    QToolButton *safetyShutter1Button_;



};

#endif // PGMPERSISTENTVIEW_H
