#ifndef AMCONTROLSETINFO_H
#define AMCONTROLSETINFO_H

#include "dataman/AMDbObject.h"
#include "dataman/AMDatabaseDefinition.h"
#include <QStringList>
#include <QAbstractTableModel>
#include <QDebug>

class AMControlSetInfoModel;
class AMControlSlimInfo;

class AMControlSetInfo : public AMDbObject
{
Q_OBJECT
public:
	explicit AMControlSetInfo(QObject *parent = 0);
	AMControlSetInfo(AMControlSetInfo *copyFrom, QObject *parent = 0);

	~AMControlSetInfo() {}

	AMControlSetInfoModel* model();

	bool checkSync();

	int count() const;

	QString nameAt(size_t index) const;

	double valueAt(size_t index) const;

	double minimumAt(size_t index) const;

	double maximumAt(size_t index) const;

	QString unitsAt(size_t index) const;

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

	/// Reimplemented from AMDbObject; provides a general human-readable description
	virtual QString typeDescription() const;

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

	void copyFrom(AMControlSetInfo *copyFrom);

protected slots:
	void onDataChanged(QModelIndex a,QModelIndex b);
	void onRowsInserted(QModelIndex parent, int start, int end);
	void onRowsRemoved(QModelIndex parent, int start, int end);

protected:
	virtual bool setupModel();

protected:
	AMControlSetInfoModel *ctrlInfoList_;

private:
	int insertRowLatch;
};

class AMControlSetInfoModel : public QAbstractTableModel
{
Q_OBJECT
public:
	AMControlSetInfoModel(QObject *parent = 0);

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
	QList<AMControlSlimInfo*> *ctrlInfoList_;
};

class AMControlSlimInfo : public QObject
{
Q_OBJECT
public:
	AMControlSlimInfo(QString name, double value, double minimum, double maximum, QString units, QObject *parent = 0);

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
