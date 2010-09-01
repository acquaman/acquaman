#ifndef AMSAMPLEPLATE_H
#define AMSAMPLEPLATE_H

#include <QList>
#include <QDateTime>
#include <QAbstractListModel>
#include <QDebug>

#include "AMSample.h"
#include "AMControlSetInfo.h"
#include "AMBiHash.h"
#include "AMDbObject.h"
#include "acquaman.h"

/// An AMSamplePosition stores a combination of an AMSample, and an AMControlSetInfo position. As an entity on a sample plate, an AMSamplePosition is all you need to know to get there.
/*! This object takes ownership of the AMSample and the AMControlSetInfo, and will delete them  when destroyed.*/
class AMSamplePosition : public QObject
{
Q_OBJECT
public:
	AMSamplePosition(AMSample *sample, AMControlSetInfo *position, QObject *parent = 0);

	/// Deleting an AMSamplePosition deletes its sample and AMControlSetInfo as well
	~AMSamplePosition() {
		if(sample_) {
			delete sample_;
			sample_ = 0;
		}
		if(position_) {
			delete position_;
			position_ = 0;
		}
	}

	AMSample* sample();
	AMControlSetInfo* position();

public slots:
	void setSample(AMSample *sample);
	void setPosition(AMControlSetInfo *position);

signals:
	void positionValuesChanged(int index);
	void sampleLoadedFromDb();
	void positionLoadedFromDb();

protected:
	AMSample *sample_;
	AMControlSetInfo *position_;
};

Q_DECLARE_METATYPE(AMSamplePosition*);

/// This model stores a list of AMSamplePositions for AMSamplePlate.
/*!
The samples are stored in rows, under the first (only) column.
The model takes ownership of the AMSamplePositions, and will delete them when removed.

Roles:
- The name of each sample is returned via the Qt::DisplayRole, and a pointer to the AMSamplePosition object is returned with the AM::PointerRole.

The dataChanged() signal is emitted when any of the properties of the sample or position within the AMSamplePosition object are changed. (Even those that are not accessible through a data() role.)

For convenience, you can retrieve an AMSamplePosition pointer by index with samplePositionAt(int row), and insert a new AMSamplePosition with insert(int row, AMSamplePosition*).

*/
class AMSamplePlateModel : public QStandardItemModel
{
Q_OBJECT
public:
	/// Construct an empty sample plate model
	AMSamplePlateModel(QObject *parent = 0);
	/// Deletes all internal samplePosition objects (automatic, as children)
	~AMSamplePlateModel() {}

	/// Re-implemented to lookup the current name of the sample, for Qt::DisplayRole
	QVariant data(const QModelIndex &index, int role) const;
	/// Re-implemented to delete the AMSamplePositions
	virtual bool removeRows(int row, int count, const QModelIndex &parent);

	// Convenience functions
	////////////////////////////
	/// Convenience function to retrieve an AMSamplePosition. Returns 0 if the row is out of range.
	AMSamplePosition* samplePositionAt(int row) const {
		if(row<0 || row>=count()) return 0;
		return data(indexForRow(row), AM::PointerRole).value<AMSamplePosition*>();
	}

	/// Function to insert an AMSamplePosition. This model takes ownership of the sample position, and will delete it when removed from the model.  If row == -1, the sample will be appended.
	void insertSamplePosition(int row, AMSamplePosition* newSamplePosition);

	/// Same as rowCount().
	int count() const { return rowCount(); }

	/// Create a model index for a given row number. The index will be invalid if the row doesn't exist.
	QModelIndex indexForRow(int row) const { if(row<0 || row>=count()) return QModelIndex(); return index(row, 0); }

	/// Returns the row of an existing AMSamplePosition, or -1 if not found.
	int indexOf(AMSamplePosition* existing) const;
	/// Returns the row of an existing AMSamplePosition, located via sample name.  Returns -1 if not found.
	int indexOfSampleName(const QString& sampleName) const;


protected slots:
	/// Handle changes from AMSamplePositions, and emit our own dataChanged() signal
	void onSampleLoadedFromDb();
	/// Handle changes from AMSamplePositions, and emit our own dataChanged() signal
	void onPositionLoadedFromDb();

};


class AMSamplePlate : public AMDbObject
{
Q_OBJECT
public:

	explicit AMSamplePlate(QObject *parent = 0);

	~AMSamplePlate() {}

	AMSamplePlateModel* model();
	QString plateName() const;
	QString userName() const;
	QDateTime createTime() const;
	QString timeString() const;
	int count() const;

	AMSamplePosition* samplePositionAt(int index) { return model()->samplePositionAt(index); }
	AMSamplePosition* samplePositionByName(const QString &name) { return model()->samplePositionAt(model()->indexOfSampleName(name)); }

	AMSample* sampleAt(int index);
	AMSample* sampleByName(const QString &name);

	AMControlSetInfo* positionAt(int index);
	AMControlSetInfo* positionByName(const QString &name);

	int indexOf(const QString &name) { return model()->indexOfSampleName(name); }

	/// Sample plates are valid after being loaded or saved. You can also manually mark a sample plate as invalid by calling setInvalid(). (For example, at the beginning of a sample transfer.)
	bool valid() const { return valid_; }
	/// Mark a sample plate as 'invalid' (ie: non-existent, or not trustworthy. It won't become valid until successfully saved to the db, or loaded from the db.)
	void setInvalid() { valid_ = false; }
	/// This is a convenience function that tells a sample plate object to "become another sample plate" that already exists in the user's database.  It's equivalent to "loadFromDb(AMDatabase::userdb(), newSamplePlateId);"
	void changeSamplePlate(int newSamplePlateId) { loadFromDb(AMDatabase::userdb(), newSamplePlateId); }


	// AMDbObject database interface
	////////////////////////////////////
	/// Specify all of our unique pieces of meta-data (excluding those inherited from base classes -- ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys();

	/// This function needs to be overloaded to return all the available pieces of meta data for this type of object, including those inherited from base classes. (ie: own + base classes'). We simply append our unique meta-data onto the base class:
	static QList<AMMetaMetaData> metaDataKeys();

	/// This virtual function returns all the available pieces of meta data for this type of object, by introspecting it's most detailed type. (ie: own + base classes' + subclasses')
	virtual QList<AMMetaMetaData> metaDataAllKeys() const;

	/// We want to store this in a separate table (so that it's easy to create relationships between detectors and scan objects).  Therefore, we reimplement databaseTableName():
	virtual QString databaseTableName() const;

	/// Load yourself from the database. (returns true on success)
	/*! Re-implemented from AMDbObject. */
	virtual bool loadFromDb(AMDatabase* db, int id);

	/// Store or update self in the database. (returns true on success)
	/*! Re-implemented from AMDbObject::storeToDb(), this version also writes out the dimension and rank, even though they aren't strictly part of the meta-data.
	  */
	virtual bool storeToDb(AMDatabase* db);

	/// Reimplemented from AMDbObject; provides a general human-readable description
	virtual QString typeDescription() const;

signals:
	void samplePositionChanged(int index);
	void samplePositionAdded(int index);
	void samplePositionRemoved(int index);

	/// This signal is emitted when the sample plate is "changed out", or re-loaded. It may now contain completely different samples, at completely different positions.
	void samplePlateChanged(bool isValid);

public slots:

	// bool setSamplePosition(int index, AMSamplePosition *sp);

	bool addSamplePosition(int index, AMSamplePosition *sp);
	bool addSamplePosition(int index, AMSample *sample, AMControlSetInfo *position);
	bool appendSamplePosition(AMSamplePosition *sp);
	bool appendSamplePosition(AMSample *sample, AMControlSetInfo *position);

	bool removeSamplePosition(AMSamplePosition *sp);
	bool removeSamplePosition(int index);

protected slots:
	void onDataChanged(QModelIndex a, QModelIndex b);
	void onRowsInserted(QModelIndex parent, int start, int end);
	void onRowsRemoved(QModelIndex parent, int start, int end);

protected:
	void setupModel();

protected:
	AMSamplePlateModel *samples_;

	/// Sample plates are valid when they've been successfully stored to or loaded from the Db.
	bool valid_;

	/// wtf?
	int insertRowLatch;


};





#endif // AMSAMPLEPLATE_H
