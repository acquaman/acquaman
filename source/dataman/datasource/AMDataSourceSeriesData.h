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


#ifndef AMDATASOURCESERIESDATA_H
#define AMDATASOURCESERIESDATA_H

#include <QObject>

#include "MPlot/MPlotSeriesData.h"

#include "dataman/datasource/AMDataSource.h"
#include "util/AMRange.h"

/// This class wraps any AMDataSource for use as 2-dimensional (XY scatter) series data.  The rank() of the underlying data source must be 1. If the dimensionality is not correct, the wrapper will report a count() of 0.
class AMDataSourceSeriesData : public QObject, public MPlotAbstractSeriesData
{
	Q_OBJECT
public:
	/// Constructor. \c dataSource is the source to represent as XY scatter data.
	AMDataSourceSeriesData(const AMDataSource* dataSource, QObject* parent = 0);
	/// Destructor.
	virtual ~AMDataSourceSeriesData();

	/// Call this to switch to representing a different data source
	void setDataSource(const AMDataSource* dataSource);

	/// Access the underlying data source
	const AMDataSource* dataSource() const { return source_; }

	/// Return the x-value at \c index. \c index must be greater or equal to 0, and less than count().
	virtual double x(unsigned index) const;
	/// Copy the x-values from \c indexStart to \c indexEnd (inclusive) into \c outputValues.
	virtual void xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const;
	/// Return the y-value at \c index, which must be >= 0 and less than count().
	virtual double y(unsigned index) const;
	/// Copy the y-values from \c indexStart to \c indexEnd (inclusive) into \c outputValues.
	virtual void yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const;

	/// Return the number of elements
	virtual int count() const;

	/// Return the bounds of the data (the rectangle containing the max/min x- and y-values). It should be expressed as: QRectF(left, top, width, height) = QRectF(minX, minY, maxX-minX, maxY-minY);
	virtual QRectF boundingRect() const;

protected slots:
	/// Handles updating the axis values associated with the given index (will be 0 or 1).
	void onAxisValuesChanged();
	/// Handles updating the size of the given axis.  Will invalidate the axis values of that axis.
	void onSizeChanged();
	/// Forward the sizeChanged(), valuesChanged(), and stateChanged() signals from the data source.
	void onDataChanged(const AMnDIndex &start, const AMnDIndex &end);
	/// ensure that we don't keep trying to read data from a source that has been deleted.
	void onDataSourceDeleted();

protected:
	/// Required method that must be reimplemented that fills in the data when requested.  This method must reset the updateCacheRequired flag and set the dirty rect to null.
	virtual void updateCachedValues() const;

	/// The data source we are listening to.
	const AMDataSource* source_;
	/// The validity flag.
	bool isValid_;

	/// The size of the independent axis.
	int axisSize_;
	/// The cached independent axis data.
	QVector<qreal> axis_;
	/// The cached dependent data.
	mutable QVector<qreal> cachedData_;
	/// Flag for when to update the cached data.
	mutable bool cacheUpdateRequired_;
	/// Holds the data range of the dependent values.
	mutable AMRange range_;
};

#endif // AMDATASOURCESERIESDATA_H
