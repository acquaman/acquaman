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


#ifndef AMRAWDATASOURCE_H
#define AMRAWDATASOURCE_H

#include "dataman/database/AMDbObject.h"
#include "dataman/datasource/AMDataSource.h"
#include "dataman/datastore/AMDataStore.h"

/// This class exposes one channel of an AMScan's raw data through the AMDataSource interface. It uses the scan's AMDataStore to access the data.  It also implements AMDbObject, so that it can be easily stored to the database.  The dimensions of its output include (first) the scan dimensions, then the detector dimensions.
class AMRawDataSource : public AMDbObject, public AMDataSource
{
	Q_OBJECT

	Q_PROPERTY(int measurementId READ measurementId WRITE dbLoadMeasurementId)
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(int scanRank READ scanRank WRITE dbLoadScanRank)
	Q_PROPERTY(int measurementRank READ measurementRank WRITE dbLoadMeasurementRank)
	Q_PROPERTY(int rank READ rank WRITE dbLoadRank)
	Q_PROPERTY(bool visibleInPlots READ visibleInPlots WRITE setVisibleInPlots)
	Q_PROPERTY(bool hiddenFromUsers READ hiddenFromUsers WRITE setHiddenFromUsers)

	Q_CLASSINFO("measurementId", "hidden=true")
	Q_CLASSINFO("AMDbObject_Attributes", "description=Data Source")
	Q_CLASSINFO("visibleInPlots", "upgradeDefault=false")
	Q_CLASSINFO("hiddenFromUsers", "upgradeDefault=false")

public:
	/// Construct a raw data source which exposes measurement number \c measurementId of the specified \c dataStore. Both \c dataStore and \c measurementId must be valid.
	AMRawDataSource(const AMDataStore* dataStore, int measurementId, QObject* parent = 0);
	/// This constructor re-loads a previously-stored source from the database.
	Q_INVOKABLE AMRawDataSource(const QString &outputName = "InvalidInput", QObject *parent = 0);
	/// Destructor.
	virtual ~AMRawDataSource();

	/// Both AMDbObject() and AMDataSource() have a name(). Here we un-ambiguate those two.
	QString name() const { return AMDataSource::name(); }

	/// Sets the description because currently we call AMDataSource::setDescription which doesn't take care of setting setModified.  This adds that.
	void setDescription(const QString &description) { AMDataSource::setDescription(description); setModified(true); }

	/// Provided to reconnect this source to a valid \c dataStore, either after using the second constructor, or when replacing an AMScan's underlying data store. The current measurementId() must be a valid id within the new \c dataStore, and the scan and measurement dimensions of the new datastore must match our old ones. (This is a requirement of AMDataSources never changing rank.).
	/*! Returns false if the new \c dataStore cannot work.  (If this happens after using the second constructor, the source will remain in the Invalid state.) */
	bool setDataStore(const AMDataStore* dataStore);

	/// Returns true if a replacement \c dataStore would be valid for this data source.  The current measurementId() must be a valid id within the new \c dataStore, and the scan and measurement dimensions of the new datastore must match our old ones. (This is a requirement of AMDataSources never changing rank.).
	/*! Called by setDataStore() to check these conditions before accepting the new \c dataStore. */
	bool isDataStoreCompatible(const AMDataStore* dataStore) const;


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
		int rank = axes_.count();
		AMnDIndex s(rank, AMnDIndex::DoNotInit);
		for(int i=0; i<rank; i++)
			s[i] = axes_.at(i).size;
		return s;
	}
	/// Returns the size along a single axis \c axisId. This should be fast. \c axisId is assumed to be between 0 and rank()-1.
	virtual int size(int axisId) const { return axes_.at(axisId).size; }
	/// Returns a bunch of information about a particular axis. \c axisId is assumed to be between 0 and rank()-1.
	virtual AMAxisInfo axisInfoAt(int axisId) const { return axes_.at(axisId); }
	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it number to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly.  Returns -1 if not found.
	virtual int idOfAxis(const QString& axisName) const {
		for(int i=0; i<axes_.count(); i++)
			if(axes_.at(i).name == axisName)
				return i;
		return -1;
	}


	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient/wrong dimensionality, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const {

		if(!isValid())
			return AMNumber(AMNumber::InvalidError);
		if(indexes.rank() != rank())
			return AMNumber(AMNumber::DimensionError);


		/// optimize for common dimensional cases, to avoid the for-loop overhead
		switch(scanAxesCount_) {
		case 0:	// scalar scan space. indexes are all in measurement space.
			return dataStore_->value(AMnDIndex(), measurementId_, indexes);
			break;
		case 1:
			switch(measurementAxesCount_) {
			case 0:
				return dataStore_->value(indexes.i(), measurementId_, AMnDIndex());	// 1d data: one scan axis, scalar measurements
			case 1:
				return dataStore_->value(indexes.i(), measurementId_, indexes.j()); // 2d data: one scan axis, one measurement axis (ie: XAS scan with 1D detector, like SDD)
			case 2:
				return dataStore_->value(indexes.i(), measurementId_, AMnDIndex(indexes.j(), indexes.k()));	// 3d data: one scan axis, two measurement axes (ie: XAS scan with 2D detector, like XES)
			}
		case 2:
			switch(measurementAxesCount_) {
			case 0:
				return dataStore_->value(AMnDIndex(indexes.i(), indexes.j()), measurementId_, AMnDIndex());
			case 1:
				return dataStore_->value(AMnDIndex(indexes.i(), indexes.j()), measurementId_, indexes.k());
			case 2:
				return dataStore_->value(AMnDIndex(indexes.i(), indexes.j()), measurementId_, AMnDIndex(indexes.k(), indexes.l()));	// 4D data: really?
			}

		default:
			break;	// nothing... We'll pick this up below in the general case.
		}

		// general case:
		AMnDIndex scanIndex(scanAxesCount_, AMnDIndex::DoNotInit);
		for(int i=0; i<scanAxesCount_; i++)
			scanIndex[i] = indexes.at(i);

		AMnDIndex measurementIndex(measurementAxesCount_, AMnDIndex::DoNotInit);
		for(int i=0; i<measurementAxesCount_; i++)
			measurementIndex[i] = indexes.at(i+scanAxesCount_);

		return dataStore_->value(scanIndex, measurementId_, measurementIndex);
	}


	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
	/*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

\code
int outputSize = indexStart.totalPointsTo(indexEnd);
\endcode
*/
	virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const {

		if(!isValid())
			return false;
		if(indexStart.rank() != rank() || indexEnd.rank() != rank())
			return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
		for(int mu=rank()-1; mu>=0; --mu) {
			if(indexEnd.at(mu) >= size(mu))
				return false;
			if(indexEnd.at(mu) < indexStart.at(mu))
				return false;
		}
#endif

		// optimize for common dimensional cases
		switch(scanAxesCount_) {
		case 0:	// scalar scan space. indexes are all in measurement space.
			return dataStore_->values(AMnDIndex(), AMnDIndex(), measurementId_, indexStart, indexEnd, outputValues);
		case 1:
			switch(measurementAxesCount_) {
			case 0:  // 1d data: one scan axis, scalar measurements
				return dataStore_->values(indexStart, indexEnd, measurementId_, AMnDIndex(), AMnDIndex(), outputValues);
			case 1: // 2d data: one scan axis, one measurement axis (ie: XAS scan with 1D detector, like SDD)
				return dataStore_->values(indexStart.i(), indexEnd.i(), measurementId_, indexStart.j(), indexEnd.j(), outputValues);
			case 2: // 3d data: one scan axis, two measurement axes (ie: XAS scan with 2D detector, like XES)
				return dataStore_->values(indexStart.i(), indexEnd.i(), measurementId_, AMnDIndex(indexStart.j(), indexStart.k()), AMnDIndex(indexEnd.j(), indexEnd.k()), outputValues);
			}
		case 2:
			switch(measurementAxesCount_) {
			case 0:
				return dataStore_->values(AMnDIndex(indexStart.i(), indexStart.j()), AMnDIndex(indexEnd.i(), indexEnd.j()), measurementId_, AMnDIndex(), AMnDIndex(), outputValues);
			case 1:
				return dataStore_->values(AMnDIndex(indexStart.i(), indexStart.j()), AMnDIndex(indexEnd.i(), indexEnd.j()), measurementId_, indexStart.k(), indexEnd.k(), outputValues);
			case 2: // 4D data: really?
				return dataStore_->values(AMnDIndex(indexStart.i(), indexStart.j()), AMnDIndex(indexEnd.i(), indexEnd.j()), measurementId_, AMnDIndex(indexStart.k(), indexStart.l()), AMnDIndex(indexEnd.k(), indexEnd.l()), outputValues);
			}

		default: {
			// general case:
			AMnDIndex scanIndexStart(scanAxesCount_, AMnDIndex::DoNotInit);
			AMnDIndex scanIndexEnd(scanAxesCount_, AMnDIndex::DoNotInit);
			for(int mu=0; mu<scanAxesCount_; ++mu) {
				scanIndexStart[mu] = indexStart.at(mu);
				scanIndexEnd[mu] = indexEnd.at(mu);
			}

			AMnDIndex measurementIndexStart(measurementAxesCount_, AMnDIndex::DoNotInit);
			AMnDIndex measurementIndexEnd(measurementAxesCount_, AMnDIndex::DoNotInit);
			for(int mu=0; mu<measurementAxesCount_; ++mu) {
				measurementIndexStart[mu] = indexStart.at(mu+scanAxesCount_);
				measurementIndexEnd[mu] = indexEnd.at(mu+scanAxesCount_);
			}

			return dataStore_->values(scanIndexStart, scanIndexEnd, measurementId_, measurementIndexStart, measurementIndexEnd, outputValues);
		}
		}
	}

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const {
		if(!isValid())
			return AMNumber(AMNumber::InvalidError);
		if(axisNumber < scanAxesCount_)
			return dataStore_->axisValue(axisNumber, index);	// value along a scan axis
		else if (axisNumber < rank() ){

			const AMAxisInfo& axis = axes_.at(axisNumber);
			return (double)axis.start + (double)axis.increment*index;	/// \todo Implement non-uniform measurement axes
		}
		else
			return AMNumber(AMNumber::DimensionError);	// no such axis.

	}

	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
    virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

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
	}



	/// Specify that this data source should be visible (in plots and graphical displays).  Users are free to toggle this visibility.
	/*! Re-implemented from AMDataSource to call setModified().  */
	virtual void setVisibleInPlots(bool isVisible) { if(isVisible == visibleInPlots()) return; AMDataSource::setVisibleInPlots(isVisible); setModified(true); }

	/// Specify that this data source should be hidden from users by default. (ie: it contains some programming internals). This means that users shouldn't see it, or be able to toggle its visibility.
	/*! Re-implemented from AMDataSource to call setModified().  */
	virtual void setHiddenFromUsers(bool isHidden = true) { AMDataSource::setHiddenFromUsers(isHidden); setModified(true); }

	/// Re-implemented from AMDbObject to set the AMDataSource name once we have an AMDbObject::name()
	bool loadFromDb(AMDatabase *db, int id);

	/// Creates an editor widget that reports the dimensions and size of the data source... And for 0D data sources, includes the value.
	virtual QWidget* createEditorWidget();

protected slots:

	// Forwarding signals from the data store:
	//////////////////////
	/// Called when the data changes within the region described. We only care when \c measurementId matches our measurementId_
	void onDataChanged(const AMnDIndex& scanIndexStart, const AMnDIndex scanIndexEnd, int measurementId);
	/// Called when the size of a scan axis changes. (Data stores can only change the size of their FIRST scan axis.)
	void onScanAxisSizeChanged();

protected:
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
