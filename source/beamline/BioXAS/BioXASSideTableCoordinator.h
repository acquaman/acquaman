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


#ifndef BioXASSideTableCoordinator_H
#define BioXASSideTableCoordinator_H

#include "beamline/AMControlSet.h"

///
/// NOTE:
/// The position of Height / lateral / pitch / yaw is based on the position of the center of the table
///

class BioXASSideTableCoordinator : public QObject
{
	Q_OBJECT
public:
	/// Simple constructor for class
	BioXASSideTableCoordinator(QObject *parent = 0);

	/// Destructor
	virtual ~BioXASSideTableCoordinator();

protected slots:
	/// Handles watching for when the controls actually connect
	void onAllControlsConnected(bool connected);

	/// ===== Handles changes coming from the MaxVMotor controls ====
	/// Handles Vertical Upstream/Downstream feedback control value changed
	void onVerticalFeedbackControlValueChanged();
	/// Handles Vertical Upstream/Downstream status control value changed
	void onVerticalStatusControlValueChanged();


	/// Handles Horizontal Upstream/Downstrea feedback control value changed
	void onHorizontalFeedbackControlValueChanged();
	/// Handles Horizontal Upstream/Downstrea status control value changed
	void onHorizontalStatusControlValueChanged();

	/// ==== Handles changes coming from the softIOC conrols ====
	/// Handles table Height control value changed
	void onHeightControlValueChanged(double value);
	/// Handles table Height stop control value changed
	void onHeightStopControlValueChanged();
	/// Handles table Pitch control value changed
	void onPitchControlValueChanged(double angle);
	/// Handles table Pitch stop control value changed
	void onPitchStopControlValueChanged();
	/// Handles table Lateral control value changed
	void onLateralControlValueChanged(double value);
	/// Handles table Lateral stop control value changed
	void onLateralStopControlValueChanged();
	/// Handles table Yaw control value changed
	void onYawControlValueChanged(double angle);
	/// Handles table yaw stop control value changed
	void onYawStopControlValueChanged();


protected:
	double radianToDegree(double radian) const { return radian * 57.2957795; }
	double degreeToRadian(double degree) const { return degree * 0.0174532925; }

	/// to initialize the phycisal Motor controls
	void initializePhysicalMotorControls();
	/// to initialize the softIOC Motor controls
	void initializeSoftIOCMotorControls();
	/// to initialize the PV control set
	void initializePVControlSet();
	/// to initialize the signal slot connectors
	void initializeSignalConnector();

	/// get the current height of the tablle
	double calculateTableHeight(const double upstreamHeight, const double downstreamHeight) const;
	/// get the current pitch of the tablle
	double calculateTablePitch(const double upstreamHeight, const double downstreamHeight) const;
	/// get the current lateral of the tablle
	double calculateTableLateral(const double upstreamLateral, const double downstreamLateral) const;
	/// get the current yaw of the tablle
	double calculateTableYaw(const double upstreamLateral, const double downstreamLateral) const;

	/// Apply the value changes of the vertical (feedback) PVs to the softIOC height (feedback) PV and Pitch (feedback) PV
	void manipulateVerticalPVChange(double upstreamHeight, double downstreamHeight, AMControl* heightPV, AMControl *pitchPV);
	/// Apply the value changes of the horizontal (feedback) PVs to the softIOC lateral (feedback) PV and yaw (feedback) PV
	void manipulateHorizontalPVChange(double upstreamOffset, double downstreamOffset, AMControl* lateralPV, AMControl* yawPV);

protected:
	/// const for motors
	double tableVerticalMotorPosition_; // the distance between the upstream / downstream motor to the center of the table, which is fixed
	double tableHorizontalMotorPosition_; // the distance between the upstream / downstream motor to the center of the table, which is fixed

	/// flag of whether the coordinator is initialized or not
	bool initialized_;
	/// Holds whether or not we've connected at least once
	bool connectedOnce_;
	/// All the controls (for checking connectivity)
	AMControlSet *allControls_;

	/// ============ Original PVs ==================
	/// BioXAS Side table Vertical Upstream PV control
	AMControl *verticalUpstreamControl_;
	/// BioXAS Side table Vertical Upstream feedbak PV control
	AMControl *verticalUpstreamFeedbackControl_;
	/// BioXAS Side table Vertical Upstream status PV control
	AMControl *verticalUpstreamStatusControl_;
	AMControl *verticalUpstreamStopControl_;

	/// BioXAS Side table Vertical Downstream PV control
	AMControl *verticalDownstreamControl_;
	/// BioXAS Side table Vertical Downstream feedbak PV control
	AMControl *verticalDownstreamFeedbackControl_;
	/// BioXAS Side table Vertical Downstream status PV control
	AMControl *verticalDownstreamStatusControl_;
	AMControl *verticalDownstreamStopControl_;

	/// BioXAS Side table Horizontal Upstream PV control
	AMControl *horizontalUpstreamControl_;
	/// BioXAS Side table Horizontal Upstream feedbak PV control
	AMControl *horizontalUpstreamFeedbackControl_;
	/// BioXAS Side table Horizontal Upstream status PV control
	AMControl *horizontalUpstreamStatusControl_;
	AMControl *horizontalUpstreamStopControl_;

	/// BioXAS Side table horizonal Downstream PV control
	AMControl *horizontalDownstreamControl_;
	/// BioXAS Side table Horizontal Downstream feedbak PV control
	AMControl *horizontalDownstreamFeedbackControl_;
	/// BioXAS Side table Horizontal Downstream status PV control
	AMControl *horizontalDownstreamStatusControl_;
	AMControl *horizontalDownstreamStopControl_;

	/// ============ SoftIOC PVs ==================
	/// BioXAS Side table softIOC height PV control
	AMControl *softIOCHeightControl_;
	/// BioXAS Side table softIOC height feedbak PV control
	AMControl *softIOCHeightFeedbackControl_;
	/// BioXAS Side table softIOC height status PV control
	AMControl *softIOCHeightStatusControl_;
	/// BioXAS Side table softIOC height stop PV control
	AMControl *softIOCHeightStopControl_;

	/// BioXAS Side table softIOC pitch PV control
	AMControl *softIOCPitchControl_;
	/// BioXAS Side table softIOC pitch feedbak PV control
	AMControl *softIOCPitchFeedbackControl_;
	/// BioXAS Side table softIOC pitch status PV control
	AMControl *softIOCPitchStatusControl_;
	/// BioXAS Side table softIOC pitch stop PV control
	AMControl *softIOCPitchStopControl_;

	/// BioXAS Side table softIOC Lateral PV control
	AMControl *softIOCLateralControl_;
	/// BioXAS Side table softIOC Lateral feedbak PV control
	AMControl *softIOCLateralFeedbackControl_;
	/// BioXAS Side table softIOC Lateral status PV control
	AMControl *softIOCLateralStatusControl_;
	/// BioXAS Side table softIOC Lateral stop PV control
	AMControl *softIOCLateralStopControl_;

	/// BioXAS Side table softIOC yaw PV control
	AMControl *softIOCYawControl_;
	/// BioXAS Side table softIOC yaw feedbak PV control
	AMControl *softIOCYawFeedbackControl_;
	/// BioXAS Side table softIOC yaw status PV control
	AMControl *softIOCYawStatusControl_;
	/// BioXAS Side table softIOC yaw stop PV control
	AMControl *softIOCYawStopControl_;

};

#endif // BioXASSideTableCoordinator_H
