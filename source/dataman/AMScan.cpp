#include "AMScan.h"
#include "dataman/AMDatabase.h"

#include <QDebug>




// static dbColumnNames (each instance has same)
QStringList AMScan::dbColumnNames_;

AMScan::AMScan(QObject *parent)
	: AMDbObject(parent)
{

	// created a new top-level data tree (not shared with anyone). Assigning it to dshared_ gives it a reference count of 1. The tree will be automatically deleted when dshared_ goes out of scope (ie: when dshared_ gets deleted, moving the reference count to 0.)
	dshared_ = d_ = new AMDataTree(0, "x", true);

	// Ensure the static (class-wide) dbColumnNames_ has already been filled:
	dbColumnNames();
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

/// This member function updates a scan in the database (if it exists already), otherwise it adds it to the database.

        // - AMScan Objects (ex: new scans) have their id() field set to <1, unless they've been retrieved from the database, in which case they have id() = [valid database key]
	// Watch out: by creating a scan and giving it id() = [some arbitrary positive number]; you'll destroy data in the db.

bool AMScan::storeToDb(AMDatabase* db) {

	// Always call base class implementation first.
	// Return false if it fails.
        if( !AMDbObject::storeToDb(db) )
		return false;

	QList<const QVariant*> values;

	QVariant v1(sampleName());
	QVariant v2(comments());
	QVariant v3(channelNames().join(","));
	values << &v1 << &v2 << &v3;

	// If this returns with a positive id, it's succeeded.
	return ( db->insertOrUpdate(id(), dbTableName(), dbColumnNames_, values) > 0);

}


/// load a scan (set its properties) by retrieving it based on id.
bool AMScan::loadFromDb(AMDatabase* db, int sourceId) {

	// always call the base class implementation first. This retrieves/loads all the base-class properties.
	// return false if it fails:
        if( !AMDbObject::loadFromDb(db, sourceId))
		return false;

	// Provide memory storage for return value:
	QList<QVariant*> values;
	QVariant v1, v2, v3;
	values << &v1 << &v2 << &v3;

	if( db->retrieve( sourceId, dbTableName(), dbColumnNames_, values) ) {
		setSampleName(v1.toString());
		setComments(v2.toString());
		// v3 ignored; channel names are read-only.
		return true;
	}
	else {
		return false;
	}
}


