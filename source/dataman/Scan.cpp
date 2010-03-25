#include "Scan.h"
#include "dataman/Database.h"

#include <QDebug>

// static dbColumnNames (each instance has same)
QStringList Scan::dbColumnNames_;

Scan::Scan(QObject *parent) : DbObject(parent) {
	// Ensure the static (class-wide) dbColumnNames_ has already been filled:
	dbColumnNames();
}



/// Returns specified channel by name: (returns 0 if not found)
/// Warning: this will only return the first one if multiple channels with the same name
Channel* Scan::channel(QString name) {

    foreach(Channel* ch, ch_) {
        if(ch->name() == name)
            return ch;
    }
    return 0;
}


/// Delete a channel from scan: (All return true on success)
bool Scan::deleteChannel(Channel* channel) {
    return ch_.removeOne(channel);
}

bool Scan::deleteChannel(const QString& channelName) {
    return deleteChannel(channel(channelName));
}

bool Scan::deleteChannel(size_t index) {
    if(index < (size_t)ch_.count()) {
        ch_.removeAt(index);
        return true;
    }
    else
        return false;
}

/// Return comma-separated list of channel names currently available
QString Scan::channelNames() const {
	QStringList names;
	foreach(Channel* ch, ch_) {
		names << ch->name();
	}
	return names.join(",");
}


/// This member function updates a scan in the database (if it exists already), otherwise it adds it to the database.

	// - Scan Objects (ex: new scans) have their id() field set to <1, unless they've been retrieved from the database, in which case they have id() = [valid database key]
	// Watch out: by creating a scan and giving it id() = [some arbitrary positive number]; you'll destroy data in the db.

bool Scan::storeToDb(Database* db) {

	// Always call base class implementation first.
	// Return false if it fails.
	if( !DbObject::storeToDb(db) )
		return false;

	QList<const QVariant*> values;

	QVariant v1(sampleName());
	QVariant v2(comments());
	QVariant v3(channelNames());
	values << &v1 << &v2 << &v3;

	// If this returns with a positive id, it's succeeded.
	return ( db->insertOrUpdate(id(), dbTableName(), dbColumnNames_, values) > 0);

}


/// load a scan (set its properties) by retrieving it based on id.
bool Scan::loadFromDb(Database* db, int sourceId) {

	// always call the base class implementation first. This retrieves/loads all the base-class properties.
	// return false if it fails:
	if( !DbObject::loadFromDb(db, sourceId))
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


