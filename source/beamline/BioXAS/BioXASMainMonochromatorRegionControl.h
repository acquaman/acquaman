#ifndef BIOXASMAINMONOCHROMATORREGIONCONTROL_H
#define BIOXASMAINMONOCHROMATORREGIONCONTROL_H

#include "beamline/AMPVControl.h"

#include "BioXASSSRLMonochromator.h"
#include "BioXASSSRLMonochromatorRegionControl.h"

class BioXASMainBeamline;
class BioXASMainMonochromatorRegionControlView;

class BioXASMainMonochromatorRegionControl : public BioXASSSRLMonochromatorRegionControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainMonochromatorRegionControl(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMainMonochromatorRegionControl();
	/// Returns the current region.
	virtual double value() const { return value_; }
	/// Returns the current region setpoint.
	virtual double setpoint() const { return setpoint_; }
	/// Returns the control view.
	BioXASMainMonochromatorRegionControlView* view() const;

signals:

public slots:
	/// Sets the new region setpoint and performs a crystal change, if necessary.
	virtual FailureExplanation move(double setpoint);

protected slots:
	/// Sets the current region in response to a value change from one of the region controls.
	void onRegionControlValueChanged();

protected:
	/// Returns a new action that closes the upper slit, 0 if not connected.
	AMAction3* createCloseUpperSlitAction();
	/// Returns a new action that closes the lower slit, 0 if not connected.
	AMAction3* createCloseLowerSlitAction();
	/// Returns a new action that closes both slits, 0 if not connected.
	AMAction3* createCloseSlitsAction();
	/// Returns a new action that waits for the slits status to change from 'not closed' to 'closed'.
	AMAction3* createWaitForSlitsClosedAction();
	/// Returns a new action that removes the paddle, 0 if not connected.
	AMAction3* createRemovePaddleAction();
	/// Returns a new action that waits for the paddle to be removed.
	AMAction3* createWaitForPaddleRemovedAction();
	/// Returns a new action that waits for the key status to change to 'Enabled'.
	AMAction3* createWaitForKeyEnabledAction();
	/// Returns a new action that moves the bragg motor control to the given destination (degrees), 0 if not connected.
	AMAction3* createMoveBraggAction(double destination);
	/// Returns a new action that sends the mono bragg motor to the change position.
	AMAction3* createMoveBraggToCrystalChangePositionAction();
	/// Returns a new action that waits for the mono to signal it has reached the crystal change position.
	AMAction3* createWaitForBraggAtCrystalChangePositionAction();
	/// Returns a new action that waits for the mono brake to be disabled, 0 if not connected.
	AMAction3* createWaitForBrakeDisabledAction();
	/// Returns a new action that moves the crystal change motor to the given destination (relative move), 0 if not connected.
	AMAction3* createMoveCrystalChangeAction(double destination);
	/// Returns a new action that moves the crystal change motor to the appropriate limit for the given region destination, 0 if not connected or if the destination is Region::None.
	AMAction3* createMoveCrystalChangeToRegionLimitAction(int region);
	/// Returns a new action that waits for the crystal change motor to reach its clockwise limit, 0 if not connected.
	AMAction3* createWaitForCrystalChangeAtCWLimitAction();
	/// Returns a new action that waits for the crystal change motor to reach its counter-clockwise limit, 0 if not connected.
	AMAction3* createWaitForCrystalChangeAtCCWLimitAction();
	/// Returns a new action that waits for the mono crystal change motor to reach a limit. Which particular limit (cw, ccw) depends on the region destination. Returns 0 if not connected, or if the destination is Region::None.
	AMAction3* createWaitForCrystalChangeAtRegionLimitAction(int region);
	/// Returns a new action that waits for the mono brake to be turned on, 0 if not connected.
	AMAction3* createWaitForBrakeEnabledAction();
	/// Returns a new action that moves the bragg motor into the destination region, 0 if not connected or if the destination is Region::None.
	AMAction3* createMoveBraggToRegionAction(int destination);
	/// Returns a new action that waits for the mono to move into region A.
	AMAction3* createWaitForRegionChangedToAAction();
	/// Returns a new action that waits for the mono to move into region B.
	AMAction3* createWaitForRegionChangedToBAction();
	/// Returns a new action that waits for the mono to move into a new region, 0 if not connected or if the destination is Region::None (to include this option is trickier to implement).
	AMAction3* createWaitForRegionChangedAction(int destination);
	/// Returns a new action that waits for the mono region key to be turned CW to Disabled, 0 if not connected.
	AMAction3* createWaitForKeyDisabledAction();

	/// Returns a new action that performs a crystal change to change the region.
	AMListAction3* createChangeRegionAction(int newRegion);

protected:
	/// The current region.
	int value_;
	/// The region setpoint.
	int setpoint_;
	/// The current move state, true if this control has intiated a move.
	bool moveInProgress_;

	// Child controls

	/// The upper slit motor control.
	AMControl* upperSlitMotor_;
	/// The lower slit motor control.
	AMControl* lowerSlitMotor_;
	/// The slits closed status control.
	AMControl* slitsStatus_;
	/// The paddle motor control.
	AMControl* paddleMotor_;
	/// The paddle status control.
	AMControl* paddleStatus_;
	/// The key status control.
	AMControl* keyStatus_;
	/// The bragg motor control.
	AMControl* braggMotor_;
	/// The bragg motor 'in position' control.
	AMControl* braggAtCrystalChangePositionStatus_;
	/// The brake status control.
	AMControl* brakeStatus_;
	/// The crystal change motor control.
	AMControl* crystalChangeMotor_;
	/// The crystal change motor clockwise limit control.
	AMControl* crystalChangeCWLimitStatus_;
	/// The crystal change motor counter-clockwise limit control.
	AMControl* crystalChangeCCWLimitStatus_;
	/// The region A status control.
	AMControl* regionAStatus_;
	/// The region B status control.
	AMControl* regionBStatus_;
};

#endif // BIOXASMAINMONOCHROMATORREGIONCONTROL_H
