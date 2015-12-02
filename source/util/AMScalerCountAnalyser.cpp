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

QString AMScalerCountAnalyser::toString() const
{

	QString results;

	for(int iPeriodOfInterest = 0, periodCount = periodsOfInterest_.count();
	    iPeriodOfInterest < periodCount;
	    ++iPeriodOfInterest) {

		QPair<int, int> currentPeriod = periodsOfInterest_.at(iPeriodOfInterest);
		results.append(QString("Start: %1, End: %2\n")
		               .arg(currentPeriod.first)
		               .arg(currentPeriod.second));
	}

	return results;
}


QList<QPair<int, int> > AMScalerCountAnalyser::periodsOfInterest() const
{
	return periodsOfInterest_;
}

void AMScalerCountAnalyser::locatePeriodsOfInterest()
{

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
			}

		} else {
			// We're looking for an end

			if(currentValue <= noiseThreshold_) {
				// We've dipped below the threshold

				if(potentialEnd == -1) {
					// This is our first element below noise level
					potentialEnd = iRawData;
				}
				++currentNumberOfConsecutiveCounts;


			} else if(currentNumberOfConsecutiveCounts > 0){

				currentNumberOfConsecutiveCounts = 0;
				potentialEnd = -1;
			}

			if(currentNumberOfConsecutiveCounts >= requiredConsecutivePeriods_) {
				// It's been long enough to consider this an end

				periodsOfInterest_.append(QPair<int,int>(startIndex, potentialEnd));

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

}


