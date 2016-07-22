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

	/// Adds a given control to the list of child controls.
	virtual bool addChildControl(AMControl *control);
	/// Removes a given control from the list of child controls.
	virtual bool removeChildControl(AMControl *control);

	/// Returns the current connected state, considers the connected state of each child.
	virtual bool isConnected() const { return connected_; }

protected slots:
	/// Sets the connected state.
	void setConnected(bool isConnected);
	/// Updates the connected state.
	virtual void updateConnected();

protected:
	/// Returns true if the child controls are connected.
	virtual bool childrenConnected() const;

protected:
	/// The current connected state.
	bool connected_;
};

#endif // AMCONNECTEDCONTROL_H
