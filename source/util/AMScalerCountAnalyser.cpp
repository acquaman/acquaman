#include "AMScalerCountAnalyser.h"
#include <QStringList>

AMScalerCountAnalyser::AMScalerCountAnalyser(const QVector<double>& scalerData,
					     double noiseThreshold,
					     int requiredConsecutivePeriods)
{
	scalerData_ = scalerData;
	noiseThreshold_ = noiseThreshold;
	requiredConsecutivePeriods_ = requiredConsecutivePeriods;

	locatePeriodsOfInterest();
}
#include <QDebug>
QString AMScalerCountAnalyser::toString() const
{
	qDebug() << "toString() ===================================";
	QStringList elementStrings;

	for (int iRawData = 0, rawDataCount = scalerData_.count();
	     iRawData < rawDataCount;
	     ++iRawData) {

		elementStrings.append(QString("%1\t%2")
				      .arg(iRawData)
				      .arg(scalerData_.at(iRawData)));
	}

	qDebug() << "Interest periods count " << periodsOfInterest_.count();
	for(int iInterestPeriod = 0, periodCount = periodsOfInterest_.count();
	    iInterestPeriod < periodCount;
	    ++iInterestPeriod) {


		QPair<int, int> interestPeriod = periodsOfInterest_.at(iInterestPeriod);

		qDebug() << "Pair index "<<iInterestPeriod << ":" << interestPeriod.first << ", " << interestPeriod.second;

		QString periodStart = elementStrings.at(interestPeriod.first);
		qDebug() << "Start was" << periodStart;
		periodStart = periodStart.append("\tStart");
		elementStrings[interestPeriod.first] = periodStart;
		qDebug() << "String made to" << periodStart;
		qDebug() << "List entry: " << elementStrings[interestPeriod.first];

		QString periodEnd = elementStrings.at(interestPeriod.second);
		periodEnd = periodEnd.append("\tEnd");
		elementStrings[interestPeriod.second] = periodEnd;


	}

	QString finalString;

	foreach(QString elementString, elementStrings) {

		finalString.append(QString("%1\n").arg(elementString));
	}

	return finalString;
}


QList<QPair<int, int> > AMScalerCountAnalyser::periodsOfInterest() const
{
	return periodsOfInterest_;
}

void AMScalerCountAnalyser::locatePeriodsOfInterest()
{
	qDebug() << "Threshold = " << noiseThreshold_;
	qDebug() << "Required End Counts = " << requiredConsecutivePeriods_;

	int currentNumberOfConsecutiveCounts = 0;
	int startIndex = -1;
	int potentialEnd = -1;

	for(int iRawData = 0, rawDataCount = scalerData_.count();
	    iRawData < rawDataCount - 1;
	    ++iRawData) {

		double currentValue = scalerData_.at(iRawData);

		if(startIndex == -1) {
			// We're looking for a start point

			if(currentValue > noiseThreshold_) {
				// This is a start

				startIndex = iRawData;
				qDebug() << "Found a start at" << startIndex;
			}

		} else {
			// We're looking for an end

			if(currentValue <= noiseThreshold_) {
				// We've dipped below the threshold

				if(potentialEnd == -1) {
					// This is our first element below noise level
					potentialEnd = iRawData;
					qDebug() << "Found a potential end at" << potentialEnd;
				}
				++currentNumberOfConsecutiveCounts;
				qDebug() << "\tBelow noise" << currentNumberOfConsecutiveCounts << "times";


			} else if(currentNumberOfConsecutiveCounts > 0){

				currentNumberOfConsecutiveCounts = 0;
				potentialEnd = -1;
				qDebug() << "Back above noise";
			}

			if(currentNumberOfConsecutiveCounts >= requiredConsecutivePeriods_) {
				// It's been long enough to consider this an end

				periodsOfInterest_.append(QPair<int,int>(startIndex, potentialEnd));

				qDebug() << "\n==== Found Period from" << startIndex << "to" << potentialEnd;
				startIndex = -1;
				potentialEnd = -1;
				currentNumberOfConsecutiveCounts = 0;
			}
		}

	}

	// We consider the last point separately. If we're still looking for an
	// end, make it this.
	if(startIndex != -1) {
		periodsOfInterest_.append(QPair<int, int>(startIndex, scalerData_.count() - 1));
	}

	qDebug() << "Number of periods located = " <<periodsOfInterest_.count();
}


