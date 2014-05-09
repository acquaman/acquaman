#ifndef AMAXISVALUEFINISHEDACTIONINFO_H
#define AMAXISVALUEFINISHEDACTIONINFO_H

#include "actions3/AMActionInfo3.h"

class AMAxisValueFinishedActionInfo : public AMActionInfo3
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMAxisValueFinishedActionInfo(QObject *parent = 0);
	/// Copy constructor.
	AMAxisValueFinishedActionInfo(const AMAxisValueFinishedActionInfo &other);
	/// Destructor.
	virtual ~AMAxisValueFinishedActionInfo(){}

	/// This function is used as a virtual copy constructor.
	virtual AMAxisValueFinishedActionInfo *createCopy() const { return new AMAxisValueFinishedActionInfo(*this); }

	/// Returns the description of the action.
	virtual QString typeDescription() const { return "Axis Value Finished"; }
};

#endif // AMAXISVALUEFINISHEDACTIONINFO_H
