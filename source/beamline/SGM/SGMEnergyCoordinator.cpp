#include "SGMEnergyCoordinator.h"
#include "beamline/SGM/energy/SGMEnergyCoordinatorControl.h"

#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"
#include "beamline/SGM/SGMNewEnergyPVSet.h"
#include "beamline/SGM/SGMOldEnergyPVSet.h"
#include "beamline/SGM/SGMBeamCoordinatorControl.h"
#include "beamline/SGM/SGMNewBeamOnControls.h"
#include <QApplication>

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
	  * 5: Constructing the beam on control pseudo-motor
	  * 6: Waiting for the beam on control to respond that it is connected.
	  */
	beamControlCoordinator_ = 0;
	energyControlCoordinator_ = 0;

	pvsConnectedOnce_ = false;
	energyControlConnectedOnce_ = false;
	beamOnControlConnectedOnce_ = false;

	energySetpointInitialized_ = false;
	gratingAngleSetpointInitialized_ = false;
	gratingTranslationSetpointInitialized_ = false;
	gratingTranslationOptimizationModeSetpointInitialized_ = false;
	undulatorPositionSetpointInitialized_ = false;
	undulatorHarmonicSetpointInitialized_ = false;
	undulatorOffsetSetpointInitialized_ = false;
	exitSlitPositionSetpointInitialized_ = false;
	beamOnInitialized_ = false;

	oldEnergyControls_ = new SGMOldEnergyPVSet(this);
	newEnergyControls_ = new SGMNewEnergyPVSet(this);
	newBeamOnControls_ = new SGMNewBeamOnControls(this);

	qDebug() << "Waiting for PVs to be connected...";
	connect(oldEnergyControls_, SIGNAL(connected(bool)), this, SLOT(onPVSetsConnected()));
	connect(newEnergyControls_, SIGNAL(connected(bool)), this, SLOT(onPVSetsConnected()));
	connect(newBeamOnControls_, SIGNAL(connected(bool)), this, SLOT(onPVSetsConnected()));

	if(oldEnergyControls_->isConnected() && newEnergyControls_->isConnected() && newBeamOnControls_->isConnected()) {

		onPVSetsConnected();
	}
}

void SGMEnergyCoordinator::onPVSetsConnected()
{
	if(oldEnergyControls_->isConnected() &&
			newEnergyControls_->isConnected() &&
	                newBeamOnControls_->isConnected() &&
			!pvsConnectedOnce_) {

		qDebug() << "PVs connected.";
		pvsConnectedOnce_ = true;

		AMControl* oldUndulatorHarmonicControl = oldEnergyControls_->undulatorHarmonic();
		SGMUndulatorSupport::UndulatorHarmonic correspondingUndulatorHarmonic =
		        SGMUndulatorSupport::UnknownUndulatorHarmonic;

		if(oldUndulatorHarmonicControl->withinTolerance(1)) {

			correspondingUndulatorHarmonic = SGMUndulatorSupport::FirstHarmonic;
		} else {

			correspondingUndulatorHarmonic = SGMUndulatorSupport::ThirdHarmonic;
		}

		energyControlCoordinator_ = new SGMEnergyCoordinatorControl(correspondingUndulatorHarmonic, this);
		connect(energyControlCoordinator_, SIGNAL(connected(bool)), this, SLOT(onEnergyControlConnected(bool)));

		qDebug() << "Waiting for energy control to be connected...";
		if(energyControlCoordinator_->isConnected()) {

			onEnergyControlConnected(true);
		}

	} else if(pvsConnectedOnce_ && (!oldEnergyControls_->isConnected() || !newEnergyControls_->isConnected())) {

		qDebug() << "FATAL ERROR: Lost connection to AM Soft IOC";
		QApplication::exit(EXIT_FAILURE);
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
		if(!newEnergyControls_->energySetpoint()->withinTolerance(energyControlCoordinator_->value())) {

			qDebug() << QString("Initializing new energy setpoint PV to %1").arg(energyControlCoordinator_->value());
			newEnergyControls_->energySetpoint()->move(energyControlCoordinator_->value());
		} else {
			energySetpointInitialized_ = true;
		}


		// Grating angle setpoint
		if(newEnergyControls_->gratingAngleSetpoint()->withinTolerance(energyControlCoordinator_->gratingAngleControl()->value())) {

			qDebug() << QString("Initializing new grating angle setpoint PV to %1").arg(energyControlCoordinator_->gratingAngleControl()->value());
			newEnergyControls_->gratingAngleSetpoint()->move(energyControlCoordinator_->gratingAngleControl()->value());
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

		if(!newEnergyControls_->gratingTranslationSetpoint()->withinTolerance(initialGratingTranslationValue)
		        && initialGratingTranslation != SGMGratingSupport::UnknownGrating) {

			qDebug() << QString("Initializing new grating translation setpoint PV to %1").arg(initialGratingTranslationValue);
			newEnergyControls_->gratingTranslationSetpoint()->move(initialGratingTranslationValue);
		} else {
			gratingTranslationSetpointInitialized_ = true;
		}

		// Grating translation optimization setpoint
		SGMGratingSupport::GratingTranslationOptimizationMode initialOptimizationMode =
		        energyControlCoordinator_->gratingTranslationOptimizationMode();

		double initialOptimizationModeValue;
		switch(initialOptimizationMode) {
		case SGMGratingSupport::ManualMode:
			initialOptimizationModeValue = 0;
			break;
		case SGMGratingSupport::OptimizeFlux:
			initialOptimizationModeValue = 1;
			break;
		default:
			initialOptimizationModeValue = 2;
		}

		if(!newEnergyControls_->gratingTranslationOptimizationModeSetpoint()->withinTolerance(initialOptimizationModeValue)) {

			qDebug() << QString("Initializing new grating optimization mode setpoint PV to %1").arg(initialOptimizationModeValue);
			newEnergyControls_->gratingTranslationOptimizationModeSetpoint()->move(initialOptimizationModeValue);
		} else {
			gratingTranslationOptimizationModeSetpointInitialized_ = true;
		}


		// Undulator position
		if(!newEnergyControls_->undulatorPositionSetpoint()->withinTolerance(energyControlCoordinator_->undulatorPositionControl()->value())) {

			qDebug() << QString("Initializing new undulator position setpoint PV to %1").arg(energyControlCoordinator_->undulatorPositionControl()->value());
			newEnergyControls_->undulatorPositionSetpoint()->move(energyControlCoordinator_->undulatorPositionControl()->value());
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

		if(!newEnergyControls_->undulatorHarmonicSetpoint()->withinTolerance(initialUndulatorHarmonicValue)) {

			qDebug() << QString("Initializing new undulator harmonic setpoint PV to %1").arg(initialUndulatorHarmonicValue);
			newEnergyControls_->undulatorPositionSetpoint()->move(initialUndulatorHarmonicValue);
		} else {
			undulatorHarmonicSetpointInitialized_ = true;
		}

		// Exit slit position
		if(!newEnergyControls_->exitSlitPositionSetpoint()->withinTolerance(energyControlCoordinator_->exitSlitPositionControl()->setpoint())) {

			qDebug() << QString("Initializing new exit slit position setpoint PV to %1").arg(energyControlCoordinator_->exitSlitPositionControl()->setpoint());
			newEnergyControls_->exitSlitPositionSetpoint()->move(energyControlCoordinator_->exitSlitPositionControl()->setpoint());
		} else {
			exitSlitPositionSetpointInitialized_ = true;
		}

		// Connect signals from energy control coordinator
		connect(energyControlCoordinator_, SIGNAL(valueChanged(double)),
		        this, SLOT(onEnergyControlValueChanged(double)));

		connect(energyControlCoordinator_, SIGNAL(movingChanged(bool)),
		        this, SLOT(onEnergyControlIsMovingChanged(bool)));

		connect(energyControlCoordinator_, SIGNAL(gratingTranslationOptimizationModeChanged(SGMGratingSupport::GratingTranslationOptimizationMode)),
			this, SLOT(onEnergyControlGratingTranslationOptimizationModeChanged(SGMGratingSupport::GratingTranslationOptimizationMode)));

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
		connect(newEnergyControls_->energySetpoint(), SIGNAL(valueChanged(double)),
		        this, SLOT(onEnergySetpointPVChanged(double)));

		connect(newEnergyControls_->energyStop(), SIGNAL(valueChanged(double)),
		        this, SLOT(onEnergyStopPVChanged(double)));

		connect(newEnergyControls_->energyTrajectoryStart(), SIGNAL(valueChanged(double)),
		        this, SLOT(onEnergyTrajectoryStartPVChanged(double)));

		connect(newEnergyControls_->gratingAngleSetpoint(), SIGNAL(valueChanged(double)),
		        this, SLOT(onGratingAngleSetpointPVChanged(double)));

		connect(newEnergyControls_->gratingAngleStop(), SIGNAL(valueChanged(double)),
		        this, SLOT(onGratingAngleStopPVChanged(double)));

		connect(newEnergyControls_->gratingTranslationOptimizationModeSetpoint(), SIGNAL(valueChanged(double)),
		        this, SLOT(onGratingTranslationOptimizationModeSetpointPVChanged(double)));

		connect(newEnergyControls_->gratingTranslationSetpoint(), SIGNAL(valueChanged(double)),
		        this, SLOT(onGratingTranslationSetpointPVChanged(double)));

		connect(newEnergyControls_->gratingTranslationStop(), SIGNAL(valueChanged(double)),
		        this, SLOT(onGratingTranslationStopPVChanged(double)));

		connect(newEnergyControls_->undulatorPositionSetpoint(), SIGNAL(valueChanged(double)),
		        this, SLOT(onUndulatorSetpointPVChanged(double)));

		connect(newEnergyControls_->undulatorPositionStop(), SIGNAL(valueChanged(double)),
		        this, SLOT(onUndulatorStopPVChanged(double)));

		connect(newEnergyControls_->undulatorOffset(), SIGNAL(valueChanged(double)),
		        this, SLOT(onUndulatorOffsetPVChanged(double)));

		connect(newEnergyControls_->undulatorHarmonicSetpoint(), SIGNAL(valueChanged(double)),
		        this, SLOT(onUndulatorHarmonicSetpointPVChanged(double)));

		connect(newEnergyControls_->undulatorTracking(), SIGNAL(valueChanged(double)),
		        this, SLOT(onUndulatorTrackingPVChanged(double)));

		connect(newEnergyControls_->exitSlitPositionSetpoint(), SIGNAL(valueChanged(double)),
		        this, SLOT(onExitSlitPositionSetpointPVChanged(double)));

		connect(newEnergyControls_->exitSlitPositionStop(), SIGNAL(valueChanged(double)),
		        this, SLOT(onExitSlitPositionStopPVChanged(double)));

		connect(newEnergyControls_->exitSlitPositionTracking(), SIGNAL(valueChanged(double)),
		        this, SLOT(onExitSlitPositionTrackingPVChanged(double)));

		beamControlCoordinator_ = new SGMBeamCoordinatorControl(this);
		qDebug() << "Waiting for beam on controls to be connected...";

		connect(beamControlCoordinator_, SIGNAL(connected(bool)), this, SLOT(onBeamOnControlConnected(bool)));
		if(beamControlCoordinator_->isConnected()) {

			onBeamOnControlConnected(true);
		}

	} else if(energyControlConnectedOnce_ && !isConnected) {

		// We were connected to the energy control, but are no longer. Terminate

		qDebug() << "FATAL ERROR: Lost connection to old energy PVs";
		QApplication::exit(EXIT_FAILURE);
	}
}

void SGMEnergyCoordinator::onBeamOnControlConnected(bool isConnected)
{
	if(isConnected && !beamOnControlConnectedOnce_) {

		qDebug() << "Beam on controls connected.";
		if(beamControlCoordinator_->isMoving()) {

			if(!newBeamOnControls_->beamStatusControl()->withinTolerance(2)) {
				qDebug() << "Initializing beam status to 2";
				newBeamOnControls_->beamStatusControl()->move(2);
			}
		} else {

			if(!newBeamOnControls_->beamStatusControl()->withinTolerance(beamControlCoordinator_->value())) {
				qDebug() << "Initializing beam status to " << beamControlCoordinator_->value();
				newBeamOnControls_->beamStatusControl()->move(beamControlCoordinator_->value());
			}
		}

		connect(beamControlCoordinator_, SIGNAL(moveStarted()), this, SLOT(onBeamOnControlMoveStarted()));
		connect(beamControlCoordinator_, SIGNAL(moveFailed(int)), this, SLOT(onBeamOnControlMoveFailed()));
		connect(beamControlCoordinator_, SIGNAL(moveSucceeded()), this, SLOT(onBeamOnControlMoveSucceeded()));
		connect(beamControlCoordinator_, SIGNAL(valueChanged(double)), this, SLOT(onBeamOnControlValueChanged(double)));
		connect(newBeamOnControls_->beamOnOperationControl(), SIGNAL(valueChanged(double)),
		        this, SLOT(onBeamOnPVChanged(double)));
		connect(newBeamOnControls_->beamOffOperationControl(), SIGNAL(valueChanged(double)),
		        this, SLOT(onBeamOffPVChanged(double)));

	} else if(!isConnected) {

		qDebug() << "FATAL ERROR: Lost connection to beam on PVs";
		QApplication::exit(EXIT_FAILURE);
	}
}

void SGMEnergyCoordinator::onEnergyControlValueChanged(double energy)
{
	newEnergyControls_->energyFeedback()->move(energy);
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
	newEnergyControls_->energyStatus()->move(movingValue);
}

void SGMEnergyCoordinator::onEnergyControlGratingTranslationOptimizationModeChanged(SGMGratingSupport::GratingTranslationOptimizationMode gratingTranslationOptimizationMode)
{
	double gratingOptimizationModeValue = 0;
	switch(gratingTranslationOptimizationMode) {

	case SGMGratingSupport::ManualMode:
		gratingOptimizationModeValue = 0.0;
		break;
	case SGMGratingSupport::OptimizeFlux:
		gratingOptimizationModeValue = 1.0;
		break;
	case SGMGratingSupport::OptimizeResolution:
		gratingOptimizationModeValue = 2.0;
		break;
	}

	if(!newEnergyControls_->gratingTranslationOptimizationModeFeedback()->withinTolerance(gratingOptimizationModeValue)) {

		qDebug() << QString("Setting new grating translation optimization mode fbk PV to %1").arg(gratingOptimizationModeValue);
		newEnergyControls_->gratingTranslationOptimizationModeFeedback()->move(gratingOptimizationModeValue);
	}
}

void SGMEnergyCoordinator::onEnergyControlUndulatorOffsetChanged(double undulatorOffset)
{
	if(!newEnergyControls_->undulatorOffset()->withinTolerance(undulatorOffset)) {

		qDebug() << QString("Setting new undulator offset PV to %1").arg(undulatorOffset);
		newEnergyControls_->undulatorOffset()->move(undulatorOffset);
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

		if(!newEnergyControls_->undulatorHarmonicFeedback()->withinTolerance(undulatorHarmonicValue)) {

			qDebug() << QString("Setting new undulator harmonic fbk PV to").arg(undulatorHarmonicValue);
			newEnergyControls_->undulatorHarmonicFeedback()->move(undulatorHarmonicValue);
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

	if(!newEnergyControls_->undulatorTracking()->withinTolerance(isTrackingValue)) {

		qDebug() << QString("Setting new undulator tracking PV to %1").arg(isTracking);
		newEnergyControls_->undulatorTracking()->move(isTrackingValue);
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

	if(!newEnergyControls_->exitSlitPositionTracking()->withinTolerance(isTrackingValue)) {

		qDebug() << QString("Setting new exit slit tracking PV to %1").arg(isTracking);
		newEnergyControls_->exitSlitPositionTracking()->move(isTrackingValue);
	}
}

void SGMEnergyCoordinator::onOldGratingAnglePVChanged(double value)
{
	if(!newEnergyControls_->gratingAngleFeedback()->withinTolerance(value)) {

		newEnergyControls_->gratingAngleFeedback()->move(value);
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
	newEnergyControls_->gratingAngleStatus()->move(movingValue);
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

	if(!newEnergyControls_->gratingTranslationFeedback()->withinTolerance(newGratingTranslationPVValue)) {

		qDebug() << QString("Setting new grating translation fbk PV to %1").arg(newGratingTranslationPVValue);
		newEnergyControls_->gratingTranslationFeedback()->move(newGratingTranslationPVValue);
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
	newEnergyControls_->gratingTranslationStatus()->move(movingValue);
}

void SGMEnergyCoordinator::onOldUndulatorPositionPVChanged(double value)
{
	if(!newEnergyControls_->undulatorPositionFeedback()->withinTolerance(value)) {

		newEnergyControls_->undulatorPositionFeedback()->move(value);
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
	newEnergyControls_->undulatorPositionStatus()->move(movingValue);
}

void SGMEnergyCoordinator::onOldExitSlitPositionPVChanged(double value)
{
	if(!newEnergyControls_->exitSlitPositionFeedback()->withinTolerance(value)) {

		newEnergyControls_->exitSlitPositionFeedback()->move(value);
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
	newEnergyControls_->exitSlitPositionStatus()->move(movingValue);

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
	if(newEnergyControls_->energyStop()->withinTolerance(1.0)) {

		qDebug() <<"Forwarding stop energy command to energy control";
		energyControlCoordinator_->stop();
		newEnergyControls_->energyStop()->move(0);
	}

	if(!newEnergyControls_->energyStop()->withinTolerance(0.0)) {
		newEnergyControls_->energyStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onEnergyTrajectoryStartPVChanged(double /*value*/)
{
	if(newEnergyControls_->energyTrajectoryStart()->withinTolerance(1.0)) {

		qDebug() << "Forwarding start trajectory move instruction to energy control";
		double target = newEnergyControls_->energyTrajectoryTarget()->value();
		double time = newEnergyControls_->energyTrajectoryTime()->value();

		if(energyControlCoordinator_->move(target, time) == AMControl::NoFailure) {
			qDebug() << "Moving energy to" << target << "in" << time << "s";
		}
	}

	if(!newEnergyControls_->gratingAngleStop()->withinTolerance(0.0)) {
		newEnergyControls_->gratingAngleStop()->move(0.0);
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
	if(newEnergyControls_->gratingAngleStop()->withinTolerance(1.0)) {

		qDebug() << "Forwarding stop grating angle command to energy control";
		energyControlCoordinator_->gratingAngleControl()->stop();
		newEnergyControls_->gratingAngleStop()->move(0);
	}

	if(!newEnergyControls_->gratingAngleStop()->withinTolerance(0.0)) {
		newEnergyControls_->gratingAngleStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onGratingTranslationSetpointPVChanged(double value)
{
	qDebug() << "Grating translation setpoint value of" << value << "received";
	if(gratingTranslationSetpointInitialized_) {
		qDebug() << "\tIs Initialized";
		if(energyControlCoordinator_->gratingTranslationOptimizationMode() == SGMGratingSupport::ManualMode) {
			qDebug() << "\tIs in manual mode";
			SGMGratingSupport::GratingTranslation enumValue = SGMGratingSupport::UnknownGrating;

			if(newEnergyControls_->gratingTranslationSetpoint()->withinTolerance(0.0)) {

				enumValue = SGMGratingSupport::LowGrating;
			} else if (newEnergyControls_->gratingTranslationSetpoint()->withinTolerance(1.0)) {

				enumValue = SGMGratingSupport::MediumGrating;
			} else if (newEnergyControls_->gratingTranslationSetpoint()->withinTolerance(2.0)) {

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
			newEnergyControls_->gratingTranslationSetpoint()->move(newEnergyControls_->gratingTranslationFeedback()->value());
		}
	} else {
		gratingTranslationSetpointInitialized_ = true;
	}
}

void SGMEnergyCoordinator::onGratingTranslationStopPVChanged(double)
{
	if(newEnergyControls_->gratingTranslationStop()->withinTolerance(1.0)) {

		qDebug() << "Forwarding stop grating translation command to energy control";
		energyControlCoordinator_->gratingTranslationControl()->stop();
		newEnergyControls_->gratingTranslationStop()->move(0);
	}

	if(!newEnergyControls_->gratingTranslationStop()->withinTolerance(0.0)) {
		newEnergyControls_->gratingTranslationStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onGratingTranslationOptimizationModeSetpointPVChanged(double)
{
	if(gratingTranslationOptimizationModeSetpointInitialized_) {
		SGMGratingSupport::GratingTranslationOptimizationMode optimizationMode;

		if(newEnergyControls_->gratingTranslationOptimizationModeSetpoint()->withinTolerance(0)) {

			optimizationMode = SGMGratingSupport::ManualMode;
		} else if(newEnergyControls_->gratingTranslationOptimizationModeSetpoint()->withinTolerance(1)) {

			optimizationMode = SGMGratingSupport::OptimizeFlux;
		} else if(newEnergyControls_->gratingTranslationOptimizationModeSetpoint()->withinTolerance(2)) {

			optimizationMode = SGMGratingSupport::OptimizeResolution;
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
	if(newEnergyControls_->undulatorPositionStop()->withinTolerance(1.0)) {

		qDebug() << "Forwarding stop undulator position command to energy control";
		energyControlCoordinator_->undulatorPositionControl()->stop();
		newEnergyControls_->undulatorPositionStop()->move(0);
	}

	if(!newEnergyControls_->undulatorPositionStop()->withinTolerance(0.0)) {
		newEnergyControls_->undulatorPositionStop()->move(0.0);
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
		if(newEnergyControls_->undulatorHarmonicSetpoint()->withinTolerance(0)) {

			undulatorHarmonic = SGMUndulatorSupport::FirstHarmonic;
		} else if(newEnergyControls_->undulatorHarmonicSetpoint()->withinTolerance(1)) {

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

	if(newEnergyControls_->undulatorTracking()->withinTolerance(0)) {

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
	if(newEnergyControls_->exitSlitPositionStop()->withinTolerance(1.0)) {

		qDebug() << "Forwarding stop exit slit position command to energy control";
		energyControlCoordinator_->exitSlitPositionControl()->stop();
		newEnergyControls_->exitSlitPositionStop()->move(0);
	}

	if(!newEnergyControls_->exitSlitPositionStop()->withinTolerance(0.0)) {
		newEnergyControls_->exitSlitPositionStop()->move(0.0);
	}
}

void SGMEnergyCoordinator::onExitSlitPositionTrackingPVChanged(double)
{
	bool isTracking;

	if(newEnergyControls_->exitSlitPositionTracking()->withinTolerance(0)) {

		isTracking = false;
	} else {

		isTracking = true;
	}

	if(energyControlCoordinator_->isExitSlitPositionTracking() != isTracking) {

		qDebug() << QString("Forwarding exit slit position tracking setpoint value of %1 to energy control").arg(isTracking);
		energyControlCoordinator_->setExitSlitPositionTracking(isTracking);
	}
}

void SGMEnergyCoordinator::onBeamOnControlMoveStarted()
{
	qDebug() << "Beam on action started:";
	if(!newBeamOnControls_->beamStatusControl()->withinTolerance(2)) {
		qDebug() << "\tSetting new beam status PV to 2";
		newBeamOnControls_->beamStatusControl()->move(2);
	}
}

void SGMEnergyCoordinator::onBeamOnControlMoveFailed()
{
	qDebug() << "Beam on action failed:";
	if(!newBeamOnControls_->beamStatusControl()->withinTolerance(beamControlCoordinator_->value())) {

		qDebug() << "\tSetting new beam status PV to " << beamControlCoordinator_->value();
		newBeamOnControls_->beamStatusControl()->move(beamControlCoordinator_->value());
	}
}

void SGMEnergyCoordinator::onBeamOnControlMoveSucceeded()
{
	qDebug() << "Beam on action succeeded:";
	if(!newBeamOnControls_->beamStatusControl()->withinTolerance(beamControlCoordinator_->value())) {

		qDebug() << "\tSetting new beam status PV to " << beamControlCoordinator_->value();
		newBeamOnControls_->beamStatusControl()->move(beamControlCoordinator_->value());
	}
}

void SGMEnergyCoordinator::onBeamOnControlValueChanged(double value)
{
	if(!newBeamOnControls_->beamStatusControl()->withinTolerance(value)) {
		qDebug() << "Setting new beam status PV to"<<value;
		newBeamOnControls_->beamStatusControl()->move(value);
	}
}

void SGMEnergyCoordinator::onBeamOnPVChanged(double)
{
	if(newBeamOnControls_->beamOnOperationControl()->withinTolerance(1.0)) {

		beamControlCoordinator_->move(1);
		newBeamOnControls_->beamOnOperationControl()->move(0);
	}

	if(!newBeamOnControls_->beamOnOperationControl()) {
		newBeamOnControls_->beamOnOperationControl()->move(0);
	}
}

void SGMEnergyCoordinator::onBeamOffPVChanged(double)
{
	if(newBeamOnControls_->beamOffOperationControl()->withinTolerance(1.0)) {

		qDebug() <<"Forwarding beam off operation to beam control";
		beamControlCoordinator_->move(0);
		newBeamOnControls_->beamOffOperationControl()->move(0);
	}

	if(!newBeamOnControls_->beamOffOperationControl()) {
		newBeamOnControls_->beamOffOperationControl()->move(0);
	}
}



