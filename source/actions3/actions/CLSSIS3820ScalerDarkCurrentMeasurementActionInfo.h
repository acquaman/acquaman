#ifndef CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTIONINFO_H
#define CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"

class CLSSIS3820ScalerDarkCurrentMeasurementActionInfo : public AMListActionInfo3
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(double secondsDwell = 0, QObject *parent = 0);
	/// Copy constructor.
	CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(const CLSSIS3820ScalerDarkCurrentMeasurementActionInfo &other);
	/// Destructor.
	virtual ~CLSSIS3820ScalerDarkCurrentMeasurementActionInfo();

	/// Returns an AMActionInfo3 instance that is identical to this action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action info's description.
	virtual QString typeDescription() const { return "Make scaler channel dark current measurement"; }

	/// Returns the dwell time to use for the dark current measurement, in seconds.
	double dwellTime() const { return secondsDwell_; }

protected:
	/// The time to use for the dark current measurement, in seconds.
	double secondsDwell_;

};

#endif // CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTIONINFO_H
