#ifndef AMSCANQUERYMODEL_H
#define AMSCANQUERYMODEL_H

#include <QSqlQueryModel>
#include "dataman/AMDatabase.h"


/// A helper class for AMScanQueryModel, this class encapsulates the lookup information for a column in that model.
class AMScanQueryModelColumnInfo {

public:
	/// Default constructor
	AMScanQueryModelColumnInfo(const QString& iHeading = "Row",
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

	/// Copy constructor.  \note If foreignKeyCache was not a pointer, we could skip these and use the default copy and assigment operators. However, a default-created QHash takes up memory, and we don't want to do that in the case where isForeignKey is false (ie: don't need the hash).
	AMScanQueryModelColumnInfo(const AMScanQueryModelColumnInfo& other)
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
	AMScanQueryModelColumnInfo& operator=(const AMScanQueryModelColumnInfo& other) {
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

	~AMScanQueryModelColumnInfo() {
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
class AMScanQueryModel : public QSqlQueryModel
{
	Q_OBJECT
public:
	/// Construct a model exposing the given database \c db.  If \c tableName is left empty, the default table is AMDbObjectSupport::tableForClass<AMScan>().  If \c columnsToShow is left empty, the default columns are: {name, number, dateTime, scanInfo, sampleId, notes, AMDbObjectType, facilityId}.  \c whereClause allows you to specify a filter on the results, that is suitable for appending after an SQL "WHERE" statement; you can leave this empty if you don't want to filter the results.
	explicit AMScanQueryModel(AMDatabase* db,
							  QObject *parent = 0,
							  const QString& tableName = QString(),
							  const QString& whereClause = QString(),
							  const QList<AMScanQueryModelColumnInfo> columnsToShow = QList<AMScanQueryModelColumnInfo>());

	/// Returns a comma-separated list of the current column names that will be displayed
	QString columnNames() const;

	/// Re-implemented from QSqlQueryModel, here we resolve foreign keys if they've been configured within our columns_.
	virtual QVariant data(const QModelIndex &item, int role) const;
	/// Re-implemented fomr QSqlQueryModel with our human-readable column headings
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	/// Re-implemented from QSqlQueryModel to sort the data. Note that for now, foreign-key columns will be sorted by foreign key, not by their replacement (visible) data
	virtual void sort(int column, Qt::SortOrder order);

public slots:
	/// Refreshes the query based on the current columns and database. Resets the model and sets the data provider to be the new query.  Note that we do not hit the database in the constructor; you must call refreshQuery() after creation to actually load data from the db.  Returns true if the query was executed successfully.
	bool refreshQuery();

signals:

public slots:

protected:
	QString tableName_, whereClause_, orderClause_;
	QList<AMScanQueryModelColumnInfo> columns_;
	AMDatabase* db_;


	/// Looks up a replacement value for a foreign key. Only called if role is either Qt::EditRole or Qt::DisplayRole, and item is a top-level index that is valid within our row and column range.
	QVariant resolveForeignKey(const QModelIndex& item) const;
};

#endif // AMSCANQUERYMODEL_H
