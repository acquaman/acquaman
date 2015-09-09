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


#include "BioXASMainTableCoordinator.h"

#include <math.h>

#include "beamline/AMPVControl.h"


BioXASMainTableCoordinator::BioXASMainTableCoordinator(QObject *parent) :
	QObject(parent)
{
	tableVerticalMotorPosition_ = 600;
	tableVerticalDownstreamMotorPosition_ = 400;
	tableHorizontalMotorPosition_ = 600;

	initialized_ = false;
	connectedOnce_ = false;

	initializePhysicalMotorControls();
	initializeSoftIOCMotorControls();
	initializePVControlSet();
	initializeSignalConnector();
}

BioXASMainTableCoordinator::~BioXASMainTableCoordinator()
{
	if (allControls_)
		allControls_->clear();
}

void BioXASMainTableCoordinator::onAllControlsConnected(bool connected){
	if( connected ){
		connectedOnce_ = true;

		qDebug() << "Checking start up value from the OLD vertical upstream feedback as " << verticalUpstreamFeedbackControl_->value();
		qDebug() << "Checking start up value from the OLD vertical DownStream feedback as " << verticalDownstreamInboundFeedbackControl_->value();
		onVerticalFeedbackControlValueChanged();
		qDebug() << "Checking start up value from the OLD vertical upstream status as " << verticalUpstreamStatusControl_->value();
		qDebug() << "Checking start up value from the OLD vertical DownStream status as " << verticalDownstreamInboundStatusControl_->value();
		onVerticalStatusControlValueChanged();

		qDebug() << "Checking start up value from the OLD Horizontal upstream feedback as " << horizontalUpstreamFeedbackControl_->value();
		qDebug() << "Checking start up value from the OLD Horizontal DownStream feedback as " << horizontalDownstreamFeedbackControl_->value();
		onHorizontalFeedbackControlValueChanged();
		qDebug() << "Checking start up value from the OLD Horizontal upstream status as " << horizontalUpstreamStatusControl_->value();
		qDebug() << "Checking start up value from the OLD Horizontal DownStream status as " << horizontalDownstreamStatusControl_->value();
		onHorizontalStatusControlValueChanged();

		initialized_ = true;
	} else {
		initialized_ = false;
	}
}

void BioXASMainTableCoordinator::onVerticalFeedbackControlValueChanged()
{
	if ( !connectedOnce_ )
		return;

	double upstreamHeight = verticalUpstreamFeedbackControl_->value();
	double downstreamInboundHeight = verticalDownstreamInboundFeedbackControl_->value();
	double downstreamOutboundHeight = verticalDownstreamOutboundFeedbackControl_->value();
	manipulateVerticalPVChange(upstreamHeight, downstreamInboundHeight, downstreamOutboundHeight, softIOCHeightFeedbackControl_, softIOCPitchFeedbackControl_, softIOCRollFeedbackControl_);
}

void BioXASMainTableCoordinator::onVerticalStatusControlValueChanged()
{
	if ( !connectedOnce_ )
		return;

	// Status value: 0 - "MOVE DONE";  1 - "MOVE ACTIVE"; 2 - "AT LIMIT" ("MINOR"); 3 - "FORCED STOP" ("MINOR"); 4 - "ERROR" ("MAJOR")
	// Status is "MOVE ACTIVE" if at least one motor is moving or the largest status value
	double upstreamControlStatus = verticalUpstreamStatusControl_->value();

	double downstreamInboundControlStatus = verticalDownstreamInboundStatusControl_->value();
	double downstreamOutboundControlStatus = verticalDownstreamInboundStatusControl_->value();
	double downstreamControlStatus = (downstreamInboundControlStatus > downstreamOutboundControlStatus ? downstreamInboundControlStatus : downstreamOutboundControlStatus);

	double status = (upstreamControlStatus > downstreamControlStatus ? upstreamControlStatus : downstreamControlStatus);

	softIOCHeightStatusControl_->move(status);
	softIOCPitchStatusControl_->move(status);
	softIOCRollStatusControl_->move(status);
}

void BioXASMainTableCoordinator::onHorizontalFeedbackControlValueChanged()
{
	if ( !connectedOnce_ )
		return;

	double horizontalUpstreamFeedbackValue = horizontalUpstreamFeedbackControl_->value();
	double horizontalDownstreamFeedbackValue = horizontalDownstreamFeedbackControl_->value();
	manipulateHorizontalPVChange(horizontalUpstreamFeedbackValue, horizontalDownstreamFeedbackValue, softIOCLateralFeedbackControl_, softIOCYawFeedbackControl_);
}

void BioXASMainTableCoordinator::onHorizontalStatusControlValueChanged()
{
	if ( !connectedOnce_ )
		return;

	// Status value: 0 - "MOVE DONE";  1 - "MOVE ACTIVE"; 2 - "AT LIMIT" ("MINOR"); 3 - "FORCED STOP" ("MINOR"); 4 - "ERROR" ("MAJOR")
	// Status is "MOVE ACTIVE" if at least one motor is moving or the largest status value
	double upstreamControlStatus = horizontalUpstreamStatusControl_->value();
	double downstreamControlStatus = horizontalDownstreamStatusControl_->value();
	double status = (upstreamControlStatus > downstreamControlStatus ? upstreamControlStatus : downstreamControlStatus);

	softIOCLateralStatusControl_->move(status);
	softIOCYawStatusControl_->move(status);
}


void BioXASMainTableCoordinator::onHeightControlValueChanged(double value)
{
	// if we didn't finish the initializaion process, skip this signal
	if(!initialized_)
		return;

	double upstreamHeight = verticalUpstreamFeedbackControl_->value();
	double downstreamInboundHeight = verticalDownstreamInboundFeedbackControl_->value();
	double downstreamOutboundHeight = verticalDownstreamOutboundFeedbackControl_->value();

	// try to populate the height changes to the physical PVs
	double delta = value - softIOCHeightFeedbackControl_->value();
	double targetUpstreamHeight = upstreamHeight + delta;
	double targetDownstreamInboundHeight = downstreamInboundHeight + delta;
	double targetDownstreamOutboundHeight = downstreamOutboundHeight + delta;

	verticalUpstreamControl_->move(targetUpstreamHeight);
	verticalDownstreamInboundControl_->move(targetDownstreamInboundHeight);
	verticalDownstreamOutboundControl_->move(targetDownstreamOutboundHeight);
}

void BioXASMainTableCoordinator::onHeightStopControlValueChanged()
{
	if(!initialized_)
		return;

	if (softIOCHeightStopControl_->withinTolerance(1)) {
		verticalUpstreamStopControl_->move(1);
		verticalDownstreamInboundStopControl_->move(1);
		verticalDownstreamOutboundStopControl_->move(1);

		softIOCHeightStopControl_->move(0); // restore the value to 0
	}
}

void BioXASMainTableCoordinator::onPitchControlValueChanged(double angle)
{
	// if we didn't finish the initializaion process, skip this signal
	if(!initialized_)
		return;

	// downstream control will move in the direction of the angle and upstream will move against the direction of the angle
	double pitchDelta = tableVerticalMotorPosition_ * tan(degreeToRadian(angle));
	double targetUpstreamHeight = softIOCHeightFeedbackControl_->value() - pitchDelta ;
	double targetDownstreamHeight = softIOCHeightFeedbackControl_->value() + pitchDelta;

	double rollDelta = tableVerticalDownstreamMotorPosition_ * tan(degreeToRadian(softIOCRollFeedbackControl_->value()));
	double targetDownstreamInboundHeight = targetDownstreamHeight - rollDelta;
	double targetDownstreamOutboundHeight = targetDownstreamHeight + rollDelta;

	verticalUpstreamControl_->move(targetUpstreamHeight);
	verticalDownstreamInboundControl_->move(targetDownstreamInboundHeight);
	verticalDownstreamOutboundControl_->move(targetDownstreamOutboundHeight);
}

void BioXASMainTableCoordinator::onPitchStopControlValueChanged()
{
	if(!initialized_)
		return;

	if (softIOCPitchStopControl_->withinTolerance(1)) {
		verticalUpstreamStopControl_->move(1);
		verticalDownstreamInboundStopControl_->move(1);
		verticalDownstreamOutboundStopControl_->move(1);

		softIOCPitchStopControl_->move(0); // restore the value to 0
	}
}

void BioXASMainTableCoordinator::onRollControlValueChanged(double angle)
{
	// if we didn't finish the initializaion process, skip this signal
	if(!initialized_)
		return;

	// calculate the downstream table height
	double pitchDelta = tableVerticalMotorPosition_ * tan(degreeToRadian(softIOCPitchFeedbackControl_->value()));
	double downstreamTableHeight = softIOCHeightFeedbackControl_->value() + pitchDelta;

	// downstream outbound control will move in the direction of the angle and downstream inbound will move against the direction of the angle
	double rollDelta = tableVerticalDownstreamMotorPosition_ * tan(degreeToRadian(angle));
	double targetDownstreamInboundHeight = downstreamTableHeight - rollDelta;
	double targetDownstreamOutboundHeight = downstreamTableHeight + rollDelta;

	verticalDownstreamInboundControl_->move(targetDownstreamInboundHeight);
	verticalDownstreamOutboundControl_->move(targetDownstreamOutboundHeight);
}

void BioXASMainTableCoordinator::onRollStopControlValueChanged()
{
	if(!initialized_)
		return;

	if (softIOCRollStopControl_->withinTolerance(1)) {
		verticalUpstreamStopControl_->move(1);
		verticalDownstreamInboundStopControl_->move(1);
		verticalDownstreamOutboundStopControl_->move(1);

		softIOCRollStopControl_->move(0); // restore the value to 0
	}
}

void BioXASMainTableCoordinator::onLateralControlValueChanged(double value)
{
	if(!initialized_)
		return;

	double upstreamLateral = horizontalUpstreamFeedbackControl_->value();
	double downstreamLateral = horizontalDownstreamFeedbackControl_->value();

	// try to populate the lateral changes to the physical PVs
	double delta = value - softIOCLateralFeedbackControl_->value();
	double horizontalUpstreamPosition = upstreamLateral + delta;
	double horizontalDownstreamPosition = downstreamLateral + delta;

	horizontalUpstreamControl_->move(horizontalUpstreamPosition);
	horizontalDownstreamControl_->move(horizontalDownstreamPosition);
}

void BioXASMainTableCoordinator::onLateralStopControlValueChanged()
{
	if(!initialized_)
		return;

	if (softIOCLateralStopControl_->withinTolerance(1)) {
		horizontalUpstreamStopControl_->move(1);
		horizontalDownstreamStopControl_->move(1);

		softIOCLateralStopControl_->move(0); // restore the value to 0
	}
}

void BioXASMainTableCoordinator::onYawControlValueChanged(double angle)
{
	if(!initialized_)
		return;

	// try to populate the yaw changes to the physical PVs
	double setpoint = tableHorizontalMotorPosition_ * tan(degreeToRadian(angle));
	double targetUpstreamLateral = softIOCLateralFeedbackControl_->value() - setpoint ;
	double targetDownstreamLateral = softIOCLateralFeedbackControl_->value() + setpoint;

	horizontalUpstreamControl_->move(targetUpstreamLateral);
	horizontalDownstreamControl_->move(targetDownstreamLateral);
}

void BioXASMainTableCoordinator::onYawStopControlValueChanged()
{
	if(!initialized_)
		return;

	if (softIOCYawStopControl_->withinTolerance(1)) {
		horizontalUpstreamStopControl_->move(1);
		horizontalDownstreamStopControl_->move(1);

		softIOCYawStopControl_->move(0); // restore the value to 0
	}
}

void BioXASMainTableCoordinator::initializePhysicalMotorControls()
{
	verticalUpstreamControl_ = new AMSinglePVControl("Main Vertical Upstream", "SMTR1607-7-I21-02:mm", this, 0.001);
	verticalUpstreamFeedbackControl_ = new AMReadOnlyPVControl("Main Vertical Upstream Feedback", "SMTR1607-7-I21-02:mm:fbk", this);
	verticalUpstreamStatusControl_ = new AMReadOnlyPVControl("Main Vertical Upstream Status", "SMTR1607-7-I21-02:status", this);
	verticalUpstreamStopControl_ = new AMSinglePVControl("Main Vertical Upstream Stop", "SMTR1607-7-I21-02:stop", this);

	verticalDownstreamInboundControl_ = new AMSinglePVControl("Main Vertical Downstream Inbound", "SMTR1607-7-I21-03:mm", this, 0.001);
	verticalDownstreamInboundFeedbackControl_ = new AMReadOnlyPVControl("Main Vertical Downstream Inbound Feedback", "SMTR1607-7-I21-03:mm:fbk", this);
	verticalDownstreamInboundStatusControl_ = new AMReadOnlyPVControl("Main Vertical Downstream Inbound Status", "SMTR1607-7-I21-03:status", this);
	verticalDownstreamInboundStopControl_ = new AMSinglePVControl("Main Vertical Downstream Inbound Stop", "SMTR1607-7-I21-03:stop", this);

	verticalDownstreamOutboundControl_ = new AMSinglePVControl("Main Vertical Downstream Outbound", "SMTR1607-7-I21-04:mm", this, 0.001);
	verticalDownstreamOutboundFeedbackControl_ = new AMReadOnlyPVControl("Main Vertical Downstream Outbound Feedback", "SMTR1607-7-I21-04:mm:fbk", this);
	verticalDownstreamOutboundStatusControl_ = new AMReadOnlyPVControl("Main Vertical Downstream Outbound Status", "SMTR1607-7-I21-04:status", this);
	verticalDownstreamOutboundStopControl_ = new AMSinglePVControl("Main Vertical Downstream Outbound Stop", "SMTR1607-7-I21-04:stop", this);

	horizontalUpstreamControl_ = new AMSinglePVControl("Main Horizonal Upstream", "SMTR1607-7-I21-05:mm", this, 0.001);
	horizontalUpstreamFeedbackControl_ = new AMReadOnlyPVControl("Main Horizonal Upstream Feedback", "SMTR1607-7-I21-05:mm:fbk", this);
	horizontalUpstreamStatusControl_ = new AMReadOnlyPVControl("Main Horizonal Upstream Status", "SMTR1607-7-I21-05:status", this);
	horizontalUpstreamStopControl_ = new AMSinglePVControl("Main Horizonal Upstream Stop", "SMTR1607-7-I21-05:stop", this);

	horizontalDownstreamControl_ = new AMSinglePVControl("Main Horizonal Downstream", "SMTR1607-7-I21-06:mm", this, 0.001);
	horizontalDownstreamFeedbackControl_ = new AMReadOnlyPVControl("Main Horizonal Downstream Feedback", "SMTR1607-7-I21-06:mm:fbk", this);
	horizontalDownstreamStatusControl_ = new AMReadOnlyPVControl("Main Horizonal Downstream Status", "SMTR1607-7-I21-06:status", this);
	horizontalDownstreamStopControl_ = new AMSinglePVControl("Main Horizonal Downstream Stop", "SMTR1607-7-I21-06:stop", this);
}

void BioXASMainTableCoordinator::initializeSoftIOCMotorControls()
{
	softIOCHeightControl_ = new AMSinglePVControl("Main softIOC Height", "BL1607-7-I21:Height:mm", this, 0.001);
	softIOCHeightFeedbackControl_ = new AMSinglePVControl("Main softIOC Height Feedback", "BL1607-7-I21:Height:mm:fbk", this, 0.001);
	softIOCHeightStatusControl_ = new AMSinglePVControl("Main softIOC Height Status", "BL1607-7-I21:Height:status", this, 0.5);
	softIOCHeightStopControl_ = new AMSinglePVControl("Main softIOC Height Stop", "BL1607-7-I21:Height:stop", this, 0.5);

	softIOCPitchControl_ = new AMSinglePVControl("Main softIOC Pitch", "BL1607-7-I21:Pitch:deg", this, 0.001);
	softIOCPitchFeedbackControl_ = new AMSinglePVControl("Main softIOC Pitch Feedback", "BL1607-7-I21:Pitch:deg:fbk", this, 0.001);
	softIOCPitchStatusControl_ = new AMSinglePVControl("Main softIOC Pitch Status", "BL1607-7-I21:Pitch:status", this, 0.5);
	softIOCPitchStopControl_ = new AMSinglePVControl("Main softIOC Pitch Stop", "BL1607-7-I21:Pitch:stop", this, 0.5);

	softIOCRollControl_ = new AMSinglePVControl("Main softIOC Roll", "BL1607-7-I21:Roll:deg", this, 0.001);
	softIOCRollFeedbackControl_ = new AMSinglePVControl("Main softIOC Roll Feedback", "BL1607-7-I21:Roll:deg:fbk", this, 0.001);
	softIOCRollStatusControl_ = new AMSinglePVControl("Main softIOC Roll Status", "BL1607-7-I21:Roll:status", this, 0.5);
	softIOCRollStopControl_ = new AMSinglePVControl("Main softIOC Roll Stop", "BL1607-7-I21:Roll:stop", this, 0.5);

	softIOCLateralControl_ = new AMSinglePVControl("Main softIOC Lateral", "BL1607-7-I21:Lateral:mm", this, 0.001);
	softIOCLateralFeedbackControl_ = new AMSinglePVControl("Main softIOC Lateral Feedback", "BL1607-7-I21:Lateral:mm:fbk", this, 0.001);
	softIOCLateralStatusControl_ = new AMSinglePVControl("Main softIOC Lateral Status", "BL1607-7-I21:Lateral:status", this, 0.5);
	softIOCLateralStopControl_ = new AMSinglePVControl("Main softIOC Lateral Stop", "BL1607-7-I21:Lateral:stop", this, 0.5);

	softIOCYawControl_ = new AMSinglePVControl("Main softIOC Yaw", "BL1607-7-I21:Yaw:deg", this, 0.001);
	softIOCYawFeedbackControl_ = new AMSinglePVControl("Main softIOC Yaw Feedback", "BL1607-7-I21:Yaw:deg:fbk", this, 0.001);
	softIOCYawStatusControl_ = new AMSinglePVControl("Main softIOC Yaw Status", "BL1607-7-I21:Yaw:status", this, 0.5);
	softIOCYawStopControl_ = new AMSinglePVControl("Main softIOC Yaw Stop", "BL1607-7-I21:Yaw:stop", this, 0.5);
}

void BioXASMainTableCoordinator::initializePVControlSet()
{
	allControls_ = new AMControlSet(this);

	// physical PV controls
	allControls_->addControl(verticalUpstreamControl_);
	allControls_->addControl(verticalUpstreamFeedbackControl_);
	allControls_->addControl(verticalUpstreamStatusControl_);
	allControls_->addControl(verticalUpstreamStopControl_);
	allControls_->addControl(verticalDownstreamInboundControl_);
	allControls_->addControl(verticalDownstreamInboundFeedbackControl_);
	allControls_->addControl(verticalDownstreamInboundStatusControl_);
	allControls_->addControl(verticalDownstreamInboundStopControl_);
	allControls_->addControl(verticalDownstreamOutboundControl_);
	allControls_->addControl(verticalDownstreamOutboundFeedbackControl_);
	allControls_->addControl(verticalDownstreamOutboundStatusControl_);
	allControls_->addControl(verticalDownstreamOutboundStopControl_);
	allControls_->addControl(horizontalUpstreamControl_);
	allControls_->addControl(horizontalUpstreamFeedbackControl_);
	allControls_->addControl(horizontalUpstreamStatusControl_);
	allControls_->addControl(horizontalUpstreamStopControl_);
	allControls_->addControl(horizontalDownstreamControl_);
	allControls_->addControl(horizontalDownstreamFeedbackControl_);
	allControls_->addControl(horizontalDownstreamStatusControl_);
	allControls_->addControl(horizontalDownstreamStopControl_);

	// SoftIOC PV controls
	allControls_->addControl(softIOCHeightControl_);
	allControls_->addControl(softIOCHeightFeedbackControl_);
	allControls_->addControl(softIOCHeightStatusControl_);
	allControls_->addControl(softIOCHeightStopControl_);
	allControls_->addControl(softIOCPitchControl_);
	allControls_->addControl(softIOCPitchFeedbackControl_);
	allControls_->addControl(softIOCPitchStatusControl_);
	allControls_->addControl(softIOCPitchStopControl_);
	allControls_->addControl(softIOCRollControl_);
	allControls_->addControl(softIOCRollFeedbackControl_);
	allControls_->addControl(softIOCRollStatusControl_);
	allControls_->addControl(softIOCRollStopControl_);

	allControls_->addControl(softIOCLateralControl_);
	allControls_->addControl(softIOCLateralFeedbackControl_);
	allControls_->addControl(softIOCLateralStatusControl_);
	allControls_->addControl(softIOCLateralStopControl_);
	allControls_->addControl(softIOCYawControl_);
	allControls_->addControl(softIOCYawFeedbackControl_);
	allControls_->addControl(softIOCYawStatusControl_);
	allControls_->addControl(softIOCYawStopControl_);
}

void BioXASMainTableCoordinator::initializeSignalConnector()
{
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));

	// physical PV controls
	connect(verticalUpstreamFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalFeedbackControlValueChanged()));
	connect(verticalUpstreamStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalStatusControlValueChanged()));

	connect(verticalDownstreamInboundFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalFeedbackControlValueChanged()));
	connect(verticalDownstreamInboundStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalStatusControlValueChanged()));

	connect(verticalDownstreamOutboundFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalFeedbackControlValueChanged()));
	connect(verticalDownstreamOutboundStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalStatusControlValueChanged()));

	connect(horizontalUpstreamFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalFeedbackControlValueChanged()));
	connect(horizontalUpstreamStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalStatusControlValueChanged()));

	connect(horizontalDownstreamFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalFeedbackControlValueChanged()));
	connect(horizontalDownstreamStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalStatusControlValueChanged()));


	// SoftIOC PV controls
	connect(softIOCHeightControl_, SIGNAL(valueChanged(double)), this, SLOT(onHeightControlValueChanged(double)));
	connect(softIOCHeightStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onHeightStopControlValueChanged()));
	connect(softIOCPitchControl_, SIGNAL(valueChanged(double)), this, SLOT(onPitchControlValueChanged(double)));
	connect(softIOCPitchStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onPitchStopControlValueChanged()));
	connect(softIOCRollControl_, SIGNAL(valueChanged(double)), this, SLOT(onRollControlValueChanged(double)));
	connect(softIOCRollStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onRollStopControlValueChanged()));

	connect(softIOCLateralControl_, SIGNAL(valueChanged(double)), this, SLOT(onLateralControlValueChanged(double)));
	connect(softIOCLateralStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onLateralStopControlValueChanged()));
	connect(softIOCYawControl_, SIGNAL(valueChanged(double)), this, SLOT(onYawControlValueChanged(double)));
	connect(softIOCYawStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onYawStopControlValueChanged()));

}

double BioXASMainTableCoordinator::calculateTableHeight(double upstreamHeight, double downstreamInboundHeight, double downstreamOutboundHeight) const
{
	double downstreamHeight = (downstreamInboundHeight + downstreamOutboundHeight) / 2;

	// The vertical height is the height of the center of the table, which is the average of upstream value and the downstream value
	return (upstreamHeight + downstreamHeight) / 2;
}

double BioXASMainTableCoordinator::calculateTablePitch(double upstreamHeight, double downstreamInboundHeight, double downstreamOutboundHeight) const
{
	double downstreamHeight = (downstreamInboundHeight + downstreamOutboundHeight) / 2;

	// The pitch is the degree of the arcTangent value of (dH - uH) / distance
	return radianToDegree(atan((downstreamHeight - upstreamHeight) / (tableVerticalMotorPosition_ * 2)));
}

double BioXASMainTableCoordinator::calculateTableRoll(double downstreamInboundHeight, double downstreamOutboundHeight) const
{
	// The Roll is the degree of the arcTangent value of (dOutH - dInH) / distance
	return radianToDegree(atan((downstreamOutboundHeight - downstreamInboundHeight) / (tableVerticalDownstreamMotorPosition_ * 2)));
}

double BioXASMainTableCoordinator::calculateTableLateral(const double upstreamLateral, const double downstreamLateral) const
{
	// The lateral is the lateral of the center of the table, which is the average of upstream value and the downstream value
	return ( upstreamLateral + downstreamLateral ) / 2;
}

double BioXASMainTableCoordinator::calculateTableYaw(const double upstreamLateral, const double downstreamLateral) const
{
	// The yaw is the degree of the arcTangent value of (dH - uH) / distance
	return radianToDegree(atan((downstreamLateral - upstreamLateral) / (tableHorizontalMotorPosition_ * 2)));
}

void BioXASMainTableCoordinator::manipulateVerticalPVChange(double upstreamHeight, double downstreamInboundHeight, double downstreamOutboundHeight, AMControl* heightPV, AMControl *pitchPV, AMControl *rollPV)
{
	double height = calculateTableHeight(upstreamHeight, downstreamInboundHeight, downstreamOutboundHeight);
	double pitch =  calculateTablePitch(upstreamHeight, downstreamInboundHeight, downstreamOutboundHeight);
	double roll =  calculateTableRoll(downstreamInboundHeight, downstreamOutboundHeight);

	if (!heightPV->withinTolerance(height)) {
		heightPV->move(height);
	}

	if (!pitchPV->withinTolerance(pitch)) {
		pitchPV->move(pitch);
	}

	if (!rollPV->withinTolerance(roll)) {
		rollPV->move(roll);
	}
}

void BioXASMainTableCoordinator::manipulateHorizontalPVChange(double upstreamOffset, double downstreamOffset, AMControl* lateralPV, AMControl* yawPV)
{
	double lateral = calculateTableLateral(upstreamOffset, downstreamOffset);
	double yaw = calculateTableYaw(upstreamOffset, downstreamOffset);

	if ( !lateralPV->withinTolerance(lateral) ) {
		lateralPV->move(lateral);
	}
	if ( !yawPV->withinTolerance(yaw) ) {
		yawPV->move(yaw);
	}
}
