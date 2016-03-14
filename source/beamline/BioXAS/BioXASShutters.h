#ifndef BIOXASSHUTTERS_H
#define BIOXASSHUTTERS_H

#include "beamline/BioXAS/BioXASTriStateGroup.h"

class BioXASShutters : public BioXASTriStateGroup
{
	Q_OBJECT

public:
	/// Enumeration of the possible value states.
	enum Value { Open = 0, Closed = 1 };

	/// Constructor.
	explicit BioXASShutters(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASShutters();

	/// Returns true if this control is open, false otherwise. Finds this out by investigating the states of all children.
	virtual bool isOpen() const;
	/// Returns true if this control is closed, false otherwise. Finds this out by investigating the states of all children.
	virtual bool isClosed() const;

	/// Returns true if the given control is one of the shutters, false otherwise.
	bool hasShutter(AMControl *control) const;

	/// Returns the list of shutters.
	QList<AMControl*> shuttersList() const { return children_; }

signals:
	/// Notifier that the shutters have changed.
	void shuttersChanged();

public slots:
	/// Adds a shutter control.
	bool addShutter(AMControl *newShutter, double openValue, double closedValue);
	/// Removes a shutter control.
	bool removeShutter(AMControl *newValve);
	/// Clears all shutter controls.
	bool clearShutters();

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

#endif // BIOXASSHUTTERS_H
