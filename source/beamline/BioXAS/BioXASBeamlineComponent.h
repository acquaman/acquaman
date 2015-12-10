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

public slots:
	/// Stops all controls that can be stopped.
	virtual bool stop() const;

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
