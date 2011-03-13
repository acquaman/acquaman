#ifndef AMDETECTORINFOLIST_H
#define AMDETECTORINFOLIST_H

#include <QObject>
#include <beamline/AMDetector.h>
#include <util/AMOrderedList.h>
#include <util/AMOrderedSet.h>

#define AMDETECTORSET_CONTROL_TIMEOUT_MS 5000

class AMDetectorInfoSet : public AMDbObject, public AMOrderedSet<QString, QPair<AMDetectorInfo*, bool> >
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

	friend QDebug operator<<(QDebug d, const AMDetectorInfoSet& dis);

	QString description();

	/// Returns a list of pointers to the AMDetectorInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
	AMDbObjectList dbReadDetectorInfos();
	/// Called by the database system on loadFromDb() to give us our new set of AMDetectorlInfo objects. We copy these ones into our internal list and then delete them.
	void dbLoadDetectorInfos(const AMDbObjectList& newControlInfos);

	/// Return the index of a given \c detectorInfo in the set. The comparison is done on the name() function returned by the detectorInfo passed into the function, not the pointer value. (Returns -1 if not found in the set.)
	int indexOf(AMDetectorInfo *detectorInfo) const;
	/// Return the index of the detectorInfo named \c detectorName. (Returns -1 if not found in the set.)
	int indexOf(const QString& detectorName) const;
	/// Returns the detectorInfo named \c detectorName. (A NULL pointer is returned if there is nothing by that name in the set.)
	AMDetectorInfo* detectorInfoNamed(const QString& detectorName);
	const AMDetectorInfo* const detectorInfoNamed(const QString &detectorName) const;
	/// Returns the detectorInfo at the given index. (A NULL pointer is returned if the index is out of bounds.)
	AMDetectorInfo* detectorInfoAt(int index);
	// THIS WILL HAVE TO COPY AND CREATE A NEW ONE
	AMDetectorInfo* detectorInfoAt(int index) const;
	//const AMDetectorInfo* const detectorInfoAt(int index) const;
	/// Returns whether or not the detectorInfo named \c detectorName has been requested for a scan.
	bool isActiveNamed(const QString& detectorName) const;
	/// Returns whether or not the \c detectorInfo has been requested for a scan. The comparison is done on the name() function returned by the detectorInfo passed into the function, not the pointer value.
	bool isActiveDetectorInfo(AMDetectorInfo *detectorInfo) const;
	/// Returns whether or not the detectorInfo at the given index has been requested for a scan.
	bool isActiveAt(int index) const;

	/// Adds an AMDetectorInfo to the detectorInfo set. Returns true if the addition was successful. Failure could result from adding an AMDetectorInfo when one with the same name is already in the set.
	bool addDetectorInfo(AMDetectorInfo *newDetectorInfo, bool isActive = false);

	/// Removes an AMDetectorInfo \c detectorInfo from the set. Returns true if the removal was successful. Failure could result from removing an AMDetectorInfo not in the set.
	bool removeDetector(AMDetectorInfo *detectorInfo);

	/// Changes whether or not the detectorInfo named \c detectorName has been requested for a scan.
	bool setActiveNamed(const QString& detectorName, bool active);
	/// Changes whether or not the \c detectorInfo has been requested for a scan. The comparison is done on the name() function returned by the detectorInfo passed into the function, not the pointer value.
	bool setActiveDetectorInfo(AMDetectorInfo *detectorInfo, bool active);
	/// Changes whether or not the detectorInfo at the given index has been requested for a scan.
	bool setActiveAt(int index, bool active);

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
	/// Called when a detector is accessed and potentially modified.
	void onDetectorValuesChanged(int index);
	/// Called after a detector is added at \c index
	void onDetectorAdded(int index);
	/// Called after a detector is removed from \c index
	void onDetectorRemoved(int index);

protected:
	int indexOfValue(const AMDetectorInfo *detectorInfo) const;

protected:
	QString description_;

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
