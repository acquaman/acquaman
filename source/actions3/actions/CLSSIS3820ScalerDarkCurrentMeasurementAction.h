#ifndef CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_H
#define CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementActionInfo.h"

class CLSSIS3820ScalerDarkCurrentMeasurementAction : public AMListAction3
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSSIS3820ScalerDarkCurrentMeasurementAction(CLSSIS3820ScalerDarkCurrentMeasurementActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	CLSSIS3820ScalerDarkCurrentMeasurementAction(const CLSSIS3820ScalerDarkCurrentMeasurementAction &other);
	/// Destructor.
	virtual ~CLSSIS3820ScalerDarkCurrentMeasurementAction();
	/// Returns a new AMAction3 that's a copy of this one.
	virtual AMAction3* createCopy() const { return new CLSSIS3820ScalerDarkCurrentMeasurementAction(*this); }

	/// Returns whether this action can be paused, false for this action.
	virtual bool canPause() const { return false; }
	/// Returns whether this action can be skipped, false for this action.
	virtual bool canSkip() const { return false; }

protected:
	/// Populates this action with its child actions.
	void initialize();
	/// Reimplementation of AMListAction3::startImplementation(). Used for initial testing.
	virtual void startImplementation();

	/// Returns the specific action info for this class.
	const CLSSIS3820ScalerDarkCurrentMeasurementActionInfo* scalerDarkCurrentMeasurementActionInfo() const { return qobject_cast<const CLSSIS3820ScalerDarkCurrentMeasurementActionInfo*>(info()); }
	/// Returns the specific action info for this class.
	const CLSSIS3820ScalerDarkCurrentMeasurementActionInfo* scalerDarkCurrentMeasurementActionInfo() { return qobject_cast<CLSSIS3820ScalerDarkCurrentMeasurementActionInfo*>(info()); }
};

#endif // CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTION_H
