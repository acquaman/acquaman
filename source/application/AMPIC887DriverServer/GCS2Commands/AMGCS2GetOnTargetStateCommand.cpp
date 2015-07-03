#include "AMGCS2GetOnTargetStateCommand.h"

#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
#include "../AMPIC887Controller.h"

AMGCS2GetOnTargetStateCommand::AMGCS2GetOnTargetStateCommand(const QList<AMGCS2::Axis>& axes )
{
	axesToQuery_ = axes;
}

QString AMGCS2GetOnTargetStateCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	QString outputString("Axis\t\tOn Target\n");

	foreach(AMGCS2::Axis currentAxis, onTargetStates_.keys()) {

		QString stateString;
		if(onTargetStates_.value(currentAxis)) {
			stateString = "Yes";
		} else {
			stateString = "No";
		}

		outputString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(stateString));
	}

	return outputString.trimmed();
}

QHash<AMGCS2::Axis, bool> AMGCS2GetOnTargetStateCommand::onTargetStates() const
{
	return onTargetStates_;
}

bool AMGCS2GetOnTargetStateCommand::validateArguments()
{
	if(axesToQuery_.count() > AXIS_COUNT) {
		lastError_ = "Too many axes specified";
		return false;
	}

	foreach(AMGCS2::Axis currentAxis, onTargetStates_.keys()) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Cannot get on target state - Unknown axis";
			return false;
		}
	}

	return true;
}

bool AMGCS2GetOnTargetStateCommand::runImplementation()
{
	// Clear previous results.
	onTargetStates_.clear();

	AMCArrayHandler<int> onTargetValuesHandler(AXIS_COUNT);
	bool successful = false;

	if(axesToQuery_.isEmpty()) {
		successful = PI_qONT(controller_->id(), 0, onTargetValuesHandler.cArray());

		if(successful) {

			onTargetStates_.insert(AMGCS2::XAxis, (onTargetValuesHandler.cArray()[0] == 1));
			onTargetStates_.insert(AMGCS2::YAxis, (onTargetValuesHandler.cArray()[1] == 1));
			onTargetStates_.insert(AMGCS2::ZAxis, (onTargetValuesHandler.cArray()[2] == 1));
			onTargetStates_.insert(AMGCS2::UAxis, (onTargetValuesHandler.cArray()[3] == 1));
			onTargetStates_.insert(AMGCS2::VAxis, (onTargetValuesHandler.cArray()[4] == 1));
			onTargetStates_.insert(AMGCS2::WAxis, (onTargetValuesHandler.cArray()[5] == 1));
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

		successful = PI_qONT(controller_->id(),
							 axesArgumentString.toStdString().c_str(),
							 onTargetValuesHandler.cArray());

		if(successful) {
			for(int iAxis = 0, axisCount = axesToQuery_.count();
				iAxis < axisCount;
				++iAxis) {

				onTargetStates_.insert(axesToQuery_.at(iAxis), (onTargetValuesHandler.cArray()[iAxis] == 1));
			}
		}
	}

	if(!successful) {
		lastError_ = controllerErrorMessage();
	}

	return successful;


}
