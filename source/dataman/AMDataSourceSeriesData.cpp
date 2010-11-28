#include "AMDataSourceSeriesData.h"

AMDataSourceSeriesData::AMDataSourceSeriesData(const AMDataSource* dataSource, QObject* parent)
	: QObject(parent), MPlotAbstractSeriesData()
{
	source_ = 0;
	setDataSource(dataSource);
}

/// Call this to switch to representing a different data source
void AMDataSourceSeriesData::setDataSource(const AMDataSource* dataSource) {

	// disconnect the old source, if there is a valid old source.
	if(source_) {
		disconnect(source_->signalSource(), 0, this, 0);
	}

	source_ = dataSource;
	if(dataSource == 0 || dataSource->rank() != 1) {
		isValid_ = false;
	}
	else {
		isValid_ = true;
		connect(dataSource->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onDataSourceDataChanged()));
		connect(dataSource->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataSourceDataChanged()));
		connect(dataSource->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onDataSourceDataChanged()));
		connect(dataSource->signalSource(), SIGNAL(deleted(void*)), this, SLOT(onDataSourceDeleted()));
	}

	onDataSourceDataChanged();
}

