#ifndef BIOXASFRONTENDBEAMSTATUS_H
#define BIOXASFRONTENDBEAMSTATUS_H

#include "beamline/BioXAS/BioXASBeamStatus.h"

class BioXASFrontEndShutters;
class BioXASMasterValves;

class BioXASFrontEndBeamStatus : public BioXASBeamStatus
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASFrontEndBeamStatus(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndBeamStatus();

	/// Returns true if this control is connected, false otherwise. Reimplemented to consider the front-end-specific controls.
	virtual bool isConnected() const;

	/// Returns the front-end shutters.
	BioXASFrontEndShutters* shutters() const { return shutters_; }
	/// Returns the valves.
	BioXASMasterValves* valves() const { return valves_; }

signals:
	/// Notifier that the front-end shutters have changed.
	void shuttersChanged(BioXASFrontEndShutters *newShutters);
	/// Notifier that the valves have changed.
	void valvesChanged(BioXASMasterValves *newValves);

public slots:
	/// Sets the front-end shutters.
	virtual void setShutters(BioXASFrontEndShutters *newShutters);
	/// Sets the valves.
	virtual void setValves(BioXASMasterValves *newValves);

protected:
	/// The front-end shutters.
	BioXASFrontEndShutters* shutters_;
	/// The beamline valves.
	BioXASMasterValves *valves_;
};

#endif // BIOXASFRONTENDBEAMSTATUS_H
