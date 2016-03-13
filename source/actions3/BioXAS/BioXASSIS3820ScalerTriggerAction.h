#ifndef BIOXASSIS3820SCALERTRIGGERACTION_H
#define BIOXASSIS3820SCALERTRIGGERACTION_H

#include "actions3/CLS/CLSSIS3820ScalerTriggerAction.h"

class AMDetector;

class BioXASSIS3820ScalerTriggerAction : public CLSSIS3820ScalerTriggerAction
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE BioXASSIS3820ScalerTriggerAction(CLSSIS3820ScalerTriggerActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	BioXASSIS3820ScalerTriggerAction(const BioXASSIS3820ScalerTriggerAction &original);
	/// Destructor.
	virtual ~BioXASSIS3820ScalerTriggerAction();

	/// Returns a new AMAction3 that's a copy of this one.
	virtual AMAction3* createCopy() const { return new BioXASSIS3820ScalerTriggerAction(*this); }

protected:
	/// Updates the saved pre-measurement settings.
	virtual void measurementInitialization();
	/// Applies the saved pre-measurement settings.
	virtual void measurementCleanup();

protected:
	/// The pre-trigger list of Zebra detectors.
	QList<AMDetector*> originalDetectors_;
};

#endif // BIOXASSIS3820SCALERTRIGGERACTION_H
