/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSPERSISTENTVIEW_H
#define VESPERSPERSISTENTVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>

#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/AMShutterButton.h"

class VESPERSPersistentView : public QWidget
{
	Q_OBJECT
public:
	/// Default constructor.
	explicit VESPERSPersistentView(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles the valves button push.  Opens or closes based the current state of the current state.
	void onValvesButtonPushed();
	/// Handles when the valves state changes.
	void onValvesStateChanged();
	/// Handles when the temperature state changes.  If any temperature control is bad, then the state is RED, otherwise it's green.
	void onTemperatureStateChanged();
	/// Handles when the pressure state changes.  If any of the pressure controls are bad, then the state is RED, otherwise it's green.
	void onPressureStateChanged();
	/// Handles when the water state changes.  This holds the flow switches and the flow transducers.  If any of them are bad the state is RED, otherwise it's green.
	void onWaterStateChanged();
	/// Handles the logic for opening PSH1.
	void onPSH1Clicked();
	/// Handles the logic for opening PSH2.
	void onPSH2Clicked();
	/// Handles the logic for opening SSH1.
	void onSSH1Clicked();
	/// Handles updates from the lower shutter filter push button.
	void onLowerFilterUpdate();
	/// Handles the state change from the shutter.  Changes the label to the either a red or green light.  Green means open.
	void onFilterStatusChanged();
	/// Handles the state change from the experiment ready status.
	void onExperimentStatusChanged(bool ready) { experimentReady_->setPixmap(QIcon(ready == true ? ":/ON.png" : ":/RED.png").pixmap(25)); }
	/// Handles changes to the energy from outside the program.
	void onEnergyChanged(double energy) { energySetpoint_->blockSignals(true); energySetpoint_->setValue(energy); energySetpoint_->blockSignals(false); }
	/// Sets the new energy.
	void setEnergy() { VESPERSBeamline::vespers()->mono()->setEo(energySetpoint_->value()); }
	/// Handles changes to the energy feedback.
	void onEnergyFeedbackChanged(double energy) { energyFeedback_->setText(QString::number(energy, 'f', 2)+" eV"); }
	/// Handles enabling and disabling the energy setpoint if the beam is either Pink or None.
	void onBeamChanged(VESPERSBeamline::Beam beam);

protected:
	/// Button and label for the valves.
	QPushButton *valvesButton_;
	QLabel *valvesStatus_;

	/// Label for the temperature, pressure and water sensors.
	QLabel *tempLabel_;
	QLabel *pressureLabel_;
	QLabel *waterLabel_;

	/// Button and label for the endstation shutter.
	QPushButton *filterLowerButton_;
	QLabel *filterLabel_;

	/// The icon label for the experiment status.
	QLabel *experimentReady_;

	/// The energy spin box and label.
	QDoubleSpinBox *energySetpoint_;
	QLabel *energyFeedback_;

	/// The valve control.
	VESPERSValveGroupControl *valves_;
	/// The temperature control.
	AMControlSet *temperature_;
	/// The pressure control.
	AMControlSet *pressure_;
	/// The flow switches.
	AMControlSet *flowSwitches_;
	/// The flow transducers.
	AMControlSet *flowTransducers_;
	/// The photon and safety shutters.
	AMShutterButton *psh1_;
	AMShutterButton *psh2_;
	AMShutterButton *ssh1_;
	AMShutterButton *ssh2_;

private:
	/// Helper function to properly toggle the filter PVs.  Takes an AMControl *, casts it to an AMPVControl * then toggles them.
	void toggleFilter(AMControl *filter);
};

#endif // VESPERSPERSISTENTVIEW_H
