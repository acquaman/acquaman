#include "Scan.h"
#include "dataman/Database.h"

#include <QDebug>

Scan::Scan(QObject *parent) : QObject(parent), DbStorable() {

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


/// Provide access to a property, indexed by column:
QVariant Scan::dbValue(int colNumber) const {
	switch(colNumber) {
		case 0: return name(); break;
		case 1: return number(); break;
		case 2: return sampleName(); break;
		case 3: return comments(); break;
		case 4: return startTime(); break;
		case 5: return channelNames(); break;
		default: return QVariant(QVariant::Int); break;	// return "NULL" value
	}
}

/// Set a property, indexed by column:
void Scan::dbSetValue(int colNumber, const QVariant& value) {
	switch(colNumber) {
		case 0: setName(value.toString()); break;
		case 1: setNumber(value.toInt()); break;
		case 2: setSampleName(value.toString()); break;
		case 3: setComments(value.toString()); break;
		case 4: setStartTime(value.toDateTime()); break;
		case 5: break;	// TODO: what to do about setting channel names? These are read-only in the main db table.
		default: break;
	}
}


#include "dataman/Database.h"

/// This member function updates a scan in the database (if it exists already), otherwise it adds it to the database.
// TODO: we require the following behaviour from the database:
	// - column name id is a unique row index within the table
	// - if id = NULL is inserted, a new row will be created and given the next available unique id
	// - id numbering starts at 1
	// - INSERT OR REPLACE operation exists (enforces unique id)
	// - Scan Objects (ex: new scans) have their id() field set to <1, unless they've been retrieved from the database, in which case they have id() = id
	// Watch out: by creating a scan and giving it id() = [some arbitrary positive number]; you'll destroy data in the db.

bool Scan::storeToDb(Database* db) {

	return db->insertOrUpdate(*this);
}


/// load a scan into 'destination' based on unique id.
bool Scan::loadFromDb(Database* db, int id) {

	return db->retrieve(*this, id);

}

/// This global function enables using the insertion operator to add scans to the database
///		ex: *Database::db() << myScan
/// Because Scan::storeToDb() is virtual, this version can be used properly for all sub-types of Scans.
Database& operator<<(Database& db, Scan& s) {
	s.storeToDb(&db);
	return db;
}

