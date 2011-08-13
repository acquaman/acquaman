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

	AMAxisInfo ai("xAxis", 0, "Array X-axis");
	ai.isUniform = true;
	ai.increment = scale_;
	axes_ << ai;
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

	AMAxisInfo xaxis("xAxis", length_, "Image X-axis");
	xaxis.isUniform = true;
	xaxis.increment = sx_;
	AMAxisInfo yaxis("yAxis", data_->count()/length_, "Image Y-axis");
	yaxis.isUniform = true;
	yaxis.increment = sy_;
	axes_ << xaxis << yaxis;
}
