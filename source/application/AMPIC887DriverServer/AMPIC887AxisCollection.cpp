#include "AMPIC887AxisCollection.h"
#include <QStringList>

#include "AMGCS2Support.h"

AMPIC887AxisCollection::AMPIC887AxisCollection(InitializationState initializationState)
{
	if(initializationState == LinearAxes || initializationState == AllAxes) {
		append(AMGCS2::XAxis);
		append(AMGCS2::YAxis);
		append(AMGCS2::ZAxis);
	}

	if(initializationState == RotationalAxes || initializationState == AllAxes) {
		append(AMGCS2::UAxis);
		append(AMGCS2::VAxis);
		append(AMGCS2::WAxis);
	}
}

AMPIC887AxisCollection::AMPIC887AxisCollection(const QList<AMGCS2::Axis>& axes)
	:	QList<AMGCS2::Axis>(axes)
{

}

AMPIC887AxisCollection::ValidState AMPIC887AxisCollection::validate() const
{
	foreach(AMGCS2::Axis currentAxis, *this) {

		if(currentAxis == AMGCS2::UnknownAxis) {
			return ContainsUnknownAxis;
		}
	}

	if(count() > 6) {
		return ContainsDuplicateAxes;
	}

	return Valid;
}

bool AMPIC887AxisCollection::containsRotationalAxes() const
{
	foreach(AMGCS2::Axis currentAxis, *this) {
		if(currentAxis == AMGCS2::UAxis ||
				currentAxis == AMGCS2::VAxis ||
				currentAxis == AMGCS2::WAxis) {
			return true;
		}
	}

	return false;
}

QString AMPIC887AxisCollection::toString() const
{
	QString returnString;

	foreach(AMGCS2::Axis currentAxis, *this) {
		returnString.append(QString(" %1")
							.arg(AMGCS2Support::axisToCharacter(currentAxis)));
	}

	return returnString.trimmed();
}

AMPIC887AxisCollection AMPIC887AxisCollection::fromString(const QString &axesString,
														  bool *parseSuccess,
														  InitializationState stateIfEmpty)
{

	QStringList axisStringList = axesString.trimmed().split(" ", QString::SkipEmptyParts);

	if(axisStringList.isEmpty()) {
		if(parseSuccess) {
			*parseSuccess = true;
		}

		return AMPIC887AxisCollection(stateIfEmpty);
	}

	AMPIC887AxisCollection returnCollection(EmptyCollection);
	foreach(QString currentAxisString, axisStringList) {
		// We can only parse single characters.
		if(currentAxisString.length() > 1) {
			if(parseSuccess) {
				*parseSuccess = false;
				return AMPIC887AxisCollection(EmptyCollection);
			}
		}

		 returnCollection.append(AMGCS2Support::characterToAxis(currentAxisString.at(0)));
	}

	if(parseSuccess) {
		*parseSuccess = true;
	}

	return returnCollection;
}

