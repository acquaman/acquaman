#include "AMUtility.h"

AMRange AMUtility::rangeFinder(const QVector<double> &data)
{
	AMRange range = AMRange();

	if (!data.isEmpty()){

		double minimum = data.first();
		double maximum = minimum;

		for (int i = 1, size = data.size(); i < size; i++){

			double value = data.at(i);

			if (value < minimum)
				minimum = value;

			if (value > maximum)
				maximum = value;
		}

		range.setRange(minimum, maximum);
	}

	return range;
}

double AMUtility::minimumFinder(const QVector<double> &data)
{
	double minimum = 0;

	if (!data.isEmpty()){

		minimum = data.first();

		for (int i = 1, size = data.size(); i < size; i++){

			double value = data.at(i);

			if (value < minimum)
				minimum = value;
		}
	}

	return minimum;
}

double AMUtility::maximumFinder(const QVector<double> &data)
{
	double maximum = 0;

	if (!data.isEmpty()){

		maximum = data.first();

		for (int i = 1, size = data.size(); i < size; i++){

			double value = data.at(i);

			if (value > maximum)
				maximum = value;
		}
	}

	return maximum;
}

int AMUtility::flatIndexOfMinimum(const QVector<double> &data)
{
	int result = -1;

	if (data.count() > 0) {

		double minValue = data.at(0);
		int minIndex = -1;

		// Iterate through the entries in data to find the smallest
		// value and the corresponding index.

		for (int i = 1, count = data.count(); i < count; i++) {
			double value = data.at(i);

			if (value < minValue) {
				minValue = value;
				minIndex = i;
			}
		}

		result = minIndex;
	}

	return result;
}

int AMUtility::flatIndexOfMaximum(const QVector<double> &data)
{
	int result = -1;

	if (data.count() > 0) {

		double maxValue = data.at(0);
		int maxIndex = -1;

		// Iterate through the entries in data to find the largest
		// value and the corresponding index.

		for (int i = 1, count = data.count(); i < count; i++) {
			double value = data.at(i);

			if (value > maxValue) {
				maxValue = value;
				maxIndex = i;
			}
		}

		result = maxIndex;
	}

	return result;
}

AMnDIndex AMUtility::indexOfMinimum(const QVector<double> &data, AMnDIndex size)
{
	AMnDIndex result = AMnDIndex();

	// Identify the flat index for the minimum value.

	int minIndex = flatIndexOfMinimum(data);

	// Find the corresponding AMnDIndex, given the size.

	if (minIndex > -1 && size.isValid())
		result = AMnDIndex::fromFlatIndexInArrayOfSize(size, minIndex);

	return result;
}

AMnDIndex AMUtility::indexOfMaximum(const QVector<double> &data, AMnDIndex size)
{
	AMnDIndex result = AMnDIndex();

	// Identify the flat index for the minimum value.

	int maxIndex = flatIndexOfMaximum(data);

	// Find the corresponding AMnDIndex, given the size.

	if (maxIndex > -1 && size.isValid())
		result = AMnDIndex::fromFlatIndexInArrayOfSize(size, maxIndex);

	return result;
}

AMRange AMUtility::rangeFinder(const QVector<double> &data, double valueToIgnore)
{
	AMRange range = AMRange();

	if (!data.isEmpty()){

		double minimum = data.first();
		double maximum = minimum;

		for (int i = 1, size = data.size(); i < size; i++){

			double value = data.at(i);

			if (value < minimum && value != valueToIgnore)
				minimum = value;

			if (value > maximum && value != valueToIgnore)
				maximum = value;
		}

		range.setRange(minimum, maximum);
	}

	return range;
}

double AMUtility::minimumFinder(const QVector<double> &data, double valueToIgnore)
{
	double minimum = 0;

	if (!data.isEmpty()){

		minimum = data.first();

		for (int i = 1, size = data.size(); i < size; i++){

			double value = data.at(i);

			if (value < minimum && value != valueToIgnore)
				minimum = value;
		}
	}

	return minimum;
}

double AMUtility::maximumFinder(const QVector<double> &data, double valueToIgnore)
{
	double maximum = 0;

	if (!data.isEmpty()){

		maximum = data.first();

		for (int i = 1, size = data.size(); i < size; i++){

			double value = data.at(i);

			if (value > maximum && value != valueToIgnore)
				maximum = value;
		}
	}

	return maximum;
}
