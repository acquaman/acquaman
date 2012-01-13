#ifndef VESPERSROPERCCDDETECTORINFO_H
#define VESPERSROPERCCDDETECTORINFO_H

#include "dataman/info/AMDetectorInfo.h"

class VESPERSRoperCCDDetectorInfo : public AMDetectorInfo
{
	Q_OBJECT

	Q_PROPERTY(double acquireTime READ acquireTime WRITE setAcquireTime)
	Q_PROPERTY(double temperature READ temperature WRITE setTemperature)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Roper CCD Detector")

public:
	/// Constructor.
	VESPERSRoperCCDDetectorInfo(const QString &name, const QString &description, QObject *parent = 0);
	/// Copy constructor.
	VESPERSRoperCCDDetectorInfo(const VESPERSRoperCCDDetectorInfo &original);

	/// Creates a new info pointer from this one, caller is reponsible for memory.
	virtual AMDetectorInfo *toNewInfo() const { return new AMDetectorInfo(*this); }

	VESPERSRoperCCDDetectorInfo &operator =(const VESPERSRoperCCDDetectorInfo &other);

	/// Returns the acquire time for the detector.
	virtual double acquireTime() const { return acquireTime_; }
	/// Returns the temperature setpoint of the detector.
	virtual double temperature() const { return temperature_; }

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the number of dimensions in the output of this detector. A single point has rank 0. A spectrum output would have rank 1. An image output would have rank 2.
	virtual int rank() const { return 2;}
	/// Returns the size (ie: number of elements) along each dimension of the detector.  For a single-point detector, returns an empty AMnDIndex(). For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual AMnDIndex size() const { return AMnDIndex(2084, 2084); }
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo>  axes() const { return axes_; }

public slots:
	/// Sets the acquire time.
	virtual void setAcquireTime(double time) { acquireTime_ = time; setModified(true); }
	/// Sets the temperature setpoint.
	virtual void setTemperature(double temperature) { temperature_ = temperature; setModified(true); }

protected:
	/// The time the detector should acquire for.
	double acquireTime_;
	/// The temperature that the detector should be set to.
	double temperature_;
	/// The axes of the detector.  They are fixed so they may as well be stored statically.
	QList<AMAxisInfo> axes_;
};

#endif // VESPERSROPERCCDDETECTORINFO_H
