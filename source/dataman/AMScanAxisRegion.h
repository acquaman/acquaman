#ifndef AMSCANAXISREGION_H
#define AMSCANAXISREGION_H

#include "dataman/AMNumber.h"
#include "dataman/database/AMDbObject.h"

class AMScanAxisRegion : public AMDbObject
{
	Q_OBJECT

public:
	/// Constructor. Takes values for start, step, end, and time. Defaults are AMNumbers in the AMNumber::Null state
	AMScanAxisRegion(AMNumber start = AMNumber(AMNumber::Null), AMNumber step = AMNumber(AMNumber::Null), AMNumber end = AMNumber(AMNumber::Null), AMNumber time = AMNumber(AMNumber::Null), QObject *parent = 0);
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

public slots:
	/// Sets the start of the region from the AMNumber
	void setRegionStart(AMNumber regionStart);
	/// Sets the step size for the region from the AMNumber
	void setRegionStep(AMNumber regionStep);
	/// Sets the end of the region from the AMNumber
	void setRegionEnd(AMNumber regionEnd);
	/// Sets the time for the region from the AMNumber
	void setRegionTime(AMNumber regionTime);

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
