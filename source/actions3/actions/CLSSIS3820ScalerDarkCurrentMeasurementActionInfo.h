#ifndef CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTIONINFO_H
#define CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"

class CLSSIS3820ScalerDarkCurrentMeasurementActionInfo : public AMListActionInfo3
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(CLSSIS3820Scaler *scaler, double secondsDwell, QObject *parent = 0);
	/// Copy constructor.
	CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(const CLSSIS3820ScalerDarkCurrentMeasurementActionInfo &other);
	/// Destructor.
	virtual ~CLSSIS3820ScalerDarkCurrentMeasurementActionInfo();
	/// Returns an AMActionInfo3 instance that is identical to this action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action info's description.
	virtual QString typeDescription() const { return "Make dark current measurement"; }
	/// Returns the scaler to execute the dark current measurement.
	CLSSIS3820Scaler* scaler() const { return scaler_; }
	/// Returns the dwell time to use for the dark current measurement, in seconds.
	double dwellTime() const { return secondsDwell_; }

signals:

public slots:

protected:
	/// The scaler to execute the dark current measurement.
	CLSSIS3820Scaler *scaler_;
	/// The time to use for the dark current measurement, in seconds.
	double secondsDwell_;

};

#endif // CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTIONINFO_H
