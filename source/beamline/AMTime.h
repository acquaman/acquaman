#ifndef AMTIME_H
#define AMTIME_H

#include <QString>

class AMTime
{

public:

	/// Enum describing the available time units.
	enum Units { mSeconds = 0, Seconds, Minutes, Hours };

	/// Returns a string representation of the given time units.
	static QString unitsToString(const AMTime::Units &units)
	{
		QString result;

		switch (int(units)) {
		case AMTime::mSeconds:
			result = "mSec";
			break;
		case AMTime::Seconds:
			result = "sec";
			break;
		case AMTime::Minutes:
			result = "min";
			break;
		case AMTime::Hours:
			result = "hr";
			break;
		default:
			break;
		}

		return result;
	}

	/// Returns the time value converted from the given time to the given units.
	static double convert(double initialValue, const AMTime::Units &initialUnits, const AMTime::Units &finalUnits)
	{
		double seconds = 0.0;
		double result = 0.0;

		switch (int(initialUnits)) {
		case AMTime::mSeconds:
			seconds = initialValue / 1000.0;
			break;
		case AMTime::Seconds:
			seconds = initialValue;
			break;
		case AMTime::Minutes:
			seconds = initialValue * 60.0;
			break;
		case AMTime::Hours:
			seconds = initialValue * 60.0 * 60.0;
			break;
		default:
			break;
		}

		switch (int(finalUnits)) {
		case AMTime::mSeconds:
			result = seconds * 1000.0;
			break;
		case AMTime::Seconds:
			result = seconds;
			break;
		case AMTime::Minutes:
			result = seconds / 60.0;
			break;
		case AMTime::Hours:
			result = seconds / 60.0 / 60.0;
			break;
		default:
			break;
		}

		return result;
	}
};

#endif // AMTIME_H
