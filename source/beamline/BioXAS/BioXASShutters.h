#ifndef BIOXASSHUTTERS_H
#define BIOXASSHUTTERS_H

#include "beamline/BioXAS/BioXASShuttersGroup.h"

class CLSExclusiveStatesControl;
class BioXASFrontEndShutters;

class BioXASShutters : public BioXASShuttersGroup
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASShutters(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASShutters();

	/// Returns true if this control is closed, false otherwise. Reimplemented because there may be more than one possibility for 'Closed' status for these controls: either 0, 4.
	virtual bool isClosed() const { return !isOpen(); }

	/// Returns the front-end shutters.
	BioXASFrontEndShutters* frontEndShutters() const { return frontEndShutters_; }
	/// Returns the endstation shutters.
	CLSExclusiveStatesControl* endstationShutter() const { return endstationShutter_; }

signals:
	/// Notifier that the front-end shutters have changed.
	void frontEndShuttersChanged(BioXASFrontEndShutters *newShutters);
	/// Notifier that the endstation shutters have changed.
	void endstationShutterChanged(CLSExclusiveStatesControl *newShutter);

public slots:
	/// Sets the front-end shutters.
	void setFrontEndShutters(BioXASFrontEndShutters *newShutters);
	/// Sets the endstation shutter.
	void setEndstationShutter(CLSExclusiveStatesControl *newShutter);

protected:
	/// Creates and returns a new move action to Open. Reimplemented to open shutters in a particular order.
	virtual AMAction3* createMoveToOpenAction();
	/// Creates and returns a new move action to Closed. Reimplemented to close shutters in a particular order.
	virtual AMAction3* createMoveToClosedAction();

protected:
	/// The front-end shutters.
	BioXASFrontEndShutters *frontEndShutters_;
	/// The endstation shutter.
	CLSExclusiveStatesControl *endstationShutter_;
};

#endif // BIOXASSHUTTERS_H
