/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AM_SCAN_H
#define AM_SCAN_H


#include <QObject>
#include <QMap>
#include <QMutex>

#include "dataman/database/AMDbObject.h"
#include "dataman/datastore/AMDataStore.h"
#include "util/AMOrderedSet.h"
#include "dataman/datasource/AMRawDataSource.h"
#include "dataman/AMAnalysisBlock.h"
#include "dataman/info/AMControlInfoList.h"

#include "dataman/AMFileLoaderInterface.h"

typedef AMOrderedSet<QString, AMRawDataSource*> AMRawDataSourceSet;
typedef AMOrderedSet<QString, AMAnalysisBlock*> AMAnalyzedDataSourceSet;

class AMScanConfiguration;
class AMScanDictionary;

class AMSamplePre2013;
class AMSample;
class AMConstDbObject;

#ifndef ACQUAMAN_NO_ACQUISITION
class AMScanController;
#endif

#define AMSCAN_CANNOT_FIND_SUITABLE_PLUGIN_FOR_FILE_FORMAT -2147
#define AMSCAN_DEBUG_DELETING_SCAN -2148
#define AMSCAN_SCAN_DELETE_DIRECTLY -2149
#define AMSCAN_ANALYZED_DATASOURCE_COUNT_MISMATCH -2150
#define AMSCAN_THUMBNAIL_SCANNING_MESSAGE -2151

/// This class is the base of all objects that represent a single 'scan' on a beamline.
/*! AMScan provides access to the following information:

  - Basic meta-data: the start and end time, the run (including facility where it was taken), arbitrary notes,
  - The file format of, and paths to, the raw data: fileFormat(), filePath(), additionalFilePaths()
  - A container for the raw data sources and analyzed data sources \see AMDataSource, AMRawDataSource, AMAnalysisBlock
  - The scan configuration, which is a customized class based on the type of the scan: scanConfiguration()
  - A list of control values, which can store conditions at the time the scan was taken: scanInitialConditions()

  You can load raw data into memory using loadData(), which will attempt to find an available file loader plugin that works with the scan's fileFormat()

  You can use currentlyScanning() to find out if a scan is currently still acquiring data (ie: it has an associated scan controller).

  <b>Memory Management</b>

  Because multiple objects may be interested in interacting with an AMScan instance, this class uses a retain/release memory management model.  This is different from the conventional Qt memory management system, where objects are owned by a single parent object.  Instead, any QObject that is interested in working with an AMScan instance should call retain() to guarantee that it will remain available in memory.  Once you're finished with it, instead of deleting it directly, you should call release().  When all interested parties have release()'d a scan, it will delete itself.

  \note This means that you should never \c delete an AMScan explicitly.  Instead, always call release().

  For example, a common situation is for a scan controller to want to put data into an AMScan, while an AMScanView visualizes that same instance.  Initially, they both call retain() on the scan to declare this interest.  When the scan controller finishes, it calls release(), but nothing happens because the scan is still "called for" by the AMScanView.  When the scan is removed from the AMScanView, AMScanView also calls release().  With no one interested in the scan anymore, it deletes itself.

*/
class AMScan : public AMDbObject {

	Q_OBJECT

	/// Database Persistent Properties
	Q_PROPERTY(QString unEvaluatedName READ unEvaluatedName WRITE setUnEvaluatedName)
	Q_PROPERTY(int number READ number WRITE setNumber NOTIFY numberChanged)
	Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime NOTIFY dateTimeChanged)
	Q_PROPERTY(QDateTime endDateTime READ endDateTime WRITE setEndDateTime NOTIFY endDateTimeChanged)
	Q_PROPERTY(int runId READ runId WRITE setRunId)
	Q_PROPERTY(AMConstDbObject* samplePre2013 READ dbReadSamplePre2013 WRITE dbWriteSamplePre2013)
	Q_PROPERTY(AMConstDbObject* sample READ dbReadSample WRITE dbWriteSample)
	Q_PROPERTY(QString notes READ notes WRITE setNotes)
	Q_PROPERTY(QString fileFormat READ fileFormat WRITE setFileFormat)
	Q_PROPERTY(QString filePath READ filePath WRITE setFilePath)
	Q_PROPERTY(QStringList additionalFilePaths READ additionalFilePaths WRITE setAdditionalFilePaths)
	Q_PROPERTY(AMDbObject* scanInitialConditions READ scanInitialConditions WRITE dbLoadScanInitialConditions)
	Q_PROPERTY(AMDbObjectList rawDataSources READ dbReadRawDataSources WRITE dbLoadRawDataSources)
	Q_PROPERTY(AMDbObjectList analyzedDataSources READ dbReadAnalyzedDataSources WRITE dbLoadAnalyzedDataSources)
	Q_PROPERTY(QString analyzedDataSourcesConnections READ dbReadAnalyzedDataSourcesConnections WRITE dbLoadAnalyzedDataSourcesConnections)
	Q_PROPERTY(AMDbObject* scanConfiguration READ dbGetScanConfiguration WRITE dbLoadScanConfiguration)
	Q_PROPERTY(bool currentlyScanning READ currentlyScanning WRITE dbLoadCurrentlyScanning NOTIFY currentlyScanningChanged)
	Q_PROPERTY(QString indexType READ indexType WRITE setIndexType)

	Q_CLASSINFO("dateTime", "createIndex=true")
	//Q_CLASSINFO("sampleId", "createIndex=true")
	Q_CLASSINFO("runId", "createIndex=true")

	Q_CLASSINFO("rawDataSources", "hidden=true")
	Q_CLASSINFO("analyzedDataSources", "hidden=true")
	Q_CLASSINFO("scanInitialConditions", "hidden=true")
	Q_CLASSINFO("analyzedDataSourcesConnections", "hidden=true")
	Q_CLASSINFO("scanConfiguration", "hidden=true")
	Q_CLASSINFO("unEvaluatedName", "upgradeDefault=<none>")
	Q_CLASSINFO("indexType", "upgradeDefault=<none>")

	Q_CLASSINFO("sample", "constStore=true")

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Scan")

public:

	// Constructors and Destructors
	//////////////////////////////////

	/// default constructor
	Q_INVOKABLE explicit AMScan(QObject *parent = 0);

	/// Static convenience function to create an AMScan from a database URL in the Acquaman URL format ("amd://databaseConnection/tableName/id"). Returns 0 if the scan could not be found or the URL is invalid.
	/*! It is generally not advisable to open a second instance of scan from the DB if there is one currently scanning because of the potential for overwriting/saving (and because raw data likely won't be available.)  If \c allowIfScanning is false, will return 0 if the database indicates the scan is still scanning ('currentlyScanning' column).

	Optional outputs:

		- If \c isScanning is provided, will be set with whether the scan is still scanning.
		- If \c scanName is provided, will be set the full name of the scan (assuming it is found).
*/
	static AMScan* createFromDatabaseUrl(const QUrl& url, bool allowIfScanning = false, bool* isScanning = 0, QString* scanName = 0);


	/// Destructor: deletes all channels.
	virtual ~AMScan();

	/// \todo copy constructor and assignment operator required, but not yet implemented. Do not copy. (implementation note: handle channels as children?)

	// Role 1: Meta Data Elements
	////////////////////////////////

	/// Returns a user-given number
	int number() const { return number_;}
	/// Returns creation time / scan start time
	QDateTime dateTime() const {return dateTime_;}
	/// Returns scan end time.
	QDateTime endDateTime() const { return endDateTime_; }

	/// Returns the elapsed time of the scan in seconds if a valid scan end time exists.  Returns -1 otherwise.
	double elapsedTime() const;
	/// Returns the id of the run containing this scan, or (-1) if not associated with a run.
	int runId() const { return runId_; }
	/// Returns id of the scan's sample (or -1 if a sample has not been assigned)
	int sampleId() const;
	const AMSamplePre2013* samplePre2013() const;
	const AMSample* sample() const;


	/// Returns notes/comments for scan
	QString notes() const { return notes_; }

	/// The string describing the format of the stored raw data file
	QString fileFormat() const { return fileFormat_; }
	/// The directory path and file name of this scan's raw data file
	QString filePath() const { return filePath_; }
	/// Any additional files of raw data that need to be referenced
	QStringList additionalFilePaths() const { return additionalFilePaths_; }

	/// Returns the indexation type that this scan will follow.
	QString indexType() const { return indexType_; }

	// Convenience functions on meta-data:
	/////////////////////////
	QString unEvaluatedName() const;
	/// Returns the full scan name: number appended to name
	QString fullName() const {return QString("%1 #%2").arg(name()).arg(number()); }
	/// Returns the name of the sample (if a sample is set, otherwise returns "[no sample]")
	QString sampleName() const;


	// Role 2: Database system
	///////////////////////////////////////////
	/// Loads a saved scan from the database into self. Returns true on success.
	/*! Re-implemented from AMDbObject::loadFromDb(), this version also loads the scan's raw data if autoLoadData() is set to true, and the stored filePath doesn't match the existing filePath()*/
	virtual bool loadFromDb(AMDatabase* db, int id);


	// Role 3: Data Sources (Raw and Analyzed)
	////////////////////////////////

	/// Returns read-only access to the set of raw data sources. (A data source represents a single "stream" or "channel" of data. For example, in a simple absorption scan, the electron yield measurements are one raw data source, and the fluorescence yield measurements are another data source.)
	const AMRawDataSourceSet* rawDataSources() const { return &rawDataSources_; }
	int rawDataSourceCount() const { return rawDataSources_.count(); }
	// AMRawDataSourceSet* rawDataSources() { return &rawDataSources_; }
	/// Publicly expose part of the rawData(), by adding a new AMRawDataSource to the scan. The new data source \c newRawDataSource should be valid, initialized and connected to the data store already.  The scan takes ownership of \c newRawDataSource.  This function returns false if raw data source already exists with the same name as the \c newRawDataSource.
	bool addRawDataSource(AMRawDataSource* newRawDataSource);
	/// This overloaded function calls addRawDataSource() after setting the visibleInPlots() and hiddenFromUsers() hints of the data source.
	bool addRawDataSource(AMRawDataSource* newRawDataSource, bool visibleInPlots, bool hiddenFromUsers);
	/// Delete and remove an existing raw data source.  \c id is the idnex of the source in rawDataSources().
	bool deleteRawDataSource(int id) { if((unsigned)id >= (unsigned)rawDataSources_.count()) return false; delete rawDataSources_.takeAt(id); return true; }

	/// Returns read-only access to the set of analyzed data sources. (Analyzed data sources are built on either raw data sources, or other analyzed sources, by applying an AMAnalysisBlock.)
	const AMAnalyzedDataSourceSet* analyzedDataSources() const { return &analyzedDataSources_; }
	int analyzedDataSourceCount() const { return analyzedDataSources_.count(); }
	// AMAnalyzedDataSourceSet* analyzedDataSources() { return &analyzedDataSources_; }
	/// Add an new analysis block to the scan.  The scan takes ownership of the \c newAnalysisBlock and exposes it as one of the analyzed data sources.
	bool addAnalyzedDataSource(AMAnalysisBlock* newAnalyzedDataSource);
	/// This overloaded function calls addAnalyzedDataSource() after setting the visibleInPlots() and hiddenFromUsers() hints of the data source.
	bool addAnalyzedDataSource(AMAnalysisBlock *newAnalyzedDataSource, bool visibleInPlots, bool hiddenFromUsers);
	/// Delete and remove an existing analysis block. \c id is the index of the source in analyzedDataSources().
	bool deleteAnalyzedDataSource(int id) { if((unsigned)id >= (unsigned)analyzedDataSources_.count()) return false; delete analyzedDataSources_.takeAt(id); return true; }

	// Provides a simple access model to all the data sources (combination of rawDataSources() and analyzedDataSources()

	/// The total number of data sources (raw and analyzed)
	int dataSourceCount() const { return rawDataSources_.count() + analyzedDataSources_.count(); }
	/// Returns a data source by index.  (\c index must be from 0 to < dataSourceCount(), otherwise returns 0. )  Raw sources are listed first, from 0 to rawDataSources().count()-1. Next come analyzed sources, from rawDataSources().count() to dataSourceCount()-1.
	/*! This function is useful when considering the combined set of all the data sources. If you want to retrieve an analyzed source by its own index, just use analyzedDataSources()->at().*/
	AMDataSource* dataSourceAt(int index) const {
		if(index<0)
			return 0;
		int rawCount = rawDataSources_.count();
		if(index<rawCount)
			return rawDataSources_.at(index);
		if(index-rawCount < analyzedDataSources_.count())
			return analyzedDataSources_.at(index-rawCount);
		return 0;
	}
	/// Returns the index of a data source (in the combined set of raw+analyzed sources) identified by \c sourceName, or -1 if not found. You can then use the return value in dataSourceAt().
	int indexOfDataSource(const QString& sourceName) const {
		int rawSourceIndex = rawDataSources_.indexOfKey(sourceName);
		if(rawSourceIndex >= 0)
			return rawSourceIndex;
		int analyzedSourceIndex = analyzedDataSources_.indexOfKey(sourceName);
		if(analyzedSourceIndex >= 0)
			return analyzedSourceIndex + rawDataSources_.count();
		return -1;
	}

	/// Returns the index of a data source (in the combined set of raw+analyzed sources) identified by pointer \c dataSource, or -1 if not found.
	/*! Performance note: This involves a linear seach through all sources. Unless you have thousands of sources, it's fast enough, but don't call it repeatedly if you don't have to.*/
	int indexOfDataSource(const AMDataSource* source) const {
		int rawCount = rawDataSources_.count();
		for(int i=0; i<rawCount; i++)
			if(source == rawDataSources_.at(i))
				return i;
		for(int i=0; i<analyzedDataSources_.count(); i++)
			if(source == analyzedDataSources_.at(i))
				return i+rawCount;	// is an index in the combined set; raw sources come first.
		return -1;
	}


	/// Removes and deletes a data source.  \c index is the index of the data source in dataSourceAt().  Returns true on success, false if \c index < 0 or >= dataSourceCount().
	bool deleteDataSourceAt(int index ) {
		if(index < 0)
			return false;
		int rawCount = rawDataSources_.count();
		if(index < rawCount)	{ // is a raw data source.
			delete rawDataSources_.takeAt(index);
			return true;
		}
		index -= rawCount;
		if(index < analyzedDataSources_.count()) {
			delete analyzedDataSources_.takeAt(index);
			return true;
		}
		return false;
	}

	// Data sources have an option to be hiddenFromUsers(), for internal-only data that shouldn't be exposed. As one example, default thumbnails are only generated for non-hiddenFromUsers() data sources.  These are convenience helper functions to return the indexes of just the non-hiddenFromUsers data sources.
	//////////////

	/// Convenience function to count the number of data sources that do NOT have the AMDataSource::hiddenFromUsers() attribute set.
	int nonHiddenDataSourceCount() const;
	/// Convenience function to count the number of raw data sources that do NOT have the AMDataSource::hiddenFromUsers() attribute set.
	int nonHiddenRawDataSourceCount() const;
	/// Convenience function to count the number of analyzed data sources that do NOT have the AMDataSource::hiddenFromUsers() attribute set.
	int nonHiddenAnalyzedDataSourceCount() const;

	/// Convenience function to provide a list of the data sources that do NOT have the AMDataSource::hiddenFromUsers() attribute set. The returned indexes can be used in dataSourceAt().
	QVector<int> nonHiddenDataSourceIndexes() const;
	/// Convenience function to provide a list of the raw data sources that do NOT have the AMDataSource::hiddenFromUsers() attribute set. The returned indexes can be used in rawDataSourceAt().
	QVector<int> nonHiddenRawDataSourceIndexes() const;
	/// Convenience function to provide a list of the analyzed data sources that do NOT have the AMDataSource::hiddenFromUsers() attribute set. The returned indexes can be used in analyzedDataSourceAt().
	QVector<int> nonHiddenAnalyzedDataSourceIndexes() const;


	// Role 4: Loading/Clearing Raw Data
	////////////////////////////
	/// Load raw data into memory from storage, using the AMFileLoaderInterface plugin system to find the appropriate file loader based on fileFormat(). Returns true on success.
	bool loadData();

	/// Replace the scan's raw data store (rawData()) with a different instance/implementation of AMDataStore.  Returns false and does nothing if the new \c dataStore is incompatible with any existing raw data sources.  The scan takes ownership of the new \c dataStore and will delete it when deleted.
	/*! Some types of file loader plugins and scan controllers may recognize that it would be more efficient for a certain scan to use one of the disk-based raw data stores, rather than the default AMInMemoryDataStore implementation. (This is critically important for scans with huge data sets.)  They can call this function to replace the existing rawData() instance of AMDataStore with another instance.

	  For scan controllers, it is recommended to do this before creating any raw data sources.

	  File loaders must remember that there will already be AMRawDataSources created when the scan was loaded from the database. Therefore...

When the scan already has raw data sources present, the replacement \c dataStore must be compatible with all the existing raw data sources:

- It must have the same scan rank
- Each raw data source's \c measurementId must be valid within it, and
- The measurement rank for each raw data source must match.

[The easiest way to ensure this is to fill the new \c dataStore appropriately, and then call replaceDataStore().]

Returns false and does nothing if the new \c dataStore is incompatible with any existing raw data sources.
	  */
	bool replaceRawDataStore(AMDataStore* dataStore);

	/// Static function: Controls whether raw data is loaded automatically inside loadFromDb(), FOR ALL AMScan INSTANCES.  This function is thread-safe and can be set on a per-thread basis.
	/*! If autoLoadData() is true, then whenever loadFromDb() is called and the new filePath() is different than the old filePath(), loadData() will be called as well.  If you want to turn off loading raw data for performance reasons, call setAutoLoadData(false).  Auto-loading is enabled by default.*/
	static bool autoLoadData();

	/// Enables or disables automatically loading raw data inside loadFromDb().
	static void setAutoLoadData(bool autoLoadDataOn);


	/// Clears the scan's rawData() completely, including all measurements configured within the data store. Also deletes all rawDataSources() that expose this data.
	void clearRawDataPointsAndMeasurementsAndDataSources() {
		while(rawDataSources_.count())
			delete rawDataSources_.takeAt(rawDataSources_.count()-1);

		data_->clearAllMeasurements();
	}

	/// Clears the scan's rawData(), including all measurements configured within the data store. Leaves the configured scan axes as-is.
	/*! Caution: Leaves the rawDataSources() as-is; make sure that they don't attempt to access non-existent raw data.*/
	void clearRawDataPointsAndMeasurements() {
		data_->clearAllMeasurements();
	}

	/// Clears the scan's rawData() completely, including all configured measurements and scan axes in the data store. If a scan instance has held data previously, it is recommended that file loaders and scan controllers call this to start with a "clean slate".
	void clearRawDataCompletely() {
		data_->clearAll();
	}

	/// Clears the scans's raw data, but leaves all measurements, scan axes, and raw data sources as-is.
	void clearRawDataPoints() {
		data_->clearScanDataPoints();
	}


	// Role 5: DataStore (Raw Data) Interface
	//////////////////////////////////

	/// This should only be exposed to certain objects (such as scan controllers), which are allowed to modify the raw data store.
	AMDataStore* rawData() { return data_; }
	/// Provides read-only access to the raw data store
	const AMDataStore* rawData() const { return data_; }

	// should anything be exposed directly, from the data store? For ex:
	/// Returns the number of dimensions in the scan. (This does not include the dimensions of any multi-dimensional detectors; it's only the dimensions that were 'scanned over'.  For example, an XAS scan over energy has scanRank() of 1. A 2D micro-map scan at fixed energy has scanRank() 2.  A 2D micro-map with an absorption scan at each point has a scanRank() of 3.  For each scan point, you might have a set of detector measurements, each with their own dimensionality, that are not included here.)
	virtual int scanRank() const { return data_->scanRank(); }
	/// Returns the size of the scan along each dimension
	virtual AMnDIndex scanSize() const { return data_->scanSize(); }
	/// Returns the size of the scan along a specific scan axis \c axisId
	virtual int scanSize(int axisId) const { return data_->scanSize(axisId); }



	// Role 6: Beamline conditions
	//////////////////////////////
	/// Independent from the hardware you're connected to right now, an AMControlSetInfo can remember values and descriptions of how some hardware was set at the time of the scan.
	const AMControlInfoList* scanInitialConditions() const { return &scanInitialConditions_; }
	AMControlInfoList* scanInitialConditions() { return &scanInitialConditions_; }

	// Role 7: Access to Scan Configuration and Scan Controller
	///////////////////////////////

	///  Access the scan's configuration
	AMScanConfiguration* scanConfiguration() { return configuration_; }
	///  Read-only access the scan's configuration
	const AMScanConfiguration* scanConfiguration() const { return configuration_; }
	/// Set the scan configuration. Deletes the existing scanConfiguration() if there is one.  The scan takes ownership of the \c newConfiguration and will delete it when being deleted.
	void setScanConfiguration(AMScanConfiguration* newConfiguration);



	// Role 8: Thumbnail system:
	////////////////////////////////

	/// This is an arbitrary decision, but let's define it like this (for usability): If we have any analyzed data sources, we have a thumbnail for each analyzed data source. Otherwise, rather than showing nothing, we have a thumbnail for each raw data source.  Unless we are currently scanning, in which case we just have one (which visually indicates this).  In all cases, we exclude data sources that have the AMDataSource::hiddenFromUsers() attribute set.
	int thumbnailCount() const;

	/// Return a thumbnail picture of the data sources. If we have any analyzed data sources, we have a thumbnail for each analyzed data source. Otherwise, rather than showing nothing, we have a thumbnail for each raw data source.  Unless we are currently scanning, in which case we just have one (which visually indicates this). In all cases, we exclude data sources that have the AMDataSource::hiddenFromUsers() attribute set.
	AMDbThumbnail thumbnail(int index) const;

	/// Generating these thumbnails is time-consuming, because we have to draw a bunch of plots and render them to PNGs. Therefore, we should do it in a seperate thread.
	virtual bool shouldGenerateThumbnailsInSeparateThread() const { return false; }


	// Role 9: Acquisition status, and link to scan controller
	///////////////////////////////

#ifndef ACQUAMAN_NO_ACQUISITION
	/// If this scan is currently in progress, returns the scan controller that is running it. Otherwise returns 0.
	AMScanController* scanController() const { return controller_; }
	/// This function should not be considered part of the public interface, and only be used by AMScanController.
	void setScanController(AMScanController*);
#endif
	/// Returns true if currently scanning (ie: there is a valid scan controller, or the currentlyScanning column was true when we were loaded out of the database). This is useful because we want to know this at the database level even while scans are in progress.
	bool currentlyScanning() const { return currentlyScanning_; }


	// Role 10: Memory Management:
	/////////////////////////////////

	/// Call this to assert that \c owner is interested in using this scan instance, and wants it to stay in memory.
	/*! \note It is harmless to call this multiple times with the same \c owner; only one interest will be registered. */
	void retain(QObject* owner);
	/// Call this to release an interest in this scan instance.  If no objects are interested in it anymore (ie: no one else has retain()'d it), it will be deleted instantly.
	/*! You can call this with \c pastOwner = 0 to check and delete the scan if no other objects have registered an interest. You should always do this instead of calling \c delete. */
	void release(QObject* pastOwner = 0);



	// Miscellaneous
	/////////////////////////

	/// Returns the largest AMScan::number() of all the scans in the scan table, subject to the provided WHERE clause (ex: "sampleId = 3").
	static int largestNumberInScansWhere(AMDatabase* db, const QString& whereClause);

public slots:

	// Role 1: Setting Meta-Data
	///////////////////////////////

	void setUnEvaluatedName(QString unEvaluatedName);
	/// Sets appended number
	void setNumber(int number);
	/// set the date/time:
	void setDateTime(const QDateTime& dt);
	/// Set the scan end time.
	void setEndDateTime(const QDateTime &endTime);
	/// associate this object with a particular run. Set to (-1) to dissociate with any run.  (Note: for now, it's the caller's responsibility to make sure the runId is valid.)
	void setRunId(int newRunId);
	/// Sets the sample associated with this scan.
	void setSampleId(int newSampleId, const QString &databaseTableName = "");
	void setSamplePre2013(const AMSamplePre2013 *samplePre2013);
	void setSample(const AMSample *sample);
	/// Sets the indexation type.
	void setIndexType(const QString &newType) { indexType_ = newType; setModified(true); }

	/// Sets notes for scan
	void setNotes(const QString &notes) { notes_ = notes; setModified(true); }
	/// Set file path. (Be careful if changing this, not to break the association to a raw data file)
	void setFilePath(const QString& newPath) { filePath_ = newPath;  setModified(true); }
	/// Set the file format. This is a string matching the AMAbstractFileLoader::formatTag() in one of the available file loaders.
	void setFileFormat(const QString& format) { fileFormat_ = format;  setModified(true); }
	/// Any additional files of raw data that need to be referenced
	void setAdditionalFilePaths(const QStringList& additionalFilePaths) { additionalFilePaths_ = additionalFilePaths; setModified(true); }

signals:


	// Meta-data changed signals:
	/////////////////
	void dateTimeChanged(const QDateTime& newDateTime);
	void endDateTimeChanged(const QDateTime &);
	void sampleIdChanged(int sampleId);
	void numberChanged(int number);
	void currentlyScanningChanged(bool currentlyScanning);
	void scanConfigurationChanged();


	// Combined Data Source Model: Signals
	////////////////////////////////////////

	/// Emitted just before a new data source is added. \c index is the index where it will end up.  It's not there yet.
	void dataSourceAboutToBeAdded(int index);
	/// Emitted when a new data source is added.  \c index is the index of the source in dataSourceAt().
	void dataSourceAdded(int index);
	/// Emitted just before a data source is removed. \c index in the index of the source in dataSourceAt().
	void dataSourceAboutToBeRemoved(int index);
	/// Emitted after a data source was removed. \c index is the index the source used to occupy in dataSourceAt(); it's not there anymore.
	void dataSourceRemoved(int index);



protected slots:


	// Combined Data Source Model: insert/remove handling
	////////////////////////////////////////

	/// Receives itemAboutToBeAdded() signals from rawDataSources_ and analyzedDataSources, and emits dataSourceAboutToBeAdded().
	void onDataSourceAboutToBeAdded(int index);
	/// Receives itemAdded() signals from rawDataSources_ and analyzedDataSources, and emits dataSourceAdded().
	void onDataSourceAdded(int index);
	/// Receives itemAboutToBeRemoved() signals from rawDataSources_ and analyzedDataSources_, and emits dataSourceAboutToBeRemoved.
	void onDataSourceAboutToBeRemoved(int index);
	/// Receives itemRemoved() signals from rawDataSources_ and analyzedDataSources_, and emits dataSourceRemoved.
	void onDataSourceRemoved(int index);
	/// Receives modified() signals from the rawDataSources_ and analyszedDataSources_ and calls setModified for the scan.
	void onDataSourceModified(bool modified);

protected:
	const AMDbObject* sampleHelper() const;

protected:

	// meta data values
	//////////////////////

	QString unEvaluatedName_;
	/// user-given number for this scan
	int number_;
	/// Scan start time
	QDateTime dateTime_;
	/// Scan end time.
	QDateTime endDateTime_;
	/// database id of the run and sample that this scan is associated with
	int runId_;
	AMConstDbObject *samplePre2013_;
	AMConstDbObject *sample_;
	/// notes for this sample. Can be plain or rich text, as long as you want it...
	QString notes_;
	/// The absolute file path where this scan's data is stored (if there is an external data file), and the format tag describing the data format.
	QString filePath_, fileFormat_;
	/// Any additional files of raw data that need to be referenced.
	QStringList additionalFilePaths_;
	/// String holding what type of indexation the scan index can take.  This is a first attempt at actually using the scan index.  Currently, the only index type is fileSystem.
	QString indexType_;

	AMScanDictionary *nameDictionary_;
	AMScanDictionary *exportNameDictionary_;

	// Composite members
	//////////////////////

	/// Raw data storage. All scans will have one of these, but the implementation can vary.  The default is an AMInMemoryDataStore; replace using replaceDataStore().
	AMDataStore* data_;
	/// Raw data sources.  Provide AMDataSource interfaces to the data_.
	AMRawDataSourceSet rawDataSources_;
	/// Analyzed data sources.  A set of AMAnalysisBlocks.
	AMAnalyzedDataSourceSet analyzedDataSources_;
	/// Conditions of the beamline/experimental hardware at the beginning of the scan
	AMControlInfoList scanInitialConditions_;
	/// The scan configuration (0 if none has been provided with setScanConfiguration() ). The scan object takes ownership of the scan configuration.
	AMScanConfiguration* configuration_;



	// Database loading and storing.  Protected functions to support loading and storing of composite properties (scanInitialConditions, rawDataSources, analyzeDataSources) in the database. You should never need to use these directly.
	///////////////////////////////

	/// Called when a stored scanInitialCondition is loaded out of the database, but scanInitialConditions() is not returning a pointer to a valid AMControlInfoList. Note: this should never happen, unless the database storage was corrupted and is loading the wrong object type.
	void dbLoadScanInitialConditions(AMDbObject* newLoadedObject);
	/// Returns a list of pointers to the raw data sources, to support db storage.
	AMDbObjectList dbReadRawDataSources() const;
	/// Returns a list of pointers to the analyzed data sources, to support db storage.
	AMDbObjectList dbReadAnalyzedDataSources() const;
	/// Called when loadFromDb() finds a different number (or types) of stored raw data sources than we currently have in-memory.
	/*! Usually, this would only happen when calling loadFromDb() a scan object for the first time, or when re-loading after creating additional raw data sources but not saving them.*/
	void dbLoadRawDataSources(const AMDbObjectList& newRawSources);
	/// Called when loadFromDb() finds a different number (or types) of stored analyzed data sources than we currently have in-memory.
	/*! Usually, this would only happen when calling loadFromDb() on a scan object for the first time, or when re-loading after creating additional analyzed data sources but not saving them.*/
	void dbLoadAnalyzedDataSources(const AMDbObjectList& newAnalyzedSources);

	/// Used by the database system (storeToDb()) to read and store the scan configuration
	AMDbObject* dbGetScanConfiguration() const;
	/// Used by the database system (loadFromDb()) to load a saved scan configuration (if there is no existing scan configuration yet, or if the existing one doesn't match the type stored in the database).
	void dbLoadScanConfiguration(AMDbObject* newObject);

	/// Override of AMDbObject::storeToDb(). Checks if first time stored and auto-generates a scan number before calling AMDbObject::storeToDb() if it is.
	bool storeToDb(AMDatabase *db, bool generateThumbnails);

	/// Helper function which queries the database to determine what the next number is for the given scan name
	int getNextNumberByName(AMDatabase *db);

	AMConstDbObject* dbReadSample() const;
	void dbWriteSample(AMConstDbObject *newSample);
	AMConstDbObject* dbReadSamplePre2013() const;
	void dbWriteSamplePre2013(AMConstDbObject *newSample);

	/// This returns a string describing the input connections of all the analyzed data sources. It's used to save and restore these connections when loading from the database.  (This system is necessary because AMAnalysisBlocks use pointers to AMDataSources to specify their inputs; these pointers will not be the same after new objects are created when restoring from the database.)
	/*! Implementation note: The string contains one line for each AMAnalysisBlock in analyzedDataSources_, in order.  Every line is a sequence of comma-separated numbers, where the number represents the index of a datasource in dataSourceAt().  So for an analysis block using the 1st, 2nd, and 5th sources (in order), the line would be "0,1,4".

Lines are separated by single '\n', so a full string could look like:
\code
0,1,4\n
3,2\n
0,4,3
\endcode
*/
	QString dbReadAnalyzedDataSourcesConnections() const;
	/// When loadFromDb() is called, this receives the string describing the input connections of all the analyzed data sources, and restores their input data connections.
	void dbLoadAnalyzedDataSourcesConnections(const QString& connectionString);

	/// Used to load the currentlyScanning state from the database. If you call setScanController(0), this will be reset to false.
	void dbLoadCurrentlyScanning(bool currentlyScanning) { currentlyScanning_ = currentlyScanning; }


	// Raw Data Loading
	////////////////////////
	/// Controls whether raw data is automatically loaded when restoring this scan from the database (ie: loadData() is called automatically inside loadFromDb().)  This is true by default, but you may want to turn it off for performance reasons when loading a large group of scans just to look at their meta-data. It can be set on a per-thread basis, so this is a map from thread IDs to bool values.
	static QMap<Qt::HANDLE, bool> threadId2autoLoadData_;
	static QMutex threadId2autoLoadDataMutex_;

	// Other
	//////////////////////////

#ifndef ACQUAMAN_NO_ACQUISITION
	/// If this scan is currently in progress, the scan controller should set this to refer to itself, and set it back to 0 when finished, using setScanController().
	AMScanController* controller_;
#endif
	/// This variable is set to true while a scan is in progress (ie: scan controller running), or if the scan was loaded out of the database with the currentlyScanning column true.
	bool currentlyScanning_;


	/// A set of QObjects that want to make sure that this AMScan instance is kept in memory.
	QSet<QObject*> owners_;


private:



};

Q_DECLARE_METATYPE(AMScan*)


#endif // SCAN_H
