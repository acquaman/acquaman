#ifndef AMSCANAXISREGION_H
#define AMSCANAXISREGION_H

#include "dataman/AMNumber.h"
#include "dataman/database/AMDbObject.h"

class AMScanAxisRegion : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(double regionStart READ regionStart WRITE setRegionStart)
	Q_PROPERTY(double regionStep READ regionStep WRITE setRegionStep)
	Q_PROPERTY(double regionEnd READ regionEnd WRITE setRegionEnd)
	Q_PROPERTY(double regionTime READ regionTime WRITE setRegionTime)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Defines a single region that can make up an axis.")

public:
	/// Constructor. Takes values for start, step, end, and time. Defaults are AMNumbers in the AMNumber::Null state
	AMScanAxisRegion(const AMNumber &start = AMNumber(AMNumber::Null), const AMNumber &step = AMNumber(AMNumber::Null), const AMNumber &end = AMNumber(AMNumber::Null), const AMNumber &time = AMNumber(AMNumber::Null), QObject *parent = 0);
	/// Copy constructor.
	AMScanAxisRegion(const AMScanAxisRegion &original);
	/// Destructor.
	virtual ~AMScanAxisRegion();

	/// Returns the start of the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionStart() const;
	/// Returns the step size for the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionStep() const;
	/// Returns the end of the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionEnd() const;
	/// Returns the time for the region as an AMNumber (which may be in the state AMNumber::Null)
	AMNumber regionTime() const;

signals:
	/// Notifier that the start value has changed.
	void regionStartChanged(const AMNumber &);
	/// Notifier that the step value value has changed.
	void regionStepChanged(const AMNumber &);
	/// Notifier that the end value has changed.
	void regionEndChanged(const AMNumber &);
	/// Notifier that the time per step has changed.
	void regionTimeChanged(const AMNumber &);

public slots:
	/// Sets the start of the region from the AMNumber
	void setRegionStart(const AMNumber &regionStart);
	/// Sets the step size for the region from the AMNumber
	void setRegionStep(const AMNumber &regionStep);
	/// Sets the end of the region from the AMNumber
	void setRegionEnd(const AMNumber &regionEnd);
	/// Sets the time for the region from the AMNumber
	void setRegionTime(const AMNumber &regionTime);

protected:
	/// Holds the start of the region
	AMNumber regionStart_;
	/// Holds the step size for the region
	AMNumber regionStep_;
	/// Holds the end of the region
	AMNumber regionEnd_;
	/// Holds the time for the region
	AMNumber regionTime_;
};

#endif // AMSCANAXISREGION_H
