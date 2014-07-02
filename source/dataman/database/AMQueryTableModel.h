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


#ifndef AMQUERYTABLEMODEL_H
#define AMQUERYTABLEMODEL_H

#include <QAbstractTableModel>
#include "dataman/database/AMDatabase.h"
#include <QStringList>
#include <QVector>
#include <QVariant>

/// A helper class for AMScanQueryModel, this class encapsulates the lookup information for a column in that model.
class AMQueryTableModelColumnInfo {

public:
	/// Default constructor
	AMQueryTableModelColumnInfo(const QString& iHeading = "Row",
							   const QString& iName = "id",
							   bool iIsForeignKey = false,
							   const QString& iForeignKeyTable = QString(),
							   const QString& iForeignKeyReplacementColumn = QString(),
							   const QString& iForeignKeyMatchColumn = "id")
		: heading(iHeading),
		  name(iName),
		  isForeignKey(iIsForeignKey),
		  foreignKeyTable(iForeignKeyTable),
		  foreignKeyReplacementColumn(iForeignKeyReplacementColumn),
		  foreignKeyMatchColumn(iForeignKeyMatchColumn)
	{
		if(isForeignKey)
			foreignKeyCache = new QHash<QString, QVariant>();
		else
			foreignKeyCache = 0;
	}

	/// Copy constructor.
	/*! \note If foreignKeyCache was not a pointer, we could skip these and use the default copy and assigment operators. However, a default-created QHash takes up memory, and we don't want to do that in the case where isForeignKey is false (ie: don't need the hash).*/
	AMQueryTableModelColumnInfo(const AMQueryTableModelColumnInfo& other)
		: heading(other.heading),
		  name(other.name),
		  isForeignKey(other.isForeignKey),
		  foreignKeyTable(other.foreignKeyTable),
		  foreignKeyReplacementColumn(other.foreignKeyReplacementColumn),
		  foreignKeyMatchColumn(other.foreignKeyMatchColumn)
	{
		if(isForeignKey)
			foreignKeyCache = new QHash<QString, QVariant>();
		else
			foreignKeyCache = 0;
	}

	/// Assignment operator.
	AMQueryTableModelColumnInfo& operator=(const AMQueryTableModelColumnInfo& other) {
		heading = other.heading;
		name = other.name;
		isForeignKey = other.isForeignKey;
		foreignKeyTable = other.foreignKeyTable;
		foreignKeyReplacementColumn = other.foreignKeyReplacementColumn;
		foreignKeyMatchColumn = other.foreignKeyMatchColumn;
		if(isForeignKey)
			foreignKeyCache->clear();

		return *this;
	}

	virtual ~AMQueryTableModelColumnInfo() {
		delete foreignKeyCache;
	}

	/// User-visible text for the column heading
	QString heading;
	/// Database name of the column
	QString name;
	/// True if this column should be used as a foreign-key lookup. If true, the column data will be replaced with data from an external table.
	bool isForeignKey;
	/// If isForeignKey is true, this is the external table to look in for the replacement data.
	QString foreignKeyTable;
	/// If isForeignKey is true, this is the name of the column in the external table to be used as replacement data.
	QString foreignKeyReplacementColumn;
	/// If isForeignKey is true, this is the name of the column in the external table to match against the foreign key (usually "id")
	QString foreignKeyMatchColumn;

	/// When isForeignKey is true, this will be a hash from foreign key values to replacement values... used to cache and avoid repeated lookups
	QHash<QString, QVariant>* foreignKeyCache;

};

/// A model that can be used with QTableView to show (read-only) scan information from the database. The columns and information to show are adjustable, and foreign-key lookups can be done automatically (for ex: the name of a sample can be substituted for its id).
class AMQueryTableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	/// Construct a model exposing the given database \c db.  \c whereClause allows you to specify a filter on the results, that is suitable for appending after an SQL "WHERE" statement; you can leave this empty if you don't want to filter the results. \c orderClause allows you to specify the content of an SQL "ORDER BY" statement, for ex: "dateTime ASC" to specify the initial sort order.
	AMQueryTableModel(AMDatabase* db,
							  QObject *parent,
							  const QString& tableName,
							  const QString& idColumnName,
							  const QString& whereClause = QString(),
							  const QString& orderClause = QString(),
							  const QList<AMQueryTableModelColumnInfo>& columnsToShow = QList<AMQueryTableModelColumnInfo>());

	/// Returns a comma-separated list of the current column names that will be displayed
	QString columnNames() const;

	/// Re-implemented from QAbstractTableModel.  The rowCount() is zero until calling refreshQuery().
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const { Q_UNUSED(parent) return rowCount_; }

	/// Re-implemented from QAbstractTableModel. The columnCount() depends on the number of columns set in the constructor's \c columnsToShow.
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const { Q_UNUSED(parent) return columnCount_; }

	/// Re-implemented from QAbstractTableModel, here we resolve foreign keys if they've been configured within our columns_, otherwise return a cached or queried value for the row values.
	virtual QVariant data(const QModelIndex &item, int role) const;
	/// Re-implemented from QAbstractTableModel with our human-readable column headings
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	/// Item flags return ItemIsEnabled and ItemIsSelectable, as well as ItemIsDragEnabled to support drag-and-drop of scans out of the table.
	Qt::ItemFlags flags(const QModelIndex &index) const {
		Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

		if (index.isValid())
			return Qt::ItemIsDragEnabled | defaultFlags;
		else
			return defaultFlags;
	}

	QStringList mimeTypes() const {
		return QStringList() << "text/uri-list";
	}

	QMimeData* mimeData(const QModelIndexList &indexes) const;

	/// Re-implemented from QAbstractTableModel to sort the data. Note that for now, foreign-key columns will be sorted by foreign key, not by their replacement (visible) data
	virtual void sort(int column, Qt::SortOrder order);

public slots:
	/// Refreshes the query based on the current columns and database. Resets the model and sets the data provider to be the new query.  Note that we do not hit the database in the constructor; you must call refreshQuery() after creation to actually load data from the db.  Returns true if the query was executed successfully.
	bool refreshQuery();

signals:

public slots:

protected:
	QString tableName_, whereClause_, orderClause_, idColumnName_;
	QList<AMQueryTableModelColumnInfo> columns_;
	AMDatabase* db_;

	int rowCount_, columnCount_;

	mutable QVector<QVector<QVariant> > data_;

	/// Looks up a replacement value for a foreign key. Only called if role is either Qt::EditRole or Qt::DisplayRole, and item is a top-level index that is valid within our row and column range.
	QVariant resolveForeignKey(const QModelIndex& item) const;
};

#endif // AMSCANQUERYMODEL_H
