#include "AMGCS2GetMinCommandablePositionCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"

AMGCS2GetMinCommandablePositionCommand::AMGCS2GetMinCommandablePositionCommand(const QList<AMGCS2::Axis> axes)
{
	axesToQuery_ = axes;
}

QString AMGCS2GetMinCommandablePositionCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	QString returnString("Axis\t\tMin Commandable Position\n");

	foreach(AMGCS2::Axis currentAxis, minCommandablePositions_.keys()) {

		returnString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(minCommandablePositions_.value(currentAxis)));
	}

	return returnString.trimmed();
}

QHash<AMGCS2::Axis, double> AMGCS2GetMinCommandablePositionCommand::minCommandablePositions() const
{
	return minCommandablePositions_;
}

bool AMGCS2GetMinCommandablePositionCommand::validateArguments()
{
	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Unknown axis provided";
			return false;
		}
	}

	if(axesToQuery_.count() > AXIS_COUNT) {
		lastError_ = "Too many axes provided";
		return false;
	}

	return true;
}

bool AMGCS2GetMinCommandablePositionCommand::runImplementation()
{
	minCommandablePositions_.clear();

	AMCArrayHandler<double> minPositionHandler(AXIS_COUNT);
	bool success = false;

	if(axesToQuery_.count() == 0) {

		success = PI_qTMN(controllerId_, 0, minPositionHandler.cArray());

		if(success) {

			minCommandablePositions_.insert(AMGCS2::XAxis, minPositionHandler.cArray()[0]);
			minCommandablePositions_.insert(AMGCS2::YAxis, minPositionHandler.cArray()[1]);
			minCommandablePositions_.insert(AMGCS2::ZAxis, minPositionHandler.cArray()[2]);
			minCommandablePositions_.insert(AMGCS2::UAxis, minPositionHandler.cArray()[3]);
			minCommandablePositions_.insert(AMGCS2::VAxis, minPositionHandler.cArray()[4]);
			minCommandablePositions_.insert(AMGCS2::WAxis, minPositionHandler.cArray()[5]);
		}
	} else {

		QString axesString;
		for(int iAxis = 0, axisCount = axesToQuery_.count();
			iAxis < axisCount;
			++iAxis) {

			AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);

			axesString.append(QString(" %1")
							  .arg(AMGCS2Support::axisToCharacter(currentAxis)));

			success = PI_qTMN(controllerId_,
								   axesString.toStdString().c_str(),
								   minPositionHandler.cArray());

			if(success) {

				for(int iAxis = 0, axisCount = axesToQuery_.count();
					iAxis < axisCount;
					++iAxis) {

					AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);

					minCommandablePositions_.insert(currentAxis,
													minPositionHandler.cArray()[iAxis]);
				}
			}
		}
	}

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
