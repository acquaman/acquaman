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


#ifndef AMCDFDATASTORE_H
#define AMCDFDATASTORE_H

#include "dataman/datastore/AMDataStore.h"
#include <QString>
#include <QList>

#include "dataman/AMAxisInfo.h"
#include "dataman/AMMeasurementInfo.h"

/// How many bytes of memory to allocate when copying for an insertRows operation
#define AM_CDFDATASTORE_MOVERECORDS_MEM_BYTES 10000000

/// Delimeter used for joining strings together when we need to store in a single CDF_CHAR attribute
#define AM_CDFDATSTORE_STRING_DELIMITER "|@^@|"

/// This class implements multi-dimensional storage of scan data, according to the AMDataStore interface.  It is optimized for scan data sets that are too large to store completely in memory.  Data is stored on disk in a CDF formatted file, but accessing the values should be reasonably fast thanks to reading and caching of chunks at a time.  Like AMInMemoryDataStore, using the values() method to access data is much faster than reading by individual value().
/*!  <b>CDF Layout</b>

  We use the following CDF layout to represent the AMDataStore scan space / measurement space concept:

  Measurements:
  =============

  One CDF "zVariable" is created for each measurement.  The name of the CDF variable is the same as the name of the AMDataStore measurement.  The CDF provides native support for multi-dimensional variables, so a single multi-dimensional measurement reading can be stored in one record of a CDF variable.  The CDF library remembers the rank and size of the measurement.

  One CDF "record" is used to represent a single point in scan space.  For a scalar scan space, there exists only a single record in the file.  For a multi-dimensional scan space, the scan points are stored by records in row major order (the first scan index varies the slowest; the last scan index varies the fastest).  For example, for a 2D scan with 3 x 3 points, there will be 9 records: (0,0), (0,1), (0,2), (1,0), (1,1)... stored for each variable.

  One other set of CDF zVariables is created: for each scan axis that has non-uniform independent axisValue()s, there is a scalar measurement with the special name "AxisValues::[axisName]".  Each record stores a single axis value.

  Attributes:
  ===========

  The above measurements are sufficient to hold all the raw data in memory, but depend on the AMAxisInfo and AMMeasurementInfo in-memory structures to describe the structure and meaning of that raw data. Therefore, AMCDFDataStore also creates additional CDF "attributes" to make the CDF file self-documenting; this means that a pre-existing CDF file with those attributes can be used to completely instantiate an AMCDFDataStore.  These attributes are only used by instantiateFromExistingFile().

\code
  Attribute Name						Data Type	Num Entries			Num Elements		Notes
  ==========================================================================================
  <i>Global Attributes</i>

  AMCDF::Version						CDF_CHAR		1					<string length>	Version string of the CDF layout we're using: "AMCDFDataStore_1" for now.

  Measurement::Name					CDF_CHAR		One per measurement	<string length>	Can be used to introspect available measurements

  Axis::Name							CDF_CHAR		One per scan axis	<string length>	Can be used to introspect number of, names of scan axes
  Axis::Size							CDF_INT4		One per scan axis	<string length>	Length of each scan axis (AMAxisInfo::size)
  Axis::Description					CDF_CHAR		One per scan axis	<string length>	Scan axis AMAxisInfo::description
  Axis::Units						CDF_CHAR		One per scan axis	<string length>	Scan axis AMAxisInfo::units
  Axis::IsUniform					CDF_INT4		One per scan axis	1				Scan axis AMAxisInfo::isUniform (1 for true,
  AxisValues::Start					CDF_DOUBLE	One per scan axis	1				Scan axis AMAxisInfo::start
  AxisValues::Increment				CDF_DOUBLE	One per scan axis	1				Scan axis AMAxisInfo::increment

  <i>Variable Attributes [only apply to measurement variables]</i>

  Measurement::Description			CDF_CHAR		n/a	(index: varNum)	<string length>	AMMeasurementInfo::description
  Measurement::Units					CDF_CHAR		n/a					<string length>	AMMeasurementInfo::units
  // get rank and size from introspecting the measurement's zVariable
  Measurement::Axes::Names			CDF_CHAR		n/a					<string length>	AMAxisInfo::name for all measurement axes, delimited with AM_CDFDATSTORE_STRING_DELIMITER
  Measurement::Axes::Descriptions		CDF_CHAR		n/a					<string length>	AMAxisInfo::description for all meas. axes, delimited with AM_CDFDATSTORE_STRING_DELIMITER
  Measurement::Axes::Units			CDF_CHAR		n/a					<string length>	AMAxisInfo::units for all meas. axes, delimited with AM_CDFDATSTORE_STRING_DELIMITER
  Measurement::AxisValues::Start		CDF_DOUBLE	n/a					<meas. rank>		AMAxisInfo::start for all meas. axes.
  Measurement::AxisValues::Increment	CDF_DOUBLE	n/a					<meas. rank>		AMAxisInfo::increment for all meas. axes.

\endcode

  */
class AMCDFDataStore : public AMDataStore
{
	Q_OBJECT
public:
	/// Construct an empty data store.  The data store creates a temporary CDF file in the system-appropriate temp directory (ex: /tmp), and deletes the CDF file when the data store is deleted.
	AMCDFDataStore(QObject* parent = 0);
	/// Construct an empty data store, by creating a new CDF file at \c newFilePath.  If \c isTemporary is true, the CDF file will be deleted when the data store instance is deleted.  IMPORTANT: \c newFilePath must end in '.cdf', because otherwise the CDF API will change the file name by adding this on its own.
	/*! \note If a file already exists at \c newFilePath, this constructor will fail and the data store will be in an invalid state. You can check isValid() to make sure the CDF was created successfully. */
	AMCDFDataStore(const QString& newFilePath, bool isTemporary, QObject* parent = 0);

	/// Construct a new data store using the existing CDF file at \c existingFilePath. If \c createTemporaryCopy is true, the original file will be copied into the system-appropriate temp directory (ex: /tmp) first, and the copy will be opened instead of the original. If \c setReadOnly is true, the CDF will be put into read-only mode after opening \see setReadOnlyMode().
	/*! \note If the CDF file could not be found or opened correctly, this constructor will fail and the data store will be in an invalid state. You can check isValid() to make sure the CDF was opened successfully. */
	AMCDFDataStore(const QString& existingFilePath, bool createTemporaryCopy, bool setReadOnly, QObject* parent = 0);

	/// Destructor. If the data store was using a temporary file (cdfFileIsTemporary()), it will be deleted.
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


	// Accessing Values:
	////////////////////////////////
	/// Retrieve a value from a measurement, at a specific scan point.
	virtual AMNumber value(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex) const;

	/// Performance optimization of value(): this allows a block of multi-dimensional data to be retrieved in a single setValue call. The data is returned in a flat array, ordered in row-major form with the first scan index varying the slowest, and the measurement index's last axis varying the fastest.   /c scanIndexStart and \c scanIndexEnd specify the (inclusive) range in scan space. You can use the same start and end values to access the measurement values for a single scan point.  Which measurement to access is specified with \c measurementId, and \c measurementIndexStart and \c measurementIndexEnd specify the (inclusive) range in measurement space.  Returns false if any of the indexes are the wrong dimension or out of range.  It is the responsibility of the caller to make sure that \c outputValues is pre-allocated with enough room for all the data. Use valuesSize() to calculate this conveniently.
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
	/// Performance optimization of axisValue().  This will provide all the axis values desired in a single call.  Requires the id of the axis and the range of axis values you request.  Lastly, you need to provide a valid pointer to accept the new data tha must have the necessary space to hold the requested data.
    virtual bool axisValues(int axisId, long axisStartIndex, long axisEndIndex, double *outputValues) const;

	/// Set the independent variable along an axis \c axisId, at a specific scan point \c axisIndex. This is necessary after adding a "row" with beginInsertRows(), unless the axis scale is uniform. (See AMAxisInfo::isUniform).
	virtual bool setAxisValue(int axisId, long axisIndex, AMNumber newValue);


	// New Public Functions:
	////////////////////////////////////////

	/// Call this to load everything (scan axes, measurements, and data points) from an existing CDF file. The file at \c filePath must be in the AMCDFDataStore layout.  If \c createTemporaryCopy is true, a copy of the file will be created in the system's temporary folder, opened instead of the original file, and deleted along with the data store. If \c setReadOnly is true, all modification (non-const) functions will be disabled.
	/*! This function either succeeds completely and returns true, or fails and leaves the CDF in its original state. */
	bool initializeFromExistingCDF(const QString& filePath, bool createTemporaryCopy, bool setReadOnly);

	/// Set Read-Only mode.  In read-only mode, all modification (non-const) functions (beginInsertRows(), setValue(), addScanAxis(), addMeasurement(), etc.) will be disabled and return false. This ensures that the underlying CDF file will not be changed.
	/*! \note Also engages the read-only mode at the CDF library level; Read-only mode may improve performance because CDF "metadata" is copied fully into memory instead of being read from disk. */
	void setReadOnlyMode(bool readOnlyOn);

	/// In Read-Only mode, all modification functions are disabled to ensure that the underlying CDF file will not be changed.  \see setReadOnlyMode().  Returns true if readOnly mode is on.
	bool readOnlyMode() const { return readOnly_; }

	/// Returns the path of the CDF file currently in use.
	QString cdfFilePath() const { return cdfFilePath_; }

	/// Returns true if the CDF file is a temporary file that will be deleted when the data store instance is deleted.
	bool cdfFileIsTemporary() const { return fileIsTemporary_; }

	/// Returns true if the underlying CDF file has been created/opened successfully.
	bool isValid() const { return cdfId_ != 0; }

	/// Changes to the CDF can be cached, although they will be saved automatically to disk when the CDF is closed by the destructor. In between, the integrity of the CDF file is not guaranteed to be valid. Call this function to flush the cache and make sure the CDF is fully saved to disk.
	bool flushToDisk();


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

	/// A flag to indicate that all functions that could modify the CDF should be disabled.
	bool readOnly_;

	/// Maintains the set of measurements that we have at each scan point
	QList<AMMeasurementInfo> measurements_;
	/// This list is the same size as measurements_, and holds the CDF var num for each
	QVector<long> measurementVarNums_;
	/// Maintains information about each scan axis
	QList<AMAxisInfo> axes_;
	/// This list is the same size as axes_, and holdes the CDF var num for the axis values along that axis (or -1 if the axis is uniform and we are not storing it.)
	QVector<long> axisValueVarNums_;
	/// This stores the size of each axis in axes_.  (The information is duplicated, but this version is needed for performance in some situations. They should always be updated together.)
	AMnDIndex scanSize_;


	// Helper Functions:
	//////////////////////////////

	/// Generates a safe temporary file name in the QDir::tempPath() directory. Returns the full path.
	QString tempFileName(const QString& basename) const;

	/// Creates a CDF variable for a measurement, and instantiates \c numInitialRecords records.
	bool createVarForMeasurement(const AMMeasurementInfo& mi, long numInitialRecords);

	/// Implements values() for scan ranks larger than 4.
	bool valuesImplementationRecursive(const AMnDIndex &siStart, const AMnDIndex &siEnd, long varNum, const long* miStart, const long* miSize, const long* miInterval, double **outputValues, long flatReadSize, int currentDimension, long scanSpaceOffset) const;

	/// Creates the attributes in a new CDF for our self-descriptive CDF layout. Returns false and gives up if any fail.
	bool createAttributes();

	/// Update the attributes for a given \c measurementId
	bool updateAttributesForMeasurement(int measurementId);

	/// Update the attributes for a given \c scanAxisId
	bool updateAttributesForScanAxis(int axisId);

	// CDF helper functions (Could be moved to a C++ CDF API wrapper)
	///////////////

	/// Moves records in a CDF for a single variable. The source records will overwrite the records in the destination block.  It is OK for the source and destination blocks to overlap. In this case, the records will be copied one-by-one in the correct order to preserve integrity.
	static bool cdfCopyZRecords(void* cdfId, long varNum, long sourceRecordStart, long recordCount, long destinationRecordStart);

	/// Returns the dimensions of a zVariable in a CDF, by introspecting the CDF. If there is a failure, returns AMnDIndex(-1).  If the variable is 0-dimensional (scalar), returns AMnDIndex().
	static AMnDIndex cdfGetVarDimensions(void* cdfId, long varNum);

	/// Returns the value of a string (CDF_CHAR) attribute in a CDF. Returns a Null string (QString()) on error [no attribute with that name, attribute date definition is not a string, etc.]  If the attribute is empty, may return a valid empty string (QString("")).
	/*! \c entryNumber specifies which entry of that attribute; for variable-scope attributes it must be a valid variable number. For global-scope attributes it can be anything.*/
	static QString cdfGetStringAttribute(void* cdfId, const QString& attributeName, long entryNumber);

	/// Gets the value of an int (CDF_INT4) attribute in a CDF. Returns false on error (missing attribute or entry, entry does not match type or size \c numElements, etc.)
	/*! \c entryNumber specifies which entry of that attribute; for variable-scope attributes it must be a valid variable number. For global-scope attributes it can be anything.*/
	static bool cdfGetIntAttribute(void* cdfId, const QString& attributeName, long entryNumber, qint32* out, long numElements = 1L);

	/// Gets the value of a double (CDF_DOUBLE) attribute in a CDF. Returns false on error (missing attribute or entry, entry does not match type or size \c numElements, etc.)
	static bool cdfGetDoubleAttribute(void* cdfId, const QString& attributeName, long entryNumber, double* out, long numElements = 1L);

	/// Put the value of a string attribute to an entry in the CDF. The entry data type is set to CDF_CHAR and resized according to the size of the string. Returns false if there is no such attribute.
	static bool cdfPutStringAttribute(void* cdfId, const QString& attributeName, long entryNumber, const QString& value);

	/// Put the value of an int (or int array) attribute to an entry in the CDF.  The entry data type is set to CDF_INT4 and resized according to \c numElements.  Returns false if there is no such attribute.
	static bool cdfPutIntAttribute(void* cdfId, const QString& attributeName, long entryNumber, const qint32* value, long numElements = 1L);

	/// Put the value of a double (or double array) attribute to an entry in the CDF.  The entry data type is set to CDF_DOUBLE and resized according to \c numElements.  Returns false if there is no such attribute.
	static bool cdfPutDoubleAttribute(void* cdfId, const QString& attributeName, long entryNumber, const double* value, long numElements = 1L);


	/// Delete an attribute entry in the CDF. Does not delete the attribute definition. Just the entry at \c entryNumber.
	static bool cdfDeleteAttributeEntry(void* cdfId, const QString& attributeName, long entryNumber);


};

#endif // AMCDFDATASTORE_H
