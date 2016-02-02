#include "AMControlCalibrateAction.h"
#include "beamline/AMControl.h"
#include "beamline/AMBeamline.h"
#include "util/AMErrorMonitor.h"

AMControlCalibrateAction::AMControlCalibrateAction(AMControlCalibrateActionInfo *info, AMControl *control, QObject *parent) :
	AMAction3(info, parent)
{
	// Initialize class variables.

	control_ = 0;

	// Current settings.

	setControl(control);
}

AMControlCalibrateAction::AMControlCalibrateAction(const AMControlCalibrateAction &original) :
	AMAction3(original)
{
	setControl(original.control());
}

AMControlCalibrateAction::~AMControlCalibrateAction()
{

}

void AMControlCalibrateAction::setControl(AMControl *control)
{
	if (control)
		control_ = control;
	else
		control_ = AMBeamline::bl()->exposedControlByInfo(controlCalibrateInfo()->controlInfo());
}

void AMControlCalibrateAction::onCalibrateStarted()
{
	// Set action as started.

	setStarted();

	// Start progress timer updates.

	connect( &progressTimer_, SIGNAL(timeout()), this, SLOT(onProgressTick()) );
	progressTimer_.start(250);
}

void AMControlCalibrateAction::onCalibrateFailed()
{
	// Disconnect from the control.

	disconnect(control_, 0, this, 0);

	// End progress timer updates.

	progressTimer_.stop();
	disconnect(&progressTimer_, 0, this, 0);

	// Create failure message and set action as failed.

	QString fundamentalFailureMessage = QString("There was an error calibrating the control '%1' such that %2 becomes %3.").arg(control_->name()).arg(double(controlCalibrateInfo()->oldValue())).arg(double(controlCalibrateInfo()->newValue()));
	setFailed(fundamentalFailureMessage);
}

void AMControlCalibrateAction::onCalibrateSucceeded()
{
	// Disconnect from the control.

	disconnect( control_, 0, this, 0 );

	// End progress timer updates.

	progressTimer_.stop();
	disconnect( &progressTimer_, 0, this, 0 );
	setProgress(100, 100);

	// Set action as succeeded.

	setSucceeded();
}

void AMControlCalibrateAction::startImplementation()
{
	// Must have a control, and it must support calibration.

	if(!control_) {
		QString fundamentalFailureMessage = QString("There was an error calibrating the control '%1', because the control was not found.").arg(controlCalibrateInfo()->controlInfo().name());
		AMErrorMon::alert(this, AMCONTROLCALIBRATEACTION_CONTROL_NOT_VALID, QString("%1. Please report this problem to the Acquaman developers.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}

	if(!control_->canCalibrate()) {
		QString fundamentalFailureMessage = QString("There was an error calibrating the control '%1', because the control reported it cannot calibrate.").arg(control_->name());
		AMErrorMon::alert(this, AMCONTROLCALIBRATEACTION_CONTROL_CANT_CALIBRATE, QString("%1. Please report this problem to the beamline staff.").arg(fundamentalFailureMessage));
		setFailed(fundamentalFailureMessage);
		return;
	}

	// Check that the calibration values are valid.

	if (!controlCalibrateInfo()->oldValueValid()) {
		QString fundamentalFailureMessage = QString("There was an error calibrating the control '%1', because the old calibration value is invalid.").arg(control_->name());
		AMErrorMon::alert(this, AMCONTROLCALIBRATEACTION_VALUES_NOT_VALID, fundamentalFailureMessage);
		setFailed(fundamentalFailureMessage);
		return;
	}

	if (!controlCalibrateInfo()->newValueValid()) {
		QString fundamentalFailureMessage = QString("There was an error calibrating the control '%1', because the new calibration value is invalid.").arg(control_->name());
		AMErrorMon::alert(this, AMCONTROLCALIBRATEACTION_VALUES_NOT_VALID, fundamentalFailureMessage);
		setFailed(fundamentalFailureMessage);
		return;
	}

	// If the calibration values are identical, a calibration does not need to take place.

	if (controlCalibrateInfo()->oldValue() == controlCalibrateInfo()->newValue()) {
		QString fundamentalMessage = QString("Calibration for the control '%1' did not take place, because the old calibration value and the new calibration value are identical.").arg(control_->name());
		AMErrorMon::information(this, AMCONTROLCALIBRATEACTION_VALUES_IDENTICAL, fundamentalMessage);
		setSucceeded();
		return;
	}

	// Make connections.

	connect( control_, SIGNAL(calibrationStarted()), this, SLOT(onCalibrateStarted()) );
	connect( control_, SIGNAL(calibrationFailed(int)), this, SLOT(onCalibrateFailed()) );
	connect( control_, SIGNAL(calibrationSucceeded()), this, SLOT(onCalibrateSucceeded()) );

	// Start the calibration.

	int failureExplanation = control_->calibrate(double(controlCalibrateInfo()->oldValue()), double(controlCalibrateInfo()->newValue()));
	if (failureExplanation != AMControl::NoFailure)
		onCalibrateFailed();
}
