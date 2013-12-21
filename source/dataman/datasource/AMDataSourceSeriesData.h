/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

/// This class wraps any AMDataSource for use as 2-dimensional (XY scatter) series data.  The rank() of the underlying data source must be 1; if the dimensionality is not correct, the wrapper will report a count() of 0.
class AMDataSourceSeriesData : public QObject, public MPlotAbstractSeriesData
{
	Q_OBJECT
public:
	/// Constructor. \c dataSource is the source to represent as XY scatter data.
 	virtual ~AMDataSourceSeriesData();
	AMDataSourceSeriesData(const AMDataSource* dataSource, QObject* parent = 0);

	/// Call this to switch to representing a different data source
	void setDataSource(const AMDataSource* dataSource);

	/// Access the underlying data source
	const AMDataSource* dataSource() const { return source_; }

	/// Return the x-value at \c index. \c index must be greater or equal to 0, and less than count().
	virtual double x(unsigned index) const {
		return source_->axisValue(0, index);
	}
	/// Copy the x-values from \c indexStart to \c indexEnd (inclusive) into \c outputValues.
	virtual void xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const {
		for(unsigned i=indexStart; i<=indexEnd; ++i)
			*(outputValues++) = source_->axisValue(0, i);
	}
	/// Return the y-value at \c index, which must be >= 0 and less than count().
	virtual double y(unsigned index) const {
		return source_->value(AMnDIndex(index));
	}
	/// Copy the y-values from \c indexStart to \c indexEnd (inclusive) into \c outputValues.
	virtual void yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const {
		source_->values(AMnDIndex(indexStart), AMnDIndex(indexEnd), outputValues);
	}

	/// Return the number of elements
	virtual int count() const {
		if(isValid_)
			return source_->size(0);
		else
			return 0;
	}

protected slots:
	/// Forward the sizeChanged(), valuesChanged(), and stateChanged() signals from the data source.
	void onDataSourceDataChanged() { MPlotAbstractSeriesData::emitDataChanged(); }
	/// ensure that we don't keep trying to read data from a source that has been deleted.
	void onDataSourceDeleted() { source_ = 0; setDataSource(0); }

protected:
	const AMDataSource* source_;
	bool isValid_;
};

#endif // AMDATASOURCESERIESDATA_H
