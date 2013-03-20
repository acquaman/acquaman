#ifndef AMAXISSTARTEDACTIONINFO_H
#define AMAXISSTARTEDACTIONINFO_H

#include "actions3/AMActionInfo3.h"

class AMAxisStartedActionInfo : public AMActionInfo3
{
Q_OBJECT
public:
	/// Constructor
	Q_INVOKABLE AMAxisStartedActionInfo(const QString &axisName, QObject *parent = 0);

	/// Copy Constructor
	AMAxisStartedActionInfo(const AMAxisStartedActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMAxisStartedActionInfo* createCopy() const { return new AMAxisStartedActionInfo(*this); }

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Start Axis"; }

	QString axisName() const { return axisName_; }

protected:
	QString axisName_;
};

#endif // AMAXISSTARTEDACTIONINFO_H
