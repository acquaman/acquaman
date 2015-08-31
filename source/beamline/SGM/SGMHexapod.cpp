#include "SGMHexapod.h"
#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/SGM/SGMMAXvMotor.h"
#include "beamline/SGM/SGMHexapodTransformedAxis.h"
SGMHexapod::SGMHexapod(QObject *parent) :
    QObject(parent)
{
	AMControl* globalXAxisSetpoint = new AMSinglePVControl("Hexapod Global X Axis", "HXPD1611-4-I10-01:trajectory:X:mm", this, 0.01, 2.0);
	AMControl* globalYAxisSetpoint = new AMSinglePVControl("Hexapod Global Y Axis", "HXPD1611-4-I10-01:trajectory:Y:mm", this, 0.01, 2.0);
	AMControl* globalZAxisSetpoint = new AMSinglePVControl("Hexapod Global Z Axis", "HXPD1611-4-I10-01:trajectory:Z:mm", this, 0.01, 2.0);

	AMControl* globalXAxisFeedback = new AMReadOnlyPVwStatusControl("Hexapod Global X Axis Feedback", "HXPD1611-4-I10-01:X:mm:fbk", "HXPD1611-4-I10-01:X:status",this, new CLSMAXvControlStatusChecker());
	AMControl* globalYAxisFeedback = new AMReadOnlyPVwStatusControl("Hexapod Global Y Axis Feedback", "HXPD1611-4-I10-01:Y:mm:fbk", "HXPD1611-4-I10-01:Y:status",this, new CLSMAXvControlStatusChecker());
	AMControl* globalZAxisFeedback = new AMReadOnlyPVwStatusControl("Hexapod Global Z Axis Feedback", "HXPD1611-4-I10-01:Z:mm:fbk", "HXPD1611-4-I10-01:Z:status",this, new CLSMAXvControlStatusChecker());



	trajectoryStart_ = new AMSinglePVControl("Hexapod Trajectory Move Start", "HXPD1611-4-I10-01:trajectory:start", this, 0.5);
	trajectoryReset_ = new AMSinglePVControl("Hexapod Trajectory Reset", "HXPD1611-4-I10-01:trajectory:reset", this, 0.5);

	systemVelocity_ = new AMPVControl("Hexapod Velocity", "HXPD1611-4-I10-01:velocity:fbk", "HXPD1611-4-I10-01:velocity",QString(), this, 0.001, 2.0);
	stopAll_ = new AMSinglePVControl("Hexapod Stop", "HXPD1611-4-I10-01:stop", this, 0.5);

	xAxisPrimeControl_ = new SGMHexapodTransformedAxis(AM3DCoordinatedSystemControl::XAxis,
													   globalXAxisSetpoint,
													   globalXAxisFeedback,
													   globalYAxisSetpoint,
													   globalYAxisFeedback,
													   globalZAxisSetpoint,
													   globalZAxisFeedback,
													   trajectoryStart_,
													   "Hexapod X Axis Prime",
													   "mm",
													   this);

	yAxisPrimeControl_ = new SGMHexapodTransformedAxis(AM3DCoordinatedSystemControl::YAxis,
													   globalXAxisSetpoint,
													   globalXAxisFeedback,
													   globalYAxisSetpoint,
													   globalYAxisFeedback,
													   globalZAxisSetpoint,
													   globalZAxisFeedback,
													   trajectoryStart_,
													   "Hexapod Y Axis Prime",
													   "mm",
													   this);

	zAxisPrimeControl_ = new SGMHexapodTransformedAxis(AM3DCoordinatedSystemControl::ZAxis,
													   globalXAxisSetpoint,
													   globalXAxisFeedback,
													   globalYAxisSetpoint,
													   globalYAxisFeedback,
													   globalZAxisSetpoint,
													   globalZAxisFeedback,
													   trajectoryStart_,
													   "Hexapod Z Axis Prime",
													   "mm",
													   this);

	xAxisPrimeTrajectoryControl_ = new AM3DRotatedSystemControl(AM3DCoordinatedSystemControl::XAxis,
																		   globalXAxisSetpoint,
																		   globalYAxisSetpoint,
																		   globalZAxisSetpoint,
																		   "Hexapod Trajectory X Axis Prime",
																		   "mm",
																		   this);

	yAxisPrimeTrajectoryControl_ = new AM3DRotatedSystemControl(AM3DCoordinatedSystemControl::YAxis,
																		   globalXAxisSetpoint,
																		   globalYAxisSetpoint,
																		   globalZAxisSetpoint,
																		   "Hexapod Trajectory Y Axis Prime",
																		   "mm",
																		   this);

	zAxisPrimeTrajectoryControl_ = new AM3DRotatedSystemControl(AM3DCoordinatedSystemControl::ZAxis,
																		   globalXAxisSetpoint,
																		   globalYAxisSetpoint,
																		   globalZAxisSetpoint,
																		   "Hexapod Trajectory Z Axis Prime",
																		   "mm",
																		   this);



	//Test rotations:
	rotateSystem(75, 79.4547, 45);


	allControls_ = new AMControlSet(this);
	allControls_->addControl(xAxisPrimeControl_);
	allControls_->addControl(yAxisPrimeControl_);
	allControls_->addControl(zAxisPrimeControl_);

	allControls_->addControl(systemVelocity_);
	allControls_->addControl(stopAll_);

	allControls_->addControl(xAxisPrimeTrajectoryControl_);
	allControls_->addControl(yAxisPrimeTrajectoryControl_);
	allControls_->addControl(zAxisPrimeTrajectoryControl_);

	allControls_->addControl(trajectoryStart_);
	allControls_->addControl(trajectoryReset_);

	connect(allControls_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
}

bool SGMHexapod::isConnected()
{
	return allControls_->isConnected();
}


AMControl * SGMHexapod::xAxisPrimeControl() const
{
	return xAxisPrimeControl_;
}

AMControl * SGMHexapod::yAxisPrimeControl() const
{
	return yAxisPrimeControl_;
}

AMControl * SGMHexapod::zAxisPrimeControl() const
{
	return zAxisPrimeControl_;
}

AMControl * SGMHexapod::xAxisPrimeTrajectoryControl() const
{
	return xAxisPrimeTrajectoryControl_;
}

AMControl * SGMHexapod::yAxisPrimeTrajectoryControl() const
{
	return yAxisPrimeTrajectoryControl_;
}

AMControl * SGMHexapod::zAxisPrimeTrajectoryControl() const
{
	return zAxisPrimeTrajectoryControl_;
}


AMControl * SGMHexapod::systemVelocity() const
{
	return systemVelocity_;
}

AMControl * SGMHexapod::stopAll() const
{
	return stopAll_;
}

AMControl * SGMHexapod::trajectoryStart() const
{
	return trajectoryStart_;
}

AMControl * SGMHexapod::trajectoryReset() const
{
	return trajectoryReset_;
}

void SGMHexapod::rotateSystem(double rX, double rY, double rZ)
{
	xAxisPrimeControl_->rotate(rX, rY, rZ);
	yAxisPrimeControl_->rotate(rX, rY, rZ);
	zAxisPrimeControl_->rotate(rX, rY, rZ);

	xAxisPrimeTrajectoryControl_->rotate(rX, rY, rZ);
	yAxisPrimeTrajectoryControl_->rotate(rX, rY, rZ);
	zAxisPrimeTrajectoryControl_->rotate(rX, rY, rZ);
}

