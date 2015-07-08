#include "BioXASBeamline.h"

#include "actions3/AMActionSupport.h"

#include "beamline/CLS/CLSStorageRing.h"

#include "util/AMErrorMonitor.h"

BioXASBeamline::~BioXASBeamline()
{

}

bool BioXASBeamline::isConnected() const
{
	bool shuttersOK = (
				photonShutterFEUpstream_ && photonShutterFEUpstream_->isConnected() &&
				photonShutterFEDownstream_ && photonShutterFEDownstream_->isConnected() &&
				safetyShutterFE_ && safetyShutterFE_->isConnected()
				);

	bool diagnosticsOK = (
				pressureSet_ && pressureSet_->isConnected() &&
				valveSet_ && valveSet_->isConnected() &&
				ionPumpSet_ && ionPumpSet_->isConnected() &&
				flowTransducerSet_ && flowTransducerSet_->isConnected() &&
				flowSwitchSet_ && flowSwitchSet_->isConnected() &&
				temperatureSet_ && temperatureSet_->isConnected()
				);

	return (shuttersOK && diagnosticsOK);
}

bool BioXASBeamline::beamOff() const
{
	bool result = false;

	return result;
}

bool BioXASBeamline::beamOn() const
{
	bool result = false;

	return result;
}

bool BioXASBeamline::beamAvailable() const
{
	bool result = false;

	return result;
}

AMAction3* BioXASBeamline::createTurnOffBeamActions()
{
	AMAction3 *action = 0;

//	if (safetyShutterES_ && safetyShutterES_->isConnected()) {

//		// For bi-state controls, the move setpoint for 'close' is 0 and the desired state is 4.

//		AMListAction3 *beamOff = new AMListAction3(new AMListActionInfo3("Turn off beam", "Turn off beam"), AMListAction3::Sequential);
//		beamOff->addSubAction(AMActionSupport::buildControlMoveAction(safetyShutterES_, 0));
//		beamOff->addSubAction(AMActionSupport::buildControlWaitAction(safetyShutterES_, 4, 5));

//		action = beamOff;
//	}

	return action;
}

AMAction3* BioXASBeamline::createTurnOnBeamActions()
{
	AMAction3 *action = 0;

//	if (safetyShutterES_ && safetyShutterES_->isConnected()) {

//		// For bi-state controls, the move setpoint for 'open' is 1 and the desired state is 1.

//		AMListAction3 *beamOn = new AMListAction3(new AMListActionInfo3("Turn off beam", "Turn off beam"), AMListAction3::Sequential);
//		beamOn->addSubAction(AMActionSupport::buildControlMoveAction(safetyShutterES_, 1));
//		beamOn->addSubAction(AMActionSupport::buildControlWaitAction(safetyShutterES_, 1, 5));

//		action = beamOn;
//	}

	return action;
}

void BioXASBeamline::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connectedChanged(connected_);
	}
}

void BioXASBeamline::updateConnected()
{
	setConnected( isConnected() );
}

void BioXASBeamline::onPressureSetConnected()
{
	if (pressureSet_ && pressureSet_->isConnected()) {
		for(int i = 0; i < pressureSet_->count(); i++)
			connect( pressureSet_->at(i), SIGNAL(movingChanged(bool)), this, SLOT(onPressureError()) );
	}

	onPressureError();
	updateConnected();
}

void BioXASBeamline::onPressureError()
{
	if (pressureSet_ && pressureSet_->isConnected()) {

		QString error("");
		AMReadOnlyPVwStatusControl *current = 0;

		// Iterate through the pressure controls, identifying controls that are in an error state.

		for (int i = 0; i < pressureSet_->count(); i++) {
			current = qobject_cast<AMReadOnlyPVwStatusControl *>(pressureSet_->at(i));

			if (current && current->isMoving())
				error += tr("%1 (%2) %3 %4\n").arg(current->name()).arg(current->readPVName()).arg(current->value(), 0, 'e', 3).arg(current->units());
		}

		if (!error.isEmpty()){
			error.prepend("The following pressure readings are at a critical level:\n");
			AMErrorMon::error(this, BIOXASBEAMLINE_PRESSURE_TOO_HIGH, error);
		}

		emit pressureStatusChanged(error.isEmpty());
	}
}

void BioXASBeamline::onValveSetConnected()
{
	if (valveSet_ && valveSet_->isConnected()) {
		connect( valveSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(onValveError()) );
	}

	onValveError();
	updateConnected();
}

void BioXASBeamline::onValveError()
{
	if (valveSet_ && valveSet_->isConnected()) {

		QString error("");
		CLSBiStateControl *current = 0;

		for (int i = 0; i < valveSet_->count(); i++) {

			if (i == 0) {
				AMReadOnlyPVwStatusControl *first = qobject_cast<AMReadOnlyPVwStatusControl *>(valveSet_->at(i));

				if (first && first->isMoving())
					error += QString("%1 (%2)\n").arg(first->name()).arg(first->movingPVName());

			} else {
				current = qobject_cast<CLSBiStateControl *>(valveSet_->at(i));

				if (current && current->isClosed())
					error += QString("%1 (%2)\n").arg(current->name()).arg(current->statePVName());
			}
		}

		if (!error.isEmpty()){
			error.prepend("The following valves are closed:\n");
			AMErrorMon::error(this, BIOXASBEAMLINE_VALVES_CLOSED, error);
		}

		emit valveStatusChanged(error.isEmpty());
	}
}

void BioXASBeamline::onIonPumpSetConnected()
{
	if (ionPumpSet_ && ionPumpSet_->isConnected()) {
		connect( ionPumpSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(onIonPumpError()) );
	}

	onIonPumpError();
	updateConnected();
}

void BioXASBeamline::onIonPumpError()
{
	if (ionPumpSet_ && ionPumpSet_->isConnected()) {

		QString error("");
		AMReadOnlyPVControl *current = 0;

		for (int i = 0; i < ionPumpSet_->count(); i++) {
			current = qobject_cast<AMReadOnlyPVControl *>(ionPumpSet_->at(i));

			if (current && !current->value())
				error += tr("%1 (%2)\n").arg(current->name()).arg(current->readPVName());
		}

		if (!error.isEmpty()) {
			error.prepend("The following ion pumps are no longer operating correctly:\n");
			AMErrorMon::error(this, BIOXASBEAMLINE_ION_PUMP_TRIP, error);
		}

		emit ionPumpStatusChanged(error.isEmpty());
	}
}

void BioXASBeamline::onFlowTransducerSetConnected()
{
	if (flowTransducerSet_ && flowTransducerSet_->isConnected()) {
		for (int i = 0; i < flowTransducerSet_->count(); i++)
			connect( flowTransducerSet_->at(i), SIGNAL(movingChanged(bool)), this, SLOT(onFlowTransducerError()) );
	}

	onFlowTransducerError();
	updateConnected();
}

void BioXASBeamline::onFlowTransducerError()
{
	if (flowTransducerSet_ && flowTransducerSet_->isConnected()) {

		QString error("");
		AMReadOnlyPVwStatusControl *current = 0;

		for (int i = 0; i < flowTransducerSet_->count(); i++) {
			current = qobject_cast<AMReadOnlyPVwStatusControl *>(flowTransducerSet_->at(i));

			if (current && current->isMoving())
				error += tr("%1 (%2) %3 %4\n").arg(current->name()).arg(current->readPVName()).arg(current->value(), 0, 'e', 3).arg(current->units());
		}

		if (!error.isEmpty()) {
			error.prepend("The following flow transducers are measuring too low:\n");
			AMErrorMon::error(this, BIOXASBEAMLINE_WATER_FLOW_TOO_LOW, error);
		}

		emit flowTransducerStatusChanged(error.isEmpty());
	}
}

void BioXASBeamline::onFlowSwitchSetConnected()
{
	if (flowSwitchSet_ && flowSwitchSet_->isConnected()) {
		connect( flowSwitchSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(onFlowSwitchError()) );
	}

	onFlowSwitchError();
	updateConnected();
}

void BioXASBeamline::onFlowSwitchError()
{
	if (flowSwitchSet_ && flowSwitchSet_->isConnected()) {

		QString error("");
		AMReadOnlyPVControl *current = 0;

		for (int i = 0; i < flowSwitchSet_->count(); i++) {
			current = qobject_cast<AMReadOnlyPVControl *>(flowSwitchSet_->at(i));

			if (current && !current->value())
				error += tr("%1 (%2)\n").arg(current->name()).arg(current->readPVName());
		}

		if (!error.isEmpty()) {
			error.prepend("The following flow switches have tripped:\n");
			AMErrorMon::error(this, BIOXASBEAMLINE_WATER_FLOW_SWITCH_TRIP, error);
		}

		emit flowSwitchStatusChanged(error.isEmpty());
	}
}

void BioXASBeamline::onTemperatureSetConnected()
{
	if (temperatureSet_ && temperatureSet_->isConnected()) {
		for (int i = 0; i < temperatureSet_->count(); i++)
			connect( temperatureSet_->at(i), SIGNAL(movingChanged(bool)), this, SLOT(onTemperatureError()) );
	}

	onTemperatureError();
	updateConnected();
}

void BioXASBeamline::onTemperatureError()
{
	if (temperatureSet_ && temperatureSet_->isConnected()) {

		QString error("");
		AMReadOnlyPVwStatusControl *current = 0;

		for (int i = 0; i < temperatureSet_->count(); i++) {
			current = qobject_cast<AMReadOnlyPVwStatusControl *>(temperatureSet_->at(i));

			if (current && current->isMoving())
				error += tr("%1 (%2) %3 %4\n").arg(current->name()).arg(current->readPVName()).arg(current->value(), 0, 'e', 3).arg(current->units());
		}

		if (!error.isEmpty()) {
			error.prepend("The following temperature sensors are reading too high:\n");
			AMErrorMon::error(this, BIOXASBEAMLINE_TEMPERATURE_TOO_HIGH, error);
		}

		emit temperatureStatusChanged(error.isEmpty());
	}
}

void BioXASBeamline::setupStorageRing()
{
	CLSStorageRing::sr1();
}

void BioXASBeamline::setupComponents()
{
	// The front end upstream photon shutter.
	photonShutterFEUpstream_ = new CLSBiStateControl("PhotonShutterFE1", "BioXAS front end photon shutter 1", "IPSH1407-I00-01:state", "IPSH1407-I00-01:opr:open", "IPSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterFEUpstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// The front end downstream photon shutter.
	photonShutterFEDownstream_ = new CLSBiStateControl("PhotonShutterFE2", "BioXAS front end photon shutter 2", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( photonShutterFEDownstream_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// The front end safety shutter.
	safetyShutterFE_ = new CLSBiStateControl("SafetyShutterFE", "BioXAS front end safety shutter", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this);
	connect( safetyShutterFE_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
}

void BioXASBeamline::setupDiagnostics()
{
	// Pressure controls.

	ccg1_ = new AMReadOnlyPVwStatusControl("CCG1", "CCG1407-I00-01:vac:p", "CCG1407-I00-01:vac", this, new AMControlStatusCheckerDefault(0));
	ccg2_ = new AMReadOnlyPVwStatusControl("CCG2", "CCG1407-I00-02:vac:p", "CCG1407-I00-02:vac", this, new AMControlStatusCheckerDefault(0));
	ccg3_ = new AMReadOnlyPVwStatusControl("CCG3", "CCG1607-5-I00-02:vac:p", "CCG1607-5-I00-02:vac", this, new AMControlStatusCheckerDefault(0));

	pressureSet_ = new AMControlSet(this);
	pressureSet_->addControl(ccg1_);
	pressureSet_->addControl(ccg2_);
	pressureSet_->addControl(ccg3_);

	connect( pressureSet_, SIGNAL(connected(bool)), this, SLOT(onPressureSetConnected()) );

	onPressureSetConnected();

	// Vacuum valve controls.

	vvr1_ = new CLSBiStateControl("VVR1", "VVR1", "VVR1407-I00-01:state", "VVR1407-I00-01:opr:open", "VVR1407-I00-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr2_ = new CLSBiStateControl("VVR2", "VVR2", "VVR1607-5-I00-01:state", "VVR1607-5-I00-01:opr:open", "VVR1607-5-I00-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr3_ = new CLSBiStateControl("VVR3", "VVR3", "VVR1607-5-I22-01:state", "VVR1607-5-I22-01:opr:open", "VVR1607-5-I22-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr4_ = new CLSBiStateControl("VVR4", "VVR4", "VVR1607-5-I21-01:state", "VVR1607-5-I21-01:opr:open", "VVR1607-5-I21-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	vvr5_ = new CLSBiStateControl("VVR5", "VVR5", "VVR1607-5-I10-01:state", "VVR1607-5-I10-01:opr:open", "VVR1607-5-I10-01:opr:close", new AMControlStatusCheckerDefault(4), this);

	valveSet_ = new AMControlSet(this);
	valveSet_->addControl(vvr1_);
	valveSet_->addControl(vvr2_);
	valveSet_->addControl(vvr3_);
	valveSet_->addControl(vvr4_);
	valveSet_->addControl(vvr5_);

	connect( valveSet_, SIGNAL(connected(bool)), this, SLOT(onValveSetConnected()) );

	onValveSetConnected();

	// Ion pump controls.

	iop1_ = new AMReadOnlyPVControl("IOP1", "IOP1407-I00-01", this);
	iop2_ = new AMReadOnlyPVControl("IOP2", "IOP1407-I00-02", this);
	iop3_ = new AMReadOnlyPVControl("IOP3", "IOP1407-I00-03", this);
	iop4_ = new AMReadOnlyPVControl("IOP4", "IOP1607-5-I00-01", this);
	iop5_ = new AMReadOnlyPVControl("IOP5", "IOP1607-5-I00-02", this);

	ionPumpSet_ = new AMControlSet(this);
	ionPumpSet_->addControl(iop1_);
	ionPumpSet_->addControl(iop2_);
	ionPumpSet_->addControl(iop3_);
	ionPumpSet_->addControl(iop4_);
	ionPumpSet_->addControl(iop5_);

	connect( ionPumpSet_, SIGNAL(connected(bool)), this, SLOT(onIonPumpSetConnected()) );

	onIonPumpSetConnected();

	// Flow transducer controls.

	flt1_ = new AMReadOnlyPVwStatusControl("FLT1", "FLT1407-I00-01", "FLT1407-I00-01:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt2_ = new AMReadOnlyPVwStatusControl("FLT2", "FLT1407-I00-02", "FLT1407-I00-02:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt3_ = new AMReadOnlyPVwStatusControl("FLT3", "FLT1407-I00-03", "FLT1407-I00-03:lowflow", this, new AMControlStatusCheckerDefault(0));
	flt4_ = new AMReadOnlyPVwStatusControl("FLT4", "FLT1607-5-I00-01", "FLT1607-5-I00-01:lowflow", this, new AMControlStatusCheckerDefault(0));

	flowTransducerSet_ = new AMControlSet(this);
	flowTransducerSet_->addControl(flt1_);
	flowTransducerSet_->addControl(flt2_);
	flowTransducerSet_->addControl(flt3_);
	flowTransducerSet_->addControl(flt4_);

	connect( flowTransducerSet_, SIGNAL(connected(bool)), this, SLOT(onFlowTransducerSetConnected()) );

	onFlowTransducerSetConnected();

	// Flow switch controls.

	swf1_ = new AMReadOnlyPVControl("SWF1", "SWF1407-I00-01", this);
	swf2_ = new AMReadOnlyPVControl("SWF2", "SWF1407-I00-02", this);
	swf3_ = new AMReadOnlyPVControl("SWF3", "SWF1407-I00-03", this);
	swf4_ = new AMReadOnlyPVControl("SWF4", "SWF1607-5-I00-01", this);

	flowSwitchSet_ = new AMControlSet(this);
	flowSwitchSet_->addControl(swf1_);
	flowSwitchSet_->addControl(swf2_);
	flowSwitchSet_->addControl(swf3_);
	flowSwitchSet_->addControl(swf4_);

	connect( flowSwitchSet_, SIGNAL(connected(bool)), this, SLOT(onFlowSwitchSetConnected()) );

	onFlowSwitchSetConnected();

	// Temperature monitor controls.

	tm1_ = new AMReadOnlyPVwStatusControl("TM1", "TM1407-I00-01", "TM1407-I00-01:trip", this, new AMControlStatusCheckerDefault(0));
	tm2_ = new AMReadOnlyPVwStatusControl("TM2", "TM1407-I00-02", "TM1407-I00-02:trip", this, new AMControlStatusCheckerDefault(0));
	tm3_ = new AMReadOnlyPVwStatusControl("TM3", "TM1407-I00-03", "TM1407-I00-03:trip", this, new AMControlStatusCheckerDefault(0));
	tm4_ = new AMReadOnlyPVwStatusControl("TM4", "TM1407-I00-04", "TM1407-I00-04:trip", this, new AMControlStatusCheckerDefault(0));
	tm5_ = new AMReadOnlyPVwStatusControl("TM5", "TM1407-I00-05", "TM1407-I00-05:trip", this, new AMControlStatusCheckerDefault(0));
	tm6_ = new AMReadOnlyPVwStatusControl("TM6", "TM1607-5-I00-01", "TM1607-5-I00-01:trip", this, new AMControlStatusCheckerDefault(0));

	temperatureSet_ = new AMControlSet(this);
	temperatureSet_->addControl(tm1_);
	temperatureSet_->addControl(tm2_);
	temperatureSet_->addControl(tm3_);
	temperatureSet_->addControl(tm4_);
	temperatureSet_->addControl(tm5_);
	temperatureSet_->addControl(tm6_);

	connect( temperatureSet_, SIGNAL(connected(bool)), this, SLOT(onTemperatureSetConnected()) );

	onTemperatureSetConnected();
}

BioXASBeamline::BioXASBeamline(const QString &controlName) :
	CLSBeamline(controlName)
{
	// Initialize member variables.

	connected_ = false;

	photonShutterFEUpstream_ = 0;
	photonShutterFEDownstream_ = 0;
	safetyShutterFE_ = 0;

	ccg1_ = 0;
	ccg2_ = 0;
	ccg3_ = 0;
	pressureSet_ = 0;

	vvr1_ = 0;
	vvr2_ = 0;
	vvr3_ = 0;
	vvr4_ = 0;
	vvr5_ = 0;
	valveSet_ = 0;

	iop1_ = 0;
	iop2_ = 0;
	iop3_ = 0;
	iop4_ = 0;
	iop5_ = 0;
	ionPumpSet_ = 0;

	flt1_ = 0;
	flt2_ = 0;
	flt3_ = 0;
	flt4_ = 0;
	flowTransducerSet_ = 0;

	swf1_ = 0;
	swf2_ = 0;
	swf3_ = 0;
	swf4_ = 0;
	flowSwitchSet_ = 0;

	tm1_ = 0;
	tm2_ = 0;
	tm3_ = 0;
	tm4_ = 0;
	tm5_ = 0;
	tm6_ = 0;
	temperatureSet_ = 0;

	// Setup procedures.

	setupStorageRing();
	setupComponents();
	setupDiagnostics();
}
