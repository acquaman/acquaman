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
