#include "AMProcessVariableDataSource.h"

AMProcessVariableDataSource::AMProcessVariableDataSource(const AMProcessVariable *data, const QString &name, QObject *parent)
	: QObject(parent), AMDataSource(name)
{
	data_ = data;
	scale_ = 1;
	connect(data_, SIGNAL(valueChanged()), this, SLOT(onDataChanged()));
	connect(data_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onStateChanged()));
}

void AMProcessVariableDataSource::onDataChanged()
{
	emitValuesChanged();
}

void AMProcessVariableDataSource::onStateChanged()
{
	emitStateChanged(state());
}
