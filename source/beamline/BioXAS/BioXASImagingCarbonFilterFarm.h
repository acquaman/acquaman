#ifndef BIOXASIMAGINGCARBONFILTERFARM_H
#define BIOXASIMAGINGCARBONFILTERFARM_H

#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"

// Valid actuator position setpoints and ranges.

#define BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_OUT 19.97
#define BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_OUT_MIN 19.92
#define BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_OUT_MAX 20.02

#define BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_BOTTOM -2.6
#define BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_BOTTOM_MIN -2.65
#define BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_BOTTOM_MAX -2.55

#define BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_TOP -14.65
#define BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_TOP_MIN -14.70
#define BIOXASIMAGINGCARBONFILTERFARM_UPSTREAM_TOP_MAX -14.60

#define BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_OUT 21.0
#define BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_OUT_MIN 20.95
#define BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_OUT_MAX 21.05

#define BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_BOTTOM -2.5
#define BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_BOTTOM_MIN -2.55
#define BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_BOTTOM_MAX -2.45

#define BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_TOP -14.5
#define BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_TOP_MIN -14.55
#define BIOXASIMAGINGCARBONFILTERFARM_DOWNSTREAM_TOP_MAX -14.45

class CLSMAXvMotor;

class BioXASImagingCarbonFilterFarm : public BioXASCarbonFilterFarm
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASImagingCarbonFilterFarm(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASImagingCarbonFilterFarm();

	/// Returns the upstream actuator motor control.
	CLSMAXvMotor* upstreamMotor() const { return upstreamMotor_; }
	/// Return the downstream actuator motor control.
	CLSMAXvMotor* downstreamMotor() const { return downstreamMotor_; }

protected:
	/// The upstream actuator motor control.
	CLSMAXvMotor *upstreamMotor_;
	/// The downstream actuator motor control.
	CLSMAXvMotor *downstreamMotor_;
};

#endif // BIOXASIMAGINGCARBONFILTERFARM_H