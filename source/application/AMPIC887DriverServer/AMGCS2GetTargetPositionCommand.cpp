#include "AMGCS2GetTargetPositionCommand.h"
#include "AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
AMGCS2GetTargetPositionCommand::AMGCS2GetTargetPositionCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToQuery_ = axes;
}

QString AMGCS2GetTargetPositionCommand::outputString() const
{
	if(!wasSuccessful_) {
		return "";
	}

	QString returnString("Axis\t\tTarget Position\n");

	foreach(AMGCS2::Axis currentAxis, axisTargetPositions_.keys()) {

		returnString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(axisTargetPositions_.value(currentAxis)));
	}

	return returnString.trimmed();
}

QHash<AMGCS2::Axis, double> AMGCS2GetTargetPositionCommand::axisTargetPositions() const
{
	return axisTargetPositions_;
}

bool AMGCS2GetTargetPositionCommand::validateArguments()
{
	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Can not obtain target position for unknown axis.";
			return false;
		}
	}

	return true;
}

bool AMGCS2GetTargetPositionCommand::runImplementation()
{
	//Clear previous results:
	axisTargetPositions_.clear();

	AMCArrayHandler<double> targetResults(axesToQuery_.length());
	QString axesString;



	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {
		axesString.append(QString(" %1")
						  .arg(AMGCS2Support::axisToCharacter(currentAxis)));
	}

	axesString = axesString.trimmed();


	bool success = PI_qMOV(controllerId_,
						   axesString.toStdString().c_str(),
						   targetResults.cArray());


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

			axisTargetPositions_.insert(axisResults.at(iAxis),
										targetResults.cArray()[iAxis]);
		}
	}

	return success;
}
