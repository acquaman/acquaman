#ifndef BIOXASFRONTENDSHUTTERS_H
#define BIOXASFRONTENDSHUTTERS_H

#include "beamline/BioXAS/BioXASShutters.h"

class AMReadOnlyPVControl;
class CLSBiStateControl;

class BioXASFrontEndShutters : public BioXASShutters
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASFrontEndShutters(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndShutters();

	/// Returns true if this control can move right now. False otherwise. Reimplemented to consider only a subset of child controls.
	virtual bool canMove() const;

	/// Returns the upstream photon shutter control.
	AMReadOnlyPVControl* upstreamPhotonShutter() const { return upstreamPhotonShutter_; }
	/// Returns the downstream photon shutter control.
	CLSBiStateControl* downstreamPhotonShutter() const { return downstreamPhotonShutter_; }
	/// Returns the safety shutter control.
	CLSBiStateControl* safetyShutter() const { return safetyShutter_; }

signals:
	/// Notifier that the upstream photon shutter control has changed.
	void upstreamPhotonShutterChanged(AMControl *newControl);
	/// Notifier that the downstream photon shutter control has changed.
	void downstreamPhotonShutterChanged(AMControl *newControl);
	/// Notifier that the safety shutter control has changed.
	void safetyShutterChanged(AMControl *newControl);

public slots:
	/// Sets the upstream photon shutter control.
	void setUpstreamPhotonShutter(AMReadOnlyPVControl *newControl);
	/// Sets the downstream photon shutter control.
	void setDownstreamPhotonShutter(CLSBiStateControl *newControl);
	/// Sets the safety shutter control.
	void setSafetyShutter(CLSBiStateControl *newControl);

protected:
	/// Creates and returns an action that moves this control to the Open state. Reimplemented to account for the specific order that child controls must be opened in.
	virtual AMAction3* createMoveToOpenAction();
	/// Creates and returns an action that moves this control to the Closed state. Reimplemented to account for the specific order that child controls must be closed in.
	virtual AMAction3* createMoveToClosedAction() { return 0; }

protected:
	/// The upstream photon shutter.
	AMReadOnlyPVControl *upstreamPhotonShutter_;
	/// The downstream photon shutter.
	CLSBiStateControl *downstreamPhotonShutter_;
	/// The safety shutter.
	CLSBiStateControl *safetyShutter_;
};

#endif // BIOXASFRONTENDSHUTTERS_H
