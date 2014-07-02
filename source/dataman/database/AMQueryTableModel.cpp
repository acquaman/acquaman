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


#include "AMQueryTableModel.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMDateTimeUtils.h"
#include "util/AMErrorMonitor.h"

#include <QStringBuilder>


AMQueryTableModel::AMQueryTableModel(AMDatabase* db,
								   QObject *parent,
								   const QString& tableName,
								   const QString& idColumnName,
								   const QString& whereClause,
								   const QString& orderClause,
								   const QList<AMQueryTableModelColumnInfo>& columnsToShow) :
	QAbstractTableModel(parent)
{

	rowCount_ = 0;
	columnCount_ = columnsToShow.count();
	db_ = db;
	whereClause_ = whereClause;
	orderClause_  = orderClause;
	idColumnName_ = idColumnName;

	tableName_ = tableName;
	columns_ = columnsToShow;
	for(int i=columns_.count()-1; i>=0; i--) {
		if(columns_.at(i).isForeignKey)
			columns_[i].foreignKeyCache->clear();
	}

	setSupportedDragActions(Qt::CopyAction);
}



QString AMQueryTableModel::columnNames() const
{
	QStringList rv;
	foreach(const AMQueryTableModelColumnInfo& col, columns_)
		rv << col.name;

	return rv.join(", ");
}

bool AMQueryTableModel::refreshQuery()
{
	for(int i=columns_.count()-1; i>=0; i--) {
		if(columns_.at(i).isForeignKey) {
			columns_[i].foreignKeyCache->clear();
		}
	}

	// check for empty column set? Bad database?

	// set the query
	QString whereString = whereClause_.isEmpty() ? QString() : " WHERE " % whereClause_;
	QString orderByString = orderClause_.isEmpty() ? QString() : " ORDER BY " % orderClause_;
	QString queryString = "SELECT " % idColumnName_ % " FROM " % tableName_ % whereString % orderByString % ";";

	QSqlQuery q = db_->query();
	q.prepare(queryString);
	if(q.exec()) {
		emit layoutAboutToBeChanged();
		data_.clear();
		rowCount_ = 0;
		int numRows;
		if((numRows = q.size()) == -1) {
			// sql driver doesn't support sizes. Need to proceed less efficiently
			while(q.next()) {
				QVector<QVariant> row = QVector<QVariant>(columnCount_+1, QVariant(QChar(QChar::Null)));
				row[0] = q.value(0);	// the row id.
				data_ << row;
				rowCount_++;
			}
		}
		else {
			// sql driver knows the number of rows. Efficient to allocate all at once.
			data_ = QVector<QVector<QVariant> >(numRows,
												QVector<QVariant>(columnCount_+1, QVariant(QChar(QChar::Null))));
			for(int i=0; i<numRows; i++) {
				q.next();
				data_[i][0] = q.value(0); // the row id
			}
			rowCount_ = numRows;
		}
		// We should have all the rows now.
		emit layoutChanged();
		return true;
	}
	else {
		q.finish();
		AMErrorMon::report(AMErrorReport(this,
										 AMErrorReport::Debug,
										 -1,
										 "Could not load the results out of the database for this query: '" % queryString % "'.  The error was: " % q.lastError().text()));
		return false;
	}
}

QVariant AMQueryTableModel::data(const QModelIndex &item, int role) const
{
	if(!item.isValid()) {
		return QVariant();
	}
	if(item.column() >= columnCount_) {
		return QVariant();
	}
	if(item.row() >= rowCount_) {
		return QVariant();
	}

	// We only return Qt::EditRole or Qt::DisplayRole
	if(!(role == Qt::EditRole || role == Qt::DisplayRole))
		return QVariant();

	QVariant rv = data_.at(item.row()).at(item.column()+1);
	if(rv.type() == QVariant::Char && rv.toChar() == QChar(QChar::Null)) {
		// a Qchar-type QVariant with a value of QChar::Null we use to indicate that we haven't cached the value yet. Let's grab the whole row at once.
		QSqlQuery q = db_->query();
		q.prepare("SELECT " % columnNames() % " FROM " % tableName_ % " WHERE " % idColumnName_ % " = ?");
		q.bindValue(0, data_.at(item.row()).at(0).toInt());
		if(q.exec() && q.first()) {
			// cache the row values
			for(int i=0, cc=columnCount_; i<cc; i++)
				data_[item.row()][i+1] = q.value(i);	// remember that the index is in the first row.
		}
		q.finish();
		// todo: what if we failed to find it? Should we repeat database hit every time?
		rv = data_.at(item.row()).at(item.column()+1);
	}


	// It's valid and been pulled. Just return it.

	if(columns_.at(item.column()).isForeignKey)
		rv = resolveForeignKey(item);

	// format if it's a dateTime column: (For now we guess because the name of the column is "dateTime". Works for us, but not for everyone.
	if(columns_.at(item.column()).name == "dateTime") {
		QDateTime dt = rv.toDateTime();
		if(dt.isValid())
			return AMDateTimeUtils::prettyDateTime(dt);
	}
	return rv;
}

QVariant AMQueryTableModel::resolveForeignKey(const QModelIndex &item) const {

	// optimization: check if item.data() is an integer index, and is less than 1.  If so, we know it's an invalid foreign key (at least, that's our convention) and we can save time by not even doing a database search
	QVariant foreignKey = data_.at(item.row()).at(item.column()+1);
	QString foreignKeyString = foreignKey.toString();

	bool conversionOk;
	int foreignKeyId = foreignKey.toInt(&conversionOk);
	if(conversionOk && foreignKeyId < 1)
		return QVariant();

	const AMQueryTableModelColumnInfo& col = columns_.at(item.column());

	// Do we have the value cached already?
	QHash<QString, QVariant>::const_iterator iForeignKeyCache = col.foreignKeyCache->find(foreignKeyString);
	if(iForeignKeyCache != col.foreignKeyCache->end())
		return iForeignKeyCache.value();

	QSqlQuery q = db_->query();
	q.prepare("SELECT " % col.foreignKeyReplacementColumn % ", " % col.foreignKeyMatchColumn % " FROM " % col.foreignKeyTable % " WHERE " % col.foreignKeyMatchColumn % " = ?;");
	q.bindValue(0, foreignKey);
	QVariant value;

	if(q.exec() && q.first()) {
		value = q.value(0);
	}

	(*(col.foreignKeyCache))[foreignKeyString] = value;
	return value;
}

QVariant AMQueryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Horizontal && section < columns_.count() && role == Qt::DisplayRole)
		return columns_.at(section).heading;

	if(orientation == Qt::Vertical && section < rowCount_ && role == Qt::DisplayRole)
		return section;

	return QVariant();
}

void AMQueryTableModel::sort(int column, Qt::SortOrder order)
{
	QString orderClause;

	if(column < columns_.count()) {
		orderClause = columns_.at(column).name % ((order == Qt::AscendingOrder) ? " ASC" : " DESC");
	}


	if(orderClause_ != orderClause) {
		orderClause_ = orderClause;
		refreshQuery();
	}
}

#include <QMimeData>
#include <QUrl>

QMimeData * AMQueryTableModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData* mimeData = new QMimeData();
	QList<QUrl> urls;

	QSet<int> rowsCompleted;	// indexes will be all the selected indexes...
	foreach (const QModelIndex &i, indexes) {
		if (i.isValid() && !rowsCompleted.contains(i.row())) {
			rowsCompleted << i.row();

			bool ok;
			int id = data(index(i.row(),0), Qt::DisplayRole).toInt(&ok);

			if(ok)
				urls << QUrl("amd://" % db_->connectionName() % "/" % tableName_ % "/" % QString::number(id));
		}
	}

	mimeData->setUrls(urls);
	return mimeData;
}


