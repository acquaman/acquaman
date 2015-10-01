#include "BioXASValvesControl.h"

BioXASValvesControl::BioXASValvesControl(QObject *parent) :
    AMPseudoMotorControl(parent)
{
	// Initialize inherited variables.

	value_ = None;
	setpoint_ = None;
	minimumValue_ = None;
	maximumValue_ = Closed;

	setEnumStates(QStringList() << valueToString(None) << valueToString(Open) << valueToString(Closed));
	setMoveEnumStates(QStringList() << valueToString(Open));

	setContextKnownDescription("ValvesControl");

	// Create child controls.
	// Front-end vacuum valves.

	vvr1_ = new CLSBiStateControl("VVR1", "VVR1", "VVR1407-I00-01:state", "VVR1407-I00-01:opr:open", "VVR1407-I00-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	addChildControl(vvr1_);

	vvr2_ = new CLSBiStateControl("VVR2", "VVR2", "VVR1607-5-I00-01:state", "VVR1607-5-I00-01:opr:open", "VVR1607-5-I00-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	addChildControl(vvr2_);

	vvr3_ = new CLSBiStateControl("VVR3", "VVR3", "VVR1607-5-I22-01:state", "VVR1607-5-I22-01:opr:open", "VVR1607-5-I22-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	addChildControl(vvr3_);

	vvr4_ = new CLSBiStateControl("VVR4", "VVR4", "VVR1607-5-I21-01:state", "VVR1607-5-I21-01:opr:open", "VVR1607-5-I21-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	addChildControl(vvr4_);

	vvr5_ = new CLSBiStateControl("VVR5", "VVR5", "VVR1607-5-I10-01:state", "VVR1607-5-I10-01:opr:open", "VVR1607-5-I10-01:opr:close", new AMControlStatusCheckerDefault(4), this);
	addChildControl(vvr5_);

	// Side beamline vacuum valves.

	vvrSide1_ = new CLSBiStateControl("Side valve control 1", "Side valve control 1", "VVR1607-5-I22-02:state", "VVR1607-5-I22-02:opr:open", "VVR1607-5-I22-02:opr:close", new AMControlStatusCheckerDefault(4), this);
	addChildControl(vvrSide1_);

	vvrSide2_ = new CLSBiStateControl("Side valve control 2", "Side valve control 2", "VVR1607-5-I22-03:state", "VVR1607-5-I22-03:opr:open", "VVR1607-5-I22-03:opr:close", new AMControlStatusCheckerDefault(4), this);
	addChildControl(vvrSide2_);

	vvrSide3_ = new CLSBiStateControl("Side valve control 3", "Side valve control 3", "VVR1607-5-I22-04:state", "VVR1607-5-I22-04:opr:open", "VVR1607-5-I22-04:opr:close", new AMControlStatusCheckerDefault(4), this);
	addChildControl(vvrSide3_);

	vvrSide4_ = new CLSBiStateControl("Side valve control 4", "Side valve control 4", "VVR1607-5-I22-05:state", "VVR1607-5-I22-05:opr:open", "VVR1607-5-I22-05:opr:close", new AMControlStatusCheckerDefault(4), this);
	addChildControl(vvrSide4_);

	vvrSide5_ = new CLSBiStateControl("Side valve control 5", "Side valve control 5", "VVR1607-5-I22-06:state", "VVR1607-5-I22-06:opr:open", "VVR1607-5-I22-06:opr:close", new AMControlStatusCheckerDefault(4), this);
	addChildControl(vvrSide5_);

	vvrSide6_ = new CLSBiStateControl("Side valve control 6", "Side valve control 6", "VVR1607-5-I22-07:state", "VVR1607-5-I22-07:opr:open", "VVR1607-5-I22-07:opr:close", new AMControlStatusCheckerDefault(4), this);
	addChildControl(vvrSide6_);

	// Main beamline vacuum valves.

	// Imaging beamline vacuum valves.

	// Current settings.

	updateStates();
}

BioXASValvesControl::~BioXASValvesControl()
{

}

bool BioXASValvesControl::canMeasure() const
{
	return false;
}

bool BioXASValvesControl::canMove() const
{
	return false;
}

bool BioXASValvesControl::isOpen() const
{
	bool open = false;

	if (isConnected()) {
		open = (
				vvr1_->isOpen() &&
				vvr2_->isOpen() &&
				vvr3_->isOpen() &&
				vvr4_->isOpen() &&
				vvr5_->isOpen() &&

				vvrSide1_->isOpen() &&
				vvrSide2_->isOpen() &&
				vvrSide3_->isOpen() &&
				vvrSide4_->isOpen() &&
				vvrSide5_->isOpen() &&
				vvrSide6_->isOpen()

				);
	}

	return open;
}

bool BioXASValvesControl::isClosed() const
{
	bool closed = (
				vvr1_->isClosed() &&
				vvr2_->isClosed() &&
				vvr3_->isClosed() &&
				vvr4_->isClosed() &&
				vvr5_->isClosed() &&

				vvrSide1_->isClosed() &&
				vvrSide2_->isClosed() &&
				vvrSide3_->isClosed() &&
				vvrSide4_->isClosed() &&
				vvrSide5_->isClosed() &&
				vvrSide6_->isClosed()

				);

	return closed;
}

QString BioXASValvesControl::valueToString(BioXASValvesControl::Value value)
{
	QString result;

	switch (value) {
	case None:
		result = "None";
		break;
	case Open:
		result = "Open";
		break;
	case Closed:
		result = "Closed";
		break;
	default:
		break;
	}

	return result;
}

void BioXASValvesControl::updateConnected()
{
	bool connected = (
				vvr1_ && vvr1_->isOpen() &&
				vvr2_ && vvr2_->isOpen() &&
				vvr3_ && vvr3_->isOpen() &&
				vvr4_ && vvr4_->isOpen() &&
				vvr5_ && vvr5_->isOpen() &&

				vvrSide1_ && vvrSide1_->isOpen() &&
				vvrSide2_ && vvrSide2_->isOpen() &&
				vvrSide3_ && vvrSide3_->isOpen() &&
				vvrSide4_ && vvrSide4_->isOpen() &&
				vvrSide5_ && vvrSide5_->isOpen() &&
				vvrSide6_ && vvrSide6_->isOpen()

				);

	setConnected(connected);
}

void BioXASValvesControl::updateValue()
{
	Value newValue = None;

	if (isOpen())
		newValue = Open;
	else if (isClosed())
		newValue = Closed;

	setValue(newValue);
}

void BioXASValvesControl::updateMoving()
{
	bool closed = (
				vvr1_->isClosed() &&
				vvr2_->isClosed() &&
				vvr3_->isClosed() &&
				vvr4_->isClosed() &&
				vvr5_->isClosed() &&

				vvrSide1_->isClosed() &&
				vvrSide2_->isClosed() &&
				vvrSide3_->isClosed() &&
				vvrSide4_->isClosed() &&
				vvrSide5_->isClosed() &&
				vvrSide6_->isClosed()

				);

	return closed;
}
