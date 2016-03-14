#ifndef AMPIC887AXISMAP_H
#define AMPIC887AXISMAP_H

#include <QMap>
#include <QList>
#include "AMGCS2.h"
#include "AMPIC887AxisCollection.h"
#include "AMGCS2Support.h"
#include "util/AMCArrayHandler.h"
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
	AMPIC887AxisMap()
		: QMap<AMGCS2::Axis, T>()
	{
	}

	/*!
	  * Returns a collection of the axes which this map contains.
	  */
	AMPIC887AxisCollection axes() const
	{
		return AMPIC887AxisCollection(this->keys());
	}

	/*!
	  * Outputs the map values as a human readable string, if it can parse the
	  * type into a string, otherwise returns the empty string.
	  */
	QString toString(const QString& = "Data") const
	{
		// Implemented for known types in partial specializations below.
		return QString();
	}

	/*!
	  * Whether this map contains an entry for the UnknownAxis.
	  */
	bool containsUnknownAxis() const
	{
		return this->contains(AMGCS2::UnknownAxis);
	}

	/*!
	  * Whether this map contains entries for all valid axes
	  */
	bool containsAllAxes() const
	{
		return !containsUnknownAxis() &&
				this->count() == AXIS_COUNT;
	}

	/*!
	  * Whether this map contains an entry for a rotational axis
	  */
	bool containsRotationalAxes() const
	{
		QList<AMGCS2::Axis> axes = this->keys();
		foreach(AMGCS2::Axis currentAxis, axes) {

			if(currentAxis == AMGCS2::UAxis ||
					currentAxis == AMGCS2::VAxis ||
					currentAxis == AMGCS2::WAxis) {

				return true;
			}
		}

		return false;
	}


	/*!
	  * Whether this map contains an entry for a linear axis
	  */
	bool containsLinearAxis() const
	{
		QList<AMGCS2::Axis> axes = this->keys();
		foreach(AMGCS2::Axis currentAxis, axes) {

			if(currentAxis == AMGCS2::XAxis ||
					currentAxis == AMGCS2::YAxis ||
					currentAxis == AMGCS2::ZAxis) {

				return true;
			}
		}

		return false;
	}

	/*!
	  * A map of the provided axes to their values stored within this map. If
	  * a key contained within the provided collection is not contained within
	  * this map it will not be included in the returned map.
	  * \param keys ~ A collection of axes whose values within this map will
	  * be returned.
	  */
	AMPIC887AxisMap<T> matchingValues(const AMPIC887AxisCollection& keys) const
	{
		AMPIC887AxisMap<T> returnValues;
		foreach(AMGCS2::Axis currentAxis, keys) {
			if(this->contains(currentAxis)) {
				returnValues.insert(currentAxis, this->value(currentAxis));
			}
		}

		return returnValues;
	}

};

// Partial Specialization on type T to provide functionality of toString() method
////////////////////////////////////////////////////////////////////////////////

template <>
class AMPIC887AxisMap<bool> : public QMap<AMGCS2::Axis, bool>
{
public:
	/*!
	  * Creates an instance of an axis map, which is empty.
	  */
	AMPIC887AxisMap()
		: QMap<AMGCS2::Axis, bool>()
	{
	}

	/*!
	  * Returns a collection of the axes which this map contains.
	  */
	AMPIC887AxisCollection axes() const
	{
		return AMPIC887AxisCollection(keys());
	}

	/*!
	  * Outputs the map values as a human readable string, if it can parse the
	  * type into a string, otherwise returns the empty string.
	  */
	QString toString(const QString& header = "Data") const
	{
		QString returnString = QString("Axis\t\t%1\n").arg(header);

		QList<AMGCS2::Axis> axes = keys();
		foreach(AMGCS2::Axis currentAxis, axes) {
			QString axisValue;
			if( value(currentAxis)) {
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
		return contains(AMGCS2::UnknownAxis);
	}

	/*!
	  * Whether this map contains entries for all valid axes
	  */
	bool containsAllAxes() const
	{
		return !containsUnknownAxis() &&
				this->count() == AXIS_COUNT;
	}

	/*!
	  * Whether this map contains an entry for a rotational axis
	  */
	bool containsRotationalAxes() const
	{
		QList<AMGCS2::Axis> axes = keys();
		foreach(AMGCS2::Axis currentAxis, axes) {

			if(currentAxis == AMGCS2::UAxis ||
					currentAxis == AMGCS2::VAxis ||
					currentAxis == AMGCS2::WAxis) {

				return true;
			}
		}

		return false;
	}


	/*!
	  * Whether this map contains an entry for a linear axis
	  */
	bool containsLinearAxis() const
	{
		QList<AMGCS2::Axis> axes = keys();
		foreach(AMGCS2::Axis currentAxis, axes) {

			if(currentAxis == AMGCS2::XAxis ||
					currentAxis == AMGCS2::YAxis ||
					currentAxis == AMGCS2::ZAxis) {

				return true;
			}
		}

		return false;
	}

	/*!
	  * A map of the provided axes to their values stored within this map. If
	  * a key contained within the provided collection is not contained within
	  * this map it will not be included in the returned map.
	  * \param keys ~ A collection of axes whose values within this map will
	  * be returned.
	  */
	AMPIC887AxisMap<bool> matchingValues(const AMPIC887AxisCollection& keys) const
	{
		AMPIC887AxisMap<bool> returnValues;
		foreach(AMGCS2::Axis currentAxis, keys) {
			if( contains(currentAxis)) {
				returnValues.insert(currentAxis, value(currentAxis));
			}
		}

		return returnValues;
	}
};

template <>
class AMPIC887AxisMap<double> : public QMap<AMGCS2::Axis, double>
{
public:
	/*!
	  * Creates an instance of an axis map, which is empty.
	  */
	AMPIC887AxisMap()
		: QMap<AMGCS2::Axis, double>()
	{
	}

	/*!
	  * Returns a collection of the axes which this map contains.
	  */
	AMPIC887AxisCollection axes() const
	{
		return AMPIC887AxisCollection(keys());
	}

	/*!
	  * Outputs the map values as a human readable string, if it can parse the
	  * type into a string, otherwise returns the empty string.
	  */
	QString toString(const QString& header = "Data") const
	{
		QString returnString = QString("Axis\t\t%1\n").arg(header);

		QList<AMGCS2::Axis> axes = keys();
		foreach(AMGCS2::Axis currentAxis, axes) {
			returnString.append(QString("%1\t\t%2\n")
								.arg(AMGCS2Support::axisToCharacter(currentAxis))
								.arg( value(currentAxis)));
		}

		return returnString.trimmed();
	}

	/*!
	  * Whether this map contains an entry for the UnknownAxis.
	  */
	bool containsUnknownAxis() const
	{
		return contains(AMGCS2::UnknownAxis);
	}

	/*!
	  * Whether this map contains entries for all valid axes
	  */
	bool containsAllAxes() const
	{
		return !containsUnknownAxis() &&
				this->count() == AXIS_COUNT;
	}

	/*!
	  * Whether this map contains an entry for a rotational axis
	  */
	bool containsRotationalAxes() const
	{
		QList<AMGCS2::Axis> axes = keys();
		foreach(AMGCS2::Axis currentAxis, axes) {

			if(currentAxis == AMGCS2::UAxis ||
					currentAxis == AMGCS2::VAxis ||
					currentAxis == AMGCS2::WAxis) {

				return true;
			}
		}

		return false;
	}


	/*!
	  * Whether this map contains an entry for a linear axis
	  */
	bool containsLinearAxis() const
	{
		QList<AMGCS2::Axis> axes = keys();
		foreach(AMGCS2::Axis currentAxis, axes) {

			if(currentAxis == AMGCS2::XAxis ||
					currentAxis == AMGCS2::YAxis ||
					currentAxis == AMGCS2::ZAxis) {

				return true;
			}
		}

		return false;
	}

	/*!
	  * A map of the provided axes to their values stored within this map. If
	  * a key contained within the provided collection is not contained within
	  * this map it will not be included in the returned map.
	  * \param keys ~ A collection of axes whose values within this map will
	  * be returned.
	  */
	AMPIC887AxisMap<double> matchingValues(const AMPIC887AxisCollection& keys) const
	{
		AMPIC887AxisMap<double> returnValues;
		foreach(AMGCS2::Axis currentAxis, keys) {
			if(contains(currentAxis)) {
				returnValues.insert(currentAxis, value(currentAxis));
			}
		}

		return returnValues;
	}
};

template <>
class AMPIC887AxisMap<AMGCS2::PositionUnits> : public QMap<AMGCS2::Axis, AMGCS2::PositionUnits>
{
public:
	/*!
	  * Creates an instance of an axis map, which is empty.
	  */
	AMPIC887AxisMap()
		: QMap<AMGCS2::Axis, AMGCS2::PositionUnits>()
	{
	}

	/*!
	  * Returns a collection of the axes which this map contains.
	  */
	AMPIC887AxisCollection axes() const
	{
		return AMPIC887AxisCollection(keys());
	}

	/*!
	  * Outputs the map values as a human readable string, if it can parse the
	  * type into a string, otherwise returns the empty string.
	  */
	QString toString(const QString& header = "Data") const
	{
		QString returnString = QString("Axis\t\t%1\n").arg(header);

		QList<AMGCS2::Axis> axes =  keys();
		foreach(AMGCS2::Axis currentAxis, axes) {

			returnString.append(QString("%1\t\t%2\n")
								.arg(AMGCS2Support::axisToCharacter(currentAxis))
								.arg(AMGCS2Support::positionUnitsToString( value(currentAxis))));
		}

		return returnString.trimmed();
	}

	/*!
	  * Whether this map contains an entry for the UnknownAxis.
	  */
	bool containsUnknownAxis() const
	{
		return  contains(AMGCS2::UnknownAxis);
	}

	/*!
	  * Whether this map contains entries for all valid axes
	  */
	bool containsAllAxes() const
	{
		return !containsUnknownAxis() &&
				this->count() == AXIS_COUNT;
	}

	/*!
	  * Whether this map contains an entry for a rotational axis
	  */
	bool containsRotationalAxes() const
	{
		QList<AMGCS2::Axis> axes = keys();
		foreach(AMGCS2::Axis currentAxis, axes) {

			if(currentAxis == AMGCS2::UAxis ||
					currentAxis == AMGCS2::VAxis ||
					currentAxis == AMGCS2::WAxis) {

				return true;
			}
		}

		return false;
	}


	/*!
	  * Whether this map contains an entry for a linear axis
	  */
	bool containsLinearAxis() const
	{
		QList<AMGCS2::Axis> axes = keys();
		foreach(AMGCS2::Axis currentAxis, axes) {

			if(currentAxis == AMGCS2::XAxis ||
					currentAxis == AMGCS2::YAxis ||
					currentAxis == AMGCS2::ZAxis) {

				return true;
			}
		}

		return false;
	}

	/*!
	  * A map of the provided axes to their values stored within this map. If
	  * a key contained within the provided collection is not contained within
	  * this map it will not be included in the returned map.
	  * \param keys ~ A collection of axes whose values within this map will
	  * be returned.
	  */
	AMPIC887AxisMap<AMGCS2::PositionUnits> matchingValues(const AMPIC887AxisCollection& keys) const
	{
		AMPIC887AxisMap<AMGCS2::PositionUnits> returnValues;
		foreach(AMGCS2::Axis currentAxis, keys) {
			if( contains(currentAxis)) {
				returnValues.insert(currentAxis,  value(currentAxis));
			}
		}

		return returnValues;
	}
};

#endif // AMPIC887AXISMAP_H
