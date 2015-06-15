/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "BioXASSideBeamline.h"

#include <QDebug>

#include "beamline/AMPVControl.h"
#include "beamline/AMBasicControlDetectorEmulator.h"

BioXASSideBeamline::BioXASSideBeamline()
	: BioXASBeamline("BioXAS Beamline - Side Endstation")
{
	setupComponents();
	setupDiagnostics();
	setupSampleStage();
	setupDetectorStage();
	setupCryostatStage();
	setupControlSets();
	setupMotorGroup();
	setupControlsAsDetectors();
	setupExposedControls();
	setupExposedDetectors();
}

BioXASSideBeamline::~BioXASSideBeamline()
{

}

bool BioXASSideBeamline::isConnected() const
{
	bool isConnected = (
				// General components.
				BioXASBeamline::isConnected() &&

				// Mono.
				mono_->isConnected() &&

				// JJSlits
				jjSlits_->isConnected() &&

				// Scaler.
				scaler_->isConnected() &&

				// Filters
				carbonFilterFarm_->isConnected() &&
				xiaFilters_->isConnected() &&

				// Mirrors.
				m2Mirror_->isConnected() &&
				dbhrMirror_->isConnected() &&

				// Motors.
				cryostatX_->isConnected() &&
				cryostatY_->isConnected() &&
				cryostatZ_->isConnected() &&

				// Control sets.
				pressureSet_->isConnected() &&
				//valveSet_->isConnected() &&
				ionPumpSet_->isConnected() &&
				flowTransducerSet_->isConnected() &&
				flowSwitchSet_->isConnected() &&
				temperatureSet_->isConnected()
				);

	return isConnected;
}

//bool BioXASSideBeamline::openPhotonShutter1()
//{
//	if (ssh1_->isOpen() || (ssh1_->isClosed() && psh2_->isClosed())) {
//		psh1_->open();
//		return true;
//	}

//	return false;
//}

//bool BioXASSideBeamline::closePhotonShutter1()
//{
//	if (psh1_->isOpen()) {
//		psh1_->close();
//		return true;
//	}

//	return false;
//}

//bool BioXASSideBeamline::openPhotonShutter2()
//{
//	if (ssh1_->isOpen() || (ssh1_->isClosed() && psh1_->isClosed())) {
//		psh2_->open();
//		return true;
//	}

//	return false;
//}

//bool BioXASSideBeamline::closePhotonShutter2()
//{
//	if (psh2_->isOpen()) {
//		psh2_->close();
//		return true;
//	}

//	return false;
//}

//bool BioXASSideBeamline::openSafetyShutter1()
//{
//	if (ssh1_->isClosed()) {
//		ssh1_->open();
//		return true;
//	}

//	return false;
//}

//bool BioXASSideBeamline::closeSafetyShutter1()
//{
//	if ((psh1_->isOpen() && psh2_->isClosed()) || (psh1_->isClosed() && psh2_->isOpen())) {
//		ssh1_->close();
//		return true;
//	}

//	return false;
//}

//bool BioXASSideBeamline::openSafetyShutter2()
//{
//	if (sshSide1_->isClosed()) {
//		sshSide1_->open();
//		return true;
//	}

//	return false;
//}

//bool BioXASSideBeamline::closeSafetyShutter2()
//{
//	if (sshSide1_->isOpen()) {
//		sshSide1_->close();
//		return true;
//	}

//	return false;
//}

bool BioXASSideBeamline::allValvesOpen() const
{
	if (vvr1_->isOpen() && vvr2_->isOpen() && vvr3_->isOpen() && vvr4_->isOpen() && vvr5_->isOpen()
			&& vvrSide1_->isOpen() && vvrSide1_->isOpen() && vvrSide2_->isOpen() && vvrSide3_->isOpen() && vvrSide4_->isOpen() && vvrSide5_->isOpen() && vvrSide6_->isOpen())
		return true;

	return false;
}

bool BioXASSideBeamline::allValvesClosed() const
{
	if (vvr1_->isClosed() && vvr2_->isClosed() && vvr3_->isClosed() && vvr4_->isClosed() && vvr5_->isClosed()
			&& vvrSide1_->isClosed() && vvrSide1_->isClosed() && vvrSide2_->isClosed() && vvrSide3_->isClosed() && vvrSide4_->isClosed() && vvrSide5_->isClosed() && vvrSide6_->isClosed())
		return true;

	return false;
}

QList<AMControl *> BioXASSideBeamline::getMotorsByType(BioXASBeamlineDef::BioXASMotorType category) const
{
	QList<AMControl *> matchedMotors;

	switch (category) {
	case BioXASBeamlineDef::FilterMotor: // BioXAS Filter motors
		matchedMotors.append(carbonFilterFarm_->upstreamPositionControl());
		matchedMotors.append(carbonFilterFarm_->downstreamPositionControl());
		break;

	case BioXASBeamlineDef::M1Motor:	// BioXAS M1 motors
		matchedMotors.append(m1Mirror_->upstreamInboardMotorControl());
		matchedMotors.append(m1Mirror_->upstreamOutboardMotorControl());
		matchedMotors.append(m1Mirror_->downstreamMotorControl());
		matchedMotors.append(m1Mirror_->stripeSelectMotorControl());
		matchedMotors.append(m1Mirror_->yawMotorControl());
		matchedMotors.append(m1Mirror_->benderUpstreamMotorControl());
		matchedMotors.append(m1Mirror_->benderDownstreamMotorControl());
		matchedMotors.append(m1Mirror_->upperSlitBladeMotorControl());
		break;

	case BioXASBeamlineDef::MaskMotor:	// BioXAS Variable Mask motors
		matchedMotors.append(mono_->upperSlitBladeMotor());
		matchedMotors.append(mono_->lowerSlitBladeMotor());
		break;

	case BioXASBeamlineDef::MonoMotor:	// Mono motors
		matchedMotors.append(mono_->paddleMotor());
		matchedMotors.append(mono_->braggMotor());
		matchedMotors.append(mono_->verticalMotor());
		matchedMotors.append(mono_->lateralMotor());
		matchedMotors.append(mono_->crystalChangeMotor());
		matchedMotors.append(mono_->crystal1PitchMotor());
		matchedMotors.append(mono_->crystal1RollMotor());
		matchedMotors.append(mono_->crystal2PitchMotor());
		matchedMotors.append(mono_->crystal2RollMotor());
		break;

	case BioXASBeamlineDef::M2Motor:	// BioXAS M2 motors
		matchedMotors.append(m2Mirror_->upstreamInboardMotorControl());
		matchedMotors.append(m2Mirror_->upstreamOutboardMotorControl());
		matchedMotors.append(m2Mirror_->downstreamMotorControl());
		matchedMotors.append(m2Mirror_->stripeSelectMotorControl());
		matchedMotors.append(m2Mirror_->yawMotorControl());
		matchedMotors.append(m2Mirror_->benderUpstreamMotorControl());
		matchedMotors.append(m2Mirror_->benderDownstreamMotorControl());
		break;

	case BioXASBeamlineDef::PseudoM1Motor: // BioXAS Pseudo M1 motor
		matchedMotors.append(m1Mirror_->rollControl());
		matchedMotors.append(m1Mirror_->pitchControl());
		matchedMotors.append(m1Mirror_->heightControl());
		matchedMotors.append(m1Mirror_->yawControl());
		matchedMotors.append(m1Mirror_->lateralControl());
		break;

	case BioXASBeamlineDef::PseudoM2Motor: // BioXAS Pseudo M2 motor
		matchedMotors.append(m2Mirror_->rollControl());
		matchedMotors.append(m2Mirror_->pitchControl());
		matchedMotors.append(m2Mirror_->yawControl());
		matchedMotors.append(m2Mirror_->heightControl());
		matchedMotors.append(m2Mirror_->lateralControl());
		break;

//	case BioXASBeamlineDef::PseudoMonoMotor: // BioXAS Pseudo Mono motor
//		matchedMotors.append(monoPseudoEnergy_);
//		matchedMotors.append(monoBraggAngle_);
//		break;

	default:
		qDebug() << "ERROR: invalid BioXAS Motor category: " << category;
		break;
	}

	return matchedMotors;
}

void BioXASSideBeamline::onPressureSetConnected(bool connected)
{
	if (connected) {
		for(int i = 0; i < pressureSet_->count(); i++)
			connect( pressureSet_->at(i), SIGNAL(movingChanged(bool)), this, SLOT(onPressureError()) );

		onPressureError();
	}

	updateConnected();
}

void BioXASSideBeamline::onPressureError()
{
	if (!pressureSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVwStatusControl *current = 0;

	for (int i = 0; i < pressureSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVwStatusControl *>(pressureSet_->at(i));

		if (current && current->isMoving())
			error += tr("%1 (%2) %3 %4\n").arg(current->name()).arg(current->readPVName()).arg(current->value(), 0, 'e', 3).arg(current->units());
	}

	if (!error.isEmpty()){

		error.prepend("The following pressure readings are at a critical level:\n");
		AMErrorMon::error(this, BIOXASSIDEBEAMLINE_PRESSURE_TOO_HIGH, error);
	}

	emit pressureStatusChanged(error.isEmpty());
}

void BioXASSideBeamline::onValveSetConnected(bool connected)
{
	if (connected){
		connect( valveSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(onValveError()) );
		onValveError();
	}

	updateConnected();
}

void BioXASSideBeamline::onValveError()
{
	if (!valveSet_->isConnected())
		return;

	QString error("");
	CLSBiStateControl *current = 0;

	for (int i = 0; i < valveSet_->count(); i++){

		if (i == 0){

			AMReadOnlyPVwStatusControl *first = qobject_cast<AMReadOnlyPVwStatusControl *>(valveSet_->at(i));

			if (first && first->isMoving())
				error += QString("%1 (%2)\n").arg(first->name()).arg(first->movingPVName());
		}

		else {

			current = qobject_cast<CLSBiStateControl *>(valveSet_->at(i));

			if (current && current->isClosed())
				error += QString("%1 (%2)\n").arg(current->name()).arg(current->statePVName());
		}
	}

	if (!error.isEmpty()){

		error.prepend("The following valves are closed:\n");
		AMErrorMon::error(this, BIOXASSIDEBEAMLINE_VALVES_CLOSED, error);
	}

	emit valveStatusChanged(error.isEmpty());
}

void BioXASSideBeamline::onIonPumpSetConnected(bool connected)
{
	if (connected){
		connect( ionPumpSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(onIonPumpError()) );
		onIonPumpError();
	}

	updateConnected();
}

void BioXASSideBeamline::onIonPumpError()
{
	if (!ionPumpSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVControl *current = 0;

	for (int i = 0; i < ionPumpSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(ionPumpSet_->at(i));

		if (current && !current->value())
			error += tr("%1 (%2)\n").arg(current->name()).arg(current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following ion pumps are no longer operating correctly:\n");
		AMErrorMon::error(this, BIOXASSIDEBEAMLINE_ION_PUMP_TRIP, error);
	}

	emit ionPumpStatusChanged(error.isEmpty());
}

void BioXASSideBeamline::onFlowTransducerSetConnected(bool connected)
{
	if (connected) {
		for (int i = 0; i < flowTransducerSet_->count(); i++)
			connect( flowTransducerSet_->at(i), SIGNAL(movingChanged(bool)), this, SLOT(onFlowTransducerError()) );

		onFlowTransducerError();
	}

	updateConnected();
}

void BioXASSideBeamline::onFlowTransducerError()
{
	if (!flowTransducerSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVwStatusControl *current = 0;

	for (int i = 0; i < flowTransducerSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVwStatusControl *>(flowTransducerSet_->at(i));

		if (current && current->isMoving())
			error += tr("%1 (%2) %3 %4\n").arg(current->name()).arg(current->readPVName()).arg(current->value(), 0, 'e', 3).arg(current->units());
	}

	if (!error.isEmpty()){

		error.prepend("The following flow transducers are measuring too low:\n");
		AMErrorMon::error(this, BIOXASSIDEBEAMLINE_WATER_FLOW_TOO_LOW, error);
	}

	emit flowTransducerStatusChanged(error.isEmpty());
}

void BioXASSideBeamline::onFlowSwitchSetConnected(bool connected)
{
	if (connected){
		connect( flowSwitchSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(onFlowSwitchError()) );
		onFlowSwitchError();
	}

	updateConnected();
}

void BioXASSideBeamline::onFlowSwitchError()
{
	if (!flowSwitchSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVControl *current = 0;

	for (int i = 0; i < flowSwitchSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVControl *>(flowSwitchSet_->at(i));

		if (current && !current->value())
			error += tr("%1 (%2)\n").arg(current->name()).arg(current->readPVName());
	}

	if (!error.isEmpty()){

		error.prepend("The following flow switches have tripped:\n");
		AMErrorMon::error(this, BIOXASSIDEBEAMLINE_WATER_FLOW_SWITCH_TRIP, error);
	}

	emit flowSwitchStatusChanged(error.isEmpty());
}

void BioXASSideBeamline::onTemperatureSetConnected(bool connected)
{
	if (connected) {
		for (int i = 0; i < temperatureSet_->count(); i++)
			connect( temperatureSet_->at(i), SIGNAL(movingChanged(bool)), this, SLOT(onTemperatureError()) );

		onTemperatureError();
	}

	updateConnected();
}

void BioXASSideBeamline::onTemperatureError()
{
	if (!temperatureSet_->isConnected())
		return;

	QString error("");
	AMReadOnlyPVwStatusControl *current = 0;

	for (int i = 0; i < temperatureSet_->count(); i++){

		current = qobject_cast<AMReadOnlyPVwStatusControl *>(temperatureSet_->at(i));

		if (current && current->isMoving())
			error += tr("%1 (%2) %3 %4\n").arg(current->name()).arg(current->readPVName()).arg(current->value(), 0, 'e', 3).arg(current->units());
	}

	if (!error.isEmpty()){

		error.prepend("The following temperature sensors are reading too high:\n");
		AMErrorMon::error(this, BIOXASSIDEBEAMLINE_TEMPERATURE_TOO_HIGH, error);
	}

	emit temperatureStatusChanged(error.isEmpty());
}

void BioXASSideBeamline::setupDiagnostics()
{
	// Pressure controls

	ccg1_ = new AMReadOnlyPVwStatusControl("Pressure 1", "CCG1407-I00-01:vac:p", "CCG1407-I00-01:vac", this, new AMControlStatusCheckerDefault(0));
	ccg2_ = new AMReadOnlyPVwStatusControl("Pressure 2", "CCG1407-I00-02:vac:p", "CCG1407-I00-02:vac", this, new AMControlStatusCheckerDefault(0));
	ccg3_ = new AMReadOnlyPVwStatusControl("Pressure 3", "CCG1607-5-I00-02:vac:p", "CCG1607-5-I00-02:vac", this, new AMControlStatusCheckerDefault(0));

	ccgSide1_ = new AMReadOnlyPVwStatusControl("Side pressure 1", "CCG1607-5-I00-03:vac:p", "CCG1607-5-I00-03:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide2_ = new AMReadOnlyPVwStatusControl("Side pressure 2", "CCG1607-5-I00-04:vac:p", "CCG1607-5-I00-04:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide3_ = new AMReadOnlyPVwStatusControl("Side pressure 3", "CCG1607-5-I00-05:vac:p", "CCG1607-5-I00-05:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide4_ = new AMReadOnlyPVwStatusControl("Side pressure 4", "CCG1607-5-I22-01:vac:p", "CCG1607-5-I22-01:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide5_ = new AMReadOnlyPVwStatusControl("Side pressure 5", "CCG1607-5-I22-02:vac:p", "CCG1607-5-I22-02:vac", this, new AMControlStatusCheckerDefault(0));
	ccgSide6_ = new AMReadOnlyPVwStatusControl("Side pressure 6", "CCG1607-5-I22-03:vac:p", "CCG1607-5-I22-03:vac", this, new AMControlStatusCheckerDefault(0));

	// Vaccum valve controls

	vvr1_ = new CLSBiStateControl("Valve control 1", "Valve control 1", "VVR1407-I00-01:state", "VVR1407-I00-01:opr:open", "VVR1407-I00-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr2_ = new CLSBiStateControl("Valve control 2", "Valve control 2", "VVR1607-5-I00-01:state", "VVR1607-5-I00-01:opr:open", "VVR1607-5-I00-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr3_ = new CLSBiStateControl("Valve control 3", "Valve control 3", "VVR1607-5-I22-01:state", "VVR1607-5-I22-01:opr:open", "VVR1607-5-I22-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr4_ = new CLSBiStateControl("Valve control 4", "Valve control 4", "VVR1607-5-I21-01:state", "VVR1607-5-I21-01:opr:open", "VVR1607-5-I21-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr5_ = new CLSBiStateControl("Valve control 5", "Valve control 5", "VVR1607-5-I10-01:state", "VVR1607-5-I10-01:opr:open", "VVR1607-5-I10-01:opr:close", new AMControlStatusCheckerDefault(4), this);

	vvrSide1_ = new CLSBiStateControl("Side valve control 1", "Side valve control 1", "VVR1607-5-I22-02:state", "VVR1607-5-I22-02:opr:open", "VVR1607-5-I22-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide2_ = new CLSBiStateControl("Side valve control 2", "Side valve control 2", "VVR1607-5-I22-03:state", "VVR1607-5-I22-03:opr:open", "VVR1607-5-I22-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide3_ = new CLSBiStateControl("Side valve control 3", "Side valve control 3", "VVR1607-5-I22-04:state", "VVR1607-5-I22-04:opr:open", "VVR1607-5-I22-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide4_ = new CLSBiStateControl("Side valve control 4", "Side valve control 4", "VVR1607-5-I22-05:state", "VVR1607-5-I22-05:opr:open", "VVR1607-5-I22-05:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide5_ = new CLSBiStateControl("Side valve control 5", "Side valve control 5", "VVR1607-5-I22-06:state", "VVR1607-5-I22-06:opr:open", "VVR1607-5-I22-06:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvrSide6_ = new CLSBiStateControl("Side valve control 6", "Side valve control 6", "VVR1607-5-I22-07:state", "VVR1607-5-I22-07:opr:open", "VVR1607-5-I22-07:opr:close", new AMControlStatusCheckerDefault(4), this);

	// Ion pump controls

	iop1_ = new AMReadOnlyPVControl("Ion pump 1", "IOP1407-I00-01", this);
	iop2_ = new AMReadOnlyPVControl("Ion pump 2", "IOP1407-I00-02", this);
	iop3_ = new AMReadOnlyPVControl("Ion pump 3", "IOP1407-I00-03", this);
	iop4_ = new AMReadOnlyPVControl("Ion pump 4", "IOP1607-5-I00-01", this);
	iop5_ = new AMReadOnlyPVControl("Ion pump 5", "IOP1607-5-I00-02", this);

	iopSide1_ = new AMReadOnlyPVControl("Side ion pump 1", "IOP1607-5-I00-03", this);
	iopSide2_ = new AMReadOnlyPVControl("Side ion pump 2", "IOP1607-5-I00-04", this);
	iopSide3_ = new AMReadOnlyPVControl("Side ion pump 3", "IOP1607-5-I00-05", this);
	iopSide4_ = new AMReadOnlyPVControl("Side ion pump 4", "IOP1607-5-I22-01", this);
	iopSide5_ = new AMReadOnlyPVControl("Side ion pump 5", "IOP1607-5-I22-02", this);
	iopSide6_ = new AMReadOnlyPVControl("Side ion pump 6", "IOP1607-5-I22-03", this);
	iopSide7_ = new AMReadOnlyPVControl("Side ion pump 7", "IOP1607-5-I22-04", this);

	// Flow transducer controls

	flt1_ = new AMReadOnlyPVwStatusControl("Flow transducer 1", "FLT1407-I00-01", "FLT1407-I00-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt2_ = new AMReadOnlyPVwStatusControl("Flow transducer 2", "FLT1407-I00-02", "FLT1407-I00-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt3_ = new AMReadOnlyPVwStatusControl("Flow transducer 3", "FLT1407-I00-03", "FLT1407-I00-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt4_ = new AMReadOnlyPVwStatusControl("Flow transducer 4", "FLT1607-5-I00-01", "FLT1607-5-I00-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt5_ = new AMReadOnlyPVwStatusControl("Flow transducer 5", "FLT1607-5-I10-01", "FLT1607-5-I10-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt6_ = new AMReadOnlyPVwStatusControl("Flow transducer 6", "FLT1607-5-I10-02", "FLT1607-5-I10-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt7_ = new AMReadOnlyPVwStatusControl("Flow transducer 7", "FLT1607-5-I10-03", "FLT1607-5-I10-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt8_ = new AMReadOnlyPVwStatusControl("Flow transducer 8", "FLT1607-5-I10-04", "FLT1607-5-I10-04:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt9_ = new AMReadOnlyPVwStatusControl("Flow transducer 9", "FLT1607-5-I10-05", "FLT1607-5-I10-05:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt10_ = new AMReadOnlyPVwStatusControl("Flow transducer 10", "FLT1607-5-I10-06", "FLT1607-5-I10-06:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt11_ = new AMReadOnlyPVwStatusControl("Flow transducer 11", "FLT1607-5-I10-07", "FLT1607-5-I10-07:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt12_ = new AMReadOnlyPVwStatusControl("Flow transducer 12", "FLT1607-5-I20-01", "FLT1607-5-I20-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt13_ = new AMReadOnlyPVwStatusControl("Flow transducer 13", "FLT1607-5-I21-01", "FLT1607-5-I21-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt14_ = new AMReadOnlyPVwStatusControl("Flow transducer 14", "FLT1607-5-I21-02", "FLT1607-5-I21-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt15_ = new AMReadOnlyPVwStatusControl("Flow transducer 15", "FLT1607-5-I21-03", "FLT1607-5-I21-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt16_ = new AMReadOnlyPVwStatusControl("Flow transducer 16", "FLT1607-5-I21-04", "FLT1607-5-I21-04:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt17_ = new AMReadOnlyPVwStatusControl("Flow transducer 17", "FLT1607-5-I22-01", "FLT1607-5-I22-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt18_ = new AMReadOnlyPVwStatusControl("Flow transducer 18", "FLT1607-5-I22-02", "FLT1607-5-I22-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt19_ = new AMReadOnlyPVwStatusControl("Flow transducer 19", "FLT1607-5-I22-03", "FLT1607-5-I22-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt20_ = new AMReadOnlyPVwStatusControl("Flow transducer 20", "FLT1607-5-I22-04", "FLT1607-5-I22-04:lowflow", this, new AMControlStatusCheckerDefault(0));

	// Flow switch controls

	swf1_ = new AMReadOnlyPVControl("Flow switch 1", "SWF1407-I00-01", this);
	swf2_ = new AMReadOnlyPVControl("Flow switch 2", "SWF1407-I00-02", this);
	swf3_ = new AMReadOnlyPVControl("Flow switch 3", "SWF1407-I00-03", this);
	swf4_ = new AMReadOnlyPVControl("Flow switch 4", "SWF1607-5-I00-01", this);

	// Temperature controls

	tm1_ = new AMReadOnlyPVwStatusControl("Temperature 1", "TM1407-I00-01", "TM1407-I00-01:trip", this, new AMControlStatusCheckerDefault(0));
	tm2_ = new AMReadOnlyPVwStatusControl("Temperature 2", "TM1407-I00-02", "TM1407-I00-02:trip", this, new AMControlStatusCheckerDefault(0));
	tm3_ = new AMReadOnlyPVwStatusControl("Temperature 3", "TM1407-I00-03", "TM1407-I00-03:trip", this, new AMControlStatusCheckerDefault(0));
	tm4_ = new AMReadOnlyPVwStatusControl("Temperature 4", "TM1407-I00-04", "TM1407-I00-04:trip", this, new AMControlStatusCheckerDefault(0));
	tm5_ = new AMReadOnlyPVwStatusControl("Temperature 5", "TM1407-I00-05", "TM1407-I00-05:trip", this, new AMControlStatusCheckerDefault(0));

}

void BioXASSideBeamline::setupSampleStage()
{

}

void BioXASSideBeamline::setupDetectorStage()
{
	// Side Endstation table
	endstationTable_ = new BioXASEndstationTable("SideBL endstation table", "BL1607-6-I22", false, this);

	detectorStageLateral_ = new CLSMAXvMotor("SMTR1607-6-I22-16 Side Detector Lateral", "SMTR1607-6-I22-16", "SMTR1607-6-I22-16 Side Detector Lateral", true, 0.05, 2.0, this, ":mm");
}

void BioXASSideBeamline::setupMotorGroup()
{
	motorGroup_ = new AMMotorGroup(this);

	AMMotorGroupObject *cryostatStageGroupObject = new AMMotorGroupObject("Cryostat Stage - X, Y, Z",
																		  QStringList() << "X" << "Z" << "Y",
																		  QStringList() << "mm" << "mm" << "mm",
																		  QList<AMControl*>() << cryostatX_ << cryostatZ_ << cryostatY_,
																		  QList<AMMotorGroupObject::Orientation>() << AMMotorGroupObject::Horizontal << AMMotorGroupObject::Vertical << AMMotorGroupObject::Normal,
																		  QList<AMMotorGroupObject::MotionType>() << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational << AMMotorGroupObject::Translational,
																		  this);
	motorGroup_->addMotorGroupObject(cryostatStageGroupObject->name(), cryostatStageGroupObject);
}

void BioXASSideBeamline::setupDetectors()
{
	i0Detector_ = new CLSBasicScalerChannelDetector("I0Detector", "I0 Detector", scaler_, 16, this);
	i1Detector_ = new CLSBasicScalerChannelDetector("I1Detector", "I1 Detector", scaler_, 17, this);
	i2Detector_ = new CLSBasicScalerChannelDetector("I2Detector", "I2 Detector", scaler_, 18, this);
//	ge32ElementDetector_ = new BioXAS32ElementGeDetector("Ge32Element", "Ge 32 Element", this);
	fourElementVortexDetector_ = new BioXASFourElementVortexDetector("FourElementVortex", "Four Element Vortex Detector", this);
}

void BioXASSideBeamline::setupCryostatStage()
{
	cryostatX_ = new CLSMAXvMotor("SideCryostatX", "SMTR1607-6-I22-14", "SideCryostatX", true, 0.01, 2.0, this, ":mm");
	cryostatY_ = new CLSMAXvMotor("SideCryostatY", "SMTR1607-6-I22-13", "SideCryostatY", true, 0.01, 2.0, this, ":mm");
	cryostatZ_ = new CLSMAXvMotor("SideCryostatZ", "SMTR1607-6-I22-15", "SideCryostatZ", true, 0.01, 2.0, this, ":mm");
}

void BioXASSideBeamline::setupControlSets()
{
	// Pressure control set.

	pressureSet_ = new AMControlSet(this);
	pressureSet_->addControl(ccg1_);
	pressureSet_->addControl(ccg2_);
	pressureSet_->addControl(ccg3_);
	pressureSet_->addControl(ccgSide1_);
	pressureSet_->addControl(ccgSide2_);
	pressureSet_->addControl(ccgSide3_);
	pressureSet_->addControl(ccgSide4_);
	pressureSet_->addControl(ccgSide5_);
	pressureSet_->addControl(ccgSide6_);

	connect( pressureSet_, SIGNAL(connected(bool)), this, SLOT(onPressureSetConnected(bool)) );

	// Vacuum valve control set.

	valveSet_ = new AMControlSet(this);
	valveSet_->addControl(vvr1_);
	valveSet_->addControl(vvr2_);
	valveSet_->addControl(vvr3_);
	valveSet_->addControl(vvr4_);
	valveSet_->addControl(vvr5_);
	valveSet_->addControl(vvrSide1_);
	valveSet_->addControl(vvrSide2_);
	valveSet_->addControl(vvrSide3_);
	valveSet_->addControl(vvrSide4_);
	valveSet_->addControl(vvrSide5_);
	valveSet_->addControl(vvrSide6_);

	connect( valveSet_, SIGNAL(connected(bool)), this, SLOT(onValveSetConnected(bool)) );

	// Ion pump control set.

	ionPumpSet_ = new AMControlSet(this);
	ionPumpSet_->addControl(iop1_);
	ionPumpSet_->addControl(iop2_);
	ionPumpSet_->addControl(iop3_);
	ionPumpSet_->addControl(iop4_);
	ionPumpSet_->addControl(iop5_);
	ionPumpSet_->addControl(iopSide1_);
	ionPumpSet_->addControl(iopSide2_);
	ionPumpSet_->addControl(iopSide3_);
	ionPumpSet_->addControl(iopSide4_);
	ionPumpSet_->addControl(iopSide5_);
	ionPumpSet_->addControl(iopSide6_);
	ionPumpSet_->addControl(iopSide7_);

	connect( ionPumpSet_, SIGNAL(connected(bool)), this, SLOT(onIonPumpSetConnected(bool)) );

	// Flow transducer control set.

	flowTransducerSet_ = new AMControlSet(this);
	flowTransducerSet_->addControl(flt1_);
	flowTransducerSet_->addControl(flt2_);
	flowTransducerSet_->addControl(flt3_);
	flowTransducerSet_->addControl(flt4_);
	flowTransducerSet_->addControl(flt5_);
	flowTransducerSet_->addControl(flt6_);
	flowTransducerSet_->addControl(flt7_);
	flowTransducerSet_->addControl(flt8_);
	flowTransducerSet_->addControl(flt9_);
	flowTransducerSet_->addControl(flt10_);
	flowTransducerSet_->addControl(flt11_);
	flowTransducerSet_->addControl(flt12_);
	flowTransducerSet_->addControl(flt13_);
	flowTransducerSet_->addControl(flt14_);
	flowTransducerSet_->addControl(flt15_);
	flowTransducerSet_->addControl(flt16_);
	flowTransducerSet_->addControl(flt17_);
	flowTransducerSet_->addControl(flt18_);
	flowTransducerSet_->addControl(flt19_);
	flowTransducerSet_->addControl(flt20_);

	connect( flowTransducerSet_, SIGNAL(connected(bool)), this, SLOT(onFlowTransducerSetConnected(bool)) );

	// Flow switch control set.

	flowSwitchSet_ = new AMControlSet(this);
	flowSwitchSet_->addControl(swf1_);
	flowSwitchSet_->addControl(swf2_);
	flowSwitchSet_->addControl(swf3_);
	flowSwitchSet_->addControl(swf4_);

	connect( flowSwitchSet_, SIGNAL(connected(bool)), this, SLOT(onFlowSwitchSetConnected(bool)) );

	// Temperature monitor control set.

	temperatureSet_ = new AMControlSet(this);
	temperatureSet_->addControl(tm1_);
	temperatureSet_->addControl(tm2_);
	temperatureSet_->addControl(tm3_);
	temperatureSet_->addControl(tm4_);
	temperatureSet_->addControl(tm5_);

	connect( temperatureSet_, SIGNAL(connected(bool)), this, SLOT(onTemperatureSetConnected(bool)) );
}

void BioXASSideBeamline::setupComponents()
{
	// The Side endstation safety shutter.
	safetyShutterDownstream_ = new  CLSBiStateControl("SideShutter", "SideShutter", "SSH1607-5-I22-01:state", "SSH1607-5-I22-01:opr:open", "SSH1607-5-I22-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( safetyShutterDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// The scaler.
	scaler_ = new CLSSIS3820Scaler("BL1607-5-I22:mcs", this);
	connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	scalerDwellTime_ = new AMReadOnlyPVControl("ScalerDwellTime", "BL1607-5-I22:mcs:delay", this, "Scaler dwell time");

	standardsWheel_ = new CLSStandardsWheel("StandardsWheel", "SMTR1607-6-I22-19", this);
	standardsWheel_->setName(0, "Cr");
	standardsWheel_->setName(1, "Mn");
	standardsWheel_->setName(2, "Fe");
	standardsWheel_->setName(3, "Co");
	standardsWheel_->setName(4, "Ni");
	standardsWheel_->setName(5, "Cu");
	standardsWheel_->setName(6, "Zn");
	standardsWheel_->setName(7, "As");
	standardsWheel_->setName(8, "Se");
	standardsWheel_->setName(9, "Hg");
	standardsWheel_->setName(10, "Mo");
	standardsWheel_->setName(11, "None");

	// Create the detectors (the scaler channel detectors need to be instantiated before they can be used below).
	setupDetectors();

	// The I0 channel amplifier.
	i0Keithley_ = new CLSKeithley428("I0 Channel", "AMP1607-601", this);
	scaler_->channelAt(16)->setCustomChannelName("I0 Channel");
	scaler_->channelAt(16)->setCurrentAmplifier(i0Keithley_);
	scaler_->channelAt(16)->setDetector(i0Detector_);

	// The I1 channel amplifier.
	i1Keithley_ = new CLSKeithley428("I1 Channel", "AMP1607-602", this);
	scaler_->channelAt(17)->setCustomChannelName("I1 Channel");
	scaler_->channelAt(17)->setCurrentAmplifier(i1Keithley_);
	scaler_->channelAt(17)->setDetector(i1Detector_);

	// The I2 channel amplifier.
	i2Keithley_ = new CLSKeithley428("I2 Channel", "AMP1607-603", this);
	scaler_->channelAt(18)->setCustomChannelName("I2 Channel");
	scaler_->channelAt(18)->setCurrentAmplifier(i2Keithley_);
	scaler_->channelAt(18)->setDetector(i2Detector_);

	// The carbon filter farm.
	carbonFilterFarm_ = new BioXASSideCarbonFilterFarm(this);
	connect( carbonFilterFarm_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// The aluminum XIA filters.
	xiaFilters_ = new BioXASSideXIAFilters(this);
	connect( xiaFilters_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// The m1 mirror. Must be instantiated before the mono.
	m1Mirror_ = new BioXASSideM1Mirror(this);
	connect( m1Mirror_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// The mono.
	mono_ = new BioXASSideMonochromator(this);
	mono_->setM1MirrorPitchControl(m1Mirror_->pitchControl());
	connect( mono_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// The JJ slits.
	jjSlits_ = new CLSJJSlits("JJSlits", "SMTR1607-6-I22-10", "SMTR1607-6-I22-09", "SMTR1607-6-I22-11", "SMTR1607-6-I22-12", this);
	connect( jjSlits_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// The m2 mirror.
	m2Mirror_ = new BioXASSideM2Mirror(this);
	connect( m2Mirror_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );

	// The DBHR mirrors.
	dbhrMirror_ = new BioXASSideDBHRMirror(this);
	connect( dbhrMirror_, SIGNAL(connectedChanged(bool)), this, SLOT(updateConnected()) );
}

void BioXASSideBeamline::setupControlsAsDetectors()
{
	encoderEnergySetpointDetector_ = new AMBasicControlDetectorEmulator("EncoderEnergySetpoint", "EncoderEnergySetpoint", mono_->encoderEnergyControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	encoderEnergySetpointDetector_->setControlProperty(AMBasicControlDetectorEmulator::Control::Setpoint);
	encoderEnergySetpointDetector_->setHiddenFromUsers(false);
	encoderEnergySetpointDetector_->setIsVisible(true);

	encoderEnergyFeedbackDetector_ = new AMBasicControlDetectorEmulator("EncoderEnergyFeedback", "EncoderEnergyFeedback", mono_->encoderEnergyControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	encoderEnergyFeedbackDetector_->setHiddenFromUsers(false);
	encoderEnergyFeedbackDetector_->setIsVisible(true);

	stepEnergyFeedbackDetector_ = new AMBasicControlDetectorEmulator("StepEnergyFeedback", "StepEnergyFeedback", mono_->stepEnergyControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	stepEnergyFeedbackDetector_->setHiddenFromUsers(false);
	stepEnergyFeedbackDetector_->setIsVisible(true);

	dwellTimeDetector_ = new AMBasicControlDetectorEmulator("DwellTimeFeedback", "Dwell Time Feedback", scalerDwellTime_, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);

	braggDetector_ = new AMBasicControlDetectorEmulator("GoniometerMotorFeedback", "Goniometer Motor Feedback", mono_->braggMotor(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	braggDetector_->setHiddenFromUsers(false);
	braggDetector_->setIsVisible(true);

	braggEncoderFeedbackDetector_ = new AMBasicControlDetectorEmulator("GoniometerMotorEncoderFeedback", "Goniometer Motor Encoder Feedback", mono_->braggMotor()->encoderFeedbackControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	braggEncoderFeedbackDetector_->setHiddenFromUsers(false);
	braggEncoderFeedbackDetector_->setIsVisible(true);

	braggMoveRetriesDetector_ = new AMBasicControlDetectorEmulator("GoniometerMotorMoveRetries", "Number of mono move retries", mono_->braggMotor()->retries(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	braggMoveRetriesDetector_->setHiddenFromUsers(false);
	braggMoveRetriesDetector_->setIsVisible(true);

	braggStepSetpointDetector_ = new AMBasicControlDetectorEmulator("GoniometerMotorStepSetpoint", "Goniometer motor step setpoint", mono_->braggMotor()->stepSetpointControl(), 0, 0, 0, AMDetectorDefinitions::ImmediateRead, this);
	braggStepSetpointDetector_->setHiddenFromUsers(false);
	braggStepSetpointDetector_->setIsVisible(true);
}

void BioXASSideBeamline::setupExposedControls()
{
	// M1 mirror controls

	addExposedControl(m1Mirror_->upstreamInboardMotorControl());
	addExposedControl(m1Mirror_->upstreamOutboardMotorControl());
	addExposedControl(m1Mirror_->downstreamMotorControl());
	addExposedControl(m1Mirror_->stripeSelectMotorControl());
	addExposedControl(m1Mirror_->yawControl());
	addExposedControl(m1Mirror_->benderUpstreamMotorControl());
	addExposedControl(m1Mirror_->benderDownstreamMotorControl());
	addExposedControl(m1Mirror_->upperSlitBladeMotorControl());

	addExposedControl(m1Mirror_->rollControl());
	addExposedControl(m1Mirror_->pitchControl());
	addExposedControl(m1Mirror_->heightControl());
	addExposedControl(m1Mirror_->yawControl());
	addExposedControl(m1Mirror_->lateralControl());
	addExposedControl(m1Mirror_->bendControl());

	// M2 mirror controls.

	addExposedControl(m2Mirror_->upstreamInboardMotorControl());
	addExposedControl(m2Mirror_->upstreamOutboardMotorControl());
	addExposedControl(m2Mirror_->downstreamMotorControl());
	addExposedControl(m2Mirror_->stripeSelectMotorControl());
	addExposedControl(m2Mirror_->yawMotorControl());
	addExposedControl(m2Mirror_->benderUpstreamMotorControl());
	addExposedControl(m2Mirror_->benderDownstreamMotorControl());
	addExposedControl(m2Mirror_->screenMotorControl());

	addExposedControl(m2Mirror_->rollControl());
	addExposedControl(m2Mirror_->pitchControl());
	addExposedControl(m2Mirror_->heightControl());
	addExposedControl(m2Mirror_->yawControl());
	addExposedControl(m2Mirror_->lateralControl());
	addExposedControl(m2Mirror_->bendControl());

	// Mono controls.

	addExposedControl(mono_->encoderEnergyControl());
	addExposedControl(mono_->stepEnergyControl());
	addExposedControl(mono_->regionControl());
	addExposedControl(mono_->braggMotor());
	addExposedControl(mono_->braggMotor()->EGUVelocityControl());
	addExposedControl(mono_->braggMotor()->EGUBaseVelocityControl());
	addExposedControl(mono_->braggMotor()->EGUAccelerationControl());
	addExposedControl(mono_->braggMotor()->preDeadBandControl());
	addExposedControl(mono_->braggMotor()->postDeadBandControl());
	addExposedControl(mono_->braggMotor()->encoderFeedbackControl());
	addExposedControl(mono_->braggMotor()->encoderMovementTypeControl());
	addExposedControl(mono_->braggMotor()->encoderStepSoftRatioControl());
	addExposedControl(mono_->braggMotor()->encoderCalibrationSlopeControl());
	addExposedControl(mono_->braggMotor()->stepCalibrationSlopeControl());
	addExposedControl(mono_->braggMotor()->retries());
	addExposedControl(mono_->crystal1PitchMotor());
	addExposedControl(mono_->crystal1RollMotor());
	addExposedControl(mono_->crystal2PitchMotor());
	addExposedControl(mono_->crystal2RollMotor());

	// JJ slit controls.

	addExposedControl(jjSlits_->verticalCenterControl());
	addExposedControl(jjSlits_->verticalGapControl());
	addExposedControl(jjSlits_->horizontalCenterControl());
	addExposedControl(jjSlits_->horizontalGapControl());

	// Carbon filter farm control.

	addExposedControl(carbonFilterFarm_->filterControl());

	// DBHR controls.

	addExposedControl(dbhrMirror_->pitchControl());
	addExposedControl(dbhrMirror_->m1VerticalControl());
	addExposedControl(dbhrMirror_->m2VerticalControl());

	// Detector stage controls.

	addExposedControl(detectorStageLateral_);

	// Cryostat stage controls.

	addExposedControl(cryostatX_);
	addExposedControl(cryostatY_);
	addExposedControl(cryostatZ_);
}

void BioXASSideBeamline::setupExposedDetectors()
{
	addExposedDetector(dwellTimeDetector_);
	addExposedDetector(i0Detector_);
	addExposedDetector(i1Detector_);
	addExposedDetector(i2Detector_);
	addExposedDetector(encoderEnergySetpointDetector_);
	addExposedDetector(encoderEnergyFeedbackDetector_);
	addExposedDetector(stepEnergyFeedbackDetector_);
	addExposedDetector(braggDetector_);
	addExposedDetector(braggEncoderFeedbackDetector_);
	addExposedDetector(braggMoveRetriesDetector_);
	addExposedDetector(braggStepSetpointDetector_);
//	addExposedDetector(ge32ElementDetector_);
	addExposedDetector(fourElementVortexDetector_);
}
