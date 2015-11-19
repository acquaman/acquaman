#ifndef SGMCONTROL_H
#define SGMCONTROL_H

#include "beamline/AMControl.h"

class SGMControl : public AMControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMControl(const QString &name, const QString &units, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMControl();

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

#endif // SGMCONTROL_H
