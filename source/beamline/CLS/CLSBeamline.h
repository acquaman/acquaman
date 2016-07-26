#ifndef CLSBEAMLINE_H
#define CLSBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"

#include "util/AMErrorMonitor.h"

#define CLSBEAMLINE_VALVE_OPEN 1
#define CLSBEAMLINE_VALVE_CLOSED 4

class CLSBeamline : public AMBeamline
{
    Q_OBJECT

public:
	/// Singleton accessor.
	static CLSBeamline *clsBeamline()
	{
		if (!instance_) {
			AMErrorMon::alert(0, AMBEAMLINE_BEAMLINE_NOT_CREATED_YET, "No beamline created yet.  You need to call YourBeamline::bl() before calling CLSBeamline::clsBeamline().");
		}

		return static_cast<CLSBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~CLSBeamline();

	/// function to be called after beamline instantiated, which will finalize the initialization of the CLSBeamline
	virtual void initializeBeamlineSupport();

	/// Returns the beamline's scaler.
	virtual CLSSIS3820Scaler* scaler() const { return 0; }
	/// Returns a default XRF detector
	virtual AMXRFDetector *xrfDetector() const = 0;

signals:
	/// Signal to notify that the beam availability is changed
	void beamAvaliabilityChanged(bool beamOn);

protected slots:
	/// Slot to handle the signals which will affect the beam availability
	void updateBeamAvailabilityStatus();

protected:
	/// Protected constructor, for singleton pattern.
	CLSBeamline(const QString &controlName);

	/// helper function to check whether the beam of a beamline is available or not --- this usually is combined with beamline status PV and/or beamline shutters/valves stauts
	virtual bool isBeamlineBeamAvailable() = 0;

protected:
	/// the flag of whether beam is available
	bool wasBeamAvailable_;

};

#endif // CLSBEAMLINE_H
