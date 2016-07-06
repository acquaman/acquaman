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


#include "AMDataSource.h"

#include "util/AMErrorMonitor.h"
#include "util/AMUtility.h"

AMDataSourceSignalSource::AMDataSourceSignalSource(AMDataSource *parent)
	: QObject() {
	data_ = parent;
}

AMDataSourceSignalSource::~AMDataSourceSignalSource(){}

AMDataSource::AMDataSource(const QString& name)
	: name_(name)
{
	signalSource_ = new AMDataSourceSignalSource(this);

	visibleInPlots_ = true;
	hiddenFromUsers_ = false;
}

AMDataSource::~AMDataSource() {
	signalSource_->emitDeleted();
	signalSource_->deleteLater();
	signalSource_ = 0;
}

// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension.
/* This base-class implementation simply calls value() repeatedly and should absolutely be re-implemented for better performance. */
bool AMDataSource::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	static bool programmerWarningIssued = false;
	if(!programmerWarningIssued) {
		AMErrorMon::debug(0, AMDATASOURCE_VALUES_BASE_IMPLEMENTATION_CALLED, QString("AMDataSource: Warning: Data source '%1' is using the base implementation of AMDataSource::values(), which is very inefficient. Re-implement values() to improve performance.  (This warning will only be given once.)").arg(name()));
		programmerWarningIssued = true;	// one problem with this warning method: if multiple classes have this problem, it will only be given once, and the subsequent classes will not be named.
	}

	int _rank = rank();

	if(indexStart.rank() != _rank || indexEnd.rank() != _rank)
		return false;

	for(int mu=0; mu<_rank; ++mu) {
		if(indexEnd.at(mu) >= size(mu))
			return false;
		if(indexEnd.at(mu) < indexStart.at(mu))
			return false;
	}

	switch(_rank) {
	case 0:
		*outputValues = double(value(indexStart));
		break;

	case 1: {
		for(int i=indexStart.i(); i<=indexEnd.i(); ++i)
			*(outputValues++) = double(value(AMnDIndex(i)));
		break;
	}

	case 2: {
		for(int i=indexStart.i(); i<=indexEnd.i(); ++i)
			for(int j=indexStart.j(); j<=indexEnd.j(); ++j)
				*(outputValues++) = double(value(AMnDIndex(i,j)));
		break;
	}

	case 3: {
		for(int i=indexStart.i(); i<=indexEnd.i(); ++i)
			for(int j=indexStart.j(); j<=indexEnd.j(); ++j)
				for(int k=indexStart.k(); k<=indexEnd.k(); ++k)
					*(outputValues++) = double(value(AMnDIndex(i,j,k)));
		break;
	}

	case 4: {
		for(int i=indexStart.i(); i<=indexEnd.i(); ++i)
			for(int j=indexStart.j(); j<=indexEnd.j(); ++j)
				for(int k=indexStart.k(); k<=indexEnd.k(); ++k)
					for(int l=indexStart.l(); l<=indexEnd.l(); ++l)
						*(outputValues++) = double(value(AMnDIndex(i,j,k,l)));
		break;
	}

	default: {
		valuesImplementationRecursive(indexStart, indexEnd, AMnDIndex(_rank, AMnDIndex::DoNotInit), 0, &outputValues);
		break;
	}
	}
	return true;
}

// Helper function to implement the base-class version of values() when rank > 4.
void AMDataSource::valuesImplementationRecursive(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, AMnDIndex current, int dimension, double **outputValues) const
{
	if(dimension == current.rank()-1) {	// base case: final dimension
		for(int i=indexStart.at(dimension); i<=indexEnd.at(dimension); ++i) {
			current[dimension] = i;
			*((*outputValues)++) = double(value(current));
		}
	}
	else {
		for(int i=indexStart.at(dimension); i<indexEnd.at(dimension); ++i) {
			current[dimension] = i;
			valuesImplementationRecursive(indexStart, indexEnd, current, dimension+1, outputValues);
		}
	}
}

bool AMDataSource::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	AMErrorMon::debug(0, AMDATASOURCE_AXISVALUES_BASE_IMPLEMENTATION_CALLED, QString("AMDataSource: Warning: Data source '%1' is using the base implementation of AMDataSource::axisValues(), which is very inefficient. Re-implement axisValues() to improve performance.  (This warning will only be given once.)").arg(name()));

	if (!isValid())
		return false;

	if (axisNumber >= rank())
		return false;

	if (startIndex < 0 || startIndex >= size(axisNumber))
		return false;

	if (endIndex < 0 || endIndex >= size(axisNumber))
		return false;

	for (int i = 0, size = endIndex-startIndex+1; i < size; i++)
        outputValues[i] = double(axisValue(axisNumber, i+startIndex));

	return true;
}

AMRange AMDataSource::dataRange() const
{
	QVector<double> sourceData = QVector<double>(size().product());
	values(AMnDIndex(rank(), AMnDIndex::DoInit), size()-1, sourceData.data());

	return AMUtility::rangeFinder(sourceData);
}

void AMDataSource::setVisibleInPlots(bool isVisible)
{
	if(isVisible == visibleInPlots_) return;

	visibleInPlots_ = isVisible;
	emitInfoChanged();
}
