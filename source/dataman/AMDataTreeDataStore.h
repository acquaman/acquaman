#ifndef AMDATATREEDATASTORE_H
#define AMDATATREEDATASTORE_H

#include "dataman/AMDbObject.h"
#include "util/AMOrderedSet.h"
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

	// Right now can only add outer axis, not insert inner ones
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

	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const int* inputData, int numArrayElements);
	virtual bool setValue(const AMnDIndex &scanIndex, int measurementId, const double* inputData, int numArrayElements);

	void dataStoreDimensionsPuke();
	void dataStoreDataPuke();
	void dataStoreBasePuke();

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

	void appendToDepth(AMDataTree* dataTree, QList<int> newCounts, AMDataTree* initializerTree = NULL);
	AMDataTree* measurementInfoToTree(const AMMeasurementInfo &measurementDetails, QList<AMAxisInfo> remainingAxes);
	AMDataTree* bottomTreeFinder(AMDataTree* treeTop, const AMMeasurementInfo &measurementDetails, const int offset);
	bool setValueFillBottom(AMDataTree* dataTree, const int *inputData, const int bottomDimension);
	bool setValueFillBottom(AMDataTree* dataTree, const double *inputData, const int bottomDimension);
	bool measurementIndexExists(int measurementId) const;

	void dataStoreDimensionsPukeHelper(const AMDataTree *dataTree, const int depthToGo, const int depthNow);
	void dataStoreDataPukeHelper(const AMDataTree *dataTree, const int depthToGo, const int depthNow);

protected:
	AMDataTree *dataTree_;
	AMDataTree *baseTree_;
	bool emptyTree_;
	AMOrderedSet<QString, AMMeasurementInfo> measurements_;
	AMOrderedSet<QString, AMAxisInfo> axes_;
};

void dataTreeColumnsPuke(const AMDataTree *dataTree);

#endif // AMDATATREEDATASTORE_H
