#include "AMUtility.h"
#include <math.h>
#include <QDebug>
AMRange AMUtility::rangeFinder(const QVector<double> &data)
{
	AMRange range = AMRange();

	if (!data.isEmpty() && !isnan(data.first()) && !isinf(data.first())){

		double minimum = data.first();
		double maximum = minimum;

		qDebug() << "\nData first:" << data.first();
		qDebug() << "Data size:" << data.size();

		for (int i = 1, size = data.size(); i < size; i++){

		    if (!isnan(data.at(i)) && !isinf(data.at(i))) {
			double value = data.at(i);

			if (value < minimum)
				minimum = value;

			if (value > maximum)
				maximum = value;
		    }
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

AMRange AMUtility::rangeFinder(const QVector<double> &data, double valueToIgnore)
{
	AMRange range = AMRange();

	if (!data.isEmpty() && !isnan(data.first()) && !isinf(data.first())){

		double minimum = data.first();
		double maximum = minimum;

		for (int i = 1, size = data.size(); i < size; i++){

		    if (!isnan(data.at(i)) && !isinf(data.at(i))) {

			double value = data.at(i);

			if (value < minimum && value != valueToIgnore)
				minimum = value;

			if (value > maximum && value != valueToIgnore)
				maximum = value;
		    }
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
