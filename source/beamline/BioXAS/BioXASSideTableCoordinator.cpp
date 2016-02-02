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


#include "BioXASSideTableCoordinator.h"

#include <math.h>

#include "beamline/AMPVControl.h"


BioXASSideTableCoordinator::BioXASSideTableCoordinator(QObject *parent) :
	QObject(parent)
{
	tableVerticalMotorPosition_ = 619.125;
	tableHorizontalMotorPosition_ = 543.725;

	initialized_ = false;
	connectedOnce_ = false;

	initializePhysicalMotorControls();
	initializeSoftIOCMotorControls();
	initializePVControlSet();
	initializeSignalConnector();
}

BioXASSideTableCoordinator::~BioXASSideTableCoordinator(){}

void BioXASSideTableCoordinator::onAllControlsConnected(bool connected){
	if( connected ){
		connectedOnce_ = true;

		qDebug() << "Checking start up value from the OLD vertical upstream feedback as " << verticalUpstreamFeedbackControl_->value();
		qDebug() << "Checking start up value from the OLD vertical DownStream feedback as " << verticalDownstreamFeedbackControl_->value();
		onVerticalFeedbackControlValueChanged();
		qDebug() << "Checking start up value from the OLD vertical upstream status as " << verticalUpstreamStatusControl_->value();
		qDebug() << "Checking start up value from the OLD vertical DownStream status as " << verticalDownstreamStatusControl_->value();
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

void BioXASSideTableCoordinator::onVerticalFeedbackControlValueChanged()
{
	if ( !connectedOnce_ )
		return;

	double upstreamHeight = verticalUpstreamFeedbackControl_->value();
	double downstreamHeight = verticalDownstreamFeedbackControl_->value();
	manipulateVerticalPVChange(upstreamHeight, downstreamHeight, softIOCHeightFeedbackControl_, softIOCPitchFeedbackControl_);
}

void BioXASSideTableCoordinator::onVerticalStatusControlValueChanged()
{
	if ( !connectedOnce_ )
		return;

	// Status value: 0 - "MOVE DONE";  1 - "MOVE ACTIVE"; 2 - "AT LIMIT" ("MINOR"); 3 - "FORCED STOP" ("MINOR"); 4 - "ERROR" ("MAJOR")
	// Status is "MOVE ACTIVE" if at least one motor is moving or the largest status value
	double upstreamControlStatus = verticalUpstreamStatusControl_->value();
	double downstreamControlStatus = verticalDownstreamStatusControl_->value();
	double status = upstreamControlStatus > downstreamControlStatus ? upstreamControlStatus : downstreamControlStatus;

	softIOCHeightStatusControl_->move(status);
	softIOCPitchStatusControl_->move(status);
}

void BioXASSideTableCoordinator::onHorizontalFeedbackControlValueChanged()
{
	if ( !connectedOnce_ )
		return;

	double horizontalUpstreamFeedbackValue = horizontalUpstreamFeedbackControl_->value();
	double horizontalDownstreamFeedbackValue = horizontalDownstreamFeedbackControl_->value();
	manipulateHorizontalPVChange(horizontalUpstreamFeedbackValue, horizontalDownstreamFeedbackValue, softIOCLateralFeedbackControl_, softIOCYawFeedbackControl_);
}

void BioXASSideTableCoordinator::onHorizontalStatusControlValueChanged()
{
	if ( !connectedOnce_ )
		return;

	// Status value: 0 - "MOVE DONE";  1 - "MOVE ACTIVE"; 2 - "AT LIMIT" ("MINOR"); 3 - "FORCED STOP" ("MINOR"); 4 - "ERROR" ("MAJOR")
	// Status is "MOVE ACTIVE" if at least one motor is moving or the largest status value
	double upstreamControlStatus = horizontalUpstreamStatusControl_->value();
	double downstreamControlStatus = horizontalDownstreamStatusControl_->value();
	double status = upstreamControlStatus > downstreamControlStatus ? upstreamControlStatus : downstreamControlStatus;

	softIOCLateralStatusControl_->move(status);
	softIOCYawStatusControl_->move(status);
}


void BioXASSideTableCoordinator::onHeightControlValueChanged(double value)
{
	// if we didn't finish the initializaion process, skip this signal
	if(!initialized_)
		return;

	double upstreamHeight = verticalUpstreamFeedbackControl_->value();
	double downstreamHeight = verticalDownstreamFeedbackControl_->value();

	// try to populate the height changes to the physical PVs
	double delta = value - softIOCHeightFeedbackControl_->value();
	double targetUpstreamHeight = upstreamHeight + delta;
	double targetDownstreamHeight = downstreamHeight + delta;

	verticalUpstreamControl_->move(targetUpstreamHeight);
	verticalDownstreamControl_->move(targetDownstreamHeight);
}

void BioXASSideTableCoordinator::onHeightStopControlValueChanged()
{
	if(!initialized_)
		return;

	if (softIOCHeightStopControl_->withinTolerance(1)) {
		verticalUpstreamStopControl_->move(1);
		verticalDownstreamStopControl_->move(1);

		softIOCHeightStopControl_->move(0); // restore the value to 0
	}
}

void BioXASSideTableCoordinator::onPitchControlValueChanged(double angle)
{
	// if we didn't finish the initializaion process, skip this signal
	if(!initialized_)
		return;

	// downstream control will move in the direction of the angle and upstream will move against the direction of the angle
	double delta = tableVerticalMotorPosition_ * tan(degreeToRadian(angle));
	double targetUpstreamHeight = softIOCHeightFeedbackControl_->value() - delta ;
	double targetDownstreamHeight = softIOCHeightFeedbackControl_->value() + delta;

	verticalUpstreamControl_->move(targetUpstreamHeight);
	verticalDownstreamControl_->move(targetDownstreamHeight);
}

void BioXASSideTableCoordinator::onPitchStopControlValueChanged()
{
	if(!initialized_)
		return;

	if (softIOCPitchStopControl_->withinTolerance(1)) {
		verticalUpstreamStopControl_->move(1);
		verticalDownstreamStopControl_->move(1);

		softIOCPitchStopControl_->move(0); // restore the value to 0
	}
}

void BioXASSideTableCoordinator::onLateralControlValueChanged(double value)
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

void BioXASSideTableCoordinator::onLateralStopControlValueChanged()
{
	if(!initialized_)
		return;

	if (softIOCLateralStopControl_->withinTolerance(1)) {
		horizontalUpstreamStopControl_->move(1);
		horizontalDownstreamStopControl_->move(1);

		softIOCLateralStopControl_->move(0); // restore the value to 0
	}
}

void BioXASSideTableCoordinator::onYawControlValueChanged(double angle)
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

void BioXASSideTableCoordinator::onYawStopControlValueChanged()
{
	if(!initialized_)
		return;

	if (softIOCYawStopControl_->withinTolerance(1)) {
		horizontalUpstreamStopControl_->move(1);
		horizontalDownstreamStopControl_->move(1);

		softIOCYawStopControl_->move(0); // restore the value to 0
	}
}

void BioXASSideTableCoordinator::initializePhysicalMotorControls()
{
	// ============ Physical PVs ==================
	verticalUpstreamControl_ = new AMSinglePVControl("Side Vertical Upstream", "SMTR1607-6-I22-02:mm", this, 0.001);
	verticalUpstreamFeedbackControl_ = new AMReadOnlyPVControl("Side Vertical Upstream Feedback", "SMTR1607-6-I22-02:mm:fbk", this);
	verticalUpstreamStatusControl_ = new AMReadOnlyPVControl("Side Vertical Upstream Status", "SMTR1607-6-I22-02:status", this);
	verticalUpstreamStopControl_ = new AMSinglePVControl("Side Vertical Upstream Stop", "SMTR1607-6-I22-02:stop", this);

	verticalDownstreamControl_ = new AMSinglePVControl("Side Vertical Downstream", "SMTR1607-6-I22-03:mm", this, 0.001);
	verticalDownstreamFeedbackControl_ = new AMReadOnlyPVControl("Side Vertical Downstream Feedback", "SMTR1607-6-I22-03:mm:fbk", this);
	verticalDownstreamStatusControl_ = new AMReadOnlyPVControl("Side Vertical Downstream Status", "SMTR1607-6-I22-03:status", this);
	verticalDownstreamStopControl_ = new AMSinglePVControl("Side Vertical Downstream Stop", "SMTR1607-6-I22-03:stop", this);


	horizontalUpstreamControl_ = new AMSinglePVControl("Side Horizonal Upstream", "SMTR1607-6-I22-04:mm", this, 0.001);
	horizontalUpstreamFeedbackControl_ = new AMReadOnlyPVControl("Side Horizonal Upstream Feedback", "SMTR1607-6-I22-04:mm:fbk", this);
	horizontalUpstreamStatusControl_ = new AMReadOnlyPVControl("Side Horizonal Upstream Status", "SMTR1607-6-I22-04:status", this);
	horizontalUpstreamStopControl_ = new AMSinglePVControl("Side Horizonal Upstream Stop", "SMTR1607-6-I22-04:stop", this);

	horizontalDownstreamControl_ = new AMSinglePVControl("Side Horizonal Downstream", "SMTR1607-6-I22-05:mm", this, 0.001);
	horizontalDownstreamFeedbackControl_ = new AMReadOnlyPVControl("Side Horizonal Downstream Feedback", "SMTR1607-6-I22-05:mm:fbk", this);
	horizontalDownstreamStatusControl_ = new AMReadOnlyPVControl("Side Horizonal Downstream Status", "SMTR1607-6-I22-05:status", this);
	horizontalDownstreamStopControl_ = new AMSinglePVControl("Side Horizonal Downstream Stop", "SMTR1607-6-I22-05:stop", this);
}

void BioXASSideTableCoordinator::initializeSoftIOCMotorControls()
{
	// ============ SoftIOC PVs ==================
	softIOCHeightControl_ = new AMSinglePVControl("Side softIOC Height", "BL1607-6-I22:Height:mm", this, 0.001);
	softIOCHeightFeedbackControl_ = new AMSinglePVControl("Side softIOC Height Feedback", "BL1607-6-I22:Height:mm:fbk", this, 0.001);
	softIOCHeightStatusControl_ = new AMSinglePVControl("Side softIOC Height Status", "BL1607-6-I22:Height:status", this, 0.5);
	softIOCHeightStopControl_ = new AMSinglePVControl("Side softIOC Height Stop", "BL1607-6-I22:Height:stop", this, 0.5);

	softIOCPitchControl_ = new AMSinglePVControl("Side softIOC Pitch", "BL1607-6-I22:Pitch:deg", this, 0.001);
	softIOCPitchFeedbackControl_ = new AMSinglePVControl("Side softIOC Pitch Feedback", "BL1607-6-I22:Pitch:deg:fbk", this, 0.001);
	softIOCPitchStatusControl_ = new AMSinglePVControl("Side softIOC Pitch Status", "BL1607-6-I22:Pitch:status", this, 0.5);
	softIOCPitchStopControl_ = new AMSinglePVControl("Side softIOC Pitch Stop", "BL1607-6-I22:Pitch:stop", this, 0.5);

	softIOCLateralControl_ = new AMSinglePVControl("Side softIOC Lateral", "BL1607-6-I22:Lateral:mm", this, 0.001);
	softIOCLateralFeedbackControl_ = new AMSinglePVControl("Side softIOC Lateral Feedback", "BL1607-6-I22:Lateral:mm:fbk", this, 0.001);
	softIOCLateralStatusControl_ = new AMSinglePVControl("Side softIOC Lateral Status", "BL1607-6-I22:Lateral:status", this, 0.5);
	softIOCLateralStopControl_ = new AMSinglePVControl("Side softIOC Lateral Stop", "BL1607-6-I22:Lateral:stop", this, 0.5);

	softIOCYawControl_ = new AMSinglePVControl("Side softIOC Yaw", "BL1607-6-I22:Yaw:deg", this, 0.001);
	softIOCYawFeedbackControl_ = new AMSinglePVControl("Side softIOC Yaw Feedback", "BL1607-6-I22:Yaw:deg:fbk", this, 0.001);
	softIOCYawStatusControl_ = new AMSinglePVControl("Side softIOC Yaw Status", "BL1607-6-I22:Yaw:status", this, 0.5);
	softIOCYawStopControl_ = new AMSinglePVControl("Side softIOC Yaw Stop", "BL1607-6-I22:Yaw:stop", this, 0.5);
}

void BioXASSideTableCoordinator::initializePVControlSet()
{
	allControls_ = new AMControlSet(this);

	// ============ Physical PVs ==================
	allControls_->addControl(verticalUpstreamControl_);
	allControls_->addControl(verticalUpstreamFeedbackControl_);
	allControls_->addControl(verticalUpstreamStatusControl_);
	allControls_->addControl(verticalUpstreamStopControl_);
	allControls_->addControl(verticalDownstreamControl_);
	allControls_->addControl(verticalDownstreamFeedbackControl_);
	allControls_->addControl(verticalDownstreamStatusControl_);
	allControls_->addControl(verticalDownstreamStopControl_);
	allControls_->addControl(horizontalUpstreamControl_);
	allControls_->addControl(horizontalUpstreamFeedbackControl_);
	allControls_->addControl(horizontalUpstreamStatusControl_);
	allControls_->addControl(horizontalUpstreamStopControl_);
	allControls_->addControl(horizontalDownstreamControl_);
	allControls_->addControl(horizontalDownstreamFeedbackControl_);
	allControls_->addControl(horizontalDownstreamStatusControl_);
	allControls_->addControl(horizontalDownstreamStopControl_);


	// ============ SoftIOC PVs ==================
	allControls_->addControl(softIOCHeightControl_);
	allControls_->addControl(softIOCHeightFeedbackControl_);
	allControls_->addControl(softIOCHeightStatusControl_);
	allControls_->addControl(softIOCHeightStopControl_);
	allControls_->addControl(softIOCPitchControl_);
	allControls_->addControl(softIOCPitchFeedbackControl_);
	allControls_->addControl(softIOCPitchStatusControl_);
	allControls_->addControl(softIOCPitchStopControl_);
	allControls_->addControl(softIOCLateralControl_);
	allControls_->addControl(softIOCLateralFeedbackControl_);
	allControls_->addControl(softIOCLateralStatusControl_);
	allControls_->addControl(softIOCLateralStopControl_);
	allControls_->addControl(softIOCYawControl_);
	allControls_->addControl(softIOCYawFeedbackControl_);
	allControls_->addControl(softIOCYawStatusControl_);
	allControls_->addControl(softIOCYawStopControl_);
}

void BioXASSideTableCoordinator::initializeSignalConnector()
{
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));

	// ============ Physical PVs ==================
	connect(verticalUpstreamFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalFeedbackControlValueChanged()));
	connect(verticalUpstreamStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalStatusControlValueChanged()));

	connect(verticalDownstreamFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalFeedbackControlValueChanged()));
	connect(verticalDownstreamStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalStatusControlValueChanged()));

	connect(horizontalUpstreamFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalFeedbackControlValueChanged()));
	connect(horizontalUpstreamStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalStatusControlValueChanged()));

	connect(horizontalDownstreamFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalFeedbackControlValueChanged()));
	connect(horizontalDownstreamStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalStatusControlValueChanged()));

	// ============ SoftIOC PVs ==================
	connect(softIOCHeightControl_, SIGNAL(valueChanged(double)), this, SLOT(onHeightControlValueChanged(double)));
	connect(softIOCHeightStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onHeightStopControlValueChanged()));
	connect(softIOCPitchControl_, SIGNAL(valueChanged(double)), this, SLOT(onPitchControlValueChanged(double)));
	connect(softIOCPitchStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onPitchStopControlValueChanged()));
	connect(softIOCLateralControl_, SIGNAL(valueChanged(double)), this, SLOT(onLateralControlValueChanged(double)));
	connect(softIOCLateralStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onLateralStopControlValueChanged()));
	connect(softIOCYawControl_, SIGNAL(valueChanged(double)), this, SLOT(onYawControlValueChanged(double)));
	connect(softIOCYawStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onYawStopControlValueChanged()));
}

double BioXASSideTableCoordinator::calculateTableHeight(const double upstreamHeight, const double downstreamHeight) const
{
	// The vertical height is the height of the center of the table, which is the average of upstream value and the downstream value
	return (upstreamHeight + downstreamHeight) / 2;
}

double BioXASSideTableCoordinator::calculateTablePitch(const double upstreamHeight, const double downstreamHeight) const
{
	// The pitch is the degree of the arcTangent value of (dH - uH) / distance
	return radianToDegree(atan((downstreamHeight - upstreamHeight) / (tableVerticalMotorPosition_ * 2)));
}

double BioXASSideTableCoordinator::calculateTableLateral(const double upstreamLateral, const double downstreamLateral) const
{
	// The lateral is the lateral of the center of the table, which is the average of upstream value and the downstream value
	return ( upstreamLateral + downstreamLateral ) / 2;
}

double BioXASSideTableCoordinator::calculateTableYaw(const double upstreamLateral, const double downstreamLateral) const
{
	// The yaw is the degree of the arcTangent value of (dH - uH) / distance
	return radianToDegree(atan((downstreamLateral - upstreamLateral) / (tableHorizontalMotorPosition_ * 2)));
}

void BioXASSideTableCoordinator::manipulateVerticalPVChange(double upstreamHeight, double downstreamHeight, AMControl* heightPV, AMControl *pitchPV)
{
	double height = calculateTableHeight(upstreamHeight, downstreamHeight);
	double pitch =  calculateTablePitch(upstreamHeight, downstreamHeight);

	if (!heightPV->withinTolerance(height)) {
		heightPV->move(height);
	}

	if (!pitchPV->withinTolerance(pitch)) {
		pitchPV->move(pitch);
	}
}

void BioXASSideTableCoordinator::manipulateHorizontalPVChange(double upstreamOffset, double downstreamOffset, AMControl* lateralPV, AMControl* yawPV)
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
