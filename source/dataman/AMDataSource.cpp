#include "AMDataSource.h"

AMDataSourceSignalSource::AMDataSourceSignalSource(AMDataSource *parent)
	: QObject() {
	data_ = parent;
}

AMDataSource::AMDataSource()
{
	signalSource_ = new AMDataSourceSignalSource(this);
}

AMDataSource::~AMDataSource() {
	delete signalSource_;
	signalSource_ = 0;
}
