#ifndef AMAXISSTARTEDACTIONINFO_H
#define AMAXISSTARTEDACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/AMScanAxis.h"

class AMAxisStartedActionInfo : public AMActionInfo3
{
	Q_OBJECT

	Q_PROPERTY(QString axisName READ axisName WRITE setAxisName)
	Q_PROPERTY(int axisType READ dbAxisType WRITE setDbAxisType)

public:
	/// Constructor
	Q_INVOKABLE AMAxisStartedActionInfo(const QString &axisName, AMScanAxis::AxisType axisType, QObject *parent = 0);
	/// Destructor.
	virtual ~AMAxisStartedActionInfo();

	/// Copy Constructor
	AMAxisStartedActionInfo(const AMAxisStartedActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMAxisStartedActionInfo* createCopy() const { return new AMAxisStartedActionInfo(*this); }

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Start Axis"; }
	/// Returns the name of the axis as a string.
	QString axisName() const { return axisName_; }
	/// Returns the type of axis.
	AMScanAxis::AxisType axisType() const { return axisType_; }

protected:
	/// Getter for database loading.
	int dbAxisType() const { return (int)axisType_; }
	/// Sets the axis name.
	void setAxisName(const QString &axisName) { axisName_ = axisName; }
	/// Sets the axis type.
	void setAxisType(AMScanAxis::AxisType axisType) { axisType_ = axisType; }
	/// Setter for database saving.
	void setDbAxisType(int axisType) { axisType_ = (AMScanAxis::AxisType)axisType; }

protected:
	/// String holding the name of the axis.
	QString axisName_;
	/// The flag holder for what kind of axis this is.
	AMScanAxis::AxisType axisType_;
};

#endif // AMAXISSTARTEDACTIONINFO_H
