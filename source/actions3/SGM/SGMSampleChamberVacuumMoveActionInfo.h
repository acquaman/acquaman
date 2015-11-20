#ifndef SGMSAMPLECHAMBERVACUUMMOVEACTIONINFO_H
#define SGMSAMPLECHAMBERVACUUMMOVEACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/AMNumber.h"

class SGMSampleChamberVacuumMoveActionInfo : public AMActionInfo3
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE SGMSampleChamberVacuumMoveActionInfo(const AMNumber &setpoint = AMNumber(AMNumber::InvalidError), QObject *parent = 0);
	/// Copy constructor.
	SGMSampleChamberVacuumMoveActionInfo(const SGMSampleChamberVacuumMoveActionInfo &original);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumMoveActionInfo();

	/// Returns an AMActionInfo3 instance that is identical to this action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action info's description.
	virtual QString typeDescription() const { return "Change sample chamber vacuum state"; }

	/// Returns the move action destination.
	AMNumber setpoint() const { return setpoint_; }

protected:
	/// The move action destination.
	AMNumber setpoint_;
};

#endif // SGMSAMPLECHAMBERVACUUMMOVEACTIONINFO_H
