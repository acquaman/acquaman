#ifndef SGMENERGYCOORDINATOR_H
#define SGMENERGYCOORDINATOR_H

#include <QObject>
#include "beamline/SGM/energy/SGMEnergyPosition.h"
class SGMEnergyControl;
class SGMNewEnergyPVSet;
class SGMOldEnergyPVSet;
class SGMEnergyCoordinator : public QObject
{
    Q_OBJECT
public:
	explicit SGMEnergyCoordinator(QObject *parent = 0);

signals:

public slots:
protected slots:
	void onPVSetsConnected();

	void onEnergyControlConnected();

protected slots:
	// Slots which connect the energy control pseudo motor output to the new PVs
	////////////////////////////////////////////////////////////////////////////

	/// Handles the energy control signalling that its energy has changed.
	void onEnergyControlValueChanged(double);

	/// Handles the energy control signalling that its moving status has changed.
	void onEnergyControlIsMovingChanged(bool);

	/// Handles the energy control signalling that its grating optimization mode has changed.
	void onEnergyControlGratingTranslationOptimizationModeChanged(SGMEnergyPosition::GratingTranslationOptimizationMode gratingTranslationOptimizationMode);

	/// Handles the energy control signalling that its undulator offset has changed
	void onEnergyControlUndulatorOffsetChanged(double);

	/// Handles the energy control signalling that its undulator harmonic has changed
	void onEnergyControlUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic);

	/// Handles the energy control signalling that its undulator tracking has changed
	void onEnergyControlUndulatorTrackingChanged(bool);

	/// Handles the energy control signalling that its exit slit tracking has changed
	void onEnergyControlExitSlitTrackingChanged(bool);

	/// Handles the old grating translation step PV signalling that its value has changed.
	void onOldGratingTranslationStepPVChanged(double);

	/// Handles the old grating translation step PV signalling that its moving state has changed.
	void onOldGratingTranslationStepPVMovingChanged(bool);

	// Slots which connect the new setpoint PVs to the energy control pseudo motor
	////////////////////////////////////////////////////////////////////////////

	/// Handles the new energy stop PV being altered.
	void onEnergyStopPVChanged(double);

	/// Handles the new grating optimization PV being altered.
	void onGratingTranslationOptimizationModeSetpointPVChanged(double);

	/// Handles the new grating translation stop PV being altered.
	void onGratingTranslationStopPVChanged(double);

	/// Handles the new undulator offset PV being altered.
	void onUndulatorOffsetPVChanged(double);

	/// Handles the new undulator harmonic setpoint PV being altered.
	void onUndulatorHarmonicSetpointPVChanged(double);

	/// Handles the new undulator tracking PV being altered.
	void onUndulatorTrackingPVChanged(double);

	/// Handles the new exit slit position tracking PV being altered.
	void onExitSlitTrackingPVChanged(double);
protected:

	SGMEnergyControl* energyControlCoordinator_;

	bool connectedOnce_;
	SGMNewEnergyPVSet* newControls_;
	SGMOldEnergyPVSet* oldControls_;
};

#endif // SGMENERGYCOORDINATOR_H
