#ifndef AMSCALERCOUNTANALYSER_H
#define AMSCALERCOUNTANALYSER_H

#include <QVector>
#include <QPair>
/*!
 * A class which identified motion periods within scaler count data. The analyser
 * makes some assumptions about this data:
 *    - When motion stops the counts will fall below the noise threshold.
 *    - Each period of motion will be characterised by a series of consecutive
 *      counts, with no zero count elements within them.
 *    - Two periods of motion will always contain at least one zero count element
 *      between them.
 */
class AMScalerCountAnalyser
{
public:
	/*!
	* Creates a new instance of a scaler count analyser which
	* will analyse the passed vector containing scaler counts.
	* \param scalerCounts ~ The container of scaler counts which will be analysed.
	* \param countNoiseThreshold ~ The value below which an element of counts will
	* be considered not moving.
	* \param countChangeThreshold ~ The value above which the difference between
	* two elements will be considered a transition.
	*/
	AMScalerCountAnalyser(const QVector<double>& scalerCounts,
	                      double countNoiseThreshold,
	                      double countChangeThreshold);

	/*!
	  * A string representation of the analysed results
	  */
	QString toString() const;

	/*!
	  * A vector containg the motion periods identified by the analyser. Each
	  * pair contained has its first element as the start index in the scaler count
	  * data of a period of motion, and its second element as the end index of the
	  * same motion. The motions appear in the vector in the order which they occured
	  * in the originally passed scaler data.
	  */
	QVector<QPair<int, int> > motionPeriods() const;
protected:

	/*!
	  * Helper method which performs analyses the count data in order
	  * to identify tranition points
	  */
	void locateTransitions();

	/*!
	  * Helper method which takes the vector of transition points and makes
	  * them into periods of motion, with a start and end.
	  */
	void locateMotionPeriods();

	/// The raw scaler counts
	QVector<double> scalerCounts_;

	/// The derived points of transition within the counts.
	QVector<int> transitionIndices_;

	/// A list of the start and end periods of motion within the scaler counts
	QVector< QPair<int, int> > motionPeriods_;

	/// The number of counts below which is considered a non moving state
	double countNoiseThreshold_;

	/// The threshold which represents the change between two consecutive periods
	/// of counts which will be considered a transition.
	double countChangeThreshold_;
};

#endif // AMSCALERCOUNTANALYSER_H
