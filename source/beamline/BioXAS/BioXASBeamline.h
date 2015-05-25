#ifndef BIOXASBEAMLINE_H
#define BIOXASBEAMLINE_H

#include "beamline/CLS/CLSBeamline.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASMonochromator;
class BioXASM2Mirror;

class BioXASBeamline : public CLSBeamline
{
    Q_OBJECT

public:
	/// Enum indicating different shutter open states.
	class Shutters { public: enum State { Open = 1, Between = 2, Closed = 4 }; };

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
	/// Returns the current 'beam off' state.
	virtual bool beamOff() const;
	/// Returns the current 'beam on' state.
	virtual bool beamOn() const;

	/// Returns the beamline's photon shutter.
	AMControl* photonShutter() { return photonShutter_; }
	/// Returns the beamline's safety shutter.
	AMControl* safetyShutter() const { return safetyShutter_; }
	/// Returns the beamline endstation safety shutter.
	AMControl* endstationSafetyShutter() const { return endstationSafetyShutter_; }
	/// Returns the beamline's monochromator.
	virtual BioXASMonochromator* mono() const { return 0; }
	/// Returns the m2 mirror.
	virtual BioXASM2Mirror* m2Mirror() const { return 0; }
	/// Returns the beamline's scaler.
	virtual CLSSIS3820Scaler* scaler() const { return 0; }

	/// Returns a newly created action that turns off beam by closing the shutters.
	virtual AMAction3* createTurnOffBeamActions();
	/// Returns a newly created action that turns on beam by opening the shutters.
	virtual AMAction3* createTurnOnBeamActions();

signals:
	/// Notifier that the current connected state has changed.
	void connected(bool isConnected);
	/// Notifier that the current 'beam off' state has changed.
	void beamStatusChanged();

protected:
	/// Sets up various beamline components.
	virtual void setupComponents();

	/// Protected constructor.
	BioXASBeamline(const QString &controlName);

protected:

	// Shutters.
	/// The photon shutter.
	CLSBiStateControl *photonShutter_;
	/// The safety shutter.
	CLSBiStateControl *safetyShutter_;
	/// The endstation safety shutter.
	CLSBiStateControl *endstationSafetyShutter_;

};

#endif // BIOXASBEAMLINE_H
