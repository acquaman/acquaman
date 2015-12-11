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
	/// Constructor. \c dataSource is the source to represent as Z=f(X,Y) image data.  Needs to use setDataSource to set the model.
	AMDataSourceImageData(QObject* parent = 0);
	/// Destructor.
	virtual ~AMDataSourceImageData();

	/// Call this to switch to representing a different data source
	void setDataSource(const AMDataSource* dataSource);

	/// Access the underlying data source
	const inline AMDataSource* dataSource() const { return source_; }

	/// Returns the range.  Reimplmented to use the caching scheme used in this class.
	virtual MPlotRange range() const;

	/// Return the x (data value) corresponding an (x,y) \c index.
	virtual double x(int index) const;
	/// Copy an entire block of x values from \c startIndex to \c endIndex into \c outputValues.
	virtual void xValues(int startIndex, int endIndex, double *outputValues);
	/// Return the y (data value) corresponding an (x,y) \c index.
	virtual double y(int index) const;
	/// Copy an entire block of y values from \c startIndex to \c endIndex into \c outputValues.
	virtual void yValues(int startIndex, int endIndex, double *outputValues);
	/// Return the z = f(x,y) value corresponding an index (\c xIndex, \c yIndex)
	virtual double z(int xIndex, int yIndex) const;
	/// Copy an entire block of z = f(x,y) values from (xStart,yStart) to (xEnd,yEnd) inclusive, into \c outputValues. The data is copied in row-major order, ie: with the x-axis varying the slowest. (Can assume \c outputValues has enough room to hold all the values, that (xStart,yStart) <= (xEnd,yEnd), and that the indexes are not out of range.)
	virtual void zValues(int xStart, int yStart, int xEnd, int yEnd, double* outputValues) const;

	/// Return the number of elements in x and y
	virtual QPoint count() const;

	/// Return the extent of the data. We assume the axis values are ordered... is this a valid assumption?
	virtual QRectF boundingRect() const;

protected slots:
	/// Handles updating the axis values associated with the given index (will be 0 or 1).
	void onAxisValuesChanged();
	/// Handles updating the data values from the given index to the end index.
	void onDataChanged(const AMnDIndex &start, const AMnDIndex &end);
	/// Handles updating the size of the given axis.  Will invalidate the axis values of that axis.
	void onSizeChanged();
	/// Recomputes the bounding rect. Called if the size or axis values change.  Only updates the corresponding size if specified.
	void recomputeBoundingRect();

	/// ensure that we don't keep trying to read data from a source that has been deleted.
	void onDataSourceDeleted();

protected:
	/// Required method that must be reimplemented that fills in the data when requested.  This method must reset the updateCacheRequired flag and set the dirty rect to null.
	virtual void updateCachedValues() const;

	/// Pointer to the data source this is encapsulating.
	const AMDataSource* source_;
	/// Flag on whether or not this is valid.
	bool isValid_;

	/// The cached bounding rect.
	QRectF boundingRect_;
	/// The size of the x axis.
	int xSize_;
	/// The size of the y axis.
	int ySize_;
	/// The cached x-axis values.
	QVector<double> xAxis_;
	/// The cached y-axis values.
	QVector<double> yAxis_;
	/// The cached z-values.
	mutable QVector<double> cachedData_;
	/// Flag used for knowing when to recache the data in the model.
	mutable bool cacheUpdateRequired_;
};


#endif // AMDATASOURCEIMAGEDATA_H
