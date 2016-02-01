#ifndef CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTIONINFO_H
#define CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"

class CLSSIS3820ScalerDarkCurrentMeasurementActionInfo : public AMListActionInfo3
{
    Q_OBJECT

	Q_PROPERTY(double dwellTime READ dwellTime WRITE setDwellTime)
public:
	/// Constructor.
	Q_INVOKABLE CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(double dwellTime = 0, QObject *parent = 0);
	/// Copy constructor.
	CLSSIS3820ScalerDarkCurrentMeasurementActionInfo(const CLSSIS3820ScalerDarkCurrentMeasurementActionInfo &other);
	/// Destructor.
	virtual ~CLSSIS3820ScalerDarkCurrentMeasurementActionInfo();

	/// Returns an AMActionInfo3 instance that is identical to this action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns this action info's description.
	virtual QString typeDescription() const { return "Make scaler channel dark current measurement"; }

	/// Returns the dwell time to use for the dark current measurement, in seconds.
	double dwellTime() const { return dwellTime_; }

protected:
	/// Sets the seconds dwell time. Used in loading values from the database.
	void setDwellTime(double dwellTime);

	/// The time to use for the dark current measurement, in seconds.
	double dwellTime_;

};

#endif // CLSSIS3820SCALERDARKCURRENTMEASUREMENTACTIONINFO_H
