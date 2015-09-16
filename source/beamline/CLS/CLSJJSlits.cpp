#include "CLSJJSlits.h"
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMOptimizeControlAction.h"

CLSJJSlits::CLSJJSlits(const QString &name, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, QObject *parent) :
	QObject(parent)
{
	// Initialize member variables.

	name_ = name;
	connected_ = false;

	upperBlade_ = new AMPVwStatusControl(name_+"UpperBlade", upperBladePVName+":mm:sp", upperBladePVName+":mm", upperBladePVName+":status", upperBladePVName+":stop", this, 0.005, 5.0);
	lowerBlade_ = new AMPVwStatusControl(name_+"LowerBlade", lowerBladePVName+":mm:sp", lowerBladePVName+":mm", lowerBladePVName+":status", lowerBladePVName+":stop", this, 0.005, 5.0);
	inboardBlade_ = new AMPVwStatusControl(name_+"InboardBlade", inboardBladePVName+":mm:sp", inboardBladePVName+":mm", inboardBladePVName+":status", inboardBladePVName+":stop", this, 0.005, 5.0);
	outboardBlade_ = new AMPVwStatusControl(name_+"OutboardBlade", outboardBladePVName+":mm:sp", outboardBladePVName+":mm", outboardBladePVName+":status", outboardBladePVName+":stop", this, 0.005, 5.0);

	verticalGap_ = new CLSJJSlitGapControl(name_+"VerticalGap", upperBlade_, lowerBlade_, this);
	verticalCenter_ = new CLSJJSlitCenterControl(name_+"VerticalCenter", upperBlade_, lowerBlade_, this);
	horizontalGap_ = new CLSJJSlitGapControl(name_+"HorizontalGap", outboardBlade_, inboardBlade_, this);
	horizontalCenter_ = new CLSJJSlitCenterControl(name_+"HorizontalCenter", outboardBlade_, inboardBlade_, this);

	configuration_ = new AMGenericStepScanConfiguration(this);

	// Listen for connected states.

	connect( verticalGap_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( verticalCenter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( horizontalGap_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );
	connect( horizontalCenter_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Current settings.

	updateConnected();
}

CLSJJSlits::~CLSJJSlits()
{

}

AMControl* CLSJJSlits::control(CLSJJSlits::Direction::Option directionOption, CLSJJSlits::Control::Option controlOption)
{
	AMControl *result = 0;

	if (directionOption == Direction::Vertical && controlOption == Control::Gap)
		result = verticalGap_;
	else if (directionOption == Direction::Vertical && controlOption == Control::Center)
		result = verticalCenter_;
	else if (directionOption == Direction::Horizontal && controlOption == Control::Gap)
		result = horizontalGap_;
	else if (directionOption == Direction::Horizontal && controlOption == Control::Center)
		result = horizontalCenter_;
	else if (directionOption == Direction::Upper && controlOption == Control::Blade)
		result = upperBlade_;
	else if (directionOption == Direction::Lower && controlOption == Control::Blade)
		result = lowerBlade_;
	else if (directionOption == Direction::Inboard && controlOption == Control::Blade)
		result = inboardBlade_;
	else if (directionOption == Direction::Outboard && controlOption == Control::Blade)
		result = outboardBlade_;

	return result;
}

AMAction3* CLSJJSlits::createOptimizationAction(CLSJJSlits::Direction::Option directionOption, CLSJJSlits::Control::Option controlOption)
{
	AMAction3 *result = 0;

	AMControl *controlToOptimize = control(directionOption, controlOption);

	if (controlToOptimize) {
		setupDefaultConfiguration(controlToOptimize, configuration_);
		result = new AMOptimizeControlAction(new AMOptimizeControlActionInfo(configuration_));
	}

	return result;
}

AMAction3* CLSJJSlits::createOptimizationAction(CLSJJSlits::Control::Option controlOption)
{
	Q_UNUSED(controlOption)
	AMAction3 *result = 0;
	return result;
}

void CLSJJSlits::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connectedChanged(connected_);
	}
}

void CLSJJSlits::updateConnected()
{
	bool newState = verticalGap_->isConnected() && verticalCenter_->isConnected() && horizontalGap_->isConnected() && horizontalCenter_->isConnected();
	setConnected(newState);
}

void CLSJJSlits::clearConfiguration(AMGenericStepScanConfiguration *configuration)
{
	if (configuration) {
		foreach (AMScanAxis *axis, configuration->scanAxes()) {
			if (axis)
				configuration->removeControl(axis->id());
		}
	}
}

void CLSJJSlits::setConfigurationControl(AMGenericStepScanConfiguration *configuration, int scanAxis, AMControl *control)
{
	if (configuration && control) {

		// Add new control, plus default region settings.

		configuration->setControl(scanAxis, control->toInfo());
		configuration->scanAxisAt(scanAxis)->regionAt(0)->setRegionStart(control->minimumValue());
		configuration->scanAxisAt(scanAxis)->regionAt(0)->setRegionStep(0.5);
		configuration->scanAxisAt(scanAxis)->regionAt(0)->setRegionEnd(control->maximumValue());
		configuration->scanAxisAt(scanAxis)->regionAt(0)->setRegionTime(1);
	}
}

void CLSJJSlits::setupDefaultConfiguration(AMControl *control, AMGenericStepScanConfiguration *configuration)
{
	if (control && configuration) {

		// Clear any previous scan axes/controls.

		clearConfiguration(configuration);

		// Add new control.

		setConfigurationControl(configuration, 0, control);
	}
}

void CLSJJSlits::setupDefaultConfiguration(const QList<AMControl *> &controls, AMGenericStepScanConfiguration *configuration)
{
	if (configuration) {

		// Clear any previous scan axes/controls.

		clearConfiguration(configuration);

		// Add new controls.

		bool controlOK = true;

		for (int controlIndex = 0, controlCount = controls.count(); controlIndex < controlCount && controlOK; controlIndex++) {
			AMControl *control = controls.at(controlIndex);

			if (control)
				setConfigurationControl(configuration, controlIndex, control);
			else
				controlOK = false;
		}
	}
}
