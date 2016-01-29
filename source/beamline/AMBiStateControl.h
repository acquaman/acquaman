#ifndef AMBISTATECONTROL_H
#define AMBISTATECONTROL_H

#include "beamline/AMExclusiveStatesEnumeratedControl.h"

class AMBiStateControl : public AMExclusiveStatesEnumeratedControl // Rename to CLSExclusiveStatesControl?
{
    Q_OBJECT

public:
	/// Enumeration of the possible values.
	enum Value { Closed = 0, Open = 1, Between = 2 };

	/// Constructor.
	explicit AMBiStateControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMBiStateControl();

	/// Returns true if this control is open, false otherwise.
	bool isOpen() const;
	/// Returns true if this control is closed, false otherwise.
	bool isClosed() const;
	/// Returns true if this control is between states, false otherwise.
	bool isBetween() const;

public slots:
	/// Adds the open state.
	void addOpenState(AMControl *openTriggerControl, double openStatusValue, double triggerValue);
	/// Removes the open state.
	void removeOpenState();

	/// Adds the closed state.
	void addClosedState(AMControl *closeTriggerControl, double closedStatusValue, double triggerValue);
	/// Removes the closed state.
	void removeClosedState();

	/// Adds the between state.
	void addBetweenState(double betweenStatusValue);
	/// Removes the between state.
	void removeBetweenState();

protected slots:
	/// Updates the moving status. Check that this is called appropriately.
	virtual void updateMoving();
};

#endif // AMBISTATECONTROL_H
