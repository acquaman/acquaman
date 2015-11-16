#include "AMScalerCountAnalyser.h"
#include <QStringList>

AMScalerCountAnalyser::AMScalerCountAnalyser(const QVector<double>& scalerCounts,
                                             double countNoiseThreshold,
					                         double countChangeThreshold)
{
	scalerCounts_ = scalerCounts;
	countNoiseThreshold_ = countNoiseThreshold;
	countChangeThreshold_ = countChangeThreshold;
	locateTransitions();
	locateMotionPeriods();
}

QString AMScalerCountAnalyser::toString()
{
	QStringList elementsList;

	for(int iRawScalerData = 0, rawDataCount = scalerCounts_.count();
	    iRawScalerData < rawDataCount;
	    ++iRawScalerData) {

		elementsList.append(QString("%1\t%2").arg(iRawScalerData).arg(scalerCounts_.at(iRawScalerData)));
	}

	foreach(int transitionIndex, transitionIndices_) {

		elementsList[transitionIndex].prepend("*");
	}

	for(int iMotionPeriod = 0, count = motionPeriods_.count();
	    iMotionPeriod < count;
	    ++iMotionPeriod) {

		elementsList[motionPeriods_.at(iMotionPeriod).first].append("\tStart");
		elementsList[motionPeriods_.at(iMotionPeriod).second].append("\tEnd");
	}

	QString finalString = QString("Index\tValue\n");
	foreach(QString indexString, elementsList) {

		finalString.append(indexString + "\n");
	}

	return finalString;
}


QVector<QPair<int, int> > AMScalerCountAnalyser::motionPeriods() const
{
	return motionPeriods_;
}

void AMScalerCountAnalyser::locateTransitions()
{

	int previousCountDifference = 0;

	for (int iScalerCount = 1, totalCounts = scalerCounts_.count(); iScalerCount < totalCounts; ++iScalerCount) {

		int currentCountDifference = scalerCounts_.at(iScalerCount - 1) - scalerCounts_.at(iScalerCount);

		if(iScalerCount == 1 && scalerCounts_.at(0) > countNoiseThreshold_ ) {
			// The first element is above noise tolerance, mark it as a transition

			transitionIndices_.append(0);

		} else if(qAbs(currentCountDifference) > countChangeThreshold_) {
			// We are not in a steady state.

			if(scalerCounts_.at(iScalerCount) < countNoiseThreshold_) {
				// We've just hit a stop point.

				transitionIndices_.append(iScalerCount);

			} else if(previousCountDifference < countNoiseThreshold_) {
				// The non steady state is new

				transitionIndices_.append(iScalerCount);

			} else {
				// We were already in a non steady state.

				if(previousCountDifference * currentCountDifference < 0) {
					// We've just gone from increasing to decreasing, or vice versa.

					transitionIndices_.append(iScalerCount);
				}
			}


		} else {
			// We are in a steady state

			if(qAbs(previousCountDifference) > countNoiseThreshold_) {
				// The steady state is new

				transitionIndices_.append(iScalerCount);
			}

		}

		previousCountDifference = currentCountDifference;
	}
}

void AMScalerCountAnalyser::locateMotionPeriods()
{
	int currentMotionStart = -1;

	for(int iTransition = 0, transitionCount = transitionIndices_.count();
	    iTransition < transitionCount; ++iTransition) {

		int currentTransitionIndex = transitionIndices_.at(iTransition);

		if(currentTransitionIndex == 0) {
			// The frist index is a transition. It must, therefore, be a start point

			currentMotionStart = currentTransitionIndex;

		} else if(scalerCounts_.at(currentTransitionIndex - 1) < countNoiseThreshold_) {
			// Our previous point was below the noise threshold. We are definitely
			// a start point, but we might also be the end of a previous motion.

			if(currentMotionStart != -1 && currentMotionStart != currentTransitionIndex-1) {
				// We are looking for an end, this must be it (or rather, this -1)
				motionPeriods_.append(QPair<int, int>(currentMotionStart, currentTransitionIndex -1));
				currentMotionStart = -1;
			}

			currentMotionStart = currentTransitionIndex;

		} else if(currentTransitionIndex == scalerCounts_.count() - 1) {

			if(currentMotionStart != -1) {
				// We're at the end, but we're still waiting for an end point. This must be it

				motionPeriods_.append(QPair<int, int>(currentMotionStart, currentTransitionIndex));
				currentMotionStart = -1;
			}
		}
	}
}


