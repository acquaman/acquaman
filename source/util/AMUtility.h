#ifndef AMUTILITY_H
#define AMUTILITY_H

#include <QVector>

#include "util/AMRange.h"
#include "dataman/AMnDIndex.h"

class AMUtility
{
public:
	/// This is a helper static method that goes through the provided data and returns the minimum and maximum in an AMRange.  If data is empty or null then a null AMRange is returned.
	static AMRange rangeFinder(const QVector<double> &data);
	/// This is a helper static method that goes through the provided data and returns the minimum value.  If data is empty or null then 0 is returned.
	static double minimumFinder(const QVector<double> &data);
	/// This is a helper static method that goes through the provided data and returns the maximum value.  If data is empyt or null then 0 is returned.
	static double maximumFinder(const QVector<double> &data);

	/// This is a helper static method that goes through the provided data and returns the index for the minimum value found. If data is empty or null then returns -1.
	static int indexOfMinimum(const QVector<double> &data);
	/// This is a helper static method that goes through the provided data and returns the index for the maximum value found. If data is empty or null then returns -1.
	static int indexOfMaximum(const QVector<double> &data);
	/// This is a helper static method that goes through the provided data and returns the AMnDIndex for the minimum value found. If data is empty or null then a null AMnDIndex is returned.
	static AMnDIndex indexOfMinimum(const QVector<double> &data, AMnDIndex size);
	/// This is a helper static method that goes through the provided data and returns the AMnDIndex for the maximum value found. If data is empty or null then a null AMnDIndex is returned.
	static AMnDIndex indexOfMaximum(const QVector<double> &data, AMnDIndex size);

	/// This is a helper static method that goes through the provided data and returns the minimum and maximum in an AMRange.  If data is empty or null then a null AMRange is returned.
	static AMRange rangeFinder(const QVector<double> &data, double valueToIgnore);
	/// This is a helper static method that goes through the provided data and returns the minimum value.  If data is empty or null then 0 is returned.
	static double minimumFinder(const QVector<double> &data, double valueToIgnore);
	/// This is a helper static method that goes through the provided data and returns the maximum value.  If data is empyt or null then 0 is returned.
	static double maximumFinder(const QVector<double> &data, double valueToIgnore);
};

#endif // AMUTILITY_H
