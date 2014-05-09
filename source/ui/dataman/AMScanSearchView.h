#ifndef AMSCANSEARCHVIEW_H
#define AMSCANSEARCHVIEW_H

#include <QWidget>
#include <QTableView>
#include <QAbstractTableModel>
#include <dataman/AMScan.h>
#include <dataman/AMSample.h>
#include <QList>
#include <QHBoxLayout>

/// A class representing a Dialog Window for searching scan's for:
///		Sample name
///		Sample date
///		Scan Exit Slit Value
///		Scan Grating Value
///		Scan Harmonic Value
class AMScanSearchView : public QWidget
{
	Q_OBJECT
public:
	explicit AMScanSearchView(QWidget *parent = 0);


protected:
	/// Initialises all the needed settings for the window (constructs Widgets, sets title etc.)
	void initDialog();
	/// Gets a list of all the required scans
	void refreshScanList();
signals:
	
public slots:
	
};

/// A class representing a scan list model
class AMScanListModel : public QAbstractTableModel{
	Q_OBJECT
public:
	explicit AMScanListModel(QObject* parent = 0);
	/// Returns the number of scans in the list to generate the number of rows in a table or list
	int rowCount(const QModelIndex & /*parent*/) const { return scans_->count(); }
	/// Returns "6" statically. There are always 6 fields in the ScanModel: id, name, number, dataTime, sampleName, sampleTime
	int columnCount(const QModelIndex & /*parent*/) const { return 6; }
	/// Retrieves the data from an index (row and column) and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
	QVariant data(const QModelIndex &index, int role) const;
	/// Retrieves the header data for a column or row and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	QList<AMScan*>* scans() { return scans_; }
protected:
	QList<AMScan*>* scans_;

signals:

public slots:
};

#endif // AMSCANSEARCHVIEW_H
