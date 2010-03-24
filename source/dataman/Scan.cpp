#include "Scan.h"
#include "dataman/Database.h"

#include <QDebug>

Scan::Scan(QObject *parent) : QObject(parent) {
	// Scans have their unique id initialized to 0 (invalid/not stored in DB)
	id_ = 0;
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

QString Scan::channelNames() const {
	QString names;
	foreach(Channel* ch, ch_) {
		names.append(ch->name() + ",");
	}
	return names;
}



#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>

/// This member function updates a scan in the database (if it exists already), otherwise it adds it to the database.
// TODO: we require the following behaviour from the database:
	// - column name id is a unique row index within the table
	// - if id = NULL is inserted, a new row will be created and given the next available unique id
	// - id numbering starts at 1
	// - INSERT OR REPLACE operation exists (enforces unique id)
	// - Scan Objects (ex: new scans) have their id() field set to <1, unless they've been retrieved from the database, in which case they have id() = id
	// Watch out: by creating a scan and giving it id() = [some arbitrary positive number]; you'll destroy data in the db.

bool Scan::storeToDb(Database* db, bool toPublic) {

	QSqlDatabase qdb;

	if(toPublic)
		qdb = db->publicDb();
	else
		qdb = db->userDb();

	if (!qdb.driver()->hasFeature(QSqlDriver::QuerySize)) {
		qDebug() << "Scan-Database (temp comment): missing the query size feature...";
	}

	if(!qdb.isOpen()) {
		qDebug() << "Scan-Database: saving failed; database is not open.";
		return false;
	}

	// start a transaction
	qdb.transaction();

	// prepare the query:
	QSqlQuery query(qdb);
	query.prepare("INSERT OR REPLACE INTO scanTable (id, name, number, sampleName, comments, startTime, channels)"
				  "VALUES (:id, :name, :number, :sampleName, :comments, :startTime, :channels)");

	// If we have a unique id already, use that (will update ourself in the DB)
	if(id_ > 0)
		query.bindValue(":id", id_);
	// Otherwise, use NULL for the :id. (This will create a new one.)
	else
		query.bindValue(":id", QVariant(QVariant::Int));

	// Bind remaining values
	query.bindValue(":name", name());
	query.bindValue(":number", number());
	query.bindValue(":sampleName", sampleName());
	query.bindValue(":comments", comments());
	query.bindValue(":startTime", startTime());
	query.bindValue(":channels", channelNames());


	// Query failed:
	if(!query.exec()) {
		qDebug() << "Scan-Database: saving failed; could not execute query: " << query.executedQuery();
		return false;
	}
	// Query succeeded.
	else {
		// If we don't have one, set the unique id for this scan (now that the database has established it)
		if(id_ < 1) {
			QVariant lastId = query.lastInsertId();
			if(lastId.isValid())
				id_ = lastId.toInt();
			else {
				qDebug() << "Scan-Database: insert succeeded, but could not get lastId after insert. This should never happen...";
			}
		}
	}
	// end transaction
	qdb.commit();

	return true;
}


/// load a scan into 'destination' based on unique id.
bool Scan::loadFromDb(Database* db, int id, bool fromPublic) {
	QSqlDatabase qdb;

	if(fromPublic)
		qdb = db->publicDb();
	else
		qdb = db->userDb();

	QSqlQuery q(qdb);
	q.prepare(QString("SELECT (id, name, number, sampleName, comments, startTime) FROM scanTable WHERE id = %1").arg(id));
	if(q.exec() && q.first()) {

		id_ = id;
		setName(q.value(1).toString());
		setNumber(q.value(2).toInt());
		setSampleName(q.value(3).toString());
		setComments(q.value(4).toString());
		setStartTime(q.value(5).toDateTime());
		q.finish();
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

