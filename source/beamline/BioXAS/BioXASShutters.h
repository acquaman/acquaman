#ifndef BIOXASSHUTTERS_H
#define BIOXASSHUTTERS_H

#include "beamline/AMEnumeratedControl.h"

class AMControlSet;

class BioXASShutters : public AMEnumeratedControl
{
	Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Open = 0, Closed = 1 };

	/// Constructor.
	BioXASShutters(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASShutters();

	/// Returns true if this control can measure its value right now. False otherwise.
	virtual bool canMeasure() const;
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop a move right now. False otherwise.
	virtual bool canStop() const;

	/// Returns true if the shutters are open, false otherwise.
	virtual bool isOpen() const;
	/// Returns true if the shutters are closed, false otherwise.
	virtual bool isClosed() const;

	/// Returns the list of shutters.
	QList<AMControl*> shuttersList() const;

signals:
	/// Notifier that the shutters have changed.
	void shuttersChanged();

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();
	/// Updates the moving state.
	virtual void updateMoving();

	/// Adds a shutter control to the shutters set.
	void addShutter(AMControl *newValve);
	/// Removes a shutter control from the shutters set.
	void removeShutter(AMControl *newValve);

	/// Adds shutters to the shutters set.
	void addShutters(BioXASShutters *newShutters);
	/// Removes shutters from the shutters set.
	void removeShutters(BioXASShutters *shutters);
	/// Clears the shutters set.
	void clearShutters();

protected:
	/// Creates and returns a new move action.
	virtual AMAction3* createMoveAction(double setpoint);
	/// Creates and returns a new move action to Open.
	virtual AMAction3* createOpenShuttersAction();

	/// Creates and returns a list of move enum states for this control.
	virtual QStringList generateEnumStates() const;

	/// Returns the current index. Subclasses must reimplement for their specific behavior/interaction.
	virtual int currentIndex() const;

protected:
	/// The set of shutters controls.
	AMControlSet *shuttersSet_;
};

#endif // BIOXASSHUTTERS_H
