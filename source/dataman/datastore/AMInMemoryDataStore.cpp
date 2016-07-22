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


#include "AMInMemoryDataStore.h"
#include "util/AMErrorMonitor.h"

AMInMemoryDataStore::AMInMemoryDataStore(QObject* parent)
	: AMDataStore(parent)
{

}

AMInMemoryDataStore::~AMInMemoryDataStore() {

}

bool AMInMemoryDataStore::addMeasurement(const AMMeasurementInfo &measurementDetails) {

	// already a measurement with this name?
	for(int i=measurements_.count()-1; i>=0; i--)
		if(measurements_.at(i).name == measurementDetails.name)
			return false;


	// add this measurement to our registry
	/// \todo Anything to verify that we need to make sure is true in the details?
	measurements_.append(measurementDetails);

	// scalar scan space. append null measurement to scalar scan point.
	scalarScanPoint_.append(AMIMDSMeasurement(measurementDetails.spanSize()));
	// if we have existing scan points... need to add storage for this measurement to each one.
	if(!scanSpaceIsEmpty()) {
		int spanSize = measurementDetails.spanSize();
		for(int i=0, cc=scanPoints_.count(); i<cc; ++i)
			scanPoints_[i].append(AMIMDSMeasurement(spanSize));
	}

	return true;
}

int AMInMemoryDataStore::idOfMeasurement(const QString &measurementName) const {
	for(int i=measurements_.count()-1; i>=0; i--)
		if(measurements_.at(i).name == measurementName)
			return i;
	return -1;
}

bool AMInMemoryDataStore::addScanAxis(const AMAxisInfo &axisDetails) {
	// axis already exists with this name... Not allowed.
	for(int i=axes_.count()-1; i>=0; --i)
		if(axes_.at(i).name == axisDetails.name) {
			AMErrorMon::debug(0, -721, QString("AMInMemoryDataStore: Could not add this scan axis to the data store; an axis with the name %1 already exists.").arg(axisDetails.name));
			return false;
		}


	AMAxisInfo axisInfo = axisDetails;
	if(axes_.count() == 0) {	// if this is the first axis to be added...
		axisInfo.size = 0;
	}

	else {	// there are axes already.
		// check 1: the size of this axis must not be 0, they must provide a final size
		if(axisInfo.size < 1) {
			AMErrorMon::debug(0, -722, QString("AMInMemoryDataStore: Could not add a scan axis '%1' with a size of 0 to the data store; all axes except for the first must specify their final size.").arg(axisDetails.name));
			return false;
		}

		// check 2: there cannot be rows already in the data store.
		if(axes_.at(0).size != 0) {
			AMErrorMon::debug(0, -723, QString("AMInMemoryDataStore: Could not add scan axis '%1' to the data store because there are already rows/scan points present.").arg(axisDetails.name));
			return false;
		}
	}

	axes_.append(axisInfo);
	scanSize_.append(axisInfo.size);

	// Add entry in axisValues_ for this new axis.  If this axis is uniform, we don't need actual storage for axis values. However, we need to insert one into axisValues_ anyway to maintain indexing.
	if(axisInfo.isUniform)
		axisValues_ << QVector<AMNumber>(0);
	else
		axisValues_ << QVector<AMNumber>(axisInfo.size);

	return true;
}

int AMInMemoryDataStore::idOfScanAxis(const QString &axisName) const {
	for(int i=axes_.count()-1; i>=0; i--)
		if(axes_.at(i).name == axisName)
			return i;
	return -1;
}

AMNumber AMInMemoryDataStore::value(const AMnDIndex &scanIndex, int measurementId, const AMnDIndex &measurementIndex) const {

	// scan axis index doesn't provide enough / too many dimensions
	if(scanIndex.rank() != axes_.count())
		return AMNumber(AMNumber::DimensionError);

	if((unsigned)measurementId >= (unsigned)measurements_.count())
		return AMNumber(AMNumber::InvalidError);	// invalid measurement specified

	if(measurementIndex.rank() != measurements_.at(measurementId).rank())
		return AMNumber(AMNumber::DimensionError);

	int flatMeasurementIndex = flatIndexForMeasurement(measurementId, measurementIndex);

	if(axes_.count() == 0) {
		if(flatMeasurementIndex >= scalarScanPoint_.at(measurementId).size())
			return AMNumber(AMNumber::OutOfBoundsError);
		return scalarScanPoint_.at(measurementId).at(flatMeasurementIndex);
	}
	else { // higher dimensions:
		int flatScanIndex = scanIndex.flatIndexInArrayOfSize(scanSize_);
		if(flatScanIndex >= scanPoints_.count())
			return AMNumber(AMNumber::OutOfBoundsError);
		if(flatMeasurementIndex >= scanPoints_.at(flatScanIndex).at(measurementId).size())
			return AMNumber(AMNumber::OutOfBoundsError);
		return scanPoints_.at(flatScanIndex).at(measurementId).at(flatMeasurementIndex);
	}
}

bool AMInMemoryDataStore::setValue(const AMnDIndex &scanIndex, int measurementId, const AMnDIndex &measurementIndex, const AMNumber &newValue) {

	if(scanIndex.rank() != axes_.count())
		return false; // scan axis index doesn't provide enough / too many dimensions

	if((unsigned)measurementId >= (unsigned)measurements_.count())
		return false;	// invalid measurement specified

	if(measurementIndex.rank() != measurements_.at(measurementId).rank())
		return false;

	int flatMeasurementIndex = flatIndexForMeasurement(measurementId, measurementIndex);

	if(axes_.count() == 0) {
		if(flatMeasurementIndex >= scalarScanPoint_.at(measurementId).size())
			return false;
		scalarScanPoint_[measurementId][flatMeasurementIndex] = newValue;
	}
	else { // higher dimensions:
		int flatScanIndex = scanIndex.flatIndexInArrayOfSize(scanSize_);
		if(flatScanIndex >= scanPoints_.count())
			return false;
		if(flatMeasurementIndex >= scanPoints_.at(flatScanIndex).at(measurementId).size())
			return false;
		scanPoints_[flatScanIndex][measurementId][flatMeasurementIndex] = newValue;
	}

	emitDataChanged(scanIndex, scanIndex, measurementId);
	return true;
}

bool AMInMemoryDataStore::values(const AMnDIndex &scanIndexStart, const AMnDIndex &scanIndexEnd, int measurementId, const AMnDIndex &measurementIndexStart, const AMnDIndex &measurementIndexEnd, double *outputValues) const {

	if(scanIndexStart.rank() != axes_.count() || scanIndexEnd.rank() != axes_.count())
		return false;
	if(measurementId >= measurements_.count())
		return false;

	const AMMeasurementInfo& mi = measurements_.at(measurementId);
	if(measurementIndexStart.rank() != mi.rank() || measurementIndexEnd.rank() != mi.rank())
		return false;

	// check bounds for scan axes
	for(int mu=axes_.count()-1; mu >= 0; --mu) {
		if(scanIndexEnd.at(mu) < scanIndexStart.at(mu))
			return false;
		if(scanIndexEnd.at(mu) >= axes_.at(mu).size)
			return false;
	}

	// check bounds for measurement axes
	for(int mu=mi.rank()-1; mu >= 0; --mu) {
		if(measurementIndexEnd.at(mu) < measurementIndexStart.at(mu))
			return false;
		if(measurementIndexEnd.at(mu) >= mi.size(mu))
			return false;
	}

	// Determine the full size of the measurement (not necessarily the size of the block that we want to read out).
	AMnDIndex measurementSize = mi.size();
	int flatMeasurementSize = measurementSize.product();

	// specific cases of scan rank:
	switch(scanIndexStart.rank()) {
	case 0: {
		// null scan space, just copy in the measurement block

		if(measurementIndexStart.rank() == 0) {	// If measurements are scalar values, can optimize.
			outputValues[0] = double(scalarScanPoint_.at(measurementId).at(0));
		}

		else {
			// need to find out how many points one measurement block takes
			int measurementSpaceSize = measurementIndexStart.totalPointsTo(measurementIndexEnd);

			if(measurementSpaceSize == flatMeasurementSize)	// if asking for the whole measurement, can optimize.
				measurementValues(scalarScanPoint_.at(measurementId), flatMeasurementSize, outputValues);
			else
				measurementValues(scalarScanPoint_.at(measurementId), measurementSize, measurementIndexStart, measurementIndexEnd, outputValues);
		}
		break;
	}

	case 1:{
		if(measurementIndexStart.rank() == 0) {	// If measurements are scalar values, can optimize.
			for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i)
				*(outputValues++) = double(scanPoints_.at(i).at(measurementId).at(0));
		}

		else {
			// need to find out how many points one measurement block takes
			int measurementSpaceSize = measurementIndexStart.totalPointsTo(measurementIndexEnd);

			if(measurementSpaceSize == flatMeasurementSize)	// if asking for the whole measurement, can optimize.
				for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
					measurementValues(scanPoints_.at(i).at(measurementId), flatMeasurementSize, outputValues);
					outputValues += measurementSpaceSize;
				}
			else
				for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
					measurementValues(scanPoints_.at(i).at(measurementId), measurementSize, measurementIndexStart, measurementIndexEnd, outputValues);
					outputValues += measurementSpaceSize;
				}
		}
		break;
	}

	case 2:{
		if(measurementIndexStart.rank() == 0) {	// If measurements are scalar values, can optimize.
			for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
				int ic = i*scanSize_.j();
				for(int j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
					*(outputValues++) = double(scanPoints_.at(ic+j).at(measurementId).at(0));
				}
			}
		}

		else {
			// need to find out how many points one measurement block takes
			int measurementSpaceSize = measurementIndexStart.totalPointsTo(measurementIndexEnd);

			if(measurementSpaceSize == flatMeasurementSize) {	// if asking for the whole measurement, can optimize.
				for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
					int ic = i*scanSize_.j();
					for(int j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
						measurementValues(scanPoints_.at(ic+j).at(measurementId), flatMeasurementSize, outputValues);
						outputValues += measurementSpaceSize;
					}
				}
			}
			else {
				for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
					int ic = i*scanSize_.j();
					for(int j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
						measurementValues(scanPoints_.at(ic+j).at(measurementId), measurementSize, measurementIndexStart, measurementIndexEnd, outputValues);
						outputValues += measurementSpaceSize;
					}
				}
			}
		}
		break;
	}

	case 3:{
		if(measurementIndexStart.rank() == 0) {	// If measurements are scalar values, can optimize.
			for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
				int ic = i*scanSize_.j()*scanSize_.k();
				for(int j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
					int jc = j*scanSize_.k();
					for(int k=scanIndexStart.k(); k<=scanIndexEnd.k(); ++k) {
						*(outputValues++) = double(scanPoints_.at(ic+jc+k).at(measurementId).at(0));
					}
				}
			}
		}

		else {
			// need to find out how many points one measurement block takes
			int measurementSpaceSize = measurementIndexStart.totalPointsTo(measurementIndexEnd);

			if(measurementSpaceSize == flatMeasurementSize) {	// if asking for the whole measurement, can optimize.
				for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
					int ic = i*scanSize_.j()*scanSize_.k();
					for(int j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
						int jc = j*scanSize_.k();
						for(int k=scanIndexStart.k(); k<=scanIndexEnd.k(); ++k) {
							measurementValues(scanPoints_.at(ic+jc+k).at(measurementId), flatMeasurementSize, outputValues);
							outputValues += measurementSpaceSize;
						}
					}
				}
			}
			else {
				for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
					int ic = i*scanSize_.j()*scanSize_.k();
					for(int j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
						int jc = j*scanSize_.k();
						for(int k=scanIndexStart.k(); k<=scanIndexEnd.k(); ++k) {
							measurementValues(scanPoints_.at(ic+jc+k).at(measurementId), measurementSize, measurementIndexStart, measurementIndexEnd, outputValues);
							outputValues += measurementSpaceSize;
						}
					}
				}
			}
		}
		break;
	}

	case 4:{
		if(measurementIndexStart.rank() == 0) {	// If measurements are scalar values, can optimize.
			for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
				int ic = i*scanSize_.j()*scanSize_.k()*scanSize_.l();
				for(int j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
					int jc = j*scanSize_.k()*scanSize_.l();
					for(int k=scanIndexStart.k(); k<=scanIndexEnd.k(); ++k) {
						int kc = k*scanSize_.l();
						for(int l=scanIndexStart.l(); l<=scanIndexEnd.l(); ++l) {
							*(outputValues++) = double(scanPoints_.at(ic+jc+kc+l).at(measurementId).at(0));
						}
					}
				}
			}
		}

		else {
			int measurementSpaceSize = measurementIndexStart.totalPointsTo(measurementIndexEnd);

			if(measurementSpaceSize == flatMeasurementSize) {	// if asking for the whole measurement, can optimize.
				for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
					int ic = i*scanSize_.j()*scanSize_.k()*scanSize_.l();
					for(int j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
						int jc = j*scanSize_.k()*scanSize_.l();
						for(int k=scanIndexStart.k(); k<=scanIndexEnd.k(); ++k) {
							int kc = k*scanSize_.l();
							for(int l=scanIndexStart.l(); l<=scanIndexEnd.l(); ++l) {
								measurementValues(scanPoints_.at(ic+jc+kc+l).at(measurementId), flatMeasurementSize, outputValues);
								outputValues += measurementSpaceSize;
							}
						}
					}
				}
			}
			else {
				for(int i=scanIndexStart.i(); i<=scanIndexEnd.i(); ++i) {
					int ic = i*scanSize_.j()*scanSize_.k()*scanSize_.l();
					for(int j=scanIndexStart.j(); j<=scanIndexEnd.j(); ++j) {
						int jc = j*scanSize_.k()*scanSize_.l();
						for(int k=scanIndexStart.k(); k<=scanIndexEnd.k(); ++k) {
							int kc = k*scanSize_.l();
							for(int l=scanIndexStart.l(); l<=scanIndexEnd.l(); ++l) {
								measurementValues(scanPoints_.at(ic+jc+kc+l).at(measurementId), measurementSize, measurementIndexStart, measurementIndexEnd, outputValues);
								outputValues += measurementSpaceSize;
							}
						}
					}
				}
			}
		}
		break;
	}
	default:{
		int measurementSpaceSize = measurementIndexStart.totalPointsTo(measurementIndexEnd);

		valuesImplementationRecursive(scanIndexStart, scanIndexEnd, measurementId, measurementIndexStart, measurementIndexEnd, &outputValues, 0, 0, measurementSize, measurementSpaceSize);
		return false;
		break;
	}
	}

	return true;
}

bool AMInMemoryDataStore::setValue(const AMnDIndex &scanIndex, int measurementId, const int *inputData)  {
	// scan axis index doesn't provide enough / too many dimensions
	if(scanIndex.rank() != axes_.count())
		return false;

	if((unsigned)measurementId >= (unsigned)measurements_.count())
		return false;	// invalid measurement specified

	// scalar scan space:
	if(axes_.count() == 0) {
		AMIMDSMeasurement& measurement = scalarScanPoint_[measurementId];
		for(int i=0,cc=measurement.size(); i<cc; ++i)
			measurement[i] = inputData[i];
	}
	// higher dimension scan space
	else {
		int flatScanIndex = scanIndex.flatIndexInArrayOfSize(scanSize_);
		if(flatScanIndex >= scanPoints_.count())
			return false;
		AMIMDSMeasurement& measurement = scanPoints_[flatScanIndex][measurementId];
		for(int i=0,cc=measurement.size(); i<cc; ++i)
			measurement[i] = inputData[i];
	}

	emitDataChanged(scanIndex, scanIndex, measurementId);
	return true;
}

bool AMInMemoryDataStore::setValue(const AMnDIndex &scanIndex, int measurementId, const double *inputData) {
	// scan axis index doesn't provide enough / too many dimensions
	if(scanIndex.rank() != axes_.count())
		return false;

	if((unsigned)measurementId >= (unsigned)measurements_.count())
		return false;	// invalid measurement specified

	// scalar scan space:
	if(axes_.count() == 0) {
		AMIMDSMeasurement& measurement = scalarScanPoint_[measurementId];
		for(int i=0,cc=measurement.size(); i<cc; ++i)
			measurement[i] = inputData[i];
	}
	// higher dimension scan space:
	else {
		int flatScanIndex = scanIndex.flatIndexInArrayOfSize(scanSize_);
		if(flatScanIndex >= scanPoints_.count())
			return false;
		AMIMDSMeasurement& measurement = scanPoints_[flatScanIndex][measurementId];
		for(int i=0,cc=measurement.size(); i<cc; ++i)
			measurement[i] = inputData[i];
	}

	emitDataChanged(scanIndex, scanIndex, measurementId);
	return true;
}

AMNumber AMInMemoryDataStore::axisValue(int axisId, long axisIndex) const {

	if((unsigned)axisId >= (unsigned)axes_.count())
		return AMNumber(AMNumber::InvalidError);	// invalid axis specified.

	if((unsigned)axisIndex >= (unsigned)axes_.at(axisId).size)
		return AMNumber(AMNumber::OutOfBoundsError);

	const AMAxisInfo& ai = axes_.at(axisId);
	if(ai.isUniform)
		return (double)ai.start + axisIndex*(double)ai.increment;
	else
		return axisValues_.at(axisId).at(axisIndex);

}

bool AMInMemoryDataStore::axisValues(int axisId, long axisStartIndex, long axisEndIndex, double *outputValues) const
{
	if((unsigned)axisId >= (unsigned)axes_.count())
		return false;	// invalid axis specified.

	if((unsigned)axisStartIndex >= (unsigned)axes_.at(axisId).size)
		return false;

	if((unsigned)axisEndIndex >= (unsigned)axes_.at(axisId).size)
		return false;

	for (int i = 0, size = (axisEndIndex-axisStartIndex+1); i < size; i++)
        outputValues[i] = double(axisValues_.at(axisId).at(i+axisStartIndex));

	return true;
}

bool AMInMemoryDataStore::setAxisValue(int axisId, long axisIndex, AMNumber newValue) {

	if((unsigned)axisId >= (unsigned)axes_.count())
		return false;	// invalid axis specified.

	if((unsigned)axisIndex >= (unsigned)axes_.at(axisId).size)
		return false;

	if(axes_.at(axisId).isUniform)
		return false;

	axisValues_[axisId][axisIndex] = newValue;
	return true;
}

bool AMInMemoryDataStore::beginInsertRowsImplementation(long numRows, long atRowIndex) {

	axes_[0].size += numRows;
	scanSize_[0] += numRows;

	// number of scan points to add:
	int pointsPerRow = 1;
	for(int mu=axes_.count()-1; mu>=1; --mu)
		pointsPerRow *= axes_.at(mu).size;

	// Build a scan point for all of our measurements
	AMIMDSScanPoint sp;
	for(int i=0,cc=measurements_.count(); i<cc; ++i)
		sp.append(AMIMDSMeasurement(measurements_.at(i).spanSize()));

	// insert that scan point (times pointsPerRow) for all rows to insert.
	scanPoints_.insert(atRowIndex*pointsPerRow, numRows*pointsPerRow, sp);

	// add to axis values of first scan axis.
	if(!axes_.at(0).isUniform)
		axisValues_[0].insert(atRowIndex, numRows, AMNumber());

	return true;

}

void AMInMemoryDataStore::clearScanDataPointsImplementation() {
	if(axes_.count() >= 1) {

		axes_[0].size = 0;
		scanSize_[0] = 0;

		scanPoints_.clear();
		axisValues_[0].clear();
	}
}

void AMInMemoryDataStore::clearMeasurementsImplementation() {
	scalarScanPoint_.clear();
	measurements_.clear();
}

void AMInMemoryDataStore::clearScanAxesImplementation() {
	axes_.clear();
	scanSize_ = AMnDIndex();
	axisValues_.clear();
}


void AMInMemoryDataStore::measurementValues(const AMIMDSMeasurement& measurement, const AMnDIndex& fullSize, const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const {

	/// \todo Use memcpy once we move to a packed 64-bit size for AMNumber storage.

	switch(indexStart.rank()) {
	case 0: {
		outputValues[0] = double(measurement.at(0));
		break;
	}

	case 1: {
		for(int i=indexStart.i(); i<=indexEnd.i(); ++i)
			*(outputValues++) = double(measurement.at(i));
		break;
	}

	case 2: {
		for(int i=indexStart.i(); i<=indexEnd.i(); ++i) {
			int ic = i*fullSize.j();
			for(int j=indexStart.j(); j<=indexEnd.j(); ++j) {
				*(outputValues++) = double(measurement.at(ic+j));
			}
		}
		break;
	}

	case 3: {
		for(int i=indexStart.i(); i<=indexEnd.i(); ++i) {
			int ic = i*fullSize.j()*fullSize.k();
			for(int j=indexStart.j(); j<=indexEnd.j(); ++j) {
				int jc = j*fullSize.k();
				for(int k=indexStart.k(); k<=indexEnd.k(); ++k) {
					*(outputValues++) = double(measurement.at(ic+jc+k));
				}
			}
		}
		break;
	}

	case 4: {
		for(int i=indexStart.i(); i<=indexEnd.i(); ++i) {
			int ic = i*fullSize.j()*fullSize.k()*fullSize.l();
			for(int j=indexStart.j(); j<=indexEnd.j(); ++j) {
				int jc = j*fullSize.k()*fullSize.l();
				for(int k=indexStart.k(); k<=indexEnd.k(); ++k) {
					int kc = k*fullSize.l();
					for(int l=indexStart.l(); l<=indexEnd.l(); ++l) {
						*(outputValues++) = double(measurement.at(ic+jc+kc+l));
					}
				}
			}
		}
		break;
	}

	default: {
		// general recursive case:
		measurementValuesImplementationRecursive(measurement, indexStart, indexEnd, fullSize, &outputValues, 0, 0);
		break;
	}
	}
}

void AMInMemoryDataStore::measurementValues(const AMIMDSMeasurement& measurement, int fullSize, double* outputValues) const {
	for(int i=0; i<fullSize; ++i)
		*(outputValues++) = double(measurement.at(i));
}


void AMInMemoryDataStore::measurementValuesImplementationRecursive(const AMIMDSMeasurement &measurement, const AMnDIndex &indexStart, const AMnDIndex &indexEnd, const AMnDIndex &fullSize, double **outputValues, int dimension, int cOffset) const {

	if(dimension == indexStart.rank()-1) { // base case: final dimension
		for(int i=indexStart.at(dimension); i<=indexEnd.at(dimension); ++i) {
			*((*outputValues)++) = double(measurement.at(cOffset+i));
		}
	}
	else {
		for(int i=indexStart.at(dimension); i<=indexEnd.at(dimension); ++i) {
			// get product of all higher dimensions:
			int multiplier = 1;
			for(int mu=dimension+1; mu<indexStart.rank(); ++mu)
				multiplier *= fullSize.at(mu);

			// recurse:
			measurementValuesImplementationRecursive(measurement, indexStart, indexEnd, fullSize, outputValues, dimension+1, cOffset + i*multiplier);
		}
	}
}

void AMInMemoryDataStore::valuesImplementationRecursive(const AMnDIndex &siStart, const AMnDIndex &siEnd, int measurementId, const AMnDIndex &miStart, const AMnDIndex &miEnd, double **outputValues, int scanDimension, int scanSpaceOffset, const AMnDIndex &fullSize, int measurementSpaceSize) const {

	if(scanDimension == axes_.count()-1) { // base case: last (final) dimension
		for(int i=siStart.at(scanDimension); i<=siEnd.at(scanDimension); ++i) {
			measurementValues(scanPoints_.at(scanSpaceOffset+i).at(measurementId), fullSize, miStart, miEnd, *outputValues);
			*outputValues += measurementSpaceSize;
		}
	}
	else {
		for(int i=siStart.at(scanDimension); i<=siEnd.at(scanDimension); ++i) {
			// get product of all higher scan dimensions:
			int multiplier = 1;
			for(int mu=scanDimension+1; mu<siStart.rank(); ++mu)
				multiplier *= scanSize_.at(mu);

			// recurse:
			valuesImplementationRecursive(siStart, siEnd, measurementId, miStart, miEnd, outputValues, scanDimension+1, scanSpaceOffset + i*multiplier, fullSize, measurementSpaceSize);
		}
	}
}
