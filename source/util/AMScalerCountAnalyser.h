#ifndef AMSCALERCOUNTANALYSER_H
#define AMSCALERCOUNTANALYSER_H

#include <QVector>
#include <QPair>

/*!
  * A class which is designed to analyse scaler count data for periods of interest.
  * Such periods are defined as periods in which the counts changes from a low
  * amount to a higher amount above some noise threshold.
  */
class AMScalerCountAnalyser
{
public:
	/*!
	* Creates a new instance of a scaler count analyser which
	* will analyse the passed vector containing scaler data.
	* \param scalerCounts ~ The container of scaler data which will be analysed.
	* \param countNoiseThreshold ~ The value below which an element of counts will
	* be considered not moving.
	* \param requiredConsecutivePeriods ~ The number of periods during which the
	* data has moved below the noise theshold to consider a period of interest
	* over.
	*/
	AMScalerCountAnalyser(const QVector<double>& scalerData,
			      double noiseThreshold,
			      int requiredConsecutivePeriods);

	/*!
	  * A string representation of the analysed results
	  */
	QString toString() const;

	/*!
	  * A vector containg the interest periods identified by the analyser. Each
	  * pair contained has its first element as the start index in the scaler count
	  * data of a period of interest, and its second element as the end index of the
	  * same period. The periods appear in the vector in the order which they occured
	  * in the scaler data analysed.
	  */
	QList<QPair<int, int> > periodsOfInterest() const;
protected:

	/*!
	  * Helper method which takes the vector of transition points and makes
	  * them into periods of motion, with a start and end.
	  */
	void locatePeriodsOfInterest();

	/// The raw scaler data
	QVector<double> scalerData_;

	/// A list of the start and end periods of interest within the scaler data
	QList< QPair<int, int> > periodsOfInterest_;

	/// The value below which an element of the scaler data is considered noise
	double noiseThreshold_;

	/// The required number of periods which a change in the data must occur for
	/// to cease a period.
	int requiredConsecutivePeriods_;
};

#endif // AMSCALERCOUNTANALYSER_H
