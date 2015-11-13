#include "AMScalerCountAnalyser.h"


AMScalerCountAnalyser::AMScalerCountAnalyser(const QVector<int>& scalerCounts)
{
	scalerCounts_ = scalerCounts;
	locateTransitions();
	locateMotionPeriods();
}

const QVector<int> AMScalerCountAnalyser::transitionIndices() const
{
	return transitionIndices_;
}

QVector<int> AMScalerCountAnalyser::transitionIndices()
{
	return transitionIndices_;
}

QString AMScalerCountAnalyser::toString()
{
	QString outputString;

	int nextTransitionIndex = 0;
	int nextMotionPeriodIndex = 0;

	for(int iRawScalerIndex = 0, scalerDataCount = scalerCounts_.count();
	    iRawScalerIndex < scalerDataCount;
	    ++iRawScalerIndex) {

		if(transitionIndices_.at(nextTransitionIndex) == iRawScalerIndex) {

			if(motionPeriods_.at(nextMotionPeriodIndex).first == iRawScalerIndex) {

				outputString.append(QString("%1\t%2\tStart").arg(iRawScalerIndex).arg(scalerCounts_.at(iRawScalerIndex));
			} else if(motionPeriods_.at(nextMotionPeriodIndex).second == iRawScalerIndex) {

				outputString.append(QString("%1\t%2\tEnd").arg(iRawScalerIndex).arg(scalerCounts_.at(iRawScalerIndex));
				++nextMotionPeriodIndex;
			}


			++nextTransitionIndex;
		} else {

			outputString.append(QString("%1\t%2").arg(iRawScalerIndex).arg(scalerCounts_.at(iRawScalerIndex)));
		}
	}

	return outputString;
}

void AMScalerCountAnalyser::locateTransitions()
{

	int previousCountDifference = 0;

	for (int iScalerCount = 1, totalCounts = scalerCounts_.count(); iScalerCount < totalCounts; ++iScalerCount) {

		int currentCountDifference = scalerCounts_.at(iScalerCount - 1) - scalerCounts_.at(iScalerCount);

		if(qAbs(currentCountDifference) > SCALER_COUNT_NOISE_CHANGE_THRESHOLD) {
			// We are not in a steady state.

			if(previousCountDifference == 0) {
				// The non steady state is new

				transitionIndices_.append(iScalerCount);

			} else {
				// We were already in a non steady state.

				if(previousCountDifference * currentCountDifference < 0) {
					// We've just gone from increasing to decreasing, or vice versa.

					transitionIndices_.append(iScalerCount);
				}
			}

			previousCountDifference = currentCountDifference;
		} else {
			// We are in a steady state

			if(previousCountDifference != 0) {
				// The steady state is new

				transitionIndices_.append(iScalerCount);
			}

			previousCountDifference = 0;
		}
	}
}

void AMScalerCountAnalyser::locateMotionPeriods()
{
	int currentMotionStart = -1;
	int currentMotionEnd = -1;

	for(int iTransition = 0, transitionCount = transitionIndices_.count();
	    iTransition < transitionCount; ++iTransition) {

		int currentTransitionIndex = transitionIndices_.at(iTransition);

		if(currentTransitionIndex == 0) {
			// This is the first index. It must, therefore, be a start point

			currentMotionStart = currentTransitionIndex;
		} else if(currentTransitionIndex == scalerCounts_.count() - 1) {

			if(currentMotionStart != -1) {
				// We're at the end, and we're waiting for an end point. This must be it

				currentMotionEnd = currentTransitionIndex;
			}

		} else {

			if(scalerCounts_.at(currentTransitionIndex - 1) < SCALER_COUNT_NOISE_THRESHOLD) {
				// Our previous point was below the noise threshold. We might be a start or end at
				// this point

				if(currentMotionStart == -1) {
					// Looking for a start, this must be it.

					currentMotionStart = currentTransitionIndex;
				} else {

					// Looking for an end, this must be it.
					currentMotionEnd = currentTransitionIndex;
				}
			}
		}

		if(currentMotionStart != -1 && currentMotionEnd != -1) {
			// We've identified a start and end. Add it to the list and reset the values

			motionPeriods_.append(QPair<int, int>(currentMotionStart, currentMotionEnd));
			currentMotionStart = -1;
			currentMotionEnd = -1;
		}

	}
}

