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

	connectedOnce_ = false;

	// ============ Original PVs ==================
	verticalUpstreamControl_ = new AMSinglePVControl("Side Vertical Upstream", "SMTR1607-6-I22-02:mm", this, 0.01);
	verticalUpstreamFeedbackControl_ = new AMReadOnlyPVControl("Side Vertical Upstream Feedback", "SMTR1607-6-I22-02:mm:fbk", this);
	verticalUpstreamStatusControl_ = new AMReadOnlyPVControl("Side Vertical Upstream Status", "SMTR1607-6-I22-02:status", this);

	verticalDownstreamControl_ = new AMSinglePVControl("Side Vertical Downstream", "SMTR1607-6-I22-03:mm", this, 0.01);
	verticalDownstreamFeedbackControl_ = new AMReadOnlyPVControl("Side Vertical Downstream Feedback", "SMTR1607-6-I22-03:mm:fbk", this);
	verticalDownstreamStatusControl_ = new AMReadOnlyPVControl("Side Vertical Downstream Status", "SMTR1607-6-I22-03:status", this);


	horizontalUpstreamControl_ = new AMSinglePVControl("Side Horizonal Upstream", "SMTR1607-6-I22-04:mm", this, 0.01);
	horizontalUpstreamFeedbackControl_ = new AMReadOnlyPVControl("Side Horizonal Upstream Feedback", "SMTR1607-6-I22-04:mm:fbk", this);
	horizontalUpstreamStatusControl_ = new AMReadOnlyPVControl("Side Horizonal Upstream Status", "SMTR1607-6-I22-04:status", this);

	horizontalDownstreamControl_ = new AMSinglePVControl("Side Horizonal Downstream", "SMTR1607-6-I22-05:mm", this, 0.01);
	horizontalDownstreamFeedbackControl_ = new AMReadOnlyPVControl("Side Horizonal Downstream Feedback", "SMTR1607-6-I22-05:mm:fbk", this);
	horizontalDownstreamStatusControl_ = new AMReadOnlyPVControl("Side Horizonal Downstream Status", "SMTR1607-6-I22-05:status", this);

	// ============ SoftIOC PVs ==================
	softIOCHeightControl_ = new AMSinglePVControl("Side softIOC Height", "BL1607-I22:Height:mm", this, 0.01);
	softIOCHeightFeedbackControl_ = new AMReadOnlyPVControl("Side softIOC Height Feedback", "BL1607-I22:Height:mm:fbk", this);
	softIOCHeightStatusControl_ = new AMReadOnlyPVControl("Side softIOC Height Status", "BL1607-I22:Height:status", this);
	softIOCHeightStopControl_ = new AMReadOnlyPVControl("Side softIOC Height Stop", "BL1607-I22:Height:stop", this);

	softIOCPitchControl_ = new AMSinglePVControl("Side softIOC Pitch", "BL1607-I22:Pitch:deg", this, 0.01);
	softIOCPitchFeedbackControl_ = new AMReadOnlyPVControl("Side softIOC Pitch Feedback", "BL1607-I22:Pitch:deg:fbk", this);
	softIOCPitchStatusControl_ = new AMReadOnlyPVControl("Side softIOC Pitch Status", "BL1607-I22:Pitch:status", this);
	softIOCPitchStopControl_ = new AMReadOnlyPVControl("Side softIOC Pitch Stop", "BL1607-I22:Pitch:stop", this);

	softIOCLateralControl_ = new AMSinglePVControl("Side softIOC Lateral", "BL1607-I22:Lateral:mm", this, 0.01);
	softIOCLateralFeedbackControl_ = new AMReadOnlyPVControl("Side softIOC Lateral Feedback", "BL1607-I22:Lateral:mm:fbk", this);
	softIOCLateralStatusControl_ = new AMReadOnlyPVControl("Side softIOC Lateral Status", "BL1607-I22:Lateral:status", this);
	softIOCLateralStopControl_ = new AMReadOnlyPVControl("Side softIOC Lateral Stop", "BL1607-I22:Lateral:stop", this);

	softIOCYawControl_ = new AMSinglePVControl("Side softIOC Yaw", "BL1607-I22:Yaw:deg", this, 0.01);
	softIOCYawFeedbackControl_ = new AMReadOnlyPVControl("Side softIOC Yaw Feedback", "BL1607-I22:Yaw:deg:fbk", this);
	softIOCYawStatusControl_ = new AMReadOnlyPVControl("Side softIOC Yaw Status", "BL1607-I22:Yaw:status", this);
	softIOCYawStopControl_ = new AMReadOnlyPVControl("Side softIOC Yaw Stop", "BL1607-I22:Yaw:stop", this);


	allControls_ = new AMControlSet(this);
	allControls_->addControl(verticalUpstreamControl_);
	allControls_->addControl(verticalUpstreamFeedbackControl_);
	allControls_->addControl(verticalUpstreamStatusControl_);
	allControls_->addControl(verticalDownstreamControl_);
	allControls_->addControl(verticalDownstreamFeedbackControl_);
	allControls_->addControl(verticalDownstreamStatusControl_);
	allControls_->addControl(horizontalUpstreamControl_);
	allControls_->addControl(horizontalUpstreamFeedbackControl_);
	allControls_->addControl(horizontalUpstreamStatusControl_);
	allControls_->addControl(horizontalDownstreamControl_);
	allControls_->addControl(horizontalDownstreamFeedbackControl_);
	allControls_->addControl(horizontalDownstreamStatusControl_);
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

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));

	connect(verticalUpstreamControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalControlValueChanged(double)));
	connect(verticalUpstreamFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalFeedbackControlValueChanged(double)));
	connect(verticalUpstreamStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalStatusControlValueChanged(double)));

	connect(verticalDownstreamControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalControlValueChanged(double)));
	connect(verticalDownstreamFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalFeedbackControlValueChanged(double)));
	connect(verticalDownstreamStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onVerticalStatusControlValueChanged(double)));

	connect(horizontalUpstreamControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalControlValueChanged(double)));
	connect(horizontalUpstreamFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalFeedbackControlValueChanged(double)));
	connect(horizontalUpstreamStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalStatusControlValueChanged(double)));

	connect(horizontalDownstreamControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalControlValueChanged(double)));
	connect(horizontalDownstreamFeedbackControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalFeedbackControlValueChanged(double)));
	connect(horizontalDownstreamStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onHorizontalStatusControlValueChanged(double)));

	connect(softIOCHeightControl_, SIGNAL(valueChanged(double)), this, SLOT(onHeightControlValueChanged(double)));
	connect(softIOCHeightStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onHeightControlStopChanged(double)));
	connect(softIOCPitchControl_, SIGNAL(valueChanged(double)), this, SLOT(onPitchControlValueChanged(double)));
	connect(softIOCPitchStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onPitchStopControlValueChanged(double)));
	connect(softIOCLateralControl_, SIGNAL(valueChanged(double)), this, SLOT(onLateralControlValueChanged(double)));
	connect(softIOCLateralStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onLateralStopControlValueChanged(double)));
	connect(softIOCYawControl_, SIGNAL(valueChanged(double)), this, SLOT(onYawControlValueChanged(double)));
	connect(softIOCYawStopControl_, SIGNAL(valueChanged(double)), this, SLOT(onYawStopControlValueChanged(double)));
}

BioXASSideTableCoordinator::~BioXASSideTableCoordinator(){}

void BioXASSideTableCoordinator::onAllControlsConnected(bool connected){
	if(connected){
		connectedOnce_ = true;

		qDebug() << "Checking start up value from the OLD vertical upstream as " << verticalUpstreamControl_->value();
		qDebug() << "Checking start up value from the OLD vertical DownStream as " << verticalDownstreamControl_->value();
		onVerticalControlValueChanged(verticalUpstreamControl_->value());
		qDebug() << "Checking start up value from the OLD vertical upstream feedback as " << verticalUpstreamFeedbackControl_->value();
		qDebug() << "Checking start up value from the OLD vertical DownStream feedback as " << verticalDownstreamFeedbackControl_->value();
		onVerticalFeedbackControlValueChanged(verticalUpstreamFeedbackControl_->value());
		qDebug() << "Checking start up value from the OLD vertical upstream status as " << verticalUpstreamStatusControl_->value();
		qDebug() << "Checking start up value from the OLD vertical DownStream status as " << verticalDownstreamStatusControl_->value();
		onVerticalStatusControlValueChanged(verticalUpstreamStatusControl_->value());

		qDebug() << "Checking start up value from the OLD Horizontal upstream as " << horizontalUpstreamControl_->value();
		qDebug() << "Checking start up value from the OLD Horizontal DownStream as " << horizontalDownstreamControl_->value();
		onHorizontalControlValueChanged(horizontalUpstreamControl_->value());
		qDebug() << "Checking start up value from the OLD Horizontal upstream feedback as " << horizontalUpstreamFeedbackControl_->value();
		qDebug() << "Checking start up value from the OLD Horizontal DownStream feedback as " << horizontalDownstreamFeedbackControl_->value();
		onHorizontalFeedbackControlValueChanged(horizontalUpstreamFeedbackControl_->value());
		qDebug() << "Checking start up value from the OLD Horizontal upstream status as " << horizontalUpstreamStatusControl_->value();
		qDebug() << "Checking start up value from the OLD Horizontal DownStream status as " << horizontalDownstreamStatusControl_->value();
		onHorizontalStatusControlValueChanged(horizontalUpstreamStatusControl_->value());


		// Max / Min value?
//		qDebug() << "Checking start up range for OLD energy as " << oldEnergy_->minimumValue() << oldEnergy_->maximumValue();
//		addOnsEnergyDRVH_->move(oldEnergy_->maximumValue());
//		addOnsEnergyDRVL_->move(oldEnergy_->minimumValue());

//		qDebug() << "Checking start up range for OLD sample X as " << sxrmbMicroprobeSampleStageX_->minimumValue() << sxrmbMicroprobeSampleStageX_->maximumValue();
//		addOnsMicroprobeSampleStageDRVHX_->move(sxrmbMicroprobeSampleStageX_->maxi156mumValue());
//		addOnsMicroprobeSampleStageDRVLX_->move(sxrmbMicroprobeSampleStageX_->minimumValue());

//		qDebug() << "Checking start up range for OLD sample Y as " << sxrmbMicroprobeSampleStageY_->minimumValue() << sxrmbMicroprobeSampleStageY_->maximumValue();
//		addOnsMicroprobeSampleStageDRVHY_->move(sxrmbMicroprobeSampleStageY_->maximumValue());
//		addOnsMicroprobeSampleStageDRVLY_->move(sxrmbMicroprobeSampleStageY_->minimumValue());

//		qDebug() << "Checking start up range for OLD sample Z as " << sxrmbMicroprobeSampleStageZ_->minimumValue() << sxrmbMicroprobeSampleStageZ_->maximumValue();
//		addOnsMicroprobeSampleStageDRVHZ_->move(sxrmbMicroprobeSampleStageZ_->maximumValue());
//		addOnsMicroprobeSampleStageDRVLZ_->move(sxrmbMicroprobeSampleStageZ_->minimumValue());
	}
}

void BioXASSideTableCoordinator::onVerticalControlValueChanged(double value)
{
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	double upstreamHeight = verticalUpstreamControl_->value();
	double downstreamHeight = verticalDownstreamControl_->value();
	manipulateVerticalPVChange(upstreamHeight, downstreamHeight, softIOCHeightControl_, softIOCPitchControl_);
}

void BioXASSideTableCoordinator::onVerticalFeedbackControlValueChanged(double value)
{
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	double upstreamHeight = verticalUpstreamFeedbackControl_->value();
	double downstreamHeight = verticalDownstreamFeedbackControl_->value();
	manipulateVerticalPVChange(upstreamHeight, downstreamHeight, softIOCHeightFeedbackControl_, softIOCPitchFeedbackControl_);
}

void BioXASSideTableCoordinator::onVerticalStatusControlValueChanged(double value)
{
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	// Status value: 0 - "MOVE DONE";  1 - "MOVE ACTIVE"; 2 - "AT LIMIT" ("MINOR"); 3 - "FORCED STOP" ("MINOR"); 4 - "ERROR" ("MAJOR")
	// Status is "MOVE ACTIVE" if at least one motor is moving or the largest status value
	double status = 0;
	if (verticalUpstreamStatusControl_->withinTolerance(1) || verticalDownstreamStatusControl_->withinTolerance(1)) {
		status = 1;
	} else {
		double verticalUpstreamControlStatus = verticalUpstreamStatusControl_->value();
		double verticalDownstreamControlStatus = verticalDownstreamStatusControl_->value();
		status = verticalUpstreamControlStatus > verticalDownstreamControlStatus ? verticalUpstreamControlStatus : verticalDownstreamControlStatus;
	}

	if (!softIOCHeightStatusControl_->withinTolerance(status)) {
		softIOCHeightStatusControl_->move(status);
	}
	if (!softIOCPitchStatusControl_->withinTolerance(status)) {
		softIOCPitchStatusControl_->move(status);
	}
}


void BioXASSideTableCoordinator::onHorizontalControlValueChanged(double value)
{
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	double horizontalUpstreamValue = horizontalUpstreamControl_->value();
	double horizontalDownstreamValue = horizontalDownstreamControl_->value();
	manipulateHorizontalPVChange(horizontalUpstreamValue, horizontalDownstreamValue, softIOCLateralControl_, softIOCYawControl_);
}

void BioXASSideTableCoordinator::onHorizontalFeedbackControlValueChanged(double value)
{
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	double horizontalUpstreamFeedbackValue = horizontalUpstreamFeedbackControl_->value();
	double horizontalDownstreamFeedbackValue = horizontalDownstreamFeedbackControl_->value();
	manipulateHorizontalPVChange(horizontalUpstreamFeedbackValue, horizontalDownstreamFeedbackValue, softIOCLateralFeedbackControl_, softIOCYawFeedbackControl_);
}

void BioXASSideTableCoordinator::onHorizontalStatusControlValueChanged(double value)
{
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	// Status value: 0 - "MOVE DONE";  1 - "MOVE ACTIVE"; 2 - "AT LIMIT" ("MINOR"); 3 - "FORCED STOP" ("MINOR"); 4 - "ERROR" ("MAJOR")
	// Status is "MOVE ACTIVE" if at least one motor is moving or the largest status value
	double status = 0;
	if (horizontalUpstreamStatusControl_->withinTolerance(1) || horizontalDownstreamStatusControl_->withinTolerance(1)) {
		status = 1;
	} else {
		double horizontalUpstreamControlStatus = horizontalUpstreamStatusControl_->value();
		double horizontalDownstreamControlStatus = horizontalDownstreamStatusControl_->value();
		status = horizontalUpstreamControlStatus > horizontalDownstreamControlStatus ? horizontalUpstreamControlStatus : horizontalDownstreamControlStatus;
	}

	if (!softIOCYawStatusControl_->withinTolerance(status)) {
		softIOCYawStatusControl_->move(status);
	}
}


void BioXASSideTableCoordinator::onHeightControlValueChanged(double value)
{
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	double delta = value - softIOCHeightFeedbackControl_->value();

	double targetUpstreamHeight = verticalUpstreamControl_->value() + delta;
	double targetDownstreamHeight = verticalDownstreamControl_->value() + delta;

	if (!verticalUpstreamControl_->withinTolerance(targetUpstreamHeight)) {
		verticalUpstreamControl_->move(targetUpstreamHeight);
	}
	if (!verticalDownstreamControl_->withinTolerance(targetDownstreamHeight)) {
		verticalDownstreamControl_->move(targetDownstreamHeight);
	}
}

void BioXASSideTableCoordinator::onHeightStopControlValueChanged(double value)
{
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	if (softIOCHeightStopControl_->withinTolerance(1)) {
		verticalUpstreamControl_->stop();
		verticalDownstreamControl_->stop();

		softIOCHeightStopControl_->move(0); // restore the value to 0
	}
}

void BioXASSideTableCoordinator::onPitchControlValueChanged(double angle)
{
	Q_UNUSED(angle)
	if(!connectedOnce_)
		return;

	double delta = tableVerticalMotorPosition_ * tan(degreeToRadian(angle));

	// downstream control will move in the direction of the angle and upstream will move against the direction of the angle
	double targetUpstreamHeight = softIOCHeightControl_->value() - delta ;
	double targetDownstreamHeight = softIOCHeightControl_->value() + delta;

	if (!verticalUpstreamControl_->withinTolerance(targetUpstreamHeight)) {
		verticalUpstreamControl_->move(targetUpstreamHeight);
	}
	if (!verticalDownstreamControl_->withinTolerance(targetDownstreamHeight)) {
		verticalDownstreamControl_->move(targetDownstreamHeight);
	}
}

void BioXASSideTableCoordinator::onPitchStopControlValueChanged(double value)
{
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	if (softIOCPitchStopControl_->withinTolerance(1)) {
		verticalUpstreamControl_->stop();
		verticalDownstreamControl_->stop();

		softIOCPitchStopControl_->move(0); // restore the value to 0
	}
}

void BioXASSideTableCoordinator::onLateralControlValueChanged(double value)
{
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	double delta = value - softIOCLateralFeedbackControl_->value();

	double horizontalUpstreamPosition = horizontalUpstreamControl_->value() - delta;
	double horizontalDownstreamPosition = horizontalDownstreamControl_->value() + delta;

	if (!horizontalUpstreamControl_->withinTolerance(horizontalUpstreamPosition)) {
		horizontalUpstreamControl_->move(horizontalUpstreamPosition);
	}
	if (!horizontalDownstreamControl_->withinTolerance(horizontalDownstreamPosition)) {
		horizontalDownstreamControl_->move(horizontalDownstreamPosition);
	}
}

void BioXASSideTableCoordinator::onLateralStopControlValueChanged(double value)
{
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	if (softIOCLateralStopControl_->withinTolerance(1)) {
		horizontalUpstreamControl_->stop();
		horizontalDownstreamControl_->stop();

		softIOCLateralStopControl_->move(0); // restore the value to 0
	}
}

void BioXASSideTableCoordinator::onYawControlValueChanged(double angle)
{
	Q_UNUSED(angle)
	if(!connectedOnce_)
		return;

	double delta = tableHorizontalMotorPosition_ * tan(degreeToRadian(angle));

	if (!horizontalUpstreamControl_->withinTolerance(-delta)) {
		horizontalUpstreamControl_->move(-delta);
	}
	if (!horizontalDownstreamControl_->withinTolerance(delta)) {
		horizontalDownstreamControl_->move(delta);
	}
}

void BioXASSideTableCoordinator::onYawStopControlValueChanged(double value)
{
	Q_UNUSED(value)
	if(!connectedOnce_)
		return;

	if (softIOCYawStopControl_->withinTolerance(1)) {
		horizontalUpstreamControl_->stop();
		horizontalDownstreamControl_->stop();

		softIOCYawStopControl_->move(0); // restore the value to 0
	}
}

void BioXASSideTableCoordinator::manipulateVerticalPVChange(double upstreamHeight, double downstreamHeight, AMControl* heightPV, AMControl *pitchPV)
{
	// The vertical height is the height of the center of the table, which is the average of upstream value and the downstream value
	double height = (upstreamHeight + downstreamHeight) / 2;
	// The pitch is the degree of the arcTangent value of (dH - uH) / distance
	double pitch =  radianToDegree(atan((downstreamHeight - upstreamHeight) / (tableVerticalMotorPosition_ * 2)));

	if (!heightPV->withinTolerance(height)) {
		heightPV->move(height);
	}
	if (!pitchPV->withinTolerance(pitch)) {
		pitchPV->move(pitch);
	}
}

void BioXASSideTableCoordinator::manipulateHorizontalPVChange(double upstreamOffset, double downstreamOffset, AMControl* lateralPV, AMControl* yawPV)
{
	double lateral = ( downstreamOffset + upstreamOffset ) / 2;
	double yaw = radianToDegree(atan((downstreamOffset - upstreamOffset) / (tableHorizontalMotorPosition_ * 2)));

	if ( !lateralPV->withinTolerance(lateral) ) {
		lateralPV->move(lateral);
	}
	if ( !yawPV->withinTolerance(yaw) ) {
		yawPV->move(yaw);
	}
}
