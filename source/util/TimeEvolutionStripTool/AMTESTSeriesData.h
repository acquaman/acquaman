#ifndef AMTESTSERIESDATA_H
#define AMTESTSERIESDATA_H

#include "MPlot/MPlotSeriesData.h"

#include "util/TimeEvolutionStripTool/AMTESTRingBuffer.h"

/// This encapsulates the data to be visualized by the strip tool view.  Internally, it holds a ring buffer for easy handling.
class AMTESTSeriesData : public MPlotAbstractSeriesData
{
public:
	/// Constructor.  \param dataSize specifies the size of the ring buffer.
	AMTESTSeriesData(int dataSize, int updateRate);
	/// Destructor.
	virtual ~AMTESTSeriesData();

	/// Return the x-value at index.  You can assume that \c index is valid (< count()).
	virtual qreal x(unsigned index) const;
	/// Copy all the x values from \c indexStart to \c indexEnd (inclusive) into \c outputValues. You can assume that the indexes are valid.
	virtual void xValues(unsigned indexStart, unsigned indexEnd, qreal* outputValues) const;

	/// Return the y-value at index.  You can assume that \c index is valid (< count()).
	virtual qreal y(unsigned index) const;
	/// Copy all the y values from \c indexStart to \c indexEnd (inclusive) into \c outputValues.  You can assume that the indexes are valid.
	virtual void yValues(unsigned indexStart, unsigned indexEnd, qreal* outputValues) const;

	/// Return the number of data points.
	virtual int count() const;

	/// Adds data to the series.
	void addData(const QVector<qreal> &data);
	/// Clears the data from the ring buffer and resets the x-axis values.
	void clear();

protected:
	/// Holds the x-values for the series data.
	QVector<qreal> xAxis_;
	/// Holds the y-values for the series data.
	AMTESTRingBuffer yAxis_;
	/// Holds the time step per point (e.g.: 1 for 1 second)
	qreal updateRate_;
};

#endif // AMTESTSERIESDATA_H
