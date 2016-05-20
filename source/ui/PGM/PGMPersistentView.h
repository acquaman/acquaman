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

    /// Handles the state changes for the button appearance for photon shutter 3.
    void onPhotonShutter3ValueChanged(double state);
    /// Handles the state changes for the button appearance for photon shutter 2.
    void onPhotonShutter2ValueChanged(double state);
    /// Handles the state changes for the button appearance for photon shutter 1.
    void onPhotonShutter1ValueChanged(double state);
    /// Handles the state changes for the button appearance for safety shutter 1.
    void onSafetyShutter1ValueChanged(double state);

    /// Handles state change in valves
    void onValveStatusChanged();

protected:
	/// helper function to create and layout the ui components for the photon shutters
	QLayout * createAndLayoutBeamlingShutters();
    /// Helper method that returns the color based on the current state of the shutter
    QColor colorFromShutterState(double state) const;

	///
	void initializationForStartup();

protected:
	/// The button for photon shutters
	QToolButton *photonShutter1Button_;
	QToolButton *photonShutter2Button_;
	QToolButton *photonShutter3Button_;
	QToolButton *safetyShutter1Button_;

	/// Button and label for the valves.
    QPushButton *valveAButton_;
    QPushButton *valveBButton_;

};

#endif // PGMPERSISTENTVIEW_H
