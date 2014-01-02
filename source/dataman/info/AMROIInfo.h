/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMROIINFO_H
#define AMROIINFO_H

#include "dataman/database/AMDbObject.h"
#include "util/AMOrderedList.h"

/// This class can be used to store the essential information of an AMROI that is used for spectral detectors (ex: Silicon Drift Detectors), without actually needing to have a live control object. This information includes the the name, centre energy and width of the region.
class AMROIInfo : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(double energy READ energy WRITE setEnergy)
	Q_PROPERTY(double low READ low WRITE setLow)
	Q_PROPERTY(double high READ high WRITE setHigh)
	Q_PROPERTY(double scale READ scale WRITE setScale)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Saved ROI State")

public:
	/// Constructor.  Takes in the name, centre energy, lower bound, upper bound, and the scale of the region.
	Q_INVOKABLE AMROIInfo(const QString& name = "Invalid ROI Info", double energy = 0.0, double low = 0.0, double high = 0.0, double scale = 0.0, QObject *parent = 0);
	/// Convenience constructor.  Uses a width to to auto-create a low and high value.  This does not have default values. \note The order of the constructor had to be different so that the compiler doesn't complain.  This is because the default constructor has assigned values for all parameters.
	AMROIInfo(double energy, double width, double scale, const QString& name, QObject *parent = 0);

	/// Returns the centre energy of the region of interest.  Generally will be the known energy of a particular emission line.
	double energy() const { return energy_; }
	/// Returns the low bound of the region of interest.  This will generally be calculated, but if it is different then the calculation based on width, it will overide the width because it is a customized value.
	double low() const { return low_; }
	/// Returns the high bound of the region of interest.  This will generally be calculated, but if it is different then the calculation based on width, it will overide the width because it is a customized value.
	double high() const { return high_; }
	/// Returns the scale that would transform an energy and width into upper and lower bounds for a detector.
	double scale() const { return scale_; }
	/// The default copy constructor and assignment operator will copy the values from \c other, but they will also copy over the database identity (ie: id(), database(), modified() state, etc.).  This means that calling storeToDb() will now save to \c other's database location.  If you want to copy the values but retain your old database identity, call this function instead.
	void setValuesFrom(const AMROIInfo& other)
	{
		energy_ = other.energy_;
		low_ = other.low_;
		high_ = other.high_;
		scale_ = other.scale_;
		setName(other.name());	// will take care of calling setModified().
	}

public slots:
	/// Sets the centre energy of the region of interest.
	void setEnergy(double energy) { energy_ = energy; setModified(true); }
	/// Sets the low bound for the region of interest.
	void setLow(double low) { low_ = low; setModified(true); }
	/// Sets the high bound for the region of interest.
	void setHigh(double high) { high_ = high; setModified(true); }
	/// Sets the scal of the region of interest.
	void setScale(double scale) { scale_ = scale; setModified(true); }

protected:
	/// The central energy of the region of interest.  Generally will be the known energy of a particular emission line.
	double energy_;
	/// The low bound of the region of interest.
	double low_;
	/// The high bound of the region of interest.
	double high_;
	/// The scale that would convert an energy and width into upper and lower bounds for a detector.
	double scale_;
};

/// This is a container class for AMROIInfo, most commonly used to represent an arbitrary set of ROI positions.  It is an AMDbObject subclass that can be stored to the database, but it's also a lightweight class that can be copied and passed around by value.
/*! Note that the assignment operator and copy constructor copy the other object's <i>database identity</i> as well as its values; these are a "shallow copy" from the perspective of the database: both source and destination will point to the same location in the database, although they are separate objects in C++ memory. If you want to copy all the values from another AMROIInfoList, but preserve your database identity, use setValuesFrom() instead. For more information, see setValuesFrom(). */
class AMROIInfoList : public AMDbObject, public AMOrderedList<AMROIInfo>
{
Q_OBJECT
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(AMDbObjectList roiInfos READ dbReadROIInfos WRITE dbLoadROIInfos)


public:
	/// Default constructor
	Q_INVOKABLE explicit AMROIInfoList(QObject *parent = 0);

	/// Copy constructor. Note that this copies the values from the \c other list, but it also copies over the database identify (ie: id(), database(), and modified() state) of \c other.
	AMROIInfoList(const AMROIInfoList& other);
	/// Assignment operator.  Note that this copies the values from the \c other list, but it also copies over the database identify (ie: id(), database(), and modified() state) of \c other.  Saving with storeToDb() will now save to \c other's database location.  If you want to copy the values but retain your old database identity, call setValuesFrom().
	AMROIInfoList& operator=(const AMROIInfoList& other);

	/// Destructor
	virtual ~AMROIInfoList() {}

	/// A human-readable description for this set of ROIs
	QString description() const { return description_; }

	/// Sorts the current list by energy from lowest to highest.
	void sort();

	/// Convenience function that gives the index of an AMROIInfo using the name.  This means that having an instance of an AMROIInfo is necessary to find particular information inside the list.  The name was chosen as the finding parameter.  Returns the first instance inside the list and doesn't continue looking for repeats.  Returns -1 if the result if no match is found.
	int indexOf(QString name);

	// Support for saving / restoring an AMROIInfoSet to the database
	////////////////////////////////

	/// Returns a list of pointers to the AMROIInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
	AMDbObjectList dbReadROIInfos();
	/// Called by the database system on loadFromDb() to give us our new set of AMROIInfo objects. We copy these ones into our internal list and then delete them.
	void dbLoadROIInfos(const AMDbObjectList& newROIInfos);


	/// This is just like the assignment operator, but it retains the existing database identity of ourself (and any existing AMROIInfo's we have).  For example, id() and database() will return what they did before calling this, but our AMROIInfo values will be set to match \c other.
	/*! If \c other has more AMROIInfo elements than we do, we will call setFromValues() on the number that we have in common, and create new (independent) AMROIInfo items for the rest.

	If we have more AMROIInfo objects than \c other, we'll orphan our extras... (\todo These should be removed from the db somehow... possibly in a new storeToDb() that takes care of deleting orphans (at AMDbObject level).

	Either way, using setFromValues() instead of the assignment operator means that all of our AMROIInfo items will have separate database identities from \c other's items.*/
	void setValuesFrom(const AMROIInfoList& other);


signals:
	/// Forwarded from signalSource()->itemChanged(). Emitted when a ROI is replaced, OR after a ROI is accessed for modification using operator[]() and program execution returns back to the event loop.
	void roiValuesChanged(int index);
	/// Forwarded from signalSource()->itemAdded(). Emitted after a new ROI is added at \c index.
	void roiAdded(int index);
	/// Forwarded from signalSource()->itemRemoved(). Emitted after a ROI was removed (previously at \c index).
	void roiRemoved(int index);

public slots:
	/// Set the human-readable description
	void setDescription(const QString& description) {
		description_ = description;
		setModified(true);
	}


protected slots:
	/// Called when a ROI is accessed and potentially modified.  This will be emitted after any non-const operator[] access is used, but only after ROI returns back to the event loop.
	void onROIValuesChanged(int index) {
		setModified(true);
		emit roiValuesChanged(index);
	}
	/// Called after a ROI is added at \c index
	void onROIAdded(int index) {
		setModified(true);
		emit roiAdded(index);
	}

	/// Called after a ROI is removed from \c index
	void onROIRemoved(int index) {
		setModified(true);
		emit roiRemoved(index);
	}

protected:
	QString description_;

};

#endif // AMROIINFO_H
