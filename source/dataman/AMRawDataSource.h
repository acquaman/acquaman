#ifndef AMRAWDATASOURCE_H
#define AMRAWDATASOURCE_H

#include "dataman/AMDbObject.h"
#include "dataman/AMDataSource.h"
#include "dataman/AMDataStore.h"

/// This class exposes one channel of an AMScan's raw data through the AMDataSource interface. It uses the scan's AMDataStore to access the data.  It also implements AMDbObject, so that it can be easily stored to the database.  The dimensions of its output include (first) the scan dimensions, then the detector dimensions.
class AMRawDataSource : public AMDbObject, public AMDataSource
{
	Q_OBJECT

	Q_PROPERTY(int measurementId READ measurementId WRITE dbLoadMeasurementId)
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(int scanRank READ scanRank WRITE dbLoadScanRank)
	Q_PROPERTY(int measurementRank READ measurementRank WRITE dbLoadMeasurementRank)
	Q_PROPERTY(int rank READ rank WRITE dbLoadRank)

	Q_CLASSINFO("measurementId", "hidden=true")
public:
	/// Construct a raw data source which exposes measurement number \c measurementId of the specified \c dataStore. Both \c dataStore and \c measurementId must be valid.
	AMRawDataSource(const AMDataStore* dataStore, int measurementId, QObject* parent = 0);
	/// This constructor re-loads a previously-stored source from the database.
	Q_INVOKABLE AMRawDataSource(AMDatabase* db, int id);

	/// Both AMDbObject() and AMDataSource() have a name(). Here we un-ambiguate those two.
	QString name() const { return AMDataSource::name(); }

	/// Provided to reconnect this source to a valid \c dataStore, after using the second constructor. The current measurementId() must be a valid id within the new \c dataStore, and the scan and measurement dimensions of the new datastore must match our old ones. (This is a requirement of AMDataSources never changing rank.). Returns false if the new \c dataStore cannot work, and this source is going to remain Invalid.
	bool setDataStore(const AMDataStore* dataStore);


	/// Returns an OR-combination of StateFlags describing the current state of the data. Implementing classes should return InvalidFlag when they don't have valid data, and/or ProcessingFlag if their data might be changing. No flags indicate the data is valid and generally static.
	virtual int state() const { return stateFlags_; }


	// Axis Information
	//////////////////////
	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const { return axes_; }

	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return axes_.count(); }
	/// Returns the size of (ie: count along) each dimension
	virtual AMnDIndex size() const {
		AMnDIndex s;
		for(int i=0; i<axes_.count(); i++)
			s.append(axes_.at(i).size);
		return s;
	}
	/// Returns the size along a single axis \c axisNumber. This should be fast. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual int size(int axisNumber) const { return axes_.at(axisNumber).size; }
	/// Returns a bunch of information about a particular axis. \c axisNumber is assumed to be between 0 and rank()-1.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const { return axes_.at(axisNumber); }
	/// Returns the number of an axis, by name. (By number, we mean the index of the axis. We called it number to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly.  Returns -1 if not found.
	virtual int numberOfAxis(const QString& axisName) {
		for(int i=0; i<axes_.count(); i++)
			if(axes_.at(i).name == axisName)
				return i;
		return -1;
	}


	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const {
		if(!isValid())
			return AMNumber(AMNumber::InvalidError);
		if(indexes.count() != rank())
			return AMNumber(AMNumber::DimensionError);

		/// \todo optimize for common dimensional cases, to avoid the for-loop overhead
		AMnDIndex scanIndex;
		for(int i=0; i<scanAxesCount_; i++)
			scanIndex.append(indexes[i]);
		AMnDIndex measurementIndex;
		for(int i=scanAxesCount_; i<indexes.count(); i++)
			measurementIndex.append(indexes[i]);

		return dataStore_->value(scanIndex, measurementId_, measurementIndex);
	}

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const {
		if(!isValid())
			return AMNumber(AMNumber::InvalidError);
		if(axisNumber < scanAxesCount_)
			return dataStore_->axisValue(axisNumber, index);	// value along a scan axis
		else if (axisNumber < rank() )	// value along a measurement axis. Unsupported so far... All we have is the direct value
			return index;	/// \todo Implement scaled and non-uniform measurement axes
		else
			return AMNumber(AMNumber::DimensionError);	// no such axis.

	}


	// Access for database stored values
	///////////////////
	/// The id of the detector we're exposing out of the raw data
	int measurementId() const { return measurementId_; }
	/// The number of dimensions or axes that were scanned over
	int scanRank() const { return scanAxesCount_; }
	/// The number of dimensions or axes that the detector has, at each scan point
	int measurementRank() const { return measurementAxesCount_; }


	/// Called when reloading from the database
	void dbLoadMeasurementId(int newId) { measurementId_ = newId; setInvalid(); }
	/// Called when reloading from the database
	void dbLoadScanRank(int newScanRank) { scanAxesCount_ = newScanRank; setInvalid(); }
	/// Called when reloading from the database
	void dbLoadMeasurementRank(int newMeasurementRank) { measurementAxesCount_ = newMeasurementRank; setInvalid(); }
	/// Called when reloading from the database. Must be followed with a call to setDataStore() to make this source valid again.
	void dbLoadRank(int newRank) {
		setInvalid();
		dataStore_ = 0;

		axes_.clear();
		for(int i=0; i<newRank; i++)
			axes_ << AMAxisInfo("invalid", 0);	// create the proper number of axes, even though they're just placeholders. This keeps the dimension information correct until setDataStore() fills this properly.

		// minimum of one axis, always.
		if(axes_.count() == 0)
			axes_ << AMAxisInfo("invalid", 0);
	}



protected:

	// Forwarding signals from the data store:
	//////////////////////
	/// Called when the data changes within the region described. We only care when \c measurementId matches our measurementId_
	void onDataChanged(const AMnDIndex& scanIndexStart, const AMnDIndex scanIndexEnd, int measurementId);
	/// Called when the size of a scan axis changes.  \c axisId is the id of the changing axis, or -1 if they all did.
	void onScanAxisSizeChanged(int axisId);

	/// Call to add the InvalidFlag to the state, and emit stateChanged() if required.
	void setInvalid() {
		int newState = stateFlags_ | AMDataSource::InvalidFlag;
		if(stateFlags_ != newState)
			emitStateChanged(stateFlags_ = newState);
	}

	/// Call to remove the InvalidFlag from the state, and emit stateChanged() if required.
	void setValid() {
		int newState = stateFlags_ & ~AMDataSource::InvalidFlag;
		if(stateFlags_ != newState)
			emitStateChanged(stateFlags_ = newState);
	}

	/// Axis information (including sizes) for each axis.  The first axes are the scan axes, followed by the detector/measurement axes.  Since the rank() of an AMDataSource cannot change over its lifetime, the number of axes in this list must always be correct and non-zero.  It is initialized when constructing based on a valid AMDataStore. The database-restoring constructor creates the memorized number of axes (all sized at 0), and waits for setDataStoreUsed() to re-fill them with valid data.
	QList<AMAxisInfo> axes_;
	/// number of scan axes
	int scanAxesCount_;
	/// number of measurement axes
	int measurementAxesCount_;

	/// The data store exposed by this source
	const AMDataStore* dataStore_;
	/// The measurement/detector/channel exposed by this source.  It must be a valid measurementId for AMDataStore::measurementAt().
	int measurementId_;
	/// For quickly knowing the extent of our data, this is the AMnDIndex of the starting "corner" of the measurement dimensionality.  For example, on a 2D detector, it might be AMnDIndex(0, 0).  On a 1D detector, it might be AMnDIndex(0).
	AMnDIndex measurementIndexStart_;
	/// For quickly knowing the extent of our data, this is the AMnDIndex of the ending "corner" of the measurement dimensionality.  For example, on 2D detector with \c width and \c height, it might be AMnDIndex(\c width, \c height).  \note There is a system-wide assumption that detectors/measurements don't change their size.
	AMnDIndex measurementIndexEnd_;

	/// State of the data (OR-combination of AMDataSource::StateFlags)
	int stateFlags_;
};

#endif // AMRAWDATASOURCE_H
