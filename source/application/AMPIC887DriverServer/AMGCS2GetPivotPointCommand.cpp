#include "AMGCS2GetPivotPointCommand.h"
#include "AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"
AMGCS2GetPivotPointCommand::AMGCS2GetPivotPointCommand(const QList<AMGCS2::Axis> axes)
{
	axesToQuery_ = axes;
}

QString AMGCS2GetPivotPointCommand::outputString() const
{
	if(!wasSuccessful_) {
		return "";
	}


	QString outputString("Axis\t\tPivot Point\n");

	foreach(AMGCS2::Axis currentAxis, axisPivotPoints_.keys()) {

		double pivotPointValue = axisPivotPoints_.value(currentAxis);
		outputString.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(pivotPointValue));
	}

	return outputString.trimmed();
}

QHash<AMGCS2::Axis, double> AMGCS2GetPivotPointCommand::axisPivotPoints() const
{
	return axisPivotPoints_;
}

bool AMGCS2GetPivotPointCommand::validateArguments()
{
	if(axesToQuery_.count() > 3) {
		lastError_ = "Too many axes specified";
		return false;
	}

	foreach(AMGCS2::Axis currentAxis, axesToQuery_) {

		if(currentAxis == AMGCS2::UnknownAxis ||
				currentAxis == AMGCS2::UAxis ||
				currentAxis == AMGCS2::VAxis ||
				currentAxis == AMGCS2::WAxis) {

			lastError_ = "Cannot obtain pivot point - Only X, Y and Z axes valid";
			return false;
		}
	}

	return true;
}

bool AMGCS2GetPivotPointCommand::runImplementation()
{
	AMCArrayHandler<double> pivotPointValuesHandler(AXIS_COUNT);
	bool success = false;

	if(axesToQuery_.isEmpty()) {

		success = PI_qSPI(controllerId_, 0, pivotPointValuesHandler.cArray());

		if(success) {
			axisPivotPoints_.insert(AMGCS2::XAxis, pivotPointValuesHandler.cArray()[0]);
			axisPivotPoints_.insert(AMGCS2::YAxis, pivotPointValuesHandler.cArray()[1]);
			axisPivotPoints_.insert(AMGCS2::ZAxis, pivotPointValuesHandler.cArray()[2]);
		}

	} else {

		QString axesString;

		foreach(AMGCS2::Axis currentAxis, axesToQuery_) {
			axesString.append(QString(" %1")
							  .arg(AMGCS2Support::axisToCharacter(currentAxis)));
		}

		success = PI_qSPI(controllerId_,
						  axesString.toStdString().c_str(),
						  pivotPointValuesHandler.cArray());

		if(success) {

			for(int iAxis = 0, axisCount = axesToQuery_.count();
				iAxis < axisCount;
				++iAxis) {

				AMGCS2::Axis currentAxis = axesToQuery_.at(iAxis);
				axisPivotPoints_.insert(currentAxis,
										pivotPointValuesHandler.cArray()[iAxis]);
			}
		}
	}

	if(!success) {
		lastError_ = controllerErrorMessage();
	}

	return success;


}
