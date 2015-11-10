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
	pvsConnectedOnce_ = false;
	energyControlConnectedOnce_ = false;

	energySetpointInitialized_ = false;
	gratingAngleSetpointInitialized_ = false;
	gratingTranslationSetpointInitialized_ = false;
	gratingTranslationOptimizationModeSetpointInitialized_ = false;
	undulatorPositionSetpointInitialized_ = false;
	undulatorHarmonicSetpointInitialized_ = false;
	undulatorOffsetSetpointInitialized_ = false;
	exitSlitPositionSetpointInitialized_ = false;

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
	if(oldControls_->isConnected() &&
			newControls_->isConnected() &&
			!pvsConnectedOnce_) {

		qDebug() << "PVs connected.";
		pvsConnectedOnce_ = true;

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

			onEnergyControlConnected(true);
		} else {

			connect(energyControlCoordinator_, SIGNAL(connected(bool)), this, SLOT(onEnergyControlConnected(bool)));
		}
	}
}

// Connecting the energy control coordinator output to the new PVs

void SGMEnergyCoordinator::onEnergyControlConnected(bool isConnected)
{
	if(isConnected && !energyControlConnectedOnce_) {

		qDebug() << "Energy control connected.";
		energyControlConnectedOnce_ = true;

		// Initialize values now the energy control coordinator is connected
		// Initialize feedback values
		qDebug() << "Initializing Values:";
		onEnergyControlValueChanged(energyControlCoordinator_->value());
		onEnergyControlIsMovingChanged(energyControlCoordinator_->isMoving());
		onEnergyControlGratingTranslationOptimizationModeChanged(energyControlCoordinator_->gratingTranslationOptimizationMode());

		onEnergyControlUndulatorOffsetChanged(energyControlCoordinator_->undulatorOffset());
		onEnergyControlUndulatorHarmonicChanged(energyControlCoordinator_->undulatorHarmonic());
		onEnergyControlUndulatorTrackingChanged(energyControlCoordinator_->isUndulatorTracking());
		onEnergyControlExitSlitPositionTrackingChanged(energyControlCoordinator_->isExitSlitPositionTracking());


		qDebug() << QString("Initializing grating angle fbk to %1").arg(energyControlCoordinator_->gratingAngleControl()->value());
		onOldGratingAnglePVChanged(energyControlCoordinator_->gratingAngleControl()->value());
		onOldGratingAnglePVMovingChanged(energyControlCoordinator_->gratingAngleControl()->isMoving());
		onOldGratingTranslationStepPVChanged(energyControlCoordinator_->gratingTranslationControl()->value());
		onOldGratingTranslationStepPVMovingChanged(energyControlCoordinator_->gratingTranslationControl()->isMoving());
		qDebug() << QString("Initializing undulator position fbk to %1").arg(energyControlCoordinator_->undulatorPositionControl()->value());
		onOldUndulatorPositionPVChanged(energyControlCoordinator_->undulatorPositionControl()->value());
		onOldUndulatorPositionPVMovingChanged(energyControlCoordinator_->undulatorPositionControl()->isMoving());
		qDebug() << QString("Initializing exit slit position fbk to %1").arg(energyControlCoordinator_->exitSlitPositionControl()->value());
		onOldExitSlitPositionPVChanged(energyControlCoordinator_->exitSlitPositionControl()->value());
		onOldExitSlitPositionPVMovingChanged(energyControlCoordinator_->exitSlitPositionControl()->isMoving());


		// Initialize setpoint values
		// Energy
		if(!newControls_->energySetpoint()->withinTolerance(energyControlCoordinator_->value())) {

			qDebug() << QString("Initializing new energy setpoint PV to %1").arg(energyControlCoordinator_->value());
			newControls_->energySetpoint()->move(energyControlCoordinator_->value());
		} else {
			energySetpointInitialized_ = true;
		}


		// Grating angle setpoint
		if(newControls_->gratingAngleSetpoint()->withinTolerance(energyControlCoordinator_->gratingAngleControl()->value())) {

			qDebug() << QString("Initializing new grating angle setpoint PV to %1").arg(energyControlCoordinator_->gratingAngleControl()->value());
			newControls_->gratingAngleSetpoint()->move(energyControlCoordinator_->gratingAngleControl()->value());
		} else {
			gratingAngleSetpointInitialized_ = true;
		}

		// Grating translation setpoint
		SGMGratingSupport::GratingTranslation initialGratingTranslation =
				SGMGratingSupport::encoderCountToEnum(energyControlCoordinator_->gratingTranslationControl()->value());

		double initialGratingTranslationValue;
		switch(initialGratingTranslation) {
		case SGMGratingSupport::LowGrating:
			initialGratingTranslationValue = 0;
			break;
		case SGMGratingSupport::MediumGrating:
			initialGratingTranslationValue = 1;
			break;
		case SGMGratingSupport::HighGrating:
			initialGratingTranslationValue = 2;
			break;
		case SGMGratingSupport::UnknownGrating:
			initialGratingTranslationValue = 3;
		}

		if(!newControls_->gratingTranslationSetpoint()->withinTolerance(initialGratingTranslationValue)
				&& initialGratingTranslation != SGMGratingSupport::UnknownGrating) {

			qDebug() << QString("Initializing new grating translation setpoint PV to %1").arg(initialGratingTranslationValue);
			newControls_->gratingTranslationSetpoint()->move(initialGratingTranslationValue);
		} else {
			gratingTranslationSetpointInitialized_ = true;
		}

		// Grating translation optimization setpoint
		SGMEnergyPosition::GratingTranslationOptimizationMode initialOptimizationMode =
				energyControlCoordinator_->gratingTranslationOptimizationMode();

		double initialOptimizationModeValue;
		switch(initialOptimizationMode) {
		case SGMEnergyPosition::ManualMode:
			initialOptimizationModeValue = 0;
			break;
		case SGMEnergyPosition::OptimizeFlux:
			initialOptimizationModeValue = 1;
			break;
		default:
			initialOptimizationModeValue = 2;
		}

		if(!newControls_->gratingTranslationOptimizationModeSetpoint()->withinTolerance(initialOptimizationModeValue)) {

			qDebug() << QString("Initializing new grating optimization mode setpoint PV to %1").arg(initialOptimizationModeValue);
			newControls_->gratingTranslationOptimizationModeSetpoint()->move(initialOptimizationModeValue);
		} else {
			gratingTranslationOptimizationModeSetpointInitialized_ = true;
		}


		// Undulator position
		if(!newControls_->undulatorPositionSetpoint()->withinTolerance(energyControlCoordinator_->undulatorPositionControl()->value())) {

			qDebug() << QString("Initializing new undulator position setpoint PV to %1").arg(energyControlCoordinator_->undulatorPositionControl()->value());
			newControls_->undulatorPositionSetpoint()->move(energyControlCoordinator_->undulatorPositionControl()->value());
		} else {
			undulatorPositionSetpointInitialized_ = true;
		}

		// Undulator harmonic
		SGMUndulatorSupport::UndulatorHarmonic initialUndulatorHarmonic = energyControlCoordinator_->undulatorHarmonic();
		double initialUndulatorHarmonicValue;
		switch(initialUndulatorHarmonic) {
		case SGMUndulatorSupport::FirstHarmonic:
			initialUndulatorHarmonicValue = 0;
			break;
		default:
			initialUndulatorHarmonicValue = 1;
		}

		if(!newControls_->undulatorHarmonicSetpoint()->withinTolerance(initialUndulatorHarmonicValue)) {

			qDebug() << QString("Initializing new undulator harmonic setpoint PV to %1").arg(initialUndulatorHarmonicValue);
			newControls_->undulatorPositionSetpoint()->move(initialUndulatorHarmonicValue);
		} else {
			undulatorHarmonicSetpointInitialized_ = true;
		}

		// Exit slit position
		if(!newControls_->exitSlitPositionSetpoint()->withinTolerance(energyControlCoordinator_->exitSlitPositionControl()->setpoint())) {

			qDebug() << QString("Initializing new exit slit position setpoint PV to %1").arg(energyControlCoordinator_->exitSlitPositionControl()->setpoint());
			newControls_->exitSlitPositionSetpoint()->move(energyControlCoordinator_->exitSlitPositionControl()->setpoint());
		} else {
			exitSlitPositionSetpointInitialized_ = true;
		}

		qDebug() << "Initialization Complete";

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
				this, SLOT(onEnergySetpointPVChanged(double)));

		connect(newControls_->energyStop(), SIGNAL(valueChanged(double)),
				this, SLOT(onEnergyStopPVChanged(double)));

		connect(newControls_->energyTrajectoryStart(), SIGNAL(valueChanged(double)),
				this, SLOT(onEnergyTrajectoryStartPVChanged(double)));

		connect(newControls_->gratingAngleSetpoint(), SIGNAL(valueChanged(double)),
				this, SLOT(onGratingAngleSetpointPVChanged(double)));

		connect(newControls_->gratingAngleStop(), SIGNAL(valueChanged(double)),
				this, SLOT(onGratingAngleStopPVChanged(double)));

		connect(newControls_->gratingTranslationOptimizationModeSetpoint(), SIGNAL(valueChanged(double)),
				this, SLOT(onGratingTranslationOptimizationModeSetpointPVChanged(double)));

		connect(newControls_->gratingTranslationSetpoint(), SIGNAL(valueChanged(double)),
				this, SLOT(onGratingTranslationSetpointPVChanged(double)));

		connect(newControls_->gratingTranslationStop(), SIGNAL(valueChanged(double)),
				this, SLOT(onGratingTranslationStopPVChanged(double)));

		connect(newControls_->undulatorPositionSetpoint(), SIGNAL(valueChanged(double)),
				this, SLOT(onUndulatorSetpointPVChanged(double)));

		connect(newControls_->undulatorPositionStop(), SIGNAL(valueChanged(double)),
				this, SLOT(onUndulatorStopPVChanged(double)));

		connect(newControls_->undulatorOffset(), SIGNAL(valueChanged(double)),
				this, SLOT(onUndulatorOffsetPVChanged(double)));

		connect(newControls_->undulatorHarmonicSetpoint(), SIGNAL(valueChanged(double)),
				this, SLOT(onUndulatorHarmonicSetpointPVChanged(double)));

		connect(newControls_->undulatorTracking(), SIGNAL(valueChanged(double)),
				this, SLOT(onUndulatorTrackingPVChanged(double)));

		connect(newControls_->exitSlitPositionSetpoint(), SIGNAL(valueChanged(double)),
				this, SLOT(onExitSlitPositionSetpointPVChanged(double)));

		connect(newControls_->exitSlitPositionStop(), SIGNAL(valueChanged(double)),
				this, SLOT(onExitSlitPositionStopPVChanged(double)));

		connect(newControls_->exitSlitPositionTracking(), SIGNAL(valueChanged(double)),
				this, SLOT(onExitSlitPositionTrackingPVChanged(double)));

	}
}

void SGMEnergyCoordinator::onEnergyControlValueChanged(double energy)
{
	if(!newControls_->energyFeedback()->withinTolerance(energy)) {

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

	qDebug() << QString("Setting new energy status PV to %1").arg(movingValue);
	newControls_->energyStatus()->move(movingValue);
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

		qDebug() << QString("Setting new grating translation optimization mode fbk PV to %1").arg(gratingOptimizationModeValue);
		newControls_->gratingTranslationOptimizationModeFeedback()->move(gratingOptimizationModeValue);
	}
}

void SGMEnergyCoordinator::onEnergyControlUndulatorOffsetChanged(double undulatorOffset)
{
	if(!newControls_->undulatorOffset()->withinTolerance(undulatorOffset)) {

		qDebug() << QString("Setting new undulator offset PV to %1").arg(undulatorOffset);
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

			qDebug() << QString("Setting new undulator harmonic fbk PV to").arg(undulatorHarmonicValue);
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

		qDebug() << QString("Setting new undulator tracking PV to %1").arg(isTracking);
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

		qDebug() << QString("Setting new exit slit tracking PV to %1").arg(isTracking);
		newControls_->exitSlitPositionTracking()->move(isTrackingValue);
	}
}

void SGMEnergyCoordinator::onOldGratingAnglePVChanged(double value)
{
	if(!newControls_->gratingAngleFeedback()->withinTolerance(value)) {

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

	qDebug() << QString("Setting new grating angle status PV to %1").arg(movingValue);
	newControls_->gratingAngleStatus()->move(movingValue);
}

void SGMEnergyCoordinator::onOldGratingTranslationStepPVChanged(double value)
{
	SGMGratingSupport::GratingTranslation gratingTranslation =
			SGMGratingSupport::encoderCountToEnum(value);

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

		qDebug() << QString("Setting new grating translation fbk PV to %1").arg(newGratingTranslationPVValue);
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

	qDebug() << QString("Setting new grating translation status PV to %1").arg(movingValue);
	newControls_->gratingTranslationStatus()->move(movingValue);
}

void SGMEnergyCoordinator::onOldUndulatorPositionPVChanged(double value)
{
	if(!newControls_->undulatorPositionFeedback()->withinTolerance(value)) {

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

	qDebug() << QString("Setting new undulator position status PV to %1").arg(movingValue);
	newControls_->undulatorPositionStatus()->move(movingValue);
}

void SGMEnergyCoordinator::onOldExitSlitPositionPVChanged(double value)
{
	if(!newControls_->exitSlitPositionFeedback()->withinTolerance(value)) {

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

	qDebug() << QString("Setting new exit slit position status PV to %1").arg(movingValue);
	newControls_->exitSlitPositionStatus()->move(movingValue);

}

void SGMEnergyCoordinator::onEnergySetpointPVChanged(double value)
{
	if(energySetpointInitialized_)  {

		qDebug() << QString("Forwarding energy setpoint value of %1 to energy control").arg(value);
		AMControl* controlVersion = energyControlCoordinator_;
		controlVersion->move(value);
	} else {
		energySetpointInitialized_ = true;
	}
}

void SGMEnergyCoordinator::onEnergyStopPVChanged(double)
{
	if(newControls_->energyStop()->withinTolerance(1.0)) {

		qDebug() <<"Forwarding stop energy command to energy control";
		energyControlCoordinator_->stop();
		newControls_->energyStop()->move(0);
	}

	if(!newControls_->energyStop()->withinTolerance(0.0)) {
		newControls_->energyStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onEnergyTrajectoryStartPVChanged(double value)
{
	qDebug() << "\tTrajectory start value of" << value << "received";
	if(newControls_->energyTrajectoryStart()->withinTolerance(1.0)) {

		double startpoint = newControls_->energyTrajectoryStartpoint()->value();
		double endpoint = newControls_->energyTrajectoryEndpoint()->value();
		double time = newControls_->energyTrajectoryTime()->value();
		double velocity = (endpoint - startpoint) / time;

		if(energyControlCoordinator_->move(startpoint, endpoint, velocity) == AMControl::NoFailure) {
			qDebug() << "Moving energy from" << startpoint << "to" << endpoint << "at" << velocity << "eV/s";
		}
	}

	if(!newControls_->gratingAngleStop()->withinTolerance(0.0)) {
		newControls_->gratingAngleStop()->move(0.0);
	}

}

void SGMEnergyCoordinator::onGratingAngleSetpointPVChanged(double value)
{
	if(gratingAngleSetpointInitialized_) {

		qDebug() << QString("Forwarding grating angle setpoint value of %1 to energy control").arg(value);
		energyControlCoordinator_->gratingAngleControl()->move(value);
	} else {
		gratingAngleSetpointInitialized_ = true;
	}
}

void SGMEnergyCoordinator::onGratingAngleStopPVChanged(double)
{
	if(newControls_->gratingAngleStop()->withinTolerance(1.0)) {

		qDebug() << "Forwarding stop grating angle command to energy control";
		energyControlCoordinator_->gratingAngleControl()->stop();
		newControls_->gratingAngleStop()->move(0);
	}

	if(!newControls_->gratingAngleStop()->withinTolerance(0.0)) {
		newControls_->gratingAngleStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onGratingTranslationSetpointPVChanged(double value)
{
	qDebug() << "Grating translation setpoint value of" << value << "received";
	if(gratingTranslationSetpointInitialized_) {
		qDebug() << "\tIs Initialized";
		if(energyControlCoordinator_->gratingTranslationOptimizationMode() == SGMEnergyPosition::ManualMode) {
			qDebug() << "\tIs in manual mode";
			SGMGratingSupport::GratingTranslation enumValue = SGMGratingSupport::UnknownGrating;

			if(newControls_->gratingTranslationSetpoint()->withinTolerance(0.0)) {

				enumValue = SGMGratingSupport::LowGrating;
			} else if (newControls_->gratingTranslationSetpoint()->withinTolerance(1.0)) {

				enumValue = SGMGratingSupport::MediumGrating;
			} else if (newControls_->gratingTranslationSetpoint()->withinTolerance(2.0)) {

				enumValue = SGMGratingSupport::HighGrating;
			} else {

				qDebug() << "Unknown grating translation set - ignoring value.";
				return;
			}

			double gratingTranslationStepValue = SGMGratingSupport::enumToEncoderCount(enumValue);

			qDebug() << QString("Forwarding grating translation setpoint value of %1 to energy control").arg(gratingTranslationStepValue);
			energyControlCoordinator_->gratingTranslationControl()->move(gratingTranslationStepValue);
		} else {

			// If we are not in manual mode, we push a valid valid back at the setpoint PV.
			newControls_->gratingTranslationSetpoint()->move(newControls_->gratingTranslationFeedback()->value());
		}
	} else {
		gratingTranslationSetpointInitialized_ = true;
	}
}

void SGMEnergyCoordinator::onGratingTranslationStopPVChanged(double)
{
	if(newControls_->gratingTranslationStop()->withinTolerance(1.0)) {

		qDebug() << "Forwarding stop grating translation command to energy control";
		energyControlCoordinator_->gratingTranslationControl()->stop();
		newControls_->gratingTranslationStop()->move(0);
	}

	if(!newControls_->gratingTranslationStop()->withinTolerance(0.0)) {
		newControls_->gratingTranslationStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onGratingTranslationOptimizationModeSetpointPVChanged(double)
{
	if(gratingTranslationOptimizationModeSetpointInitialized_) {
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

		qDebug() << QString("Forwarding grating translation optimization mode setpoint value of %1 to energy control").arg(optimizationMode);
		energyControlCoordinator_->setGratingTranslationOptimizationMode(optimizationMode);
	} else {
		gratingTranslationOptimizationModeSetpointInitialized_ = true;
	}
}

void SGMEnergyCoordinator::onUndulatorSetpointPVChanged(double value)
{
	if(undulatorPositionSetpointInitialized_) {
		qDebug() << QString("Forwarding grating undulator position setpoint value of %1 to energy control").arg(value);
		energyControlCoordinator_->undulatorPositionControl()->move(value);
	} else {
		undulatorPositionSetpointInitialized_ = true;
	}
}

void SGMEnergyCoordinator::onUndulatorStopPVChanged(double)
{
	if(newControls_->undulatorPositionStop()->withinTolerance(1.0)) {

		qDebug() << "Forwarding stop undulator position command to energy control";
		energyControlCoordinator_->undulatorPositionControl()->stop();
		newControls_->undulatorPositionStop()->move(0);
	}

	if(!newControls_->undulatorPositionStop()->withinTolerance(0.0)) {
		newControls_->undulatorPositionStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onUndulatorOffsetPVChanged(double value)
{
	if(undulatorOffsetSetpointInitialized_) {

		qDebug() << QString("Forwarding undulator offset setpoint value of %1 to energy control").arg(value);
		energyControlCoordinator_->setUndulatorOffset(value);
	} else {
		undulatorOffsetSetpointInitialized_ = true;
	}
}

void SGMEnergyCoordinator::onUndulatorHarmonicSetpointPVChanged(double)
{
	if(undulatorHarmonicSetpointInitialized_) {
		SGMUndulatorSupport::UndulatorHarmonic undulatorHarmonic;
		if(newControls_->undulatorHarmonicSetpoint()->withinTolerance(0)) {

			undulatorHarmonic = SGMUndulatorSupport::FirstHarmonic;
		} else if(newControls_->undulatorHarmonicSetpoint()->withinTolerance(1)) {

			undulatorHarmonic = SGMUndulatorSupport::ThirdHarmonic;
		} else {

			qDebug() << "Unknown undulator harmonic value set - ignoring value";
			return;
		}

		if(energyControlCoordinator_->undulatorHarmonic() != undulatorHarmonic) {
			qDebug() << QString("Forwarding undulator harmonic setpoint value of %1 to energy control").arg(undulatorHarmonic);
			energyControlCoordinator_->setUndulatorHarmonic(undulatorHarmonic);
		}
	} else {
		undulatorHarmonicSetpointInitialized_ = true;
	}

}

void SGMEnergyCoordinator::onUndulatorTrackingPVChanged(double)
{
	bool isTracking;

	if(newControls_->undulatorTracking()->withinTolerance(0)) {

		isTracking = false;
	} else {

		isTracking = true;
	}

	if(energyControlCoordinator_->isUndulatorTracking() != isTracking) {

		qDebug() << QString("Forwarding undulator tracking setpoint value of %1 to energy control").arg(isTracking);
		energyControlCoordinator_->setUndulatorTracking(isTracking);
	}
}

void SGMEnergyCoordinator::onExitSlitPositionSetpointPVChanged(double value)
{
	if(exitSlitPositionSetpointInitialized_) {
		qDebug() << QString("Forwarding exit slit position setpoint value of %1 to energy control").arg(value);
		energyControlCoordinator_->exitSlitPositionControl()->move(value);
	} else {
		exitSlitPositionSetpointInitialized_ = true;
	}

}

void SGMEnergyCoordinator::onExitSlitPositionStopPVChanged(double)
{
	if(newControls_->exitSlitPositionStop()->withinTolerance(1.0)) {

		qDebug() << "Forwarding stop exit slit position command to energy control";
		energyControlCoordinator_->exitSlitPositionControl()->stop();
		newControls_->exitSlitPositionStop()->move(0);
	}

	if(!newControls_->exitSlitPositionStop()->withinTolerance(0.0)) {
		newControls_->exitSlitPositionStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onExitSlitPositionTrackingPVChanged(double)
{
	bool isTracking;

	if(newControls_->exitSlitPositionTracking()->withinTolerance(0)) {

		isTracking = false;
	} else {

		isTracking = true;
	}

	if(energyControlCoordinator_->isExitSlitPositionTracking() != isTracking) {

		qDebug() << QString("Forwarding exit slit position tracking setpoint value of %1 to energy control").arg(isTracking);
		energyControlCoordinator_->setExitSlitPositionTracking(isTracking);
	}
}



