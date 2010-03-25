#ifndef SCAN_H
#define SCAN_H

#include <QObject>
#include <QMap>
#include <QDateTime>
#include "SChannel.h"

#include "dataman/DbStorable.h"
#include "dataman/Database.h"

class Scan : public QObject, public DbStorable
{
Q_OBJECT
Q_PROPERTY(QString id READ id)
Q_PROPERTY(QString name READ name WRITE setName)
Q_PROPERTY(int number READ number WRITE setNumber)
Q_PROPERTY(QString sampleName READ sampleName WRITE setSampleName)
Q_PROPERTY(QString comments READ comments WRITE setComments NOTIFY commentsChanged)
Q_PROPERTY(QDateTime startTime READ startTime WRITE setStartTime)

friend void Channel::addToScan(Scan& destination);

public:
    explicit Scan(QObject *parent = 0);

    /// Returns scan's unique id
	int id() const { return dbId(); }
    /// Returns scan's user given name
    QString name() const { return name_;}
    /// Returns scan's appended number
    int number() const { return number_;}
    /// Returns name of sample
    QString sampleName() const { return sampleName_;}
    /// Returns comments for scan
    QString comments() const { return comments_;}
    /// Returns original start time
    QDateTime startTime() const {return startTime_;}
    /// Returns the full scan name: number appended to name
    QString fullName() const {return QString("%1%2").arg(name_).arg(number_); }

    /// Return number of available channels
    int numChannels() const { return ch_.count();}
    /// Returns specified channel by name: (returns 0 if not found)
    Channel* channel(QString name);
    /// Return specified channel by index: (returns 0 if not found)
    Channel* channel(size_t index) { if(index < (size_t)ch_.count() ) return ch_.at(index); else return 0; }
	/// Return a comma-separated list if all channel names (Used for channel hints in database)
	QString channelNames() const;


	/// Implementation of DbStorable. This allows us to be stored in the database.
	// ===================================
	/// Section 1: These static methods define the database structure for the object:

	/// A unique description of the class name for identifying db objects by type
	static QString dbClassName() { return Scan::staticMetaObject.className(); }
	/// The name of the table that will store the object's properties:
	static QString dbTableName() { return "scanTable"; }	// todo: move into system settings?
	/// A list of the column names required to store the object's properties. (note: the key column 'id' is always included; don't specify it here.)
	static QStringList dbColumnNames() { QStringList rl; rl << "name"; rl << "number"; rl << "sampleName"; rl << "comments"; rl << "startTime"; rl << "channels"; return rl; }
	/// A list of the column types recommended to store the object's properties. (note: this must have the same number of items and correspond to dbColumnNames())
	static QStringList dbColumnTypes() { QStringList rl; rl << "TEXT"; rl << "INTEGER"; rl<< "TEXT";		rl << "TEXT";	   rl << "TEXT";		rl << "TEXT"; return rl; }

	/// Section 2: These set and get methods are used by the database to store and retrieve an object:

	/// Provide access to a property, indexed by column:
	QVariant dbValue(int colNumber) const;
	/// Set a property, indexed by column:
	void dbSetValue(int colNumber, const QVariant& value);
	// ======================================

signals:
    /// Emitted when comments string changed
    void commentsChanged(const QString &);
/*
  Belongs in scan controller
    /// Scan has started
    void started();
    /// Scan completed
    void finished();
    /// Scan canceled by user
    void cancelled();
    /// Scan paused
    void paused();
    /// Scan resumed
    void resumed();
    /// Time left in scan
    void timeRemaining(double seconds);
*/

public slots:
    /// Sets user given name
    void setName(const QString &name) { name_ = name;}
    /// Sets appended number
    void setNumber(int number) { number_ = number;}
    /// Sets name of sample
    void setSampleName(const QString &sampleName) { sampleName_ = sampleName;}
    /// Sets comments for scan
    void setComments(const QString &comments) { comments_ = comments; /* TODO: necessary? emit commentsChanged(comments_);*/ }
    /// Sets original start time
    void setStartTime(const QDateTime &startTime) { startTime_ = startTime;}

    /// Delete a channel from scan:
    bool deleteChannel(Channel* channel);
    bool deleteChannel(const QString& channelName);
    bool deleteChannel(size_t index);

	/// Load yourself from the database. (returns true on success)
	/// Detailed subclasses of Scan must re-implement this to retrieve all of their unique data fields.
		/// When doing so, always call the parent class implemention first.
	virtual bool loadFromDb(Database* db, int id);
	/// Store or update self in the database. (returns true on success)
	/// Detailed subclasses of Scan must re-implement this to store all of their unique data.
		/// When doing so, always call the parent class implemention first.
	virtual bool storeToDb(Database* db);

protected:

    /// List of channels
    QList<Channel*> ch_;

    /// User defined scan name
    QString name_;
    /// Number to be appended to scan name
    int number_;
    /// Sample name
    QString sampleName_;
    /// Commments for scan
    QString comments_;
    /// Start time of original scan
    QDateTime startTime_;

};


/// This global function enables using the insertion operator to add scans to the database
///		ex: *Database::db() << myScan
/// Because Scan::storeToDb() is virtual, this version can be used properly for all sub-types of Scans.
Database& operator<<(Database& db, Scan& s);


#endif // SCAN_H
