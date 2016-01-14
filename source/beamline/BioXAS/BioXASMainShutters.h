#ifndef BIOXASMAINSHUTTERS_H
#define BIOXASMAINSHUTTERS_H

#include "beamline/BioXAS/BioXASShutters.h"

class BioXASFrontEndShutters;
class CLSBiStateControl;

class BioXASMainShutters : public BioXASShutters
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainShutters(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainShutters();

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

#endif // BIOXASMAINSHUTTERS_H
