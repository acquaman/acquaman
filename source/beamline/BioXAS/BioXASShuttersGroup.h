#ifndef BIOXASSHUTTERSGROUP_H
#define BIOXASSHUTTERSGROUP_H

#include "beamline/BioXAS/BioXASBiStateGroup.h"

class AMControl;

class BioXASShuttersGroup : public BioXASBiStateGroup
{
	Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Open = 0, Closed = 1 };

	/// Constructor.
	explicit BioXASShuttersGroup(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASShuttersGroup();

	/// Returns true if this control is open, false otherwise. Finds this out by investigating the states of all children.
	virtual bool isOpen() const;
	/// Returns true if this control is closed, false otherwise. Finds this out by investigating the states of all children.
	virtual bool isClosed() const;

	/// Returns the list of shutters.
	QList<AMControl*> shuttersList() const { return children_; }

signals:
	/// Notifier that the shutters have changed.
	void shuttersChanged();

protected slots:
	/// Adds a shutter control.
	void addShutter(AMControl *newShutter, double openValue, double closedValue);
	/// Removes a shutter control.
	void removeShutter(AMControl *newValve);
	/// Clears all shutter controls.
	void clearShutters();

protected:
	/// Creates and returns a move action.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Creates and returns a new move action to Open.
	virtual AMAction3* createMoveToOpenAction();
	/// Creates and returns a new move action to Closed.
	virtual AMAction3* createMoveToClosedAction();

	/// Creates and returns a new action that opens the child control.
	AMAction3* createMoveChildToOpen(AMControl *child);
	/// Creates and returns a new action that closes the child control.
	AMAction3* createMoveChildToClosed(AMControl *child);

	/// Creates and returns a new action that checks that the child control is open, waiting the timeout amount.
	AMAction3* createCheckChildIsOpen(AMControl *child, double timeoutSec);
	/// Creates and returns a new action that checks that the child control is closed, waiting the timeout amount.
	AMAction3* createCheckChildIsClosed(AMControl *child, double timeoutSec);

	/// Returns the index for the current value.
	virtual int currentIndex() const;
};

#endif // BIOXASSHUTTERSGROUP_H
