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


#ifndef AMOldDetectorInfo_H
#define AMOldDetectorInfo_H

#include <QMetaProperty>
#include <QDebug>

#include "acquaman.h"
#include "dataman/database/AMDbObject.h"
#include "dataman/AMMeasurementInfo.h"

class QStringList;


/// This is the base class of all customizeable detectors and detector information objects. It gets associated with AMScans to indicate how their data was collected.  Subclasses should eventually contain the unique configuration and state of a particular physical detector (ex: the integration time and gain of a pico-ammeter, the HV setting for a CMA/channeltron, the bin configuration of a spectral detector like the Silicon Drift Detector.)  This base class is suitable for an un-configurable, single-point-recording generic detector.
class AMOldDetectorInfo : public AMDbObject
{
Q_OBJECT

	/// A description (human-readable string) for this type of detector
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(QString units READ units WRITE setUnits)
	Q_PROPERTY(int rank READ rank)
	Q_PROPERTY(AMnDIndex size READ size WRITE setSize)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Detector")

public:
	// Constructors and destructors
	/////////////////////////////////////
	/// Default constructor. \c name is a unique programmer's name to access this detector with. \c description is a human-readable version.
	Q_INVOKABLE AMOldDetectorInfo(const QString& name = "unknownDetector", const QString& description = "Generic Detector", QObject *parent = 0);

	/// Copy constructor.  QObject parent/child relationships are NOT copied, but the essential characteristics (id, database, and modified state and Q_PROPERTIES) of the AMDetectorInfo are.  Making a copy will create an independent instance in memory. However, if the original has been previously saved to or loaded from the database, both the original and the copy will store/restore to the same location in the database (ie: they refer to the same persistent object).
	/*! If the original has never been successfully saved or loaded (ie: id() and database() return 0) then the two instances remain fully independent objects (both in memory, and in the database after calling storeToDb() for the first time.)

	  The parent QObject is not set when using this copy constructor; the copy's parent() will be 0.  If you want the copy to share the same parent(), you must call QObject::setParent() afterward.
	  */
	AMOldDetectorInfo(const AMOldDetectorInfo& original);

	AMOldDetectorInfo& operator=(const AMOldDetectorInfo &other);

	virtual ~AMOldDetectorInfo() {}

	/// Creates a new info pointer from this one, caller is responsible for memory
	virtual AMOldDetectorInfo* toNewInfo() const;

	// Basic meta-data:
	////////////////////////////////////
	/// Access a human-readable description (ex: "Sample Current" or "Total Fluorescence Yield")
	QString description() const { return description_; }
	/// The units describing this detector's readings. (ex: "counts", "milliAmps", etc.)
	QString units() const { return units_; }

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has rank 0. A spectrum output would have rank 1. An image output would have rank 2.
	virtual int rank() const { return 0;}
	/// Returns the size (ie: number of elements) along each dimension of the detector.  For a single-point detector, returns an empty AMnDIndex(). For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual AMnDIndex size() const { return AMnDIndex(); }
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo>  axes() const { return QList<AMAxisInfo>(); }

	/// Returns (or casts) this AMDetectorInfo as an AMMeasurementInfo, which contains the bare-bones dimensional information.
	operator AMMeasurementInfo() {
		if(!description().isEmpty())
			return AMMeasurementInfo(description().remove(" "), description(), units(), axes());
		else
			return AMMeasurementInfo(name(), name(), units(), axes());
	}

	friend QDebug operator<<(QDebug d, const AMOldDetectorInfo& di);
	virtual QDebug qDebugPrint(QDebug &d) const;

	/// I don't know what this is for.
	virtual bool hasDetails() const { return false; }



public slots:
	/// Set the human-readable description
	void setDescription(const QString& description) {
		description_ =  description;
		setModified(true);
	}
	/// Set the units describing the detector's readings
	void setUnits(const QString& units) {
		units_ = units;
		setModified(true);
	}

	/// Set the size of the detector. This may not be applicable or valid for some detectors (for example, a single-point detector, or one with a fixed dimensionality.  Returns false if it's invalid to set the size.
	virtual bool setSize(const AMnDIndex& size) { Q_UNUSED(size); return false; }

protected:
	QString description_;
	QString units_;
};

#endif // AMDetectorInfo_H
