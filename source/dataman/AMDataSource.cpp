#include "AMDataSource.h"

AMDataSourceSignalSource::AMDataSourceSignalSource(AMDataSource *parent)
	: QObject() {
	data_ = parent;
}

AMDataSource::AMDataSource(const QString& name)
	: name_(name)
{
	signalSource_ = new AMDataSourceSignalSource(this);
}

AMDataSource::~AMDataSource() {
	signalSource_->emitDeleted();
	delete signalSource_;
	signalSource_ = 0;
}
