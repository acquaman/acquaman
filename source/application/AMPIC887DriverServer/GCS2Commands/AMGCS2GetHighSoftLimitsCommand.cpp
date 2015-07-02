#include "AMGCS2GetHighSoftLimitsCommand.h"
#include "../AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
AMGCS2GetHighSoftLimitsCommand::AMGCS2GetHighSoftLimitsCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToQuery_ = axes;
}

QHash<AMGCS2::Axis, double> AMGCS2GetHighSoftLimitsCommand::axesHighSoftLimits() const
{
	return axesHighSoftLimits_;
}

QString AMGCS2GetHighSoftLimitsCommand::outputString() const
{
	if(runningState_ != Succeeded) {
		return "";
	}

	QString outputString("Axis\t\tHigh Limit\n");
	QList<AMGCS2::Axis> axesResults = axesHighSoftLimits_.keys();

	for(int iAxis = 0, axisCount = axesResults.count();
		iAxis < axisCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axesResults.at(iAxis);
		outputString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(axesHighSoftLimits_.value(currentAxis)));
	}

	return outputString.trimmed();
}

bool AMGCS2GetHighSoftLimitsCommand::validateArguments()
{
	if(axesToQuery_.count() > AXIS_COUNT) {
		lastError_ = "Too many axes specified";
		return false;
	}

	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {
		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Cannot get soft high limits - Unknown axis provided";
			return false;
		}
	}

	return true;
}

bool AMGCS2GetHighSoftLimitsCommand::runImplementation()
{
	// Clear previous results.
	axesHighSoftLimits_.clear();

	AMCArrayHandler<double> limitValuesHandler(AXIS_COUNT);
	bool successful = false;

	if(axesToQuery_.isEmpty()) {
		successful = PI_qPLM(controllerId_, 0, limitValuesHandler.cArray());

		if(successful) {

			axesHighSoftLimits_.insert(AMGCS2::XAxis, limitValuesHandler.cArray()[0]);
			axesHighSoftLimits_.insert(AMGCS2::YAxis, limitValuesHandler.cArray()[1]);
			axesHighSoftLimits_.insert(AMGCS2::ZAxis, limitValuesHandler.cArray()[2]);
			axesHighSoftLimits_.insert(AMGCS2::UAxis, limitValuesHandler.cArray()[3]);
			axesHighSoftLimits_.insert(AMGCS2::VAxis, limitValuesHandler.cArray()[4]);
			axesHighSoftLimits_.insert(AMGCS2::WAxis, limitValuesHandler.cArray()[5]);
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

		successful = PI_qPLM(controllerId_,
							 axesArgumentString.toStdString().c_str(),
							 limitValuesHandler.cArray());

		if(successful) {
			for(int iAxis = 0, axisCount = axesToQuery_.count();
				iAxis < axisCount;
				++iAxis) {

				axesHighSoftLimits_.insert(axesToQuery_.at(iAxis), limitValuesHandler.cArray()[iAxis]);
			}
		}
	}

	if(!successful) {
		lastError_ = controllerErrorMessage();
	}

	return successful;
}
