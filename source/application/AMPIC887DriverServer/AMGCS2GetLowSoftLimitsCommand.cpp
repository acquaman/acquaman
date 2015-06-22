#include "AMGCS2GetLowSoftLimitsCommand.h"
#include "AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
AMGCS2GetLowSoftLimitsCommand::AMGCS2GetLowSoftLimitsCommand(const QList<AMGCS2::Axis>& axes)
{
	axesToQuery_ = axes;
}

QHash<AMGCS2::Axis, double> AMGCS2GetLowSoftLimitsCommand::axesSoftLowLimits() const
{
	return axesSoftLowLimits_;
}

QString AMGCS2GetLowSoftLimitsCommand::outputString() const
{
	if(!wasSuccessful_) {
		return "";
	}

	QString outputString("Axis\t\tLow Limit\n");
	QList<AMGCS2::Axis> axesResults = axesSoftLowLimits_.keys();

	for(int iAxis = 0, axisCount = axesResults.count();
		iAxis < axisCount;
		++iAxis) {

		AMGCS2::Axis currentAxis = axesResults.at(iAxis);
		outputString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(axesSoftLowLimits_.value(currentAxis)));
	}

	return outputString.trimmed();
}

bool AMGCS2GetLowSoftLimitsCommand::validateArguments()
{
	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {
		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Cannot get soft low limits - Unknown axis provided";
			return false;
		}
	}

	return true;
}

bool AMGCS2GetLowSoftLimitsCommand::runImplementation()
{
	// Clear previous results.
	axesSoftLowLimits_.clear();

	AMCArrayHandler<double> limitValuesHandler(AXIS_COUNT);
	bool successful = false;

	if(axesToQuery_.isEmpty()) {
		successful = PI_qNLM(controllerId_, 0, limitValuesHandler.cArray());

		if(successful) {

			axesSoftLowLimits_.insert(AMGCS2::XAxis, limitValuesHandler.cArray()[0]);
			axesSoftLowLimits_.insert(AMGCS2::YAxis, limitValuesHandler.cArray()[1]);
			axesSoftLowLimits_.insert(AMGCS2::ZAxis, limitValuesHandler.cArray()[2]);
			axesSoftLowLimits_.insert(AMGCS2::UAxis, limitValuesHandler.cArray()[3]);
			axesSoftLowLimits_.insert(AMGCS2::VAxis, limitValuesHandler.cArray()[4]);
			axesSoftLowLimits_.insert(AMGCS2::WAxis, limitValuesHandler.cArray()[5]);
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

		successful = PI_qNLM(controllerId_,
							 axesArgumentString.toStdString().c_str(),
							 limitValuesHandler.cArray());

		if(successful) {
			for(int iAxis = 0, axisCount = axesToQuery_.count();
				iAxis < axisCount;
				++iAxis) {

				axesSoftLowLimits_.insert(axesToQuery_.at(iAxis), limitValuesHandler.cArray()[iAxis]);
			}
		}
	}

	if(!successful) {
		lastError_ = controllerErrorMessage();
	}

	return successful;
}
