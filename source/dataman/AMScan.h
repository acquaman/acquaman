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
#define AMSCAN_ANALYZED_DATASOURCE_COUNT_MISMATCH -2150
#define AMSCAN_THUMBNAIL_SCANNING_MESSAGE -2151

/*!
  * A class representing all the information relating to a single scan performed
  * on a beamline.
  * AMScan's contain:
  *		- The meta-data for the scan (start and end date/time, the facility at which
  *		  it was run, notes etc.).
  *		- The file format of and paths to the raw data collected by the scan.
  *		- A container for the raw and analyzed data sources
  *		- The configuration of the scan
  *		- A list of the control values storing the initial conditions on the beamline
  *		  when the scan took place.
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
	Q_PROPERTY(AMDbObject* scanInitialConditions READ dbReadScanInitialConditions WRITE dbLoadScanInitialConditions)
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

	/*!
	  * Default constructor for an AMScan.
	  * Creates an instance of an AMScan with blank details, no configuration and
	  * no controller.
	  */
	Q_INVOKABLE explicit AMScan(QObject *parent = 0);

	/*!
	  * Static convenience function which creates an AMScan loaded from the database
	  * specified in the provided Acquaman database url format
	  * ("amd://databaseConnection/tableName/id").
	  *
	  * Will return 0 in the case that:
	  *  - No scan is found with the provided id
	  *  - The URL is not valid
	  *  - The scan with the provided url is currently scanning and the passed
	  *    allowIfScanning argument is false (default)
	  *
	  * \param url ~ The Acquaman database url which specifies the location from
	  * which to load the scan, and the id of the scan to load.
	  * \param allowIfScanning (= false) ~ Whether to allow the loading of a scan
	  * which is listed in the database with currentlyScanning as true.
	  * \param[out] isScanning (= 0) ~ If a reference is provided its value
	  * will be set indicating whether the scan is still running.
	  * \param[out] scanName (= 0) ~ If a reference is provided its value will be
	  * set with the full name of the scan (if it is found).
	  */
	static AMScan* createFromDatabaseUrl(const QUrl& url, bool allowIfScanning = false, bool* isScanning = 0, QString* scanName = 0);


	/*!
	  * Cleans up all the resources for this scan.
	  */
	virtual ~AMScan();

	/*!
	 **********************************************************************************
	 * \todo copy constructor and assignment operator required, but not yet implemented.
	 * Do not copy. (implementation note: handle channels as children?)
	 **********************************************************************************
	*/

	// Role 1: Meta Data Elements
	////////////////////////////////

	/*!
	  * A user provided number for the scan.
	  */
	int number() const { return number_;}

	/*!
	  * The start time of the scan. If the scan has not yet been run this will
	  * contain the creation time of the scan.
	  */
	QDateTime dateTime() const {return dateTime_;}

	/*!
	  * The end time of the scan. If the scan has not yet ended this will return
	  * an invalid date/time.
	  */
	QDateTime endDateTime() const { return endDateTime_; }

	/*!
	  * The time taken to perform the scan. If the scan has not yet been run a value
	  * of -1 will be returned.
	  */
	double elapsedTime() const;

	/*!
	  * The id of the run to which the scan belongs. If the scan is not associated
	  * with a run a value of -1 will be returned.
	  */
	int runId() const { return runId_; }

	/*!
	  * The id of the scan's sample. If the scan has not had a sample assigned
	  * a value of -1 will be returned.
	  */
	int sampleId() const;

	/*!
	  * The sample for the scan for those scans which are associated with a sample
	  * in the pre-2013 sample format. If the scan is not currently assigned a
	  * sample in the pre-2013 sample format a value of 0 will be returned.
	  */
	const AMSamplePre2013* samplePre2013() const;

	/*!
	  * The sample for the scan for those scans which are associated with a sample
	  * in the post-2013 sample formet. If the scan is not currently assigned a
	  * sample in the post-2013 sample format a value of 0 will be returned.
	  */
	const AMSample* sample() const;


	/*!
	  * Any additional notes which pertain to the scan.
	  */
	QString notes() const { return notes_; }

	/*!
	  * A string which describes the file format of the stored raw data file.
	  */
	QString fileFormat() const { return fileFormat_; }

	/*!
	  * The directory path and file name of the scan's raw data file.
	  */
	QString filePath() const { return filePath_; }

	/*!
	  * A list of path and file names of any additional raw data files associated
	  * with the scan.
	  */
	QStringList additionalFilePaths() const { return additionalFilePaths_; }

	/*!
	  * The indexation type that the scan will follow.
	  */
	QString indexType() const { return indexType_; }

	// Convenience functions on meta-data:
	/////////////////////////

	/*!
	  * The unevaludate name of the scan
	  * \todo What does this actually mean?
	  */
	QString unEvaluatedName() const;

	/*!
	  * The full scan name. (ie. name #number)
	  */
	QString fullName() const {return QString("%1 #%2").arg(name()).arg(number()); }

	/*!
	  * The name of the sample assinged to this scan. If no sample is set, then
	  * "[no sample]" is returned.
	  */
	QString sampleName() const;


	// Role 2: Database system
	///////////////////////////////////////////

	/*!
	  * Loads the scan from the provided database, which is identified by the
	  * provided id. If the autoloadData() property of the scan is set to true,
	  * and the stored filePath() does not match the existing filePath(), then
	  * the raw data sources are also loaded.
	  *
	  * \param db ~ The database from which to load the scan.
	  * \param id ~ The key identifier of the scan to load from the database.
	  * \returns True if the scan was successfully loaded, false otherwise.
	  */
	virtual bool loadFromDb(AMDatabase* db, int id);


	// Role 3: Data Sources (Raw and Analyzed)
	////////////////////////////////

	/*!
	  * A read-only set of the scan's raw data sources.
	  * NOTE: Each data source in the set represents a single "stream" or "channel"
	  * of data (eg. electron yield measurements and fluorescence yield measurements
	  * would be individual data sources for a simple absorption scan).
	  */
	const AMRawDataSourceSet* rawDataSources() const { return &rawDataSources_; }

	/*!
	  * The number of raw data sources within the scan.
	  */
	int rawDataSourceCount() const { return rawDataSources_.count(); }

	/*!
	  * Adds the provided raw data source to the scan's set of raw data sources
	  * if a raw data source by that name is not already contained in the set.
	  * NOTE: The provided new data source should be initialized, valid and connected
	  * to the data store before being added to the scan.
	  * NOTE2: The scan will take ownership of the raw data source.
	  * \param newRawDataSource ~ The already initialized, valid, connected  and
	  * and unique raw data source to add to the scan.
	  * \returns True if the new raw data source is added to the scan's raw data
	  * source collection, false otherwise.
	  */
	bool addRawDataSource(AMRawDataSource* newRawDataSource);

	/*!
	  * Adds the provided raw data source to the scan, after setting the data
	  * source's visibility in plots and hidden from user state.
	  * NOTE: The provided new data source should be initialized, valid and connected
	  * to the data store before being added to the scan, and a raw data source with
	  * that name should not already be known to the scan.
	  * \param newRawDataSource ~ The already initialized, valid, connected  and
	  * and unique raw data source to add to the scan.
	  * \param visibleInPlots ~ Whether the new raw data source should be drawn
	  * in  plots.
	  * \param hiddenFromUsers ~ Whether the new raw data source should be hidden
	  * from users.
	  * \returns True if the new raw data source is added to the scan's raw data
	  * source collection, false otherwise.
	  */
	bool addRawDataSource(AMRawDataSource* newRawDataSource, bool visibleInPlots, bool hiddenFromUsers);

	/*!
	  * Removes and deletes the scan's raw data source at the provided index.
	  * \param id ~ The index position of the raw data source to remove from the
	  * scan and delete.
	  * \returns True if a data source is removed from the scan, false otherwise
	  */
	bool deleteRawDataSource(int id) { if((unsigned)id >= (unsigned)rawDataSources_.count()) return false; rawDataSources_.takeAt(id)->deleteLater(); return true; }

	/*!
	  * A read-only set of the scan's analysed data sources.
	  */
	const AMAnalyzedDataSourceSet* analyzedDataSources() const { return &analyzedDataSources_; }

	/*!
	  * The number of analysed data sources within the scan.
	  */
	int analyzedDataSourceCount() const { return analyzedDataSources_.count(); }

	/*!
	  * Adds a new analysis block to the set of the scan's analysed data sources.
	  * NOTE: The scan takes ownership of the provided analysis block.
	  * \param newAnalyzedDataSource ~ The new analysis block to be added to the
	  * scan's set of analysis blocks.
	  * \returns True if the analysis block was added to the scan's set of analysed
	  * data sources, false otherwise.
	  */
	bool addAnalyzedDataSource(AMAnalysisBlock* newAnalyzedDataSource);

	/*!
	  * Adds a new analysis block to the set of the scan's analysed data sources
	  * after setting the analysis block's visibleInPlots and hiddenFromUsers state.
	  * NOTE: The scan takes ownership of the provided analysis block.
	  * \param newAnalyzedDataSource ~ The new analysis block to be added to the
	  * scan's set of analysis blocks.
	  * \param visibleInPlots ~ Whether the analysed data source should be drawn
	  * in plots
	  * \param hiddenFromUsers ~ Whether the analysed data source should be hidden
	  * from users.
	  * \returns True if the analysis block was added to the scan' set of analysed
	  * data sources, false otherwise.
	  */
	bool addAnalyzedDataSource(AMAnalysisBlock *newAnalyzedDataSource, bool visibleInPlots, bool hiddenFromUsers);

	/*!
	  * Removes and deletes the scan's analysed data source at the provided index.
	  * \param id ~ The index position of the analysis block to remove and delete
	  * from the scan's set of analysed data sources.
	  */
	bool deleteAnalyzedDataSource(int id) { if((unsigned)id >= (unsigned)analyzedDataSources_.count()) return false; analyzedDataSources_.takeAt(id)->deleteLater(); return true; }

	// Provides a simple access model to all the data sources (combination of rawDataSources()
	// and analyzedDataSources()

	/*!
	  * The total number of data sources associated with the scan (raw and analysed).
	  */
	int dataSourceCount() const { return rawDataSources_.count() + analyzedDataSources_.count(); }

	/*!
	  * The scan's data source at the provided index. Includes both raw data sources
	  * (listed first) and analysed data sources (listed second).
	  * \param index ~ The index of the data source to be returned
	  * \returns The data source at the provided index, or 0 if no data source is
	  * found at the provided index.
	  */
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

	/*!
	  * The index of the scan's data source in the complete list of data sources
	  * (raw and analysed) with the provided name.
	  * \param sourceName ~ The name of the data source whose index is to be returned.
	  * \returns The index of the first data source found with the provided index,
	  * or -1 if no data source is found with the provided name.
	  */
	int indexOfDataSource(const QString& sourceName) const {
		int rawSourceIndex = rawDataSources_.indexOfKey(sourceName);
		if(rawSourceIndex >= 0)
			return rawSourceIndex;
		int analyzedSourceIndex = analyzedDataSources_.indexOfKey(sourceName);
		if(analyzedSourceIndex >= 0)
			return analyzedSourceIndex + rawDataSources_.count();
		return -1;
	}

	/*!
	  * The index of the provided data source in the scan's set of combined data
	  * sources (raw and analysed).
	  * \param source ~ The data source whose index is to be returned.
	  * \returns The index of the data source in the scan's set of combined data
	  * sources, or -1 if the data source is not found.
	  */
	int indexOfDataSource(const AMDataSource* source) const {
		int rawCount = rawDataSources_.count();
		for(int i=0; i<rawCount; i++)
			if(source == rawDataSources_.at(i))
				return i;
		for(int i=0; i<analyzedDataSources_.count(); i++)
			if(source == analyzedDataSources_.at(i))
				return i+rawCount;	// is an index in the combined set, raw sources come first.
		return -1;
	}

	/*!
	  * Removes and deletes the data source which lies at the provided index in
	  * the scan's set of combined data sources (raw and analysed). The data source
	  * will no longer be found in either the combined set of data sources, or the
	  * relevant set of analysed/raw data sources.
	  * \param index ~ The index of the data source to be removed from the scan's
	  * combined set of data sources (raw and analysed).
	  * \returns True if a data source was removed from the set, false otherwise.
	  */
	bool deleteDataSourceAt(int index ) {
		if(index < 0)
			return false;
		int rawCount = rawDataSources_.count();
		if(index < rawCount)	{ // is a raw data source.
			rawDataSources_.takeAt(index)->deleteLater();
			return true;
		}
		index -= rawCount;
		if(index < analyzedDataSources_.count()) {
			analyzedDataSources_.takeAt(index)->deleteLater();
			return true;
		}
		return false;
	}

	// Data sources have an option to be hiddenFromUsers(), for internal-only data
	// that shouldn't be exposed. As one example, default thumbnails are only
	// generated for non-hiddenFromUsers() data sources.  These are convenience
	// helper functions to return the indexes of just the non-hiddenFromUsers data
	// sources.
	//////////////

	/*!
	  * The number of combined (raw and analysed) data source within the scan
	  * which are not hidden from users.
	  */
	int nonHiddenDataSourceCount() const;

	/*!
	  * The number of raw data sources within the scan which are not hidden from
	  * users.
	  */
	int nonHiddenRawDataSourceCount() const;

	/*!
	  * The number of analysed data sources within the scan which are not hidden
	  * from users.
	  */
	int nonHiddenAnalyzedDataSourceCount() const;

	/*!
	  * A list of the indices of data sources within the scan' list of combined
	  * data sources (raw and analysed) which are not hidden from users.
	  */
	QVector<int> nonHiddenDataSourceIndexes() const;

	/*!
	  * A list of the indices of the scan's raw data sources which are not hidden
	  * from users.
	  */
	QVector<int> nonHiddenRawDataSourceIndexes() const;

	/*!
	  * A list of the indices of the scan's analysed data sources which are not
	  * hidden from users.
	  */
	QVector<int> nonHiddenAnalyzedDataSourceIndexes() const;

	// Role 4: Loading/Clearing Raw Data
	////////////////////////////

	/*!
	  * Loads raw data into memory from storage, using the AMFileLoaderInterface
	  * plugin system to find the appropriate file loader, based on fileFormat().
	  * \returns True if the raw data was successfully loaded from storage,
	  * false otherwise.
	  */
	bool loadData();

	/*!
	  * Replaces the scan's raw data store (rawData()) with a different instance/
	  * implementation of AMDataStore.
	  * NOTE: The scan takes ownership of the new data store.
	  *
	  * Some types of file loader plugins and scan controllers may recognize that
	  * it would be more efficient for a certain scan to user one of the disk-based
	  * raw data sources, rather than the default AMInMemortDataStore. They can
	  * call this method to replace the existing data store with a different instance.
	  *
	  * - For scan controllers it is recommended to do this before creating any raw
	  * data sources.
	  * - For file loaders bear in mind that there will already be a raw data source
	  * created when the scan was loaded from the database, therefore:
	  *   - It must have the same scan rank.
	  *   - Each raw data source's measurementId must be valid within it.
	  *   - The measurement rank for each raw data source must match.
	  *   The easiest way to ensure this is to fill the new dataStore appropriately,
	  *   and then call replateDataStore().
	  * \returns False if the new data store is incompatible with any existing
	  * raw data sources, true otherwise.
	  */
	bool replaceRawDataStore(AMDataStore* dataStore);

	/*!
	  * Whether raw data is loaded automatically within loadFromDb() for all AMScan
	  * instances. This value defaults to true.
	  * NOTE: This function is thread safe, and can be set on a per thread basis.
	  * \returns True if raw data should be loaded when calling the loadFromDb()
	  * function when loading scans.
	  */
	static bool autoLoadData();

	/*!
	  * Enables or disables automatically loading raw data inside loadFromDb().
	  * \param autoLoadDataOn ~ Whether auto loading of raw data takes place when
	  * a future call to loadFromDb() is called on AMScan
	  */
	static void setAutoLoadData(bool autoLoadDataOn);

	/*!
	  * Clears the scan's rawData() completely, including all measurements configured
	  * within the data store. Also deletes all rawDataSources() which expose this
	  * data.
	  */
	void clearRawDataPointsAndMeasurementsAndDataSources() {
		while(rawDataSources_.count())
			rawDataSources_.takeAt(rawDataSources_.count()-1)->deleteLater();

		data_->clearAllMeasurements();
	}

	/*!
	  * Clear's the scan' rawData() completely, including all measurements configured
	  * within the data store. Leaves the configured scan axes as-is.
	  * NOTE: Leaves the rawDataSources() as-is. Do not attemp to access non-existent
	  * raw data from these sources.
	  */
	void clearRawDataPointsAndMeasurements() {
		data_->clearAllMeasurements();
	}

	/*!
	  * Clears the scan's rawData() completely, including all configured measurements
	  * and scan axes in the data store. If a scan instance has held data previously,
	  * it is recommended that file loaders and scan controllers call this to start
	  * with a clean slate.
	  */
	void clearRawDataCompletely() {
		data_->clearAll();
	}

	/*!
	  * Clears the scan's raw data, but leaves all measurements, scan axes, and
	  * raw data sources as-is.
	  */
	void clearRawDataPoints() {
		data_->clearScanDataPoints();
	}

	// Role 5: DataStore (Raw Data) Interface
	//////////////////////////////////

	/*!
	  * The raw data for the scan. Should only be accessed from certain objects
	  * (such as scan controllers), which are allowed to modify a scan's raw data.
	  */
	AMDataStore* rawData() { return data_; }

	/*!
	  * The raw data for the scan in a read-only form.
	  */
	const AMDataStore* rawData() const { return data_; }

	// should anything be exposed directly, from the data store? For ex:

	/*!
	  * The number of dimensions in the scan
	  * NOTE: This does not include the dimensions of any multi-dimension detectors,
	  * it is only the dimensions that were scanned over.
	  */
	virtual int scanRank() const { return data_->scanRank(); }

	/*!
	  * The size of the scan along each of its dimensions.
	  */
	virtual AMnDIndex scanSize() const { return data_->scanSize(); }

	/*!
	  * The size of the scan along the dimension at the provided index.
	  * \param axisId ~ The index of the scan axes whose size is to be returned.
	  */
	virtual int scanSize(int axisId) const { return data_->scanSize(axisId); }

	// Role 6: Beamline conditions
	//////////////////////////////

	/*!
	  * The values and descriptions of some of the hardware on the beamline at
	  * the start of the scan.
	  */
	const AMControlInfoList* scanInitialConditions() const { return &scanInitialConditions_; }

	// Role 7: Access to Scan Configuration and Scan Controller
	///////////////////////////////

	/*!
	  * The scan's configuarion
	  */
	AMScanConfiguration* scanConfiguration() { return configuration_; }

	/*!
	  * The scan's configuartion in read-only format.
	  */
	const AMScanConfiguration* scanConfiguration() const { return configuration_; }

	/*!
	  * Sets the scan's configuration, deleting any existing configuration that
	  * the scan might have had.
	  * NOTE: The scan will take ownership of the new configuration.
	  * \param newConfiguration ~ The new configuration of the scan.
	  */
	void setScanConfiguration(AMScanConfiguration* newConfiguration);

	// Role 8: Thumbnail system:
	////////////////////////////////

	/*!
	  * The number of thumbnails the scan currently has. This number relates to:
	  *  - If there are only raw data sources, this will be the number of data sources
	  *  which are not hidden from users.
	  *  - If there are any analysed data sources, this will be the number of analysed
	  *  data sources which are not hidden from users.
	  *  - If the scan is currently in progress, this will be 1 (a scanning in progress
	  *  thumbnail)
	  */
	int thumbnailCount() const;

	/*!
	  * The scan's thumbnail at the provided index.
	  * The thumbnails for a scan reates to
	  *  - If there are only raw data sources, this will be the number of data sources
	  *  which are not hidden from users.
	  *  - If there are any analysed data sources, this will be the number of analysed
	  *  data sources which are not hidden from users.
	  *  - If the scan is currently in progress, this will be 1 (a scanning in progress
	  *  thumbnail)
	  * \param index ~ The index of the thumbnail to be returned.
	  * \returns The scan't thumbnail at the provided index, or an invalid thumbnail
	  * if none is found at that index.
	  */
	AMDbThumbnail thumbnail(int index) const;

	/*!
	  * Whether generating thumbnails (which is a time-consuming task) should be
	  * handed off to a separate thread.
	  */
	virtual bool shouldGenerateThumbnailsInSeparateThread() const { return false; }

	// Role 9: Acquisition status, and link to scan controller
	///////////////////////////////

#ifndef ACQUAMAN_NO_ACQUISITION
	/*!
	  * The scan controller that is currently running the scan, if it is in progress,
	  * otherwise returns 0.
	  */
	AMScanController* scanController() const { return controller_; }

	/*!
	  * Sets the scan controller which will run the scan.
	  * NOTE: This function should not be considered part of the public interface
	  * - only needing to be called from the scan controller itself.
	  */
	void setScanController(AMScanController*);
#endif

	/*!
	  * Whether the scan is currently being run.
	  */
	bool currentlyScanning() const { return currentlyScanning_; }

	// Miscellaneous
	/////////////////////////

	/*!
	  * The largest scan number in the database subject to the provided SQL format
	  * WHERE clause.
	  * \param db ~ The database to look for the larges scan number in
	  * \param whereClause ~ The SQL format where clause used to narrow down the
	  * largest scan number which will be returned.
	  */
	static int largestNumberInScansWhere(AMDatabase* db, const QString& whereClause);

public slots:

	// Role 1: Setting Meta-Data
	///////////////////////////////

	/*!
	  * Sets the unevaluated name of the scan.
	  * \param unEvaluatedName ~ The new unevalulated name of the scan.
	  */
	void setUnEvaluatedName(QString unEvaluatedName);

	/*!
	  * Sets the user defined number of the scan.
	  * \param number ~ The new user defined number of the scan.
	  */
	void setNumber(int number);

	/*!
	  * Sets the scan's start date/time.
	  * \param dt ~ The scan's new start date/time.
	  */
	void setDateTime(const QDateTime& dt);

	/*!
	  * Sets the scan's end date/time.
	  * \param endTime ~ The scan's new end date/time
	  */
	void setEndDateTime(const QDateTime &endTime);

	/*!
	  * Associates the scan with the run with the provided runId.
	  * NOTE: Performs no check that the runId is valid. This is the callers responsibility.
	  * \param newRunId ~ The id of the run which the scan will be associated with.
	  */
	void setRunId(int newRunId);

	/*!
	  * Sets the sample associated with this scan to the sample with the provided
	  * id, located in the table with the provided name. If no database table name
	  * is provided a default of the sample pre-2013 table is assumed.
	  * \param newSampleId ~ The id of the sample to be associated with this scan.
	  * \param databaseTableName (= "") ~ The table name from which to load the
	  * sample with the provided id. If no table name is provided the default
	  * pre-2013 sample table is assumed.
	  */
	void setSampleId(int newSampleId, const QString &databaseTableName = "");

	/*!
	  * Sets the sample associated with this scan to the provided pre-2013 format
	  * sample.
	  * \param samplePre2013 ~ The pre-2013 format sample to associate with this
	  * scan.
	  */
	void setSamplePre2013(const AMSamplePre2013 *samplePre2013);

	/*!
	  * Sets the sample associated with this scan to the provided post-2013 format
	  * sample.
	  * \param sample ~ The post-2013 format sample to associate with this scan.
	  */
	void setSample(const AMSample *sample);

	/*!
	  * Sets the scan's indexation type.
	  */
	void setIndexType(const QString &newType) { indexType_ = newType; setModified(true); }

	/*!
	  * Sets the additional notes which pertain to the scan.
	  * \param notes ~ The new additional notes pertaining to the scan.
	  */
	void setNotes(const QString &notes) { notes_ = notes; setModified(true); }

	/*!
	  * Sets the file path for the scan's raw data.
	  * WARNING: It is possible to break the associatoin to a raw data file by changing
	  * this.
	  * \param The new file path for the scan.
	  */
	void setFilePath(const QString& newPath) { filePath_ = newPath;  setModified(true); }

	/*!
	  * Sets the file format for the scan's raw data.
	  * \param format ~ A string matching the AMAbstractFileLoader::formatTag()
	  * for the scan's new file format.
	  */
	void setFileFormat(const QString& format) { fileFormat_ = format;  setModified(true); }

	/*!
	  * Sets the file path of the scan's additional raw data which might need to
	  * be referenced.
	  * \param additionalFilePaths ~ A list of file paths for the scan's additional
	  * raw data.
	  */
	void setAdditionalFilePaths(const QStringList& additionalFilePaths) { additionalFilePaths_ = additionalFilePaths; setModified(true); }

	/*!
	  * Stores the scan to the database with an option to generate thumbnails for
	  * the scan. If this is the first time the scan is being stored, then a
	  * scan number is auto generated.
	  * \param db ~ The database in which to store the scan.
	  * \param generateThumbnails ( = true) ~ Whether the thumbnails for the scan
	  * should be auto-generated.
	  */
	bool storeToDb(AMDatabase *db, bool generateThumbnails = true);

	/*!
	  * Sets the initial conditions on the beamline when the scan was run.
	  * \param scanInitialConditions ~ The initial conditions on the beamline when
	  * the scan was run.
	  */
	void setScanInitialConditions(const AMControlInfoList &scanInitialConditions);

signals:

	// Meta-data changed signals:
	/////////////////

	/*!
	  * Signals that the start date/time of the scan was altered.
	  * \param newDataTime ~ The date/time the scan start was altered to.
	  */
	void dateTimeChanged(const QDateTime& newDateTime);

	/*!
	  * Signals that the end date/time of the scan was altered.
	  * \param newEndDateTime ~ The date/time the scan end was altered to.
	  */
	void endDateTimeChanged(const QDateTime &newEndDateTime);

	/*!
	  * Signals that the id of the sample associated with this scan was altered.
	  * \param sampleId ~ The value that the sample id was altered to.
	  */
	void sampleIdChanged(int sampleId);

	/*!
	  * Signals that the scan number was altered.
	  * \param number ~ The value the scan number was altered to.
	  */
	void numberChanged(int number);

	/*!
	  * Signals that the currently scanning state of the scan was altered
	  * \param currentlyScanning ~ The currently scanning state the scan was altered
	  * to.
	  */
	void currentlyScanningChanged(bool currentlyScanning);

	/*!
	  * Signals that the configuration of the scan was altered.
	  */
	void scanConfigurationChanged();


	// Combined Data Source Model: Signals
	////////////////////////////////////////

	/*!
	  * Signals that a data source is about to be added at a given index.
	  * \param index ~ The index at which the new data source will be added.
	  */
	void dataSourceAboutToBeAdded(int index);

	/*!
	  * Signals that a data source has just been added at a given index.
	  * \param index ~ The index at which the new data source is now located.
	  */
	void dataSourceAdded(int index);


	/*!
	  * Signals that a data source is about to be removed from a given index.
	  * \param index ~ The index of the data source which is about to be removed.
	  */
	void dataSourceAboutToBeRemoved(int index);

	/*!
	  * Signals that a data source has just been removed from a given index.
	  * \param index ~ The index which the data source was at previous to being
	  * removed.
	  */
	void dataSourceRemoved(int index);

	/*!
	  * Signals that the initial conditions of a scan have been altered.
	  */
	void scanInitialConditionsChanged();

protected slots:


	// Combined Data Source Model: insert/remove handling
	////////////////////////////////////////

	/*!
	  * Slot which handles a notification that a data source is about to be added
	  * at a given index.
	  * \param index ~ The index at which the data source will be added.
	  */
	void onDataSourceAboutToBeAdded(int index);

	/*!
	  * Slot which handles a notification that a data source has been added at a
	  * given index.
	  * \param index ~ The index at which the data source has been added.
	  */
	void onDataSourceAdded(int index);

	/*!
	  * Slot which handles a notification that a data source is about to be removed
	  * from a given index.
	  * \param index ~ The index which the data source will be removed from.
	  */
	void onDataSourceAboutToBeRemoved(int index);

	/*!
	  * Slot which handles a notification that a data source has been removed from
	  * a given index.
	  * \param index ~ The index which the data source was at before it was removed.
	  */
	void onDataSourceRemoved(int index);

	/*!
	  * Slot which handles notification from raw or analysed data sources indicating
	  * that they have been modified.
	  */
	void onDataSourceModified(bool modified);

protected:

	/*!
	  * Returns the sample associated with the scan - with a preference for the
	  * post-2013 format sample.
	  * \returns The post-2013 format sample associated with the scan if it exists,
	  * if not the pre-2013 format sample associated with the scan. If neither
	  * exists 0 will be returned.
	  */
	const AMDbObject* sampleHelper() const;

protected:

	// meta data values
	//////////////////////

	/// The unevaluated name of the scan
	QString unEvaluatedName_;
	/// User-given number for the scan
	int number_;
	/// The scan start date/time (or if not started the creation date/time)
	QDateTime dateTime_;
	/// The end date/time (or if not ended an invalid QDateTime)
	QDateTime endDateTime_;
	/// The database id of the run to which the scan belongs (-1 if no run)
	int runId_;
	/// The pre-2013 format sample associated with the scan
	AMConstDbObject *samplePre2013_;
	/// The post-2013 format sample associated with the scan
	AMConstDbObject *sample_;
	/// additional notes pertaining to the scan
	QString notes_;
	/// The absolute file path where this scan's data is stored (if there is an external data file)
	QString filePath_;
	/// The format tag describing the data format.
	QString fileFormat_;
	/// The paths of any additional files of raw data that need to be referenced.
	QStringList additionalFilePaths_;
	/// String holding what type of indexation the scan index can take.  This is a first attempt at actually using the scan index.  Currently, the only index type is fileSystem.
	QString indexType_;


	AMScanDictionary *nameDictionary_;
	AMScanDictionary *exportNameDictionary_;

	// Composite members
	//////////////////////

	/// Raw data storage. All scans will have one of these, but the implementation can vary.  The default is an AMInMemoryDataStore. Replace using replaceDataStore().
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

	AMControlInfoList* dbReadScanInitialConditions() { return &scanInitialConditions_; }
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

	AMConstDbObject* dbReadSample() const;
	void dbWriteSample(AMConstDbObject *newSample);
	AMConstDbObject* dbReadSamplePre2013() const;
	void dbWriteSamplePre2013(AMConstDbObject *newSample);

	/// This returns a string describing the input connections of all the analyzed data sources. It's used to save and restore these connections when loading from the database.  (This system is necessary because AMAnalysisBlocks use pointers to AMDataSources to specify their inputs. These pointers will not be the same after new objects are created when restoring from the database.)
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
};

Q_DECLARE_METATYPE(AMScan*)


#endif // SCAN_H
