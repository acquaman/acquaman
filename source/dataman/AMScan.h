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

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;


	/// returns a list of channel names currently stored.
	QStringList channelNames() const;

	/// returns a list of the channel expressions. (Channels are ordered the same as channelNames(). )
	QStringList channelExpressions() const;

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

	bool addChannel(AMChannel* newChannel);

	bool deleteChannel(unsigned index);


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
	Q_PROPERTY(QString notes READ notes WRITE setNotes)

	AM_DBPROPERTYINFO(dateTime, "createIndex=true")
	AM_DBPROPERTYINFO(sampleId, "createIndex=true")

	AM_DBOBJECTINFO("description=Generic Scan")

public:

	// Constructors and Destructors
	//////////////////////////////////

	/// default constructor
	explicit AMScan(QObject *parent = 0);

	/// Destructor: deletes all channels.
	virtual ~AMScan();

	/// \todo copy constructor and assignment operator required, but not yet implemented. Do not copy. (implementation note: handle channels as children?)

	// Raw Data Management
	/// Load raw data into memory from storage. Returns true on success.
	/*! This will attempt to use the scan's current filePath() and fileFormat() as the source. Subclasses must re-implement this function to handle their set of readable file formats.  The base class implementation does nothing and returns true.*/
	virtual bool loadData() {
		return true;
	}
	/// Controls whether raw data is loaded automatically inside loadFromDb().  If autoLoadData() is true, then whenever loadFromDb() is called and the new filePath() is different than the old filePath(), loadData() will be called as well.  If you want to turn off loading raw data for performance reasons, call setAutoLoadData(false).  Auto-loading is enabled by default.
	bool autoLoadData() const {
		return autoLoadData_;
	}
	/// Enables or disables automatically loading raw data inside loadFromDb().
	void setAutoLoadData(bool autoLoadDataOn) {
		autoLoadData_ = autoLoadDataOn;
	}


	// Meta Data Elements
	////////////////////////////////
	/// Returns a user-given number
	int number() const { return metaData_.value("number").toInt();}
	/// Returns creation time / scan start time
	QDateTime dateTime() const {return metaData_.value("dateTime").toDateTime();}
	/// Returns the id of the run containing this scan, or (-1) if not associated with a run.
	int runId() const { QVariant v = metaData_.value("runId"); if(v.isNull()) return -1; else return v.toInt(); }

	/// Returns name of sample (or -1 if a sample has not been assigned)
	int sampleId() const { QVariant v = metaData_.value("sampleId"); if(v.isNull()) return -1; else return v.toInt();}
	/// Returns notes/comments for scan
	QString notes() const { return metaData_.value("notes").toString();}

	/// Returns the full scan name: number appended to name
	QString fullName() const {return QString("%1 #%2").arg(name()).arg(number()); }


	/// Convenience function: returns the name of the sample (if a sample is set, otherwise returns "[no sample]")
	QString sampleName() const;

	/// The string describing the format of the stored raw data file
	QString fileFormat() const { return metaData_.value("fileFormat").toString(); }
	/// The directory path and file name of this scan's raw data file
	QString filePath() const { return metaData_.value("filePath").toString(); }


	// Meta-data system
	/////////////////////////////////////////////


	/// Returns the value of a piece of meta data. (Re-implemented from AMDbObject to catch channelNames or channelExpressions, which aren't found in the metaData_ hash.)
	virtual QVariant metaData(const QString& key) const;

	/// set a meta data value. (Re-fieldsimplemented from AMDbObject to catch channelNames or channelExpressions, which are not allowed to be set as metaData like this.)
	virtual bool setMetaData(const QString& key, const QVariant& value);


	// Database system
	///////////////////////////////////////////
	/// Load yourself from the database. (returns true on success).  In addition to loading meta-data, this version checks to see if the new filePath() is different than the current one, and if it is, it automatically calls loadData().
	/*! Re-implemented from AMDbObject, this version loads all of the meta data found for keys metaDataAllKeys(), and also restores the chanel names and channel formulas.*/
	virtual bool loadFromDb(AMDatabase* db, int id);
	/// Store or update self in the database. (returns true on success)
	/*! Re-implemented from AMDbObject::storeToDb(), this version saves all of the meta data found for keys metaDataAllKeys(), as well as saving the channel names and channel formulas.
	  */
	virtual bool storeToDb(AMDatabase* db);


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




	// Data Tree Interface
	//////////////////////////////////

	/// the number of datapoints in the scan:
	unsigned count() const { return d_->count(); }

	/// Clear all of the raw data in the tree:
	void clear() { d_->clear(); }

	/// Get a list of all the data tree column names: (1D channels only)
	QStringList rawDataColumnNames() const {
		QStringList rv = d_->yColumnNames();
		rv.prepend(d_->xName());
		return rv;
	}


	// Thumbnail system:
	////////////////////////////////

	/// We can have a thumbnail for each channel
	int thumbnailCount() const {
		return numChannels();
	}

	/// Return a thumbnail picture of the channel
	AMDbThumbnail thumbnail(int index) const;



public slots:

	/// Sets appended number
	void setNumber(int number) { setMetaData("number", number);}
	/// set the date/time:
	void setDateTime(const QDateTime& dt) { setMetaData("dateTime", dt); }
	/// associate this object with a particular run. Set to (-1) to dissociate with any run.  (Note: for now, it's the caller's responsibility to make sure the runId is valid.)
	/*! This will also tell the new run (and the old run, if it exists) to update their date ranges */
	void setRunId(int newRunId) {
		if(newRunId <= 0) setMetaData("runId", QVariant());
		else setMetaData("runId", newRunId);
	}
	/// Sets name of sample
	void setSampleId(int newSampleId) {
		if(newSampleId <= 0) setMetaData("sampleId", QVariant());
		else setMetaData("sampleId", newSampleId);
	}
	/// Sets notes for scan
	void setNotes(const QString &notes) { setMetaData("notes", notes); }
	/// Set file path. (Be careful if changing this, not to break the association to a raw data file)
	void setFilePath(const QString& newPath) { setMetaData("filePath", newPath); }
	/// Set the file format. This is a string matching the AMAbstractFileLoader::formatTag() in one of the available file loaders.
	void setFileFormat(const QString& format) { setMetaData("fileFormat", format); }

	/// Test a possible channel expression for validity. The expression will be valid if it's an acceptable mathematical formula, involving valid raw data column names for variables.
	bool validateChannelExpression(const QString& expression);

	/// create a new channel. The channel will be owned and deleted by the scan.  Returns true on success. Will fail if there is an existing channel with the same name \c chName.  Will also fail if you turn \c ensureValid on, and the expression is invalid.
	bool addChannel(const QString& chName, const QString& expression, bool ensureValid = false);

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

	/// temporary; re-implement?
	QHash<QString, QVariant> metaData_;

	/// List of channels
	AMChannelListModel ch_;

	/// raw data storage. All scans will have one of these, but the contents and structure will vary.
	AMDataTree* d_;

	/// Allow channels to access the datatree:
	friend AMDataTree* AMChannel::dataTree() const;

	/// Allow channels to tell us when they (and hence us) have been modified:
	friend void AMChannel::informScanModified();

	/// Controls whether loadData() is called automatically inside loadFromDb().
	bool autoLoadData_;

	/// Caches the sample name
	mutable QString sampleName_;
	/// Status of sample name cache
	mutable bool sampleNameLoaded_;
	/// retrieves the sample name from the database, based on our sampleId. Sets sampleName_, and sets sampleNameLoaded_ = true;
	void retrieveSampleName() const;

	friend class AMAcqScanOutput;
	friend class AMAcqScanSpectrumOutput;
	friend class AMDacqScanController;
	friend class SGMXASDacqScanController;

private:

	/// This is used to maintain a reference count of 1 on the implicitly shared AMDataTree d_, and delete d_ when this (dshared_) goes out of scope
	QSharedDataPointer<AMDataTree> dshared_;

};

Q_DECLARE_METATYPE(AMScan*)


#endif // SCAN_H
