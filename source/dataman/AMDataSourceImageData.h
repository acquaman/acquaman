#ifndef AMDATASOURCEIMAGEDATA_H
#define AMDATASOURCEIMAGEDATA_H

#include <QObject>
#include "MPlot/MPlotImageData.h"
#include "dataman/AMDataSource.h"

/// This class wraps any AMDataSource for use as 2-dimensional (XY scatter) series data.  The rank() of the underlying data source must be 2; if the dimensionality is not correct, the wrapper will report a count() of 0.  It only supports data sources with uniform axis scales for now.
class AMDataSourceImageData : public QObject, public MPlotAbstractImageData
{
	Q_OBJECT
public:
	/// Constructor. \c dataSource is the source to represent as Z=f(X,Y) image data.
	AMDataSourceImageData(const AMDataSource* dataSource, QObject* parent = 0);

	/// Call this to switch to representing a different data source
	void setDataSource(const AMDataSource* dataSource);

	/// Access the underlying data source
	const inline AMDataSource* dataSource() const { return source_; }


	/// Return the x (data value) corresponding an (x,y) \c index:
	virtual inline double x(int indexX) const { return source_->axisValue(0, indexX); }
	/// Return the y (data value) corresponding an (x,y) \c index:
	virtual inline double y(int indexY) const { return source_->axisValue(1, indexY); }
	/// Return the z = f(x,y) value corresponding an (x,y) \c index:
	virtual inline double z(int xIndex, int yIndex) const { return source_->value(AMnDIndex(xIndex, yIndex)); }


	/// Return the number of elements in x and y
	virtual inline QPoint count() const { if(isValid_) return QPoint(source_->size(0), source_->size(1)); else return QPoint(0,0); }

	/// Return the extent of the data. We assume the axis values are ordered... is this a valid assumption?
	virtual inline QRectF boundingRect() const {
		int sizeX = source_->size(0);
		int sizeY = source_->size(1);

		if(sizeX == 0 || sizeY == 0)
			return QRectF();

		double minX = source_->axisValue(0, 0);
		double maxX = source_->axisValue(0, sizeX-1);
		double minY = source_->axisValue(1, 0);
		double maxY = source_->axisValue(1, sizeY-1);

		if(maxX < minX)
			qSwap(minX, maxX);
		if(maxY < minY)
			qSwap(minY, maxY);

		return QRectF(minX, minY, maxX-minX, maxY-minY);
	}


protected slots:
	/// Forward the valuesChanged() and stateChanged() signals from the data source.
	inline void onDataSourceDataChanged() { MPlotAbstractImageData::emitDataChanged(); }
	/// Forward the sizeChanged() and stateChanged() signals from the data source.
	inline void onDataSourceBoundsChanged() { MPlotAbstractImageData::emitBoundsChanged(); }

	/// ensure that we don't keep trying to read data from a source that has been deleted.
	void onDataSourceDeleted() { source_ = 0; setDataSource(0); }

protected:
	const AMDataSource* source_;
	bool isValid_;
};


#endif // AMDATASOURCEIMAGEDATA_H
