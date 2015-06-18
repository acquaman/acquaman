#include "AMGCS2GetLimitSwitchStatusCommand.h"

#include "AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
AMGCS2GetLimitSwitchStatusCommand::AMGCS2GetLimitSwitchStatusCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToQuery_ = axes;
}

QString AMGCS2GetLimitSwitchStatusCommand::outputString() const
{
	if(wasSuccessful_) {
		QString returnString("Axis\t\tLimit Switch Active\n");
		foreach(AMGCS2::Axis currentAxis, limitSwitchStatuses_.keys()) {

			QString result;
			if(limitSwitchStatuses_.value(currentAxis)) {
				result = "Yes";
			} else {
				result = "No";
			}

			returnString.append(QString("%1\t\t%2\n")
								.arg(AMGCS2Support::axisToCharacter(currentAxis))
								.arg(result));
		}

		return returnString;
	} else {
		return "";
	}
}

QHash<AMGCS2::Axis, bool> AMGCS2GetLimitSwitchStatusCommand::limitSwitchStatuses() const
{
	return limitSwitchStatuses_;
}

bool AMGCS2GetLimitSwitchStatusCommand::validateArguments()
{
	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {
		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Cannot get limit switch status of unknown axis";
			return false;
		}
	}

	return true;
}

bool AMGCS2GetLimitSwitchStatusCommand::runImplementation()
{
	//Clear previous results:
	limitSwitchStatuses_.clear();

	AMCArrayHandler<int> statusResults(axesToQuery_.length());
	QString axesString;



	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {
		axesString.append(QString(" %1")
						  .arg(AMGCS2Support::axisToCharacter(currentAxis)));
	}

	axesString = axesString.trimmed();


	bool success = PI_qLIM(controllerId_,
						   axesString.toStdString().c_str(),
						   statusResults.cArray());


	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {

		QList<AMGCS2::Axis> axisResults = axesToQuery_;

		if(axesToQuery_.count() == 0) {
			axisResults << AMGCS2::XAxis
						<< AMGCS2::YAxis
						<< AMGCS2::ZAxis
						<< AMGCS2::UAxis
						<< AMGCS2::VAxis
						<< AMGCS2::WAxis;
		}


		for(int iAxis = 0;
			iAxis < axisResults.count();
			++iAxis) {

			limitSwitchStatuses_.insert(axisResults.at(iAxis),
										 (statusResults.cArray()[iAxis] == 1) );
		}
	}

	return success;
}
