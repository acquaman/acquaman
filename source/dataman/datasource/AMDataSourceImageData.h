/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMDATASOURCEIMAGEDATA_H
#define AMDATASOURCEIMAGEDATA_H

#include <QObject>

#include "MPlot/MPlotImageData.h"
#include "dataman/datasource/AMDataSource.h"

/// This class wraps any AMDataSource for use as 2-dimensional (XY scatter) series data.  The rank() of the underlying data source must be 2. If the dimensionality is not correct, the wrapper will report a count() of 0.  It only supports data sources with uniform axis scales for now.
class AMDataSourceImageData : public QObject, public MPlotAbstractImageData
{
	Q_OBJECT
public:
	/// Constructor. \c dataSource is the source to represent as Z=f(X,Y) image data.
 	virtual ~AMDataSourceImageData();
	AMDataSourceImageData(const AMDataSource* dataSource, QObject* parent = 0);

	/// Call this to switch to representing a different data source
	void setDataSource(const AMDataSource* dataSource);

	/// Access the underlying data source
	const inline AMDataSource* dataSource() const { return source_; }


	/// Return the x (data value) corresponding an (x,y) \c index.
	virtual inline double x(int indexX) const { return source_->axisValue(0, indexX); }
	/// Return the y (data value) corresponding an (x,y) \c index.
	virtual inline double y(int indexY) const { return source_->axisValue(1, indexY); }
	/// Return the z = f(x,y) value corresponding an index (\c xIndex, \c yIndex)
	virtual inline double z(int xIndex, int yIndex) const { return source_->value(AMnDIndex(xIndex, yIndex)); }
	/// Copy an entire block of z = f(x,y) values from (xStart,yStart) to (xEnd,yEnd) inclusive, into \c outputValues. The data is copied in row-major order, ie: with the x-axis varying the slowest. (Can assume \c outputValues has enough room to hold all the values, that (xStart,yStart) <= (xEnd,yEnd), and that the indexes are not out of range.)
	virtual inline void zValues(int xStart, int yStart, int xEnd, int yEnd, double* outputValues) const { source_->values(AMnDIndex(xStart,yStart), AMnDIndex(xEnd,yEnd), outputValues); }


	/// Return the number of elements in x and y
	virtual inline QPoint count() const { if(isValid_) return QPoint(source_->size(0), source_->size(1)); else return QPoint(0,0); }

	/// Return the extent of the data. We assume the axis values are ordered... is this a valid assumption?
	virtual inline QRectF boundingRect() const {
		if(!isValid_)
			return QRectF();

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
