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


#ifndef AMINMEMORYDATASTORE_H
#define AMINMEMORYDATASTORE_H

#include <QVector>
#include "dataman/AMNumber.h"
#include "dataman/datastore/AMDataStore.h"


/// (Internal class for AMInMemoryDataStore) One instance stores a single measurement. (Flat array, nD measurement data is stored in row-major order. Last index varies the fastest.)
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
	Q_OBJECT
public:
	/// Constructs an empty data store.
	AMInMemoryDataStore(QObject* parent = 0);

	virtual ~AMInMemoryDataStore();

	/// Creates space to support an additional measurement at every scan point. \c measurementDetails describes the dimensionality and details of the new set of measurements.  (If a set of scan points exist already, the initial values for this measurement at those scan points should be Null.)
	/*! If you want to retrieve measurements by name, \c measurementDetails must contain a unique \c name.  This function should return false if a measurement with that name already exists. */
	virtual bool addMeasurement(const AMMeasurementInfo& measurementDetails);

	/// Retrieve the id of an existing set of measurements, by name.  (Depending on the implementation, this may not be fast. Avoid calling it repeatedly.) Returns -1 if not found.
	virtual int idOfMeasurement(const QString& measurementName) const;

	/// Retrieve information about a set of measurements, by id. \c id assumed to be >= 0 and < measurementCount().
	virtual AMMeasurementInfo measurementAt(int id) const {
		return measurements_.at(id);
	}

	/// Return the number of measurements stored for each scan point
	virtual int measurementCount() const {
		return measurements_.count();
	}




	/// Create space to support an (additional) scan axis.  \c axisDetails describes the characteristics of the axis, but if this is the first axis to be added, the \c axisDetails.size will be set to 0.  All subsequent axes must specify their final size (>0), and this size cannot be changed later. (ie: Only the first axis can be extended via beginInsertRows()).  Note that it is also impossible to add scan axes after rows have already been added with beginInsertRows(). In that case, this function should return false.
	/*! These restrictions are in place to simplify behaviour and increase performance for implementations.

If you want to retrieve axes by name, \c axisDetails must contain a unique \c name.  This function should return false if an axis with that name already exists.

   \note No signalling is provided for alerting observers of new scan axes. It's also prohibited for AMDataSources that expose this data (for ex: AMRawDataSource) to change dimensionality (ie: add another axis). Therefore, it's recommended to only call this function when first setting up a dataStore, before any observers get involved.
*/
	virtual bool addScanAxis(const AMAxisInfo& axisDetails);

	/// Retrieve the id of an existing axis, by name.
	virtual int idOfScanAxis(const QString& axisName) const;
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
	virtual long scanSize(int axisId) const {
		return scanSize_.at(axisId);
	}

	/// Retrieve a value from a measurement, at a specific scan point.
	virtual AMNumber value(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex) const;

	/// Set the value of a measurement, at a specific scan point
	virtual bool setValue(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex, const AMNumber& newValue);

	/// Performance optimization of value(): this allows a block of multi-dimensional data to be retrieved in a single setValue call. The data is returned in a flat array, ordered in row-major form with the first scan index varying the slowest, and the measurement index's last axis varying the fastest.   /c scanIndexStart and \c scanIndexEnd specify the (inclusive) range in scan space. You can use the same start and end values to access the measurement values for a single scan point.  Which measurement to access is specified with \c measurementId, and \c measurementIndexStart and \c measurementIndexEnd specify the (inclusive) range in measurement space.  Returns false if any of the indexes are the wrong dimension or out of range.  It is the responsibility of the caller to make sure that \c outputValues is pre-allocated with enough room for all the data. Use valuesSize() to calculate this conveniently.
	virtual bool values(const AMnDIndex& scanIndexStart, const AMnDIndex& scanIndexEnd, int measurementId, const AMnDIndex& measurementIndexStart, const AMnDIndex& measurementIndexEnd, double* outputValues) const;


	/// Performance optimization for setValue(): this allows multi-dimensional measurements to be set in a single setValue call.  \c inputData is interpreted as being in a flat array, ordered where the measurement's first axis varies the slowest, and the measurement's last axis varies the fastest (as you step through the array).  The size of the \c inputData must match the product of the sizes of all dimensions in the measurement.
	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const int* inputData);

	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const double* inputData);

	/// Retrieve the independent variable along an axis \c axisId, at a specific scan point \c axisIndex.  If the axis scale is uniform (see AMAxisInfo::isUniform) this can be calculated from the axis' \c start and \c increment.
	virtual AMNumber axisValue(int axisId, long axisIndex) const;
	/// Performance optimization of axisValue().  This will provide all the axis values desired in a single call.  Requires the id of the axis and the range of axis values you request.  Lastly, you need to provide a valid pointer to accept the new data tha must have the necessary space to hold the requested data.
    virtual bool axisValues(int axisId, long axisStartIndex, long axisEndIndex, double *outputValues) const;

	/// Set the independent variable along an axis \c axisId, at a specific scan point \c axisIndex. This is necessary after adding a "row" with beginInsertRows(), unless the axis scale is uniform. (See AMAxisInfo::isUniform).
	virtual bool setAxisValue(int axisId, long axisIndex, AMNumber newValue);


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
	virtual bool beginInsertRowsImplementation(long numRows, long atRowIndex);

	/// Implementing subclasses must provide a clearImplementation(), which removes all data values and sets the size of the first axis to 0.  It should leave the set of configured measurements as-is.
	virtual void clearScanDataPointsImplementation();

	/// Implementing subclasses must provide a clearMeasurementsImplementation(), which clears the set of configured measurements.  They can assume that the set of scan data values is already cleared.
	virtual void clearMeasurementsImplementation();

	/// Implementing subclasses must provide a clearScanAxesImplementation(), which clears all the axes for the scan.
	virtual void clearScanAxesImplementation();


	/// Helper function: read a block out of a single measurement, from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  There is no error checking that the size or dimension of the indexes is correct, and \c outputValues must be pre-allocated with enough space to hold the results.
	void measurementValues(const AMIMDSMeasurement& measurement, const AMnDIndex& fullSize, const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;

	/// Helper function to implement measurementValues() for arbitrary dimensions
	void measurementValuesImplementationRecursive(const AMIMDSMeasurement& measurement, const AMnDIndex& indexStart, const AMnDIndex& indexEnd, const AMnDIndex& fullSize, double** outputValues, int dimension, int cOffset) const;

	/// Helper function: read a complete measurement (\c fullSize points) directly into \c outputValues. This is faster than the other version of measurementValues when you want the whole thing.
	void measurementValues(const AMIMDSMeasurement& measurement, int fullSize, double* outputValues) const;

	/// Helper function: implements a nested for-loop up to the required number of scan dimensions. Used by values() when there is more than 4 scan dimensions.
	void valuesImplementationRecursive(const AMnDIndex& siStart, const AMnDIndex& siEnd, int measurementId, const AMnDIndex& miStart, const AMnDIndex& miEnd, double** outputValues, int scanDimension, int scanSpaceOffset, const AMnDIndex& fullSize, int measurementSpaceSize) const;

};

#endif // AMINMEMORYDATASTORE_H
