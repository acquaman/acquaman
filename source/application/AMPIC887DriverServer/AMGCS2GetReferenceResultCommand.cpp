#include "AMGCS2GetReferenceResultCommand.h"
#include "AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
#include "PI_GCS2_DLL.h"

AMGCS2GetReferenceResultCommand::AMGCS2GetReferenceResultCommand(const QList<AMGCS2::Axis> & axes)
{
	axes_ = axes;
}

QString AMGCS2GetReferenceResultCommand::outputString() const
{
	if(!wasSuccessful_) {
		return "";
	}

	QString returnOutput("Axis\t\tIs Referenced\n");

	QList<AMGCS2::Axis> axisKeys = axisReferenceResults_.keys();

	foreach(AMGCS2::Axis currentAxis, axisKeys) {


		QString result;
		if(axisReferenceResults_.value(currentAxis)) {
			result = "Yes";
		} else {
			result = "No";
		}

		returnOutput.append(QString("%1\t\t%2\n")
							.arg(AMGCS2Support::axisToCharacter(currentAxis))
							.arg(result));
	}

	return returnOutput.trimmed();
}

QHash<AMGCS2::Axis, bool> AMGCS2GetReferenceResultCommand::axesReferenceResults() const
{
	return axisReferenceResults_;
}

bool AMGCS2GetReferenceResultCommand::validateArguments()
{
	if(axes_.count() > AXIS_COUNT) {
		lastError_ = "Too many axes specified";
		return false;
	}

	foreach (AMGCS2::Axis currentAxis, axes_) {
		if(currentAxis == AMGCS2::UnknownAxis) {
			lastError_ = "Could not obtain reference status of unknown axis";
			return false;
		}
	}

	return true;
}

bool AMGCS2GetReferenceResultCommand::runImplementation()
{
	//Clear previous results:
	axisReferenceResults_.clear();

	AMCArrayHandler<int> referenceResults(axes_.length());
	QString axesString;



	foreach(AMGCS2::Axis currentAxis, axes_) {
		axesString.append(QString(" %1")
						  .arg(AMGCS2Support::axisToCharacter(currentAxis)));
	}

	axesString = axesString.trimmed();


	bool success = PI_qFRF(controllerId_,
						   axesString.toStdString().c_str(),
						   referenceResults.cArray());


	if(!success) {
		lastError_ = controllerErrorMessage();
	} else {

		QList<AMGCS2::Axis> axisResults = axes_;

		if(axes_.count() == 0) {
			axisResults << AMGCS2::XAxis
						<< AMGCS2::YAxis
						<< AMGCS2::ZAxis
						<< AMGCS2::UAxis
						<< AMGCS2::VAxis
						<< AMGCS2::WAxis;
		}


		for(int iAxis = 0;
			iAxis < axisResults.count();
			++iAxis) {

			axisReferenceResults_.insert(axisResults.at(iAxis),
										 (referenceResults.cArray()[iAxis] == 1) );
		}
	}

	return success;
}
