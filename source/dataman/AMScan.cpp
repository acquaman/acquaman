#include "AMScan.h"
#include "dataman/AMDatabase.h"
#include "dataman/AMDatabaseDefinition.h"

#include <QDebug>


AMScan::AMScan(QObject *parent)
	: AMDbObject(parent)
{

	// created a new top-level data tree (not shared with anyone). Assigning it to dshared_ gives it a reference count of 1. The tree will be automatically deleted when dshared_ goes out of scope (ie: when dshared_ gets deleted, moving the reference count to 0.)
	dshared_ = d_ = new AMDataTree(0, "x", true);

	metaData_["number"] = 0;
	metaData_["dateTime"] = QDateTime::currentDateTime();
	metaData_["runId"] = QVariant();
	metaData_["sampleId"] = QVariant();
	metaData_["notes"] = QString();
}



/// Convenience function: returns the name of the sample (if a sample is set)
QString AMScan::sampleName() const {

	if(sampleId() == -1 || database() == 0)
		return QString();

	QVariant vSampleName;
	QList<QVariant*> vList;
	vList << &vSampleName;
	if(database()->retrieve(sampleId(), AMDatabaseDefinition::sampleTableName(), QString("name").split(','), vList))
		return vSampleName.toString();
	else
		return QString();
}



/// Delete a channel from scan: (All return true on success)
bool AMScan::deleteChannel(AMChannel* channel) {
	return ch_.deleteChannel(ch_.indexOf(channel));
}

bool AMScan::deleteChannel(const QString& channelName) {
	return deleteChannel(ch_.channel(channelName));
}

bool AMScan::deleteChannel(unsigned index) {
	return ch_.deleteChannel(index);
}




/// create a new channel. The channel is created with a QObject parent of 0, but will be owned and deleted by this Scan.  Protects against creating channels with duplicate names.
bool AMScan::addChannel(const QString& chName, const QString& expression) {

	if(channelNames().contains(chName))
		return false;

	AMChannel *tmpCh = new AMChannel(this, chName, expression);
	if(!tmpCh->isValid()){
		delete tmpCh;
		return false;
	}
	ch_.addChannel(tmpCh);
	//	ch_.addChannel(new AMChannel(this, chName, expression));
	return true;
}


// DBObject database implementation:
///////////////////////////////


#include "dataman/AMDatabaseDefinition.h"


/// Store or update self in the database. (returns true on success)
/*! Re-implemented from AMDbObject::storeToDb(), this version saves all of the meta data found for keys metaDataAllKeys(), as well as saving the channel names and channel formulas.
  */
bool AMScan::storeToDb(AMDatabase* db) {

	// the base class version is good at saving all the values in the metaData_ hash. Let's just exploit that.
	metaData_["channelNames"] = channelNames();
	metaData_["channelExpressions"] = channelExpressions();

	// Call the base class implementation
	// Return false if it fails.
	bool retval = AMDbObject::storeToDb(db);

	// This was cheating... channelNames and channelExpressions aren't stored authoritatively in the metaData_. Let's get rid of them.
	metaData_.remove("channelNames");
	metaData_.remove("channelExpressions");

	return retval;
}


/// Store or update self in the database. (returns true on success)
/*! Re-implemented from AMDbObject::storeToDb(), this version saves all of the meta data found for keys metaDataAllKeys(), as well as saving the channel names and channel formulas.
  */
bool AMScan::loadFromDb(AMDatabase* db, int sourceId) {

	// always call the base class implementation first. This retrieves/loads all the base-class properties.
	// return false if it fails:
	if( !AMDbObject::loadFromDb(db, sourceId))
		return false;

	// clear the existing channels:
	while(numChannels() != 0)
		deleteChannel(numChannels()-1);

	// retrieve channelNames and channelExpressions: they've been "accidentally" loaded into the hash by AMDbObject::loadFromDb().
	QStringList chNames = metaData_.take("channelNames").toStringList();
	QStringList chExpressions = metaData_.take("channelExpressions").toStringList();
	if(chNames.count() != chExpressions.count()) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -1, "AMScan: couldn't restore saved channels. (The data was corrupted.)"));
		return false;
	}
	for(int i=0; i<chNames.count(); i++)
		addChannel(chNames[i], chExpressions[i]);

	return true;
}


#include <QPixmap>
#include <QBuffer>
#include <QByteArray>

// hackish... just needed for colors. Move the color table somewhere else.
#include "dataman/AMScanSetModel.h"

#include "MPlot/MPlot.h"

/// Return a thumbnail picture of the channel
AMDbThumbnail AMScan::thumbnail(int index) const {

	if((unsigned)index >= (unsigned)numChannels())
		return AMDbThumbnail(QString(), QString(), AMDbThumbnail::InvalidType, QByteArray());

	QPixmap pixmap(240, 180);
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing, true);
	QGraphicsScene gscene(QRectF(0,0,240,180));
	MPlot plot(QRectF(0,0,240,180));
	gscene.addItem(&plot);

	plot.setMarginLeft(0);
	plot.setMarginRight(0);
	plot.setMarginTop(0);
	plot.setMarginBottom(10);
	plot.axisRight()->setTicks(0);
	plot.axisTop()->setTicks(0);
	plot.axisBottom()->setTicks(2);
	plot.axisLeft()->showGrid(false);
	plot.axisBottom()->showGrid(false);
	plot.axisBottom()->showAxisName(false);
	plot.axisLeft()->showAxisName(false);

	MPlotSeriesBasic series(channel(index));
	/// Todo: non-arbitrary colors here; don't mess up the ordering in AMScanSetModelChannelMetaData
	QPen seriesPen(QBrush(AMScanSetModelChannelMetaData::nextColor()), 1);
	series.setLinePen(seriesPen);
	series.setMarker(MPlotMarkerShape::None);

	plot.enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
	plot.addItem(&series);

	gscene.render(&painter);
	painter.end();

	/// todo: pretty names like "Total Electron Yield" instead of "tey_n"
	return AMDbThumbnail(channel(index)->name(), QString(), pixmap);

}
