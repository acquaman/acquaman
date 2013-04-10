#ifndef VESPERSCCDDETECTORINFO_H
#define VESPERSCCDDETECTORINFO_H

#include "dataman/info/AMOldDetectorInfo.h"

/// This class holds the basic information needed for CCD detectors on VESPERS.
class VESPERSCCDDetectorInfo : public AMOldDetectorInfo
{
	Q_OBJECT

	Q_PROPERTY(double acquireTime READ acquireTime WRITE setAcquireTime)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS CDD Detector")

public:
	/// Constructor.
	VESPERSCCDDetectorInfo(const QString &name, const QString &description, const AMnDIndex &size, QObject *parent = 0);
	/// Copy constructor.
	VESPERSCCDDetectorInfo(const VESPERSCCDDetectorInfo &original);

	/// Creates a new info pointer from this one, caller is responsible for memory.
	virtual AMOldDetectorInfo *toNewInfo() const { return new AMOldDetectorInfo(*this); }

	/// = implementation.
	VESPERSCCDDetectorInfo &operator =(const VESPERSCCDDetectorInfo &other);

	/// Returns the acquire time for the detector.
	virtual double acquireTime() const { return acquireTime_; }

	// Dimensionality and size:
	////////////////////////////////////
	/// Returns the rank of the detector.  CCD detectors are images and will always have a rank of 2.
	virtual int rank() const { return 2; }
	/// Returns the size (ie: number of elements) along each dimension of the detector.
	virtual AMnDIndex size() const { return size_; }
	/// Returns a list of AMAxisInfo describing the size and ntature of each detector axis, in order.
	virtual QList<AMAxisInfo> axes() const { return axes_; }

public slots:
	/// Sets the acquire time.
	virtual void setAcquireTime(double time) { acquireTime_ = time; setModified(true); }
	/// Set the size of the detector.
	virtual bool setSize(const AMnDIndex& size);

protected:
	/// The time the detector should acquire for.
	double acquireTime_;
	/// The axes of the detector.  They are fixed so they may as well be stored statically.
	QList<AMAxisInfo> axes_;
	/// The size of the detector.
	AMnDIndex size_;
};

#endif // VESPERSCCDDETECTORINFO_H
