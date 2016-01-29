#ifndef AMCONNECTEDCONTROL_H
#define AMCONNECTEDCONTROL_H

#include "source/beamline/AMControl.h"

class AMConnectedControl : public AMControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMConnectedControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = "");
	/// Destructor.
	virtual ~AMConnectedControl();

	/// Returns the current connected state, considers the connected state of each child.
	virtual bool isConnected() const;

protected slots:
	/// Sets the connected state.
	void setConnected(bool isConnected);
	/// Updates the connected state.
	void updateConnected();

	/// Adds a child control: connects to child's connected signal.
	virtual void addChildControl(AMControl *control);
	/// Removes a child control: disconnects from all signals from the child.
	virtual void removeChildControl(AMControl *control);

protected:
	/// The current connected state.
	bool connected_;
};

#endif // AMCONNECTEDCONTROL_H
