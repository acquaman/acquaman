#ifndef SGMENERGYCOORDINATOR_H
#define SGMENERGYCOORDINATOR_H

#include <QObject>
#include "beamline/SGM/energy/SGMEnergyPosition.h"
#include "beamline/AMControl.h"

class SGMEnergyCoordinatorControl;
class SGMNewEnergyPVSet;
class SGMOldEnergyPVSet;
class SGMBeamCoordinatorControl;
class SGMNewBeamOnControls;
class SGMEnergyCoordinator : public QObject
{
	Q_OBJECT
public:
	explicit SGMEnergyCoordinator(QObject *parent = 0);

signals:

public slots:
protected slots:
	void onPVSetsConnected();

	void onEnergyControlConnected(bool isConnected);

	void onBeamOnControlConnected(bool isConnected);
	// Slots which connect the energy control pseudo motor output to the new PVs
	////////////////////////////////////////////////////////////////////////////

	/// Handles the energy control signalling that its energy has changed.
	void onEnergyControlValueChanged(double);

	/// Handles the energy control signalling that its moving status has changed.
	void onEnergyControlIsMovingChanged(bool);

	/// Handles the energy control signalling that its grating optimization mode has changed.
	void onEnergyControlGratingTranslationOptimizationModeChanged(SGMGratingSupport::GratingTranslationOptimizationMode gratingTranslationOptimizationMode);

	/// Handles the energy control signalling that its undulator offset has changed
	void onEnergyControlUndulatorOffsetChanged(double);

	/// Handles the energy control signalling that its undulator harmonic has changed
	void onEnergyControlUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);

	/// Handles the energy control signalling that its undulator tracking has changed
	void onEnergyControlUndulatorTrackingChanged(bool);

	/// Handles the energy control signalling that its exit slit tracking has changed
	void onEnergyControlExitSlitPositionTrackingChanged(bool);

	/// Handles the old grating angle PV signalling that its value has changed.
	void onOldGratingAnglePVChanged(double);

	/// Handles the old grating angle PV signalling that its moving status has changed.
	void onOldGratingAnglePVMovingChanged(bool);

	/// Handles the old grating translation step PV signalling that its value has changed.
	void onOldGratingTranslationStepPVChanged(double);

	/// Handles the old grating translation step PV signalling that its moving state has changed.
	void onOldGratingTranslationStepPVMovingChanged(bool);

	/// Handles the old undulator position PV signalling that its value has changed.
	void onOldUndulatorPositionPVChanged(double);

	/// Handles the old undulator position PV signalling that its moving state has changed.
	void onOldUndulatorPositionPVMovingChanged(bool);

	/// Handles the old exit slit position PV signalling that its value has changed.
	void onOldExitSlitPositionPVChanged(double);

	/// Handles the old exit slit position PV signalling that its moving state has changed.
	void onOldExitSlitPositionPVMovingChanged(bool);

	// Slots which connect the new setpoint PVs to the energy control pseudo motor
	////////////////////////////////////////////////////////////////////////////

	/// Handles the new energy setpoint PV being altered.
	void onEnergySetpointPVChanged(double);

	/// Handles the new energy stop PV being altered.
	void onEnergyStopPVChanged(double);

	/// Handles the new start trajectory motion PV being altered.
	void onEnergyTrajectoryStartPVChanged(double);

	/// Handles the new grating angle PV being altered.
	void onGratingAngleSetpointPVChanged(double);

	/// Handles the new grating angle stop PV being altered.
	void onGratingAngleStopPVChanged(double);

	/// Handles the new grating translaiton setpoint PV being altered.
	void onGratingTranslationSetpointPVChanged(double);

	/// Handles the new grating translation stop PV being altered.
	void onGratingTranslationStopPVChanged(double);

	/// Handles the new grating optimization PV being altered.
	void onGratingTranslationOptimizationModeSetpointPVChanged(double);

	/// Handles the new undulator position setpoint PV being altered.
	void onUndulatorSetpointPVChanged(double);

	/// Handles the new undulator stop setpoint being altered.
	void onUndulatorStopPVChanged(double);

	/// Handles the new undulator offset PV being altered.
	void onUndulatorOffsetPVChanged(double);

	/// Handles the new undulator harmonic setpoint PV being altered.
	void onUndulatorHarmonicSetpointPVChanged(double);

	/// Handles the new undulator tracking PV being altered.
	void onUndulatorTrackingPVChanged(double);

	/// Handles the new exit slit position setpoint PV being altered.
	void onExitSlitPositionSetpointPVChanged(double);

	/// Handles the new exit slit position stop PV being altered.
	void onExitSlitPositionStopPVChanged(double);

	/// Handles the new exit slit position tracking PV being altered.
	void onExitSlitPositionTrackingPVChanged(double);

	// Slots for the Beam On Control/PVs
	/// Handles the Beam On Pseudo-Motor Signalling that's it has started moving
	void onBeamOnControlMoveStarted();

	/// Handles the Beam On Pseudo-Motor Signalling that its move has failed
	void onBeamOnControlMoveFailed();

	/// Handles the Beam On Pseudo-Motor Signalling that it has succeeded in a move
	void onBeamOnControlMoveSucceeded();

	/// Handles the Beam On Pseudo-Motor Signalling that's its value has changed
	void onBeamOnControlValueChanged(double);

	/// Handles the new beam on operation PV's value being changed
	void onBeamOnPVChanged(double);

	/// Handles the new beam off operation PV's value being changed
	void onBeamOffPVChanged(double);

protected:

	SGMEnergyCoordinatorControl* energyControlCoordinator_;
	SGMBeamCoordinatorControl* beamControlCoordinator_;

	bool pvsConnectedOnce_;
	bool energyControlConnectedOnce_;
	bool beamOnControlConnectedOnce_;
	SGMNewEnergyPVSet* newEnergyControls_;
	SGMOldEnergyPVSet* oldEnergyControls_;
	SGMNewBeamOnControls* newBeamOnControls_;

	bool energySetpointInitialized_;
	bool gratingAngleSetpointInitialized_;
	bool gratingTranslationSetpointInitialized_;
	bool gratingTranslationOptimizationModeSetpointInitialized_;
	bool undulatorPositionSetpointInitialized_;
	bool undulatorHarmonicSetpointInitialized_;
	bool undulatorOffsetSetpointInitialized_;
	bool exitSlitPositionSetpointInitialized_;
	bool beamOnInitialized_;
};

#endif // SGMENERGYCOORDINATOR_H
