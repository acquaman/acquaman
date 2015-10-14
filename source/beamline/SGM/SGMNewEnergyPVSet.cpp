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
												   0.01));

	controlSet_->addControl(new AMSinglePVControl("EnergySetpoint",
												   "AM1611-4-I10:energy:eV",
												   this,
												   0.1));

	controlSet_->addControl(new AMSinglePVControl("EnergyStop",
												   "AM1611-4-I10:energy:stop",
												   this,
												   0.01));

	controlSet_->addControl(new AMSinglePVControl("EnergyStatus",
												  "AM1611-4-I10:energy:status",
												  this,
												  0.5));

	controlSet_->addControl(new AMSinglePVControl("GratingTranslationOptimizationModeFeedback",
												   "AM1611-4-I10:energy:grating:translation:optimization:mode:fbk",
												   this,
												   0.5));

	controlSet_->addControl(new AMSinglePVControl("GratingTranslationOptimizationModeSetpoint",
												   "AM1611-4-I10:energy:grating:translation:optimization:mode",
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

	controlSet_->addControl(new AMSinglePVControl("GratingTranslationFeedback",
												  "AM1611-4-I10:energy:grating:translation:feedback",
												  this,
												  0.01));

	controlSet_->addControl(new AMSinglePVControl("GratingTranslationSetpoint",
												  "AM1611-4-I10:energy:grating:translation",
												  this,
												  0.01));

	controlSet_->addControl(new AMSinglePVControl("GratingTranslationStatus",
												  "AM1611-4-I10:energy:grating:translation:status",
												  this,
												  0.5));

	controlSet_->addControl(new AMSinglePVControl("GratingTranslationStop",
												  "AM1611-4-I10:energy:translation:stop",
												  this,
												  0.5));

	controlSet_->addControl(new AMSinglePVControl("UndulatorTracking",
												  "AM1611-4-I10:energy:undulator:tracking",
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

AMControl * SGMNewEnergyPVSet::gratingTranslationOptimizationModeFeedback() const
{
	return controlSet_->controlNamed("GratingTranslationOptimizationModeFeedback");
}

AMControl * SGMNewEnergyPVSet::gratingTranslationOptimizationModeSetpoint() const
{
	return controlSet_->controlNamed("GratingTranslationOptimizationModeSetpoint");
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

AMControl * SGMNewEnergyPVSet::undulatorTracking() const
{
	return controlSet_->controlNamed("UndulatorTracking");
}

AMControl * SGMNewEnergyPVSet::exitSlitTracking() const
{
	return controlSet_->controlNamed("ExitSlitTracking");
}



