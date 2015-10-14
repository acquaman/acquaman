#ifndef BIOXASFRONTENDBEAMSTATUS_H
#define BIOXASFRONTENDBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/BioXAS/BioXASFrontEndBeamStatusControl.h"

class BioXASFrontEndBeamStatus : public BioXASBeamStatus
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndBeamStatus(BioXASFrontEndShutters *shutters, BioXASValves *valves, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatus();

	/// Returns true if the beam status is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the front-end shutters.
	BioXASFrontEndShutters* frontEndShutters() const { return frontEndShutters_; }
	/// Returns the beamline valves.
	BioXASValves* beamlineValves() const { return beamlineValves_; }
	/// Returns the front-end beam status control.
	BioXASFrontEndBeamStatusControl* beamStatusControl() const { return beamStatus_; }

signals:
	/// Notifier that the front-end shutters have changed.
	void frontEndShuttersChanged(BioXASFrontEndShutters *newShutters);
	/// Notifier that the beamline valves have changed.
	void beamlineValvesChanged(BioXASValves *newValves);

public slots:
	/// Sets the front-end shutters.
	void setFrontEndShutters(BioXASFrontEndShutters *newShutters);
	/// Sets the beamline valves.
	void setBeamlineValves(BioXASValves *newValves);

protected:
	/// The front-end shutters.
	BioXASFrontEndShutters *frontEndShutters_;
	/// The beamline valves.
	BioXASValves *beamlineValves_;

	/// The front-end beam status control.
	BioXASFrontEndBeamStatusControl* beamStatus_;
};

#endif // BIOXASFRONTENDBEAMSTATUS_H
