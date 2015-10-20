#ifndef BIOXASVALVESCONTROL_H
#define BIOXASVALVESCONTROL_H

#include "application/BioXAS/BioXAS.h"
#include "beamline/AMControlSet.h"
#include "beamline/AMPseudoMotorControl.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASValvesControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Enum providing the values this control can have.
	enum Value { Open = 0, Closed = 1, None = 2 };

	/// Constructor.
	explicit BioXASValvesControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASValvesControl();

	/// Returns true if the value is always measurable, provided the control is connected.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move is always possible, provided control is connected.
	virtual bool shouldMove() const { return true; }

	/// Returns true if this control can take a measurement right now.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now.
	virtual bool canMove() const;

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns the front-end valve set.
	AMControlSet* frontEndValveSet() const { return frontEndValveSet_; }
	/// Returns the Side valve set.
	AMControlSet* sideValveSet() const { return sideValveSet_; }
	/// Returns the Main valve set.
	AMControlSet* mainValveSet() const { return mainValveSet_; }
	/// Returns the Imaging valve set.
	AMControlSet* imagingValveSet() const { return imagingValveSet_; }

	/// Returns true if this control's value is Open, false otherwise.
	bool isOpen() const;
	/// Returns true if this control's value is Closed, false otherwise.
	bool isClosed() const;

	/// Returns a string representation of the given value.
	QString valueToString(BioXASValvesControl::Value value) const;

signals:
	/// Notifier that the front-end valves set has changed.
	void frontEndValveSetChanged(AMControlSet *newValves);
	/// Notifier that the Side valves set has changed.
	void sideValvesSetChanged(AMControlSet *newValves);
	/// Notifier that the Main valves set has changed.
	void mainValvesSetChanged(AMControlSet *newValves);
	/// Notifier that the Imaging valves set has changed.
	void imagingValvesSetChanged(AMControlSet *newValves);

public slots:
	/// Sets the front-end valves.
	void setFrontEndValves(AMControlSet *valveSet);
	/// Sets the Side beamline valves.
	void setSideValves(AMControlSet *valveSet);
	/// Sets the Main beamline valves.
	void setMainValves(AMControlSet *valveSet);
	/// Sets the Imaging beamline valves.
	void setImagingValves(AMControlSet *valveSet);

protected slots:
	/// Updates the current connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();

protected:
	/// Creates and returns a move action to the given setpoint.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Creates and returns an action that opens the valves in the given control set.
	AMAction3* createOpenValvesAction(AMControlSet *valves);

	/// Returns true if all of the valves in the given set can be measured, false otherwise.
	bool valvesCanMeasure(AMControlSet *valveSet) const;
	/// Returns true if all of the valves in the given set can be moved, false otherwise.
	bool valvesCanMove(AMControlSet *valveSet) const;
	/// Returns true if all of the valves in the given set are open, false otherwise.
	bool valvesOpen(AMControlSet *valveSet) const;
	/// Returns true if all of the valves in the given set are closed, false otherwise.
	bool valvesClosed(AMControlSet *valveSet) const;

protected:
	/// The set of front-end valve controls.
	AMControlSet *frontEndValveSet_;
	/// The set of Side beamline valve controls.
	AMControlSet *sideValveSet_;
	/// The set of Main beamline valve controls.
	AMControlSet *mainValveSet_;
	/// The set of Imaging beamline valve controls.
	AMControlSet *imagingValveSet_;
};

#endif // BIOXASVALVESCONTROL_H
