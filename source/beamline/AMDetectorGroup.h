/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMDETECTORGROUP_H
#define AMDETECTORGROUP_H

#include <QObject>

class AMDetector;
class AMDetectorSet;

#define AMDETECTORGROUP_UNEXPECTED_CONNECTED_CHANGED 470101
#define AMDETECTORGROUP_UNEXPECTED_UNCONNECTED_CHANGED 470102

class AMDetectorGroup : public QObject
{
Q_OBJECT
public:
	/// Constructor
 	virtual ~AMDetectorGroup();
	AMDetectorGroup(const QString &name = "Invalid Name", QObject *parent = 0);

	/// Returns the name for this detector set
	QString name() const;

	/// Returns true if all of the detectors are connected (the unconnectedSet is empty)
	bool allAreConnected() const;

	/// Returns true if this detector is in the group (either connected or unconnected)
	bool contains(AMDetector *detector) const;

	/// Returns the total number of detectors (both connected and unconnected)
	int count() const;

	/// Returns the detector pointer given the name. Convenience call to dig into the connectedDetectors() and unconnectedDetectors(). If no detector is found given the name, a NULL pointer is returned.
	AMDetector* detectorByName(const QString &name);

	/// Returns a pointer to the set of connected detectors. Since this is a pointer, detectors may migrate back and forth between the two sets, you can monitor this using the signals in AMDetectorSet.
	AMDetectorSet* connectedDetectors();
	/// Returns a pointer to the set of unconnected detectors. Since this is a pointer, detectors may migrate back and forth between the two sets, you can monitor this using the signals in AMDetectorSet.
	AMDetectorSet* unconnectedDetectors();

	/// Adds a detector to the group. Whether or not it is initially connected will determine which set it will end up in upon addition. Returns whether or not the detector was successfully added.
	bool addDetector(AMDetector *detector);
	/// Removes a detector form the group. Whether or not it is currently connected will determine which set it will be removed from. Returns whether or not the detector was successfully removed.
	bool removeDetector(AMDetector *detector);

public slots:
	/// Set the name of the group
	void setName(const QString &name);

signals:
	/// Emitted when a detector moves from the connected set to the unconnected set
	void detectorBecameUnconnected(AMDetector *detector);
	/// Emitted when a detector moves from the unconnected set to the connected set
	void detectorBecameConnected(AMDetector *detector);

	/// Emitted when the detector group has a status change from allConnected to not all connected (and vice versa)
	void allAreConnectedChanged(bool allAreConnected);

	/// Emitted when a detector is added to the group
	void detectorAddedToGroup(AMDetector *detector);
	/// Emitted when a detector is removed from the group
	void detectorRemovedFromGroup(AMDetector *detector);

	/// Emitted once all of the detectors have responded (even if that means one or more have timed out)
	void allDetectorsResponded();

protected slots:
	/// Handles moving detectors from the connected set to the unconnected set
	void onConnectedSetDetectorConnectedChanged(bool detectorConnected, AMDetector *detector);
	/// Handles moving detectors from the unconnected set to the connected set
	void onUnconnectedSetDetectorConnectedChanged(bool detectorConnected, AMDetector *detector);

	/// Handles checking the internal connected state and setting the cached lastAllAreConnected value
	void checkAllAreConnected();

protected:
	/// Holds the programmer name of the group
	QString name_;
	/// Holds the connected detectors. Detectors may migrate between the two lists depending on their connected state.
	AMDetectorSet *connectedSet_;
	/// Holds the unconnected detectors. Detectors may migrate between the two lists depending on their connected state.
	AMDetectorSet *unconnectedSet_;

	/// Holds a cached state of whether all detectors were connected before the last connection change
	bool lastAllAreConnected_;
};

#endif // AMDETECTORGROUP_H
