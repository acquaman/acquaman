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


	/// returns a list of channel names currently stored.
	QStringList channelNames() const {
		QStringList names;
		foreach(AMChannel* ch, ch_) {
			names << ch->name();
		}
		return names;
	}

	/// returns a list of the channel expressions. (Channels are ordered the same as channelNames(). )
	QStringList channelExpressions() const {
		QStringList rv;
		foreach(AMChannel* ch, ch_)
			rv << ch->expression();
		return rv;
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

	int indexOfChannel(const QString& name) {
		if(name2chIndex_.containsF(name))
			return name2chIndex_.valueF(name);
		else
			 return -1;
	}

	/// Return specified channel by index: (returns 0 if not found)
	AMChannel* channel(int index) { if((unsigned)index < (unsigned)ch_.count() ) return ch_.at(index); else return 0; }
	const AMChannel* channel(int index) const { if((unsigned)index < (unsigned)ch_.count() ) return ch_.at(index); else return 0; }

	/// Returns the index of a channel, or -1 if not here:
	int indexOf(AMChannel* channel) const {
		return ch_.indexOf(channel);
	}

	bool addChannel(AMChannel* newChannel) {
		if(!newChannel->isValid())
			return false;
		qDebug() << "begin insert rows (channelList)";
		beginInsertRows(QModelIndex(), ch_.count(), ch_.count());
		ch_.append(newChannel);
		name2chIndex_.set(newChannel->name(), ch_.count()-1);
		qDebug() << "end insert rows (channelList)";
		endInsertRows();
		return true;
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
	- adds "sampleId()" and "notes()" meta-data to the basic AMDbObject
		- demonstrates how to subclass AMDbObject to store additional meta-data in the database
	- contains an AMDataTree, which is used to store columns of arbitrary-dimensional raw data
	- provides a list of "channels", which are scientifically-meaningful ways to look at the raw data
	\todo complete documentation
  */
class AMScan : public AMDbObject {

	Q_OBJECT

	Q_PROPERTY(int number READ number WRITE setNumber)
	Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime)
	Q_PROPERTY(int sampleId READ sampleId WRITE setSampleId)
	Q_PROPERTY(QString notes READ notes WRITE setNotes NOTIFY notesChanged)

public:

	// Constructors and Destructors
	//////////////////////////////////

	/// default constructor
	explicit AMScan(QObject *parent = 0);

	virtual ~AMScan() {
		// delete channels first.
		while(ch_.rowCount() != 0)
			ch_.deleteChannel(0);
	}

	/// \todo copy constructor and assignment operator required, but not yet implemented. Do not copy. (implementation note: handle channels as children?)




	// Meta Data Elements
	////////////////////////////////
	/// Returns a user-given number
	int number() const { return metaData_["number"].toInt();}
	/// Returns creation time / scan start time
	QDateTime dateTime() const {return metaData_["dateTime"].toDateTime();}
	/// Returns the id of the run containing this scan, or (-1) if not associated with a run. \todo return more useful run descriptive information
	int runId() const { QVariant v = metaData_["runId"]; if(v.isNull()) return -1; else return v.toInt(); }

	/// Returns name of sample (or -1 if a sample has not been assigned)
	int sampleId() const { QVariant v = metaData_["sampleId"]; if(v.isNull()) return -1; else return v.toInt();}
	/// Returns notes/comments for scan
	QString notes() const { return metaData_["notes"].toString();}

	/// Returns the full scan name: number appended to name
	QString fullName() const {return QString("%1%2").arg(name()).arg(number()); }


	/// Convenience function: returns the name of the sample (if a sample is set)
	QString sampleName() const;


	// Meta-data system
	/////////////////////////////////////////////

	/// Returns the available pieces of meta data for this type of object, including all inherited from base classes. (ie: own + base classes')
	static QList<AMMetaMetaData> metaDataKeys() {
		return AMDbObject::metaDataKeys() << metaDataUniqueKeys();
	}

	/// Returns the available pieces of meta data for this type of object, excluding those inherited from base classes. (ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys() {
		QList<AMMetaMetaData> rv;
		rv << AMMetaMetaData(QVariant::Int, "number", true);
		rv << AMMetaMetaData(QVariant::DateTime, "dateTime", true);
		rv << AMMetaMetaData(QVariant::Int, "runId", false);
		rv << AMMetaMetaData(QVariant::Int, "sampleId", true);
		rv << AMMetaMetaData(QVariant::String, "notes", true);
		rv << AMMetaMetaData(QVariant::StringList, "channelNames", false);
		rv << AMMetaMetaData(QVariant::StringList, "channelExpressions", false);
		return rv;
	}

	/// Returns all the available pieces of meta data for this type of object, by introspecting it's most detailed type. (ie: own + base classes' + subclasses')
	virtual QList<AMMetaMetaData> metaDataAllKeys() const {
		return this->metaDataKeys();
	}

	/// Returns the value of a piece of meta data. (Re-implemented from AMDbObject to catch channelNames or channelExpressions, which aren't found in the metaData_ hash.)
	virtual QVariant metaData(const QString& key) const {

		if(key == "channelNames") {
			return channelNames();
		}

		if(key == "channelExpressions") {
			return channelExpressions();
		}

		return AMDbObject::metaData(key);
	}

	/// set a meta data value. (Re-implemented from AMDbObject to catch channelNames or channelExpressions, which are not allowed to be set as metaData like this.)
	virtual bool setMetaData(const QString& key, const QVariant& value) {

		if(key == "channelNames" || key == "channelExpressions")
			return false;

		return AMDbObject::setMetaData(key, value);
	}


	// Database system
	///////////////////////////////////////////
	/// Load yourself from the database. (returns true on success)
	/*! Re-implemented from AMDbObject, this version loads all of the meta data found for keys metaDataAllKeys(), and also restores the chanel names and channel formulas.*/
	virtual bool loadFromDb(AMDatabase* db, int id);
	/// Store or update self in the database. (returns true on success)
	/*! Re-implemented from AMDbObject::storeToDb(), this version saves all of the meta data found for keys metaDataAllKeys(), as well as saving the channel names and channel formulas.
	  */
	virtual bool storeToDb(AMDatabase* db);

	/// Reimplemented from AMDbObject; provides a general human-readable description
	virtual QString typeDescription() const {
		return "Generic Scan";
	}


	// Channel System
	/////////////////////////////////
	/// Return number of available channels
	int numChannels() const { return ch_.rowCount(); }
	/// Returns specified channel by name: (returns 0 if not found)
	AMChannel* channel(const QString& name) { return ch_.channel(name); }
	const AMChannel* channel(const QString& name) const { return ch_.channel(name); }
	int indexOfChannel(const QString& name) { return ch_.indexOfChannel(name);}

	/// Return specified channel by index: (returns 0 if not found)
	AMChannel* channel(unsigned index) { return ch_.channel(index);  }
	const AMChannel* channel(unsigned index) const { return ch_.channel(index);  }
	/// Return a comma-separated list of all channel names (Used for channel hints in database)
	QStringList channelNames() const { return ch_.channelNames(); }
	QStringList channelExpressions() const { return ch_.channelExpressions(); }

	/// returns a standard Qt list model that can be used to view the channels and be notified of created/deleted channels
	const AMChannelListModel* channelList() const { return &ch_; }

	/// the number of datapoints in the scan:
	unsigned count() const { return d_->count(); }



public slots:

	/// Sets appended number
	void setNumber(int number) { setMetaData("number", number);}
	/// set the date/time:
	void setDateTime(const QDateTime& dt) { setMetaData("dateTime", dt); }
	/// associate this object with a particular run. Set to (-1) to dissociate with any run.  (Note: for now, it's the caller's responsibility to make sure the runId is valid.)
	void setRunId(int runId) { if(runId < 0) metaData_["runId"] = QVariant(); else metaData_["runId"] = runId; }
	/// Sets name of sample
	void setSampleId(int sampleId) { setMetaData("sampleId", sampleId); }
	/// Sets notes for scan
	void setNotes(const QString &notes) { setMetaData("notes", notes); }

	/// create a new channel. The channel will be owned and deleted by the scan.
	bool addChannel(const QString& chName, const QString& expression);

	/// Delete a channel from scan:
	bool deleteChannel(AMChannel* channel);
	bool deleteChannel(const QString& channelName);
	bool deleteChannel(unsigned index);


signals:

	/// Emitted when raw data changes / new data accepted
	void dataChanged(AMScan* me);
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




protected slots:
	/// called by friends after finished updating / loading from file, etc.
	void onDataChanged() {
		emit dataChanged(this);
	}

protected:

	/// List of channels
	AMChannelListModel ch_;

	/// raw data storage. All scans will have one of these, but the contents and structure will vary.
	AMDataTree* d_;


	/// Allow channels to access the datatree:
	friend AMDataTree* AMChannel::dataTree() const;

	friend class AMAcqScanOutput;
	friend class AMAcqScanSpectrumOutput;
	friend class AMDacqScanController;

private:

	/// This is used to maintain a reference count of 1 on the implicitly shared AMDataTree d_, and delete d_ when this (dshared_) goes out of scope
	QSharedDataPointer<AMDataTree> dshared_;

};

Q_DECLARE_METATYPE(AMScan*)


#endif // SCAN_H
