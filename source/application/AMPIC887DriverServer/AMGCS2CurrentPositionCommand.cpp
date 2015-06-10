#include "AMGCS2CurrentPositionCommand.h"

#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "AMGCS2Support.h"
AMGCS2CurrentPositionCommand::AMGCS2CurrentPositionCommand(const QList<AMGCS2::Axis>& axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

QHash<AMGCS2::Axis, double> AMGCS2CurrentPositionCommand::axisPositions()
{
	return axisPositions_;
}

QString AMGCS2CurrentPositionCommand::outputString() const
{
	if(!wasSuccessful_) {
		return QString();
	}

	QString returnString;

	foreach (AMGCS2::Axis axis, axisPositions_.keys()) {
		returnString.append(QString("%1 : %2")
							.arg(AMGCS2Support::axisToCharacter(axis))
							.arg(axisPositions_.value(axis)));
		returnString.append("\n");
	}

	return returnString.trimmed();
}

bool AMGCS2CurrentPositionCommand::runImplementation()
{
	AMCArrayHandler<double> positionValuesHandler(AXIS_COUNT);
	bool successful = false;

	if(axesToQuery_.isEmpty()) {
		successful = PI_qPOS(controllerId_, 0, positionValuesHandler.cArray());

		if(successful) {

			axisPositions_.insert(AMGCS2::XAxis, positionValuesHandler.cArray()[0]);
			axisPositions_.insert(AMGCS2::YAxis, positionValuesHandler.cArray()[1]);
			axisPositions_.insert(AMGCS2::ZAxis, positionValuesHandler.cArray()[2]);
			axisPositions_.insert(AMGCS2::UAxis, positionValuesHandler.cArray()[3]);
			axisPositions_.insert(AMGCS2::VAxis, positionValuesHandler.cArray()[4]);
			axisPositions_.insert(AMGCS2::WAxis, positionValuesHandler.cArray()[5]);
		}

	} else {

		QString axesArgumentString;

		for (int iAxis = 0, axisCount = axesToQuery_.count();
			 iAxis < axisCount;
			 ++iAxis) {

			AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);

			axesArgumentString.append(QString(" %1")
									  .arg(AMGCS2Support::axisToCharacter(currentAxis)));

		}

		axesArgumentString = axesArgumentString.trimmed();

		successful = PI_qPOS(controllerId_,
							 axesArgumentString.toStdString().c_str(),
							 positionValuesHandler.cArray());

		if(successful) {
			for(int iAxis = 0, axisCount = axesToQuery_.count();
				iAxis < axisCount;
				++iAxis) {

				axisPositions_.insert(axesToQuery_.at(iAxis), positionValuesHandler.cArray()[iAxis]);
			}
		}
	}

	if(!successful) {
		lastError_ = controllerErrorMessage();
	}

	return successful;
}

bool AMGCS2CurrentPositionCommand::validateArguments()
{
	for(int iAxis = 0, axisCount = axesToQuery_.count();
		iAxis < axisCount;
		++iAxis) {

		if(axesToQuery_.at(iAxis) == AMGCS2::UnknownAxis) {
			lastError_ = "Unknown axis specified";
			return false;
		}
	}

	return true;
}


