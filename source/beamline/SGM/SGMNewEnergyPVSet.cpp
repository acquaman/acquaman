#include "SGMNewEnergyPVSet.h"

#include "beamline/AMControlSet.h"
#include "beamline/AMPVControl.h"

SGMNewEnergyPVSet::SGMNewEnergyPVSet(QObject *parent) :
    QObject(parent)
{
	controlSet_ = new AMControlSet(this);
	connect(controlSet_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));

	controlSet_->addControl(new AMSinglePVControl("EnergyFeedback",
	                                              "AM1611-4-I10:energy:eV:fbk",
	                                              this,
	                                              0.1));

	controlSet_->addControl(new AMSinglePVControl("EnergySetpoint",
	                                              "AM1611-4-I10:energy:eV",
	                                              this,
	                                              0.1));

	controlSet_->addControl(new AMSinglePVControl("EnergyStop",
	                                              "AM1611-4-I10:energy:stop",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("EnergyStatus",
	                                              "AM1611-4-I10:energy:status",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("EnergyTrajectoryTarget",
						      "AM1611-4-I10:energy:trajectory:target:eV",
	                                              this,
	                                              0.1));

	controlSet_->addControl(new AMSinglePVControl("EnergyTrajectoryTime",
	                                              "AM1611-4-I10:energy:trajectory:time:s",
	                                              this,
	                                              0.1));

	controlSet_->addControl(new AMSinglePVControl("EnergyTrajectoryStart",
	                                              "AM1611-4-I10:energy:trajectory:start",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("GratingAngleFeedback",
	                                              "AM1611-4-I10:energy:grating:angle:steps:fbk",
	                                              this,
	                                              0.01));

	controlSet_->addControl(new AMSinglePVControl("GratingAngleSetpoint",
	                                              "AM1611-4-I10:energy:grating:angle:steps",
	                                              this,
	                                              0.01));

	controlSet_->addControl(new AMSinglePVControl("GratingAngleStatus",
	                                              "AM1611-4-I10:energy:grating:angle:status",
	                                              this,
	                                              0.01));

	controlSet_->addControl(new AMSinglePVControl("GratingAngleStop",
	                                              "AM1611-4-I10:energy:grating:angle:stop",
	                                              this,
	                                              0.01));

	controlSet_->addControl(new AMSinglePVControl("GratingTranslationFeedback",
	                                              "AM1611-4-I10:energy:grating:translation:fbk",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("GratingTranslationSetpoint",
	                                              "AM1611-4-I10:energy:grating:translation",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("GratingTranslationStatus",
	                                              "AM1611-4-I10:energy:grating:translation:status",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("GratingTranslationStop",
	                                              "AM1611-4-I10:energy:grating:translation:stop",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("GratingTranslationOptimizationModeFeedback",
	                                              "AM1611-4-I10:energy:grating:translation:optimization:fbk",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("GratingTranslationOptimizationModeSetpoint",
	                                              "AM1611-4-I10:energy:grating:translation:optimization",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("UndulatorPositionFeedback",
	                                              "AM1611-4-I10:energy:undulator:mm:fbk",
	                                              this,
	                                              0.01));

	controlSet_->addControl(new AMSinglePVControl("UndulatorPositionSetpoint",
	                                              "AM1611-4-I10:energy:undulator:mm",
	                                              this,
	                                              0.01));

	controlSet_->addControl(new AMSinglePVControl("UndulatorPositionStatus",
	                                              "AM1611-4-I10:energy:undulator:status",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("UndulatorPositionStop",
	                                              "AM1611-4-I10:energy:undulator:stop",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("UndulatorOffset",
	                                              "AM1611-4-I10:energy:undulator:offset:mm",
	                                              this,
	                                              0.01));

	controlSet_->addControl(new AMSinglePVControl("UndulatorHarmonicFeedback",
	                                              "AM1611-4-I10:energy:undulator:harmonic:fbk",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("UndulatorHarmonicSetpoint",
	                                              "AM1611-4-I10:energy:undulator:harmonic",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("UndulatorTracking",
	                                              "AM1611-4-I10:energy:undulator:tracking",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("ExitSlitPositionFeedback",
	                                              "AM1611-4-I10:energy:exitslit:position:mm:fbk",
	                                              this,
	                                              0.01));

	controlSet_->addControl(new AMSinglePVControl("ExitSlitPositionSetpoint",
	                                              "AM1611-4-I10:energy:exitslit:position:mm",
	                                              this,
	                                              0.01));

	controlSet_->addControl(new AMSinglePVControl("ExitSlitPositionStatus",
	                                              "AM1611-4-I10:energy:exitslit:position:status",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("ExitSlitPositionStop",
	                                              "AM1611-4-I10:energy:exitslit:position:stop",
	                                              this,
	                                              0.5));

	controlSet_->addControl(new AMSinglePVControl("ExitSlitTracking",
	                                              "AM1611-4-I10:energy:exitslit:position:tracking",
	                                              this,
	                                              0.5));


}

bool SGMNewEnergyPVSet::isConnected() const
{
	return controlSet_->isConnected();
}

AMControl * SGMNewEnergyPVSet::energyFeedback() const
{
	return controlSet_->controlNamed("EnergyFeedback");
}

AMControl * SGMNewEnergyPVSet::energySetpoint() const
{
	return controlSet_->controlNamed("EnergySetpoint");
}

AMControl * SGMNewEnergyPVSet::energyStop() const
{
	return controlSet_->controlNamed("EnergyStop");
}

AMControl * SGMNewEnergyPVSet::energyStatus() const
{
	return controlSet_->controlNamed("EnergyStatus");
}

AMControl * SGMNewEnergyPVSet::energyTrajectoryTarget() const
{
	return controlSet_->controlNamed("EnergyTrajectoryTarget");
}

AMControl * SGMNewEnergyPVSet::energyTrajectoryTime() const
{
	return controlSet_->controlNamed("EnergyTrajectoryTime");
}

AMControl * SGMNewEnergyPVSet::energyTrajectoryStart() const
{
	return controlSet_->controlNamed("EnergyTrajectoryStart");
}

AMControl * SGMNewEnergyPVSet::gratingAngleFeedback() const
{
	return controlSet_->controlNamed("GratingAngleFeedback");
}

AMControl * SGMNewEnergyPVSet::gratingAngleSetpoint() const
{
	return controlSet_->controlNamed("GratingAngleSetpoint");
}

AMControl * SGMNewEnergyPVSet::gratingAngleStatus() const
{
	return controlSet_->controlNamed("GratingAngleStatus");
}

AMControl * SGMNewEnergyPVSet::gratingAngleStop() const
{
	return controlSet_->controlNamed("GratingAngleStop");
}

AMControl * SGMNewEnergyPVSet::gratingTranslationFeedback() const
{
	return controlSet_->controlNamed("GratingTranslationFeedback");
}

AMControl * SGMNewEnergyPVSet::gratingTranslationSetpoint() const
{
	return controlSet_->controlNamed("GratingTranslationSetpoint");
}

AMControl * SGMNewEnergyPVSet::gratingTranslationStatus() const
{
	return controlSet_->controlNamed("GratingTranslationStatus");
}

AMControl * SGMNewEnergyPVSet::gratingTranslationStop() const
{
	return controlSet_->controlNamed("GratingTranslationStop");
}

AMControl * SGMNewEnergyPVSet::gratingTranslationOptimizationModeFeedback() const
{
	return controlSet_->controlNamed("GratingTranslationOptimizationModeFeedback");
}

AMControl * SGMNewEnergyPVSet::gratingTranslationOptimizationModeSetpoint() const
{
	return controlSet_->controlNamed("GratingTranslationOptimizationModeSetpoint");
}

AMControl * SGMNewEnergyPVSet::undulatorPositionFeedback() const
{
	return controlSet_->controlNamed("UndulatorPositionFeedback");
}

AMControl * SGMNewEnergyPVSet::undulatorPositionSetpoint() const
{
	return controlSet_->controlNamed("UndulatorPositionSetpoint");
}

AMControl * SGMNewEnergyPVSet::undulatorPositionStatus() const
{
	return controlSet_->controlNamed("UndulatorPositionStatus");
}

AMControl * SGMNewEnergyPVSet::undulatorPositionStop() const
{
	return controlSet_->controlNamed("UndulatorPositionStop");
}

AMControl * SGMNewEnergyPVSet::undulatorOffset() const
{
	return controlSet_->controlNamed("UndulatorOffset");
}

AMControl * SGMNewEnergyPVSet::undulatorHarmonicFeedback() const
{
	return controlSet_->controlNamed("UndulatorHarmonicFeedback");
}

AMControl * SGMNewEnergyPVSet::undulatorHarmonicSetpoint() const
{
	return controlSet_->controlNamed("UndulatorHarmonicSetpoint");
}

AMControl * SGMNewEnergyPVSet::undulatorTracking() const
{
	return controlSet_->controlNamed("UndulatorTracking");
}

AMControl * SGMNewEnergyPVSet::exitSlitPositionFeedback() const
{
	return controlSet_->controlNamed("ExitSlitPositionFeedback");
}

AMControl * SGMNewEnergyPVSet::exitSlitPositionSetpoint() const
{
	return controlSet_->controlNamed("ExitSlitPositionSetpoint");
}

AMControl * SGMNewEnergyPVSet::exitSlitPositionStatus() const
{
	return controlSet_->controlNamed("ExitSlitPositionStatus");
}

AMControl * SGMNewEnergyPVSet::exitSlitPositionStop() const
{
	return controlSet_->controlNamed("ExitSlitPositionStop");
}

AMControl * SGMNewEnergyPVSet::exitSlitPositionTracking() const
{
	return controlSet_->controlNamed("ExitSlitTracking");
}




