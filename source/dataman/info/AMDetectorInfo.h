#ifndef AMDETECTORINFO_H
#define AMDETECTORINFO_H

#include "dataman/database/AMDbObject.h"

namespace AMDetectorDefinitions{
	/// This enum lists the different types of read methods available
	enum ReadMethod {
		ImmediateRead = 0, ///< Read whatever value is currently available
		RequestRead = 1, ///< Trigger the detector and wait for the next value
		WaitRead = 2,  ///< Wait for the next value update
		InvalidReadMethod = 3 ///< Enum for invalid state
	};

	/// This enum lists the different types of read modes available
	enum ReadMode {
		SingleRead = 0, ///< Trigger to read one value at a time
		ContinuousRead = 1, ///< Trigger to continuously read over a period of time
		InvalidReadMode = 2 ///< Enum for invalid state
	};
}

class AMDetectorInfo : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(QString description READ description WRITE setDescription)
Q_PROPERTY(QString units READ units WRITE setUnits)
Q_PROPERTY(double acquisitionTime READ acquisitionTime WRITE setAcquisitionTime)
Q_PROPERTY(int readMode READ dbReadMode WRITE dbSetReadMode)

public:
	/// Default constructor, places invalid or empty parameters
	Q_INVOKABLE AMDetectorInfo(const QString &name = "Invalid Detector", const QString &description = "", const QString &units = "N/A", double acquisitionTime = -1, AMDetectorDefinitions::ReadMode readMode = AMDetectorDefinitions::InvalidReadMode, QObject *parent = 0);

	/// Returns the user readable name
	QString description() const { return description_; }
	/// Returns the units
	QString units() const { return units_; }
	/// Returns the acquisition time
	double acquisitionTime() const { return acquisitionTime_; }
	/// Returns the read mode
	AMDetectorDefinitions::ReadMode readMode() const { return readMode_; }

	/// The default copy constructor and assignment operator will copy the values from \c other, but they will also copy over the database identity (ie: id(), database(), modified() state, etc.).  This means that calling storeToDb() will now save to \c other's database location.  If you want to copy the values but retain your old database identity, call this function instead.
	void setValuesFrom(const AMDetectorInfo& other);

public slots:
	/// Sets the user readable description
	void setDescription(const QString &description);
	/// Sets the units
	void setUnits(const QString &units);
	/// Sets the acquisition time
	void setAcquisitionTime(double acquisitionTime);
	/// Sets the read mode
	void setReadMode(AMDetectorDefinitions::ReadMode readMode);

protected:
	int dbReadMode() const { return (int)readMode_; }
	void dbSetReadMode(int readMode) { readMode_ = (AMDetectorDefinitions::ReadMode)readMode; }

protected:
	/// User readable description
	QString description_;
	/// Units of this detector
	QString units_;
	/// The acquisition time in effect
	double acquisitionTime_;
	/// The read mode that the detector was using
	AMDetectorDefinitions::ReadMode readMode_;
};

#endif // AMDETECTORINFO_H
