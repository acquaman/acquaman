#include "BioXASBeamlineUtilities.h"
#include "util/AMErrorMonitor.h"

BioXASBeamlineUtilities::BioXASBeamlineUtilities(QObject *parent) :
	BioXASBeamlineComponent("BioXASFrontEndUtilities", parent)
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

	vvr1_ = new CLSExclusiveStatesControl("VVR1407-I00-01", "VVR1407-I00-01:state", "VVR1407-I00-01:opr:open", "VVR1407-I00-01:opr:close", this);
	vvr2_ = new CLSExclusiveStatesControl("VVR1607-5-I00-01", "VVR1607-5-I00-01:state", "VVR1607-5-I00-01:opr:open", "VVR1607-5-I00-01:opr:close", this);
	vvr3_ = new CLSExclusiveStatesControl("VVR1607-5-I22-01", "VVR1607-5-I22-01:state", "VVR1607-5-I22-01:opr:open", "VVR1607-5-I22-01:opr:close", this);
	vvr4_ = new CLSExclusiveStatesControl("VVR1607-5-I21-01", "VVR1607-5-I21-01:state", "VVR1607-5-I21-01:opr:open", "VVR1607-5-I21-01:opr:close", this);
	vvr5_ = new CLSExclusiveStatesControl("VVR1607-5-I10-01", "VVR1607-5-I10-01:state", "VVR1607-5-I10-01:opr:open", "VVR1607-5-I10-01:opr:close", this);

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

BioXASBeamlineUtilities::~BioXASBeamlineUtilities()
{

}

bool BioXASBeamlineUtilities::isConnected() const
{
	bool connected = (
				pressureSet_ && pressureSet_->isConnected() &&
				valveSet_ && valveSet_->isConnected() &&
				ionPumpSet_ && ionPumpSet_->isConnected() &&
				flowTransducerSet_ && flowTransducerSet_->isConnected() &&
				flowSwitchSet_ && flowSwitchSet_->isConnected() &&
				temperatureSet_ && temperatureSet_->isConnected()
				);

	return connected;
}

void BioXASBeamlineUtilities::onPressureSetConnected()
{
	if (pressureSet_ && pressureSet_->isConnected()) {
		for(int i = 0; i < pressureSet_->count(); i++)
			connect( pressureSet_->at(i), SIGNAL(movingChanged(bool)), this, SLOT(onPressureError()) );
	}

	onPressureError();
	updateConnected();
}

void BioXASBeamlineUtilities::onPressureError()
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

void BioXASBeamlineUtilities::onValveSetConnected()
{
	if (valveSet_ && valveSet_->isConnected()) {
		connect( valveSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(onValveError()) );
	}

	onValveError();
	updateConnected();
}

void BioXASBeamlineUtilities::onValveError()
{
	if (valveSet_ && valveSet_->isConnected()) {

		QString error("");
		CLSExclusiveStatesControl *current = 0;

		for (int i = 0; i < valveSet_->count(); i++) {

			if (i == 0) {
				AMReadOnlyPVwStatusControl *first = qobject_cast<AMReadOnlyPVwStatusControl *>(valveSet_->at(i));

				if (first && first->isMoving())
					error += QString("%1 (%2)\n").arg(first->name()).arg(first->movingPVName());

			} else {
				current = qobject_cast<CLSExclusiveStatesControl *>(valveSet_->at(i));

				if (current && current->isClosed())
					error += QString("%1 (%2)\n").arg(current->name()).arg(current->name());
			}
		}

		if (!error.isEmpty()){
			error.prepend("The following valves are closed:\n");
			AMErrorMon::error(this, BIOXASBEAMLINE_VALVES_CLOSED, error);
		}

		emit valveStatusChanged(error.isEmpty());
	}
}

void BioXASBeamlineUtilities::onIonPumpSetConnected()
{
	if (ionPumpSet_ && ionPumpSet_->isConnected()) {
		connect( ionPumpSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(onIonPumpError()) );
	}

	onIonPumpError();
	updateConnected();
}

void BioXASBeamlineUtilities::onIonPumpError()
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

void BioXASBeamlineUtilities::onFlowTransducerSetConnected()
{
	if (flowTransducerSet_ && flowTransducerSet_->isConnected()) {
		for (int i = 0; i < flowTransducerSet_->count(); i++)
			connect( flowTransducerSet_->at(i), SIGNAL(movingChanged(bool)), this, SLOT(onFlowTransducerError()) );
	}

	onFlowTransducerError();
	updateConnected();
}

void BioXASBeamlineUtilities::onFlowTransducerError()
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

void BioXASBeamlineUtilities::onFlowSwitchSetConnected()
{
	if (flowSwitchSet_ && flowSwitchSet_->isConnected()) {
		connect( flowSwitchSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(onFlowSwitchError()) );
	}

	onFlowSwitchError();
	updateConnected();
}

void BioXASBeamlineUtilities::onFlowSwitchError()
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

void BioXASBeamlineUtilities::onTemperatureSetConnected()
{
	if (temperatureSet_ && temperatureSet_->isConnected()) {
		for (int i = 0; i < temperatureSet_->count(); i++)
			connect( temperatureSet_->at(i), SIGNAL(movingChanged(bool)), this, SLOT(onTemperatureError()) );
	}

	onTemperatureError();
	updateConnected();
}

void BioXASBeamlineUtilities::onTemperatureError()
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
