#ifndef AMSCALERCOUNTANALYSER_H
#define AMSCALERCOUNTANALYSER_H

#define SCALER_COUNT_NOISE_THRESHOLD 2 // The value below which is considered a non moving state
#define SCALER_COUNT_NOISE_CHANGE_THRESHOLD 2 // The value below which the difference between two counts makes them steady
#include <QVector>
#include <QPair>
/*!
  * A class which idenfied points of transition in vectors containg scaler count data.
  * transition points may include:
  *  - Going from a low steady count to a high steady count
  *  - Going from a high steady count to a low steady count
  *  - Going from a consistently increading count to a consistently decreasing count
  *  - Going from a consistently decreasing count to a consistently increasing count
  */
class AMScalerCountAnalyser
{
public:
	/*!
	* Creates a new instance of a scaler count analyser which
	* will analyse the passed vector containing scaler counts.
	*/
	AMScalerCountAnalyser(const QVector<int>& scalerCounts);

	/*!
	  * The indices of the points within the scaler counts at which
	  * a point of transition was found.
	  */
	const QVector<int> transitionIndices() const;

	/*!
	  * The indices of the points within the scaler counts at which
	  * a point of transition was found.
	  */
	QVector<int> transitionIndices();

	/*!
	  * A string representation of the analysed results
	  */
	QString toString();
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
	QVector<int> scalerCounts_;

	/// The derived points of transition within the counts.
	QVector<int> transitionIndices_;

	/// A list of the start and end periods of motion within the scaler counts
	QVector< QPair<int, int> > motionPeriods_;
};

#endif // AMSCALERCOUNTANALYSER_H
