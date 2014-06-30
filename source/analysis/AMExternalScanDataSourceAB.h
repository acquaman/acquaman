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


#ifndef AMEXTERNALSCANDATASOURCEAB_H
#define AMEXTERNALSCANDATASOURCEAB_H

#include "analysis/AMStandardAnalysisBlock.h"

class AMScan;

/// This analysis block provides a way to access a data soure from another scan.  You can create one by specifying the database, id, and data source name of the source data in the constructor. The external scan will be loaded and the appropriate data source will be copied and exposed through this analysis block. Note that the data is not "live"; to pick up changes from the external scan, you must call refreshData().
/*! \note Limitation: This version only supports data sources up to 5 dimensions */
class AMExternalScanDataSourceAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_PROPERTY(QString sourceDatabaseConnectionName READ sourceDatabaseConnectionName WRITE dbLoadConnectionName)
	Q_PROPERTY(int sourceScanId READ sourceScanId WRITE dbLoadSourceScanId)
	Q_PROPERTY(QString sourceDataSourceName READ sourceDataSourceName WRITE dbLoadSourceDataSourceName)
	Q_PROPERTY(QString outputDataSourceName READ name WRITE dbLoadOutputDataSourceName)

public:

	/// This enum is used to specify when to load the external data.
	enum RefreshDataWhenSpec {
		InConstructor, ///< Load the external scan data immediately inside the constructor. This is used automatically when re-loading this analysis block out of the database.
		DeferAfterConstructor,  ///< Load the external scan data when control returns to the event loop after the constructor
		Manually ///< The external scan data will not be loaded until calling refreshData().
	};

	/// Construct an analysis block which exposes data from another scan.
	/*! \param sourceDatabase The database to load the external scan from
	  \param sourceScanId The id of the external scan, in \c sourceDatabase
	  \param sourceDataSourceName The name of of the datasource that you want to expose, in the scan
	  \param outputDataSourceName The name for our output data source (which can be different than the name of the input data source)
	  \param parent QObject parent for memory management

	  \param whenToLoadData Specifies that the external data should either be loaded: immediately (inside the constructor), or when control goes back to the event loop, or manually by calling refreshData()

	  \note You cannot change the source scan or data source after constructing this object, since the new source might have a different rank than the old source. (AMDataSource outputs are not allowed to change in rank.)
	  */
 	virtual ~AMExternalScanDataSourceAB();
	explicit AMExternalScanDataSourceAB(AMDatabase* sourceDatabase, int sourceScanId, const QString& sourceDataSourceName, const QString& outputDataSourceName, RefreshDataWhenSpec whenToLoadData = DeferAfterConstructor, QObject *parent = 0);

	/// Constructor which re-loads a previously-saved block from the database \c db at row \c id.  This is the version used when scans are opened and re-create their analysis blocks, so it loads the external scan data immediately.
	Q_INVOKABLE AMExternalScanDataSourceAB(AMDatabase* db, int id);

	/// Check if a set of inputs is valid. For this analysis block, we don't accept any input data sources; the only list we accept is the empty list.
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Set the data source inputs.  We don't do anything here, since our input is coming from the external scan instead.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>&) {}


	// Creating editors for editing parameters
	////////////////////////////////////
	/// \todo Do we need an editor widget? There's really nothing to edit here.
	virtual QWidget* createEditorWidget() { return 0; }

	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const;

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const;


	// Reading properties
	////////////////////////////

	/// The id of the external scan we're exposing
	int sourceScanId() const { return sourceScanId_; }
	/// The database where that exteral scan is found
	AMDatabase* sourceDatabase() const { return sourceDb_; }
	/// The name of the data source to expose from the external scan.  We use the name rather than the id, just in case the external scan's data sources get re-organized/re-numbered.
	QString sourceDataSourceName() const { return sourceDataSourceName_; }

	/// The "connection name" of the sourceDatabase().  We store this so we can re-connect to the same database during a different run of the program, where it won't have the same pointer address.
	QString sourceDatabaseConnectionName() const { return sourceDb_->connectionName(); }

	// Database re-loading properties:
	////////////////////////////

	/// This function should only ever be called automatically by the database loading system
	void dbLoadConnectionName(const QString& connectionName) { dbLoadConnectionName_ = connectionName; }
	/// This function should only ever be called automatically by the database loading system
	void dbLoadSourceScanId(int scanId) { dbLoadScanId_ = scanId; }
	/// This function should only ever be called automatically by the database loading system
	void dbLoadSourceDataSourceName(const QString& sourceDataSourceName) { dbLoadSourceDataSourceName_ = sourceDataSourceName; }
	/// This function should only ever be called automatically by the database loading system
	void dbLoadOutputDataSourceName(const QString& outputDataSourceName) { dbLoadOutputDataSourceName_ = outputDataSourceName; }


	/// This re-implemented version of loadFromDb() ensures we can only re-load ourselves from other external data sources with the same rank as we already have.
	bool loadFromDb(AMDatabase *db, int id);


signals:

public slots:
	/// Refresh the data. This will create, load, and copy the data from the external scan's data source, and then delete the external scan.  Returns true if data was successfully loaded; returns false if there was a problem.
	/*! \note Our output is not automatically updated if the data within the external scan changes.  You must call refreshData() again to pick up the changes. */
	bool refreshData();

protected:
	/// A flat data array of all the data values.  The final AMnDIndex varies the fastest.
	QVector<AMNumber> values_;
	/// A vector for each axis, to store the axis values.
	QList<QVector<AMNumber> > axisValues_;


	AMDatabase* sourceDb_;
	int sourceScanId_;
	QString sourceDataSourceName_;
	AMScan* scan_;

	/// Only used when re-loading out of the database [We can't store a database pointer; instead we store the connection name]
	QString dbLoadConnectionName_;
	int dbLoadScanId_;
	QString dbLoadSourceDataSourceName_;
	QString dbLoadOutputDataSourceName_;

	/// True while running inside the constructor
	bool insideConstructor_;

private:
	/// Helper function to copy the values from the source data source into values_.  Assumes that \c scan_ is set and \c dataSourceIndex is good.
	void copyValues(int dataSourceIndex);
	/// Helpver function to copy the axis values from the source data source into values_. Assumes that \c scan_ is set and \c dataSourceIndex is good.
	void copyAxisValues(int dataSourceIndex);

};

#endif // AMEXTERNALSCANDATASOURCEAB_H
