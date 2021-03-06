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


#ifndef AMSCANSEARCHVIEW_H
#define AMSCANSEARCHVIEW_H

#include <QWidget>
#include <QTableView>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <acquaman/SGM/SGMXASScanConfiguration.h>
#include <dataman/AMSample.h>
#include <dataman/database/AMDatabase.h>
#include <QList>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QUrl>

/// A class representing a Dialog Window for searching scan's for:
/// Sample name
/// Sample date
/// Scan Exit Slit Value
/// Scan Grating Value
/// Scan Harmonic Value
class AMScanSearchView : public QWidget
{
Q_OBJECT
public:
explicit AMScanSearchView(QWidget *parent = 0);


protected:
/// Initialises all the needed settings for the window (constructs Widgets, sets title etc.)
void initDialog();
QLineEdit* searchCriteria_;
QComboBox* searchFields_;
QSortFilterProxyModel* proxyModel_;
QTableView* searchResults_;
signals:
void editScanRequested(QList<QUrl> scanUrls);
void editConfigurationRequested(QList<QUrl> scanUrls);
void exportScanRequested(QList<QUrl> scanUrls);


public slots:

protected slots:
void onContextMenuRequested(QPoint pos);
void onTableDoubleClicked(QModelIndex);
void onSearchFieldChanged();
void onSearchCriteriaChanged();

};

/// A lightweight class representing scan information loaded from the db to display in the search results
class AMScanSearchInfo : public QObject{
Q_OBJECT
public:
explicit AMScanSearchInfo(int scanID, QObject* parent = 0);
int scanID();
int scanNumber();
QDateTime scanDateTime();
QString scanName();
SGMXASScanConfiguration* configuration();
AMSample* sample();

protected:
int scanID_;
int scanNumber_;
QDateTime scanDateTime_;
QString scanName_;
SGMXASScanConfiguration* configuration_;
AMSample* sample_;
int configID_;
void initializeConfig(int configID);
int sampleID_;
void initializeSample(int sampleID);

signals:
public slots:
};

/// A class representing a scan list model
class AMScanSearchInfoListModel : public QAbstractTableModel{
Q_OBJECT
public:
/// Initializes an instance of AMScanSearchInfoListModel by loading a list of ids from the user database
explicit AMScanSearchInfoListModel(QObject* parent = 0);
~AMScanSearchInfoListModel();
/// Returns the number of scans in the list to generate the number of rows in a table or list
int rowCount(const QModelIndex & /*parent*/) const { return scanIds_.count(); }
/// Returns "9" statically. There are always 9 fields in the ScanModel: id, name, number, dataTime, sampleName, sampleTime, configExitSlit, configGrating and configHarmonic
int columnCount(const QModelIndex & /*parent*/) const { return 9; }
/// Retrieves the data from an index (row and column) and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
QVariant data(const QModelIndex &index, int role) const;
/// Retrieves the header data for a column or row and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
QVariant headerData(int section, Qt::Orientation orientation, int role) const;
/// Retrieves the Id of the scan info at the given index
int getID(QModelIndex& index);

protected:
AMScanSearchInfo** scanCache_;
QVariantList scanIds_;

AMScanSearchInfo* scanInfoAt(int index, int id) const;
signals:

public slots:
};

#endif // AMSCANSEARCHVIEW_H
