#ifndef BIOXASSIDESHUTTERS_H
#define BIOXASSIDESHUTTERS_H

#include "beamline/BioXAS/BioXASShutters.h"

class BioXASFrontEndShutters;
class CLSBiStateControl;

class BioXASSideShutters : public BioXASShutters
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSideShutters(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideShutters();

	/// Returns true if this control is closed, false otherwise. Reimplemented because there may be more than one possibility for 'Closed' status for these controls: either 0, 4.
	virtual bool isClosed() const { return !isOpen(); }

	/// Returns the front-end shutters.
	BioXASFrontEndShutters* frontEndShutters() const { return frontEndShutters_; }
	/// Returns the endstation shutters.
	CLSBiStateControl* endstationShutter() const { return endstationShutter_; }

signals:
	/// Notifier that the front-end shutters have changed.
	void frontEndShuttersChanged(BioXASFrontEndShutters *newShutters);
	/// Notifier that the endstation shutters have changed.
	void endstationShutterChanged(CLSBiStateControl *newShutter);

public slots:
	/// Sets the front-end shutters.
	void setFrontEndShutters(BioXASFrontEndShutters *newShutters);
	/// Sets the endstation shutter.
	void setEndstationShutter(CLSBiStateControl *newShutter);

protected:
	/// The front-end shutters.
	BioXASFrontEndShutters *frontEndShutters_;
	/// The endstation shutter.
	CLSBiStateControl *endstationShutter_;
};

#endif // BIOXASSIDESHUTTERS_H
