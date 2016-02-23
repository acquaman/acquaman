#ifndef BIOXASSIS3820SCALERDARKCURRENTMEASUREMENTACTION_H
#define BIOXASSIS3820SCALERDARKCURRENTMEASUREMENTACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/CLSSIS3820ScalerDarkCurrentMeasurementAction.h"

class BioXASSIS3820ScalerDarkCurrentMeasurementAction : public CLSSIS3820ScalerDarkCurrentMeasurementAction
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSIS3820ScalerDarkCurrentMeasurementAction(CLSSIS3820ScalerDarkCurrentMeasurementActionInfo *info, QObject *parent = 0);
	/// Copy constructor.
	BioXASSIS3820ScalerDarkCurrentMeasurementAction(const BioXASSIS3820ScalerDarkCurrentMeasurementAction &other);
	/// Destructor.
	virtual ~BioXASSIS3820ScalerDarkCurrentMeasurementAction();

	/// Returns a new AMAction3 that's a copy of this one.
	virtual AMAction3* createCopy() const { return new BioXASSIS3820ScalerDarkCurrentMeasurementAction(*this); }

protected slots:
	/// Updates the saved pre-measurement settings.
	virtual void measurementInitialization();
	/// Applies the saved pre-measurement settings.
	virtual void measurementCleanup();

protected:
	/// The pre-measurement list of trigger source detectors.
	QList<AMDetector*> originalDetectors_;
	/// The pre-measurement list of trigger source detector managers.
	QList<QObject*> originalDetectorManagers_;
};

#endif // BIOXASSIS3820SCALERDARKCURRENTMEASUREMENTACTION_H
