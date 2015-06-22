#include "AMGCS2GetPositionUnitsCommand.h"
#include "AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include <QStringList>
AMGCS2GetPositionUnitsCommand::AMGCS2GetPositionUnitsCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToQuery_ = axes;
}

QHash<AMGCS2::Axis, AMGCS2::PositionUnits> AMGCS2GetPositionUnitsCommand::axesUnits() const
{
	return axesUnits_;
}

QString AMGCS2GetPositionUnitsCommand::outputString() const
{
	if(!wasSuccessful_) {
		return "";
	}

	QString outputString("Axis\t\tUnits\n");

	foreach(AMGCS2::Axis currentAxis, axesUnits_.keys()) {

		outputString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(AMGCS2Support::positionUnitsToString(axesUnits_.value(currentAxis))));
	}

	return outputString.trimmed();
}

bool AMGCS2GetPositionUnitsCommand::validateArguments()
{
	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {
		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Cannot get position units - Unknown axis";
			return false;
		}
	}

	return true;
}

bool AMGCS2GetPositionUnitsCommand::runImplementation()
{
	AMCArrayHandler<char> unitsBufferHandler(BUFFER_SIZE);
	bool success = false;
	if(axesToQuery_.isEmpty()) {

		success = PI_qPUN(controllerId_, 0, unitsBufferHandler.cArray(), BUFFER_SIZE);

		if(success) {
			QString unitsString = unitsBufferHandler.cArray();
			// units are returned in format: "axis=value\n"
			// we need to remove the "axis=" part and use trimmed to remove the \n
			QStringList rawUnitsList = unitsString.split(" ");
			QStringList unitsList;

			foreach(QString rawUnitReturn, rawUnitsList) {
				unitsList.append(rawUnitReturn.trimmed().mid(2, rawUnitReturn.length() - 2));
			}


			AMGCS2::PositionUnits units = AMGCS2Support::stringToPositionUnits(unitsList.at(0));
			axesUnits_.insert(AMGCS2::XAxis, units);
			units = AMGCS2Support::stringToPositionUnits(unitsList.at(1));
			axesUnits_.insert(AMGCS2::YAxis, units);
			units = AMGCS2Support::stringToPositionUnits(unitsList.at(2));
			axesUnits_.insert(AMGCS2::ZAxis, units);
			units = AMGCS2Support::stringToPositionUnits(unitsList.at(3));
			axesUnits_.insert(AMGCS2::UAxis, units);
			units = AMGCS2Support::stringToPositionUnits(unitsList.at(4));
			axesUnits_.insert(AMGCS2::VAxis, units);
			units = AMGCS2Support::stringToPositionUnits(unitsList.at(5));
			axesUnits_.insert(AMGCS2::WAxis, units);
		}

	} else {

		QString axesString;

		foreach(AMGCS2::Axis currentAxis, axesToQuery_) {
			axesString.append(QString(" %1")
							  .arg(AMGCS2Support::axisToCharacter(currentAxis)));
		}

		success = PI_qPUN(controllerId_,
						  axesString.toStdString().c_str(),
						  unitsBufferHandler.cArray(),
						  BUFFER_SIZE);

		if(success) {
			QString unitsString = unitsBufferHandler.cArray();
			// units are returned in format: "axis=value\n"
			// we need to remove the "axis=" part and use trimmed to remove the \n
			QStringList rawUnitsList = unitsString.split(" ");
			QStringList unitsList;

			foreach(QString rawUnitReturn, rawUnitsList) {
				unitsList.append(rawUnitReturn.trimmed().mid(2, rawUnitReturn.length() - 2));
			}

			for(int iAxis = 0, axisCount = axesToQuery_.count();
				iAxis < axisCount;
				++iAxis) {

				AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);
				AMGCS2::PositionUnits positionUnits =
						AMGCS2Support::stringToPositionUnits(unitsList.at(iAxis));

				axesUnits_.insert(currentAxis, positionUnits);
			}

		}
	}

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
