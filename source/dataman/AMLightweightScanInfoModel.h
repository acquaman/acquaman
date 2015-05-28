#ifndef AMLIGHTWEIGHTSCANINFOMODEL_H
#define AMLIGHTWEIGHTSCANINFOMODEL_H

#include <QAbstractItemModel>
#include "AMLightweightScanInfoCollection.h"

/**
 * Qt standard model for representing AMLightweightScanInfo in such a way as to be usable by QAbstractItemViews
 * and their descendents. The model represents a two stage heirarchy, with the top level being the scans, and
 * the level below each scan being its thumbnails.
 */
class AMLightweightScanInfoModel : public QAbstractItemModel
{
	Q_OBJECT
	/// The collection of AMLightweightScanInfos from which the model obtains its data
	AMLightweightScanInfoCollection* scanInfo_;
public:
	/// Creates an instance of an AMLightweightScanInfoModel to represent the data in the provided
	/// AMLightweightScanInfoCollection.
	explicit AMLightweightScanInfoModel(AMLightweightScanInfoCollection* scanInfo, QObject *parent = 0);
	/// Returns the text to display in the given header. Only returns valid values for Qt::Horizontal
	/// orientation and Qt::DisplayRole role.
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	/// Returns the QVariantData associated with the given index of the model. In brief:
	/// returns data for the scan if the index has an invalid parent, or
	/// returns data for the thumbnail if the index has a valid parent
	QVariant data(const QModelIndex &index, int role) const;
	/// Returns the row count for the given index. If this index is invalid, the number of scans is returned.
	/// If the index is valid, then the thumbnailCount of the scan to which parent refers is returned
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	/// Returns the column count for the given index. If the index is invalid, the column count for
	/// scans is returned (8) if not then the column count for thumbnails is returned (3)
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	/// Returns a QModelIndex for the provided row, column and parent index
	QModelIndex index(int row, int column, const QModelIndex &parent) const;
	/// Returns the parent of the provided index. If the index has no parent, then an invalid
	/// QModelIndex is returned
	QModelIndex parent(const QModelIndex &child) const;
	/// returns a standard url for the given index.
	/// URL format: amd://databaseConnectionName/tableName/objectId
	/// If the index refers to a thumbnail, then the URL of the parent scan is returned
	QUrl rowToUrl(const QModelIndex& index) const;
	/// Returns a map of all the known runs. Maps runIDs to runNames.
	const QHash<int, QString> runMap() const;
	/// Returns the flags for the items. In cases where the index is valid and has no
	/// valid parent (ie is for a scan, not a data source) then the flags are default
	/// with DragEnabled, else they are just the default flags obtains from QAbstractItemModel::flags()
	Qt::ItemFlags flags(const QModelIndex& index) const;
	/// Returns the data for drag operations which will be passed outside of the views. In this case
	/// it is a list of QUrls which indicate where the Scans can be loaded from.
	QMimeData* mimeData(const QModelIndexList &indexes) const;
	/// Whether or not the scan with the provided Index belongs to the experiment with the
	/// provided id.
	bool belongsToExperiment(const QModelIndex index, int experimentId) const;
public slots:

	// Functions which respond to signals indicating changes in the underlying collection
	/////////////////////

	/// Handles the collection signalling that a scan is about to be added at the given index
	void onScanInfoAboutToBeAdded(int newIndex);
	/// Handles the collection signalling that it has finished adding a scan
	void onScanInfoAdded();
	/// Handles the collection signalling that one of its members has been changed
	void onScanInfoUpdated(int updatedIndex);
	/// Handles the collection signalling that a given scan is about to have thumbnails added
	void onScanThumbnailAboutToBeAdded();
	/// Handles the collection signalling that it is done adding thumbnails to a scan
	void onScanThumbnailAdded();
	/// Handles the collection signalling that a given scan has had a given thumbnail removed
	void onScanThumbnailAboutToBeRemoved(int scanIndex, int oldThumbnailIndexStart, int oldThumbnailIndexEnd);
	/// Handles the collection signalling that it has finsished remvoing a thumbnail
	void onScanThumbnailRemoved();
	/// Handles the collection signalling that a scan is about to be removed
	void onScanInfoAboutToBeRemoved(int oldIndex);
	/// Handles the collection signalling that it has finished removing a scan
	void onScanInfoRemoved();

signals:
	/// Signals that the run map within the underlying collection has been updated
	void runMapUpdated();
protected:
	/// Helper function which returns the data for an index which is known to be a scan
	QVariant getScanData(const QModelIndex& index, int role) const;
	/// Helper function which returns the data for an index which is known to be a thumbnail
	QVariant getThumbnailData(const QModelIndex& index, int role) const;

};

#endif // AMLIGHTWEIGHTSCANINFOMODEL_H
