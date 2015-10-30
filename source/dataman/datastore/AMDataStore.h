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


#ifndef AMDATASTORE_H
#define AMDATASTORE_H

#include "dataman/AMAxisInfo.h"
#include "dataman/AMnDIndex.h"
#include "dataman/AMMeasurementInfo.h"
#include <QList>

#include <QObject>

/// The AMDataStore defines an interface for various implementations which store multi-dimensional experimental data (either in memory, or to disk).
/*! <b>Concepts and terminology:<b>

  A 'detector' is a device that can record a (possibly multi-dimensional) measurement. [ex: scalar intensity value, 1D spectrum, or 2D image, etc.]
  A 'measurement' is a single instance of a reading by a detector.

  A 'scan' is sequence of measurements, conducted over a (possibly multi-dimensional) space.  For every scan point, a measurement is recorded for each detector. [ex: 1D XAS scan over incident energy, 2D microprobe scan over a sample's surface, 3D scan combining microprobe with XAS, etc.]

  A 'value' is a scalar number, which can be specified completely with:
	- a detector ID (which detector?)
	- a complete set of indexes into the scan space (where in the scan?)
	- AND a complete set of indexes into the measurement dimensions

  <b>Datastore Assumptions and Restrictions</b>

  - The data store is organized to support a set of (one or more) detectors.  A measurement for ALL detectors must be stored for EACH scan point.
  - The scan space is regular and complete (not sparse). The size of other scan dimensions does not vary as a function of any scan dimension. (If certain points in the scan space are not measured, they should return null values.)
  - Every measurement by the same detector has the same dimensionality.

Two more restrictions were added after commit f846098ca399b:
  - The length of the first scan axis is originally 0, and can be extended by inserting or appending using beginInsertRows()/endInsertRows(). However, the length of all additional scan axes must be specified at the time they are defined with addScanAxis(), and cannot be changed later.
- Additionally, you cannot add additional scan axes (using addScanAxis()) after rows and data points have been inserted.

[These restrictions were added to allow good performance and simple implementations for disk-based data stores that employ contiguous multi-dimensional array storage; it is only possible to append to their slowest-changing array dimension. Any other operations require inserting entries between each existing entry, which would require copying all records of a (possibly very large) data set.]


  <b>API Terminology Clarification</b>
  Within the AMDataStore interface, we avoid talking about detectors (since they have nothing to do with storage). The functions that refer to 'measurements' in the API are actually referring to 'measurement classes' instead of single measurements (effectively, 'detectors' according to the terminology above).  For example, addMeasurement() is used to create space to store an additional measurement <i>for every scan point</i>... To match the terminology above, we should have said 'addDetector()' instead.
  */

class AMDataStore : public QObject
{
	Q_OBJECT

public:
	/// Constructs an empty data store
	AMDataStore(QObject* parent = 0);
	/// Destructor
	virtual ~AMDataStore();

	// Measurements
	///////////////////////////////
	/// Creates space to support an additional measurement at every scan point. \c measurementDetails describes the dimensionality and details of the new set of measurements.  (If a set of scan points exist already, the initial values for this measurement at those scan points should be Null.)
	/*! If you want to retrieve measurements by name, \c measurementDetails must contain a unique \c name.  This function should return false if a measurement with that name already exists. */
	virtual bool addMeasurement(const AMMeasurementInfo& measurementDetails) = 0;
	/// Retrieve the id of an existing set of measurements, by name.  (Depending on the implementation, this may not be fast. Avoid calling it repeatedly.) Returns -1 if not found.
	virtual int idOfMeasurement(const QString& measurementName) const = 0;
	/// Retrieve information about a set of measurements, by id. \c id must be >= 0 and < measurementCount().
	virtual AMMeasurementInfo measurementAt(int id) const = 0;
	/// Return the number of measurements stored for each scan point
	virtual int measurementCount() const = 0;


	// Axes
	/////////////////////////////
	/// Create space to support an (additional) scan axis.  \c axisDetails describes the characteristics of the axis, but if this is the first axis to be added, the \c axisDetails.size will be set to 0.  All subsequent axes must specify their final size (>0), and this size cannot be changed later. (ie: Only the first axis can be extended via beginInsertRows()).  Note that it is also impossible to add scan axes after rows have already been added with beginInsertRows(). In that case, this function should return false.
	/*! These restrictions are in place to simplify behaviour and increase performance for implementations.

If you want to retrieve axes by name, \c axisDetails must contain a unique \c name.  This function should return false if an axis with that name already exists.

	  \note No signalling is provided for alerting observers of new scan axes. It's also prohibited for AMDataSources that expose this data (for ex: AMRawDataSource) to change dimensionality (ie: add another axis). Therefore, it's recommended to only call this function when first setting up a dataStore, before any observers get involved.
*/
	virtual bool addScanAxis(const AMAxisInfo& axisDetails) = 0;
	/// Retrieve the id of an existing axis, by name.  (Depending on the implementation, this may not be fast. Avoid calling it repeatedly.)  Returns -1 if not found.
	virtual int idOfScanAxis(const QString& axisName) const = 0;
	/// Retrieve information about an axis, by id.  \c id must be >= 0 and < scanAxesCount().
	virtual AMAxisInfo scanAxisAt(int id) const = 0;
	/// Return the number of scan axes
	virtual int scanAxesCount() const = 0;
	/// Synonym for scanAxisCount()
	int scanRank() const { return scanAxesCount(); }
	/// Return the sizes of all the scan axes, in order.
	virtual AMnDIndex scanSize() const = 0;
	/// Return the size along a specific axis, by \c id.  (\c id must be >= 0 and < scanAxesCount().)
	virtual long scanSize(int axisId) const = 0;

	/// Indicates that the scan space is empty (no scan points yet). This is true when the size of the first axis is 0.
	bool scanSpaceIsEmpty() const {
		return (scanRank()==0 || scanSize(0)==0);
	}


	// Setting and getting values
	////////////////////////////////
	/// Retrieve a value from a measurement, at a specific scan point.
	virtual AMNumber value(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex) const = 0;
	/// Retrieve the independent variable along an axis \c axisId, at a specific scan point \c axisIndex.  If the axis scale is uniform (see AMAxisInfo::isUniform) this can be calculated from the axis' \c start and \c increment.
	virtual AMNumber axisValue(int axisId, long axisIndex) const = 0;
	/// Set the value of a measurement, at a specific scan point
	virtual bool setValue(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex, const AMNumber& newValue) = 0;
	/// Set the independent variable along an axis \c axisId, at a specific scan point \c axisIndex. This is necessary after adding a row with beginInsertRows(), unless the axis scale is uniform. (See AMAxisInfo::isUniform).
	virtual bool setAxisValue(int axisId, long axisIndex, AMNumber newValue) = 0;


	/// Performance optimization of axisValue().  This will provide all the axis values desired in a single call.  Requires the id of the axis and the range of axis values you request.  Lastly, you need to provide a valid pointer to accept the new data tha must have the necessary space to hold the requested data.
    virtual bool axisValues(int axisId, long axisStartIndex, long axisEndIndex, double *outputValues) const = 0;
	/// Performance optimization of value(): this allows a block of multi-dimensional data to be retrieved in a single call. The data is returned in a flat array, ordered in row-major form with the first scan index varying the slowest, and the measurement index's last axis varying the fastest.   /c scanIndexStart and \c scanIndexEnd specify the (inclusive) range in scan space. You can use the same start and end values to access the measurement values for a single scan point.  Which measurement to access is specified with \c measurementId, and \c measurementIndexStart and \c measurementIndexEnd specify the (inclusive) data block in measurement space.  Returns false if any of the indexes are the wrong dimension or out of range.  It is the responsibility of the caller to make sure that \c outputValues is pre-allocated with enough room for all the data. Use valuesSize() to calculate this conveniently.
	virtual bool values(const AMnDIndex& scanIndexStart, const AMnDIndex& scanIndexEnd, int measurementId, const AMnDIndex& measurementIndexStart, const AMnDIndex& measurementIndexEnd, double* outputValues) const = 0;
	/// Synonym for values(), but allocates and returns a QVector of doubles. Implemented in the base class by calling values(). Implementations do not need to provide this version.
	QVector<double> values(const AMnDIndex &scanIndexStart, const AMnDIndex &scanIndexEnd, int measurementId, const AMnDIndex &measurementIndexStart, const AMnDIndex &measurementIndexEnd) const;

	/// Calculates the total size of the array required for values(), ie: spanned by \c scanIndexStart, \c scanIndexEnd, \c measurementIndexStart, and \c measurementIndexEnd. Returns -1 if the dimensionality of the start and end indexes don't agree.
	/*! \note This does a simple multiplication of the dimension ranges; it does not check that the indexes have the right dimensionality or size for the current data store. */
	int valuesSize(const AMnDIndex& scanIndexStart, const AMnDIndex& scanIndexEnd, const AMnDIndex& measurementIndexStart, const AMnDIndex& measurementIndexEnd) const;

	/// Performance optimization for setValue(): this allows multi-dimensional measurements to be set in a single setValue call.  \c inputData is interpreted as being in a flat array, ordered in row-major form (where the measurement's first axis varies the slowest, and the measurement's last axis varies the fastest as you step through the array).  The size of the \c inputData must match the product of the sizes of all dimensions in the measurement.
	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const int* inputData) = 0;
	/// Performance optimization for setValue(): this allows multi-dimensional measurements to be set in a single setValue call.  \c inputData is interpreted as being in a flat array, ordered in row-major form (where the measurement's first axis varies the slowest, and the measurement's last axis varies the fastest as you step through the array).  The size of the \c inputData must match the product of the sizes of all dimensions in the measurement.
	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const double* inputData) = 0;


	// Adding new data points: increasing the size of the scan space
	//////////////////////////////////////////////////////
	/// Call this to increase the size of the dataset along the first scan axis.  (The length of all the other scan axes are fixed from the time they are created with addScanAxis().) Rows are inserted starting \c atRowIndex. (Use \c atRowIndex = -1 to append to the end.)  Returns true on success.
	/*! After calling this, you must call setValue and setAxisValue to fill in the data for all the new scan points that were created, and then call endInsertRows().

	  This function, in addition to creating space for the new data, suppresses the dataChanged() signal until endInsertRows() is called.  This allows you to insert valid data for all detectors and scan points within the new space, and the dataChanged() signal is only emitted once for the whole affected region.

	 Subclasses should re-implement beginInsertRowsImplementation() instead of this function.
	*/
	bool beginInsertRows(long numRows, long atRowIndex);


	/// Call this after calling beginInsertRows() and setting all the new measurement values with setValue() and setAxisValue().
	/*! The base class version will emit the sizeChanged() and dataChanged() signals for the region.  Subclasses that want to know when an insert is complete can re-implement endInsertRowsImplementation().
		*/
	void endInsertRows();


	// Removing data points: not supported. It's a storage machine, ok?
	////////////////////////////////////////////////////////////////////

	// Ok, I guess you can clear the whole thing
	////////////////////////////////////////////////////
	/// Clear all the data. This maintains the set of measurements and the scan axes, but deletes every point in the scan space. The size of every scan axis will become 1, except for the first axis, which will have size 0. Implementing subclasses must provide a clearScanDataPointsImplementation().
	void clearScanDataPoints() {
		clearScanDataPointsImplementation();
		emit sizeChanged();
	}
	/// Clear the entire data set, and also delete all configured measurements.  This function calls clearScanDataPoints() first.  Implementing subclasses must provide a clearMeasurementsImplementation().
	void clearAllMeasurements() {
		clearScanDataPoints();
		clearMeasurementsImplementation();
	}

	/// Clears the entire data set, and also delete all configured scan axes.  This function calls clearScanDataPoints() first.  Implementing subclasses must provide a clearScanAxesImplementation().
	void clearAllScanAxes() {
		clearScanDataPoints();
		clearScanAxesImplementation();
	}

	/// Clears all data, including all configured measurements and scan axes.
	void clearAll() {
		clearScanDataPoints();
		clearMeasurementsImplementation();
		clearScanAxesImplementation();
	}

	// Signals
	///////////////////////

signals:
	/// Emitted whenever the values() change. The changed data comes from \c measurementId, between \c scanIndexStart and \c scanIndexEnd. (If \c scanIndexStart is an invalid AMnDIndex, it means that all the data has changed.)
	/*! Implementations should not emit this directly, but instead call emitDataChanged(). */
	void dataChanged(const AMnDIndex& scanIndexStart, const AMnDIndex& scanIndexEnd, int measurementId);
	/// Emitted whenever the size of the first scan axis changes.
	/*! This is emitted automatically by the base-class implementation via endInsertRows(); implementations do not need to emit this explicitly. */
	void sizeChanged();


protected:
	/// Implementing subclasses must provide a beginInsertRowsImplementation() which creates space for the new measurements.  When this function completes, it should be valid to setValue()s within the new scan space. Return false if the request is not possible (ie: out of memory, etc.)  You can assume that the pre-conditions for insert are satisfied: \c atRowIndex is valid (possibly equal to the size of the first scan axis for append, but no larger), and there is at least one scan axis.
	virtual bool beginInsertRowsImplementation(long numRows, long atRowIndex) = 0;

	/// Implementing subclasses may provide a endInsertRowsImplementation(), which will be called at the beginning of endInsertRows(). You don't need to take care of emitting sizeChanged() and dataChanged().... that's done for you in endInsertRows().  The base class implementation of this does nothing.
	virtual void endInsertRowsImplementation(long numRows, long atRowIndex) {
		Q_UNUSED(numRows);
		Q_UNUSED(atRowIndex);
	}

	/// Implementing subclasses must provide a clearScanDataPointsImplementation(), which removes all data values and sets the size of the first scan axis to 0.  It should leave the set of configured measurements as-is.
	virtual void clearScanDataPointsImplementation() = 0;
	/// Implementing subclasses must provide a clearMeasurementsImplementation(), which clears the set of configured measurements.  They can assume that the data values have already been cleared with clearScanDataPoints().
	virtual void clearMeasurementsImplementation() = 0;
	/// Implementing subclasses must provide a clearScanAxesImplementation(), which clears all the configured scan axes. They can assume that the data values have already been cleared with clearScanDataPoints().
	virtual void clearScanAxesImplementation() = 0;

	/// Implementing subclasses must call this whenever a measurement value changes. (For example, in their setValue implementation).  \c scanIndexStart and \c scanIndexEnd describe the scan range affected.  Use an invalid \c scanIndexStart to indicate the whole scan space is affected.  For performance, subclasses can opt to avoid calling this on every setValue(), and combine multiple emits into one (as long as the last one covers the complete affected range.)  A separate emitDataChanged() should be sent for each different measurement.  For multi-dimensional measurements, it's suggested to delay emits until all the values for a complete measurement have been received (if you can figure out how...).
	void emitDataChanged(const AMnDIndex& scanIndexStart, const AMnDIndex& scanIndexEnd, int measurementId) {
		/// Don't emit while inserting rows... Instead, dataChanged will be emitted for the whole region when done inserting rows.
		if(isInsertingRows_)
			return;	/// \todo Check if the scanIndexStart and scanIndexEnd represent a range that extends outside of the inserting region. If they do, emit anyway.

		emit dataChanged(scanIndexStart, scanIndexEnd, measurementId);
	}



private:
	bool isInsertingRows_;
	long insertingAtRowIndex_, insertingNumRows_;

};

#endif // AMDATASTORE_H
