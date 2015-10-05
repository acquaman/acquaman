#ifndef BIOXASSHUTTERS_H
#define BIOXASSHUTTERS_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "beamline/BioXAS/BioXASShutterControl.h"

class BioXASShutters : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASShutters(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASShutters();

	/// Returns true if each of the subcomponents are connected.
	virtual bool isConnected() const;

	/// Returns the front end upstream photon shutter.
	CLSBiStateControl* photonShutterUpstream() const { return photonShutterUpstream_; }
	/// Returns the front end downstream photon shutter.
	CLSBiStateControl* photonShutterDownstream() const { return photonShutterDownstream_; }
	/// Returns the front end safety shutter.
	CLSBiStateControl* safetyShutter() const { return safetyShutter_; }
	/// Returns the Side endstation safety shutter.
	virtual AMControl* safetyShutterSide() const { return 0; }
	/// Returns the Main endstation safety shutter.
	virtual AMControl* safetyShutterMain() const { return 0; }
	/// Returns the Imaging endstation safety shutter.
	virtual AMControl* safetyShutterImaging() const { return 0; }

	/// Returns the shutters control.
	virtual AMControl* shutterControl() const { return shuttersControl_; }

protected:
	/// The front end upstream photon shutter.
	CLSBiStateControl *photonShutterUpstream_;
	/// The front end downstream photon shutter.
	CLSBiStateControl *photonShutterDownstream_;
	/// The front end safety shutter.
	CLSBiStateControl *safetyShutter_;

	/// The shutters control.
	BioXASShuttersControl *shuttersControl_;
};

#endif // BIOXASSHUTTERS_H
