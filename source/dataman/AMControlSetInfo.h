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


#ifndef AMCONTROLSETINFO_H
#define AMCONTROLSETINFO_H

#include "dataman/AMDbObject.h"
#include <QStringList>
#include <QAbstractTableModel>
#include <QDebug>

class AMControlInfoSetModel;
class AMControlInfo;

class AMControlInfoSet : public AMDbObject
{
Q_OBJECT
	Q_PROPERTY(QString description READ description WRITE setDescription)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Set of control settings")

public:
	explicit AMControlInfoSet(QObject *parent = 0);
	AMControlInfoSet(AMControlInfoSet *copyFrom, QObject *parent = 0);

	~AMControlInfoSet() {}

	AMControlInfoSetModel* model();

	int count() const;

	QString nameAt(size_t index) const;

	double valueAt(size_t index) const;

	double minimumAt(size_t index) const;

	double maximumAt(size_t index) const;

	QString unitsAt(size_t index) const;

	QString description() const { return description_; }



	/// Load yourself from the database. (returns true on success)
	/*! Re-implemented from AMDbObject. */
	virtual bool loadFromDb(AMDatabase* db, int id);

	/// Store or update self in the database. (returns true on success)
	/*! Re-implemented from AMDbObject::storeToDb(), this version also writes out the dimension and rank, even though they aren't strictly part of the meta-data.
	  */
	virtual bool storeToDb(AMDatabase* db);



signals:
	void valuesChanged(int index);
	void controlAdded(int index);
	void controlRemoved(int index);

public slots:
	/// Convenience function to set the human-readable description
	void setDescription(const QString& description);

	bool setNameAt(size_t index, QString name);

	bool setValueAt(size_t index, double value);

	bool setMinimumAt(size_t index, double minimum);

	bool setMaximumAt(size_t index, double maximum);

	bool setUnitsAt(size_t, QString units);

	bool setControlAt(size_t index, QString name, double value, double minimum, double maximum, QString units);

	bool addControlAt(size_t index, QString name, double value, double minimum, double maximum, QString units);

	bool removeControlAt(size_t index);

	void copyFrom(AMControlInfoSet *copyFrom);

protected slots:
	void onDataChanged(QModelIndex a,QModelIndex b);
	void onRowsInserted(QModelIndex parent, int start, int end);
	void onRowsRemoved(QModelIndex parent, int start, int end);

protected:
	virtual bool setupModel();

protected:
	AMControlInfoSetModel *ctrlInfoList_;
	QString description_;

private:
	int insertRowLatch;
};

class AMControlInfoSetModel : public QAbstractTableModel
{
Q_OBJECT
public:
	AMControlInfoSetModel(QObject *parent = 0);

	/// Returns the number of controls in the list to generate the number of rows in a table or list
	int rowCount(const QModelIndex & /*parent*/) const;
	/// Returns "5" statically. There are always five fields in the control's slim info: name, value, minimum, maximum, and units.
	int columnCount(const QModelIndex & /*parent*/) const;
	/// Retrieves the data from an index (row and column) and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
	QVariant data(const QModelIndex &index, int role) const;
	/// Retrieves the header data for a column or row and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	/// Sets the data value at an index (row and column). Only valid role is Qt::DisplayRole right now.
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());
	/// This allows editing of values within range (for ex: in a QTableView)
	Qt::ItemFlags flags(const QModelIndex &index) const;

protected:
	/// Internal pointer to the list of AMControlSlimInfo
	QList<AMControlInfo*> *ctrlInfoList_;
};

class AMControlInfo : public QObject
{
Q_OBJECT
public:
	AMControlInfo(QString name, double value, double minimum, double maximum, QString units, QObject *parent = 0);

	const QString& name() const;
	double value() const;
	double minimum() const;
	double maximum() const;
	const QString& units() const;

public slots:
	void setName(const QString &name);
	void setValue(double value);
	void setMinimum(double minimum);
	void setMaximum(double maximum);
	void setUnits(const QString &units);

protected:
	QString name_;
	double value_;
	double minimum_;
	double maximum_;
	QString units_;
};

#endif // AMCONTROLSETINFO_H
