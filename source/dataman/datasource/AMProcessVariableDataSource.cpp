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


#include "AMProcessVariableDataSource.h"

 AM0DProcessVariableDataSource::~AM0DProcessVariableDataSource(){}
AM0DProcessVariableDataSource::AM0DProcessVariableDataSource(const AMProcessVariable *data, const QString &name, QObject *parent)
	: QObject(parent), AMDataSource(name)
{
	data_ = data;
	connect(data_, SIGNAL(valueChanged()), this, SLOT(onDataChanged()));
	connect(data_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onStateChanged()));
}

 AM1DProcessVariableDataSource::~AM1DProcessVariableDataSource(){}
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

 AM2DProcessVariableDataSource::~AM2DProcessVariableDataSource(){}
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
