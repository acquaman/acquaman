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


#ifndef AMOLDDETECTORINFOSET_H
#define AMOLDDETECTORINFOSET_H

#include <QObject>
#include <dataman/info/AMOldDetectorInfo.h>
#include <util/AMOrderedSet.h>

#define AMDETECTORSET_CONTROL_TIMEOUT_MS 5000

#define AMDETECTORSET_NO_ENABLE_INFO_IN_DB 401001
#define AMDETECTORSET_ENABLE_INFO_COUNT_MISMATCH 401002

class AMOldDetectorInfoSet : public AMDbObject, public AMOrderedSet<QString, QPair<AMOldDetectorInfo*, bool> >
{
Q_OBJECT
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(AMDbObjectList detectorInfos READ dbReadDetectorInfos WRITE dbLoadDetectorInfos)
	Q_PROPERTY(QString activeDetectorInfos READ dbReadActiveDetectorInfos WRITE dbLoadActiveDetectorInfos)

public:
	explicit AMOldDetectorInfoSet(QObject *parent = 0);
	/// Loading-from-database constructor
	Q_INVOKABLE AMOldDetectorInfoSet(AMDatabase* db, int id);
	/// Copy constructor
	AMOldDetectorInfoSet(const AMOldDetectorInfoSet& other);
	/// Assignment operator
	AMOldDetectorInfoSet& operator=(const AMOldDetectorInfoSet& other);

	/// Destructor
	virtual ~AMOldDetectorInfoSet() {}

	friend QDebug operator<<(QDebug d, const AMOldDetectorInfoSet& dis);

	QString description();

	bool warningsSuppressed() const;

	/// Returns a list of pointers to the AMDetectorInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
	AMDbObjectList dbReadDetectorInfos();
	/// Called by the database system on loadFromDb() to give us our new set of AMDetectorlInfo objects. We copy these ones into our internal list and then delete them.
	void dbLoadDetectorInfos(const AMDbObjectList& newControlInfos);
	/// Returns a formatted string detailing with AMDetectorInfos are active, for use by the database system in storeToDb()
	QString dbReadActiveDetectorInfos();
	/// Called by the database system on loadFromDb() to tell us which detector infos were active.
	void dbLoadActiveDetectorInfos(const QString &activeDetectorInfos);


	/// Return the index of a given \c detectorInfo in the set. The comparison is done on the name() function returned by the detectorInfo passed into the function, not the pointer value. (Returns -1 if not found in the set.)
	int indexOf(AMOldDetectorInfo *detectorInfo) const;
	/// Return the index of the detectorInfo named \c detectorName. (Returns -1 if not found in the set.)
	int indexOf(const QString& detectorName) const;
	/// Returns the detectorInfo named \c detectorName. (A NULL pointer is returned if there is nothing by that name in the set.)
	AMOldDetectorInfo* detectorInfoNamed(const QString& detectorName);
	/* NTBA December 7, 2011 Darren Hunter
	  Based on research I am taking out the second const.  However, Dave still might want to test this.
	  const AMDetectorInfo* const detectorInfoNamed(const QString &detectorName) const;
	  */
	const AMOldDetectorInfo* detectorInfoNamed(const QString &detectorName) const;
	/// Returns the detectorInfo at the given index. (A NULL pointer is returned if the index is out of bounds.)
	AMOldDetectorInfo* detectorInfoAt(int index);
	// THIS WILL HAVE TO COPY AND CREATE A NEW ONE
	AMOldDetectorInfo* detectorInfoAt(int index) const;
	/* NTBA March 14, 2011 David Chevrier
	   Need to figure out these const conditions for returned pointers
	const AMDetectorInfo* const detectorInfoAt(int index) const;
	*/
	/// Returns whether or not the detectorInfo named \c detectorName has been requested for a scan.
	bool isActiveNamed(const QString& detectorName) const;
	/// Returns whether or not the \c detectorInfo has been requested for a scan. The comparison is done on the name() function returned by the detectorInfo passed into the function, not the pointer value.
	bool isActiveDetectorInfo(AMOldDetectorInfo *detectorInfo) const;
	/// Returns whether or not the detectorInfo at the given index has been requested for a scan.
	bool isActiveAt(int index) const;

	/// Adds an AMDetectorInfo to the detectorInfo set. Returns true if the addition was successful. Failure could result from adding an AMDetectorInfo when one with the same name is already in the set.
	bool addDetectorInfo(AMOldDetectorInfo *newDetectorInfo, bool isActive = false);

	/// Removes an AMDetectorInfo \c detectorInfo from the set. Returns true if the removal was successful. Failure could result from removing an AMDetectorInfo not in the set.
	bool removeDetector(AMOldDetectorInfo *detectorInfo);

	/// Changes whether or not the detectorInfo named \c detectorName has been requested for a scan.
	bool setActiveNamed(const QString& detectorName, bool active);
	/// Changes whether or not the \c detectorInfo has been requested for a scan. The comparison is done on the name() function returned by the detectorInfo passed into the function, not the pointer value.
	bool setActiveDetectorInfo(AMOldDetectorInfo *detectorInfo, bool active);
	/// Changes whether or not the detectorInfo at the given index has been requested for a scan.
	bool setActiveAt(int index, bool active);

	/// Returns a string with any warnings that occured during the load from database phase. Empty string implies no warnings.
	QString dbLoadWarnings() const;

public slots:
	void setDescription(const QString& description);

	void setWarningsSuppressed(bool warningsSuppressed);

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
	int indexOfValue(const AMOldDetectorInfo *detectorInfo) const;

protected:
	QString description_;

	/// Holds on to any warnings during the database load phase
	QString dbLoadWarnings_;

	bool warningsSuppressed_;
};

#endif // AMDETECTORINFOSET_H
