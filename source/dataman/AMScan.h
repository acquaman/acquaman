#ifndef ACQMAN_SCAN_H
#define ACQMAN_SCAN_H


#include <QObject>
#include <QMap>
#include "dataman/AMChannel.h"

#include <QAbstractListModel>

#include "dataman/AMDataTree.h"
#include "dataman/AMDbObject.h"
#include "dataman/AMDataTree.h"

#include "AMBiHash.h"

/// This helper class is a Qt standard model for the list of channels in an AMScan.  This provides a standardized way for views that need to see the channels, and be notified before and after channels are added/removed.
class AMChannelListModel : public QAbstractListModel {

	Q_OBJECT
public:
	AMChannelListModel(QObject* parent = 0) : QAbstractListModel(parent) {}

	int rowCount ( const QModelIndex & parent = QModelIndex() ) const {
		Q_UNUSED(parent)
		return ch_.count();
	}

	QVariant data(const QModelIndex &index, int role) const {
		if(!index.isValid())
			return QVariant();
		if(role == Qt::DisplayRole && index.row() < ch_.count() )
			return QVariant(ch_.at(index.row())->name());
		if(role == Qt::UserRole && index.row() < ch_.count() )
			return qVariantFromValue(ch_.at(index.row()));
		return QVariant();
	}
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const {
		if(role != Qt::DisplayRole)
			return QVariant();
		if(orientation == Qt::Horizontal)
			return QString("Channel");
		if(orientation == Qt::Vertical)
			return QVariant(section);
		return QVariant();
	}


	/// returns a list of channel names currently stored
	QStringList channelNames() const {
		QStringList names;
		foreach(AMChannel* ch, ch_) {
			names << ch->name();
		}
		return names;
	}

	/// Returns specified channel by name: (returns 0 if not found)
	AMChannel* channel(const QString& name) {
		if(name2chIndex_.containsF(name))
			return ch_.at(name2chIndex_.valueF(name));
		else
			return 0;
	}

	const AMChannel* channel(const QString& name) const {
		if(name2chIndex_.containsF(name))
			return ch_.at(name2chIndex_.valueF(name));
		else
			return 0;
	}

	/// Return specified channel by index: (returns 0 if not found)
	AMChannel* channel(int index) { if((unsigned)index < (unsigned)ch_.count() ) return ch_.at(index); else return 0; }
	const AMChannel* channel(int index) const { if((unsigned)index < (unsigned)ch_.count() ) return ch_.at(index); else return 0; }

	/// Returns the index of a channel, or -1 if not here:
	int indexOf(AMChannel* channel) const {
		return ch_.indexOf(channel);
	}

	void addChannel(AMChannel* newChannel) {
		beginInsertRows(QModelIndex(), ch_.count(), ch_.count());
		name2chIndex_.set(newChannel->name(), ch_.count());
		ch_.append(newChannel);
		endInsertRows();
	}

	bool deleteChannel(unsigned index) {
		if(index >= (unsigned)ch_.count())
			return false;

		beginRemoveRows(QModelIndex(), index, index);

		// update the name-to-index lookup... Get rid of the current index, and get rid of the highest index, since everything will move down
		name2chIndex_.removeR(index);
		name2chIndex_.removeR(ch_.count()-1);

		// remove from list
		AMChannel* deleteMe = ch_.takeAt(index);

		// in the name-to-index lookup, move everyone above this channel down
		for(int i=index; i<ch_.count(); i++)
			name2chIndex_.set(ch_.at(i)->name(), i);

		endRemoveRows();
		delete deleteMe;
		return true;
	}


protected:

	QList<AMChannel*> ch_;
	AMBiHash<QString, int> name2chIndex_;
};

/// This class is the base of all objects that represent beamline scan data (for ex: XAS scans over eV, XES detector-image "scans" over detector eV, etc.)
/*! It provides the following:
	- adds a "sampleName()" and a "comments()" field to the basic AMDbObject
		- demonstrates how to subclass AMDbObject to store more detailed information in the database
	- provides a list of pointers to a Scan's "channels",
	\todo complete documentation
  */
class AMScan : public AMDbObject {
Q_OBJECT

Q_PROPERTY(QString sampleName READ sampleName WRITE setSampleName)
Q_PROPERTY(QString comments READ comments WRITE setComments NOTIFY commentsChanged)

public:
	/// default constructor
	explicit AMScan(QObject *parent = 0);

	virtual ~AMScan() {
		// delete channels first.
		while(ch_.rowCount() != 0)
			ch_.deleteChannel(0);
	}

	// Returns scan's unique id
	// use DbObject::id()

/// \todo copy constructor and assignment operator: handle channels as children

	/// Returns name of sample
	QString sampleName() const { return sampleName_;}
	/// Returns comments for scan
	QString comments() const { return comments_;}
	/// Returns original start time
	// use DbObject::dateTime();
	/// Returns the full scan name: number appended to name
	QString fullName() const {return QString("%1%2").arg(name()).arg(number()); }

	/// Return number of available channels
	int numChannels() const { return ch_.rowCount(); }
	/// Returns specified channel by name: (returns 0 if not found)
	AMChannel* channel(const QString& name) { return ch_.channel(name); }
	const AMChannel* channel(const QString& name) const { return ch_.channel(name); }

	/// Return specified channel by index: (returns 0 if not found)
	AMChannel* channel(unsigned index) { return ch_.channel(index);  }
	const AMChannel* channel(unsigned index) const { return ch_.channel(index);  }
	/// Return a comma-separated list of all channel names (Used for channel hints in database)
	QStringList channelNames() const { return ch_.channelNames(); }

	/// returns a standard Qt list model that can be used to view the channels and be notified of created/deleted channels
	const AMChannelListModel* channelList() const { return &ch_; }


	/// the number of datapoints in the scan:
	unsigned count() const { return d_->count(); }

	/// These functions provide support for storing and retrieving from the database.
	// ===================================

	/// The name of the table that will store the object's properties:
	static QString dbTableName() { return "scanTable"; }	// todo: move into system settings?

	/// A list of the column names required to store the object's properties. (note: the key column 'id' is always included; don't specify it here.)
	static const QStringList& dbColumnNames() {
		// Insert if not filled already:
		if(dbColumnNames_.isEmpty()) {
			dbColumnNames_ = QString("sampleName,comments,channels").split(',');
		}
		return dbColumnNames_;
	}
	/// A list of the column types recommended to store the object's properties. (note: this must have the same number of items and correspond to dbColumnNames())
	static QStringList dbColumnTypes() { return QString("TEXT,TEXT,TEXT").split(','); }
	/// A static function to make sure the database is ready to hold us:
	/// When re-implementing, make sure to call base-class implementation first.
		static void dbPrepareTables(AMDatabase* db) { AMDbObject::dbPrepareTables(db); db->ensureTable(dbTableName(), dbColumnNames(), dbColumnTypes()); }

	// ======================================

signals:
	/// Emitted when comments string changed
	void commentsChanged(const QString &);
	/// Emitted when data changes / new data accepted
	void updated(AMScan* me);
/*
  Belongs in scan controller
	/// AMScan has started
	void started();
	/// AMScan completed
	void finished();
	/// AMScan canceled by user
	void cancelled();
	/// AMScan paused
	void paused();
	/// AMScan resumed
	void resumed();
	/// Time left in scan
	void timeRemaining(double seconds);
*/

public slots:

	/// Sets name of sample
	void setSampleName(const QString &sampleName) { sampleName_ = sampleName;}
	/// Sets comments for scan
	void setComments(const QString &comments) { comments_ = comments; /* TODO: necessary? emit commentsChanged(comments_);*/ }
	/// Sets original start time
	// use setDateTime() in DbObject

	/// create a new channel. The channel will be owned and deleted by the scan.
	bool addChannel(const QString& chName, const QString& expression);

	/// Delete a channel from scan:
	bool deleteChannel(AMChannel* channel);
	bool deleteChannel(const QString& channelName);
	bool deleteChannel(unsigned index);


	/// Load yourself from the database. (returns true on success)
		/// Detailed subclasses of AMScan must re-implement this to retrieve all of their unique data fields.
		/// When doing so, always call the parent class implemention first.
		virtual bool loadFromDb(AMDatabase* db, int id);
	/// Store or update self in the database. (returns true on success)
		/// Detailed subclasses of AMScan must re-implement this to store all of their unique data.
		/// When doing so, always call the parent class implemention first.
		virtual bool storeToDb(AMDatabase* db);

protected:

	/// List of channels
	AMChannelListModel ch_;

	/// Sample name
	QString sampleName_;
	/// Commments for scan
	QString comments_;

	/// raw data storage. All scans will have one of these, but the contents and structure will vary.
	AMDataTree* d_;


	/// Allow channels to access the datatree:
	friend AMDataTree* AMChannel::dataTree() const;

	friend class AMAcqScanOutput;
	friend class AMAcqScanSpectrumOutput;
	friend class AMDacqScanController;

private:
	/// List of column names required to have in DB:
	static QStringList dbColumnNames_;

	/// This is used to maintain a reference count of 1 on the implicitly shared AMDataTree d_, and delete d_ when this (dshared_) goes out of scope
	QSharedDataPointer<AMDataTree> dshared_;

};

Q_DECLARE_METATYPE(AMScan*)


#endif // SCAN_H
