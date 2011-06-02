/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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



class AMDataStore;

#include <QObject>
/// This class acts as a proxy to emit signals for AMDataStore. You can receive the dataChanged() signal by hooking up to AMDataStore::signalSource().  You should never need to create an instance of this class directly.
/*! To allow classes that implement AMDataStore to also inherit QObject, AMDataStore does NOT inherit QObject.  However, it still needs a way to emit signals notifying of changes to the data, which is the role of this class.
  */
class AMDataStoreSignalSource : public QObject {
	Q_OBJECT
public:
	AMDataStore* dataStore() const { return data_; }

protected slots:
	void emitDataChanged(const AMnDIndex& scanIndexStart, const AMnDIndex& scanIndexEnd, int measurementId) {
		emit dataChanged(scanIndexStart, scanIndexEnd, measurementId);
	}
	void emitSizeChanged(int axisId) { emit sizeChanged(axisId); }

protected:
	AMDataStoreSignalSource(AMDataStore* parent);
	AMDataStore* data_;
	friend class AMDataStore;

signals:
	void dataChanged(const AMnDIndex& scanIndexStart, const AMnDIndex& scanIndexEnd, int measurementId);
	void sizeChanged(int axisId);
};

/// The AMDataStore defines an interface for various implementations which store multi-dimensional experimental data (either in memory, or to disk).
/*! <b>Concepts and terminology:<b>

  A 'detector' is a device that can record a (possibly multi-dimensional) measurement. [ex: scalar intensity value, 1D spectrum, or 2D image, etc.]
  A 'measurement' is a single instance of a reading by a detector.

  A 'scan' is sequence of measurements, conducted over a (possibly multi-dimensional) space.  For every scan point, a measurement is recorded for each detector. [ex: 1D XAS scan over incident energy, 2D microprobe scan over a sample's surface, 3D scan combining microprobe and XAS, etc.]

  A 'value' is a scalar number, which can be specified completely with:
	- a detector ID (which detector?)
	- a complete set of indexes into the scan space (where in the scan?)
	- AND a complete set of indexes into the measurement dimensions

  <b>Datastore Assumptions and Restrictions</b>

  - The data store is organized to support a set of (one or more) detectors.  A measurement for ALL detectors must be stored for EACH scan point.
  - The scan space is regular and complete (not sparse). The size of other scan dimensions does not vary as a function of any scan dimension. (If certain points in the scan space are not measured, they should return null values.)
  - Every measurement by the same detector has the same dimensionality.


  <b>API Terminology Clarification</b>
  Within the AMDataStore interface, we avoid talking about detectors (since they have nothing to do with storage). The functions that refer to 'measurements' in the API are actually referring to 'measurement classes' instead of single measurements (effectively, 'detectors' according to the terminology above).  For example, addMeasurement() is used to create space to store an additional measurement <i>for every scan point</i>... To match the terminology above, we should have said 'addDetector()' instead.
  */

class AMDataStore
{
public:
	/// Constructs an empty data store
	AMDataStore();
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
	/// Create space to support an additional scan axis.  \c axisDetails describes the characteristics of the axis, but the \c size of axisDetails will be ignored.  If this is the first axis to be added, the size will be set to 0; otherwise it will be set to 1.
	/*! If you want to retrieve axes by name, \c axisDetails must contain a unique \c name.  This function should return false if an axis with that name already exists.

	  \note No signalling is provided for alerting observers of new scan axes. It's also prohibited for AMDataSources that expose this data (for ex: AMRawDataSource) to change dimensionality (ie: add another axis). Therefore, it's recommended to only call this function when first setting up a dataStore, before any observers get involved.
*/
	virtual bool addScanAxis(const AMAxisInfo& axisDetails) = 0;
	/// Retrieve the id of an existing axis, by name.  (Depending on the implementation, this may not be fast. Avoid calling it repeatedly.)  Returns -1 if not found.
	virtual int idOfScanAxis(const QString& axisName) const = 0;
	/// Retrieve information about an axis, by id.  \c id must be >= 0 and < scanAxesCount().
	virtual const AMAxisInfo scanAxisAt(int id) const = 0;
	/// Return the number of scan axes
	virtual int scanAxesCount() const = 0;
	/// Synonym for scanAxisCount()
	int scanRank() const { return scanAxesCount(); }
	/// Return the sizes of all the scan axes, in order.
	virtual AMnDIndex scanSize() const = 0;
	/// Return the size along a specific axis, by \c id.  (\c id must be >= 0 and < scanAxesCount().)
	virtual int scanSize(int axisId) const = 0;

	/// Indicates that the scan space is empty (no scan points yet). This is true when the size of any axis is 0.
	virtual bool isEmpty() const {
		int rank = scanAxesCount();
		for(int mu=0; mu<rank; mu++)
			if(scanSize(mu) == 0)
				return true;
		return false;
	}


	// Setting and getting values
	////////////////////////////////
	/// Retrieve a value from a measurement, at a specific scan point.
	virtual AMNumber value(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex, bool doBoundsChecking = true) const = 0;
	/// Retrieve the independent variable along an axis \c axisId, at a specific scan point \c axisIndex.  If the axis scale is uniform (see AMAxisInfo::isUniform) this can be calculated from the axis' \c start and \c increment.
	virtual AMNumber axisValue(int axisId, int axisIndex, bool doBoundsChecking = true) const = 0;
	/// Set the value of a measurement, at a specific scan point
	virtual bool setValue(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex, const AMNumber& newValue, bool doBoundsChecking = true) = 0;
	/// Set the independent variable along an axis \c axisId, at a specific scan point \c axisIndex. This is necessary after adding a "row" with beginInsertRows(), unless the axis scale is uniform. (See AMAxisInfo::isUniform).
	virtual bool setAxisValue(int axisId, int axisIndex, AMNumber newValue, bool doBoundsChecking = true) = 0;



	/// Performance optimization for setValue(): this allows multi-dimensional measurements to be set in a single setValue call.  \c inputData is interpreted as being in a flat array, ordered where the measurement's first axis varies the slowest, and the measurement's last axis varies the fastest (as you step through the array).  The size of the \c inputData must match the product of the sizes of all dimensions in the measurement.
	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const int* inputData, int numArrayElements, bool doBoundsChecking = true) = 0;
	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const double* inputData, int numArrayElements, bool doBoundsChecking = true) = 0;


	// Adding new data points: increasing the size of the scan space
	//////////////////////////////////////////////////////
	/// Call this to increase the size of the dataset along a scan axis \c axisId. "Rows" (or columns, or slices, or whatever they might be depending on the axis in question) are inserted starting \c atRowIndex.  (Use -1 to append to the end.)  Returns true on success.
	/*! After calling this, you must call setValue and setAxisValue to fill in the data for all the new scan points that were created, and then call endInsertRows().  Nested calls to beginInsertRows()/endInsertRows() to increase the size along multiple dimensions simultaneously are not supported.

	  This function, in addition to creating space for the new data, suppresses the dataChanged() signal until endInsertRows() is called.  This allows you to insert valid data for all detectors and scan points within the new space, and the dataChanged() signal is only emitted once for the whole affected region.

	  If the scan space is currently empty, the size of the first scan axis is 0.  In the future, we will change the behaviour of this function so that inserting rows to any axis automatically ensures the size of each axis is at least 1.

	 Subclasses should re-implement beginInsertRowsImplementation() instead of this function.
	*/
	bool beginInsertRows(int axisId, int numRows = 1, int atRowIndex = -1);


	/// Call this after calling beginInsertRows() and setting all the new measurement values with setValue() and setAxisValue().
	/*! The base class version will emit the sizeChanged() and dataChanged() signals for the region.  Subclasses that want to know when an insert is complete can re-implement endInsertRowsImplementation().
		*/
	void endInsertRows();

	/// This version of beginInsertRows() automatically inserts rows along all axes as required to make sure you can store data at the scan index \c scanIndex.  Call endInsertRows() once after this function, once you are done filling the new space with data using setValue().  Any un-set values will be accessible, but null.
	bool beginInsertRowsAsNecessaryForScanPoint(const AMnDIndex& scanIndex);


	// Removing data points: not supported. It's a storage machine, ok?
	////////////////////////////////////////////////////////////////////

	// Ok, I guess you can clear the whole thing
	////////////////////////////////////////////////////
	/// Clear the entire data set. This maintains the set of measurements, but deletes every point in the scan space. The size of every scan axis will become 0. Implementing subclasses must provide a clearImplementation().
	void clearScanDataPoints() {
		clearScanDataPointsImplementation();
		emitSizeChanged(-1);
	}
	/// Clear the entire data set, and also delete all configured measurements.  This function calls clearScanDataPoints() first.  Implementing subclasses must provide a clearMeasurementsImplementation().
	void clearAllMeasurements() {
		clearScanDataPoints();
		clearMeasurementsImplementation();
	}



	// Signal source
	///////////////////////
	/// Access the signal source, which provides dataChanged() and sizeChanged().
	const AMDataStoreSignalSource* signalSource() const { return signalSource_; }


protected:
	/// Implementing subclasses must provide a beginInsertRowsImplementation() which creates space for the new measurements.  When this function completes, it should be valid to setValue()s within the new scan space. Return false if the request is not possible.
	virtual bool beginInsertRowsImplementation(int axisId, int numRows = 1, int atRowIndex = -1) = 0;

	/// Implementing subclasses may provide a endInsertRowsImplementation(), which will be called at the beginning of endInsertRows(). You don't need to take care of emitting sizeChanged() and dataChanged().... that's done for you in endInsertRows().  The base class implementation of this does nothing.
	virtual void endInsertRowsImplementation(int axisId, int numRows, int atRowIndex) {
		Q_UNUSED(axisId);
		Q_UNUSED(numRows);
		Q_UNUSED(atRowIndex);
	}

	/// Implementing subclasses must provide a clearImplementation(), which removes all data values and sets the size of each axis to 1... except for the first axis (axisId == 0), which should have a size of 0.  It should leave the set of configured measurements as-is.
	virtual void clearScanDataPointsImplementation() = 0;

	/// Implementing subclasses must provide a clearMeasurementsImplementation(), which clears the set of configured measurements.  They can assume that the set of scan data values is already cleared.
	virtual void clearMeasurementsImplementation() = 0;

	/// Implementing subclasses must call this whenever a measurement value changes. (For example, in their setValue implementation).  \c scanIndexStart and \c scanIndexEnd describe the scan range affected.  Use an invalid \c scanIndexStart to indicate the whole scan space is affected.  For performance, subclasses can opt to avoid calling this on every setValue(), and combine multiple emits into one (as long as the last one covers the complete affected range.)  A separate emitDataChanged() should be sent for each different measurement.  For multi-dimensional measurements, it's suggested to delay emits until all the values for a complete measurement have been received (if you can figure out how...).
	void emitDataChanged(const AMnDIndex& scanIndexStart, const AMnDIndex& scanIndexEnd, int measurementId) {
		/// Don't emit while inserting rows... Instead, dataChanged will be emitted for the whole region when done inserting rows.
		if(isInsertingRows_)
			return;	/// \todo Check if the scanIndexStart and scanIndexEnd represent a range that extends outside of the inserting region. If they do, emit anyway.

		signalSource_->emitDataChanged(scanIndexStart, scanIndexEnd, measurementId);
	}

	/// The base class calls this when the size of an axis changes (in endInsertRows()).  An \c axisId of -1 indicates that all the axes have changed size. Subclasses do not need to worry about this, as long as they call the base class's endInsertRows() when re-implementing endInsertRows().
	void emitSizeChanged(int axisId) {
		signalSource_->emitSizeChanged(axisId);
	}


private:
	bool isInsertingRows_;
	int insertingAxisId_, insertingAtRowIndex_, insertingNumRows_;

	bool multiAxisInsertInProgress_;
	AMnDIndex scanSpaceStartIndex_, scanSpaceEndIndex_;

	AMDataStoreSignalSource* signalSource_;
};

#endif // AMDATASTORE_H
