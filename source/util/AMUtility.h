#ifndef AMUTILITY_H
#define AMUTILITY_H

#include <QVector>

#include "util/AMRange.h"

class AMUtility
{
public:
	/// This is a helper static method that goes through the provided data and returns the minimum and maximum in an AMRange.  If data is empty or null then a null AMRange is returned.
	static AMRange rangeFinder(const QVector<double> &data);
	/// This is a helper static method that goes through the provided data and returns the minimum value.  If data is empty or null then 0 is returned.
	static double minimumFinder(const QVector<double> &data);
	/// This is a helper static method that goes through the provided data and returns the maximum value.  If data is empyt or null then 0 is returned.
	static double maximumFinder(const QVector<double> &data);

	/// This is a helper static method that goes through the provided data and returns the minimum and maximum in an AMRange.  If data is empty or null then a null AMRange is returned.
	static AMRange rangeFinder(const QVector<double> &data, double valueToIgnore);
	/// This is a helper static method that goes through the provided data and returns the minimum value.  If data is empty or null then 0 is returned.
	static double minimumFinder(const QVector<double> &data, double valueToIgnore);
	/// This is a helper static method that goes through the provided data and returns the maximum value.  If data is empyt or null then 0 is returned.
	static double maximumFinder(const QVector<double> &data, double valueToIgnore);
};

#endif // AMUTILITY_H
