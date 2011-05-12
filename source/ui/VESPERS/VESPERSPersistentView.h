#ifndef VESPERSPERSISTENTVIEW_H
#define VESPERSPERSISTENTVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

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

protected:
	/// Button and label for the valves.
	QPushButton *valvesButton_;
	QLabel *valvesStatus_;

	/// Label for the temperature, pressure and water sensors.
	QLabel *tempLabel_;
	QLabel *pressureLabel_;
	QLabel *waterLabel_;

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
};

#endif // VESPERSPERSISTENTVIEW_H
