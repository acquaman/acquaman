#ifndef AMCDFDATASTORE_H
#define AMCDFDATASTORE_H

#include "dataman/datastore/AMDataStore.h"
#include <QString>
#include <QList>

#include "dataman/AMAxisInfo.h"
#include "dataman/AMMeasurementInfo.h"

/// This class implements multi-dimensional storage of scan data, according to the AMDataStore interface.  It is optimized for scan data sets that are too large to store completely in memory.  Data is stored on disk in a CDF formatted file, but accessing the values should be reasonably fast thanks to reading and caching of chunks at a time.  Like AMInMemoryDataStore, using the values() method to access data is much faster than reading by individual value().
class AMCDFDataStore : public AMDataStore
{
public:
	/// Construct an empty data store.  The data store uses a temporary file created in the system-appropriate temp space (ex: /tmp)
    AMCDFDataStore();
	/// Destructor. If the data store was using a temporary file, it will be deleted.
	virtual ~AMCDFDataStore();


	// Measurements
	///////////////////////////////

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


	// Scan axes:
	/////////////////////////////////
	/// Create space to support an (additional) scan axis.  \c axisDetails describes the characteristics of the axis, but if this is the first axis to be added, the \c axisDetails.size will be set to 0.  All subsequent axes must specify their final size (>0), and this size cannot be changed later. (ie: Only the first axis can be extended via beginInsertRows()).  Note that it is also impossible to add scan axes after rows have already been added with beginInsertRows(); in that case, this function should return false.
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


	// Accessing Values:
	////////////////////////////////
	/// Retrieve a value from a measurement, at a specific scan point.
	virtual AMNumber value(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex) const;

	/// Performance optimization of value(): this allows a block of multi-dimensional data to be retrieved in a single setValue call. The data is returned in a flat array, ordered in row-major form with the first scan index varying the slowest, and the measurement index's last axis varying the fastest.   /c scanIndexStart and \c scanIndexEnd specify the (inclusive) range in scan space; you can use the same start and end values to access the measurement values for a single scan point.  Which measurement to access is specified with \c measurementId, and \c measurementIndexStart and \c measurementIndexEnd specify the (inclusive) range in measurement space.  Returns false if any of the indexes are the wrong dimension or out of range.  It is the responsibility of the caller to make sure that \c outputValues is pre-allocated with enough room for all the data; use valuesSize() to calculate this conveniently.
	virtual bool values(const AMnDIndex& scanIndexStart, const AMnDIndex& scanIndexEnd, int measurementId, const AMnDIndex& measurementIndexStart, const AMnDIndex& measurementIndexEnd, double* outputValues) const;

	// Setting values:
	///////////////////////////////
	/// Set the value of a measurement, at a specific scan point
	virtual bool setValue(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex, const AMNumber& newValue);

	/// Performance optimization for setValue(): this allows multi-dimensional measurements to be set in a single setValue call.  \c inputData is interpreted as being in a flat array, ordered where the measurement's first axis varies the slowest, and the measurement's last axis varies the fastest (as you step through the array).  The size of the \c inputData must match the product of the sizes of all dimensions in the measurement.
	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const int* inputData);
	/// Performance optimization for setValue(): this allows multi-dimensional measurements to be set in a single setValue call.  \c inputData is interpreted as being in a flat array, ordered where the measurement's first axis varies the slowest, and the measurement's last axis varies the fastest (as you step through the array).  The size of the \c inputData must match the product of the sizes of all dimensions in the measurement.
	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const double* inputData);

	// Independent (axis) values for the scan axes:
	/////////////////////////////////////////////////

	/// Retrieve the independent variable along an axis \c axisId, at a specific scan point \c axisIndex.  If the axis scale is uniform (see AMAxisInfo::isUniform) this can be calculated from the axis' \c start and \c increment.
	virtual AMNumber axisValue(int axisId, long axisIndex) const;

	/// Set the independent variable along an axis \c axisId, at a specific scan point \c axisIndex. This is necessary after adding a "row" with beginInsertRows(), unless the axis scale is uniform. (See AMAxisInfo::isUniform).
	virtual bool setAxisValue(int axisId, long axisIndex, AMNumber newValue);


protected:

	// Re-implemented protected functions:
	////////////////////////////////////////

	/// Implementing subclasses must provide a beginInsertRowsImplementation() which creates space for the new measurements.  When this function completes, it should be valid to setValue()s within the new scan space. Return false if the request is not possible (ie: out of memory, etc.)  You can assume that the pre-conditions for insert are satisfied: \c atRowIndex is valid (possibly equal to the size of the first axis for append, but no larger), and there is at least one scan axis.
	virtual bool beginInsertRowsImplementation(long numRows, long atRowIndex);

	/// Implementing subclasses must provide a clearImplementation(), which removes all data values and sets the size of the first axis to 0.  It should leave the set of configured measurements as-is.
	virtual void clearScanDataPointsImplementation();

	/// Implementing subclasses must provide a clearMeasurementsImplementation(), which clears the set of configured measurements.  They can assume that the set of scan data values is already cleared.
	virtual void clearMeasurementsImplementation();

	/// Implementing subclasses must provide a clearScanAxesImplementation(), which clears all the axes for the scan.
	virtual void clearScanAxesImplementation();

	// Variables
	/////////////////////
	/// Full path to the CDF file on disk.
	QString cdfFilePath_;
	/// True if cdfFilePath_ is a temporary file created just for this instance, and should be deleted when going out of scope.
	bool fileIsTemporary_;
	/// CDF id of the open file
	void* cdfId_;

	/// Maintains the set of measurements that we have at each scan point
	QList<AMMeasurementInfo> measurements_;
	/// This list is the same size as measurements_, and holds the CDF var num for each
	QList<long> measurementVarNums_;
	/// Maintains information about each scan axis
	QList<AMAxisInfo> axes_;
	/// This list is the same size as axes_, and holdes the CDF var num for the axis values along that axis (or -1 if the axis is uniform and we are not storing it.)
	QList<long> axisValueVarNums_;
	/// This stores the size of each axis in axes_.  (The information is duplicated, but this version is needed for performance in some situations. They should always be updated together.)
	AMnDIndex scanSize_;


	// Helper Functions:
	//////////////////////////////

	/// Generates a safe temporary file name in the QDir::tempPath() directory. Returns the full path.
	QString tempFileName() const;

	/// Creates a CDF variable for a measurement, and instantiates \c numInitialRecords records.
	bool createVarForMeasurement(const AMMeasurementInfo& mi, long numInitialRecords);

	/// Implements values() for scan ranks larger than 4.
	bool valuesImplementationRecursive(const AMnDIndex &siStart, const AMnDIndex &siEnd, long varNum, const long* miStart, const long* miSize, const long* miInterval, double **outputValues, long flatReadSize, int currentDimension, long scanSpaceOffset) const;

};

#endif // AMCDFDATASTORE_H
