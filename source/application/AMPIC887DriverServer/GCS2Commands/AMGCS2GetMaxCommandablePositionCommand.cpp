#include "AMGCS2GetMaxCommandablePositionCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
AMGCS2GetMaxCommandablePositionCommand::AMGCS2GetMaxCommandablePositionCommand(const QList<AMGCS2::Axis> axes )
{
	axesToQuery_ = axes;
}

QString AMGCS2GetMaxCommandablePositionCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	QString returnString("Axis\t\tMax Commandable Position\n");

	foreach(AMGCS2::Axis currentAxis, maxCommandablePositions_.keys()) {

		returnString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(maxCommandablePositions_.value(currentAxis)));
	}

	return returnString.trimmed();
}

QHash<AMGCS2::Axis, double> AMGCS2GetMaxCommandablePositionCommand::maxCommandablePositions() const
{
	return maxCommandablePositions_;
}

bool AMGCS2GetMaxCommandablePositionCommand::validateArguments()
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

bool AMGCS2GetMaxCommandablePositionCommand::runImplementation()
{
	maxCommandablePositions_.clear();

	AMCArrayHandler<double> maxPositionHandler(AXIS_COUNT);
	bool success = false;

	if(axesToQuery_.count() == 0) {

		success = PI_qTMX(controller_->id(), 0, maxPositionHandler.cArray());

		if(success) {

			maxCommandablePositions_.insert(AMGCS2::XAxis, maxPositionHandler.cArray()[0]);
			maxCommandablePositions_.insert(AMGCS2::YAxis, maxPositionHandler.cArray()[1]);
			maxCommandablePositions_.insert(AMGCS2::ZAxis, maxPositionHandler.cArray()[2]);
			maxCommandablePositions_.insert(AMGCS2::UAxis, maxPositionHandler.cArray()[3]);
			maxCommandablePositions_.insert(AMGCS2::VAxis, maxPositionHandler.cArray()[4]);
			maxCommandablePositions_.insert(AMGCS2::WAxis, maxPositionHandler.cArray()[5]);
		}
	} else {

		QString axesString;
		for(int iAxis = 0, axisCount = axesToQuery_.count();
			iAxis < axisCount;
			++iAxis) {

			AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);

			axesString.append(QString(" %1")
							  .arg(AMGCS2Support::axisToCharacter(currentAxis)));

			success = PI_qTMX(controller_->id(),
								   axesString.toStdString().c_str(),
								   maxPositionHandler.cArray());

			if(success) {

				for(int iAxis = 0, axisCount = axesToQuery_.count();
					iAxis < axisCount;
					++iAxis) {

					AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);

					maxCommandablePositions_.insert(currentAxis,
													maxPositionHandler.cArray()[iAxis]);
				}
			}
		}
	}

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
