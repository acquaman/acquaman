#ifndef AMLIGHTWEIGHTSCANINFOCOLLECTION_H
#define AMLIGHTWEIGHTSCANINFOCOLLECTION_H

#include <QList>
#include <QHash>
#include <QtSql>
#include "AMLightweightScanInfo.h"

class AMDatabase;
/// A class representing a bulk loaded collection of Scan's database info
class AMLightweightScanInfoCollection : public QObject
{
	Q_OBJECT
public:
	/// Creates an instance of an AMLightweightScanInfoCollection loaded from the provided database
	AMLightweightScanInfoCollection(AMDatabase* database);
	/// Returns the scan with the provided id in the standard
	/// URL format: amd://databaseConnectionName/tableName/objectId
	QUrl getScanUrl(int id) const;
	/// Returns the AMLightWeightScanInfo at index position i in the list. i must be a valid index position in
	/// in the list (i.e. 0 <= i < size())
	AMLightweightScanInfo* at(int index) const;
	/// Returns the number of items in the list. This is effectively the same as size().
	int count() const;
	/// Returns the index position of the first occurrence of value in the list, searching forward from index
	/// position from. Returns -1 if no item matched
	int indexOf(AMLightweightScanInfo* scan, int from = 0) const;
	/// A map of all the runs currently known. Maps run ids to run name
	const QHash<int, QString> runMap() const;
signals:
	/// Emitted just before a scan info is added to the collection
	void scanAboutToBeAdded(int rowIndex);
	/// Emitted whenever a scan info is finished being added to the collection after the initial loading
	void scanAdded();
	/// Emitted whenever a scan info is altered
	void scanUpdated(int rowIndex);
	/// Emitted just before a scan info is removed from the collection
	void scanAboutToBeRemoved(int rowIndex);
	/// Emitted whenever a scan info is finished being removed from the collection
	void scanRemoved();
	/// Emitted whenever a scan is about to remove its thumbnail from the collection
	void scanThumbnailAboutToBeRemoved(int scanIndex, int oldThumbnailIndexStart, int oldThumbnailIndexEnd);
	/// Emitted whenever a scan has its thumbnail removed from the collection
	void scanThumbnailRemoved();
	/// Emitted whenever a scan is about to have a scan added
	void scanThumbnailAboutToBeAdded();
	/// Emitted whenever a thumbnail is finished being added to a scan
	void scanThumbnailAdded();
	/// Emitted whenever a run is about to be added to the map
	void runAboutToBeAdded();
	/// Emitted whenever a run is finished being added
	void runAdded();
protected slots:
	/// Slot to handle the database signal indicating that a new item has been added to the database
	void onDbItemAdded(const QString& tableName, int id);
	/// Slot to handle the database signal indicating that an existing item in the database has been updated
	void onDbItemUpdated(const QString& tableName, int id);
	/// Slot to handle the database signal indicating that an item has been removed from the database
	void onDbItemRemoved(const QString& tableName, int oldId);
protected:
	/// The index in the list of the Scan info with the provided id, or -1 if
	/// none is matched.
	int indexOfScanWithId(int id) const;
private:
	/// The database from which the scan infos will be retrieved
	AMDatabase* database_;
	/// A cache of the mapping of runIds to run names currently in the database
	QHash<int, QString> runMapCache_;
	/// A list of the contained AMLightweightScanInfos
	QList<AMLightweightScanInfo*> scanInfos_;
};

#endif // AMLIGHTWEIGHTSCANINFOCOLLECTION_H
