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
	enum Value { None = 0, Open, Closed };

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

	/// Returns the valve set for the named beamline.
	AMControlSet* valveSet(BioXAS::Beamline beamline);
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
	QString valueToString(BioXASValvesControl::Value value);

signals:

public slots:
	/// Sets the valves for the named beamline.
	void setValves(BioXAS::Beamline beamline, AMControlSet *valveSet);
	/// Sets the front-end valves.
	void setFrontEndValves(AMControlSet *valveSet);
	/// Sets the Side beamline valves.
	void setSideValves(AMControlSet *valveSet);
	/// Sets the Main beamline valves.
	void setMainValves(AMControlSet *valveSet);
	/// Sets the Imaging beamline valves.
	void setImagingValves(AMControlSet *valveSet);

	/// Adds a valve to the valve set for the named beamline.
	void addValve(BioXAS::Beamline beamline, CLSBiStateControl *valveControl);
	/// Adds a valve to the front-end valve set.
	void addFrontEndValve(CLSBiStateControl *valveControl);
	/// Adds a valve to the Side beamline valve set.
	void addSideValve(CLSBiStateControl *valveControl);
	/// Adds a valve to the Main beamline valve set.
	void addMainValve(CLSBiStateControl *valveControl);
	/// Adds a valve to the Imaging beamline valve set.
	void addImagingValve(CLSBiStateControl *valveControl);

protected slots:
	/// Updates the current connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();

	/// Sets the valves for the given valve set.
	void setValveSet(AMControlSet *toSet, AMControlSet *desiredControls);

protected:
	/// Creates and returns a move action to the given setpoint.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Creates and returns an action that opens the valves in the given control set.
	AMAction3* createOpenValvesAction(AMControlSet *valves);

	/// Adds all valves in the valve set as children of this control.
	void addChildren(AMControlSet *valveSet);
	/// Removes all valves in the valves set from the children of this control.
	void removeChildren(AMControlSet *valveSet);

	/// Adds the given valve to the given set, if both the valve and the set exist and the set doesn't already contain the valve. Returns true if valve added successfully, false otherwise.
	bool addValveToSet(AMControlSet *controlSet, AMControl *valveControl);

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
