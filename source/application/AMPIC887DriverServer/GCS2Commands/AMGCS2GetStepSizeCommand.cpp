#include "AMGCS2GetStepSizeCommand.h"

#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
AMGCS2GetStepSizeCommand::AMGCS2GetStepSizeCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToQuery_ = axes;
}

QString AMGCS2GetStepSizeCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	QString returnString("Axis\t\tStep Size\n");

	foreach(AMGCS2::Axis currentAxis, axisStepSizes_.keys()) {

		returnString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(axisStepSizes_.value(currentAxis)));
	}

	return returnString.trimmed();
}

QHash<AMGCS2::Axis, double> AMGCS2GetStepSizeCommand::axisStepSizes() const
{
	return axisStepSizes_;
}

bool AMGCS2GetStepSizeCommand::validateArguments()
{
	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Unknown axis";
			return false;
		}
	}

	if(axesToQuery_.count() > AXIS_COUNT) {

		lastError_ = "Too many axes provided";
		return false;
	}

	return true;
}

bool AMGCS2GetStepSizeCommand::runImplementation()
{
	AMCArrayHandler<double> stepSizeValues(AXIS_COUNT);
	bool success = false;

	if(axesToQuery_.isEmpty()) {

		success = PI_qSST(controllerId_, 0, stepSizeValues.cArray());

		if(success) {
			axisStepSizes_.insert(AMGCS2::XAxis, stepSizeValues.cArray()[0]);
			axisStepSizes_.insert(AMGCS2::YAxis, stepSizeValues.cArray()[1]);
			axisStepSizes_.insert(AMGCS2::ZAxis, stepSizeValues.cArray()[2]);
			axisStepSizes_.insert(AMGCS2::UAxis, stepSizeValues.cArray()[3]);
			axisStepSizes_.insert(AMGCS2::VAxis, stepSizeValues.cArray()[4]);
			axisStepSizes_.insert(AMGCS2::WAxis, stepSizeValues.cArray()[5]);
		}

	} else {

		QString axesString;

		for(int iAxis = 0, axisCount = axesToQuery_.count();
			iAxis < axisCount;
			++iAxis) {

			AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);
			axesString.append(QString(" %1")
							  .arg(AMGCS2Support::axisToCharacter(currentAxis)));
		}

		success = PI_qSST(controllerId_,
						  axesString.toStdString().c_str(),
						  stepSizeValues.cArray());

		if(success) {

			for(int iAxis = 0, axisCount = axesToQuery_.count();
				iAxis < axisCount;
				++iAxis) {

				AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);
				axisStepSizes_.insert(currentAxis,
									  stepSizeValues.cArray()[iAxis]);
			}
		}
	}

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;
}
