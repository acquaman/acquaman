#ifndef AMTESTIMAGEDATA_H
#define AMTESTIMAGEDATA_H

#include "MPlot/MPlotImageData.h"

#include "util/TimeEvolutionStripTool/AMTESTRingBuffer.h"

/// This encapsuates the data to be visualized by the strip tool view.  Internally, it holds a ring buffer for easy handling.
class AMTESTImageData : public MPlotAbstractImageData
{
public:
	/// Constructor.
	/*!
	 * \param dataSize specifies the size of the ring buffer.
	 * \param axisSize defines the size of the y-axis.
	 */
	AMTESTImageData(int dataSize, int updateRate, int axisSize);
	/// Destructor.
	virtual ~AMTESTImageData();

	/// Return the x (independent data value) corresponding to \c indexX.
	virtual qreal x(int index) const;
	/// Return the y (independendent data value) corresponding to \c indexY.
	virtual qreal y(int index) const;
	/// Return the z = f(x,y) dependent data value corresponding (\c indexX, \c indexY). Can assume (\c indexX, \c indexY) are valid.
	virtual qreal z(int xIndex, int yIndex) const;
	/// Copy an entire block of z = f(x,y) values from (xStart,yStart) to (xEnd,yEnd) inclusive, into \c outputValues. The data is copied in row-major order, ie: with the x-axis varying the slowest. (Can assume \c outputValues has enough room to hold all the values, that (xStart,yStart) <= (xEnd,yEnd), and that the indexes are not out of range.)
	virtual void zValues(int xStart, int yStart, int xEnd, int yEnd, qreal* outputValues) const;

	/// Return the number of elements in x and y
	virtual QPoint count() const;
	/// Return the bounds of the data (the rectangle containing the max/min x- and y-values)
	virtual QRectF boundingRect() const;

	/// Adds data to the series.
	void addData(const QVector<qreal> &data);
	/// Clears the data from the ring buffer and resets the x-axis values.
	void clear();

protected:
	/// Holds the x-values for the image data.
	QVector<qreal> xAxis_;
	/// Holds the y-values for the image data.
	QVector<qreal> yAxis_;
	/// Holds the z-values for the image data.
	AMTESTRingBuffer zAxis_;
	/// Holds the time step per point (e.g.: 1 for 1 second)
	qreal updateRate_;
	/// Holds the bounding rectangle.
	QRectF boundingRect_;
};

#endif // AMTESTIMAGEDATA_H
