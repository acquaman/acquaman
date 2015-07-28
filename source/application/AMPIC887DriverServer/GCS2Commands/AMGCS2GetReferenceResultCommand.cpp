#include "AMGCS2GetReferenceResultCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"

AMGCS2GetReferenceResultCommand::AMGCS2GetReferenceResultCommand(const AMPIC887AxisCollection axesToQuery)
{
	axesToQuery_ = axesToQuery;
}

AMPIC887AxisMap<bool> AMGCS2GetReferenceResultCommand::axesReferenceResults() const
{
	return axisReferenceResults_;
}

bool AMGCS2GetReferenceResultCommand::validateArguments()
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

bool AMGCS2GetReferenceResultCommand::runImplementation()
{
	// Clear previous results
	axisReferenceResults_.clear();

	AMCArrayHandler<int> referencedResultValuesHandler(axesToQuery_.count());
	bool success = false;


	QString axesArgumentsString = axesToQuery_.toString();

	success = PI_qFRF(controller_->id(),
					  axesArgumentsString.toStdString().c_str(),
					  referencedResultValuesHandler.cArray());

	if(success) {

		for(int iAxis = 0, axesCount = axesToQuery_.count();
			iAxis < axesCount;
			++iAxis) {

			axisReferenceResults_.insert(axesToQuery_.at(iAxis),
								  referencedResultValuesHandler.cArray()[iAxis] == 1);
		}
	} else {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
