#include "SGMHexapod.h"
#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/SGM/SGMMAXvMotor.h"
#include "beamline/SGM/SGMHexapodTransformedAxis.h"
#include "beamline/AMBeamline.h"
SGMHexapod::SGMHexapod(QObject *parent) :
    QObject(parent)
{

	AMControl* globalXAxisSetpoint = new AMSinglePVControl("Hexapod Global X Axis", "HXPD1611-4-I10-01:trajectory:X:mm", this, HEXAPOD_POSITION_TOLERANCE, 2.0);
	AMControl* globalYAxisSetpoint = new AMSinglePVControl("Hexapod Global Y Axis", "HXPD1611-4-I10-01:trajectory:Y:mm", this, HEXAPOD_POSITION_TOLERANCE, 2.0);
	AMControl* globalZAxisSetpoint = new AMSinglePVControl("Hexapod Global Z Axis", "HXPD1611-4-I10-01:trajectory:Z:mm", this, HEXAPOD_POSITION_TOLERANCE, 2.0);

	AMControl* globalXAxisFeedback = new AMReadOnlyPVwStatusControl("Hexapod Global X Axis Feedback", "HXPD1611-4-I10-01:X:mm:fbk", "HXPD1611-4-I10-01:X:status",this, new CLSMAXvControlStatusChecker());
	AMControl* globalYAxisFeedback = new AMReadOnlyPVwStatusControl("Hexapod Global Y Axis Feedback", "HXPD1611-4-I10-01:Y:mm:fbk", "HXPD1611-4-I10-01:Y:status",this, new CLSMAXvControlStatusChecker());
	AMControl* globalZAxisFeedback = new AMReadOnlyPVwStatusControl("Hexapod Global Z Axis Feedback", "HXPD1611-4-I10-01:Z:mm:fbk", "HXPD1611-4-I10-01:Z:status",this, new CLSMAXvControlStatusChecker());

	AMControl* globalXAxisStatus = new AMSinglePVControl("Hexapod Global X Axis Status", "HXPD1611-4-I10-01:X:status", this, 0.5);
	AMControl* globalYAxisStatus = new AMSinglePVControl("Hexapod Global Y Axis Status", "HXPD1611-4-I10-01:Y:status", this, 0.5);
	AMControl* globalZAxisStatus = new AMSinglePVControl("Hexapod Global Z Axis Status", "HXPD1611-4-I10-01:Z:status", this, 0.5);


	trajectoryStart_ = new AMSinglePVControl("Hexapod Trajectory Move Start", "HXPD1611-4-I10-01:trajectory:start", this, 0.5);
	trajectoryReset_ = new AMSinglePVControl("Hexapod Trajectory Reset", "HXPD1611-4-I10-01:trajectory:reset", this, 0.5);

	systemVelocity_ = new AMPVControl("Hexapod Velocity", "HXPD1611-4-I10-01:velocity:fbk", "HXPD1611-4-I10-01:velocity",QString(), this, 0.001, 2.0);
	stopAll_ = new AMSinglePVControl("Hexapod Stop", "HXPD1611-4-I10-01:stop", this, 0.5);

	dataRecorderRate_ = new AMPVControl("Hexapod Data Recorder Rate", "HXPD1611-4-I10-01:recorder:rate:hz:fbk", "HXPD1611-4-I10-01:recorder:rate:hz", QString(), this, 0.1, 2);
	dataRecorderStatus_ = new AMSinglePVControl("Hexapod Data Recorder Status", "HXPD1611-4-I10-01:recorder:active", this, 0.5, 2.0);

	xGlobalRecorderControl_ = new AMWaveformBinningSinglePVControl("Hexapod Untransformed X Axis Recorder Data",
	                                                               "HXPD1611-4-I10-01:recorder:X:mm",
	                                                               0,
	                                                               1024,
	                                                               this);
	xGlobalRecorderControl_->setAttemptDouble(true);

	yGlobalRecorderControl_ = new AMWaveformBinningSinglePVControl("Hexapod Untransformed Y Axis Recorder Data",
	                                                               "HXPD1611-4-I10-01:recorder:Y:mm",
	                                                               0,
	                                                               1024,
	                                                               this);
	yGlobalRecorderControl_->setAttemptDouble(true);

	zGlobalRecorderControl_ = new AMWaveformBinningSinglePVControl("Hexapod Untransformed Z Axis Recorder Data",
	                                                               "HXPD1611-4-I10-01:recorder:Z:mm",
	                                                               0,
	                                                               1024,
	                                                               this);
	zGlobalRecorderControl_->setAttemptDouble(true);

	timeRecorderControl_ = new AMWaveformBinningSinglePVControl("Hexapod Delta Time Recorder Data",
	                                                               "HXPD1611-4-I10-01:recorder:time:s",
	                                                               0,
	                                                               1024,
	                                                               this);
	timeRecorderControl_->setAttemptDouble(true);

	xAxisPrimeControl_ = new SGMHexapodTransformedAxis(AM3DCoordinatedSystemControl::XAxis,
													   globalXAxisSetpoint,
													   globalXAxisFeedback,
													   globalXAxisStatus,
													   globalYAxisSetpoint,
													   globalYAxisFeedback,
													   globalYAxisStatus,
													   globalZAxisSetpoint,
													   globalZAxisFeedback,
													   globalZAxisStatus,
													   trajectoryStart_,
	                                                   systemVelocity_,
	                                                   dataRecorderRate_,
	                                                   dataRecorderStatus_,
													   "Hexapod X Axis Prime",
													   "mm",
													   this);

	yAxisPrimeControl_ = new SGMHexapodTransformedAxis(AM3DCoordinatedSystemControl::YAxis,
													   globalXAxisSetpoint,
													   globalXAxisFeedback,
													   globalXAxisStatus,
													   globalYAxisSetpoint,
													   globalYAxisFeedback,
													   globalYAxisStatus,
													   globalZAxisSetpoint,
													   globalZAxisFeedback,
													   globalZAxisStatus,
													   trajectoryStart_,
	                                                   systemVelocity_,
	                                                   dataRecorderRate_,
	                                                   dataRecorderStatus_,
													   "Hexapod Y Axis Prime",
													   "mm",
													   this);

	zAxisPrimeControl_ = new SGMHexapodTransformedAxis(AM3DCoordinatedSystemControl::ZAxis,
													   globalXAxisSetpoint,
													   globalXAxisFeedback,
													   globalXAxisStatus,
													   globalYAxisSetpoint,
													   globalYAxisFeedback,
													   globalYAxisStatus,
													   globalZAxisSetpoint,
													   globalZAxisFeedback,
													   globalZAxisStatus,
													   trajectoryStart_,
	                                                   systemVelocity_,
	                                                   dataRecorderRate_,
	                                                   dataRecorderStatus_,
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



	allControls_ = new AMControlSet(this);
	allControls_->addControl(globalXAxisSetpoint);
	allControls_->addControl(globalYAxisSetpoint);
	allControls_->addControl(globalZAxisSetpoint);
	allControls_->addControl(globalXAxisFeedback);
	allControls_->addControl(globalYAxisFeedback);
	allControls_->addControl(globalZAxisFeedback);
	allControls_->addControl(globalXAxisStatus);
	allControls_->addControl(globalYAxisStatus);
	allControls_->addControl(globalZAxisStatus);

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

	allControls_->addControl(dataRecorderRate_);
	allControls_->addControl(dataRecorderStatus_);

	allControls_->addControl(xGlobalRecorderControl_);
	allControls_->addControl(yGlobalRecorderControl_);
	allControls_->addControl(zGlobalRecorderControl_);
	allControls_->addControl(timeRecorderControl_);

	connect(allControls_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
}

bool SGMHexapod::isConnected()
{
	return allControls_->isConnected();
}

AMControlSet *SGMHexapod::allHexapodControls() const
{
	return allControls_;
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

AMControl * SGMHexapod::dataRecorderStatus() const
{
	return dataRecorderStatus_;
}

AMControl * SGMHexapod::dataRecorderRate() const
{
	return dataRecorderRate_;
}

AMWaveformBinningSinglePVControl * SGMHexapod::xGlobalRecorderControl() const
{
	return xGlobalRecorderControl_;
}

AMWaveformBinningSinglePVControl * SGMHexapod::yGlobalRecorderControl() const
{
	return yGlobalRecorderControl_;
}

AMWaveformBinningSinglePVControl * SGMHexapod::zGlobalRecorderControl() const
{
	return zGlobalRecorderControl_;
}

AMWaveformBinningSinglePVControl * SGMHexapod::timeRecorderControl() const
{
	return timeRecorderControl_;
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

void SGMHexapod::resetSystem()
{
	xAxisPrimeControl_->resetRotationsToGlobal();
	yAxisPrimeControl_->resetRotationsToGlobal();
	zAxisPrimeControl_->resetRotationsToGlobal();

	xAxisPrimeTrajectoryControl_->resetRotationsToGlobal();
	yAxisPrimeTrajectoryControl_->resetRotationsToGlobal();
	zAxisPrimeTrajectoryControl_->resetRotationsToGlobal();
}

QList<QVector3D> SGMHexapod::transformVectors(const QList<QVector3D> &coordinates)
{
	// For this it is safe to assume that all axes are rotated to the same degree
	QList<QVector3D> rotatedAxes;

	for (int iCurrentVector = 0, count = coordinates.count();
	     iCurrentVector < count;
	     ++iCurrentVector) {

		rotatedAxes.append(xAxisPrimeControl_->globalAxisToPrime(coordinates.at(iCurrentVector)));
	}

	return rotatedAxes;
}



