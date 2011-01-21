/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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

#include "dataman/AMDbObject.h"
#include "util/AMOrderedList.h"

/// This class can be used to store the essential information and state of an AMControl, without actually needing to have a live control object. This information includes the name, units, value, minimum value, and maximum value.
class AMControlInfo : public AMDbObject {


	Q_OBJECT

	Q_PROPERTY(double value READ value WRITE setValue)
	Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
	Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
	Q_PROPERTY(QString units READ units WRITE setUnits)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Saved Control State")


public:
	AMControlInfo(const QString& name, double value, double minimum, double maximum, const QString& units, QObject* parent = 0);
	Q_INVOKABLE AMControlInfo(AMDatabase* db, int id);

	// QString name() const { return name_; }
	double value() const { return value_; }
	double minimum() const { return minimum_; }
	double maximum() const { return maximum_; }
	QString units() const { return units_; }

public slots:
	// void setName(const QString& name) { name_ = name; }
	void setValue(double value) { value_ = value; }
	void setMinimum(double minimum) { minimum_ = minimum; }
	void setMaximum(double maximum) { maximum_ = maximum; }
	void setUnits(const QString &units) { units_ = units; }

protected:
	// QString name_;
	double value_;
	double minimum_;
	double maximum_;
	QString units_;
};


/// This is a container class for AMControlInfo, most commonly used to represent an arbitrary set of control positions.
class AMControlInfoList : public AMDbObject, public AMOrderedList<AMControlInfo>
{
Q_OBJECT
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(AMDbObjectList controlInfos READ dbReadControlInfos WRITE dbLoadControlInfos)


public:
	/// Default constructor
	explicit AMControlInfoList(QObject *parent = 0);
	/// Loading-from-database constructor
	Q_INVOKABLE AMControlInfoList(AMDatabase* db, int id);
	/// Copy constructor
	AMControlInfoList(const AMControlInfoList& other);
	/// Assignment operator
	AMControlInfoList& operator=(const AMControlInfoList& other);

	/// Destructor
	~AMControlInfoList() {}

	/// A human-readable description for this set of controls
	QString description() const { return description_; }


	// Support for saving / restoring an AMControlInfoSet to the database
	////////////////////////////////

	/// Returns a list of pointers to the AMControlInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
	AMDbObjectList dbReadControlInfos() {
		AMDbObjectList rv;
		for(int i=0; i<count(); i++)
			rv << &((*this)[i]);
		return rv;
	}

	/// Called by the database system on loadFromDb() to give us our new set of AMControlInfo objects. We copy these ones into our internal list and then delete them.
	void dbLoadControlInfos(const AMDbObjectList& newControlInfos) {
		clear();	// get rid of our existing

		for(int i=0; i<newControlInfos.count(); i++) {
			AMControlInfo* newControlInfo = qobject_cast<AMControlInfo*>(newControlInfos.at(i));
			if(newControlInfo) {
				append(*newControlInfo);	// note: makes a copy of object pointed to by newControlInfo, and stores in our internal list.
			}

			if(newControlInfos.at(i))
				delete newControlInfos.at(i);	// we're copying these; don't need to keep these ones around. Our responsibility to delete.
		}
	}


signals:
	/// Forwarded from signalSource()->itemChanged(). Emitted when a control is replaced, OR after a control is accessed for modification and program execution returns back to the event loop.
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
	/// Called when a control is accessed and potentially modified.
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
