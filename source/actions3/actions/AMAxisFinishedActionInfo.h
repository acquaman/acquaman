#ifndef AMAXISFINISHEDACTIONINFO_H
#define AMAXISFINISHEDACTIONINFO_H

#include "actions3/AMActionInfo3.h"

class AMAxisFinishedActionInfo : public AMActionInfo3
{
Q_OBJECT
public:
	/// Constructor
 	virtual ~AMAxisFinishedActionInfo();
	Q_INVOKABLE AMAxisFinishedActionInfo(const QString &axisName, QObject *parent = 0);

	/// Copy Constructor
	AMAxisFinishedActionInfo(const AMAxisFinishedActionInfo &other);

	/// This function is used as a virtual copy constructor
	virtual AMAxisFinishedActionInfo* createCopy() const { return new AMAxisFinishedActionInfo(*this); }

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Start Axis"; }

	QString axisName() const { return axisName_; }

protected:
	QString axisName_;
};

#endif // AMAXISFINISHEDACTIONINFO_H
