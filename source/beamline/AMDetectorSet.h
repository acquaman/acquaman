#ifndef AMDETECTORSET_H
#define AMDETECTORSET_H

#define AMDETECTORSET_DETECTOR_TIMEOUT_MS 4000

#include "beamline/AMDetector.h"
#include "util/AMOrderedSet.h"

class AMDetectorInfoSet;

class AMDetectorSet : public QObject, public AMOrderedSet<QString, AMDetector*>
{
Q_OBJECT
public:
	/// Constructor
	explicit AMDetectorSet(QObject *parent = 0);

	/// Returns the name for this detector set
	QString name() const;

	/// Converts all of the detectors to detector infos and returns an AMDetectorInfoSet
	AMDetectorInfoSet toInfoSet() const;

	/// Returns true if all the detectors in this set are connected
	bool isConnnected() const;
	/// Returns the names of the detectors that are not connected
	QStringList unconnectedDetectors() const;

	/// Return the index of a given \c detector in the set. Returns -1 if not found.
	int indexOf(AMDetector *detector) const;
	/// Returns the index of the detector named \c detectorName. Returns -1 if not found.
	int indexOf(const QString &detectorName) const;
	/// Returns a pointer to the detector named \c detectorName, or 0 (NULL) if not found.
	AMDetector* detectorNamed(const QString &detectorName);
	/// Returns a pointer to the detector as index \c index, or 0 (NULL) if not found.
	AMDetector* detectorAt(int index);

	/// Adds an AMDetector to the detector set. Returns true if the addition was successful. Failure could result from adding the same AMDetector twice.
	bool addDetector(AMDetector *detector);
	/// Removes an AMDetector from the detector set. Returns true if the removal was successful. Failure could result from removing a detector that is not in the set.
	bool removeDetector(AMDetector *detector);

	/// Checks to see if this detector set can be set from the given AMDetectorInfoSet (they refer to the same detectors)
	bool validInfoSet(const AMDetectorInfoSet &infoSet);
	/*
	/// Sets the detector values in this set from the appropriate detector infos.
	void setFromInfoSet(const AMDetectorInfoSet &infoSet);
	*/

signals:
	/// Emitted when a detector is added with the new index
	void detectorAdded(int index);
	/// Emitted when a detector is removed with its former index
	void detectorRemoved(int index);

	/// Emitted whenever the isConnected() parameter changes
	void connected(bool isConnected);
	/// Emitted when the connection state for a detector in the set changes
	void detectorConnectedChanged(bool isConnected, AMDetector *detector);
	/// Emitted if any of the detectors in this set have timed out
	void detectorSetTimedOut();

public slots:
	/// Sets the name of the detector set
	void setName(const QString &name);

protected slots:
	/// Handles when any of the detectors becomes connected or disconnected
	void onConnectedChanged(bool detectorConnected);
	/// Handles timeouts from any of the detectors
	void onTimedOut();

protected:
	/// Holds the programmer name of the detector set
	QString name_;

	/// Caches whether all of the detectors in this set were previously connected
	bool wasConnected_;
};

#endif // AMDETECTORSET_H
