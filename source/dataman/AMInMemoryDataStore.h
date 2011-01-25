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


#ifndef AMINMEMORYDATASTORE_H
#define AMINMEMORYDATASTORE_H

#include <QVector>
#include "dataman/AMNumber.h"
#include "dataman/AMDataStore.h"
#include "util/AMOrderedSet.h"

/// With this defined, value() requests will check the bounds of the data store to make sure that they are within the range of the data that exists, and return AMNumber::OutOfRangeError if not.  You can remove this line for performance, in which case there is no protection against accessing out-of-range values.
#define AMINMEMORYDATASTORE_BOUNDS_CHECKING_ENABLED

/// (Internal class for AMInMemoryDataStore) One instance stores a single measurement. (Flat array; nD measurement data is stored by striding.)
typedef QVector<AMNumber> AMIMDSMeasurement;

/// (Internal class for AMInMemoryDataStore) One instance stores a scan point: (ie: a set of measurements at that scan point)
typedef QVector<AMIMDSMeasurement> AMIMDSScanPoint;
/*
class AMIMDSScanPoint {
public:
	/// Constructor creates room for \c numMeasurements measurements.  (Null measurements for now)
	AMIMDSScanPoint(int numMeasurements) : measurements_(numMeasurements) {}
	/// Destructor deletes all measurements at this scan point.
	~AMIMDSScanPoint() {
		for(int i=measurements_.count()-1; i>=0; i--)
			delete measurements_.at(i);
	}
	/// Add a measurement: appends a null measurement.
	void addMeasurement() {
		measurements_ << AMIMDSMeasurement();
	}
	///

protected:
	QVector<AMIMDSMeasurement*> measurements_;

};*/

class AMIMDSColumn;

/// (Internal class for AMInMemoryDataStore) Nodes in an AMIMDSColumn can either store a pointer to an AMIMDSScanPoint (if this column is of the final dimension), or a pointer to another AMIMDSColumn (if this column is not the final dimension).
/*! Note: no internal memory management is provided for scan points or columns. (If the the scan points or columns were deleted when this instance is deleted, this would cause problems when copying and assigning this class.  Intended behaviour is that copying an AMIMDSColumnNode creates a shallow copy, which points to the same scanPoint or nextColumn as the source. )
  */
typedef union {
	AMIMDSColumn* nextColumn;
	AMIMDSScanPoint* scanPoint;
} AMIMDSColumnNode;

/*
class AMIMDSColumnNode {
public:
	/// create a null node. Don't ever put these into the data structure like this.
	AMIMDSColumnNode() { nextColumn_ = 0; scanPoint_ = 0; }
	/// create a non-final node that points to the next dimension:
	AMIMDSColumnNode(AMIMDSColumn* nextColumn) { nextColumn_ = nextColumn; scanPoint_ = 0; }
	/// create a final node that points to a scan point:
	AMIMDSColumnNode(AMIMDSScanPoint* scanPoint) { scanPoint_ = scanPoint; nextColumn_ = 0; }

	bool isFinal() const { return (nextColumn_ == 0); }

	AMIMDSColumn* nextColumn() const { return nextColumn_; }
	AMIMDSScanPoint* scanPoint() const { return scanPoint_; }

	/// Sets this node to point to a scan point (isFinal() becomes true). Be careful not to leak memory if scanPoint() or nextColumn() was previously set.
	void setScanPoint(AMIMDSScanPoint* sp) { scanPoint_ = sp; nextColumn_ = 0;}
	/// Sets this node to point to another column. (isFinal becomes false). Be careful not to leak memory if scanPoint() or nextColumn() was previously set.
	void setNextColumn(AMIMDSColumn* nc) { nextColumn_ = nc; scanPoint_ = 0; }

protected:
	AMIMDSColumn* nextColumn_;
	AMIMDSScanPoint* scanPoint_;
};*/

/// (Internal class for AMInMemoryDataStore) One instance stores a scan axis. Each node in the column can either point to an AMIMDSScanPoint (if this column is the final dimension), or point to another column (if this column is not the final dimension).
class AMIMDSColumn : public QVector<AMIMDSColumnNode> {
public:
	AMIMDSColumn() : QVector<AMIMDSColumnNode>() {}
};



/// This class implements multi-dimensional storage of scan data, according to the AMDataStore interface.  All data is stored in memory, and accessing any data values should be quite fast.
/*! <b>Memory Layout</b>
The data is not stored in one contiguous chunk, so datasets approaching the size of the full computer memory may be used, without requiring a single allocated chunk. (The tradeoff is that there is an overhead of additional memory used for each dimension, to store pointers to the next dimension.)

todo: explain recursive tree structure for scan axes

	The size of scan axes may change as scan points are added, therefore we use a recursive tree structure to implement the scan axes. This gives us the flexibility to insert "rows" anywhere in any dimension, without having to re-allocate and move large amounts of data in memory. The trade-off is that additionl memory is used to maintain pointers at each dimension.

	The measurement axes (in contrast to the scan axes) are not expected to change size as the data store is used... The size and dimensionality of the measurement is a property of the detector. Therefore we can get away with using a flat array to store the measurement values (regardless of dimensionality). The values within a measurement are laid out in memory so that the last dimension specified in the measurement axes (for ex: AMnDIndex(first, second, LAST) ) is stored consecutively. (ie: varies the fastest when iterating through consecutive memory locations).
	*/

class AMInMemoryDataStore : public AMDataStore
{
public:
	AMInMemoryDataStore();

	~AMInMemoryDataStore();



	/// Creates space to support an additional measurement at every scan point. \c measurementDetails describes the dimensionality and details of the new set of measurements.  (If a set of scan points exist already, the initial values for this measurement at those scan points should be Null.)
	/*! If you want to retrieve measurements by name, \c measurementDetails must contain a unique \c name.  This function should return false if a measurement with that name already exists. */
	virtual bool addMeasurement(const AMMeasurementInfo& measurementDetails) {

		// already a measurement with this name?
		if(measurements_.contains(measurementDetails.name))
			return false;

		// add this measurement to our registry
		/// \todo Anything to verify that we need to make sure is true in the details?
		measurements_.append(measurementDetails, measurementDetails.name);


		if(axes_.count() == 0){	// scalar scan space. append null measurement to scalar scan point.
			scalarScanPoint_->append(AMIMDSMeasurement(measurementDetails.spanSize()));
		}
		else if(!isEmpty()) {		// if we have existing scan points... need to add storage for this measurement to each one.
			appendNullMeasurementToAllPoints(measurementDetails, dataRoot_, axes_.count()-1);
		}

		return true;
	}

	/// Retrieve the id of an existing set of measurements, by name.  (Depending on the implementation, this may not be fast. Avoid calling it repeatedly.) Returns -1 if not found.
	virtual inline int idOfMeasurement(const QString& measurementName) const {
		return measurements_.indexOf(measurementName);
	}

	/// Retrieve information about a set of measurements, by id. \c id assumed to be >= 0 and < measurementCount().
	virtual inline AMMeasurementInfo measurementAt(int id) const {
		return measurements_.at(id);
	}

	/// Return the number of measurements stored for each scan point
	virtual inline int measurementCount() const {
		return measurements_.count();
	}




	/// Create space to support an additional scan axis.  \c axisDetails describes the characteristics of the axis, but the \c size of axisDetails will be ignored.  If this is the first axis to be added, the size will be set to 0; otherwise it will be set to 1.
	/*! If you want to retrieve axes by name, \c axisDetails must contain a unique \c name.  This function should return false if an axis with that name already exists.

	  \note No signalling is provided for alerting observers of new scan axes. It's also prohibited for AMDataSources that expose this data (for ex: AMRawDataSource) to change dimensionality (ie: add another axis). Therefore, it's recommended to only call this function when first setting up a dataStore, before any observers get involved.
*/
	virtual bool addScanAxis(const AMAxisInfo& axisDetails) {
		// axis already exists with this name... Not allowed.
		if(axes_.contains(axisDetails.name))
			return false;

		AMAxisInfo axisInfo = axisDetails;
		if(axes_.count() == 0) {	// if this is the first axis to be added...
			axisInfo.size = 0;
			dataRoot_ = new AMIMDSColumn();	// leave it empty; size is 0.
		}

		else {	// there are axes already.  Add a new axis with a single data value.
			axisInfo.size = 1;

			AMIMDSColumn* newRoot = new AMIMDSColumn();
			AMIMDSColumnNode newNode;
			newNode.nextColumn = dataRoot_;
			newRoot->append( newNode );	// now this column has a single entry, which points to the existing axes in the data structure.
			dataRoot_ = newRoot;	// point to the new top
		}

		axes_.append(axisInfo, axisInfo.name);

		// Add entry in axisValues_ for this new axis.  If this axis is uniform, we don't need actual storage for axis values. However, we need to insert one into axisValues_ anyway to maintain indexing.
		if(axisInfo.isUniform)
			axisValues_ << QVector<AMNumber>(0);
		else
			axisValues_ << QVector<AMNumber>(axisInfo.size);

		return true;
	}

	/// Retrieve the id of an existing axis, by name.
	virtual inline int idOfScanAxis(const QString& axisName) const {
		return axes_.indexOf(axisName);
	}
	/// Retrieve information about an axis, by id.  \c id assumed to be >= 0 and < scanAxesCount().
	virtual inline const AMAxisInfo scanAxisAt(int id) const {
		return axes_.at(id);
	}
	/// Return the number of scan axes
	virtual inline int scanAxesCount() const {
		return axes_.count();
	}
	/// Synonym for scanAxisCount()
	int inline scanRank() const { return axes_.count(); }
	/// Return the sizes of all the scan axes, in order.
	virtual inline AMnDIndex scanSize() const {
		AMnDIndex s;
		for(int mu=0; mu<axes_.count(); mu++)
			s.append(axes_.at(mu).size);
		return s;
	}
	/// Return the size along a specific axis, by \c id.  \c id assumed to be >= 0 and < scanAxesCount().
	virtual inline int scanSize(int axisId) const {
		return axes_.at(axisId).size;
	}



	/// Retrieve a value from a measurement, at a specific scan point.
	virtual inline AMNumber value(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex) const {

		// scan axis index doesn't provide enough / too many dimensions
		if(scanIndex.rank() != axes_.count())
			return AMNumber(AMNumber::DimensionError);


#ifdef AMINMEMORYDATASTORE_BOUNDS_CHECKING_ENABLED
		for(int mu=0; mu<axes_.count(); mu++)
			if((unsigned)scanIndex[mu] >= (unsigned)scanSize(mu))
				return AMNumber(AMNumber::OutOfBoundsError);
#endif

		// performance optimization: avoid recursive function calls for known dimensions
		AMIMDSScanPoint* scanPoint;
		switch(scanIndex.rank()) {
		case 0:	// If no scan axes, can still have a single measurement. (Scalar scan space).
			scanPoint = scalarScanPoint_;
			break;

		case 1:
			scanPoint = dataRoot_->at(scanIndex.i()).scanPoint;
			break;

		case 2:
			scanPoint = dataRoot_->at(scanIndex.j()).nextColumn->at(scanIndex.i()).scanPoint;
			break;

		case 3:
			scanPoint = dataRoot_->at(scanIndex.k()).nextColumn
								->at(scanIndex.j()).nextColumn
								->at(scanIndex.i()).scanPoint;
			break;

		case 4:
			scanPoint = dataRoot_->at(scanIndex.l()).nextColumn
								->at(scanIndex.k()).nextColumn
								->at(scanIndex.j()).nextColumn
								->at(scanIndex.i()).scanPoint;
			break;

		default:
			scanPoint = findScanPointRecursive(scanIndex, dataRoot_, axes_.count()-1);
			break;
		}


		if((unsigned)measurementId >= (unsigned)measurementCount())
			return AMNumber(AMNumber::InvalidError);	// invalid measurement specified;

		if(measurementIndex.rank() != measurements_.at(measurementId).rank())
			return AMNumber(AMNumber::DimensionError);

		int flatIndex = flatIndexForMeasurement(measurementId, measurementIndex);
#ifdef AMINMEMORYDATASTORE_BOUNDS_CHECKING_ENABLED
		if((unsigned)flatIndex >= (unsigned)scanPoint->at(measurementId).size())
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

		return scanPoint->at(measurementId).at(flatIndex);
	}


	/// Set the value of a measurement, at a specific scan point
	virtual bool setValue(const AMnDIndex& scanIndex, int measurementId, const AMnDIndex& measurementIndex, const AMNumber& newValue) {
		// scan axis index doesn't provide enough / too many dimensions
		if(scanIndex.rank() != axes_.count()) {
			//qDebug() << "err 1";
			return false;
		}


	#ifdef AMINMEMORYDATASTORE_BOUNDS_CHECKING_ENABLED
		for(int mu=0; mu<axes_.count(); mu++)
			if((unsigned)scanIndex[mu] >= (unsigned)scanSize(mu)) {
				//qDebug() << "err 2";
				return false;
			}
	#endif

		// performance optimization: avoid recursive function calls for known dimensions
		AMIMDSScanPoint* scanPoint;
		switch(scanIndex.rank()) {
		case 0:	// If no scan axes, can still have a single measurement. (Scalar scan space).
			scanPoint = scalarScanPoint_;
			break;

		case 1:
			scanPoint = dataRoot_->at(scanIndex.i()).scanPoint;
			break;

		case 2:
			scanPoint = dataRoot_->at(scanIndex.j()).nextColumn->at(scanIndex.i()).scanPoint;
			break;

		case 3:
			scanPoint = dataRoot_->at(scanIndex.k()).nextColumn
								->at(scanIndex.j()).nextColumn
								->at(scanIndex.i()).scanPoint;
			break;

		case 4:
			scanPoint = dataRoot_->at(scanIndex.l()).nextColumn
								->at(scanIndex.k()).nextColumn
								->at(scanIndex.j()).nextColumn
								->at(scanIndex.i()).scanPoint;
			break;

		default:
			scanPoint = findScanPointRecursive(scanIndex, dataRoot_, axes_.count()-1);
			break;
		}


		if((unsigned)measurementId >= (unsigned)measurementCount()) {
			//qDebug() << "err 4";
			return false;	// invalid measurement specified;
		}

		if(measurementIndex.rank() != measurements_.at(measurementId).rank()) {
			//qDebug() << "err 5";
			return false;
		}

		int flatIndex = flatIndexForMeasurement(measurementId, measurementIndex);
	#ifdef AMINMEMORYDATASTORE_BOUNDS_CHECKING_ENABLED
		if((unsigned)flatIndex >= (unsigned)scanPoint->at(measurementId).size()) {
			//qDebug() << "err 6";
			return false;
		}
	#endif

		(*scanPoint)[measurementId][flatIndex] = newValue;
		return true;
	}


	/// Performance optimization for setValue(): this allows multi-dimensional measurements to be set in a single setValue call.  \c inputData is interpreted as being in a flat array, ordered where the measurement's first axis varies the slowest, and the measurement's last axis varies the fastest.  The size of the \c inputData must match the product of the sizes of all dimensions in the measurement.
	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const int* inputData, int numArrayElements) {
		// scan axis index doesn't provide enough / too many dimensions
		if(scanIndex.rank() != axes_.count())
			return false;


#ifdef AMINMEMORYDATASTORE_BOUNDS_CHECKING_ENABLED
		for(int mu=0; mu<axes_.count(); mu++)
			if((unsigned)scanIndex[mu] >= (unsigned)scanSize(mu))
				return false;
#endif

		// performance optimization: avoid recursive function calls for known dimensions
		AMIMDSScanPoint* scanPoint;
		switch(scanIndex.rank()) {
		case 0:	// If no scan axes, can still have a single measurement. (Scalar scan space).
			scanPoint = scalarScanPoint_;
			break;

		case 1:
			scanPoint = dataRoot_->at(scanIndex.i()).scanPoint;
			break;

		case 2:
			scanPoint = dataRoot_->at(scanIndex.j()).nextColumn->at(scanIndex.i()).scanPoint;
			break;

		case 3:
			scanPoint = dataRoot_->at(scanIndex.k()).nextColumn
								->at(scanIndex.j()).nextColumn
								->at(scanIndex.i()).scanPoint;
			break;

		case 4:
			scanPoint = dataRoot_->at(scanIndex.l()).nextColumn
								->at(scanIndex.k()).nextColumn
								->at(scanIndex.j()).nextColumn
								->at(scanIndex.i()).scanPoint;
			break;

		default:
			scanPoint = findScanPointRecursive(scanIndex, dataRoot_, axes_.count()-1);
			break;
		}


		if((unsigned)measurementId >= (unsigned)measurementCount())
			return false;	// invalid measurement specified;

		AMIMDSMeasurement& measurement = (*scanPoint)[measurementId];
		if(numArrayElements != measurement.size())
			return false;

		for(int i=0; i<measurement.size(); i++)
			measurement[i] = inputData[i];

		return true;
	}

	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const double* inputData, int numArrayElements) {
		// scan axis index doesn't provide enough / too many dimensions
		if(scanIndex.rank() != axes_.count())
			return false;


#ifdef AMINMEMORYDATASTORE_BOUNDS_CHECKING_ENABLED
		for(int mu=0; mu<axes_.count(); mu++)
			if((unsigned)scanIndex[mu] >= (unsigned)scanSize(mu))
				return false;
#endif

		// performance optimization: avoid recursive function calls for known dimensions
		AMIMDSScanPoint* scanPoint;
		switch(scanIndex.rank()) {
		case 0:	// If no scan axes, can still have a single measurement. (Scalar scan space).
			scanPoint = scalarScanPoint_;
			break;

		case 1:
			scanPoint = dataRoot_->at(scanIndex.i()).scanPoint;
			break;

		case 2:
			scanPoint = dataRoot_->at(scanIndex.j()).nextColumn->at(scanIndex.i()).scanPoint;
			break;

		case 3:
			scanPoint = dataRoot_->at(scanIndex.k()).nextColumn
								->at(scanIndex.j()).nextColumn
								->at(scanIndex.i()).scanPoint;
			break;

		case 4:
			scanPoint = dataRoot_->at(scanIndex.l()).nextColumn
								->at(scanIndex.k()).nextColumn
								->at(scanIndex.j()).nextColumn
								->at(scanIndex.i()).scanPoint;
			break;

		default:
			scanPoint = findScanPointRecursive(scanIndex, dataRoot_, axes_.count()-1);
			break;
		}


		if((unsigned)measurementId >= (unsigned)measurementCount())
			return false;	// invalid measurement specified;

		AMIMDSMeasurement& measurement = (*scanPoint)[measurementId];
		if(numArrayElements != measurement.size())
			return false;

		for(int i=0; i<measurement.size(); i++)
			measurement[i] = inputData[i];

		return true;
	}




	/// Retrieve the independent variable along an axis \c axisId, at a specific scan point \c axisIndex.  If the axis scale is uniform (see AMAxisInfo::isUniform) this can be calculated from the axis' \c start and \c increment.
	virtual inline AMNumber axisValue(int axisId, int axisIndex) const {

		if((unsigned)axisId >= (unsigned)axes_.count())
			return AMNumber(AMNumber::InvalidError);	// invalid axis specified.

#ifdef AMINMEMORYDATASTORE_BOUNDS_CHECKING_ENABLED
		if((unsigned)axisIndex >= (unsigned)scanSize(axisId))
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

		const AMAxisInfo& ai = axes_.at(axisId);
		if(ai.isUniform)
			return (double)ai.start + axisIndex*(double)ai.increment;
		else
			return axisValues_.at(axisId).at(axisIndex);

	}

	/// Set the independent variable along an axis \c axisId, at a specific scan point \c axisIndex. This is necessary after adding a "row" with beginInsertRows(), unless the axis scale is uniform. (See AMAxisInfo::isUniform).
	virtual inline bool setAxisValue(int axisId, int axisIndex, AMNumber newValue) {

		if((unsigned)axisId >= (unsigned)axes_.count())
			return false;	// invalid axis specified.

#ifdef AMINMEMORYDATASTORE_BOUNDS_CHECKING_ENABLED
		if((unsigned)axisIndex >= (unsigned)scanSize(axisId))
			return false;
#endif

		if(axes_.at(axisId).isUniform)
			return false;

		axisValues_[axisId][axisIndex] = newValue;
		return true;
	}





protected:
	/// Maintains set of measurements which we have at each scan point
	AMOrderedSet<QString, AMMeasurementInfo> measurements_;
	/// Maintains information about each scan axis
	AMOrderedSet<QString, AMAxisInfo> axes_;

	/// Points to the column of the first dimension of our data
	AMIMDSColumn* dataRoot_;
	/// If there are no scan axes ( a scalar scan space), this is the single scan point:
	AMIMDSScanPoint* scalarScanPoint_;

	/// Storage for the independent variables (axis values). Indexed by axisId, and then by position along that axis.
	QVector<QVector<AMNumber> > axisValues_;


	/// This function descends the dimensions of the data structure recursively, looking for the final scan point.  \c atDimension is the dimension index within the \c scanIndex that we will check next.  For example, in a 4-dimensional structure searching for AMnDIndex(9,12,3,17), the first function will be called with \c atDimension = 3, where we will descend into the 18th element, and then call recursively with \c atDimension = 2...
	/*! No checking is done to see if the data structure pointers are valid; it's assumed we have enough dimensions to succeed. It's up to the caller to verify this.*/
	AMIMDSScanPoint* findScanPointRecursive(const AMnDIndex& scanIndex, const AMIMDSColumn* dataRoot, int atDimension) const {
		if(atDimension == 0)
			return dataRoot->at(scanIndex[0]).scanPoint;
		else
			return findScanPointRecursive(scanIndex, dataRoot->at(scanIndex[atDimension]).nextColumn, atDimension-1);
	}

	/// Returns the flat array index for a multi-dimensional measurement.  For example, for a 4-dimensional \c measurementIndex AMnDIndex(3,4,5,600) and a measurement that has dimensions (5,10,100,1000), the flat index is 3*10*100*1000 + 4*100*1000 + 5*1000 + 600.
	/*! Note: assumes measurementId is a valid one of our measurements, and that measurementIndex is the right size for it.*/
	int inline flatIndexForMeasurement(int measurementId, const AMnDIndex& measurementIndex) const {
		int rv;
		int rank = measurementIndex.rank();

		switch(rank) {
		case 0:
			rv =  0; break;
		case 1:
			rv = measurementIndex.i();
			break;
		case 2:
			rv = measurementIndex.i()*measurements_.at(measurementId).size(1)
				 + measurementIndex.j();
			break;
		case 3: {
			const AMMeasurementInfo& mi = measurements_.at(measurementId);
			rv = measurementIndex.i()*mi.size(1)*mi.size(2)
					+ measurementIndex.j()*mi.size(2)
					+ measurementIndex.k();
			break; }

		case 4: {
			const AMMeasurementInfo& mi = measurements_.at(measurementId);
			rv = measurementIndex.i()*mi.size(1)*mi.size(2)*mi.size(3)
					+ measurementIndex.j()*mi.size(2)*mi.size(3)
					+ measurementIndex.k()*mi.size(3);
					+ measurementIndex.l();
			break; }

		default: {
			const AMMeasurementInfo& mi = measurements_.at(measurementId);
			rv = 0;
			for(int mu=0; mu<rank; mu++) {
				int multiplier = 1;
				for(int nu=mu+1; nu<rank; nu++)
					multiplier *= mi.size(nu);
				rv += measurementIndex[mu]*multiplier;
			}
			break; }
		}

		return rv;
	}

	/// This function appends a null measurement (matching the size of \c measurementDetails) to all existing scan points. It loops through columns and recurses through dimensions, starting at \c columnRoot.  \c atDimension is the dimension index that it will open on this call; for example, on a 4d data structure, \c atDimension should equal 3 at the start.  When \c atDimension = 0, we're at the final dimension.
	void appendNullMeasurementToAllPoints(const AMMeasurementInfo& measurementDetails, AMIMDSColumn* columnRoot, int atDimension) {

		if(columnRoot == 0)
			return;

		int columnSize = columnRoot->size();
		if(columnSize == 0)
			return;

		if(atDimension == 0) {
			int spanSize = measurementDetails.spanSize();
			for(int i=0; i<columnSize; i++)
				columnRoot->at(i).scanPoint->append( AMIMDSMeasurement(spanSize) );
		}

		else {
			for(int i=0; i<columnSize; i++)
				appendNullMeasurementToAllPoints(measurementDetails, columnRoot->at(i).nextColumn, atDimension-1);
		}
	}


	/// Implementing subclasses must provide a beginInsertRowsImplementation() which creates space for the new measurements.  When this function completes, it should be valid to setValue()s within the new scan space. Return false if the request is not possible.  \c axisId is the dimension index that we should add the rows to.  \c numRows will be added, with the first new row inserted at \c atRowIndex.  (If \c atRowIndex is >= the the size of the axis, or < 0, the new rows will be appended to the end.)
	virtual bool beginInsertRowsImplementation(int axisId, int numRows = 1, int atRowIndex = -1) {

		if((unsigned)axisId >= (unsigned)axes_.count())
			return false;

		beginInsertRowsImplementationRecursive(axisId, numRows, atRowIndex, dataRoot_, axes_.count()-1);

		axes_[axisId].size += numRows;

		if(!axes_.at(axisId).isUniform)
			axisValues_[axisId].insert(atRowIndex, numRows, AMNumber());

		return true;

	}

	/// Recursive helper for beginInsertRowsImplementation, which descends through dimensions. For dimensions higher than the axis to which we're adding points, it loops through all values along the axis.  When reaching the target axis/target dimension, it inserts new nodes into the column at the proper location, creating either a set of null measurements or an additional axis column under each node.
	void beginInsertRowsImplementationRecursive(int targetAxisId, int numRowsToAdd, int addAtRowIndex, AMIMDSColumn* columnRoot, int atDimension) {

		// Case 1: We're above the dimension of the target axis in the data structure. Loop through and execute the insertRows operation for all points along this axis.
		if(targetAxisId < atDimension) {
			for(int i=0; i<scanSize(atDimension); i++)
				beginInsertRowsImplementationRecursive(targetAxisId,
												  numRowsToAdd,
												  addAtRowIndex,
												  columnRoot->at(i).nextColumn,
												  atDimension-1);
		}

		///////////////////////

		// Case 2: We're at the dimension of the target axis in the data structure
		else if(targetAxisId == atDimension) {

			if(addAtRowIndex < 0 || addAtRowIndex > scanSize(targetAxisId))
				addAtRowIndex = scanSize(targetAxisId);	// constrain the insert row index to be within the existing points. If outside, append to end.

			for(int i=0; i<numRowsToAdd; i++) {
				columnRoot->insert(addAtRowIndex+i, createNewColumnOrScanPointRecursive(atDimension-1) );
			}
		}
	}



	/// Recursive helper function to create either (a) a scan point and a set of measurements, or (b) a new axis column, under a column node.  Returns the new column node.
	/*! \c atDimension specifies the number of the axis/dimension we're currently working on.  If \c atDimension < 0, a new scan point is created (with room for all the measurements we have in measurements_).  If \c atDimension >= 0, a new column is created and filled with the number of nodes matching the size of that axis.  This same function is used recursively to create those nodes.*/
	AMIMDSColumnNode createNewColumnOrScanPointRecursive(int atDimension) {

		AMIMDSColumnNode rv;	// return value

		// Case 1: if at the final dimension, just create the scan point. Make sure the scan point contains null values for all the points, in each measurement that we have.
		if(atDimension < 0) {
			AMIMDSScanPoint* newScanPoint = new AMIMDSScanPoint();
			for(int i=0; i<measurementCount(); i++)	// append AMIMDSMeasurement for each measurement class in measurements_
				newScanPoint->append( AMIMDSMeasurement(measurements_.at(i).spanSize()) );

			rv.scanPoint = newScanPoint;
		}

		// Case 2: Not at the final dimension. Create a new column, and fill it with enough nodes to match the size of this axis.
		else {
			AMIMDSColumn* newColumn = new AMIMDSColumn();
			for(int i=0; i<scanSize(atDimension); i++)
				newColumn->append( createNewColumnOrScanPointRecursive(atDimension-1) );

			rv.nextColumn = newColumn;
		}

		return rv;
	}


	/// Implementing subclasses must provide a clearImplementation(), which removes all data values and sets the size of each axis to 1... except for the first axis (axisId == 0), which should have a size of 0.  It should leave the set of configured measurements as-is.
	virtual void clearScanDataPointsImplementation() {
		clearScanDataPointsImplementationRecursive(dataRoot_, axes_.count()-1, true);

		for(int mu = axes_.count()-1; mu >=1; mu--)
			axes_[mu].size = 1;

		if(axes_.count() >= 1)
			axes_[0].size = 0;
	}

	/// Recursive helper function to delete all scan points in the data structure. For higher dimensions, it leaves a column with 1 node pointing to the next dimension. For the final dimension, it deletes all scan points and leaves a column with no nodes.
	/*! \c columnRoot: pointer to the AMIMDSColumn we're about to operate on
		\c atDimension: the axis index we're about to operate on. For example, in a 4D data structure, we would start with \c atDimension = 3. The final level in the data structure is \c atDimension = 0.
		\c leaveOne: true if this recursive call was made below the first node in every dimension all the way down. Indicates that we should also keep the first node in our column (as long as atDimension != 0)
	  */
	void clearScanDataPointsImplementationRecursive(AMIMDSColumn* columnRoot, int atDimension, bool leaveOne) {

		// Base case: at the final dimension
		////////////////////////////
		if(atDimension == 0) {
			for(int i=columnRoot->size()-1; i>=0; i--)
				delete columnRoot->at(i).scanPoint;
			columnRoot->clear();
		}


		// Recursive case: at higher dimensions
		///////////////////////////////
		else {
			// everything above the first node: recursively clear then delete.
			for(int i=columnRoot->size()-1; i>=1; i--) {
				clearScanDataPointsImplementationRecursive(columnRoot->at(i).nextColumn, atDimension-1, false);	// now the column is empty; still need to delete it.
				delete columnRoot->at(i).nextColumn;
			}

			// For the first node: behaviour depends on whether we are also a first node
			if(leaveOne) {	// we're a first node ourself, so leave our first node as well
				AMIMDSColumnNode first = columnRoot->at(0);
				clearScanDataPointsImplementationRecursive(first.nextColumn, atDimension-1, true);
				columnRoot->clear();
				columnRoot->append(first);	// clear but don't clear the whole thing; put the first node back on.
			}
			else {	// we're not a first node, so delete and clear all of our nodes (including the first one)
				clearScanDataPointsImplementationRecursive(columnRoot->at(0).nextColumn, atDimension-1, false);
				delete columnRoot->at(0).nextColumn;
				columnRoot->clear();
			}
		}
	}

	/// Implementing subclasses must provide a clearMeasurementsImplementation(), which clears the set of configured measurements.  They can assume that the set of scan data values is already cleared.
	virtual void clearMeasurementsImplementation() {
		measurements_.clear();
	}
};

#endif // AMINMEMORYDATASTORE_H
