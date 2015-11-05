#ifndef BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
#define BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H

#include "beamline/AMPseudoMotorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarmWindowOption.h"

class BioXASCarbonFilterFarmActuatorWindowControl : public AMPseudoMotorControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorWindowControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorWindowControl();

	/// Returns true if the total filter thickness is always measurable, if connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a move to a new total filter thickness is always possible, if connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if this control can stop a move in progress, if connected. False otherwise.
	virtual bool shouldStop() const { return true; }

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns true if the given value corresponds to a valid window, false otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value corresponds to a valid window setpoint, false otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns the list of windows.
	QList<BioXASCarbonFilterFarmWindowOption*> windows() const { return windows_; }
	/// Returns the window at the given index. Returns -1 if there is no window for that index.
	BioXASCarbonFilterFarmWindowOption* windowAt(int index) const;

	/// Returns the index of the given filter. Returns -1 if the given filter isn't valid or no match found.
	int indexOf(BioXASCarbonFilterFarmWindowOption *window) const;
	/// Returns the index of the given filter (string), handles all enum states (including "Unknown"). Returns -1 if no match found.
	int indexOf(const QString windowString) const;

	/// Returns the current position control.
	AMControl* currentPosition() { return currentPosition_; }

signals:
	/// Notifier that the actuator control has changed.
	void currentPositionChanged(AMControl *newControl);
	/// Notifier that the list of windows have changed.
	void windowsChanged();

public slots:
	/// Sets the current position control.
	void setCurrentPosition(AMControl *newControl);

	/// Adds a window control. The window control must be valid and not already in the list of windows to be added successfully.
	void addWindow(BioXASCarbonFilterFarmWindowOption *newControl);
	/// Removes a window control.
	void removeWindow(BioXASCarbonFilterFarmWindowOption *control);
	/// Clears all window controls.
	void clearWindows();

protected slots:
	/// Updates the states. Reimplemented to make sure the enums are updated before the current value.
	virtual void updateStates();
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the current value.
	virtual void updateValue();
	/// Updates the moving state.
	virtual void updateMoving();
	/// Updates the maximum value.
	virtual void updateMaximumValue();

	/// Updates the enum states.
	void updateEnumStates();

protected:
	/// Returns a new action that moves the actuator to the desired window setpoint.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Generates a list of the enum states with the given list of filter options.
	QStringList generateEnumStates(QList<BioXASCarbonFilterFarmWindowOption*> filterOptions);
	/// Generates a list of the move enum states with the given list of filter options.
	QStringList generateMoveEnumStates(QList<BioXASCarbonFilterFarmWindowOption*> filterOptions);

	/// Returns the window(s) that are at the given position.
	QList<BioXASCarbonFilterFarmWindowOption*> windowsAtPosition(double position) const;

protected:
	/// The actuator position control.
	AMControl *currentPosition_;
	/// The list of window controls.
	QList<BioXASCarbonFilterFarmWindowOption*> windows_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORWINDOWCONTROL_H
