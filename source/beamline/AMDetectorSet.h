#ifndef AMDETECTORSET_H
#define AMDETECTORSET_H

#include "acquaman/AMDetectorInfoList.h"

class AMDetectorSet : public QObject, public AMOrderedSet<QString, AMDetector*>
{
Q_OBJECT
public:
	/// Constructor
	explicit AMDetectorSet(QObject *parent = 0);

	/// Returns the name defined for the control set.
	QString name() const;

	/// Converts all the controls to their simplified AMControlInfo form, and returns a list like this
	AMDetectorInfoSet toInfoSet() const;

	/// Returns true if ALL the controls in this set are connected.
	bool isConnected() const;

	/// Returns the names of the subset of detectors which are not currently connected
	QStringList unconnected() const;

	/// Return the index of a given \c control in the set. You can then access the control using at() or operator[].  (Returns -1 if not found in the set.)
	int indexOf(AMDetector* detector) ;
	/// Return the index of the control named \c controlName. (Returns -1 if not found in the set.)
	int indexOf(const QString& detectorName);
	/// Returns the control named \c controlName, or 0 if not found in the set.
	AMDetector* detectorNamed(const QString& detectorName);

	/// Adds an AMControl to the control set. Returns true if the addition was successful. Failure could result from adding the same AMControl twice.
	bool addDetector(AMDetector* newDetector);

	/// Removes an AMControl \c control from the set. Returns true if the removal was successful. Failure could result from removing an AMControl not in the set.
	bool removeDetector(AMDetector* detector);

	/// Checks to see if this detector set can be set from the given AMDetectorInfoSet (they refer to the same detectors)
	bool validInfoSet(const AMDetectorInfoSet& info);

	/// Set the position of all the detectors in the set from the simplified AMDetectorInfoSet \c infoList.  The detectors in \c infoList are matched by name, and for each corresponding name in this set, the real control's value is set.
	void setFromInfoSet(const AMDetectorInfoSet& info);


signals:
	void detectorAdded(int index);

	/// This signal is emitted whenever isConnected() changes
	void connected(bool groupConnected);
	void detectorConnectedChanged(bool isConnected, AMDetector *detector);

	/// This signal is emitted whenever one of the controls has a new value
	void detectorSetValuesChanged(AMDetectorInfoSet);

public slots:
	/// Sets the name of the control set.
	void setName(const QString &name) { name_ = name; }


protected slots:
	/// Handles when any of the detectors become connected or disconnected
	void onConnected(bool detectorConnected);
	void onConnectionsTimedOut();

	/// Handles when any of the detectors has a new value
	void onDetectorValueChanged();

protected:
	/// Holds the name of the control set. Should be descriptive of the logical relationship.
	/// AMControlSetView will use this value as the title of the group box being displayed.
	QString name_;


	/// Caches whether all the detectors in this set were previously connected.
	bool wasConnected_;
};


#endif // AMDETECTORSET_H
