#ifndef CLSSIS3820SCALERTRIGGERACTIONINFO_H
#define CLSSIS3820SCALERTRIGGERACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class CLSSIS3820ScalerTriggerActionInfo : public AMActionInfo3
{
    Q_OBJECT

	Q_PROPERTY(int readMode READ readMode WRITE setReadMode)

public:
	/// Constructor.
	explicit CLSSIS3820ScalerTriggerActionInfo(int readMode = AMDetectorDefinitions::SingleRead, QObject *parent = 0);
	/// Copy constructor.
	CLSSIS3820ScalerTriggerActionInfo(const CLSSIS3820ScalerTriggerActionInfo &original);
	/// Destructor.
	virtual ~CLSSIS3820ScalerTriggerActionInfo();

	/// Creates and returns a new copy of this action info.
	virtual AMActionInfo3* createCopy() const;

	/// Returns a description of this action info.
	virtual QString typeDescription() const { return "Trigger scaler acquisition"; }

	/// Returns the acquisition read mode.
	int readMode() const { return readMode_; }

protected:
	/// Sets the read mode.
	void setReadMode(int newMode);

protected:
	/// The acquisition read mode.
	int readMode_;
};

#endif // CLSSIS3820SCALERTRIGGERACTIONINFO_H
