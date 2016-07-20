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


#ifndef AMCONTROLSETINFO_H
#define AMCONTROLSETINFO_H

#include <QDebug>

#include "dataman/info/AMControlInfo.h"
#include "util/AMOrderedList.h"

/// This is a container class for AMControlInfo, most commonly used to represent an arbitrary set of control positions.  It is an AMDbObject subclass that can be stored to the database, but it's also a lightweight class that can be copied and passed around by value.
/*! Note that the assignment operator and copy constructor copy the other object's <i>database identity</i> as well as its values; these are a "shallow copy" from the perspective of the database: both source and destination will point to the same location in the database, although they are separate objects in C++ memory. If you want to copy all the values from another AMControlInfoList, but preserve your database identity, use setValuesFrom() instead. For more information, see setValuesFrom(). */
class AMControlInfoList : public AMDbObject, public AMOrderedList<AMControlInfo>
{
Q_OBJECT
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(AMDbObjectList controlInfos READ dbReadControlInfos WRITE dbLoadControlInfos)

	Q_CLASSINFO("AMDbObject_Attributes", "description=List of Saved Control States")

public:
	/// Default constructor
	Q_INVOKABLE explicit AMControlInfoList(QObject *parent = 0);

	/// Copy constructor. Note that this copies the values from the \c other list, but it also copies over the database identify (ie: id(), database(), and modified() state) of \c other.
	AMControlInfoList(const AMControlInfoList& other);
	/// Assignment operator.  Note that this copies the values from the \c other list, but it also copies over the database identify (ie: id(), database(), and modified() state) of \c other.  Saving with storeToDb() will now save to \c other's database location.  If you want to copy the values but retain your old database identity, call setValuesFrom().
	AMControlInfoList& operator=(const AMControlInfoList& other);

	/// Comparison operator. Returns true if and only if the values from the \c other list match the list of values in this within the tolerance of the
	bool operator==(const AMControlInfoList &other) const;

	/// Inequality operator. Returns !==
	bool operator!=(const AMControlInfoList &other) const;

	/// Compares this list to another with a list of alternate tolerances for the controls in the list. The lists must have the same items, in the same order, and the tolerances must be in the same order.
	bool compareWithinTolerance(const AMControlInfoList &other, QList<double> tolerances) const;

	/// Destructor
	virtual ~AMControlInfoList() {}

	/// Allows you to push a ControlInfoList into a QDebug stream
	friend QDebug operator<<(QDebug d, const AMControlInfoList& cil);

	/// A human-readable description for this set of controls
	QString description() const { return description_; }


	// Support for saving / restoring an AMControlInfoSet to the database
	////////////////////////////////

	/// Returns a list of pointers to the AMControlInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
	AMDbObjectList dbReadControlInfos();
	/// Called by the database system on loadFromDb() to give us our new set of AMControlInfo objects. We copy these ones into our internal list and then delete them.
	void dbLoadControlInfos(const AMDbObjectList& newControlInfos);


	/// This is just like the assignment operator, but it retains the existing database identity of ourself (and any existing AMControlInfo's we have).  For example, id() and database() will return what they did before calling this, but our AMControlInfo values will be set to match \c other.
	/*! If \c other has more AMControlInfo elements than we do, we will call setFromValues() on the number that we have in common, and create new (independent) AMControlInfo items for the rest.

If we have more AMControlInfo objects than \c other, we'll orphan our extras... (\todo These should be removed from the db somehow... possibly in a new storeToDb() that takes care of deleting orphans (at AMDbObject level).

Either way, using setFromValues() instead of the assignment operator means that all of our AMControlInfo items will have separate database identities from \c other's items.*/
	void setValuesFrom(const AMControlInfoList& other);

	/// Returns true if the list has a control info with the given name, false otherwise.
	bool hasControl(const QString &controlName) const;
	/// Find the index of a control in the list by name. Returns -1 if not found.
	int indexOf(const QString& controlName) const;
	/// Shortcut: returns a reference to a control by name. Equivalent to myControlInfoList[myControlInfoList.indexOf("name")].  \warning Assumes that a control with this name exists! Otherwise, this will crash.
	AMControlInfo& controlNamed(const QString& controlName) { return (*this)[this->indexOf(controlName)]; }
	/// Shortcut: returns a reference to a control by name. Equivalent to myControlInfoList.at(myControlInfoList.indexOf("name")).  \warning Assumes that a control with this name exists! Otherwise, this will crash.
	const AMControlInfo& controlNamed(const QString& controlName) const { return this->at(this->indexOf(controlName)); }


signals:
	/// Forwarded from signalSource()->itemChanged(). Emitted when a control is replaced, OR after a control is accessed for modification using operator[]() and program execution returns back to the event loop.
	void controlValuesChanged(int index);
	/// Forwarded from signalSource()->itemAdded(). Emitted after a new control is added at \c index.
	void controlAdded(int index);
	/// Forwarded from signalSource()->itemRemoved(). Emitted after a control was removed (previously at \c index).
	void controlRemoved(int index);

public slots:
	/// Set the human-readable description
	void setDescription(const QString& description) {
		description_ = description;
		setModified(true);
	}


protected slots:
	/// Called when a control is accessed and potentially modified.  This will be emitted after any non-const operator[] access is used, but only after control returns back to the event loop.
	void onControlValuesChanged(int index) {
		setModified(true);
		emit controlValuesChanged(index);
	}
	/// Called after a control is added at \c index
	void onControlAdded(int index) {
		setModified(true);
		emit controlAdded(index);
	}

	/// Called after a control is removed from \c index
	void onControlRemoved(int index) {
		setModified(true);
		emit controlRemoved(index);
	}

protected:
	QString description_;

};


#endif // AMCONTROLSETINFO_H
