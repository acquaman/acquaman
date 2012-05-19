/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMINMEMORYDATASTORE_H
#define AMINMEMORYDATASTORE_H

#include <QVector>
#include "dataman/AMNumber.h"
#include "dataman/datastore/AMDataStore.h"

#include "util/AMErrorMonitor.h"


/// (Internal class for AMInMemoryDataStore) One instance stores a single measurement. (Flat array; nD measurement data is stored in row-major order; last index varies the fastest.)
typedef QVector<AMNumber> AMIMDSMeasurement;

/// (Internal class for AMInMemoryDataStore) One instance stores a scan point: (ie: a set of measurements at that scan point)
typedef QVector<AMIMDSMeasurement> AMIMDSScanPoint;



/// This class implements multi-dimensional storage of scan data, according to the AMDataStore interface.  All data is stored in memory, and accessing data values should be quite fast.
/*! <b>Memory Layout</b>
We need to provide a dense multi-dimensional scan space across scan axes, and at each point in that space, a set of multi-dimensional detector measurements (where each kind of measurement can have a different size).

Detector measurements are stored in a vector of AMNumbers, typedef'd as AMIMDSMeasurement.  For multi-dimensional measurements, the data is stored flattened, in row-major order (ie: the first measurement axis varies the slowest).

A vector of AMIMDSMeasurements makes up a single AMIMDSScanPoint.

Finally, a vector of AMIMDSScanPoints makes up the scan space.  Multi-dimensional scan spaces are also stored flattened, in row-major order.

With the exception of single detector blocks, the full data store is not stored contiguously, so datasets approaching the size of the full computer memory may be used, without requiring a single allocated chunk.


\note A previous version of AMInMemoryDataStore (up until commit f846098ca399b) used a hierarchical tree for storing the scan space, so that any scan axis could be extended on an already-populated data store. This version has been simplified based on the new restrictions in the AMDataStore API.
 */

class AMInMemoryDataStore : public AMDataStore
{
public:
	/// Constructs an empty data store.
	AMInMemoryDataStore();

	virtual ~AMInMemoryDataStore();

	/// Creates space to support an additional measurement at every scan point. \c measurementDetails describes the dimensionality and details of the new set of measurements.  (If a set of scan points exist already, the initial values for this measurement at those scan points should be Null.)
	/*! If you want to retrieve measurements by name, \c measurementDetails must contain a unique \c name.  This function should return false if a measurement with that name already exists. */
	virtual bool addMeasurement(const AMMeasurementInfo& measurementDetails) {

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

	/// Retrieve the id of an existing set of measurements, by name.  (Depending on the implementation, this may not be fast. Avoid calling it repeatedly.) Returns -1 if not found.
	virtual int idOfMeasurement(const QString& measurementName) const {
		for(int i=measurements_.count()-1; i>=0; i--)
			if(measurements_.at(i).name == measurementName)
				return i;
		return -1;
	}

	/// Retrieve information about a set of measurements, by id. \c id assumed to be >= 0 and < measurementCount().
	virtual AMMeasurementInfo measurementAt(int id) const {
		return measurements_.at(id);
	}

	/// Return the number of measurements stored for each scan point
	virtual int measurementCount() const {
		return measurements_.count();
	}




	/// Create space to support an (additional) scan axis.  \c axisDetails describes the characteristics of the axis, but if this is the first axis to be added, the \c axisDetails.size will be set to 0.  All subsequent axes must specify their final size (>0), and this size cannot be changed later. (ie: Only the first axis can be extended via beginInsertRows()).  Note that it is also impossible to add scan axes after rows have already been added with beginInsertRows(); in that case, this function should return false.
	/*! These restrictions are in place to simplify behaviour and increase performance for implementations.

If you want to retrieve axes by name, \c axisDetails must contain a unique \c name.  This function should return false if an axis with that name already exists.

	  \note No signalling is provided for alerting observers of new scan axes. It's also prohibited for AMDataSources that expose this data (for ex: AMRawDataSource) to change dimensionality (ie: add another axis). Therefore, it's recommended to only call this function when first setting up a dataStore, before any observers get involved.
*/
	virtual bool addScanAxis(const AMAxisInfo& axisDetails) {
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
			// check 1: the size of this axis must not be 0; they must provide a final size
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

	/// Retrieve the id of an existing axis, by name.
	virtual int idOfScanAxis(const QString& axisName) const {
		for(int i=axes_.count()-1; i>=0; i--)
			if(axes_.at(i).name == axisName)
				return i;
		return -1;
	}
	/// Retrieve information about an axis, by id.  \c id assumed to be >= 0 and < scanAxesCount().
	virtual AMAxisInfo scanAxisAt(int id) const {
		return axes_.at(id);
	}
	/// Return the number of scan axes
	virtual int scanAxesCount() const {
		return axes_.count();
	}
	/// Synonym for scanAxisCount()
	int scanRank() const { return axes_.count(); }
	/// Return the sizes of all the scan axes, in order.
	virtual AMnDIndex scanSize() const {
		return scanSize_;
	}
	/// Return the size along a specific axis, by \c id.  \c id assumed to be >= 0 and < scanAxesCount().
	virtual int scanSize(int axisId) const {
		return scanSize_.at(axisId);
	}



	/// Retrieve a value from a measurement, at a specific scan point.
	virtual AMNumber value(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex) const {

		// scan axis index doesn't provide enough / too many dimensions
		if(scanIndex.rank() != axes_.count())
			return AMNumber(AMNumber::DimensionError);

		if((unsigned)measurementId >= (unsigned)measurements_.count())
			return AMNumber(AMNumber::InvalidError);	// invalid measurement specified;

		if(measurementIndex.rank() != measurements_.at(measurementId).rank())
			return AMNumber(AMNumber::DimensionError);

		int flatMeasurementIndex = flatIndexForMeasurement(measurementId, measurementIndex);

		if(axes_.count() == 0) {
#ifdef AM_ENABLE_BOUNDS_CHECKING
			if(flatMeasurementIndex >= scalarScanPoint_.at(measurementId).size())
				return AMNumber(AMNumber::OutOfBoundsError);
#endif
			return scalarScanPoint_.at(measurementId).at(flatMeasurementIndex);
		}
		else { // higher dimensions:
			int flatScanIndex = scanIndex.flatIndexInArrayOfSize(scanSize_);
#ifdef AM_ENABLE_BOUNDS_CHECKING
			if(flatScanIndex >= scanPoints_.count())
				return AMNumber(AMNumber::OutOfBoundsError);
			if(flatMeasurementIndex >= scanPoints_.at(flatScanIndex).at(measurementId).size())
				return AMNumber(AMNumber::OutOfBoundsError);
#endif
			return scanPoints_.at(flatScanIndex).at(measurementId).at(flatMeasurementIndex);
		}
	}


	/// Set the value of a measurement, at a specific scan point
	virtual bool setValue(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex, const AMNumber& newValue) {

		if(scanIndex.rank() != axes_.count())
			return false; // scan axis index doesn't provide enough / too many dimensions

		if((unsigned)measurementId >= (unsigned)measurements_.count())
			return false;	// invalid measurement specified;

		if(measurementIndex.rank() != measurements_.at(measurementId).rank())
			return false;

		int flatMeasurementIndex = flatIndexForMeasurement(measurementId, measurementIndex);

		if(axes_.count() == 0) {
#ifdef AM_ENABLE_BOUNDS_CHECKING
			if(flatMeasurementIndex >= scalarScanPoint_.at(measurementId).size())
				return false;
#endif
			scalarScanPoint_[measurementId][flatMeasurementIndex] = newValue;
			return true;
		}
		else { // higher dimensions:
			int flatScanIndex = scanIndex.flatIndexInArrayOfSize(scanSize_);
#ifdef AM_ENABLE_BOUNDS_CHECKING
			if(flatScanIndex >= scanPoints_.count())
				return false;
			if(flatMeasurementIndex >= scanPoints_.at(flatScanIndex).at(measurementId).size())
				return false;
#endif
			scanPoints_[flatScanIndex][measurementId][flatMeasurementIndex] = newValue;
			return true;
		}


		emitDataChanged(scanIndex, scanIndex, measurementId);
		return true;
	}

	/// Performance optimization of value(): this allows a block of multi-dimensional data to be retrieved in a single setValue call. The data is returned in a flat array, ordered in row-major form with the first scan index varying the slowest, and the measurement index's last axis varying the fastest.   /c scanIndexStart and \c scanIndexEnd specify the (inclusive) range in scan space; you can use the same start and end values to access the measurement values for a single scan point.  Which measurement to access is specified with \c measurementId, and \c measurementIndexStart and \c measurementIndexEnd specify the (inclusive) range in measurement space.  Returns false if any of the indexes are the wrong dimension or out of range.  It is the responsibility of the caller to make sure that \c outputValues is pre-allocated with enough room for all the data; use valuesSize() to calculate this conveniently.
	virtual bool values(const AMnDIndex& scanIndexStart, const AMnDIndex& scanIndexEnd, int measurementId, const AMnDIndex& measurementIndexStart, const AMnDIndex& measurementIndexEnd, double* outputValues) const {

		if(scanIndexStart.rank() != axes_.count() || scanIndexEnd.rank() != axes_.count())
			return false;
		if(measurementId >= measurements_.count())
			return false;

		const AMMeasurementInfo& mi = measurements_.at(measurementId);
		if(measurementIndexStart.rank() != mi.rank() || measurementIndexEnd.rank() != mi.rank())
			return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
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
#endif

		// Determine the full size of the measurement (not necessarily the size of the block that we want to read out).
		AMnDIndex measurementSize = mi.size();
		int flatMeasurementSize = measurementSize.product();

		// specific cases of scan rank:
		switch(scanIndexStart.rank()) {
		case 0: {
			// null scan space; just copy in the measurement block

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


	/// Performance optimization for setValue(): this allows multi-dimensional measurements to be set in a single setValue call.  \c inputData is interpreted as being in a flat array, ordered where the measurement's first axis varies the slowest, and the measurement's last axis varies the fastest (as you step through the array).  The size of the \c inputData must match the product of the sizes of all dimensions in the measurement.
	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const int* inputData) {
		// scan axis index doesn't provide enough / too many dimensions
		if(scanIndex.rank() != axes_.count())
			return false;

		if((unsigned)measurementId >= (unsigned)measurements_.count())
			return false;	// invalid measurement specified;

		// scalar scan space:
		if(axes_.count() == 0) {
			AMIMDSMeasurement& measurement = scalarScanPoint_[measurementId];
			for(int i=0,cc=measurement.size(); i<cc; ++i)
				measurement[i] = inputData[i];
			return true;
		}
		// higher dimension scan space
		else {
			int flatScanIndex = scanIndex.flatIndexInArrayOfSize(scanSize_);
#ifdef AM_ENABLE_BOUNDS_CHECKING
			if(flatScanIndex >= scanPoints_.count())
				return false;
#endif
			AMIMDSMeasurement& measurement = scanPoints_[flatScanIndex][measurementId];
			for(int i=0,cc=measurement.size(); i<cc; ++i)
				measurement[i] = inputData[i];
			return true;
		}

		emitDataChanged(scanIndex, scanIndex, measurementId);
		return true;
	}

	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const double* inputData) {
		// scan axis index doesn't provide enough / too many dimensions
		if(scanIndex.rank() != axes_.count())
			return false;

		if((unsigned)measurementId >= (unsigned)measurements_.count())
			return false;	// invalid measurement specified;

		// scalar scan space:
		if(axes_.count() == 0) {
			AMIMDSMeasurement& measurement = scalarScanPoint_[measurementId];
			for(int i=0,cc=measurement.size(); i<cc; ++i)
				measurement[i] = inputData[i];
			return true;
		}
		// higher dimension scan space:
		else {
			int flatScanIndex = scanIndex.flatIndexInArrayOfSize(scanSize_);
#ifdef AM_ENABLE_BOUNDS_CHECKING
			if(flatScanIndex >= scanPoints_.count())
				return false;
#endif
			AMIMDSMeasurement& measurement = scanPoints_[flatScanIndex][measurementId];
			for(int i=0,cc=measurement.size(); i<cc; ++i)
				measurement[i] = inputData[i];
			return true;
		}

		emitDataChanged(scanIndex, scanIndex, measurementId);
		return true;
	}




	/// Retrieve the independent variable along an axis \c axisId, at a specific scan point \c axisIndex.  If the axis scale is uniform (see AMAxisInfo::isUniform) this can be calculated from the axis' \c start and \c increment.
	virtual AMNumber axisValue(int axisId, int axisIndex) const {

		if((unsigned)axisId >= (unsigned)axes_.count())
			return AMNumber(AMNumber::InvalidError);	// invalid axis specified.

#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)axisIndex >= (unsigned)axes_.at(axisId).size)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

		const AMAxisInfo& ai = axes_.at(axisId);
		if(ai.isUniform)
			return (double)ai.start + axisIndex*(double)ai.increment;
		else
			return axisValues_.at(axisId).at(axisIndex);

	}

	/// Set the independent variable along an axis \c axisId, at a specific scan point \c axisIndex. This is necessary after adding a "row" with beginInsertRows(), unless the axis scale is uniform. (See AMAxisInfo::isUniform).
	virtual bool setAxisValue(int axisId, int axisIndex, AMNumber newValue) {

		if((unsigned)axisId >= (unsigned)axes_.count())
			return false;	// invalid axis specified.

#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)axisIndex >= (unsigned)axes_.at(axisId).size)
			return false;
#endif

		if(axes_.at(axisId).isUniform)
			return false;

		axisValues_[axisId][axisIndex] = newValue;
		return true;
	}





protected:
	/// Maintains the set of measurements that we have at each scan point
	QList<AMMeasurementInfo> measurements_;
	/// Maintains information about each scan axis
	QList<AMAxisInfo> axes_;
	/// This stores the size of each axis in axes_.  (The information is duplicated, but this version is needed for performance in some situations. They should always be updated together.)
	AMnDIndex scanSize_;

	/// If there are scan axes, this vector stores the flattened scan space
	QVector<AMIMDSScanPoint> scanPoints_;
	/// If there are no scan axes (a scalar scan space), this is the single scan point.
	AMIMDSScanPoint scalarScanPoint_;

	/// Storage for the independent variables (axis values). Indexed by axisId, and then by position along that axis.
	QVector<QVector<AMNumber> > axisValues_;


	/// Returns the flat array index for a multi-dimensional measurement.  For example, for a 4-dimensional \c measurementIndex AMnDIndex(3,4,5,600) and a measurement that has dimensions (5,10,100,1000), the flat index is 3*10*100*1000 + 4*100*1000 + 5*1000 + 600.
	/*! Note: assumes measurementId is a valid one of our measurements, and that measurementIndex is the right size for it.*/
	inline int flatIndexForMeasurement(int measurementId, const AMnDIndex& measurementIndex) const {
		return measurementIndex.flatIndexInArrayOfSize(measurements_.at(measurementId).size());
	}



	/// Implementing subclasses must provide a beginInsertRowsImplementation() which creates space for the new measurements.  When this function completes, it should be valid to setValue()s within the new scan space. Return false if the request is not possible (ie: out of memory, etc.)  You can assume that the pre-conditions for insert are satisfied: \c atRowIndex is valid (possibly equal to the size of the first axis for append, but no larger), and there is at least one scan axis.
	virtual bool beginInsertRowsImplementation(int numRows, int atRowIndex) {

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

	/// Implementing subclasses must provide a clearImplementation(), which removes all data values and sets the size of the first axis to 0.  It should leave the set of configured measurements as-is.
	virtual void clearScanDataPointsImplementation() {
		if(axes_.count() >= 1) {

			axes_[0].size = 0;
			scanSize_[0] = 0;

			scanPoints_.clear();
			axisValues_[0].clear();
		}
	}

	/// Implementing subclasses must provide a clearMeasurementsImplementation(), which clears the set of configured measurements.  They can assume that the set of scan data values is already cleared.
	virtual void clearMeasurementsImplementation() {
		scalarScanPoint_.clear();
		measurements_.clear();
	}

	/// Implementing subclasses must provide a clearScanAxesImplementation(), which clears all the axes for the scan.
	virtual void clearScanAxesImplementation()
	{
		axes_.clear();
		scanSize_ = AMnDIndex();
		axisValues_.clear();
	}


	/// Helper function: read a block out of a single measurement, from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  There is no error checking that the size or dimension of the indexes is correct, and \c outputValues must be pre-allocated with enough space to hold the results.
	void measurementValues(const AMIMDSMeasurement& measurement, const AMnDIndex& fullSize, const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const {

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

	/// Helper function to implement measurementValues() for arbitrary dimensions
	void measurementValuesImplementationRecursive(const AMIMDSMeasurement& measurement, const AMnDIndex& indexStart, const AMnDIndex& indexEnd, const AMnDIndex& fullSize, double** outputValues, int dimension, int cOffset) const {

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

	/// Helper function: read a complete measurement (\c fullSize points) directly into \c outputValues. This is faster than the other version of measurementValues when you want the whole thing.
	void measurementValues(const AMIMDSMeasurement& measurement, int fullSize, double* outputValues) const {
		for(int i=0; i<fullSize; ++i)
			*(outputValues++) = double(measurement.at(i));
	}

	/// Helper function: implements a nested for-loop up to the required number of scan dimensions; used by values() when there is more than 4 scan dimensions.
	void valuesImplementationRecursive(const AMnDIndex& siStart, const AMnDIndex& siEnd, int measurementId, const AMnDIndex& miStart, const AMnDIndex& miEnd, double** outputValues, int scanDimension, int scanSpaceOffset, const AMnDIndex& fullSize, int measurementSpaceSize) const {

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
};

#endif // AMINMEMORYDATASTORE_H
