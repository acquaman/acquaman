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

class AMSamplePosition;
class AMSamplePlateModel;

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
	int count();

	AMSamplePosition* samplePositionAt(size_t index);
	AMSamplePosition* samplePositionByName(const QString &name);

	AMSample* sampleAt(size_t index);
	AMSample* sampleByName(const QString &name);

	AMControlSetInfo* positionAt(size_t index);
	AMControlSetInfo* positionByName(const QString &name);

	int indexOf(const QString &name);

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

	bool setSamplePosition(size_t index, AMSamplePosition *sp);

	bool addSamplePosition(size_t index, AMSamplePosition *sp);
	bool addSamplePosition(size_t index, AMSample *sample, AMControlSetInfo *position);
	bool appendSamplePosition(AMSamplePosition *sp);
	bool appendSamplePosition(AMSample *sample, AMControlSetInfo *position);

	bool removeSamplePosition(AMSamplePosition *sp);
	bool removeSamplePosition(size_t index);

protected slots:
	void onDataChanged(QModelIndex a, QModelIndex b);
	void onRowsInserted(QModelIndex parent, int start, int end);
	void onRowsRemoved(QModelIndex parent, int start, int end);

protected:
	bool setupModel();

protected:
	AMSamplePlateModel *samples_;
	AMBiHash<QString, AMSamplePosition*> sampleName2samplePosition_;

	/// Sample plates are valid when they've been successfully stored to or loaded from the Db.
	bool valid_;


private:
	int insertRowLatch;
};

class AMSamplePlateModel : public QAbstractListModel
{
Q_OBJECT
public:
	AMSamplePlateModel(QObject *parent = 0);

	int rowCount(const QModelIndex & /*parent*/) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
	Qt::ItemFlags flags(const QModelIndex &index) const;

protected slots:
	void onSampleLoadedFromDb();
	void onPositionLoadedFromDb();

protected:
	QList<AMSamplePosition*> *samples_;
};

class AMSamplePosition : public QObject
{
Q_OBJECT
public:
	AMSamplePosition(AMSample *sample, AMControlSetInfo *position, QObject *parent = 0);

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

#endif // AMSAMPLEPLATE_H
