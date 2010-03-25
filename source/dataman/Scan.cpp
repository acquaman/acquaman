#include "Scan.h"
#include "dataman/Database.h"

#include <QDebug>

// static dbColumnNames (each instance has same)
QStringList Scan::dbColumnNames_;

Scan::Scan(QObject *parent) : QObject(parent) {
	id_ = 0;
	// Insert if not filled already:
	if(dbColumnNames_.count() == 0) {
		dbColumnNames_ << "name";
		dbColumnNames_ << "number";
		dbColumnNames_ << "sampleName";
		dbColumnNames_ << "comments";
		dbColumnNames_ << "startTime";
		dbColumnNames_ << "channels";
	}
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





#include "dataman/Database.h"

/// This member function updates a scan in the database (if it exists already), otherwise it adds it to the database.

	// - Scan Objects (ex: new scans) have their id() field set to <1, unless they've been retrieved from the database, in which case they have id() = [valid database key]
	// Watch out: by creating a scan and giving it id() = [some arbitrary positive number]; you'll destroy data in the db.

bool Scan::storeToDb(Database* db) {

	QList<const QVariant*> values;
	QVariant v1(name());
	QVariant v2(number());
	QVariant v3(sampleName());
	QVariant v4(comments());
	QVariant v5(startTime());
	QVariant v6(channelNames());
	values << &v1 << &v2 << &v3 << &v4 << &v5 << &v6;

	int retVal = db->insertOrUpdate(id(),"scanTable", dbColumnNames_, values);

	if(retVal > 0) {
		id_ = retVal;
		return true;
	}
	else {
		return false;
	}

}


/// load a scan (set its properties) by retrieving it based on id.
bool Scan::loadFromDb(Database* db, int sourceId) {

	// Provide memory storage for return value:
	QList<QVariant*> values;
	QVariant v1, v2, v3, v4, v5, v6;
	values << &v1 << &v2 << &v3 << &v4 << &v5 << &v6;

	if( db->retrieve( sourceId,"scanTable", dbColumnNames_, values) ) {
		id_ = sourceId;
		setName(v1.toString());
		setNumber(v2.toInt());
		setSampleName(v3.toString());
		setComments(v4.toString());
		setStartTime(v5.toDateTime());
		// v6 ignored; channel names are read-only.
		return true;
	}
	else {
		return false;
	}

}

/// This global function enables using the insertion operator to add scans to the database
///		ex: *Database::db() << myScan
/// Because Scan::storeToDb() is virtual, this version can be used properly for all sub-types of Scans.
Database& operator<<(Database& db, Scan& s) {
	s.storeToDb(&db);
	return db;
}

