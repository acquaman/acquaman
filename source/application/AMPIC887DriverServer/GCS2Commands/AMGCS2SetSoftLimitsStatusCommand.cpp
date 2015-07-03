#include "AMGCS2SetSoftLimitsStatusCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"

AMGCS2SetSoftLimitsStatusCommand::AMGCS2SetSoftLimitsStatusCommand(const QHash<AMGCS2::Axis, bool>& softLimitStatuses)
{
	softLimitStatuses_ = softLimitStatuses;
}

bool AMGCS2SetSoftLimitsStatusCommand::validateArguments()
{
	if(softLimitStatuses_.isEmpty()) {
		lastError_ = "No axes / soft limit statuses provided";
		return false;
	}

	foreach(AMGCS2::Axis currentAxis, softLimitStatuses_.keys()) {

		if(currentAxis == AMGCS2::UnknownAxis) {

			lastError_ = "Unknown axis provided";
			return false;
		}
	}

	if(softLimitStatuses_.count() > AXIS_COUNT) {
		lastError_ = "Duplicate axes provided";
		return false;
	}

	return true;
}

bool AMGCS2SetSoftLimitsStatusCommand::runImplementation()
{
	AMCArrayHandler<int> softLimitStatusHandler(softLimitStatuses_.count());
	QString axesString;
	QList<AMGCS2::Axis> axesToSet = softLimitStatuses_.keys();

	for(int iAxis = 0, axisCount = axesToSet.count();
		iAxis < axisCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axesToSet.at(iAxis);
		axesString.append(QString(" %1")
						  .arg(AMGCS2Support::axisToCharacter(currentAxis)));

		bool activateLimit = softLimitStatuses_.value(currentAxis);

		if(activateLimit) {
			softLimitStatusHandler.cArray()[iAxis] = 1;
		} else {
			softLimitStatusHandler.cArray()[iAxis] = 0;
		}
	}

	axesString = axesString.trimmed();

	bool success = PI_SSL(controller_->id(),
						  axesString.toStdString().c_str(),
						  softLimitStatusHandler.cArray());

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
