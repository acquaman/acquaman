#include "AMGCS2GetPositionUnitsCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"
#include <QStringList>
AMGCS2GetPositionUnitsCommand::AMGCS2GetPositionUnitsCommand(const AMPIC887AxisCollection& axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

AMPIC887AxisMap<AMGCS2::PositionUnits> AMGCS2GetPositionUnitsCommand::axesUnits() const
{
	return axesUnits_;
}

bool AMGCS2GetPositionUnitsCommand::validateArguments()
{
	if(axesToQuery_.isEmpty()) {
		lastError_ = "No axes to query";
		return false;
	}

	AMPIC887AxisCollection::ValidState validState = axesToQuery_.validate();

	if(validState == AMPIC887AxisCollection::ContainsUnknownAxis) {
		lastError_ = "Unknown axis";
		return false;
	}

	if(validState == AMPIC887AxisCollection::ContainsDuplicateAxes) {
		lastError_ = "Duplicate axes";
		return false;
	}

	return true;
}

bool AMGCS2GetPositionUnitsCommand::runImplementation()
{
	// Clear previous results
	axesUnits_.clear();

	AMCArrayHandler<char> unitValuesHandler(BUFFER_SIZE);
	bool success = false;


	QString axesArgumentsString = axesToQuery_.toString();

	success = PI_qPUN(controller_->id(),
					  axesArgumentsString.toStdString().c_str(),
					  unitValuesHandler.cArray(),
					  BUFFER_SIZE);

	if(success) {

		// Units are returned in format "axis=value\n"
		QString rawReturnValue = unitValuesHandler.cArray();
		QStringList rawReturnList = rawReturnValue.split(" ");
		QStringList parsedStringList;

		foreach(QString rawValue, rawReturnList) {
			parsedStringList.append(rawValue.trimmed().mid(2, rawValue.length() - 2));
		}


		for(int iAxis = 0, axesCount = axesToQuery_.count();
			iAxis < axesCount;
			++iAxis) {

			AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);
			AMGCS2::PositionUnits positionUnits =
					AMGCS2Support::stringToPositionUnits(parsedStringList.at(iAxis));

			axesUnits_.insert(currentAxis, positionUnits);
		}

	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
