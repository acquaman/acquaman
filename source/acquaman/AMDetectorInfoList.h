#ifndef AMDETECTORINFOLIST_H
#define AMDETECTORINFOLIST_H

#include <QObject>
#include <beamline/AMDetector.h>
#include <util/AMOrderedList.h>
#include <util/AMOrderedSet.h>

class AMDetectorInfoSet : public AMDbObject, public AMOrderedList<AMDetectorInfo>
{
Q_OBJECT
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(AMDbObjectList detectorInfos READ dbReadDetectorInfos WRITE dbLoadDetectorInfos)

public:
	explicit AMDetectorInfoSet(QObject *parent = 0);
	/// Loading-from-database constructor
	Q_INVOKABLE AMDetectorInfoSet(AMDatabase* db, int id);
	/// Copy constructor
	AMDetectorInfoSet(const AMDetectorInfoSet& other);
	/// Assignment operator
	AMDetectorInfoSet& operator=(const AMDetectorInfoSet& other);

	/// Destructor
	~AMDetectorInfoSet() {}


	QString description();

	/// Returns a list of pointers to the AMDetectorInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
	AMDbObjectList dbReadDetectorInfos();
	/// Called by the database system on loadFromDb() to give us our new set of AMDetectorlInfo objects. We copy these ones into our internal list and then delete them.
	void dbLoadDetectorInfos(const AMDbObjectList& newControlInfos);

public slots:
	void setDescription(const QString& description);

signals:
	/// Forwarded from signalSource()->itemChanged(). Emitted when a detector is replaced, OR after a detector is accessed for modification and program execution returns back to the event loop.
	void detectorValuesChanged(int index);
	/// Forwarded from signalSource()->itemAdded(). Emitted after a new detector is added at \c index.
	void detectorAdded(int index);
	/// Forwarded from signalSource()->itemRemoved(). Emitted after a detector was removed (previously at \c index).
	void detectorRemoved(int index);

protected slots:
	/// Called when a control is accessed and potentially modified.
	void onControlValuesChanged(int index) {
		setModified(true);
		emit detectorValuesChanged(index);
	}
	/// Called after a control is added at \c index
	void onControlAdded(int index) {
		setModified(true);
		emit detectorAdded(index);
	}

	/// Called after a control is removed from \c index
	void onControlRemoved(int index) {
		setModified(true);
		emit detectorRemoved(index);
	}

protected:
	QString description_;

};

class AMDetectorSet : public QObject, public AMOrderedSet<QString, AMDetectorInfo*>
{
Q_OBJECT
public:
	/// Constructor
	explicit AMDetectorSet(QObject *parent = 0);

	/// Returns the name defined for the control set.
	QString name() const { return name_;}

	/*
	/// Converts all the controls to their simplified AMControlInfo form, and returns a list like this
	AMDetectorInfoSet toInfoSet() const;
	*/

	/*
	/// Returns true if ALL the controls in this set are connected.
	bool isConnected() const;
	*/

	/*
	/// Returns the names of the subset of controls which are not currently connected
	QStringList unconnected() const;
	*/

	/// Return the index of a given \c control in the set. You can then access the control using at() or operator[].  (Returns -1 if not found in the set.)
	int indexOf(AMDetectorInfo* detector) { return indexOfValue(detector); }
	/// Return the index of the control named \c controlName. (Returns -1 if not found in the set.)
	int indexOf(const QString& detectorName) { return indexOfKey(detectorName); }
	/// Returns the control named \c controlName, or 0 if not found in the set.
	AMDetectorInfo* detectorNamed(const QString& detectorName) {
		int index = indexOfKey(detectorName);
		if(index < 0)
			return 0;

		return at(index);
	}

	/// Adds an AMControl to the control set. Returns true if the addition was successful. Failure could result from adding the same AMControl twice.
	bool addDetector(AMDetectorInfo* newDetector) {
		if(!newDetector)
			return false;

		if( append(newDetector, newDetector->name()) ) {
			//connect(newDetector, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
			//connect(newControl, SIGNAL(valueChanged(double)), this, SLOT(onControlValueChanged()));
			return true;
		}
		return false;
	}

	/// Removes an AMControl \c control from the set. Returns true if the removal was successful. Failure could result from removing an AMControl not in the set.
	bool removeDetector(AMDetectorInfo* detector) {
		int index = indexOfValue(detector);
		if(index < 0)
			return false;

		disconnect(detector, 0, this, 0);
		remove(index);
		return true;
	}

	/*
	/// Checks to see if this control set can be set from the given AMControlInfoList (they refer to the same controls)
	bool validInfoSet(const AMDetectorInfoSet& info);

	/// Set the position of all the controls in the set from the simplified AMControlInfoList \c infoList.  The controls in \c infoList are matched by name, and for each corresponding name in this set, the real control's value is set.
	void setFromInfoSet(const AMDetectorInfoSet& info);
	*/


signals:
	void detectorAdded(int index);
	/*
	/// This signal is emitted whenever isConnected() changes
	void connected(bool groupConnected);
	void controlConnectedChanged(bool isConnected, AMControl *control);

	/// This signal is emitted whenever one of the controls has a new value
	void controlSetValuesChanged(AMControlInfoList);
	*/

public slots:
	/// Sets the name of the control set.
	void setName(const QString &name) { name_ = name; }


protected slots:
	/*
	/// Handles when any of the controls become connected or disconnected
	void onConnected(bool ctrlConnected);
	void onConnectionsTimedOut();

	/// Handles when any of the controls has a new value
	void onControlValueChanged();
	*/

protected:
	/// Holds the name of the control set. Should be descriptive of the logical relationship.
	/// AMControlSetView will use this value as the title of the group box being displayed.
	QString name_;


	/*
	/// Caches whether all the controls in this set were previously connected.
	bool wasConnected_;
	*/
};


class AMOldDetectorInfoSet : public QObject
{
Q_OBJECT
public:
	/// Constructor, only needs a QObject to act as a parent.
	explicit AMOldDetectorInfoSet(QObject *parent = 0);
	//AMDetectorInfoSet(AMDetectorInfoSet *copy, QObject *parent = 0);

	~AMOldDetectorInfoSet(){
		detectors_.clear();
	}

	/// Returns the name defined for the control set.
	QString name() const { return name_;}
	int count() { return detectors_.count();}
	AMDetectorInfo* detectorAt(int index) { return detectors_.at(index);}
	int indexOf(const QString &name);
	AMDetectorInfo* detectorByName(const QString &name);
	bool isDefaultAt(int index) { return defaultDetectors_.at(index);}
	bool isDefaultByName(const QString &name);

signals:

public slots:
	/// Sets the name of the control set.
	void setName(const QString &name) { name_ = name;}
	/// Adds an AMControl to the control set. Returns true if the addition was successful. Failure could result from adding the same AMControl twice.
	bool addDetector(AMDetectorInfo* detector, bool defaultDetector = false);
	/// Removes an AMControl from the control set. Returns true if the removal was successful. Failure could result from removing an AMControl not in the set.
	bool removeDetector(AMDetectorInfo* detector);

protected:
	/// Holds the name of the control set. Should be descriptive of the logical relationship.
	/// AMControlSetView will use this value as the title of the group box being displayed.
	QString name_;
	/// Local list of AMControl pointers, which represent the controls in the set.
	QList<AMDetectorInfo*> detectors_;
	QList<bool> defaultDetectors_;
};

#endif // AMDETECTORINFOLIST_H
