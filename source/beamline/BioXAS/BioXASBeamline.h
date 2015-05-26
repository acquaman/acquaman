#ifndef BIOXASBEAMLINE_H
#define BIOXASBEAMLINE_H

#include "beamline/CLS/CLSBeamline.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASMonochromator;
class BioXASM1Mirror;
class BioXASM2Mirror;

class BioXASBeamline : public CLSBeamline
{
    Q_OBJECT

public:
	/// Enum indicating different shutter open states.
	class Shutters { public: enum State { Open = 1, Between = 2, Closed = 4 }; };
	/// Enum indicating different beam on/off states.
	class Beam { public: enum State { Off = 0, On = 1, Available = 2 }; };

	/// Singleton accessor.
	static BioXASBeamline *bioXAS()
	{
		if (!instance_) {
			instance_ = new BioXASBeamline("BioXAS Beamline");
			instance_->initializeBeamlineSupport();
		}

		return static_cast<BioXASBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~BioXASBeamline();

	/// Returns the current connected state.
	virtual bool isConnected() const;
	/// Returns the (cached) current connected state.
	virtual bool connected() const { return connected_; }

	/// Returns the current 'beam off' state, true if the downstream safety shutter is closed. False otherwise.
	virtual bool beamOff() const;
	/// Returns the current 'beam on' state, true if the downstream safety shutter is open. False otherwise.
	virtual bool beamOn() const;
	/// Returns the current 'beam available' state, true if all shutters are open. False otherwise.
	virtual bool beamAvailable() const;

	/// Returns the upstream photon shutter.
	AMControl* photonShutterUpstream() const { return photonShutterUpstream_; }
	/// Returns the downstream photon shutter.
	AMControl* photonShutterDownstream() const { return photonShutterDownstream_; }
	/// Returns the front end (upstream) safety shutter.
	AMControl* safetyShutterUpstream() const { return safetyShutterUpstream_; }
	/// Returns the endstation (downstream) safety shutter.
	AMControl* safetyShutterDownstream() const { return safetyShutterDownstream_; }
	/// Returns the monochromator.
	virtual BioXASMonochromator* mono() const { return 0; }
	/// Returns the m1 mirror.
	virtual BioXASM1Mirror* m1Mirror() const { return m1Mirror_; }
	/// Returns the m2 mirror.
	virtual BioXASM2Mirror* m2Mirror() const { return m2Mirror_; }
	/// Returns the scaler.
	virtual CLSSIS3820Scaler* scaler() const { return 0; }

	/// Returns a newly created action that turns off beam by closing the endstation (downstream) safety shutter. Returns 0 if not connected.
	virtual AMAction3* createTurnOffBeamActions();
	/// Returns a newly created action that turns on beam by opening the endstation (upstream) safety shutter. Returns 0 if not connected.
	virtual AMAction3* createTurnOnBeamActions();

signals:
	/// Notifier that the current connected state has changed.
	void connectedChanged(bool isConnected);
	/// Notifier that the current 'beam off' state has changed.
	void beamStatusChanged();

protected slots:
	/// Sets the cached connected state.
	void setConnected(bool isConnected);
	/// Updates the cached connected state.
	void updateConnected();

protected:
	/// Sets up the storage ring.
	virtual void setupStorageRing();
	/// Sets up various beamline components.
	virtual void setupComponents();

	/// Protected constructor.
	BioXASBeamline(const QString &controlName);

protected:
	/// The current connected state.
	bool connected_;

	// Shutters.
	/// The upstream photon shutter.
	CLSBiStateControl *photonShutterUpstream_;
	/// The downstream photon shutter.
	CLSBiStateControl *photonShutterDownstream_;
	/// The front end (upstream) safety shutter.
	CLSBiStateControl *safetyShutterUpstream_;
	/// The endstation (downstream) safety shutter.
	CLSBiStateControl *safetyShutterDownstream_;

	/// The M1 mirror.
	BioXASM1Mirror *m1Mirror_;
	/// The M2 mirror.
	BioXASM2Mirror *m2Mirror_;

};

#endif // BIOXASBEAMLINE_H
