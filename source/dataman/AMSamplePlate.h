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
	Q_PROPERTY(QDateTime createTime READ createTime WRITE loadCreateTime)
	Q_PROPERTY(AMIntList sampleIDs READ sampleIDs WRITE loadSampleIDs)
	Q_PROPERTY(AMIntList positionIDs READ positionIDs WRITE loadPositionIDs)

	Q_CLASSINFO("AMDbObject_Attributes", "description=List of Samples and their Positions on a Sample Plate")
	Q_CLASSINFO("createTime", "createIndex=true")

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

	AMControlInfoList* positionAt(size_t index);
	AMControlInfoList* positionByName(const QString &name);

	int indexOf(const QString &name);

	/// Sample plates are valid after being loaded or saved. You can also manually mark a sample plate as invalid by calling setInvalid(). (For example, at the beginning of a sample transfer.)
	bool valid() const { return valid_; }
	/// Mark a sample plate as 'invalid' (ie: non-existent, or not trustworthy. It won't become valid until successfully saved to the db, or loaded from the db.)
	void setInvalid() { valid_ = false; }
	/// This is a convenience function that tells a sample plate object to "become another sample plate" that already exists in the user's database.  It's equivalent to "loadFromDb(AMDatabase::userdb(), newSamplePlateId);"
	void changeSamplePlate(int newSamplePlateId) { loadFromDb(AMDatabase::userdb(), newSamplePlateId); }

	/// Load yourself from the database. (returns true on success)
	/*! Re-implemented from AMDbObject. */
	virtual bool loadFromDb(AMDatabase* db, int id);

	/// Store or update self in the database. (returns true on success)
	/*! Re-implemented from AMDbObject::storeToDb(), this version also writes out the dimension and rank, even though they aren't strictly part of the meta-data.
	  */
	virtual bool storeToDb(AMDatabase* db);



signals:
	void samplePositionChanged(int index);
	void samplePositionAdded(int index);
	void samplePositionRemoved(int index);

	/// This signal is emitted when the sample plate is "changed out", or re-loaded. It may now contain completely different samples, at completely different positions.
	void samplePlateChanged(bool isValid);

public slots:

	bool setSamplePosition(size_t index, AMSamplePosition *sp);

	bool addSamplePosition(size_t index, AMSamplePosition *sp);
	bool addSamplePosition(size_t index, AMSample *sample, AMControlInfoList *position);
	bool appendSamplePosition(AMSamplePosition *sp);
	bool appendSamplePosition(AMSample *sample, AMControlInfoList *position);

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


	/// Time when this sample plate was first created
	QDateTime createTime_;
	void loadCreateTime(const QDateTime& createTime) { createTime_ = createTime; }
	/// list of sample ids, used for storing in the DB. May not be up to date always...
	AMIntList sampleIDs_;
	AMIntList sampleIDs() const { return sampleIDs_; }
	void loadSampleIDs(const AMIntList& sampleIDs) { sampleIDs_ = sampleIDs; }

	/// list of position ids, used for storing in the DB. Maynot be up to date always...
	AMIntList positionIDs_;
	AMIntList positionIDs() const { return positionIDs_; }
	void loadPositionIDs(const AMIntList& positionIDs) { positionIDs_ = positionIDs; }

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
	AMSamplePosition(AMSample *sample, AMControlInfoList *position, QObject *parent = 0);

	AMSample* sample();
	AMControlInfoList* position();

public slots:
	void setSample(AMSample *sample);
	void setPosition(AMControlInfoList *position);

signals:
	void positionValuesChanged(int index);
	void sampleLoadedFromDb();
	void positionLoadedFromDb();

protected:
	AMSample *sample_;
	AMControlInfoList *position_;
};

#endif // AMSAMPLEPLATE_H
