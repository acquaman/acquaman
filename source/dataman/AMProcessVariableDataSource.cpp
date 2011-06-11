#include "AMProcessVariableDataSource.h"

AM0DProcessVariableDataSource::AM0DProcessVariableDataSource(const AMProcessVariable *data, const QString &name, QObject *parent)
	: QObject(parent), AMDataSource(name)
{
	data_ = data;
	connect(data_, SIGNAL(valueChanged()), this, SLOT(onDataChanged()));
	connect(data_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onStateChanged()));
}

AM1DProcessVariableDataSource::AM1DProcessVariableDataSource(const AMProcessVariable *data, const QString &name, QObject *parent)
	: QObject(parent), AMDataSource(name)
{
	data_ = data;
	scale_ = 1;
	connect(data_, SIGNAL(valueChanged()), this, SLOT(onDataChanged()));
	connect(data_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onStateChanged()));
}

AM2DProcessVariableDataSource::AM2DProcessVariableDataSource(const AMProcessVariable *data, const QString &name, int rowLength, QObject *parent)
	: QObject(parent), AMDataSource(name)
{
	data_ = data;
	sx_ = 1;
	sy_ = 1;
	length_ = rowLength;
	connect(data_, SIGNAL(valueChanged()), this, SLOT(onDataChanged()));
	connect(data_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onStateChanged()));
}
