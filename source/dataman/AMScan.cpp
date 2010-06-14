#include "AMScan.h"
#include "dataman/AMDatabase.h"

#include <QDebug>


AMScan::AMScan(QObject *parent)
	: AMDbObject(parent)
{

	// created a new top-level data tree (not shared with anyone). Assigning it to dshared_ gives it a reference count of 1. The tree will be automatically deleted when dshared_ goes out of scope (ie: when dshared_ gets deleted, moving the reference count to 0.)
	dshared_ = d_ = new AMDataTree(0, "x", true);


	metaData_["sampleName"] = QString();
	metaData_["comments"] = QString();
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

	ch_.addChannel(new AMChannel(this, chName, expression));
	return true;
}


// DBObject database implementation:
///////////////////////////////

/// Store or update self in the database. (returns true on success)
/*! Re-implemented from AMDbObject::storeToDb(), this version saves all of the meta data found for keys metaDataAllKeys(), as well as saving the channel names and channel formulas.
  */
bool AMScan::storeToDb(AMDatabase* db) {

	// the base class version is good at saving all the values in the metaData_ hash. Let's just exploit that.
	metaData_["channelNames"] = channelNames();
	metaData_["channelExpressions"] = channelExpressions();

	// Call the base class implementation
	// Return false if it fails.
	if( !AMDbObject::storeToDb(db) )
		return false;

	// This was cheating... channelNames and channelExpressions aren't stored authoritatively in the metaData_. Let's get rid of them.
	metaData_.remove("channelNames");
	metaData_.remove("channelExpressions");

	return true;
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


