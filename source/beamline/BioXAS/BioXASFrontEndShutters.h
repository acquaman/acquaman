#ifndef BIOXASFRONTENDSHUTTERS_H
#define BIOXASFRONTENDSHUTTERS_H

#include "beamline/CLS/CLSBeamStatusShutters.h"

class AMReadOnlyPVControl;
class CLSExclusiveStatesControl;

class BioXASFrontEndShutters : public CLSBeamStatusShutters
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFrontEndShutters(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFrontEndShutters();

	/// Returns true if this control can move right now. False otherwise. Reimplemented to consider only a subset of child controls.
	virtual bool canMove() const;

	/// Returns true if this control is closed, false otherwise. Reimplemented because there may be more than one possibility for 'Closed' status for these controls: either 0, 4.
	virtual bool isClosed() const { return !isOpen(); }

	/// Returns the upstream photon shutter control.
	AMReadOnlyPVControl* upstreamPhotonShutter() const { return upstreamPhotonShutter_; }
	/// Returns the downstream photon shutter control.
	CLSExclusiveStatesControl* downstreamPhotonShutter() const { return downstreamPhotonShutter_; }

signals:
	/// Notifier that the upstream photon shutter control has changed.
	void upstreamPhotonShutterChanged(AMControl *newControl);
	/// Notifier that the downstream photon shutter control has changed.
	void downstreamPhotonShutterChanged(AMControl *newControl);

public slots:
	/// Sets the upstream photon shutter control.
	void setUpstreamPhotonShutter(AMReadOnlyPVControl *newControl);
	/// Sets the downstream photon shutter control.
	void setDownstreamPhotonShutter(CLSExclusiveStatesControl *newControl);

protected:
	/// Creates and returns an action that moves this control to the Open state. Reimplemented to account for the specific order that child controls must be opened in.
	virtual AMAction3* createMoveToOpenAction();
	/// Creates and returns an action that moves this control to the Closed state. Reimplemented to account for the specific order that child controls must be closed in.
	virtual AMAction3* createMoveToClosedAction() { return 0; }

protected:
	/// The upstream photon shutter.
	AMReadOnlyPVControl *upstreamPhotonShutter_;
	/// The downstream photon shutter.
	CLSExclusiveStatesControl *downstreamPhotonShutter_;
};

#endif // BIOXASFRONTENDSHUTTERS_H
