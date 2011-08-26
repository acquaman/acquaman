/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMDETECTORSET_H
#define AMDETECTORSET_H

#include "dataman/AMDetectorInfoList.h"

class AMDetectorSet : public QObject, public AMOrderedSet<QString, QPair<AMDetector*, bool> >
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

	/// Return the index of a given \c detector in the set. You can then access the control using at() or operator[].  (Returns -1 if not found in the set.)
	int indexOf(AMDetector* detector) const;
	/// Return the index of the detector named \c detectorName. (Returns -1 if not found in the set.)
	int indexOf(const QString& detectorName) const;
	/// Returns the detector named \c detectorName, or 0 if not found in the set.
	AMDetector* detectorNamed(const QString& detectorName);
	/// Returns the detector at the index specified
	AMDetector* detectorAt(int index);
	/// Returns whether or not the detector named \c detectorName has been defined as default for this set (If no detector by that name is in the set then false will be returned)
	bool isDefaultNamed(const QString& detectorName) const;
	/// Returns whether or not the AMDetector has been defined as default for this set (If the AMDetector is not in the set then false will be returned)
	bool isDefaultDetector(AMDetector *detector) const;
	/// Returns whether or not the detector at the specified index has been defined as default for this set
	bool isDefaultAt(int index) const;

	/// Adds an AMDetector to the detector set. Returns true if the addition was successful. Failure could result from adding the same AMControl twice.
	bool addDetector(AMDetector* newDetector, bool isDefault = false);

	/// Removes an AMControl \c control from the set. Returns true if the removal was successful. Failure could result from removing an AMControl not in the set.
	bool removeDetector(AMDetector* detector);

	/// Checks to see if this detector set can be set from the given AMDetectorInfoSet (they refer to the same detectors)
	bool validInfoSet(const AMDetectorInfoSet& info);

	/// Set the position of all the detectors in the set from the simplified AMDetectorInfoSet \c infoList.  The detectors in \c infoList are matched by name, and for each corresponding name in this set, the real control's value is set.
	void setFromInfoSet(const AMDetectorInfoSet& info);


signals:
	void detectorAdded(int index);
	void detectorRemoved(int index);

	/// This signal is emitted whenever isConnected() changes
	void connected(bool groupConnected);
	void detectorConnectedChanged(bool isConnected, AMDetector *detector);

	void detectorSetReadingsChanged();
	/// This signal is emitted whenever one of the controls has new settings
	void detectorSetSettingsChanged();

public slots:
	/// Sets the name of the control set.
	void setName(const QString &name) { name_ = name; }


protected slots:
	/// Handles when any of the detectors become connected or disconnected
	void onConnected(bool detectorConnected);
	void onConnectionsTimedOut();

protected:
	/// Holds the name of the control set. Should be descriptive of the logical relationship.
	/// AMControlSetView will use this value as the title of the group box being displayed.
	QString name_;


	/// Caches whether all the detectors in this set were previously connected.
	bool wasConnected_;
};


#endif // AMDETECTORSET_H
