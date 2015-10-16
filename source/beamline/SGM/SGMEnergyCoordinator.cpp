#include "SGMEnergyCoordinator.h"
#include "beamline/SGM/energy/SGMEnergyControl.h"

#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"
#include "beamline/SGM/SGMNewEnergyPVSet.h"
#include "beamline/SGM/SGMOldEnergyPVSet.h"

#include <QDebug>
SGMEnergyCoordinator::SGMEnergyCoordinator(QObject *parent) :
    QObject(parent)
{
	/*
	  * Initialization for the coordinator happens in 4 stages:
	  * 1: Constructing the PV sets
	  * 2: Waiting for these PV sets to respond that they are connected.
	  * 3: Constructing the energy control coordinator pseudo-motor
	  * 4: Waiting for the energy control to respond that it is connected.
	  */
	connectedOnce_ = false;

	oldControls_ = new SGMOldEnergyPVSet(this);
	newControls_ = new SGMNewEnergyPVSet(this);

	qDebug() << "Waiting for PVs to be connected...";
	if(oldControls_->isConnected() && newControls_->isConnected()) {

		onPVSetsConnected();
	} else {

		connect(oldControls_, SIGNAL(connected(bool)), this, SLOT(onPVSetsConnected()));
		connect(newControls_, SIGNAL(connected(bool)), this, SLOT(onPVSetsConnected()));
	}
}

void SGMEnergyCoordinator::onPVSetsConnected()
{
	qDebug() << "PVs connected";
	if(oldControls_->isConnected() &&
			newControls_->isConnected() &&
			!connectedOnce_) {

		connectedOnce_ = true;

		AMControl* oldUndulatorHarmonicControl = oldControls_->undulatorHarmonic();
		SGMUndulatorSupport::UndulatorHarmonic correspondingUndulatorHarmonic =
				SGMUndulatorSupport::UnknownUndulatorHarmonic;

		if(oldUndulatorHarmonicControl->withinTolerance(1)) {

			correspondingUndulatorHarmonic = SGMUndulatorSupport::FirstHarmonic;
		} else {

			correspondingUndulatorHarmonic = SGMUndulatorSupport::ThirdHarmonic;
		}

		energyControlCoordinator_ = new SGMEnergyControl(correspondingUndulatorHarmonic, this);

		qDebug() << "Waiting for energy control to be connected...";
		if(energyControlCoordinator_->isConnected()) {

			onEnergyControlConnected();
		} else {

			connect(energyControlCoordinator_, SIGNAL(connected(bool)), this, SLOT(onEnergyControlConnected()));
		}
	}
}

// Connecting the energy control coordinator output to the new PVs

void SGMEnergyCoordinator::onEnergyControlConnected()
{
	qDebug() << "Energy Control connected";

	// Initialize values now the energy control coordinator is connected
	onEnergyControlValueChanged(energyControlCoordinator_->value());
	onEnergyControlIsMovingChanged(energyControlCoordinator_->isMoving());
	onEnergyControlGratingTranslationOptimizationModeChanged(energyControlCoordinator_->gratingTranslationOptimizationMode());
	onEnergyControlUndulatorOffsetChanged(energyControlCoordinator_->undulatorOffset());
	onEnergyControlUndulatorHarmonicChanged(energyControlCoordinator_->undulatorHarmonic());
	onEnergyControlUndulatorTrackingChanged(energyControlCoordinator_->isUndulatorTracking());
	onEnergyControlExitSlitPositionTrackingChanged(energyControlCoordinator_->isExitSlitPositionTracking());

	// Connect signals from energy control coordinator
	connect(energyControlCoordinator_, SIGNAL(valueChanged(double)),
			this, SLOT(onEnergyControlValueChanged(double)));
	connect(energyControlCoordinator_, SIGNAL(movingChanged(bool)),
			this, SLOT(onEnergyControlIsMovingChanged(bool)));
	connect(energyControlCoordinator_, SIGNAL(gratingTranslationOptimizationModeChanged(SGMEnergyPosition::GratingTranslationOptimizationMode)),
			this, SLOT(onEnergyControlGratingTranslationOptimizationModeChanged(SGMEnergyPosition::GratingTranslationOptimizationMode)));
	connect(energyControlCoordinator_, SIGNAL(undulatorOffsetChanged(double)),
			this, SLOT(onEnergyControlUndulatorOffsetChanged(double)));
	connect(energyControlCoordinator_, SIGNAL(undulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic)),
			this, SLOT(onEnergyControlUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic)));
	connect(energyControlCoordinator_, SIGNAL(undulatorTrackingChanged(bool)),
			this, SLOT(onEnergyControlUndulatorTrackingChanged(bool)));
	connect(energyControlCoordinator_, SIGNAL(exitSlitTrackingChanged(bool)),
			this, SLOT(onEnergyControlExitSlitPositionTrackingChanged(bool)));
	connect(energyControlCoordinator_->gratingAngleControl(), SIGNAL(valueChanged(double)),
			this, SLOT(onOldGratingAnglePVChanged(double)));
	connect(energyControlCoordinator_->gratingAngleControl(), SIGNAL(movingChanged(bool)),
			this, SLOT(onOldGratingAnglePVMovingChanged(bool)));
	connect(energyControlCoordinator_->gratingTranslationControl(), SIGNAL(valueChanged(double)),
			this, SLOT(onOldGratingTranslationStepPVChanged(double)));
	connect(energyControlCoordinator_->gratingTranslationControl(), SIGNAL(movingChanged(bool)),
			this, SLOT(onOldGratingTranslationStepPVMovingChanged(bool)));
	connect(energyControlCoordinator_->undulatorPositionControl(), SIGNAL(valueChanged(double)),
			this, SLOT(onOldUndulatorPositionPVChanged(double)));
	connect(energyControlCoordinator_->undulatorPositionControl(), SIGNAL(movingChanged(bool)),
			this, SLOT(onOldUndulatorPositionPVMovingChanged(bool)));
	connect(energyControlCoordinator_->exitSlitPositionControl(), SIGNAL(valueChanged(double)),
			this, SLOT(onOldExitSlitPositionPVChanged(double)));
	connect(energyControlCoordinator_->exitSlitPositionControl(), SIGNAL(movingChanged(bool)),
			this, SLOT(onOldExitSlitPositionPVMovingChanged(bool)));

	// Connect signals from the new PVs
	connect(newControls_->energySetpoint(), SIGNAL(valueChanged(double)),
			energyControlCoordinator_, SLOT(move(double)));
	connect(newControls_->energyStop(), SIGNAL(valueChanged(double)),
			this, SLOT(onEnergyStopPVChanged(double)));
	connect(newControls_->gratingAngleSetpoint(), SIGNAL(valueChanged(double)),
			this, SLOT(onGratingAngleSetpointPVChanged(double)));
	connect(newControls_->gratingAngleStop(), SIGNAL(valueChanged(double)),
			this, SLOT(onGratingAngleStopPVChanged(double)));
	connect(newControls_->gratingTranslationOptimizationModeSetpoint(), SIGNAL(valueChanged(double)),
			this, SLOT(onGratingTranslationOptimizationModeSetpointPVChanged(double)));
	connect(newControls_->gratingTranslationStop(), SIGNAL(valueChanged(double)),
			this, SLOT(onGratingTranslationStopPVChanged(double)));
	connect(newControls_->undulatorPositionSetpoint(), SIGNAL(valueChanged(double)),
			this, SLOT(onUndulatorSetpointPVChanged(double)));
	connect(newControls_->undulatorPositionStop(), SIGNAL(valueChanged(double)),
			this, SLOT(onUndulatorStopPVChanged(double)));
	connect(newControls_->undulatorHarmonicSetpoint(), SIGNAL(valueChanged(double)),
			this, SLOT(onUndulatorHarmonicSetpointPVChanged(double)));
	connect(newControls_->exitSlitPositionSetpoint(), SIGNAL(valueChanged(double)),
			this, SLOT(onExitSlitPositionSetpointPVChanged(double)));
	connect(newControls_->exitSlitPositionStop(), SIGNAL(valueChanged(double)),
			this, SLOT(onExitSlitPositionStopPVChanged(double)));


}

void SGMEnergyCoordinator::onEnergyControlValueChanged(double energy)
{
	if(!newControls_->energyFeedback()->withinTolerance(energy)) {

		qDebug() << QString("New Energy Feedback set to %1").arg(energy);
		newControls_->energyFeedback()->move(energy);
	}
}

void SGMEnergyCoordinator::onEnergyControlIsMovingChanged(bool isMoving)
{
	double movingValue;
	if(isMoving) {

		movingValue = 1;
	} else {

		movingValue = 0;
	}

	if(!newControls_->energyStatus()->withinTolerance(movingValue)) {

		qDebug() << QString("New Energy Status set to %1").arg(movingValue);
		newControls_->energyStatus()->move(movingValue);
	}
}

void SGMEnergyCoordinator::onEnergyControlGratingTranslationOptimizationModeChanged(SGMEnergyPosition::GratingTranslationOptimizationMode gratingTranslationOptimizationMode)
{
	double gratingOptimizationModeValue = 0;
	switch(gratingTranslationOptimizationMode) {

	case SGMEnergyPosition::ManualMode:
		gratingOptimizationModeValue = 0.0;
		break;
	case SGMEnergyPosition::OptimizeFlux:
		gratingOptimizationModeValue = 1.0;
		break;
	case SGMEnergyPosition::OptimizeResolution:
		gratingOptimizationModeValue = 2.0;
		break;
	}

	if(!newControls_->gratingTranslationOptimizationModeFeedback()->withinTolerance(gratingOptimizationModeValue)) {

		qDebug() << QString("New Grating Translation set to %1").arg(gratingOptimizationModeValue);
		newControls_->gratingTranslationOptimizationModeFeedback()->move(gratingOptimizationModeValue);
	}
}

void SGMEnergyCoordinator::onEnergyControlUndulatorOffsetChanged(double undulatorOffset)
{
	if(!newControls_->undulatorOffset()->withinTolerance(undulatorOffset)) {

		qDebug() << QString("New Undulator Offset set to %1").arg(undulatorOffset);
		newControls_->undulatorOffset()->move(undulatorOffset);
	}
}

void SGMEnergyCoordinator::onEnergyControlUndulatorHarmonicChanged(SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic)
{
	if(undulatorHarmonic != SGMUndulatorSupport::UnknownUndulatorHarmonic) {

		double undulatorHarmonicValue = 0;
		switch(undulatorHarmonic) {
		case SGMUndulatorSupport::FirstHarmonic:
			undulatorHarmonicValue = 0;
			break;
		case SGMUndulatorSupport::ThirdHarmonic:
			undulatorHarmonicValue = 1;
			break;
		case SGMUndulatorSupport::UnknownUndulatorHarmonic:
			break;
		}

		if(!newControls_->undulatorHarmonicFeedback()->withinTolerance(undulatorHarmonicValue)) {

			qDebug() << QString("New Undulator Harmonic value set to %1").arg(undulatorHarmonicValue);
			newControls_->undulatorHarmonicFeedback()->move(undulatorHarmonicValue);
		}

	} else {

		qDebug() << "Could not set Undulator Harmonic. Unknown value";
	}
}

void SGMEnergyCoordinator::onEnergyControlUndulatorTrackingChanged(bool isTracking)
{
	double isTrackingValue;
	if(isTracking) {

		isTrackingValue = 1;
	} else {

		isTrackingValue = 0;
	}

	if(!newControls_->undulatorTracking()->withinTolerance(isTrackingValue)) {

		qDebug() << QString("New Undulator Tracking set to %1").arg(isTracking);
		newControls_->undulatorTracking()->move(isTrackingValue);
	}
}

void SGMEnergyCoordinator::onEnergyControlExitSlitPositionTrackingChanged(bool isTracking)
{

	double isTrackingValue;
	if(isTracking) {

		isTrackingValue = 1;
	} else {

		isTrackingValue = 0;
	}

	if(!newControls_->exitSlitPositionTracking()->withinTolerance(isTrackingValue)) {

		qDebug() << QString("New Exit Slit Tracking set to %1").arg(isTracking);
		newControls_->exitSlitPositionTracking()->move(isTrackingValue);
	}
}

void SGMEnergyCoordinator::onOldGratingAnglePVChanged(double value)
{
	if(!newControls_->gratingAngleFeedback()->withinTolerance(value)) {

		qDebug() << QString("New grating angle set to %1").arg(value);
		newControls_->gratingAngleFeedback()->move(value);
	}
}

void SGMEnergyCoordinator::onOldGratingAnglePVMovingChanged(bool isMoving)
{
	double movingValue;
	if(isMoving) {

		movingValue = 1;
	} else {

		movingValue = 0;
	}

	if(!newControls_->gratingAngleStatus()->withinTolerance(movingValue)) {

		qDebug() << QString("New Grating Angle set to %1").arg(movingValue);
		newControls_->gratingAngleStatus()->move(movingValue);
	}
}

void SGMEnergyCoordinator::onOldGratingTranslationStepPVChanged(double value)
{
	SGMGratingSupport::GratingTranslation gratingTranslation =
			SGMGratingSupport::encoderStepsToEnum(value);

	double newGratingTranslationPVValue;
	switch(gratingTranslation) {

	case SGMGratingSupport::LowGrating:
		newGratingTranslationPVValue = 0;
		break;
	case SGMGratingSupport::MediumGrating:
		newGratingTranslationPVValue = 1;
		break;
	case SGMGratingSupport::HighGrating:
		newGratingTranslationPVValue = 2;
		break;
	default:
		newGratingTranslationPVValue = 3;
	}

	if(!newControls_->gratingTranslationFeedback()->withinTolerance(newGratingTranslationPVValue)) {

		qDebug() << QString("New Grating Translation set to %1").arg(newGratingTranslationPVValue);
		newControls_->gratingTranslationFeedback()->move(newGratingTranslationPVValue);
	}
}

void SGMEnergyCoordinator::onOldGratingTranslationStepPVMovingChanged(bool isMoving)
{
	double movingValue;
	if(isMoving) {

		movingValue = 1;
	} else {

		movingValue = 0;
	}

	if(!newControls_->gratingTranslationStatus()->withinTolerance(movingValue)) {

		qDebug() << QString("New Grating Translation Status set to %1").arg(movingValue);
		newControls_->gratingTranslationStatus()->move(movingValue);
	}
}

void SGMEnergyCoordinator::onOldUndulatorPositionPVChanged(double value)
{
	if(!newControls_->undulatorPositionFeedback()->withinTolerance(value)) {

		qDebug() << QString("New Undulator Position set to %1").arg(value);
		newControls_->undulatorPositionFeedback()->move(value);
	}
}

void SGMEnergyCoordinator::onOldUndulatorPositionPVMovingChanged(bool isMoving)
{
	double movingValue;
	if(isMoving) {

		movingValue = 1;
	} else {

		movingValue = 0;
	}

	if(!newControls_->undulatorPositionStatus()->withinTolerance(movingValue)) {

		qDebug() << QString("New Undulator Position Status set to %1").arg(movingValue);
		newControls_->undulatorPositionStatus()->move(movingValue);
	}
}

void SGMEnergyCoordinator::onOldExitSlitPositionPVChanged(double value)
{
	if(!newControls_->exitSlitPositionFeedback()->withinTolerance(value)) {

		qDebug() << QString("New Exit Slit Position set to %1").arg(value);
		newControls_->exitSlitPositionFeedback()->move(value);
	}
}

void SGMEnergyCoordinator::onOldExitSlitPositionPVMovingChanged(bool isMoving)
{
	double movingValue;
	if(isMoving) {

		movingValue = 1;
	} else {

		movingValue = 0;
	}

	if(!newControls_->exitSlitPositionStatus()->withinTolerance(movingValue)) {

		qDebug() << QString("New Exit Slit Position Status set to %1").arg(movingValue);
		newControls_->exitSlitPositionStatus()->move(movingValue);
	}
}

void SGMEnergyCoordinator::onEnergyStopPVChanged(double)
{
	if(newControls_->energyStop()->withinTolerance(1.0)) {

		qDebug() << "Stopping coordinated energy motion";
		energyControlCoordinator_->stop();
		newControls_->energyStop()->move(0);
	}

	if(!newControls_->energyStop()->withinTolerance(0.0)) {
		newControls_->energyStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onGratingAngleSetpointPVChanged(double value)
{
	qDebug() << QString("Moving grating angle to %1").arg(value);
	energyControlCoordinator_->gratingAngleControl()->move(value);
}

void SGMEnergyCoordinator::onGratingAngleStopPVChanged(double)
{
	if(newControls_->gratingAngleStop()->withinTolerance(1.0)) {

		qDebug() << "Stopping grating angle motion";
		energyControlCoordinator_->gratingAngleControl()->stop();
		newControls_->gratingAngleStop()->move(0);
	}

	if(!newControls_->gratingAngleStop()->withinTolerance(0.0)) {
		newControls_->gratingAngleStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onGratingTranslationSetpointPVChanged(double)
{
	if(energyControlCoordinator_->gratingTranslationOptimizationMode() == SGMEnergyPosition::ManualMode) {

		SGMGratingSupport::GratingTranslation enumValue;

		if(energyControlCoordinator_->gratingTranslationControl()->withinTolerance(0.0)) {

			enumValue = SGMGratingSupport::LowGrating;
		} else if (energyControlCoordinator_->gratingTranslationControl()->withinTolerance(1.0)) {

			enumValue = SGMGratingSupport::MediumGrating;
		} else if (energyControlCoordinator_->gratingTranslationControl()->withinTolerance(2.0)) {

			enumValue = SGMGratingSupport::HighGrating;
		} else {

			qDebug() << "Unknown grating translation set - ignoring value.";
		}

		double gratingTranslationStepValue = SGMGratingSupport::enumToEncoderSteps(enumValue);

		if(!energyControlCoordinator_->gratingTranslationControl()->withinTolerance(gratingTranslationStepValue)) {

			qDebug() << QString("Setting grating translation step to %1").arg(gratingTranslationStepValue);
			energyControlCoordinator_->gratingTranslationControl()->move(gratingTranslationStepValue);
		}
	}

}

void SGMEnergyCoordinator::onGratingTranslationStopPVChanged(double)
{
	if(newControls_->gratingTranslationStop()->withinTolerance(1.0)) {

		qDebug() << "Stopping grating translation motion";
		energyControlCoordinator_->gratingTranslationControl()->stop();
		newControls_->gratingTranslationStop()->move(0);
	}

	if(!newControls_->gratingTranslationStop()->withinTolerance(0.0)) {
		newControls_->gratingTranslationStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onGratingTranslationOptimizationModeSetpointPVChanged(double)
{
	SGMEnergyPosition::GratingTranslationOptimizationMode optimizationMode;

	if(newControls_->gratingTranslationOptimizationModeSetpoint()->withinTolerance(0)) {

		optimizationMode = SGMEnergyPosition::ManualMode;
	} else if(newControls_->gratingTranslationOptimizationModeSetpoint()->withinTolerance(1)) {

		optimizationMode = SGMEnergyPosition::OptimizeFlux;
	} else if(newControls_->gratingTranslationOptimizationModeSetpoint()->withinTolerance(2)) {

		optimizationMode = SGMEnergyPosition::OptimizeResolution;
	} else {

		qDebug() << "Unknown grating translation optimization value set - ignoring value";
		return;
	}

	qDebug() << QString("Setting grating translation optimization mode to %1").arg(optimizationMode);
	energyControlCoordinator_->setGratingTranslationOptimizationMode(optimizationMode);
}

void SGMEnergyCoordinator::onUndulatorSetpointPVChanged(double value)
{
	qDebug() << QString("Setting undulator position to %1").arg(value);
	energyControlCoordinator_->undulatorPositionControl()->move(value);
}

void SGMEnergyCoordinator::onUndulatorStopPVChanged(double)
{
	if(newControls_->undulatorPositionStop()->withinTolerance(1.0)) {

		qDebug() << "Stopping undulator motion";
		energyControlCoordinator_->undulatorPositionControl()->stop();
		newControls_->undulatorPositionStop()->move(0);
	}

	if(!newControls_->undulatorPositionStop()->withinTolerance(0.0)) {
		newControls_->undulatorPositionStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onUndulatorOffsetPVChanged(double value)
{
	qDebug() << QString("Setting undulator offset to %1").arg(value);
	energyControlCoordinator_->setUndulatorOffset(value);
}

void SGMEnergyCoordinator::onUndulatorHarmonicSetpointPVChanged(double)
{

	SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic;
	if(newControls_->undulatorHarmonicSetpoint()->withinTolerance(0)) {

		undulatorHarmonic = SGMUndulatorSupport::FirstHarmonic;
	} else if(newControls_->undulatorHarmonicSetpoint()->withinTolerance(1)) {

		undulatorHarmonic = SGMUndulatorSupport::ThirdHarmonic;
	} else {

		qDebug() << "Unknown undulator harmonic value set - ignoring value";
		return;
	}

	qDebug() << QString("Setting undulator harmonic to %1").arg(undulatorHarmonic);
	energyControlCoordinator_->setUndulatorHarmonic(undulatorHarmonic);
}

void SGMEnergyCoordinator::onUndulatorTrackingPVChanged(double)
{
	if(newControls_->undulatorTracking()->withinTolerance(0)) {

		qDebug() << "Switching off undulator tracking";
		energyControlCoordinator_->setUndulatorTracking(false);
	} else {

		qDebug() << "Switching on undulator tracking";
		energyControlCoordinator_->setUndulatorTracking(true);
	}
}

void SGMEnergyCoordinator::onExitSlitPositionSetpointPVChanged(double value)
{
	qDebug() << QString("Setting exit slit position to %1").arg(value);
	energyControlCoordinator_->exitSlitPositionControl()->move(value);
}

void SGMEnergyCoordinator::onExitSlitPositionStopPVChanged(double)
{
	if(newControls_->exitSlitPositionStop()->withinTolerance(1.0)) {

		qDebug() << "Stopping exit slit position motion";
		energyControlCoordinator_->exitSlitPositionControl()->stop();
		newControls_->exitSlitPositionStop()->move(0);
	}

	if(!newControls_->exitSlitPositionStop()->withinTolerance(0.0)) {
		newControls_->exitSlitPositionStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onExitSlitPositionTrackingPVChanged(double)
{
	if(newControls_->exitSlitPositionTracking()->withinTolerance(0)) {

		qDebug() << "Switching off exit slit tracking";
		energyControlCoordinator_->setExitSlitPositionTracking(false);
	} else {

		qDebug() << "Switching on exit slit tracking";
		energyControlCoordinator_->setExitSlitPositionTracking(true);
	}
}
