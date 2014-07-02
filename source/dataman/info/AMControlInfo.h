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

#ifndef AMCONTROLINFO_H
#define AMCONTROLINFO_H

#include "dataman/database/AMDbObject.h"

/// This class can be used to store the essential information and state of an AMControl, without actually needing to have a live control object. This information includes the name, units, value, minimum value, and maximum value.
class AMControlInfo : public AMDbObject {


	Q_OBJECT

	Q_PROPERTY(double value READ value WRITE setValue)
	Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
	Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
	Q_PROPERTY(QString units READ units WRITE setUnits)
	Q_PROPERTY(double tolerance READ tolerance WRITE setTolerance)
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(QString contextKnownDescription READ contextKnownDescription WRITE setContextKnownDescription)
	Q_PROPERTY(QString enumString READ enumString WRITE setEnumString)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Saved Control State")


public:
 	virtual ~AMControlInfo();
	Q_INVOKABLE AMControlInfo(const QString& name = "Invalid Control", double value = 0.0, double minimum = 0.0, double maximum = 0.0, const QString& units = "", double tolerance = 0.0, const QString &description = "", const QString &contextKnownDescription = "", const QString& enumString = QString(), QObject* parent = 0);

	double value() const { return value_; }
	double minimum() const { return minimum_; }
	double maximum() const { return maximum_; }
	QString units() const { return units_; }
	double tolerance() const { return tolerance_; }
	QString description() const { return description_; }
	QString contextKnownDescription() const { return contextKnownDescription_; }
	/// Returns true if this was derived from an "enum" control with discrete states; it means that enumString() contains the state corresponding to value().
	bool isEnum() const { return !enumString_.isEmpty(); }
	/// If this info was derived from an "enum" control with discrete states, this will contain the state that corresponds to value().
	QString enumString() const { return enumString_; }

	/// The default copy constructor and assignment operator will copy the values from \c other, but they will also copy over the database identity (ie: id(), database(), modified() state, etc.).  This means that calling storeToDb() will now save to \c other's database location.  If you want to copy the values but retain your old database identity, call this function instead.
	void setValuesFrom(const AMControlInfo& other) {
		value_ = other.value_;
		minimum_ = other.minimum_;
		maximum_ = other.maximum_;
		units_ = other.units_;
		tolerance_ = other.tolerance_;
		description_ = other.description_;
		contextKnownDescription_ = other.contextKnownDescription_;
		enumString_ = other.enumString_;
		setName(other.name());
		setModified(true);
	}

public slots:
	void setValue(double value) { value_ = value; setModified(true); }
	void setMinimum(double minimum) { minimum_ = minimum; setModified(true); }
	void setMaximum(double maximum) { maximum_ = maximum; setModified(true); }
	void setUnits(const QString &units) { units_ = units; setModified(true); }
	void setTolerance(double tolerance) { tolerance_ = tolerance; setModified(true); }
	void setDescription(const QString &description) { description_ = description; setModified(true); }
	void setContextKnownDescription(const QString &contextKnownDescription) { contextKnownDescription_ = contextKnownDescription; setModified(true); }
	void setEnumString(const QString& enumString) { enumString_ = enumString; setModified(true); }

protected:
	double value_;
	double minimum_;
	double maximum_;
	QString units_;
	double tolerance_;
	QString description_;
	QString contextKnownDescription_;
	QString enumString_;
};

#endif // AMCONTROLINFO_H
