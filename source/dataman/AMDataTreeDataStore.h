#ifndef AMDATATREEDATASTORE_H
#define AMDATATREEDATASTORE_H

#include "dataman/AMDbObject.h"
#include "dataman/AMDataStore.h"
#include "dataman/AMDataTree.h"

class AMDataTreeDataStore : public AMDbObject, public AMDataStore
{
	Q_OBJECT
public:
	explicit AMDataTreeDataStore(const AMAxisInfo &initialAxis, QObject *parent = 0);

	virtual ~AMDataTreeDataStore();

	AMDataTree* dataTree() { return dataTree_; }

signals:

public slots:
	virtual bool addMeasurement(const AMMeasurementInfo &measurementDetails);
	virtual int idOfMeasurement(const QString &measurementName) const;
	virtual AMMeasurementInfo measurementAt(int id) const;
	virtual int measurementCount() const;

	virtual bool addScanAxis(const AMAxisInfo &axisDetails);
	virtual int idOfScanAxis(const QString &axisName) const;
	virtual AMAxisInfo scanAxisAt(int id) const;
	virtual int scanAxesCount() const;

	/// Return the sizes of all the scan axes, in order.
	virtual AMnDIndex scanSize() const;
	/// Return the size along a specific axis, by \c id.
	virtual int scanSize(int axisId) const;


	virtual AMNumber value(const AMnDIndex &scanIndex, int measurementId, const AMnDIndex &measurementIndex) const;
	virtual AMNumber axisValue(int axisId, int axisIndex) const;
	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const AMnDIndex &measurementIndex, const AMNumber &newValue);
	virtual bool setAxisValue(int axisId, int axisIndex, AMNumber newValue);

	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const int* inputData);
	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const double* inputData);

protected:
	/// Implementing subclasses must provide a beginInsertRowsImplementation() which creates space for the new measurements.  When this function completes, it should be valid to setValue()s within the new scan space. Return false if the request is not possible.
	virtual bool beginInsertRowsImplementation(int axisId, int numRows, int atRowIndex);
	/// Implementing subclasses may provide a endInsertRowsImplementation(), which will be called at the beginning of endInsertRows(). You don't need to take care of emitting sizeChanged() and dataChanged().... that's done for you in endInsertRows().  The base class implementation of this does nothing.
	virtual void endInsertRowsImplementation(int axisId, int numRows, int atRowIndex);
	/// Implementing subclasses must provide a clearImplementation(), which removes all data values and sets the size of each axis to 0.  It should leave the set of configured measurements as-is.
	virtual void clearScanDataPointsImplementation();
	/// Implementing subclasses must provide a clearMeasurementsImplementation(), which clears the set of configured measurements.  They can assume that the set of scan data values is already cleared.
	virtual void clearMeasurementsImplementation() {

	}

	AMDataTree* measurementInfoToTree(const AMMeasurementInfo &measurementDetails, QList<AMAxisInfo> remainingAxes);

protected:
	AMDataTree *dataTree_;
	AMDataTree *baseTree_;
	int axisCount_;
#warning "Need to change this to an ordered set eventually"
	QList<AMMeasurementInfo> measurements_;
};

void dataTreeColumnsPuke(const AMDataTree *dataTree);

#endif // AMDATATREEDATASTORE_H
