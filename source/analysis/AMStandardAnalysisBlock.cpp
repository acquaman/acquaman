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


#include "AMStandardAnalysisBlock.h"

// Create a new AMAnalysisBlock. The block is also an AMDataSource of output data, \c outputName is the name for this AMDataSource.
 AMStandardAnalysisBlock::~AMStandardAnalysisBlock(){}
AMStandardAnalysisBlock::AMStandardAnalysisBlock(const QString& outputName, QObject *parent)
	: AMAnalysisBlock(outputName, parent)
{

}

AMDataSource* AMStandardAnalysisBlock::inputDataSourceAt(int index) const
{
	if (index < 0 || index >= sources_.count())
		return 0;

	return sources_.at(index);
}

int AMStandardAnalysisBlock::indexOfInputSource(const QString& dataSourceName) const
{
	for (int i = 0; i < inputDataSourceCount(); i++) {
		if (inputDataSourceAt(i)->name() == dataSourceName)
			return i;
	}

	return -1;
}

int AMStandardAnalysisBlock::indexOfInputSource(const AMDataSource* source) const
{
	for (int i = 0; i < inputDataSourceCount(); i++) {
		if (inputDataSourceAt(i) == source)
			return i;
	}

	return -1;
}

AMnDIndex AMStandardAnalysisBlock::size() const
{
	int axesCount = axes_.count();
	AMnDIndex rv(axesCount, AMnDIndex::DoNotInit);
	for (int axisId = 0; axisId < axesCount; axisId++)
		rv[axisId] = size(axisId);

	return rv;
}

int AMStandardAnalysisBlock::idOfAxis(const QString& axisName) const
{
	for (int axisId = 0, axesCount = axes_.count(); axisId < axesCount; axisId++) {
		if (axes_.at(axisId).name == axisName)
			return axisId;
	}

	return -1;
}
