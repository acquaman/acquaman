#ifndef AMDATASOURCESERIESDATA_H
#define AMDATASOURCESERIESDATA_H

#include <QObject>
#include "MPlot/MPlotSeriesData.h"
#include "dataman/AMDataSource.h"

/// This class wraps any AMDataSource for use as 2-dimensional (XY scatter) series data.  The rank() of the underlying data source must be 1; if the dimensionality is not correct, the wrapper will report a count() of 0.
class AMDataSourceSeriesData : public QObject, public MPlotAbstractSeriesData
{
	Q_OBJECT
public:
	/// Constructor. \c dataSource is the source to represent as XY scatter data.
	AMDataSourceSeriesData(AMDataSource* dataSource, QObject* parent = 0);

	/// Call this to switch to representing a different data source
	void setDataSource(AMDataSource* dataSource);

	/// Access the underlying data source
	AMDataSource* dataSource() const { return source_; }

	/// Return the x-value at \c index. \c index must be greater or equal to 0, and less than count().
	virtual double x(unsigned index) const {
		return source_->axisValue(0, index);
	}
	/// Return the y-value at \c index, which must be >= 0 and less than count().
	virtual double y(unsigned index) const {
		return source_->value(AMnDIndex(index));
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
	AMDataSource* source_;
	bool isValid_;
};

#endif // AMDATASOURCESERIESDATA_H
