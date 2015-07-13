#ifndef AMPIC887AXISMAP_H
#define AMPIC887AXISMAP_H

#include <QMap>
#include <QHash> //TEMP
#include <QList>
#include "AMGCS2.h"
#include "AMPIC887AxisCollection.h"
#include "AMGCS2Support.h"
/*!
  * A class which maps Axes of a PI C887.11 controller to a type T.
  */
template <typename T>
class AMPIC887AxisMap : public QMap<AMGCS2::Axis, T>
{
public:
	/*!
	  * Creates an instance of an axis map, which is empty.
	  */
	AMPIC887AxisMap() {}

	/////////
	// TEMP
	/////////
	AMPIC887AxisMap(const QHash<AMGCS2::Axis, T>& other)
	{
		foreach(AMGCS2::Axis currentAxis, other.keys()) {
			this->value(currentAxis, other.value(currentAxis));
		}
	}

	/*!
	  * Returns a collection of the axes which this map contains.
	  */
	AMPIC887AxisCollection axes() const
	{
		AMPIC887AxisCollection returnCollection(AMPIC887AxisCollection::EmptyCollection);
		QList<AMGCS2::Axis> axisEntries = this->keys();

		foreach(AMGCS2::Axis currentAxisEntry, axisEntries) {
			returnCollection.append(currentAxisEntry);
		}

		return returnCollection;
	}

	/*!
	  * Outputs the map values as a human readable string, if it can parse the
	  * type into a string, otherwise returns the empty string.
	  */
	QString toString(const QString& = "Data")
	{
		return QString();
	}

	/*!
	  * Whether this map contains an entry for the UnknownAxis.
	  */
	bool containsUnknownAxis() const
	{
		return this->contains(AMGCS2::UnknownAxis);
	}
};

// A bunch of partial specializations on the type T, for the toString method
template <>
class AMPIC887AxisMap<bool> : public QMap<AMGCS2::Axis, bool>
{
public:
	/*!
	  * Creates an instance of an axis map, which is empty.
	  */
	AMPIC887AxisMap() {}

	/////////
	// TEMP
	/////////
	AMPIC887AxisMap(const QHash<AMGCS2::Axis, bool>& other)
	{
		foreach(AMGCS2::Axis currentAxis, other.keys()) {
			this->value(currentAxis, other.value(currentAxis));
		}
	}

	/*!
	  * Returns a collection of the axes which this map contains.
	  */
	AMPIC887AxisCollection axes() const
	{
		AMPIC887AxisCollection returnCollection(AMPIC887AxisCollection::EmptyCollection);
		QList<AMGCS2::Axis> axisEntries = this->keys();

		foreach(AMGCS2::Axis currentAxisEntry, axisEntries) {
			returnCollection.append(currentAxisEntry);
		}

		return returnCollection;
	}

	/*!
	  * Outputs the map values as a human readable string, if it can parse the
	  * type into a string, otherwise returns the empty string.
	  */
	QString toString(const QString& header = "Data")
	{
		QString returnString = QString("Axis\t\t%1\n").arg(header);

		QList<AMGCS2::Axis> axes = this->keys();
		foreach(AMGCS2::Axis currentAxis, axes) {
			QString axisValue;
			if(this->value(currentAxis)) {
				axisValue = "Yes";
			} else {
				axisValue = "No";
			}

			returnString.append(QString("%1\t\t%2\n")
								.arg(AMGCS2Support::axisToCharacter(currentAxis))
								.arg(axisValue));
		}

		return returnString.trimmed();
	}

	/*!
	  * Whether this map contains an entry for the UnknownAxis.
	  */
	bool containsUnknownAxis() const
	{
		return this->contains(AMGCS2::UnknownAxis);
	}
};

template <>
class AMPIC887AxisMap<double> : public QMap<AMGCS2::Axis, double>
{
public:
	/*!
	  * Creates an instance of an axis map, which is empty.
	  */
	AMPIC887AxisMap() {}

	/////////
	// TEMP
	/////////
	AMPIC887AxisMap(const QHash<AMGCS2::Axis, double>& other)
	{
		foreach(AMGCS2::Axis currentAxis, other.keys()) {
			this->value(currentAxis, other.value(currentAxis));
		}
	}

	/*!
	  * Returns a collection of the axes which this map contains.
	  */
	AMPIC887AxisCollection axes() const
	{
		AMPIC887AxisCollection returnCollection(AMPIC887AxisCollection::EmptyCollection);
		QList<AMGCS2::Axis> axisEntries = this->keys();

		foreach(AMGCS2::Axis currentAxisEntry, axisEntries) {
			returnCollection.append(currentAxisEntry);
		}

		return returnCollection;
	}

	/*!
	  * Outputs the map values as a human readable string, if it can parse the
	  * type into a string, otherwise returns the empty string.
	  */
	QString toString(const QString& header = "Data")
	{
		QString returnString = QString("Axis\t\t%1\n").arg(header);

		QList<AMGCS2::Axis> axes = this->keys();
		foreach(AMGCS2::Axis currentAxis, axes) {
			returnString.append(QString("%1\t\t%2\n")
								.arg(AMGCS2Support::axisToCharacter(currentAxis))
								.arg(this->value(currentAxis)));
		}

		return returnString.trimmed();
	}

	/*!
	  * Whether this map contains an entry for the UnknownAxis.
	  */
	bool containsUnknownAxis() const
	{
		return this->contains(AMGCS2::UnknownAxis);
	}
};

template <>
class AMPIC887AxisMap<AMGCS2::PositionUnits> : public QMap<AMGCS2::Axis, AMGCS2::PositionUnits>
{
public:
	/*!
	  * Creates an instance of an axis map, which is empty.
	  */
	AMPIC887AxisMap() {}

	/////////
	// TEMP
	/////////
	AMPIC887AxisMap(const QHash<AMGCS2::Axis, AMGCS2::PositionUnits>& other)
	{
		foreach(AMGCS2::Axis currentAxis, other.keys()) {
			this->value(currentAxis, other.value(currentAxis));
		}
	}

	/*!
	  * Returns a collection of the axes which this map contains.
	  */
	AMPIC887AxisCollection axes() const
	{
		AMPIC887AxisCollection returnCollection(AMPIC887AxisCollection::EmptyCollection);
		QList<AMGCS2::Axis> axisEntries = this->keys();

		foreach(AMGCS2::Axis currentAxisEntry, axisEntries) {
			returnCollection.append(currentAxisEntry);
		}

		return returnCollection;
	}

	/*!
	  * Outputs the map values as a human readable string, if it can parse the
	  * type into a string, otherwise returns the empty string.
	  */
	QString toString(const QString& header = "Data")
	{
		QString returnString = QString("Axis\t\t%1\n").arg(header);

		QList<AMGCS2::Axis> axes = this->keys();
		foreach(AMGCS2::Axis currentAxis, axes) {

			returnString.append(QString("%1\t\t%2\n")
								.arg(AMGCS2Support::axisToCharacter(currentAxis))
								.arg(AMGCS2Support::positionUnitsToString(this->value(currentAxis))));
		}

		return returnString.trimmed();
	}

	/*!
	  * Whether this map contains an entry for the UnknownAxis.
	  */
	bool containsUnknownAxis() const
	{
		return this->contains(AMGCS2::UnknownAxis);
	}
};

#endif // AMPIC887AXISMAP_H
