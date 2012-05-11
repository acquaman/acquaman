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


#include "AMScan.h"
#include "dataman/database/AMDatabase.h"
#include "acquaman.h"
#include "dataman/AMRun.h"
#include "dataman/AMSample.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/datastore/AMInMemoryDataStore.h"
#include "acquaman/AMScanConfiguration.h"
#include "application/AMPluginsManager.h"
#include "dataman/AMScanDictionary.h"
#include "util/AMErrorMonitor.h"

#include <QDebug>

AMScan::AMScan(QObject *parent)
	: AMDbObject(parent)
{
	number_ = 0;
	dateTime_ = QDateTime::currentDateTime();
	endDateTime_ = QDateTime();
	runId_ = -1;
	sampleId_ = -1;
	notes_ = QString();
	filePath_ = QString();
	fileFormat_ = "unknown";
	indexType_ = "<none>";

	configuration_ = 0;
#ifndef ACQUAMAN_NO_ACQUISITION
	controller_ = 0;
#endif

	currentlyScanning_ = false;

	data_ = new AMInMemoryDataStore();	// data store is initially empty. Needs axes configured in specific subclasses.
	//data_ = new AMDataTreeDataStore(AMAxisInfo("eV", 0, "Incidence Energy", "eV"));

	sampleNameLoaded_ = false;

	nameDictionary_ = new AMScanDictionary(this, this);
	//nameDictionary_->setOperatingOnName(true);
	//exportNameDictionary_ = new AMScanDictionary(this, this);
	//exportNameDictionary_->setOperatingOnExportName(true);

	// Connect added/removed signals from rawDataSources_ and analyzedDataSources_, to provide a model of all data sources:
	connect(rawDataSources_.signalSource(), SIGNAL(itemAboutToBeAdded(int)), this, SLOT(onDataSourceAboutToBeAdded(int)));
	connect(rawDataSources_.signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDataSourceAdded(int)));
	connect(rawDataSources_.signalSource(), SIGNAL(itemAboutToBeRemoved(int)), this, SLOT(onDataSourceAboutToBeRemoved(int)));
	connect(rawDataSources_.signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDataSourceRemoved(int)));

	connect(analyzedDataSources_.signalSource(), SIGNAL(itemAboutToBeAdded(int)), this, SLOT(onDataSourceAboutToBeAdded(int)));
	connect(analyzedDataSources_.signalSource(), SIGNAL(itemAdded(int)), this, SLOT(onDataSourceAdded(int)));
	connect(analyzedDataSources_.signalSource(), SIGNAL(itemAboutToBeRemoved(int)), this, SLOT(onDataSourceAboutToBeRemoved(int)));
	connect(analyzedDataSources_.signalSource(), SIGNAL(itemRemoved(int)), this, SLOT(onDataSourceRemoved(int)));

}


AMScan::~AMScan() {

	AMErrorMon::debug(this, AMSCAN_DEBUG_DELETING_SCAN, QString("Deleting %1").arg(fullName()));

	if(!owners_.isEmpty()) {
		AMErrorMon::alert(this, AMSCAN_SCAN_DELETE_DIRECTLY, "The scan was deleted while other objects were still interested in it. You should never delete a scan directly; instead, call AMScan::release().  Those objects might now attempt to access a deleted scan.");
	}

	// delete all data sources.
	// \note This is expensive if an AMScanSetModel and associated plots are watching. It would be faster to tell those plots, "Peace out, all my data sources are about to disappear", so that they don't need to respond to each removal separately. For now, you should remove this scan from the AMScanSetModel FIRST, and then delete it.
	int count;
	while( (count = analyzedDataSources_.count()) ) {
		AMDataSource* deleteMe = analyzedDataSources_.at(count-1);
		analyzedDataSources_.remove(count-1);
		delete deleteMe;
	}

	while( (count = rawDataSources_.count()) ) {
		AMDataSource* deleteMe = rawDataSources_.at(count-1);
		rawDataSources_.remove(count-1);
		delete deleteMe;
	}

	// delete the scan configuration, if we have one
	if(configuration_) {
		delete configuration_;
		configuration_ = 0;
	}

	// delete the raw data store, which was allocated in the constructor.
	delete data_;
}


void AMScan::setUnEvaluatedName(QString unEvaluatedName){
	if(unEvaluatedName_ != unEvaluatedName){
		unEvaluatedName_ = unEvaluatedName;
		nameDictionary_->parseKeywordStringAndOperate(unEvaluatedName_);
	}
}

void AMScan::setNumber(int number) {
	if(number_ != number){
		number_ = number;
		setModified(true);
		emit numberChanged(number_);
	}
}

void AMScan::setDateTime(const QDateTime& dt)
{
	if(dateTime_ != dt){
		dateTime_ = dt;
		setModified(true);
		emit dateTimeChanged(dateTime_);
	}
}

void AMScan::setEndDateTime(const QDateTime &endTime)
{
	if (endDateTime_ != endTime){

		endDateTime_ = endTime;
		setModified(true);
		emit endDateTimeChanged(endDateTime_);
	}
}

double AMScan::elapsedTime() const
{
	if (endDateTime_.isValid())
		return dateTime_.msecsTo(endDateTime_)/1000;

	return -1;
}

// associate this object with a particular run. Set to (-1) to dissociate with any run.  (Note: for now, it's the caller's responsibility to make sure the runId is valid.)
void AMScan::setRunId(int newRunId) {

	if(newRunId <= 0) runId_ = -1;
	else runId_ = newRunId;
	setModified(true);
}

// Sets name of sample
void AMScan::setSampleId(int newSampleId) {
	if(sampleId_ != newSampleId){
		sampleNameLoaded_ = false;	// invalidate the sample name cache
		if(newSampleId <= 0)
			sampleId_ = -1;
		else
			sampleId_ = newSampleId;

		setModified(true);
		emit sampleIdChanged(sampleId_);
	}
}

QString AMScan::unEvaluatedName() const{
	return unEvaluatedName_;
}

//QString AMScan::evaluatedName() const {
//	return nameDictionary_->parseKeywordString(name());
//}

// Convenience function: returns the name of the sample (if a sample is set)
QString AMScan::sampleName() const {
	if(!sampleNameLoaded_)
		retrieveSampleName();

	return sampleName_;

}



void AMScan::retrieveSampleName() const {
	if(sampleId() <1 || database() == 0)
		sampleName_ = "[no sample]";

	else {
		sampleNameLoaded_ = true;	// don't set sampleNameLoaded_ in the case above. That way we will keep checking until there's a valid database() (for ex: we get saved/stored.) The sampleNameLoaded_ cache is meant to speed up this database call.
		QVariant vSampleName = database()->retrieve(sampleId(), AMDbObjectSupport::s()->tableNameForClass<AMSample>(), QString("name"));
		if(vSampleName.isValid())
			sampleName_ =  vSampleName.toString();
		else
			sampleName_ = "[no sample]";
	}
}

// Loads a saved scan from the database into self. Returns true on success.
/*! Re-implemented from AMDbObject::loadFromDb(), this version also loads the scan's raw data if autoLoadData() is set to true, and the stored filePath doesn't match the existing filePath()*/
bool AMScan::loadFromDb(AMDatabase* db, int sourceId) {

	QString oldFilePath = filePath();

	// always call the base class implementation first. This retrieves/loads all the base-class properties.
	// return false if it fails:
	if( !AMDbObject::loadFromDb(db, sourceId)){

		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -482, "AMScan: Loading from database failed."));
		return false;
	}
	// In auto-load data mode: If the file path is different than the old one, clear and reload the raw data.
	if( !currentlyScanning() && autoLoadData() && filePath() != oldFilePath ) {
		if(!loadData()){

			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, -483, "AMScan: Loading data failed."));
			return false;
		}
	}

	delete nameDictionary_;
	nameDictionary_ = new AMScanDictionary(this, this);

	return true;
}


// Called when a stored scanInitialCondition is loaded out of the database, but scanInitialConditions() is not returning a pointer to a valid AMControlInfoList. Note: this should never happen, unless the database storage was corrupted and is loading the wrong object type.
void AMScan::dbLoadScanInitialConditions(AMDbObject* newLoadedObject) {
	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -89, "There was an error re-loading the initial conditions for this scan from the database. This should never happen unless your database is corrupted. Please report this bug to the Acquaman developers."));

	// delete newLoadedObject, since we don't intend to do anything with it, but we're responsible for it.
	if(newLoadedObject)
		delete newLoadedObject;
}

// Returns a list of pointers to the raw data sources, to support db storage.
AMDbObjectList AMScan::dbReadRawDataSources() const {
	AMDbObjectList rv;
	for(int i=0; i<rawDataSources_.count(); i++)
		rv << rawDataSources_.at(i);
	return rv;
}
// Returns a list of pointers to the analyzed data sources, to support db storage.
AMDbObjectList AMScan::dbReadAnalyzedDataSources() const {
	AMDbObjectList rv;
	for(int i=0; i<analyzedDataSources_.count(); i++)
		rv << analyzedDataSources_.at(i);
	return rv;
}
// Called when loadFromDb() finds a different number (or types) of stored raw data sources than we currently have in-memory.
/* Usually, this would only happen when calling loadFromDb() a scan object for the first time, or when re-loading after creating additional raw data sources but not saving them.*/
void AMScan::dbLoadRawDataSources(const AMDbObjectList& newRawSources) {
	// delete the existing raw data sources, since they will be replaced. (Does nothing if there are no sources yet.)
	int count;
	while( (count = rawDataSources_.count()) ) {
		AMRawDataSource* deleteMe = rawDataSources_.at(count-1);
		rawDataSources_.remove(count-1);	// removing at the end is fastest.
		delete deleteMe;
	}

	// add new sources. Simply adding these to rawDataSources_ will be enough to emit the signals that tell everyone watching we have new data channels.
	for(int i=0; i<newRawSources.count(); i++) {
		AMRawDataSource* newRawSource = qobject_cast<AMRawDataSource*>(newRawSources.at(i));
		if(newRawSource) {

			rawDataSources_.append(newRawSource, newRawSource->name());
			connect(newRawSource, SIGNAL(modifiedChanged(bool)), this, SLOT(onDataSourceModified()));
		}
		else
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "There was an error reloading one of this scan's raw data sources from the database. Your database might be corrupted. Please report this bug to the Acquaman developers."));
	}
}

// Called when loadFromDb() finds a different number (or types) of stored analyzed data sources than we currently have in-memory.
/* Usually, this would only happen when calling loadFromDb() on a scan object for the first time, or when re-loading after creating additional analyzed data sources but not saving them.*/
void AMScan::dbLoadAnalyzedDataSources(const AMDbObjectList& newAnalyzedSources) {
	// delete the existing data sources, since they will be replaced. (Does nothing if there are no sources yet.)
	int count;
	while( (count = analyzedDataSources_.count()) ) {
		AMAnalysisBlock* deleteMe = analyzedDataSources_.at(count-1);
		analyzedDataSources_.remove(count-1);	// removing at the end is fastest.
		delete deleteMe;
	}

	// Simply adding these to analyzedDataSources_ will be enough to emit the signals that tell everyone watching we have new data channels.
	for(int i=0; i<newAnalyzedSources.count(); i++) {

		AMAnalysisBlock* newSource = qobject_cast<AMAnalysisBlock*>(newAnalyzedSources.at(i));
		if(newSource) {

			analyzedDataSources_.append(newSource, newSource->name());
			connect(newSource, SIGNAL(modifiedChanged(bool)), this, SLOT(onDataSourceModified()));
		}
		else
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "There was an error reloading one of this scan's processed data sources from the database. Your database might be corrupted. Please report this bug to the Acquaman developers."));
	}
}

// This returns a string describing the input connections of all the analyzed data sources. It's used to save and restore these connections when loading from the database.  (This system is necessary because AMAnalysisBlocks use pointers to AMDataSources to specify their inputs; these pointers will not be the same after new objects are created when restoring from the database.)
/* Implementation note: The string contains one line for each AMAnalysisBlock in analyzedDataSources_, in order.  Every line is a sequence of comma-separated numbers, where the number represents the index of a datasource in dataSourceAt().  So for an analysis block using the 1st, 2nd, and 5th sources (in order), the line would be "0,1,4".

Lines are separated by single '\n', so a full string could look like:
"0,1,4\n
3,2\n
0,4,3"
*/
QString AMScan::dbReadAnalyzedDataSourcesConnections() const {
	QStringList rv;

	for(int i=0; i<analyzedDataSources_.count(); i++) {
		AMAnalysisBlock* block = analyzedDataSources_.at(i);
		QStringList connections;
		for(int j=0; j<block->inputDataSourceCount(); j++)
			connections << QString("%1").arg(indexOfDataSource(block->inputDataSourceAt(j)));
		rv << connections.join(",");
	}

	return rv.join("\n");
}


// This receives the string describing the input connections of all the analyzed data sources, when loadFromDb() is called., and restores the input data connections for all AMAnalysisBlocks in analyzedDataSources_.
void AMScan::dbLoadAnalyzedDataSourcesConnections(const QString& connectionString) {

	/// \todo check that properties are always loaded in their declared order. This must be called after the raw data sources and analyzed data sources are loaded.
	QStringList allConnections = connectionString.split("\n", QString::SkipEmptyParts);

	if(allConnections.count() != analyzedDataSources_.count()) {
		AMErrorMon::alert(this, AMSCAN_ANALYZED_DATASOURCE_COUNT_MISMATCH, QString("There was an error re-connecting the analysis and processing components for this scan; the number of analysis blocks didn't match. Your database might be corrupted. Please report this bug to the Acquaman developers. All connection: %1. Number of analyzed datasources: %2").arg(allConnections.join(" ")).arg(analyzedDataSources_.count()) );
		return;
	}

	// for each analysis block, set inputs
	for(int i=0; i<analyzedDataSources_.count(); i++) {
		QStringList connections = allConnections.at(i).split(",", QString::SkipEmptyParts);
		QList<AMDataSource*> inputs;
		// for each input to this block, add to input list
		for(int j=0; j<connections.count(); j++)
			inputs << dataSourceAt(connections.at(j).toInt());

		if(!analyzedDataSources_.at(i)->setInputDataSources(inputs))
			AMErrorMon::report(AMErrorReport(	this,
								AMErrorReport::Alert,
								0,
								QString("There was an error re-connecting the inputs for the analysis component '%1: %2', when reloading this scan from the database. Your database might be corrupted. Please report this bug to the Acquaman developers.").arg(analyzedDataSources_.at(i)->name()).arg(analyzedDataSources_.at(i)->description())));
	}
}


// Receives itemAdded() signals from rawDataSources_ and analyzedDataSources, and emits dataSourceAdded().
void AMScan::onDataSourceAdded(int index) {
	if(sender() == rawDataSources_.signalSource())
		emit dataSourceAdded(index);
	else if(sender() == analyzedDataSources_.signalSource() )
		emit dataSourceAdded(index+rawDataSources_.count());	// this is an index for the combined set of raw+analyzed data sources, for dataSourceAt(). Raw data sources come first.
}

void AMScan::onDataSourceAboutToBeAdded(int index) {
	if(sender() == rawDataSources_.signalSource())
		emit dataSourceAboutToBeAdded(index);
	else if(sender() == analyzedDataSources_.signalSource())
		emit dataSourceAboutToBeAdded(index+rawDataSources_.count());
}


// Receives itemAboutToBeRemoved() signals from rawDataSources_ and analyzedDataSources_, and emits dataSourceAboutToBeRemoved.
void AMScan::onDataSourceAboutToBeRemoved(int index) {
	if(sender() == rawDataSources_.signalSource())
		emit dataSourceAboutToBeRemoved(index);
	else if(sender() == analyzedDataSources_.signalSource())
		emit dataSourceAboutToBeRemoved(index+rawDataSources_.count());
}


// Receives itemAboutToBeRemoved() signals from rawDataSources_ and analyzedDataSources_, and emits dataSourceAboutToBeRemoved.
void AMScan::onDataSourceRemoved(int index) {
	if(sender() == rawDataSources_.signalSource())
		emit dataSourceRemoved(index);
	else if(sender() == analyzedDataSources_.signalSource())
		emit dataSourceRemoved(index+rawDataSources_.count());
}



// Set the scan configuration. Deletes the existing scanConfiguration() if there is one.  The scan takes ownership of the \c newConfiguration and will delete it when being deleted.
void AMScan::setScanConfiguration(AMScanConfiguration* newConfiguration) {
	if(!newConfiguration)
		return;
	if(configuration_ == newConfiguration)
		return;
	if(configuration_)
		delete configuration_;
	configuration_ = newConfiguration;
	setModified(true);
	emit scanConfigurationChanged();
}


// Used by the database system (storeToDb()) to read and store the scan configuration
AMDbObject* AMScan::dbGetScanConfiguration() const { return configuration_; }

// Used by the database system (loadFromDb()) to load a saved scan configuration (if there is no existing scan configuration yet, or if the existing one doesn't match the type stored in the database).
void AMScan::dbLoadScanConfiguration(AMDbObject* newObject) {
	AMScanConfiguration* sc;
	if((sc = qobject_cast<AMScanConfiguration*>(newObject)))
		setScanConfiguration(sc);
}

// Publicly expose part of the rawData(), by adding a new AMRawDataSource to the scan. The new data source \c newRawDataSource should be valid, initialized and connected to the data store already.  The scan takes ownership of \c newRawDataSource.  This function returns false if raw data source already exists with the same name as the \c newRawDataSource.
bool AMScan::addRawDataSource(AMRawDataSource *newRawDataSource)
{
	if(newRawDataSource && rawDataSources_.append(newRawDataSource, newRawDataSource->name())) {

		connect(newRawDataSource, SIGNAL(modifiedChanged(bool)), this, SLOT(onDataSourceModified()));
		return true;
	}

	return false;
}

// This overloaded function calls addRawDataSource() after setting the visibleInPlots() and hiddenFromUsers() hints of the data source.
bool AMScan::addRawDataSource(AMRawDataSource* newRawDataSource, bool visibleInPlots, bool hiddenFromUsers) {
	if(newRawDataSource) {
		newRawDataSource->setHiddenFromUsers(hiddenFromUsers);
		newRawDataSource->setVisibleInPlots(visibleInPlots);
	}

	return addRawDataSource(newRawDataSource);
}

bool AMScan::addAnalyzedDataSource(AMAnalysisBlock *newAnalyzedDataSource)
{
	if(newAnalyzedDataSource && analyzedDataSources_.append(newAnalyzedDataSource, newAnalyzedDataSource->name())){

		connect(newAnalyzedDataSource, SIGNAL(modifiedChanged(bool)), this, SLOT(onDataSourceModified()));
		return true;
	}

	return false;
}
// This overloaded function calls addAnalyzedDataSource() after setting the visibleInPlots() and hiddenFromUsers() hints of the data source.
bool AMScan::addAnalyzedDataSource(AMAnalysisBlock *newAnalyzedDataSource, bool visibleInPlots, bool hiddenFromUsers) {
	if(newAnalyzedDataSource) {
		newAnalyzedDataSource->setHiddenFromUsers(hiddenFromUsers);
		newAnalyzedDataSource->setVisibleInPlots(visibleInPlots);
	}
	return addAnalyzedDataSource(newAnalyzedDataSource);
}


#include <QImage>
#include <QBuffer>
#include <QByteArray>
#include <QFile>

#include "util/AMDataSourcePlotSettings.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotImage.h"
#include "dataman/datasource/AMDataSourceSeriesData.h"
#include "dataman/datasource/AMDataSourceImageData.h"
#include "dataman/datasource/AMDataSourceImageDatawDefault.h"
#include "util/AMDateTimeUtils.h"

int AMScan::thumbnailCount() const{
	if(currentlyScanning()){
		return 1;
	}
	int analyzedCount = nonHiddenAnalyzedDataSourceCount();
	if(analyzedCount)
		return analyzedCount;
	else
		return nonHiddenRawDataSourceCount();
}

// Return a thumbnail picture for thumbnail number \c index. For now, we use the following decision: Normally we provide thumbnails for all the analyzed data sources.  If there are no analyzed data sources, we provide thumbnails for all the raw data sources.
AMDbThumbnail AMScan::thumbnail(int index) const {
	if(currentlyScanning()) {

		AMErrorMon::debug(this, AMSCAN_THUMBNAIL_SCANNING_MESSAGE, "Thumbnail: AMScan know it's scanning");
		QFile file(":/240x180/currentlyScanningThumbnail.png");
		file.open(QIODevice::ReadOnly);
		return AMDbThumbnail("Started",
							 AMDateTimeUtils::prettyDateTime(dateTime()),
							 AMDbThumbnail::PNGType,
							 file.readAll());
	}


	int analyzedCount = nonHiddenAnalyzedDataSourceCount();

	bool useRawSources = (analyzedCount == 0);

	// in range?
	if( index < 0 ||
			(useRawSources && index >= nonHiddenRawDataSourceCount()) ||
			(!useRawSources && index >= analyzedCount)
			)
		return AMDbThumbnail(QString(), QString(), AMDbThumbnail::InvalidType, QByteArray());


	QImage image(240, 180, QImage::Format_ARGB32_Premultiplied);
	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing, true);
	QGraphicsScene gscene(QRectF(0,0,240,180));
	MPlot* plot = new MPlot(QRectF(0,0,240,180));
	gscene.addItem(plot);

	plot->setMarginLeft(0);
	plot->setMarginRight(0);
	plot->setMarginTop(0);
	plot->setMarginBottom(10);
	plot->axisRight()->setTicks(0);
	plot->axisTop()->setTicks(0);
	plot->axisBottom()->setTicks(2);
	plot->axisLeft()->showGrid(false);
	plot->axisBottom()->showGrid(false);
	plot->axisBottom()->showAxisName(false);
	plot->axisLeft()->showAxisName(false);
	plot->legend()->enableDefaultLegend(false);	// don't show default legend names, because we want to provide these as text later; don't include them in the bitmap

	const AMDataSource* dataSource;
	if(useRawSources)
		dataSource = rawDataSources()->at(nonHiddenRawDataSourceIndexes().at(index));
	else
		dataSource = analyzedDataSources()->at(nonHiddenAnalyzedDataSourceIndexes().at(index));

	switch(dataSource->rank()) {
	case 1: {
		MPlotSeriesBasic* series = new MPlotSeriesBasic();
		series->setModel(new AMDataSourceSeriesData(dataSource), true);
		QPen seriesPen(QBrush(AMDataSourcePlotSettings::nextColor()), 0);
		series->setLinePen(seriesPen);
		series->setMarker(MPlotMarkerShape::None);
		plot->addItem(series);
		plot->doDelayedAutoScale();
		break; }
	case 2: {
		if (scanRank() == 2){

			MPlotImageBasicwDefault* image = new MPlotImageBasicwDefault();
			image->setModel(new AMDataSourceImageDatawDefault(dataSource, 0), true);
			plot->addItem(image);
			plot->doDelayedAutoScale();
		}

		else{

			MPlotImageBasic* image = new MPlotImageBasic();
			image->setModel(new AMDataSourceImageData(dataSource), true);
			plot->addItem(image);
			plot->doDelayedAutoScale();
		}

		break; }
	default: {
		// what?
		break; }
	}

	gscene.render(&painter);
	painter.end();

	delete plot;	// deletes all plot items (series, image) with it.

	return AMDbThumbnail(dataSource->description(), dataSource->name(), image);

}

bool AMScan::loadData()

{
	bool accepts = false;
	bool success = false;

	// find the available file loaders that claim to work for our fileFormat:
	QList<AMFileLoaderFactory*> acceptingFileLoaders = AMPluginsManager::s()->availableFileLoaderPlugins().values(fileFormat());

	for(int x = 0; x < acceptingFileLoaders.count(); x++) {
		if((accepts = acceptingFileLoaders.at(x)->accepts(this))){
			AMFileLoaderInterface* fileLoader = acceptingFileLoaders.at(x)->createFileLoader();
			success = fileLoader->load(this, AMUserSettings::userDataFolder);
			break;
		}

	}
	if(!accepts)
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMSCAN_CANNOT_FIND_SUITABLE_PLUGIN_FOR_FILE_FORMAT, QString("Could not find a suitable plugin for loading the file format '%1'.  Check the Acquaman preferences for the correct plugin locations, and contact the Acquaman developers for assistance.").arg(fileFormat())));

	if(success)
		for(int i=rawDataSources_.count()-1; i>=0; i--)
			rawDataSources_.at(i)->setDataStore(rawData());
	return success;
}

#ifndef ACQUAMAN_NO_ACQUISITION
void AMScan::setScanController(AMScanController* scanController)
{
	bool wasScanning = currentlyScanning_;

	controller_ = scanController;
	currentlyScanning_ = (controller_ != 0);

	if(currentlyScanning_ != wasScanning) {
		setModified(true);
		emit currentlyScanningChanged(currentlyScanning_);
	}
}

#include <QThread>
#include <QMutexLocker>

QMap<Qt::HANDLE, bool> AMScan::threadId2autoLoadData_;
QMutex AMScan::threadId2autoLoadDataMutex_(QMutex::Recursive);

bool AMScan::autoLoadData()
{
	QMutexLocker l(&threadId2autoLoadDataMutex_);
	return threadId2autoLoadData_.value(QThread::currentThreadId(), true);
}

void AMScan::setAutoLoadData(bool autoLoadDataOn)
{
	threadId2autoLoadDataMutex_.lock();
	threadId2autoLoadData_[QThread::currentThreadId()] = autoLoadDataOn;
	threadId2autoLoadDataMutex_.unlock();
}

int AMScan::nonHiddenDataSourceCount() const
{
	int rv = 0;
	for(int i=0,cc=dataSourceCount(); i<cc; i++) {
		if(!dataSourceAt(i)->hiddenFromUsers())
			rv++;
	}
	return rv;
}

int AMScan::nonHiddenRawDataSourceCount() const
{
	int rv = 0;
	for(int i=0,cc=rawDataSourceCount(); i<cc; i++) {
		if(!rawDataSources()->at(i)->hiddenFromUsers())
			rv++;
	}
	return rv;
}

int AMScan::nonHiddenAnalyzedDataSourceCount() const
{
	int rv = 0;
	for(int i=0,cc=analyzedDataSourceCount(); i<cc; i++) {
		if(!analyzedDataSources()->at(i)->hiddenFromUsers())
			rv++;
	}
	return rv;
}

QVector<int> AMScan::nonHiddenDataSourceIndexes() const
{
	QVector<int> rv;
	for(int i=0,cc=dataSourceCount(); i<cc; i++) {
		if(!dataSourceAt(i)->hiddenFromUsers())
			rv << i;
	}
	return rv;

}

QVector<int> AMScan::nonHiddenRawDataSourceIndexes() const
{
	QVector<int> rv;
	for(int i=0,cc=rawDataSourceCount(); i<cc; i++) {
		if(!rawDataSources()->at(i)->hiddenFromUsers())
			rv << i;
	}
	return rv;
}

QVector<int> AMScan::nonHiddenAnalyzedDataSourceIndexes() const
{
	QVector<int> rv;
	for(int i=0,cc=analyzedDataSourceCount(); i<cc; i++) {
		if(!analyzedDataSources()->at(i)->hiddenFromUsers())
			rv << i;
	}
	return rv;
}

void AMScan::retain(QObject *owner)
{
	if(owner)
		owners_ << owner;
}

void AMScan::release(QObject *pastOwner)
{
	if(pastOwner)
		owners_.remove(pastOwner);

	if(owners_.isEmpty())	// nobody wants us anymore... So sad.
		delete this;			// commit suicide.
}

#endif
