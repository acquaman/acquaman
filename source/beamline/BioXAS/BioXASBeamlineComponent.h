#ifndef BIOXASBEAMLINECOMPONENT_H
#define BIOXASBEAMLINECOMPONENT_H

#include "beamline/AMControl.h"

class BioXASBeamlineComponent : public AMControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASBeamlineComponent(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamlineComponent();

	/// Returns true if this control should be able to be stopped (if connected), false otherwise.
	virtual bool shouldStop() const { return true; }
	/// Returns true if this control can be stopped right now, false otherwise. Finds this out be examining all child controls. Subclasses can reimplement to achieve their particular behavior.
	virtual bool canStop() const;

public slots:
	/// Stops all child controls that can be stopped.
	virtual bool stop();

protected slots:
	/// Adds a given control to the list of child controls.
	virtual void addChildControl(AMControl *control);
	/// Removes a given control from the list of child controls.
	virtual void removeChildControl(AMControl *control);

	/// Sets the current connected state.
	void setConnected(bool isConnected);

	/// Updates the connected state.
	virtual void updateConnected();

protected:
	/// The current connected state.
	bool connected_;
};

#endif // BIOXASBEAMLINECOMPONENT_H
