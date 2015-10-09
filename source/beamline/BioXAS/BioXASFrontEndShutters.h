#ifndef BIOXASFRONTENDSHUTTERS_H
#define BIOXASFRONTENDSHUTTERS_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/CLS/CLSBiStateControl.h"

class BioXASFrontEndShutters : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASFrontEndShutters(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndShutters();

	/// Returns true if each of the subcomponents are connected.
	virtual bool isConnected() const;

	/// Returns the front end upstream photon shutter.
	AMReadOnlyPVControl* photonShutterUpstream() const { return photonShutterUpstream_; }
	/// Returns the front end vacuum valve.
	AMReadOnlyPVControl* vacuumValve() const { return vacuumValve_; }
	/// Returns the front end fast valve.
	AMReadOnlyPVControl* fastValve() const { return fastValve_; }
	/// Returns the front end downstream photon shutter.
	CLSBiStateControl* photonShutterDownstream() const { return photonShutterDownstream_; }
	/// Returns the front end safety shutter.
	CLSBiStateControl* safetyShutter() const { return safetyShutter_; }

protected:
	/// The front end upstream photon shutter.
	AMReadOnlyPVControl *photonShutterUpstream_;
	/// The front end vacuum valve.
	AMReadOnlyPVControl *vacuumValve_;
	/// The front end fast valve.
	AMReadOnlyPVControl *fastValve_;
	/// The front end downstream photon shutter.
	CLSBiStateControl *photonShutterDownstream_;
	/// The front end safety shutter.
	CLSBiStateControl *safetyShutter_;
};

#endif // BIOXASFRONTENDSHUTTERS_H
