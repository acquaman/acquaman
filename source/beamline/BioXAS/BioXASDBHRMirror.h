#ifndef BIOXASDBHRMIRROR_H
#define BIOXASDBHRMIRROR_H

#include <QObject>

#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASDBHRMirror : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASDBHRMirror(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASDBHRMirror();

	/// Returns the current connected state.
	bool isConnected() const { return isConnected_; }
	/// Returns the m1 vertical motor control.
	CLSMAXvMotor* m1VerticalControl() const { return m1Vertical_; }
	/// Returns the m2 vertical motor control.
	CLSMAXvMotor* m2VerticalControl() const { return m2Vertical_; }
	/// Returns the pitch motor control.
	CLSMAXvMotor* pitchControl() const { return pitch_; }

signals:
	/// Notifier that the connected state has changed.
	void connectedChanged(bool isConnected);
	/// Notifier that the m1 vertical control has changed.
	void m1VerticalControlChanged(CLSMAXvMotor *newControl);
	/// Notifier that the m2 vertical control has changed.
	void m2VerticalControlChanged(CLSMAXvMotor *newControl);
	/// Notifier that the pitch control has changed.
	void pitchControlChanged(CLSMAXvMotor *newControl);

protected slots:
	/// Sets the mirror's global connected state.
	void setConnected(bool isConnected);
	/// Sets the m1 vertical motor control.
	void setM1MirrorControl(CLSMAXvMotor *newControl);
	/// Sets the m2 vertical motor control.
	void setM2MirrorControl(CLSMAXvMotor *newControl);
	/// Sets the pitch motor control.
	void setPitchControl(CLSMAXvMotor *newControl);

	/// Handles updating the mirror's global connected state, according to the controls' status.
	void onConnectedChanged();

protected:
	/// Handles control setup: connections, reparenting.
	void controlSetup(AMControl *control);
	/// Handles control cleanup: disconnects, memory management.
	void controlCleanup(AMControl *control);

protected:
	/// The mirror's connected state.
	bool isConnected_;
	/// The m1 vertical motor.
	CLSMAXvMotor *m1Vertical_;
	/// The m2 vertical motor.
	CLSMAXvMotor *m2Vertical_;
	/// The pitch motor.
	CLSMAXvMotor *pitch_;
};

#endif // BIOXASDBHRMIRROR_H
