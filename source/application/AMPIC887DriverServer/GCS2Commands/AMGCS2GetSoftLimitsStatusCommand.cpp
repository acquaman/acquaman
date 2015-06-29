#include "AMGCS2GetSoftLimitsStatusCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
AMGCS2GetSoftLimitsStatusCommand::AMGCS2GetSoftLimitsStatusCommand(const QList<AMGCS2::Axis>& axes )
{
	axesToQuery_ = axes;
}

QString AMGCS2GetSoftLimitsStatusCommand::outputString() const
{
	if(!wasSuccessful_) {
		return "";
	}

	QString outputString("Axis\t\tSoft limit status\n");

	foreach(AMGCS2::Axis currentAxis, softLimitStatuses_.keys()) {


		QString activeString;
		if(softLimitStatuses_.value(currentAxis)) {
			activeString = "Active";
		} else {
			activeString = "Inactive";
		}


		outputString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(activeString));
	}

	return outputString.trimmed();
}

QHash<AMGCS2::Axis, bool> AMGCS2GetSoftLimitsStatusCommand::softLimitStatuses() const
{
	return softLimitStatuses_;
}

bool AMGCS2GetSoftLimitsStatusCommand::validateArguments()
{
	if(axesToQuery_.count() > AXIS_COUNT) {
		lastError_ = "Too many axes specified";
		return false;
	}

	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Unknown Axis";
			return false;
		}
	}

	return true;
}

bool AMGCS2GetSoftLimitsStatusCommand::runImplementation()
{
	// Clear previous values
	softLimitStatuses_.clear();

	AMCArrayHandler<int> softLimitStatusHandler(AXIS_COUNT);
	bool success = false;

	if (axesToQuery_.isEmpty()) {

		QString axesString = ("X Y Z U V W");

		success = PI_qSSL(controllerId_,
						 axesString.toStdString().c_str(),
						 softLimitStatusHandler.cArray());

		if(success) {

			softLimitStatuses_.insert(AMGCS2::XAxis, softLimitStatusHandler.cArray()[0] != 0);
			softLimitStatuses_.insert(AMGCS2::YAxis, softLimitStatusHandler.cArray()[1] != 0);
			softLimitStatuses_.insert(AMGCS2::ZAxis, softLimitStatusHandler.cArray()[2] != 0);
			softLimitStatuses_.insert(AMGCS2::UAxis, softLimitStatusHandler.cArray()[3] != 0);
			softLimitStatuses_.insert(AMGCS2::VAxis, softLimitStatusHandler.cArray()[4] != 0);
			softLimitStatuses_.insert(AMGCS2::WAxis, softLimitStatusHandler.cArray()[5] != 0);
		}

	} else {

		QString axesString;

		foreach(AMGCS2::Axis currentAxis, axesToQuery_) {

			axesString.append(QString(" %1")
							  .arg(AMGCS2Support::axisToCharacter(currentAxis)));
		}

		success = PI_qSSL(controllerId_,
						 axesString.toStdString().c_str(),
						 softLimitStatusHandler.cArray());

		if(success) {
			for(int iAxis = 0, axisCount = axesToQuery_.count();
				iAxis < axisCount;
				++iAxis) {

				AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);

				softLimitStatuses_.insert(currentAxis,
										  softLimitStatusHandler.cArray()[iAxis] != 0);
			}
		}

	}


	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
