#include "SGMEnergyControlSet.h"

#include "beamline/SGM/energy/SGMEnergyPosition.h"
#include "beamline/SGM/energy/SGMEnergyPVControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
SGMEnergyControlSet::SGMEnergyControlSet(QObject *parent) :
    AMControlSet(parent)
{
	energyPositionValidator_ = 0;

	QString baseGroupPV = "AM1611-4-I10:energy";
	addControl(new SGMEnergyPVControl(this));
	controlNamed("Energy")->setAttemptMoveWhenWithinTolerance(true);

	addControl(new AMReadOnlyPVControl("Energy Status",
					   baseGroupPV + ":status",
					   this));
	controlNamed("Energy Status")->setTolerance(0.5);

	addControl(new AMSinglePVControl("Energy Trajectory Target",
					 baseGroupPV + ":trajectory:target:eV",
	                                 this,
	                                 0.5,
	                                 2));

	addControl(new AMSinglePVControl("Energy Trajectory Time",
	                                 baseGroupPV + ":trajectory:time:s",
	                                 this,
	                                 0.1,
	                                 2));

	addControl(new AMSinglePVControl("Energy Trajectory Start",
	                                 baseGroupPV + ":trajectory:start",
	                                 this,
	                                 0.5,
	                                 2));

	addControl(new AMPVwStatusControl("Grating Angle",
	                                  baseGroupPV + ":grating:angle:steps:fbk",
	                                  baseGroupPV + ":grating:angle:steps",
	                                  baseGroupPV + ":grating:angle:status",
	                                  baseGroupPV + ":grating:angle:stop",
	                                  this,
	                                  5,
	                                  2.0,
	                                  new CLSMAXvControlStatusChecker()));

	controlNamed("Grating Angle")->setAttemptMoveWhenWithinTolerance(true);

	addControl(new AMPVwStatusControl("Grating Translation",
	                                  baseGroupPV + ":grating:translation:fbk",
	                                  baseGroupPV + ":grating:translation",
	                                  baseGroupPV + ":grating:translation:status",
	                                  baseGroupPV + ":grating:translation:stop",
	                                  this,
	                                  0.5,
	                                  2.0,
	                                  new CLSMAXvControlStatusChecker()));

	controlNamed("Grating Translation")->setAttemptMoveWhenWithinTolerance(true);

	addControl(new AMPVControl("Grating Translation Optimization",
	                           baseGroupPV + ":grating:translation:optimization:fbk",
	                           baseGroupPV + ":grating:translation:optimization",
	                           QString(),
	                           this,
	                           0.5));

	controlNamed("Grating Translation Optimization")->setAttemptMoveWhenWithinTolerance(true);

	addControl(new AMPVwStatusControl("Undulator Position",
	                                  baseGroupPV + ":undulator:mm:fbk",
	                                  baseGroupPV + ":undulator:mm",
	                                  baseGroupPV + ":undulator:status",
	                                  baseGroupPV + ":undulator:stop",
	                                  this,
	                                  0.1,
	                                  2.0,
	                                  new CLSMAXvControlStatusChecker()));

	controlNamed("Undulator Position")->setAttemptMoveWhenWithinTolerance(true);

	addControl(new AMSinglePVControl("Undulator Detune Offset",
	                                 baseGroupPV + ":undulator:offset:mm",
	                                 this,
	                                 0.1));

	controlNamed("Undulator Detune Offset")->setAttemptMoveWhenWithinTolerance(true);

	addControl(new AMPVControl("Undulator Harmonic",
	                           baseGroupPV + ":undulator:harmonic:fbk",
	                           baseGroupPV + ":undulator:harmonic",
	                           QString(),
	                           this,
	                           0.5));

	controlNamed("Undulator Harmonic")->setAttemptMoveWhenWithinTolerance(true);

	addControl(new AMSinglePVControl("Undulator Tracking",
	                                 baseGroupPV + ":undulator:tracking",
	                                 this,
	                                 0.5));

	controlNamed("Undulator Tracking")->setAttemptMoveWhenWithinTolerance(true);

	addControl(new AMPVwStatusControl("Exit Slit Position",
	                                  baseGroupPV + ":exitslit:position:mm:fbk",
	                                  baseGroupPV + ":exitslit:position:mm",
	                                  baseGroupPV + ":exitslit:position:status",
	                                  baseGroupPV + ":exitslit:position:stop",
	                                  this,
	                                  0.1,
	                                  2.0,
	                                  new CLSMAXvControlStatusChecker()));

	controlNamed("Exit Slit Position")->setAttemptMoveWhenWithinTolerance(true);

	addControl(new AMSinglePVControl("Exit Slit Position Tracking",
	                                 baseGroupPV + ":exitslit:position:tracking",
	                                 this,
	                                 0.5));

	controlNamed("Exit Slit Position Tracking")->setAttemptMoveWhenWithinTolerance(true);

	connect(this, SIGNAL(connected(bool)), this, SLOT(onAllConnected(bool)));
}

AMControl * SGMEnergyControlSet::energy() const
{
	return controlNamed("Energy");
}

AMControl * SGMEnergyControlSet::energyTrajectoryTarget() const
{
	return controlNamed("Energy Trajectory Target");
}

AMControl * SGMEnergyControlSet::energyTrajectoryTime() const
{
	return controlNamed("Energy Trajectory Time");
}

AMControl * SGMEnergyControlSet::energyTrajectoryStart() const
{
	return controlNamed("Energy Trajectory Start");
}

AMControl * SGMEnergyControlSet::energyStatus() const
{
	return controlNamed("Energy Status");
}

AMControl * SGMEnergyControlSet::gratingAngle() const
{
	return controlNamed("Grating Angle");
}

AMControl * SGMEnergyControlSet::gratingTranslation() const
{
	return controlNamed("Grating Translation");
}

AMControl * SGMEnergyControlSet::gratingTranslationOptimization() const
{
	return controlNamed("Grating Translation Optimization");
}

AMControl * SGMEnergyControlSet::undulatorPosition() const
{
	return controlNamed("Undulator Position");
}

AMControl * SGMEnergyControlSet::undulatorDetuneOffset() const
{
	return controlNamed("Undulator Detune Offset");
}

AMControl * SGMEnergyControlSet::undulatorHarmonic() const
{
	return controlNamed("Undulator Harmonic");
}

AMControl * SGMEnergyControlSet::undulatorTracking() const
{
	return controlNamed("Undulator Tracking");
}

AMControl * SGMEnergyControlSet::exitSlitPosition() const
{
	return controlNamed("Exit Slit Position");
}

AMControl * SGMEnergyControlSet::exitSlitPositionTracking() const
{
	return controlNamed("Exit Slit Position Tracking");
}

AMValidator * SGMEnergyControlSet::errorValidator() const
{
	if(energyPositionValidator_) {

		// Todo: Return the error validator. Needs 1566 to be merged in first.
	}

	return 0;
}

AMValidator * SGMEnergyControlSet::warningValidator() const
{
	if(energyPositionValidator_) {

		// Todo: Return the warning validator. Needs 1566 to be merged in first.
	}

	return 0;
}

void SGMEnergyControlSet::onAllConnected(bool isConnected)
{
	if(isConnected && !energyPositionValidator_) {

		double currentEnergy = energy()->value();
		SGMGratingSupport::GratingTranslation currentGratingTranslation;

		if(gratingTranslation()->withinTolerance(0)) {

			currentGratingTranslation = SGMGratingSupport::LowGrating;
		} else if (gratingTranslation()->withinTolerance(1)) {

			currentGratingTranslation = SGMGratingSupport::MediumGrating;
		} else if(gratingTranslation()->withinTolerance(2)) {

			currentGratingTranslation = SGMGratingSupport::HighGrating;
		} else {

			currentGratingTranslation = SGMGratingSupport::UnknownGrating;
		}

		energyPositionValidator_ = new SGMEnergyPosition(currentEnergy,
		                                                 currentGratingTranslation);

		onGratingAnglePVValueChanged(gratingAngle()->value());
		onGratingTranslationPVValueChanged(gratingTranslation()->value());
		onGratingtranslationOptimizationPVValueChanged(gratingTranslationOptimization()->value());
		onUndulatorPositionPVValueChanged(undulatorPosition()->value());
		onUndulatorDetuneOffsetPVValueChanged(undulatorDetuneOffset()->value());
		onUndulatorHarmonicPVValueChanged(undulatorHarmonic()->value());
		onUndulatorTrackingPVValueChanged(undulatorTracking()->value());
		onExitSlitPositionPVValueChanged(exitSlitPosition()->value());
		onExitSlitTrackingPVValueChanged(exitSlitPositionTracking()->value());

		emit initialized();
	}
}

void SGMEnergyControlSet::onEnergyPVValueChanged(double value)
{
	energyPositionValidator_->requestEnergy(value);
}

void SGMEnergyControlSet::onGratingAnglePVValueChanged(double value)
{
	energyPositionValidator_->setGratingAngle(value);
}

void SGMEnergyControlSet::onGratingTranslationPVValueChanged(double)
{
	SGMGratingSupport::GratingTranslation newGratingTranslation;

	if(gratingTranslation()->withinTolerance(0)) {

		newGratingTranslation = SGMGratingSupport::LowGrating;
	} else if (gratingTranslation()->withinTolerance(1)) {

		newGratingTranslation = SGMGratingSupport::MediumGrating;
	} else if(gratingTranslation()->withinTolerance(2)) {

		newGratingTranslation = SGMGratingSupport::HighGrating;
	} else {

		newGratingTranslation = SGMGratingSupport::UnknownGrating;
	}

	energyPositionValidator_->setGratingTranslation(newGratingTranslation);
}

void SGMEnergyControlSet::onGratingtranslationOptimizationPVValueChanged(double)
{
	SGMGratingSupport::GratingTranslationOptimizationMode newOptimizationMode;

	if(gratingTranslationOptimization()->withinTolerance(0)) {

		newOptimizationMode = SGMGratingSupport::ManualMode;
	} else if(gratingTranslationOptimization()->withinTolerance(1)) {

		newOptimizationMode = SGMGratingSupport::OptimizeFlux;
	} else if(gratingTranslationOptimization()->withinTolerance(2)) {

		newOptimizationMode = SGMGratingSupport::OptimizeResolution;
	} else {

		return;
	}

	energyPositionValidator_->setGratingTranslationOptimizationMode(newOptimizationMode);
}

void SGMEnergyControlSet::onUndulatorPositionPVValueChanged(double value)
{
	energyPositionValidator_->setUndulatorPosition(value);
}

void SGMEnergyControlSet::onUndulatorDetuneOffsetPVValueChanged(double value)
{
	energyPositionValidator_->setUndulatorOffset(value);
}

void SGMEnergyControlSet::onUndulatorHarmonicPVValueChanged(double)
{
	SGMUndulatorSupport::UndulatorHarmonic newHarmonic;

	if(undulatorHarmonic()->withinTolerance(0)) {

		newHarmonic = SGMUndulatorSupport::FirstHarmonic;
	} else if(undulatorHarmonic()->withinTolerance(1)) {

		newHarmonic = SGMUndulatorSupport::ThirdHarmonic;
	} else {

		newHarmonic = SGMUndulatorSupport::UnknownUndulatorHarmonic;
	}

	energyPositionValidator_->setUndulatorHarmonic(newHarmonic);
}

void SGMEnergyControlSet::onUndulatorTrackingPVValueChanged(double)
{
	energyPositionValidator_->setUndulatorTracking(!undulatorTracking()->withinTolerance(0));
}

void SGMEnergyControlSet::onExitSlitPositionPVValueChanged(double value)
{
	energyPositionValidator_->setExitSlitPosition(value);
}

void SGMEnergyControlSet::onExitSlitTrackingPVValueChanged(double)
{
	energyPositionValidator_->setExitSlitPositionTracking(!exitSlitPositionTracking()->withinTolerance(0));
}

