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


#ifndef AMDETECTORINFOSET_H
#define AMDETECTORINFOSET_H

#include <util/AMOrderedSet.h>
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorInfoSet : public AMDbObject, public AMOrderedSet<QString, AMDetectorInfo>
{
Q_OBJECT

Q_PROPERTY(QString description READ description WRITE setDescription)
Q_PROPERTY(AMDbObjectList detectorInfos READ dbReadDetectorInfos WRITE dbLoadDetectorInfos)

public:
	/// Constructor
 	virtual ~AMDetectorInfoSet();
	explicit AMDetectorInfoSet(QObject *parent = 0);
	/// Database loading constructor
	Q_INVOKABLE AMDetectorInfoSet(AMDatabase *db, int id);
	/// Copy Constructor
	AMDetectorInfoSet(const AMDetectorInfoSet &other);
	/// Assignment operator
	AMDetectorInfoSet& operator=(const AMDetectorInfoSet &other);

	/// Comparisson operator. Defined to return true if the two sets contain the same number of infos, with the same names. We ARE NOT checking the values right now, just comparing the lists for the same items.
	bool operator==(const AMDetectorInfoSet &other);
	/// Inequality operator. Returns the opposite of the comparisson operator.
	bool operator!=(const AMDetectorInfoSet &other);

	/// Returns the user readable description
	QString description() const;

	/// Returns a list of the AMDetectorInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
	AMDbObjectList dbReadDetectorInfos();
	/// Called by the database system on loadFromDb() to give us our new set of AMDetectorlInfo objects.
	void dbLoadDetectorInfos(const AMDbObjectList& newDetectorInfos);

	/// Return the index of a given \c detectorInfo in the set. The comparison is done on the name() function. (Returns -1 if not found in the set.)
	int indexOf(const AMDetectorInfo &detectorInfo) const;
	/// Return the index of the detectorInfo named \c detectorName. (Returns -1 if not found in the set.)
	int indexOf(const QString& detectorName) const;
	/// Returns the detectorInfo named \c detectorName. (A default detector info is return if not found but this object will not last beyond the call you make, please make sure to request valid detectors)
	AMDetectorInfo& detectorInfoNamed(const QString& detectorName);
	/// Returns the detectorInfo at the given index. (A default detector info is return if not found but this object will not last beyond the call you make, please make sure to request valid detectors)
	AMDetectorInfo& detectorInfoAt(int index);

	/// Adds an AMDetectorInfo to the detectorInfo set. Returns true if the addition was successful. Failure could result from adding an AMDetectorInfo when one with the same name is already in the set.
	bool addDetectorInfo(const AMDetectorInfo &newDetectorInfo);

	/// Removes an AMDetectorInfo \c detectorInfo from the set. Returns true if the removal was successful. Failure could result from removing an AMDetectorInfo not in the set.
	bool removeDetector(const AMDetectorInfo &detectorInfo);

public slots:
	/// Set the user readable description
	void setDescription(const QString& description);

signals:
	/// Forwarded from signalSource()->itemAdded(). Emitted after a new detector is added at \c index.
	void detectorInfoAdded(int index);
	/// Forwarded from signalSource()->itemRemoved(). Emitted after a detector was removed (previously at \c index).
	void detectorInfoRemoved(int index);

protected slots:
	/// Called after a detector is added at \c index
	void onDetectorInfoAdded(int index);
	/// Called after a detector is removed from \c index
	void onDetectorInfoRemoved(int index);

protected:
	/// User readable description
	QString description_;
};

#endif // AMDETECTORINFOSET_H
